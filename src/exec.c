/* exec.c - implementation of the internal bytecode execution engine for kscript
 *
 *
 * Essentially, bytecode is just a sequence of bytes (shocking, I know...) that have operands,
 * The first operand is the first byte, i.e. `bc[0]` tells what the first instruction does. From there,
 *   each instruction has a given length (all of kscript's operands are either 1 or 5 bytes, check `ks.h`
 *   by the `KSB_*` enumeration values to see specific ones).
 * Then, the next operand is located at `bc[0 + 1]` or `bc[0 + 5]`. This process is iterated until some control
 *   flow operand is encountered; for example KSB_RET or KSB_THROW, or an exception is generated.
 * 
 * In that case, we have an exception stack that we back up; we find the first valid exception handler,
 *   and start executing at that point. We can just assume that the code generator has set up that address
 *   (given to us with the opcode `KS_TRY_START`) to handle such a case.
 * 
 * If there are none available, we set `this->exc` and `this->exc_info` (this being the current thread),
 *   and then return NULL, which signals that an exception was thrown. From there, however called the piece 
 *   of code we are executing can make a few choices:
 *   * If they are in a C-function, they will get a return value of 'NULL'; they can return 'NULL' to propogate
 *       that error upwards even higher (making sure, of course, that they delete any dangling reference or
 *       allocated memory)
 *     Or, they can choose to catch that using `ks_catch()` or `ks_catch_ignore()`
 *   * If they are in a kscript function, they have no choice, since they should have set up a 'try'/'catch'
 *       block. The kscript interpreter will exit and print a stack trace leading up to the exception,
 *       and exit with a non-zero exit code
 *
 * 
 * The actual interpreting of the bytecode is abstracted; by default it uses a `switch; case` construct to
 *   check the opcode, then perform the operation, and then continue on. This works fairly well for most things,
 *   and has the opportunity (although not guaranteed) to catch mal-formed bytecode. Again, it could be malformed
 *   and still execute (as it has A valid opcode), but at least you will get an abort message instead of a seg-fault
 * There is another method, which is faster, but carries more risks, called 'computed GOTO'. This uses jump tables
 *   instead of switch-cases, and thus reduces itself to a simple index instead of a bounds check + index
 * Obviously, the risk of this is that if a malformed byte-code is given, it may jump to an undefined location,
 *   causing a crash, or worse, continued execution with compromised memory, stack, etc.
 * 
 * For that reason, as of now, I am using the switch case, but I intend to continue improving the GOTO and doing more
 *   tests until it is stable enough to be the default. However, I will continue to support the switch case due
 *   to compatibility, error checking, error protection, and other reasons. However, the computed goto is quite
 *   possibly faster (as CPython has shown)
 * 
 * 
 * There is still progress to be made here (and in the internals around execution in general), for example, it would
 *   be nice to allow promotion to a higher scope via the `global` keyword, similar to Python. It would be good to announce:
 * ```
 * global x, y, z
 * x = y + z
 * ```
 * , for example
 * 
 * 
 *  -- REFERENCES --
 * 
 * More on computed goto: https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


/* utilities */


// Yield the GIL (i.e. and allow other threads to unlock it), and then lock it back immediately,
//   then continue executing
// This is used to give other threads a chance to execute
#define YIELDGIL() { ks_GIL_unlock(); ks_GIL_lock(); }

// Check that a given assertion is valid.
// NOTE: This may be removed (i.e. defined to nothing) to speed up execution speed;
//   anything that uses VME_CHECK() should be a sanity check only; only in seriously
//   broken builds should it ever actually abort. So, in release builds, it should probably
//   be removed for speed
#define VME_CHECK(...) assert(__VA_ARGS__)

// maximum size of the exception handler stack
#define MAX_EXC_STACK 256

/* Virtual Machine Execution Dispatch (VMED)
 *
 * This code is used to control which method (SWITCHCASE or GOTO) is used for dispatching commands
 * 
 * See the header for comments regarding the generalities of both
 * 
 */

