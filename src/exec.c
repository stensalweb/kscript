/* exec.c - execution of bytecode

This uses computed goto, essentially jumping directly to addresses

*/

#include "kscript.h"




kso ks_exec(ks_vm* vm, ks_prog* prog, int idx) {
    // program counter
    ks_bc* pc = prog->bc + idx;

    // instruction labels, for computed goto
    static void* inst_labels[] = {
        &&do_noop, // 0
        &&do_load, // 1
        &&do_store,// 2
        &&do_boolt,// 3
        &&do_boolf,// 4
        &&do_int,  // 5
        &&do_float,// 6
        &&do_str,  // 7
        &&do_new_list, // 8
        &&do_call, // 9
        &&do_get,  // 10
        &&do_set,  // 11
        &&do_jmpt,  // 12
        &&do_jmpf,  // 13
        &&do_typeof,// 14
        &&do_ret,  // 15
        &&do_retnone// 16
    };

    /* for decoded */
    struct ks_bc_load i_load;
    struct ks_bc_store i_store;
    struct ks_bc_call i_call;
    struct ks_bc_int i_int;
    struct ks_bc_float i_float;
    struct ks_bc_str i_str;

    // string table lookup variables
    ks_str _str;

    // top of the stack
    kso top = NULL;

    // for when searching or resolving a symbol
    kso found = NULL;



    // execute next instruction
    #define NEXT() { ks_trace("inst: %d", (int)*pc); goto *inst_labels[*pc]; }

    #define DECODE(_type) (*(struct _type*)(pc))
    #define PASS(_type) (pc += sizeof(struct _type));

    while (true) {
        do_noop:
            DECODE(ks_bc_noop);
            NEXT();

        do_load:
            // decode
            i_load = DECODE(ks_bc_load);
            PASS(ks_bc_load);
            // fetch string constant
            _str = prog->str_tbl[i_load.name_idx];
            // now, look it up

            found = ks_dict_get_str(&vm->dict, _str);
            if (found == NULL) {
                ks_list_push(&vm->stk, kso_new_str_fmt("Unknown value `%s`", _str._));
                goto handle_exception;
            }

            ks_list_push(&vm->stk, found);

            NEXT();

        do_store:
            // decode
            i_store = DECODE(ks_bc_store);
            PASS(ks_bc_store);
            // fetch string constant
            _str = prog->str_tbl[i_load.name_idx];
            // get top object
            top = ks_list_pop(&vm->stk);
            // now store top->"_str"
            NEXT();


        do_boolt:
            PASS(ks_bc);
            ks_list_push(&vm->stk, kso_new_bool(true));
            NEXT();
        do_boolf:
            PASS(ks_bc);
            ks_list_push(&vm->stk, kso_new_bool(false));
            NEXT();

        do_int:
            // decode int
            i_int = DECODE(ks_bc_int);
            PASS(ks_bc_int);
            ks_list_push(&vm->stk, kso_new_int(i_int.val));
            NEXT();

        do_float:
            // decode float
            i_float = DECODE(ks_bc_float);
            PASS(ks_bc_float);
            ks_list_push(&vm->stk, kso_new_int(i_float.val));
            NEXT();

        do_str:
            // decode string
            i_str = DECODE(ks_bc_str);
            PASS(ks_bc_str);
            ks_list_push(&vm->stk, kso_new_str(prog->str_tbl[i_str.val_idx]));
            NEXT();

        do_new_list:
            NEXT();

        do_call:
            // decode number of args
            i_call = DECODE(ks_bc_call);
            PASS(ks_bc_call);
            // pop off the function
            top = ks_list_pop(&vm->stk);

            if (top->type == kso_T_cfunc) {
                ks_trace("calling %p with %d", top, i_call.n_args);

                ((kso_cfunc)top)->_cfunc(i_call.n_args, &(vm->stk.items[vm->stk.len - i_call.n_args]));
                vm->stk.len -= i_call.n_args;
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

            NEXT();

        do_get:
            NEXT();

        do_set:
            NEXT();

        do_jmpt:
            NEXT();

        do_jmpf:
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





