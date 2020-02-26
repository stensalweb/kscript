/* exec.c - the main file implementing the bytecode interpreter's execution loop 

kscript uses a bytecode interpreter with computed goto to have a fairly efficient execution cycle.

Much is still in the air about this though; I may switch to smaller instructions, more instructions, and
internals are still very much not-pinned-down.

The biggest things to handle from here are type/module/function creation & closures. This includes
possibly making a frame and actual kscript object so it is reference counted, and closures would
keep a reference to it. I think that is how it will end up, but I'm not rushing right now.

*/

#include "ks.h"

// comment this out to disable execution tracing
// by default, this should be off
//#define DO_EXEC_TRACE

#ifdef DO_EXEC_TRACE
// if execution tracing is enabled, debug out the arguments
#define _exec_trace(...) { ks_debug("[EXE] " __VA_ARGS__); }
#else
#define _exec_trace(...)
#endif

// the virtual machine
static struct {

    // the virtual machine's stack
    struct vm_stk {

        // base, start of the stack
        kso* base;

        // current length of the stack
        int len;

        // maximum length of the stack
        int __max_len;

    } stk;

    // the virtual machine's frame stack
    struct vm_frame_stk {

        // current program counter
        uint8_t* pc;

        // the start of the bytecode currently executing
        uint8_t* start_bc;

        // the current constant's list
        ks_list v_const;

        // the current code executing
        ks_code code;

        // how large the stack was when this frame was entered
        int start_stk_len;

        // the variables local to this frame stack
        ks_dict local_vars;

    }* frame_stk;


    // length of the frame stack, and its maximum
    int frame_stk_len, frame_stk_len_max;


    // stack of exception handling frames
    struct vm_eframe_stk {

        // capture `VM.frame_stk_len` when the eframe is registered
        int frame_stk_len;

        // capture `VM.stk_len` when the eframe is registered
        int stk_len;

        // where to start executing in case of an error
        uint8_t* exc_to;

    }* eframe_stk;

    // length of the exception frame stack
    int eframe_stk_len;

    // the global variables
    ks_dict globals;

    
} VM = {
    .stk = {NULL, 0, 0}, 
    // frame_stk
    NULL, 0, 0, 
    // eframe_stk
    NULL, 0,

    NULL
};


// adds a frame to the VM, returns the index
static int VM_push_frame(ks_code code) {
    int idx = VM.frame_stk_len++;
    if (VM.frame_stk_len > VM.frame_stk_len_max) {
        VM.frame_stk = realloc(VM.frame_stk, VM.frame_stk_len * sizeof(*VM.frame_stk));
        VM.frame_stk_len_max = VM.frame_stk_len;
    }

    // just initialize every thing here
    VM.frame_stk[idx].code = code;
    VM.frame_stk[idx].start_bc = VM.frame_stk[idx].pc = code->bc;
    VM.frame_stk[idx].v_const = code->v_const;
    VM.frame_stk[idx].start_stk_len = VM.stk.len;
    VM.frame_stk[idx].local_vars = ks_dict_new_empty();
    return idx;
}

// pops off the top frame from the VM
static void VM_pop_frame() {
    int idx = --VM.frame_stk_len;
    KSO_DECREF(VM.frame_stk[idx].local_vars)
}


/* exception frames */

static inline int VM_push_eframe(uint8_t* exc_to) {
    int idx = VM.eframe_stk_len++;
    VM.eframe_stk = realloc(VM.eframe_stk, VM.eframe_stk_len * sizeof(*VM.eframe_stk));
    VM.eframe_stk[idx].stk_len = VM.stk.len;
    VM.eframe_stk[idx].frame_stk_len = VM.frame_stk_len;
    VM.eframe_stk[idx].exc_to = exc_to;
    return idx;
}

static inline int VM_pop_eframe() {
    VM.eframe_stk_len--;
}


/* value stack operations */

// pushes an object on the stack, returning the index
static inline int VM_stk_push(kso obj) {
    int idx = VM.stk.len++;
    if (VM.stk.len > VM.stk.__max_len) {
        VM.stk.base = realloc(VM.stk.base, sizeof(kso) * VM.stk.len);
        VM.stk.__max_len = VM.stk.len;
    }
    KSO_INCREF(obj);
    VM.stk.base[idx] = obj;
}

