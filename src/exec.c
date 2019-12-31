/* exec.c - the main file implementing the bytecode interpreter's execution loop */

#include "ks.h"


// comment this out to disable execution tracing
#define DO_EXEC_TRACE


#ifdef DO_EXEC_TRACE
// if execution tracing is enabled, debug out the arguments
#define _exec_trace(...) { ks_debug("E!: " __VA_ARGS__); }
#else
// else, do nothing
#define _exec_trace(...) { }
#endif


// internal execution routine, this should only really be used by internal functions
void ks_vm_exec(ks_vm vm, ks_code code) {

    /* helpers */

    #define DECREF_N(_objp, _n) { int _i, _stop = (_n); for (_i = 0; _i < _stop; ++_i) { KSO_DECREF((_objp)[_i]); } }

    /* instructions/computed goto */

    // macro to help initialize the instruction label by instruction name
    #define INIT_INST_LABEL(_inst_name) [_inst_name] = &&lbl_##_inst_name,

    // declare the actual label where the code appears
    #define INST_LABEL(_inst_name) lbl_##_inst_name: ;

    // go to the next instruction, assuming `pc` is pointed at a valid bytecode
    #define NEXT_INST() goto *inst_labels[*pc];

    // array of labels for computed goto. This will essentially allow us to just jump directly
    // to the correct position without 2 jumps per loop (so it saves jumping/branching by a factor of 2)
    static void* inst_labels[] = {

        INIT_INST_LABEL(KSBC_NOOP)
        INIT_INST_LABEL(KSBC_CONST)
        INIT_INST_LABEL(KSBC_POPU)
        INIT_INST_LABEL(KSBC_LOAD)
        INIT_INST_LABEL(KSBC_STORE)
        INIT_INST_LABEL(KSBC_CALL)

        INIT_INST_LABEL(KSBC_ADD)
        INIT_INST_LABEL(KSBC_SUB)
        INIT_INST_LABEL(KSBC_MUL)
        INIT_INST_LABEL(KSBC_DIV)

    };

    // call like DECODE(ksbc_), or DECODE(ksbc_i32) to read and pass an instruction of that
    //   kind at the program counter, incrementing the program counter
    #define DECODE(_inst_type) { inst = (ksbc)(*(_inst_type*)pc); pc += sizeof(_inst_type); }

    // variable to hold the currently executing instruction's data
    ksbc inst;


    /* values/temporary variables */

    // gets a constant from the current scope's constant pool
    #define GET_CONST(_idx) ((CUR_SCOPE).v_const->items[(_idx)])

    // the looked up value from the constants table (and a string specific one)
    kso v_c = NULL;
    ks_str v_str = NULL;

    // the value, if it is being popped from the top of the stack, and one that's view only,
    //   and one used as the function
    kso popped = NULL, top = NULL, func = NULL;

    // the value of the thing that was being searched (for example through a load command)
    kso found = NULL;

    // the value of the new object created
    kso new_obj = NULL;

    // a pointer to arguments (like for example, a section of the stack)
    kso* args_p = NULL;

    // immediate argument holders, for things like operators and/or small function calls
    kso imm_args[8];

    /* frame/scope pushing */

    // macro for the current scope/frame we are executing on
    #define CUR_SCOPE (vm->scopes[vm->n_scopes - 1])

    // now, record where we started on the VM's scopes
    int start_scope = vm->n_scopes++;
    vm->scopes = ks_realloc(vm->scopes, sizeof(*vm->scopes) * vm->n_scopes);
    CUR_SCOPE.pc = code->bc;
    CUR_SCOPE.v_const = code->v_const;


    /* execution time */

    // program counter, i.e. address of currently executing code
    uint8_t* pc = CUR_SCOPE.pc;

    // now, start executing at the first instruction
    NEXT_INST();

    /* here is where the actual instruction labels are held */
    {


        INST_LABEL(KSBC_NOOP)
            DECODE(ksbc_);
            _exec_trace("noop");

            return ;
            NEXT_INST();

        INST_LABEL(KSBC_CONST)
            DECODE(ksbc_i32);
            v_c = GET_CONST(inst.i32.i32);
            #ifdef DO_EXEC_TRACE
            // do type generic logging
            if (v_c->type == ks_T_int) {
                _exec_trace("const %ld # [%d]", ((ks_int)v_c)->v_int, inst.i32.i32);
            } else if (v_c->type == ks_T_str) {
                _exec_trace("const '%s' # [%d]", ((ks_str)v_c)->chr, inst.i32.i32);
            } else {
                _exec_trace("const <'%s' obj @ %p> # [%d]", v_c->type->name->chr, v_c, inst.i32.i32);
            }
            #endif

            ks_list_push(vm->stk, v_c);

            NEXT_INST();

        INST_LABEL(KSBC_POPU)
            DECODE(ksbc_);
            _exec_trace("popu");

            ks_list_popu(vm->stk);

            NEXT_INST();

        INST_LABEL(KSBC_LOAD)
            DECODE(ksbc_i32);
            v_str = (ks_str)GET_CONST(inst.i32.i32);
            #ifdef DO_EXEC_TRACE
            // do type generic logging
            if (v_str->type == ks_T_str) {
                _exec_trace("load '%s' # [%d]", ((ks_str)v_str)->chr, inst.i32.i32);
            } else {
                _exec_trace("load <'%s' obj @ %p> # [%d]", v_str->type->name->chr, v_str, inst.i32.i32);
            }
            #endif

            found = ks_dict_get(vm->globals, (kso)v_str, v_str->v_hash);
            if (found == NULL) {
                // ERROR
                ks_error("UNKNOWN");
            }


            // otherwise, it was found, so pop it on the stack
            ks_list_push(vm->stk, found);

            NEXT_INST();

        INST_LABEL(KSBC_STORE)
            DECODE(ksbc_i32);
            v_str = (ks_str)GET_CONST(inst.i32.i32);
            #ifdef DO_EXEC_TRACE
            // do type generic logging
            if (v_str->type == ks_T_str) {
                _exec_trace("store '%s' # [%d]", v_str->chr, inst.i32.i32);
            } else {
                _exec_trace("store <'%s' obj @ %p> # [%d]", v_str->type->name->chr, v_str, inst.i32.i32);
            }
            #endif

            top = vm->stk->items[vm->stk->len - 1];

            ks_dict_set(vm->globals, (kso)v_str, v_str->v_hash, top);

            NEXT_INST();


        INST_LABEL(KSBC_CALL)
            DECODE(ksbc_i32);
            _exec_trace("call %d", inst.i32.i32);

            // get a pointer to the arguments
            args_p = &vm->stk->items[vm->stk->len -= inst.i32.i32];

            // grab a function from the bottom
            func = *args_p++;

            new_obj = kso_call(func, inst.i32.i32 - 1, args_p);
            if (new_obj == NULL) {
                ks_error("ERROR DURING FUNC CALL");
            }

            // otherwise, 
            ks_list_push(vm->stk, new_obj);
            KSO_DECREF(func);
            DECREF_N(args_p, inst.i32.i32 - 1);

            NEXT_INST();

        // binary operator template
        #define T_BOP_LABEL(_BOP, _opstr, _opcfunc) INST_LABEL(_BOP) \
            DECODE(ksbc_); \
            _exec_trace("bop " _opstr); \
            imm_args[1] = ks_list_pop(vm->stk); imm_args[0] = ks_list_pop(vm->stk); \
            new_obj = _opcfunc->v_cfunc(2, imm_args); \
            if (new_obj == NULL) goto EXC; \
            ks_list_push(vm->stk, new_obj); \
            NEXT_INST(); 

        T_BOP_LABEL(KSBC_ADD, "+", ks_F_add)
        T_BOP_LABEL(KSBC_SUB, "-", ks_F_sub)
        T_BOP_LABEL(KSBC_MUL, "*", ks_F_mul)
        T_BOP_LABEL(KSBC_DIV, "/", ks_F_div)



        /* exception handling  */
        EXC:
            if (kse_dumpall()) exit(1);
            return;

    }


    // should never get here
}






