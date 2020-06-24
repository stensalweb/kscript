/* exec.c - implementation of the internal bytecode execution engine for kscript
 *
 * 
 * There is a switch-case, and a computed-goto configuration for the execution engine
 * 
 * 
 * 
 * 
 * More on computed goto: https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// define one of 'VME__SWITCHCASE' or 'VME__GOTO' to switch between switch case statements, and computed goto
//   (https://eli.thegreenplace.net/2012/07/12/computed-goto-for-efficient-dispatch-tables)

// by default, use switch case statements, as these will handle malformed inputs and give a useful error
//   message if there is an error
#define VME__SWITCHCASE

// for more optimized builds, use the computed goto to jump directly to the corresponding targets
// Note that if there is incorrectly formed bytecode, this will cause a crash and there will not be a good reason
//   printed, so this is not useful for debugging
//#define VME__GOTO

// yield the GIL temporarily and immediately take it back before continuing to execute
#define VME_YIELDGIL { ks_GIL_unlock(); ks_GIL_lock(); }

// disable yielding GIL
//#define VME_YIELDGIL { }


#define VME_ASSERT(...) assert(__VA_ARGS__)
#define VME_ABORT() assert(false && "Internal Virtual Machine Error");

// now, define the correct macros
// VMED = Virtual Machine Execution Dispatch, these macros have to do with internal code generation

#if defined(VME__SWITCHCASE)

#define VMED_START while (true) switch (*c_pc) {
#define VMED_END default: fprintf(stderr, "ERROR: in kscript VM exec (%p), unknown instruction code '%i' encountered\n", code, (int)*c_pc); VME_ABORT(); break;}

#define VMED_NEXT() { if (++gilct % 8 == 0) { VME_YIELDGIL } continue; }

#define VMED_CASE_START(_bc) case _bc: {
#define VMED_CASE_END VMED_NEXT() }

#elif defined(VME__GOTO)

#define VMED_START { VMED_NEXT();
#define VMED_END }

#define VMED_NEXT() { VME_YIELDGIL goto *goto_targets[*c_pc]; }

#define VMED_CASE_START(_bc) lbl_##_bc: {
#define VMED_CASE_END VMED_NEXT() }

#else
#error No VME__ used for execution (either 'VME__SWITCHCASE' or 'VME__GOTO' should be defined)
#endif


// a structure describing an item in the exception handler call stack
typedef struct {

    // where to go if an exception is raised
    ksb* to_c_pc;

} exc_call_stk_item;


// internal execution algorithm
//ks_obj ks_thread_call_code(ks_thread self, ks_code code) {
ks_obj ks__exec(ks_code code) {
    ks_thread self = ks_thread_get();
    assert(self != NULL && "'ks__exec()' called outside of a thread!");
    assert(self->stack_frames->len > 0 && "No stack frames available!");

    uint32_t gilct = 0;


    // the current position in the exc_call_stk array
    int exc_call_stk_p = -1;
    exc_call_stk_item exc_call_stk[256];

    // current stack frame
    ks_stack_frame this_stack_frame = (ks_stack_frame)self->stack_frames->elems[self->stack_frames->len - 1];

    // see if it was part of a kfunc
    ks_kfunc this_kfunc = (ks_kfunc)this_stack_frame->kfunc;

    // start program counter at the beginning
    #define c_pc (this_stack_frame->pc)

    // set program counter
    c_pc = code->bc;

    // what will be returned
    ks_obj ret_val = NULL;

    // starting length
    int start_stk_len = self->stk->len;

    // number of args
    int args_n = 0;

    // arguments
    ks_obj* args = NULL;
    
    // ensure 'args' holds enough
    #define ENSURE_ARGS(_n) { if ((_n) >= args_n) { args_n = (_n); args = ks_realloc(args, sizeof(*args) * args_n); } }

    ksb op;

    // where did we start on the exception call stack?
    int start_ecs = exc_call_stk_p;

    // temporary variables
    int i, j;

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


    // label to dispatch from
    dispatch: ;

    // inner dispatch loop for consuming bytecode
    VMED_START

        VMED_CASE_START(KSB_NOOP)
            VMED_CONSUME(ksb, op);

        VMED_CASE_END

        /* STACK MANIPULATION */

        VMED_CASE_START(KSB_PUSH)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_list_push(self->stk, code->v_const->elems[op_i32.arg]);

        VMED_CASE_END

        VMED_CASE_START(KSB_DUP)
            VMED_CONSUME(ksb, op);

            ks_list_push(self->stk, self->stk->elems[self->stk->len - 1]);

        VMED_CASE_END

        VMED_CASE_START(KSB_POPU)
            VMED_CONSUME(ksb, op);

            ks_obj val = ks_list_pop(self->stk);

            KS_DECREF(val);

        VMED_CASE_END

        VMED_CASE_START(KSB_GETITEM)
            VMED_CONSUME(ksb_i32, op_i32);

            // first, consume an array to call

            // load args
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

        VMED_CASE_START(KSB_LIST)
            VMED_CONSUME(ksb_i32, op_i32);

            VME_ASSERT(self->stk->len >= op_i32.arg && "'list' instruction required more arguments than existed!");

            // construct the list
            ks_list new_list = ks_list_new(op_i32.arg, &self->stk->elems[self->stk->len - op_i32.arg]);

            // remove from the stack
            for (i = 0; i < op_i32.arg; ++i) {
                ks_list_popu(self->stk);
            }

            // push it back on
            ks_list_push(self->stk, (ks_obj)new_list);
            KS_DECREF(new_list);

        VMED_CASE_END


        VMED_CASE_START(KSB_TUPLE)
            VMED_CONSUME(ksb_i32, op_i32);

            VME_ASSERT(self->stk->len >= op_i32.arg && "'tuple' instruction required more arguments than existed!");

            // construct the tuple
            ks_tuple new_tuple = ks_tuple_new(op_i32.arg, &self->stk->elems[self->stk->len - op_i32.arg]);

            // remove from the stack
            for (i = 0; i < op_i32.arg; ++i) {
                ks_list_popu(self->stk);
            }

            // push it back on
            ks_list_push(self->stk, (ks_obj)new_tuple);
            KS_DECREF(new_tuple);

        VMED_CASE_END

        /* CONTROL FLOW */

        VMED_CASE_START(KSB_CALL)
            VMED_CONSUME(ksb_i32, op_i32);

            // load args
            ENSURE_ARGS(op_i32.arg);
            ks_list_popn(self->stk, op_i32.arg, args);

            ks_obj ret = ks_call(args[0], op_i32.arg - 1, &args[1]);
            if (!ret) {
                // err
                goto EXC;
            }
            
            ks_list_push(self->stk, ret);

            for (i = 0; i < op_i32.arg; ++i) KS_DECREF(args[i]);

        VMED_CASE_END

        VMED_CASE_START(KSB_RET)
            VMED_CONSUME(ksb, op);

            // return TOS
            ret_val = ks_list_pop(self->stk);
            goto RET;

        VMED_CASE_END

        VMED_CASE_START(KSB_THROW)
            VMED_CONSUME(ksb, op);

            // throw it
            ks_obj exc_obj = ks_list_pop(self->stk);
            ks_throw(exc_obj);

            // handle it
            goto EXC;

        VMED_CASE_END

        VMED_CASE_START(KSB_ASSERT)
            VMED_CONSUME(ksb, op);

            // throw it
            ks_obj ass_obj = ks_list_pop(self->stk);
            int truthy = ks_truthy(ass_obj);
            KS_DECREF(ass_obj);
            if (truthy < 0) goto EXC;

            if (!truthy) {
                ks_throw_fmt(ks_type_AssertError, "'assert' statement failed!");
                goto EXC;
            }

        VMED_CASE_END


        VMED_CASE_START(KSB_JMP)
            VMED_CONSUME(ksb_i32, op_i32);

            // increment program counter
            c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_JMPT)
            VMED_CONSUME(ksb_i32, op_i32);

            // take the top item off
            ks_obj cond = ks_list_pop(self->stk);
            int truthy = ks_truthy(cond);
            KS_DECREF(cond);
            if (truthy < 0) goto EXC;

            if (truthy) {
                // do jump
                c_pc += op_i32.arg;
            }

        VMED_CASE_END

        VMED_CASE_START(KSB_JMPF)
            VMED_CONSUME(ksb_i32, op_i32);


            // take the top item off
            ks_obj cond = ks_list_pop(self->stk);
            int truthy = ks_truthy(cond);
            
            if (truthy < 0) goto EXC;
            KS_DECREF(cond);

            if (!truthy) {
                // do jump
                c_pc += op_i32.arg;
            }

        VMED_CASE_END


        VMED_CASE_START(KSB_TRY_START)
            VMED_CONSUME(ksb_i32, op_i32);

            VME_ASSERT(exc_call_stk_p < 4095 && "EXC Call Stack Exceeded!");

            // add the handler address
            exc_call_stk[++exc_call_stk_p] = (exc_call_stk_item){ .to_c_pc = c_pc + op_i32.arg };


        VMED_CASE_END

        VMED_CASE_START(KSB_TRY_END)
            VMED_CONSUME(ksb_i32, op_i32);

            VME_ASSERT(exc_call_stk_p >= 0 && "Input Bytecode Malformed; deleting exc call stack item where none exists!");

            // handle try block end
            // TODO

            exc_call_stk_p--;

            // now, perform an unconditional jump
            c_pc += op_i32.arg;


        VMED_CASE_END


        /* VALUE LOOKUP */

        VMED_CASE_START(KSB_LOAD)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str name = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(name->type == ks_type_str && "load [name] : 'name' must be a string");

            ks_obj val = NULL;
            
            // try local variables
            if (this_stack_frame->locals != NULL) {
                val = ks_dict_get(this_stack_frame->locals, name->v_hash, (ks_obj)name);
                if (val != NULL) goto found;
            }

            // use closures to resolve the reference
            if (this_kfunc != NULL) {
                for (i = this_kfunc->closures->len - 1; i >= 0; --i) {
                    val = ks_dict_get((ks_dict)this_kfunc->closures->elems[i], name->v_hash, (ks_obj)name);
                    if (val != NULL) goto found;
                }
            }
            
            // try global variables
            val = ks_dict_get(ks_globals, name->v_hash, (ks_obj)name);
            if (val != NULL) goto found;


            // else, we can't find the value, so throw an exception
            // throw an exception
            ks_throw_fmt(ks_type_Error, "Use of undeclared variable '%S'", name);
            goto EXC;

            // if found here
            found: ;

            ks_list_push(self->stk, val);
            KS_DECREF(val);


            // increment program counter
            //c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_STORE)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str name = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(name->type == ks_type_str && "store [name] : 'name' must be a string");

            // get top
            ks_obj val = self->stk->elems[self->stk->len - 1];

            // set it in the globals
            // TODO: add local variables too
            assert(this_stack_frame->locals != NULL && "'store' bytecode encountered in a stack frame that has no locals()!");
            ks_dict_set(this_stack_frame->locals, name->v_hash, (ks_obj)name, val);

            // increment program counter
            //c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_LOAD_ATTR)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str attr = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(attr->type == ks_type_str && "load_attr [name] : 'name' must be a string");

            ks_obj obj = ks_list_pop(self->stk);

            ks_obj val = ks_F_getattr->func(2, (ks_obj[]){ obj, (ks_obj)attr });
            if (!val) goto EXC;

            ks_list_push(self->stk, val);
            KS_DECREF(obj);

            // increment program counter
            //c_pc += op_i32.arg;

        VMED_CASE_END


        VMED_CASE_START(KSB_STORE_ATTR)
            VMED_CONSUME(ksb_i32, op_i32);

            ks_str attr = (ks_str)code->v_const->elems[op_i32.arg];
            VME_ASSERT(attr->type == ks_type_str && "store_attr [name] : 'name' must be a string");

            assert(self->stk->len >= 2 && "store_attr : Not enough items on stack!");


            ks_obj val = ks_list_pop(self->stk);
            ks_obj obj = ks_list_pop(self->stk);

            ks_obj ret = ks_F_setattr->func(3, (ks_obj[]){ obj, (ks_obj)attr, val });
            if (!ret) goto EXC;


            // ignore it
            KS_DECREF(ret);

            ks_list_push(self->stk, val);
            KS_DECREF(obj);

            // increment program counter
            //c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_NEW_FUNC)
            VMED_CONSUME(ksb, op);

            // get the TOS
            ks_kfunc top = (ks_kfunc)ks_list_pop(self->stk);

            assert(top->type == ks_type_kfunc && "'new_func' used on TOS which was not a kfunc!");

            ks_list_push(self->stk, (ks_obj)ks_kfunc_new_copy(top));

            KS_DECREF(top);

            // increment program counter
            //c_pc += op_i32.arg;

        VMED_CASE_END

        VMED_CASE_START(KSB_ADD_CLOSURE)
            VMED_CONSUME(ksb, op);

            // get the TOS
            ks_kfunc top = (ks_kfunc)self->stk->elems[self->stk->len - 1];

            assert(top->type == ks_type_kfunc && "'add_closure' used on TOS which was not a kfunc!");
            assert(this_stack_frame->locals != NULL && "'add_closure' used in stack frame which had no locals!");

            // first, add our current closures
            if (this_kfunc != NULL) {
                ks_list_pushn(top->closures, 1, &this_kfunc->closures->elems[this_kfunc->closures->len - 1]);
            }

            ks_list_push(top->closures, (ks_obj)this_stack_frame->locals);

            // increment program counter
            //c_pc += op_i32.arg;

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

            assert(ks_is_iterable(top) && "'iter_next', TOS was not an iterable!");

            ks_obj top_next = ks_F_next->func(1, &top);
            if (!top_next) {
                // exception was raised, check if it is 'OutOfIterError'
                if (self->exc && self->exc->type == ks_type_OutOfIterError) {
                    KS_DECREF(self->exc);
                    self->exc = NULL;
                    if (self->exc_info) KS_DECREF(self->exc_info);
                    self->exc_info = NULL;
                    // now, jump in byte code to the end of the loop
                    c_pc += op_i32.arg;
                } else {
                    // handle unrelatex exception
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

        T_UOP_CASE(KSB_UOP_NEG, "-", ks_F_neg, {})
        T_UOP_CASE(KSB_UOP_SQIG, "~", ks_F_sqig, {})

        VMED_CASE_START(KSB_TRUTHY)
            VMED_CONSUME(ksb, op);

            ks_obj TOS = ks_list_pop(self->stk);
            int truthy = ks_truthy(TOS);
            KS_DECREF(TOS);
            if (truthy < 0) goto EXC;


            ks_list_push(self->stk, truthy == 0 ? KSO_FALSE : KSO_TRUE);

        VMED_CASE_END


        VMED_CASE_START(KSB_UOP_NOT)
            VMED_CONSUME(ksb, op);

            ks_obj TOS = ks_list_pop(self->stk);
            int truthy = ks_truthy(TOS);
            KS_DECREF(TOS);
            if (truthy < 0) goto EXC;

            // invert it
            ks_list_push(self->stk, truthy == 0 ? KSO_TRUE : KSO_FALSE);

        VMED_CASE_END


    VMED_END


    EXC: ;

    // error handler
    if (exc_call_stk_p > start_ecs) {
        // there is a 'try'/'catch' block, so run that

        // grab the call stack at which the error occured
        ks_list exc_info = ks_list_new(0, NULL);
        ks_obj exc = ks_catch2(exc_info);

        // we have a handler ready, so push it on the stack & execute
        ks_list_push(self->stk, exc);
        KS_DECREF(exc);
        c_pc = exc_call_stk[exc_call_stk_p--].to_c_pc;
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


// end with an error
void ks_errend() {
    ks_list exc_info = ks_list_new(0, NULL);
    ks_obj exc = ks_catch2(exc_info);
    assert(exc != NULL && "ks_errend() called with no exception!");

    // error, so rethrow it and return
    ks_printf(RED BOLD "%T" RESET ": %S\n", exc, exc);

    // print in reverse order
    ks_printf("Call Stack:\n");
    
    int i;
    for (i = 0; i < exc_info->len; i++) {
        ks_printf("%*c#%i: In %S\n", 2, ' ', i, exc_info->elems[i]);
    }

    
    ks_printf("In thread %R @ %p\n", ks_thread_get()->name, ks_thread_get());
    // exit with error
    exit(1);
}