// pushes an object on the stack, returning the index
// NOTE: Does not record a reference
static inline int VM_stk_pushu(kso obj) {
    int idx = VM.stk.len++;
    if (VM.stk.len > VM.stk.__max_len) {
        VM.stk.base = realloc(VM.stk.base, sizeof(kso) * VM.stk.len);
        VM.stk.__max_len = VM.stk.len;
    }
    //KSO_INCREF(obj);
    VM.stk.base[idx] = obj;
}

// pops an item off the stack, but does not delete reference
static inline kso VM_stk_pop() {
    return VM.stk.base[--VM.stk.len];
}

// pops an unused item off the stack
static inline void VM_stk_popu() {
    kso obj = VM.stk.base[--VM.stk.len];
    KSO_DECREF(obj);
}

// pops `n` items off the stack, but does not remove their reference
static inline kso* VM_stk_popun(int n) {
    return &VM.stk.base[VM.stk.len -= n];
}

// returns the item on the top of the stack
static inline kso VM_stk_top() {
    return VM.stk.base[VM.stk.len - 1];
}


// internal execution routine, this should only really be used by internal functions
static void VM_exec() {

    /* helpers */
    #define DECREF_N(_objp, _n) { int _i, _stop = (_n); for (_i = 0; _i < _stop; ++_i) { KSO_DECREF((_objp)[_i]); } }

    /* instructions/computed goto */

    // macro to help initialize the instruction label by instruction name
    #define INIT_INST_LABEL(_inst_name) [_inst_name] = &&lbl_##_inst_name,

    // declare the actual label where the code appears
    #define INST_LABEL(_inst_name) lbl_##_inst_name: ; pre_bc = pc;

    // go to the next instruction, assuming `pc` is pointed at a valid bytecode
    #define NEXT_INST() goto *inst_labels[*pc];

    // array of labels for computed goto. This will essentially allow us to just jump directly
    // to the correct position without 2 jumps per loop (so it saves jumping/branching by a factor of 2)
    static void* inst_labels[] = {

        INIT_INST_LABEL(KSBC_NOOP)
        INIT_INST_LABEL(KSBC_DUP)
        INIT_INST_LABEL(KSBC_POPU)

        INIT_INST_LABEL(KSBC_CONST)
        INIT_INST_LABEL(KSBC_CONST_TRUE)
        INIT_INST_LABEL(KSBC_CONST_FALSE)
        INIT_INST_LABEL(KSBC_CONST_NONE)
        INIT_INST_LABEL(KSBC_LOAD)
        INIT_INST_LABEL(KSBC_LOAD_A)
        INIT_INST_LABEL(KSBC_STORE)
        INIT_INST_LABEL(KSBC_STORE_A)
        INIT_INST_LABEL(KSBC_CALL)
        INIT_INST_LABEL(KSBC_GETITEM)
        INIT_INST_LABEL(KSBC_SETITEM)
        INIT_INST_LABEL(KSBC_TUPLE)
        INIT_INST_LABEL(KSBC_LIST)

        INIT_INST_LABEL(KSBC_ADD)
        INIT_INST_LABEL(KSBC_SUB)
        INIT_INST_LABEL(KSBC_MUL)
        INIT_INST_LABEL(KSBC_DIV)
        INIT_INST_LABEL(KSBC_MOD)
        INIT_INST_LABEL(KSBC_POW)
        INIT_INST_LABEL(KSBC_LT)
        INIT_INST_LABEL(KSBC_LE)
        INIT_INST_LABEL(KSBC_GT)
        INIT_INST_LABEL(KSBC_GE)
        INIT_INST_LABEL(KSBC_EQ)
        INIT_INST_LABEL(KSBC_NE)

        INIT_INST_LABEL(KSBC_NEG)
        INIT_INST_LABEL(KSBC_SQIG)

        INIT_INST_LABEL(KSBC_JMP)
        INIT_INST_LABEL(KSBC_JMPT)
        INIT_INST_LABEL(KSBC_JMPF)

        INIT_INST_LABEL(KSBC_RET)
        INIT_INST_LABEL(KSBC_RET_NONE)

        INIT_INST_LABEL(KSBC_EXC_ADD)
        INIT_INST_LABEL(KSBC_EXC_REM)

        INIT_INST_LABEL(KSBC_THROW)

    };

    // call like DECODE(ksbc_), or DECODE(ksbc_i32) to read and pass an instruction of that
    //   kind at the program counter, incrementing the program counter
    #define DECODE(_inst_type) { inst = (ksbc)(*(_inst_type*)pc); pc += sizeof(_inst_type); }

    // variable to hold the currently executing instruction's data
    ksbc inst;

    // internal macro
    #define _EXEC_ERR(...) { \
        int i, bc_n = RELADDR, haderr = 0; \
        for (i = 0; i < CUR_SCOPE().code->meta_ast_n; ++i) { \
            if (CUR_SCOPE().code->meta_ast[i].bc_n >= bc_n) { \
                haderr = 1; \
                kse_tok(CUR_SCOPE().code->meta_ast[i].ast->tok_expr, __VA_ARGS__); \
                break; \
            } \
        } \
        if (haderr = 0) kse_fmt(__VA_ARGS__); \
    }

    // execution error, such as value not found, etc
    // give it printf-style strings
    #define EXEC_ERR(...) { \
        _EXEC_ERR(__VA_ARGS__); \
        goto EXC; \
    }

    // execution error, reclaim from the global error stack, if possible
    #define EXEC_ERR_RECLAIM(...) { \
        if (kse_N() > 0) { \
            kso last_err = kse_pop(); \
            _EXEC_ERR("%S", last_err); \
            KSO_DECREF(last_err); \
        } else { \
            _EXEC_ERR(__VA_ARGS__); \
        } \
        goto EXC; \
    }

    /* values/temporary variables */

    // gets a constant from the current scope's constant pool
    #define GET_CONST(_idx) (CUR_SCOPE().v_const->items[(_idx)])

    // the looked up value from the constants table (and a string specific one)
    kso v_c = NULL;
    ks_str v_str = NULL;

    // the value, if it is being popped from the top of the stack, and one that's view only,
    //   and one used as the function
    kso popped = NULL, top = NULL, func = NULL, val = NULL;

    // the value of the thing that was being searched (for example through a load command)
    kso found = NULL;

    // the value of the new object created
    kso new_obj = NULL;

    // a pointer to arguments (like for example, a section of the stack)
    kso* args_p = NULL;

    // immediate argument holders, for things like operators and/or small function calls
    kso imm_args[8];

    // tells how many args there are
    int n_args = 0;

    /* frame/scope pushing */

    // now, record where we started on the VM's frame stack
    int start_fsl = VM.frame_stk_len;

    // relative address in the bytecode
    #define RELADDR ((int)(pre_bc - CUR_SCOPE().start_bc))

    #define CUR_SCOPE() (VM.frame_stk[VM.frame_stk_len - 1])

    /* execution time */

    // program counter, i.e. address of currently executing code, and a pre-decode position
    uint8_t* pc = CUR_SCOPE().pc, *pre_bc = NULL;

    // now, start executing at the first instruction
    NEXT_INST();

    /* here is where the actual instruction labels are held */
    {

        INST_LABEL(KSBC_NOOP)
            DECODE(ksbc_);
            _exec_trace("noop");

            NEXT_INST();

        INST_LABEL(KSBC_DUP)
            DECODE(ksbc_);
            _exec_trace("dup");
            VM_stk_push(VM_stk_top());
            NEXT_INST();

        INST_LABEL(KSBC_POPU)
            DECODE(ksbc_);
            _exec_trace("popu");
            VM_stk_popu();
            NEXT_INST();


        INST_LABEL(KSBC_CONST)
            DECODE(ksbc_i32);
            v_c = GET_CONST(inst.i32.i32);
            // do type generic logging
            _exec_trace("const %R # [%i]", v_c, inst.i32.i32);

            VM_stk_push(v_c);
            NEXT_INST();

        INST_LABEL(KSBC_CONST_TRUE)
            DECODE(ksbc_);
            _exec_trace("const true");
            VM_stk_push(KSO_TRUE);
            NEXT_INST();

        INST_LABEL(KSBC_CONST_FALSE)
            DECODE(ksbc_);
            _exec_trace("const false");
            VM_stk_push(KSO_FALSE);
            NEXT_INST();

        INST_LABEL(KSBC_CONST_NONE)
            DECODE(ksbc_);
            _exec_trace("const none");
            VM_stk_push(KSO_NONE);

            NEXT_INST();

        INST_LABEL(KSBC_LOAD)
            DECODE(ksbc_i32);
            v_str = (ks_str)GET_CONST(inst.i32.i32);
            _exec_trace("load %R # [%i]", v_str, inst.i32.i32);

            int i;
            for (i = VM.frame_stk_len - 1; i >= 0; --i) {
                found = ks_dict_get(VM.frame_stk[i].local_vars, (kso)v_str, v_str->v_hash);
                if (found != NULL) goto load_resolve;
            }
            
            found = ks_dict_get(VM.globals, (kso)v_str, v_str->v_hash);
            if (found == NULL) EXEC_ERR("Unknown variable '%S'", v_str);

            load_resolve: ;

            // otherwise, it was found, so pop it on the stack
            VM_stk_push(found);

            NEXT_INST();

        INST_LABEL(KSBC_LOAD_A)
            DECODE(ksbc_i32);
            v_str = (ks_str)GET_CONST(inst.i32.i32);
            _exec_trace("load_a %R # [%i]", v_str, inst.i32.i32);

            imm_args[0] = VM_stk_pop();
            imm_args[1] = (kso)v_str;

            found = ks_F_getattr->v_cfunc(2, imm_args);
            if (found == NULL) {
                KSO_DECREF(imm_args[0]);

                EXEC_ERR_RECLAIM("KeyError: %R", v_str);
            }

            VM_stk_pushu(found);
            KSO_DECREF(imm_args[0]);

            NEXT_INST();

        INST_LABEL(KSBC_STORE)
            DECODE(ksbc_i32);
            v_str = (ks_str)GET_CONST(inst.i32.i32);
            _exec_trace("store %R # [%i]", v_str, inst.i32.i32);

            top = VM_stk_top();
            ks_dict_set(CUR_SCOPE().local_vars, (kso)v_str, v_str->v_hash, top);

            NEXT_INST();

        INST_LABEL(KSBC_STORE_A)
            DECODE(ksbc_i32);
            v_str = (ks_str)GET_CONST(inst.i32.i32);
            _exec_trace("store_a %R # [%i]", v_str, inst.i32.i32);

            imm_args[2] = VM_stk_pop();
            imm_args[1] = (kso)v_str;
            imm_args[0] = VM_stk_pop();

            found = ks_F_setattr->v_cfunc(3, imm_args);
            if (found == NULL) {
                KSO_DECREF(imm_args[0]);
                KSO_DECREF(imm_args[2]);
                EXEC_ERR_RECLAIM("KeyError: %R", v_str);
            }

            VM_stk_pushu(found);
            KSO_DECREF(imm_args[0]);
            KSO_DECREF(imm_args[2]);

            NEXT_INST();

        INST_LABEL(KSBC_CALL)
            DECODE(ksbc_i32);
            _exec_trace("call %i", inst.i32.i32);

            // get a pointer to the arguments
            args_p = VM_stk_popun(inst.i32.i32);

            // grab a function from the bottom
            func = *args_p++;

            if (func->type == ks_T_kfunc) {

                // just set them to the parameters
                int n_args = inst.i32.i32 - 1;
                ks_kfunc kf = (ks_kfunc)func;
                if (n_args != kf->params->len) {
                    EXEC_ERR("Tried calling a function that takes %i args with %i args instead", kf->params->len, n_args);
                }

                // otherwise, push on a new scope
                CUR_SCOPE().pc = pc;
                VM_push_frame(kf->code);
                pc = CUR_SCOPE().pc;

                // set up the rest of the scope
                int i;
                for (i = 0; i < kf->params->len; ++i) {
                    // set all the parameters
                    ks_str arg_name = (ks_str)kf->params->items[i];
                    ks_dict_set(CUR_SCOPE().local_vars, (kso)arg_name, arg_name->v_hash, args_p[i]);
                }

                // deref these, since we set them
                DECREF_N(args_p, n_args);
                // and we're done with the function
                KSO_DECREF(func);

                NEXT_INST();

            } else if (func->type == ks_T_cfunc) {
                int n_args = inst.i32.i32-1;
                if (n_args <= 8) {
                    // use immediate args
                    memcpy(imm_args, args_p, n_args * sizeof(kso));

                    //new_obj = kso_call(func, inst.i32.i32 - 1, imm_args);
                    new_obj = ((ks_cfunc)func)->v_cfunc(n_args, imm_args);

                    if (new_obj == NULL) {
                        // done with arguments
                        DECREF_N(imm_args, n_args);
                        // done with the function
                        KSO_DECREF(func);

                        EXEC_ERR_RECLAIM("During function call, calling on obj: `%S`, had an exception", func)
                    }

                    VM_stk_pushu(new_obj);

                    // done with arguments
                    DECREF_N(imm_args, n_args);
                    // done with the function
                    KSO_DECREF(func);

                    NEXT_INST();
                } else {

                    EXEC_ERR("cant do this many args, sorry :(");
                }
            } else {
                //EXEC_ERR("During function call, tried calling on obj: `%S`, which did not work", func);
                int n_args = inst.i32.i32-1;

                if (n_args <= 8) {
                    memcpy(imm_args, args_p, n_args * sizeof(kso));

                    new_obj = kso_call(func, n_args, imm_args);

                    if (new_obj == NULL) {
                        DECREF_N(imm_args, n_args);
                        KSO_DECREF(func);
                        EXEC_ERR_RECLAIM("While executing a func");
                    }

                    VM_stk_pushu(new_obj);
                    DECREF_N(imm_args, n_args);
                    KSO_DECREF(func);

                    NEXT_INST();

                } else {
                    EXEC_ERR("cant do this many args, sorry :(");
                }
            }

        INST_LABEL(KSBC_GETITEM)
            DECODE(ksbc_i32);
            _exec_trace("getitem %i", inst.i32.i32);
            n_args = inst.i32.i32;

            // get arguments
            args_p = VM_stk_popun(n_args);

            if (n_args <= 8) {
                // use immediate args
                memcpy(imm_args, args_p, n_args * sizeof(kso));
                new_obj = ks_F_getitem->v_cfunc(n_args, imm_args);
                if (new_obj == NULL) {
                    // done with arguments
                    DECREF_N(imm_args, n_args);

                    EXEC_ERR_RECLAIM("During getitem call, there was an exception", func)
                }

                VM_stk_pushu(new_obj);

                // done with arguments
                DECREF_N(imm_args, n_args);

                NEXT_INST();
            } else {
                EXEC_ERR("cant do this many args, sorry :(");
            }

        INST_LABEL(KSBC_SETITEM)
            DECODE(ksbc_i32);
            _exec_trace("setitem %i", inst.i32.i32);
            n_args = inst.i32.i32;

            // get arguments
            args_p = VM_stk_popun(n_args);

            if (n_args <= 8) {
                // use immediate args
                memcpy(imm_args, args_p, n_args * sizeof(kso));
                new_obj = ks_F_setitem->v_cfunc(n_args, imm_args);
                if (new_obj == NULL) {
                    // done with arguments
                    DECREF_N(imm_args, n_args);

                    EXEC_ERR_RECLAIM("During setitem call, there was an exception", func)
                }

                VM_stk_pushu(new_obj);

                // done with arguments
                DECREF_N(imm_args, n_args);

                NEXT_INST();
            } else {
                EXEC_ERR("cant do this many args, sorry :(");
            }


        INST_LABEL(KSBC_TUPLE)
            DECODE(ksbc_i32);
            _exec_trace("tuple %i", inst.i32.i32);

            // get a pointer to the arguments
            args_p = VM_stk_popun(inst.i32.i32);

            new_obj = (kso)ks_tuple_new(args_p, inst.i32.i32);
            if (new_obj == NULL) {
                DECREF_N(args_p, inst.i32.i32);
                EXEC_ERR("Internal error during tuple creation");
            }

            // since the objects will now be referenced by the tuple, remove the stack's reference
            // TODO: Maybe have a more efficient way of constructing a tuple from freshly moved references?
            DECREF_N(args_p, inst.i32.i32);
   
            // add list
            VM_stk_pushu(new_obj);

            NEXT_INST();

        INST_LABEL(KSBC_LIST)
            DECODE(ksbc_i32);
            _exec_trace("list %i", inst.i32.i32);

            // get a pointer to the arguments
            args_p = VM_stk_popun(inst.i32.i32);

            new_obj = (kso)ks_list_new(args_p, inst.i32.i32);
            if (new_obj == NULL) {
                DECREF_N(args_p, inst.i32.i32);
                EXEC_ERR("Internal error during list creation");
            }

            DECREF_N(args_p, inst.i32.i32);

            // add list
            VM_stk_pushu(new_obj);

            NEXT_INST();


        // binary operator template
        #define T_BOP_LABEL(_BOP, _opstr, _opcfunc) INST_LABEL(_BOP) \
            DECODE(ksbc_); \
            _exec_trace("bop " _opstr); \
            imm_args[1] = VM_stk_pop(); imm_args[0] = VM_stk_pop(); \
            new_obj = _opcfunc->v_cfunc(2, imm_args); \
            if (new_obj == NULL) { \
                DECREF_N(imm_args, 2); \
                EXEC_ERR_RECLAIM("Error in op " _opstr); \
            } \
            VM_stk_pushu(new_obj); \
            DECREF_N(imm_args, 2); \
            NEXT_INST();

        T_BOP_LABEL(KSBC_ADD, "+", ks_F_add)
        T_BOP_LABEL(KSBC_SUB, "-", ks_F_sub)
        T_BOP_LABEL(KSBC_MUL, "*", ks_F_mul)
        T_BOP_LABEL(KSBC_DIV, "/", ks_F_div)
        T_BOP_LABEL(KSBC_MOD, "%%", ks_F_mod)
        T_BOP_LABEL(KSBC_POW, "/", ks_F_pow)
        T_BOP_LABEL(KSBC_LT, "<", ks_F_lt)
        T_BOP_LABEL(KSBC_LE, "<=", ks_F_le)
        T_BOP_LABEL(KSBC_GT, ">", ks_F_gt)
        T_BOP_LABEL(KSBC_GE, ">=", ks_F_ge)
        T_BOP_LABEL(KSBC_EQ, "==", ks_F_eq)
        T_BOP_LABEL(KSBC_NE, "!=", ks_F_ne)

        // unary operator template
        #define T_UOP_LABEL(_UOP, _opstr, _opcfunc) INST_LABEL(_UOP) \
            DECODE(ksbc_); \
            _exec_trace("uop " _opstr); \
            imm_args[0] = VM_stk_pop(); \
            new_obj = _opcfunc->v_cfunc(1, imm_args); \
            if (new_obj == NULL) { \
                DECREF_N(imm_args, 1); \
                EXEC_ERR_RECLAIM("Error in op " _opstr); \
            } \
            VM_stk_pushu(new_obj); \
            DECREF_N(imm_args, 1); \
            NEXT_INST();

        T_UOP_LABEL(KSBC_NEG, "-", ks_F_neg);
        T_UOP_LABEL(KSBC_SQIG, "~", ks_F_sqig);

        INST_LABEL(KSBC_JMP)
            DECODE(ksbc_i32);
            _exec_trace("jmp %+i", inst.i32.i32);

            pc += inst.i32.i32;

            NEXT_INST();

        INST_LABEL(KSBC_JMPT)
            DECODE(ksbc_i32);
            _exec_trace("jmpt %+i", inst.i32.i32);

            popped = VM_stk_pop();

            if (kso_bool(popped) == 1) {
                // check whether it would be a `true` boolean
                pc += inst.i32.i32;
            }

            KSO_DECREF(popped);
            NEXT_INST();

        INST_LABEL(KSBC_JMPF)
            DECODE(ksbc_i32);
            _exec_trace("jmpf %+i", inst.i32.i32);

            popped = VM_stk_pop();

            if (kso_bool(popped) != 1) {
                // check whether it would not be a `true` boolean
                pc += inst.i32.i32;
            }
            
            KSO_DECREF(popped);
            NEXT_INST();

        INST_LABEL(KSBC_RET)
            DECODE(ksbc_);
            _exec_trace("ret");

            // we are expecting only one value to have been added during exec
            while (VM.stk.len > 1 + CUR_SCOPE().start_stk_len) {
                VM_stk_popu();
            }

            VM_pop_frame();
            pc = CUR_SCOPE().pc;

            // if we've reached where we're supposed to be executing
            if (VM.frame_stk_len < start_fsl) {
                return;
            }

            // otherwise, continue
            NEXT_INST();

        INST_LABEL(KSBC_RET_NONE)
            DECODE(ksbc_);
            _exec_trace("ret_none");

            // we are expecting no values, since we are about to add a none
            while (VM.stk.len > CUR_SCOPE().start_stk_len) {
                VM_stk_popu();
            }

            VM_pop_frame();
            pc = CUR_SCOPE().pc;

            VM_stk_push(KSO_NONE);

            // if we've reached where we're supposed to be executing
            if (VM.frame_stk_len < start_fsl) {
                return;
            }

            // otherwise, continue
            NEXT_INST();

        INST_LABEL(KSBC_EXC_ADD)
            DECODE(ksbc_i32);
            _exec_trace("exc_add %i", inst.i32.i32);

            // push on an exception frame
            VM_push_eframe(CUR_SCOPE().code->bc + inst.i32.i32);

            // continue on
            NEXT_INST();

        INST_LABEL(KSBC_EXC_REM)
            DECODE(ksbc_);
            _exec_trace("exc_rem");

            // push on an exception frame
            VM_pop_eframe();

            // continue on
            NEXT_INST();

        INST_LABEL(KSBC_THROW)
            DECODE(ksbc_);
            _exec_trace("throw");

            // throw the item on top of the stack, so just go to EXC
            //kse_fmt("Object thrown: %R", VM_stk_top());

            _EXEC_ERR("Object thrown: %R, but not caught!", VM_stk_top());
            goto EXC_throw;

            //EXEC_ERR("Object thrown: %R", VM_stk_top());

            // continue on
            NEXT_INST();



        /* exception handling  */
        EXC_throw:
            _exec_trace("exc_throw");
            kso toperr = VM_stk_pop();
                        
            if (VM.eframe_stk_len > 0) {
                // if we have an exception handler
                // pop off the top exc handler
                struct vm_eframe_stk top = VM.eframe_stk[--VM.eframe_stk_len];


                // rewind so the stack trace is correct
                while (VM.frame_stk_len > top.frame_stk_len) {
                    VM_pop_frame();
                }

                // rewind so the main stack is correct
                while (VM.stk.len > top.stk_len) {
                    VM_stk_popu();
                }

                // push error back
                VM_stk_push(toperr);

                kse_clear();

                // now, we change the program counter to 
                pc = VM.frame_stk[VM.frame_stk_len-1].pc = top.exc_to;

                // and keep executing
                NEXT_INST();

            } else {
                // we don't, so clear everything and exit

                // rewind, then exit
                while (VM.frame_stk_len > start_fsl) {
                    VM_pop_frame();
                }

                // dump all errors
                kse_dumpall();
                
                exit(1);

                return;
            }

        EXC:
            _exec_trace("exc");
            
            if (VM.eframe_stk_len > 0) {
                // if we have an exception handler
                // pop off the top exc handler
                struct vm_eframe_stk top = VM.eframe_stk[--VM.eframe_stk_len];

                // rewind so the stack trace is correct
                while (VM.frame_stk_len > top.frame_stk_len) {
                    VM_pop_frame();
                }

                // rewind so the main stack is correct
                while (VM.stk.len > top.stk_len) {
                    VM_stk_popu();
                }


                // get the last error
                kso last_err = kse_pop();

                // add on the error to the stack
                if (last_err != NULL) VM_stk_pushu(last_err);

                // clear the rest
                kse_clear();

                // now, we change the program counter to 
                pc = VM.frame_stk[VM.frame_stk_len-1].pc = top.exc_to;

                // and keep executing
                NEXT_INST();

            } else {
                // we don't, so clear everything and exit

                // rewind, then exit
                while (VM.frame_stk_len > start_fsl) {
                    VM_pop_frame();
                }

                // dump all errors
                kse_dumpall();
                
                exit(1);

                return;
            }
    }


    // should never get here
}