// Uncomment the one that should be used
// SWITCHCASE: Safe, pretty fast, and well documented. Standard C code
#define VME__SWITCHCASE
// GOTO: Using a computed GOTO table (see top of file comment block), which is less safe, but may be faster
//#define VME__GOTO


// decide which method to use
#if defined(VME__SWITCHCASE)

// Start & end of the dispatcher
#define VMED_START while (true) switch (*c_pc) {
#define VMED_END default: fprintf(stderr, "ERROR: in kscript VM exec (%p), unknown instruction code '%i' encountered\n", code, (int)*c_pc); assert(false && "Internal Error"); break;}

// Used to go to the next instruction
#define VMED_NEXT() { continue; }

// Start & end of a specific byte-code handler
#define VMED_CASE_START(_bc) case _bc: {
#define VMED_CASE_END VMED_NEXT() }

#elif defined(VME__GOTO)

#define VMED_START { VMED_NEXT();
#define VMED_END }

#define VMED_NEXT() { goto *goto_targets[*c_pc]; }

#define VMED_CASE_START(_bc) lbl_##_bc: {
#define VMED_CASE_END VMED_NEXT() }

#else
#error No VME__ used for execution (either 'VME__SWITCHCASE' or 'VME__GOTO' should be defined)
#endif


// a structure describing an item in the exception handler call stack
typedef struct {

    // where to go if an exception is raised
    ksb* to_c_pc;

} exc_handler;


/* ks__exec -> perform execution on a thread
 *
 * This function makes a lot of assumptions, such as:
 *   * You are on a thread currently
 *   * The current thread has already had teh stack frame loaded (i.e.
 *       this method does not create a stack frame)
 *
 * If any of these are not met, it will just abort (no exceptions generated),
 *   because this IS this code that generates exceptions, it's okay to safeguard it like this
 * 
 */
