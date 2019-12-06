/* exec.c - execution of bytecode

This uses computed goto, essentially jumping directly to addresses

*/

#include "kscript.h"




kso ks_exec(ks_vm* vm, ks_prog* prog, int idx) {
    // program counter
    ks_bc* pc = prog->bc + idx;

    // instruction labels, for computed goto
    static void* inst_labels[] = {
        &&do_noop,
        &&do_bool,
        &&do_int,
        &&do_float,
        &&do_load,
        &&do_store,
        &&do_call,
        &&do_typeof,
        &&do_ret,
        &&do_retnone
    };

    // temporary variables
    ks_bool _bool;
    ks_int _int;
    ks_int _float;
    // string table lookup variables
    int _sidx;
    ks_str _str;
    // number of arguments for a call
    int16_t n_args;

    // top of the stack
    kso top = NULL;

    // for when searching or resolving a symbol
    kso found = NULL;

    // execute next instruction
    #define NEXT() { goto *inst_labels[*pc++]; }

    while (true) {
        do_noop:
            NEXT();

        do_bool:
            // decode bool
            _bool = *(ks_bool*)pc;
            pc += sizeof(_bool);
            ks_list_push(&vm->stk, kso_new_bool(_bool));
            NEXT();

        do_int:
            // decode int
            _int = *(ks_int*)pc;
            pc += sizeof(_int);
            ks_list_push(&vm->stk, kso_new_int(_int));
            NEXT();

        do_float:
            // decode float
            _float = *(ks_float*)pc;
            pc += sizeof(_float);
            ks_list_push(&vm->stk, kso_new_float(_float));
            NEXT();

        do_load:
            // decode sidx
            _sidx = *(int*)pc;
            pc += sizeof(_sidx);
            // fetch string constant
            _str = prog->str_tbl[_sidx];
            // now, look it up
            ks_trace("loading %s", _str._);
            //ks_list_push(&vm->stk, (kso)kso_F_print);

            found = ks_dict_get_str(&vm->dict, _str);
            if (found == NULL) {
                ks_list_push(&vm->stk, kso_new_str_fmt("Unknown value `%s`", _str._));
                goto handle_exception;
            }

            ks_list_push(&vm->stk, found);

            NEXT();

        do_store:
            // decode sidx
            _sidx = *(int*)pc;
            pc += sizeof(_sidx);
            // fetch string constant
            _str = prog->str_tbl[_sidx];
            // get top object
            top = ks_list_pop(&vm->stk);
            // now store top->"_str"
            NEXT();

        do_call:
            // decode number of args
            n_args = *(int16_t*)pc;
            pc += sizeof(n_args);
            // pop off the function
            top = ks_list_pop(&vm->stk);

            if (top->type == kso_T_cfunc) {
                ((kso_cfunc)top)->_cfunc((int)n_args, &(vm->stk.items[vm->stk.len - n_args]));
                vm->stk.len -= n_args;
                if (ks_err_N() > 0) {
                    ks_list_push(&vm->stk, ks_err_pop());
                    goto handle_exception;
                }
            } else {
                ks_list_push(&vm->stk, kso_new_str_fmt("Invalid type to call, tried calling on type `%s`", top->type->name._));
                //ks_error("Calling something other than 'cfunc'");
                goto handle_exception;
                return NULL;
            }
            ks_trace("calling %p", top);

            NEXT();

        do_typeof:
            // pop it off
            top = ks_list_pop(&vm->stk);
            ks_list_push(&vm->stk, (kso)top->type);
            NEXT();

        do_ret:
            // pop off return value
            top = ks_list_pop(&vm->stk);
            return top;

        do_retnone:
        
            return NULL;




        handle_exception:

            ks_error("%s", KSO_CAST(kso_str, ks_list_pop(&vm->stk))->_str._);

            return NULL;



    }


}