// method for calling a kfunc (internal bytecode function)
static kso kso_vm_call_kfunc(ks_kfunc func, int n_args, kso* args) {
    // push on values
    if (n_args != func->params->len) {
        return kse_fmt("Tried calling %R with wrong number of args, given %i, but expected %i", func, n_args, func->params->len);
    }

    VM_push_frame(func->code);
    int i;
    for (i = 0; i < n_args; ++i) {
        ks_str param = (ks_str)func->params->items[i];
        ks_dict_set(CUR_SCOPE().local_vars, (kso)param, param->v_hash, args[i]);
    }


    VM_exec();

    kso val = VM_stk_pop();
    return val;
}


// executes code on the VM
void ks_vm_exec(ks_code code) {

    VM_push_frame(code);

    VM_exec();

    // we don't use the result
    VM_stk_popu();
}


void ks_vm_coredump() {
    #undef _EXEC_ERR
    #define _EXEC_ERR(...) { \
        int i, bc_n = CUR_SCOPE().pc, haderr = 0; \
        for (i = 0; i < CUR_SCOPE().code->meta_ast_n; ++i) { \
            if (CUR_SCOPE().code->meta_ast[i].bc_n >= bc_n) { \
                haderr = 1; \
                kse_tok(CUR_SCOPE().code->meta_ast[i].ast->tok_expr, __VA_ARGS__); \
                break; \
            } \
        } \
        if (haderr = 0) kse_fmt(__VA_ARGS__); \
    }

    _EXEC_ERR("COREDUMP");

    int i;
    for (i = 0; i < VM.frame_stk_len; ++i) {
        char *s = VM.frame_stk[i].code->hrname != NULL ? VM.frame_stk[i].code->hrname->chr : "__anon";
        printf("%2i: %s (%p)\n", (int)i, s, (void*)VM.frame_stk[i].code);
    }
    

    exit(1);
}