ks_obj ks__exec(ks_thread self, ks_code code) {
    // thread to execute on
    assert(self != NULL && "'ks__exec()' called without a thread");
    assert(self->frames->len > 0 && "No stack frames available!");

    uint32_t gilct = 0;


    // Current stack frame
    ks_stack_frame c_frame = (ks_stack_frame)self->frames->elems[self->frames->len - 1];

    // current kfunc (or NULL if it wasn't one)
    ks_kfunc c_kfunc = (ks_kfunc)(c_frame->func->type == ks_T_kfunc ? (ks_kfunc)c_frame->func : NULL);
    

    // start program counter at the beginning
    #define c_pc (c_frame->pc)

    // set program counter to the start of the bytecode
    c_pc = code->bc;

    // current index into the exception handler stack
    int exc_i = -1;

    // exception handler stack
    exc_handler exchs[MAX_EXC_STACK];


    // number of args
    int args_n = 0;

    // temporary array of arguments
    ks_obj* args = NULL;
    
    // ensure 'args' holds enough
    #define ENSURE_ARGS(_n) { if ((_n) >= args_n) { args_n = (_n); args = ks_realloc(args, sizeof(*args) * args_n); } }


    // temporary variables
    int i, j;

    // temporary variable for single op
    ksb op;

    // temporary variable for a bytecode with a 32 bit integer argument
    ksb_i32 op_i32;

    // consume a structure from the program counter
    #define VMED_CONSUME(_type, _var) { \
        _var = *(_type*)(c_pc);           \
        c_pc += sizeof(_type);            \
    }

    // if we are using computed goto, we need to innitialize the labels
    #ifdef VME__GOTO

    // initialize the goto target
    #define GOTO_TARGET(_bc) [_bc] = &&lbl_##_bc,

    static void* goto_targets[] = {

        // TODO: fill these up to use computed goto
    
    
    };

    #endif


    // the value that will be returned
    ks_obj ret_val = NULL;

    // where did we start on the exception call stack?
    int start_ecs = exc_i;

    // starting length
    int start_stk_len = self->stk->len;


    // label to dispatch from
    dispatch: ;

    // inner dispatch loop for consuming bytecode
    VMED_START

        VMED_CASE_START(KSB_NOOP)
            VMED_CONSUME(ksb, op);

        VMED_CASE_END

        /* -- Basic Stack Manipulation -- */

        VMED_CASE_START(KSB_PUSH)
            VMED_CONSUME(ksb_i32, op_i32);


            // push on a constant value indicated by the argument
            ks_list_push(self->stk, code->v_const->elems[op_i32.arg]);

        VMED_CASE_END

        VMED_CASE_START(KSB_DUP)
            VMED_CONSUME(ksb, op);

            // duplicate the top of stack
            ks_list_push(self->stk, self->stk->elems[self->stk->len - 1]);

        VMED_CASE_END

        VMED_CASE_START(KSB_POPU)
            VMED_CONSUME(ksb, op);

            // pop (unused) off the top of the stack
            ks_list_popu(self->stk);

        VMED_CASE_END

        VMED_CASE_START(KSB_TRUTHY)
            VMED_CONSUME(ksb, op);

            // convert TOS to its boolean value
            ks_obj TOS = ks_list_pop(self->stk);
            int truthy = ks_obj_truthy(TOS);
            KS_DECREF(TOS);

            // negative value indicates an exception/error was thrown
            if (truthy < 0) goto EXC;

            // otherwise, convert to a bool object
            ks_list_push(self->stk, KSO_BOOL(truthy));

        VMED_CASE_END



        /* -- Generating Primitives/Iterables -- */

        VMED_CASE_START(KSB_SLICE)
            VMED_CONSUME(ksb, op);

            // double check we have enough
            VME_CHECK(self->stk->len >= 3 && "'slice' instruction requires 3 items on the stack!");

            // last 3 objects
            ks_obj* slice_objs = &self->stk->elems[self->stk->len - 3];

            // construct a slice
            ks_slice new_slice = ks_slice_new(slice_objs[0], slice_objs[1], slice_objs[2]);

            // remove args from the stack
            for (i = 0; i < 3; ++i) {
                ks_list_popu(self->stk);
            }

            // push on the created slice
            ks_list_push(self->stk, (ks_obj)new_slice);
            KS_DECREF(new_slice);

        VMED_CASE_END


        VMED_CASE_START(KSB_TUPLE)
            VMED_CONSUME(ksb_i32, op_i32);

            // double check we have enough
            VME_CHECK(self->stk->len >= op_i32.arg && "'tuple' instruction required more arguments than existed!");

            // construct the tuple
            ks_tuple new_tuple = ks_tuple_new(op_i32.arg, &self->stk->elems[self->stk->len - op_i32.arg]);

            // remove args from the stack
            for (i = 0; i < op_i32.arg; ++i) ks_list_popu(self->stk);

            // push on the created tuple
            ks_list_push(self->stk, (ks_obj)new_tuple);
            KS_DECREF(new_tuple);

        VMED_CASE_END

        VMED_CASE_START(KSB_LIST)
            VMED_CONSUME(ksb_i32, op_i32);

            // double check we have enough
            VME_CHECK(self->stk->len >= op_i32.arg && "'list' instruction required more arguments than existed!");

            // construct the list
            ks_list new_list = ks_list_new(op_i32.arg, &self->stk->elems[self->stk->len - op_i32.arg]);

            // remove args from the stack
            for (i = 0; i < op_i32.arg; ++i) ks_list_popu(self->stk);

            // push on the created list
            ks_list_push(self->stk, (ks_obj)new_list);
            KS_DECREF(new_list);

        VMED_CASE_END

        VMED_CASE_START(KSB_DICT)
            VMED_CONSUME(ksb_i32, op_i32);
            
            // double check there are enough & there are an even number (should be (key, vals), so they better match)
            VME_CHECK(self->stk->len >= op_i32.arg && "'dict' instruction required more arguments than existed!");
            VME_CHECK(op_i32.arg % 2 == 0 && "'dict' instruction requires an even number of arguments!");

            // construct the dictionary
            ks_dict new_dict = ks_dict_new(op_i32.arg, &self->stk->elems[self->stk->len - op_i32.arg]);

            // remove args from the stack
            for (i = 0; i < op_i32.arg; ++i) ks_list_popu(self->stk);

            // push on the created dictionary
            ks_list_push(self->stk, (ks_obj)new_dict);
            KS_DECREF(new_dict);

        VMED_CASE_END


        /* CONTROL FLOW */


        VMED_CASE_START(KSB_JMP)
            VMED_CONSUME(ksb_i32, op_i32);

            // unconditionally advance the program counter
            c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_JMPT)
            VMED_CONSUME(ksb_i32, op_i32);

            // take the top item off, see if truthy
            ks_obj cond = ks_list_pop(self->stk);
            int truthy = ks_obj_truthy(cond);
            KS_DECREF(cond);
            if (truthy < 0) goto EXC;

            // conditionally 'jump' in the code
            if (truthy) c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_JMPF)
            VMED_CONSUME(ksb_i32, op_i32);


            // take the top item off, see if truthy
            ks_obj cond = ks_list_pop(self->stk);
            int truthy = ks_obj_truthy(cond);
            
            if (truthy < 0) goto EXC;
            KS_DECREF(cond);

            // conditionally do jump
            if (!truthy) c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_CALL)
            VMED_CONSUME(ksb_i32, op_i32);

            // copy into local arguments array
            // NOTE: This is neccessary, because the thread might have to have the stack resized
            //   during the subsequent function call. If that happens, all of the sudden a pointer we
            //   gave to `ks_call` is no longer valid (due to reallocation), so we need to preserve
            //   them here locally
            ENSURE_ARGS(op_i32.arg);
            ks_list_popn(self->stk, op_i32.arg, args);


            // ask kscript to call it
            // TODO: Perhaps add short-circuit logic here?
            if (args[0]->type == ks_T_kfunc) {

            }
            
            ks_obj ret = ks_obj_call(args[0], op_i32.arg - 1, &args[1]);

            if (!ret) goto EXC;

            
            // push the result on the stack where the arguments started
            ks_list_push(self->stk, ret);

            // we are finished with the arguments
            for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);

        VMED_CASE_END

        VMED_CASE_START(KSB_RET)
            VMED_CONSUME(ksb, op);

            // we need to return the top-of-stack
            ret_val = ks_list_pop(self->stk);
            goto RET;

        VMED_CASE_END


        /* -- Exceptions/Errors/Handlers -- */


        VMED_CASE_START(KSB_TRY_START)
            VMED_CONSUME(ksb_i32, op_i32);

            // ensure we haven't had too many
            VME_CHECK(exc_i < MAX_EXC_STACK && "EXC Call Stack Exceeded!");

            // add a new item to the exception stack, where the address is the argument + current position
            // (i.e. the code generator gives us the relative address to the handler)
            exchs[++exc_i] = (exc_handler){ .to_c_pc = c_pc + op_i32.arg };

        VMED_CASE_END

        VMED_CASE_START(KSB_TRY_END)
            VMED_CONSUME(ksb_i32, op_i32);

            // ensure there was an exception handler present
            VME_CHECK(exc_i >= 0 && "Input Bytecode Malformed; deleting exc call stack item where none exists!");

            // remove the top one
            exc_i--;

            // now, perform an unconditional jump (this byte tells us where the non-handler code begins, i.e. after the catch{} block ends)
            c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_THROW)
            VMED_CONSUME(ksb, op);

            // throw the object on the top of the stack
            ks_obj exc_obj = ks_list_pop(self->stk);
            ks_obj_throw(exc_obj);
            KS_DECREF(exc_obj);

            // treat it like an error/exception and go there
            goto EXC;

        VMED_CASE_END

        VMED_CASE_START(KSB_ASSERT)
            VMED_CONSUME(ksb, op);

            // we want to assert the object is truthy
            ks_obj ass_obj = ks_list_pop(self->stk);
            int truthy = ks_obj_truthy(ass_obj);
            KS_DECREF(ass_obj);
            if (truthy < 0) goto EXC;

            if (!truthy) {
                ks_throw(ks_T_AssertError, "'assert' statement failed!");
                goto EXC;
            }

        VMED_CASE_END


        /* -- Item Getting -- */

        VMED_CASE_START(KSB_GETITEM)
            VMED_CONSUME(ksb_i32, op_i32);


            // load args, ensuring we can store them
            ENSURE_ARGS(op_i32.arg);
            ks_list_popn(self->stk, op_i32.arg, args);

            // now call
            ks_obj ret = ks_F_getitem->func(op_i32.arg, args);
            if (!ret) {
                for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);
                goto EXC;
            }



            ks_list_push(self->stk, ret);
            
            for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);
            KS_DECREF(ret);

        VMED_CASE_END


        VMED_CASE_START(KSB_SETITEM)
            VMED_CONSUME(ksb_i32, op_i32);

            // first, consume an array to call

            // load args
            ENSURE_ARGS(op_i32.arg);
            ks_list_popn(self->stk, op_i32.arg, args);

            // now call
            ks_obj ret = ks_F_setitem->func(op_i32.arg, args);
            if (!ret) {
                for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);
                goto EXC;
            }
    

            ks_list_push(self->stk, ret);
            
            for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);
            KS_DECREF(ret);

        VMED_CASE_END



        /* VALUE LOOKUP */

        VMED_CASE_START(KSB_LOAD)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str name = (ks_str)code->v_const->elems[op_i32.arg];
            VME_CHECK(name->type == ks_T_str && "load [name] : 'name' must be a string");

            ks_obj val = NULL;
            
            // try local variables
            if (c_frame->locals != NULL) {
                val = ks_dict_get_h(c_frame->locals, (ks_obj)name, name->v_hash);
                if (val != NULL) goto found;
            }

            // use closures to resolve the reference
            if (c_kfunc != NULL) {
                for (i = c_kfunc->closures->len - 1; i >= 0; --i) {
                    assert(c_kfunc->closures->elems[i]->type == ks_T_dict && "closure was not dict!");
                    val = ks_dict_get_h((ks_dict)c_kfunc->closures->elems[i], (ks_obj)name, name->v_hash);
                    if (val != NULL) goto found;
                }
            }
            
            // try global variables
            val = ks_dict_get_h(ks_globals, (ks_obj)name, name->v_hash);
            if (val != NULL) goto found;


            // else, we can't find the value, so throw an exception
            // throw an exception
            ks_throw(ks_T_Error, "Use of undeclared variable '%S'", name);
            goto EXC;

            // if found here
            found: ;

            ks_list_push(self->stk, val);
            KS_DECREF(val);

        VMED_CASE_END

        VMED_CASE_START(KSB_STORE)
            VMED_CONSUME(ksb_i32, op_i32);
            /* TODO: somehow global variables need to be declared (similar to the global keyword in python)
             * Such that 'store' instructions will access those instead
             * so, `global x, y` will look upwards (similar to load) and then set the first one it finds
             * 
             * 
             */

            ks_str name = (ks_str)code->v_const->elems[op_i32.arg];
            VME_CHECK(name->type == ks_T_str && "store [name] : 'name' must be a string");

            // get top
            ks_obj val = self->stk->elems[self->stk->len - 1];

            // set it in the globals
            // TODO: add local variables too
            assert(c_frame->locals != NULL && "'store' bytecode encountered in a stack frame that has no locals()!");
            ks_dict_set_h(c_frame->locals, (ks_obj)name, name->v_hash, val);

            //KS_DECREF(val);

        VMED_CASE_END

        VMED_CASE_START(KSB_LOAD_ATTR)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str attr = (ks_str)code->v_const->elems[op_i32.arg];
            VME_CHECK(attr->type == ks_T_str && "load_attr [name] : 'name' must be a string");

            ks_obj obj = ks_list_pop(self->stk);

            ks_obj val = ks_F_getattr->func(2, (ks_obj[]){ obj, (ks_obj)attr });
            if (!val) goto EXC;

            ks_list_push(self->stk, val);
            KS_DECREF(obj);

        VMED_CASE_END


        VMED_CASE_START(KSB_STORE_ATTR)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str attr = (ks_str)code->v_const->elems[op_i32.arg];
            VME_CHECK(attr->type == ks_T_str && "store_attr [name] : 'name' must be a string");

            assert(self->stk->len >= 2 && "store_attr : Not enough items on stack!");


            ks_obj val = ks_list_pop(self->stk);
            ks_obj obj = ks_list_pop(self->stk);

            ks_obj ret = ks_F_setattr->func(3, (ks_obj[]){ obj, (ks_obj)attr, val });
            if (!ret) goto EXC;


            // ignore it
            KS_DECREF(ret);

            ks_list_push(self->stk, val);
            KS_DECREF(obj);


        VMED_CASE_END

        VMED_CASE_START(KSB_NEW_FUNC)
            VMED_CONSUME(ksb, op);

            // get the TOS
            ks_kfunc top = (ks_kfunc)ks_list_pop(self->stk);

            assert(top->type == ks_T_kfunc && "'new_func' used on TOS which was not a kfunc!");

            ks_kfunc new_top = ks_kfunc_new_copy(top);

            ks_list_push(self->stk, (ks_obj)new_top);

            KS_DECREF(top);

        VMED_CASE_END

        VMED_CASE_START(KSB_ADD_CLOSURE)
            VMED_CONSUME(ksb, op);


            // get the TOS
            ks_kfunc top = (ks_kfunc)self->stk->elems[self->stk->len - 1];

            assert(top->type == ks_T_kfunc && "'add_closure' used on TOS which was not a kfunc!");
            assert(c_frame->locals != NULL && "'add_closure' used in stack frame which had no locals!");

            // first, add our current closures
            if (c_kfunc != NULL) {
                ks_list_pushn(top->closures, 1, &c_kfunc->closures->elems[c_kfunc->closures->len - 1]);
            }

            ks_list_push(top->closures, (ks_obj)c_frame->locals);

        VMED_CASE_END


        VMED_CASE_START(KSB_MAKE_ITER)
            VMED_CONSUME(ksb, op);

            assert(self->stk->len > 0 && "'make_iter' had stack that was empty!");


            // pop off the top item
            ks_obj top = ks_list_pop(self->stk);

            ks_obj top_iter = ks_F_iter->func(1, &top);
            KS_DECREF(top);
            if (!top_iter) {
                // exception was raised
                goto EXC;    
            }

            // otherwise, push back on 'top_iter'
            ks_list_push(self->stk, top_iter);


        VMED_CASE_END


        VMED_CASE_START(KSB_ITER_NEXT)
            VMED_CONSUME(ksb_i32, op_i32);

            //printf("len: %i\n", self->stk->len);

            assert(self->stk->len > 0 && "'iter_next' had stack that was empty!");

            // pop off the top item
            ks_obj top = self->stk->elems[self->stk->len - 1];

            assert(ks_obj_is_iterable(top) && "'iter_next', TOS was not an iterable!");

            ks_obj top_next = ks_F_next->func(1, &top);
            if (!top_next) {
                // exception was raised, check if it is 'OutOfIterError'
                if (self->exc && self->exc->type == ks_T_OutOfIterError) {
                    // ignore it and break out of the loop
                    ks_catch_ignore();
                    // now, jump in byte code to the end of the loop
                    c_pc += op_i32.arg;
                } else {
                    // handle unrelated exception
                    goto EXC;
                }
            } else {

                // otherwise, push back on 'top_iter'
                ks_list_push(self->stk, top_next);

            }

        VMED_CASE_END

        // template for a binary operator case
        // 3rd argument is the 'extra code' to be ran to possibly shortcut it
        #define T_BOP_CASE(_bop,  _str, _func, ...) { \
            VMED_CASE_START(_bop) \
                VMED_CONSUME(ksb, op); \
                ks_obj R = ks_list_pop(self->stk); \
                ks_obj L = ks_list_pop(self->stk); \
                { __VA_ARGS__ } \
                ks_obj ret = (_func->func)(2, (ks_obj[]){L, R}); \
                if (!ret) goto EXC; \
                ks_list_push(self->stk, ret); \
                KS_DECREF(L); KS_DECREF(R); KS_DECREF(ret); \
            VMED_CASE_END \
        }

        // implement all the operators
        T_BOP_CASE(KSB_BOP_ADD, "+", ks_F_add, {});
        T_BOP_CASE(KSB_BOP_SUB, "-", ks_F_sub, {});
        T_BOP_CASE(KSB_BOP_MUL, "*", ks_F_mul, {});
        T_BOP_CASE(KSB_BOP_DIV, "/", ks_F_div, {});
        T_BOP_CASE(KSB_BOP_MOD, "%", ks_F_mod, {});
        T_BOP_CASE(KSB_BOP_POW, "**", ks_F_pow, {});

        T_BOP_CASE(KSB_BOP_BINOR, "|", ks_F_binor, {});
        T_BOP_CASE(KSB_BOP_BINAND, "&", ks_F_binand, {});
        T_BOP_CASE(KSB_BOP_BINXOR, "^", ks_F_binxor, {});

        T_BOP_CASE(KSB_BOP_CMP, "<=>", ks_F_cmp, {});

        T_BOP_CASE(KSB_BOP_LT, "<", ks_F_lt, {});
        T_BOP_CASE(KSB_BOP_LE, "<=", ks_F_le, {});
        T_BOP_CASE(KSB_BOP_GT, ">", ks_F_gt, {});
        T_BOP_CASE(KSB_BOP_GE, ">=", ks_F_ge, {});
        T_BOP_CASE(KSB_BOP_EQ, "==", ks_F_eq, {});
        T_BOP_CASE(KSB_BOP_NE, "!=", ks_F_ne, {});

        // template for a unary operator case
        // 3rd argument is the 'extra code' to be ran to possibly shortcut it
        #define T_UOP_CASE(_uop,  _str, _func, ...) { \
            VMED_CASE_START(_uop) \
                VMED_CONSUME(ksb, op); \
                ks_obj V = ks_list_pop(self->stk); \
                { __VA_ARGS__ } \
                ks_obj ret = (_func->func)(1, &V); \
                if (!ret) goto EXC; \
                ks_list_push(self->stk, ret); \
                KS_DECREF(V); KS_DECREF(ret); \
            VMED_CASE_END \
        }

        T_UOP_CASE(KSB_UOP_POS, "+", ks_F_pos, {})
        T_UOP_CASE(KSB_UOP_NEG, "-", ks_F_neg, {})
        T_UOP_CASE(KSB_UOP_SQIG, "~", ks_F_sqig, { })

        VMED_CASE_START(KSB_UOP_NOT)
            VMED_CONSUME(ksb, op);

            ks_obj TOS = ks_list_pop(self->stk);
            int truthy = ks_obj_truthy(TOS);
            KS_DECREF(TOS);
            if (truthy < 0) goto EXC;

            // invert it
            ks_list_push(self->stk, truthy == 0 ? KSO_TRUE : KSO_FALSE);

        VMED_CASE_END

    VMED_END


    EXC: ;

    // error handler
    if (exc_i > start_ecs) {
        // there is a 'try'/'catch' block, so run that

        // grab the call stack at which the error occured
        ks_list exc_info = NULL;
        ks_obj exc = ks_catch(&exc_info);
        if (exc_info) KS_DECREF(exc_info);

        // we have a handler ready, so push it on the stack & execute
        ks_list_push(self->stk, exc);
        KS_DECREF(exc);
        c_pc = exchs[exc_i--].to_c_pc;
        goto dispatch;
    }


    // else, return NULL, and see if someone above us handles it
    ret_val = NULL;
    goto RET;

    RET: ;



    // rewind stack, just in case
    while (self->stk->len > start_stk_len) {
        ks_list_popu(self->stk);
    }

    // free any temporary arguments
    ks_free(args);

    return ret_val;
}