// call an object as a callable, with a list of arguments
kso kso_call(kso func, int n_args, kso* args) {

    // for looping
    if (func->type == ks_T_cfunc) {
        return ((ks_cfunc)func)->v_cfunc(n_args, args);
    } else if (func->type == ks_T_kfunc) {
        return kso_vm_call_kfunc((ks_kfunc)func, n_args, args);
    } else if (func->type == ks_T_type) {

        // handle it as type(vals...)
        // so, searcth for type.__new__(vals..)
        ks_type fty = (ks_type)func;
        kso f_new = fty->f_new;
        if (f_new == NULL) {
            /*if (((ks_type)func)->f_call != NULL) {
                return kso_call(((ks_type)func)->f_call, n_args, args);
            }*/
            // nothing to call, since no constructor is known for this type.
            // this is odd, most types should have a constructor
            return kse_fmt("Tried calling on type '%S', but had no `__new__` method", fty->name);
        } else {

            // search for type.__init__(vals..)
            kso f_init = fty->f_init;

            if (f_init == NULL) {
                // since there is no __init__, we will just call the `__new__(*args)` func
                // this means there are no initialization, and this is probably an immutable type
                return kso_call(f_new, n_args, args);
            } else {
                // since there is an __init__, first call __new__ with no arguments, then 
                // initialize it with `args` in __init__

                // call the creation routine, which should accept 0 arguments
                // (those arguments are passed to __init__ instead)
                kso new_val = kso_call(f_new, 0, NULL);
                if (new_val == NULL) return NULL;

                // manually set the type, because f_new will set it to the base type
                // this is helpful for derived types
                new_val->type = fty;

                // prepend `new_val` as the self in our arguments, so memcpy them
                kso* new_args = ks_malloc(sizeof(*new_args) * (1 + n_args));
                new_args[0] = new_val;
                // copy the rest, if applicable
                if (n_args > 1) memcpy(&new_args[1], args, sizeof(kso) * n_args);

                // call the initialization function with our self, arguments
                kso res = kso_call(fty->f_init, 1 + n_args, new_args);
                if (res == NULL) return NULL;

                // the return result is ignored, as we are returning our value from __new__
                KSO_DECREF(res);

                // free our temporary arguments
                ks_free(new_args);

                // return our created value, that has been initialized
                return new_val;
            }
        }

    } else if (func->type == ks_T_pfunc) {
        // TODO: implement optimized versions for pfunc<kfunc>
        ks_pfunc pfc = (ks_pfunc)func;
        int new_n_args = n_args + pfc->n_fillin;
        if (new_n_args <= 8) {
            // we can fit in a small buffer
            kso imm_args[8];
            // current argument ptr, normal args ptr, and argument in pfunc ptr
            int ia_p, ar_p = 0, pf_p = 0;
            for (ia_p = 0; ia_p < 8 && pf_p < pfc->n_fillin; ia_p++) {
                if (pfc->fillin_args[pf_p].idx == ia_p) {
                    // fill it in
                    imm_args[ia_p] = pfc->fillin_args[pf_p].val;
                    pf_p++;
                } else {
                    // just copy from other arguments
                    imm_args[ia_p] = args[ar_p];
                    ar_p++;
                }
            }
            // copy the rest
            if (ia_p < new_n_args) memcpy(&imm_args[ia_p], &args[ar_p], sizeof(kso) * (new_n_args - ia_p));

            //printf("CALL: %d\n", ar_p);
            //ks_info("::%R", args[0]);
            return kso_call(pfc->func, n_args + pfc->n_fillin, &imm_args[0]);
        } else {
            return kse_fmt("WRONG ARGS");
        }

    }
    //ks_info("%p, %p", func->type, ks_T_type);


    // else, cause an error
    return kse_fmt("'%T' type is not callable!", func);
}

// return the globals
ks_dict ks_get_globals() {
    return VM.globals;
}

// internal method to initialize it
void ks_init__EXEC() {

    VM.globals = ks_dict_new_empty();
    
}


