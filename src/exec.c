/* exec.c - execution of bytecode

This uses computed goto, essentially jumping directly to addresses

*/

#include "kscript.h"

// disable trace
//#define etrace(...) {}
// enable trace
#define etrace(...) ks_trace("EXEC: " __VA_ARGS__)


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

        &&do_add,
        &&do_sub,
        &&do_mul,
        &&do_div,
        &&do_lt,

        &&do_call, // 9
        &&do_get,  // 10
        &&do_set,  // 11
        &&do_jmp,  // 12
        &&do_jmpt,  // 13
        &&do_jmpf,  // 14
        &&do_typeof,// 15
        &&do_ret,  // 16
        &&do_retnone,// 17
        &&do_discard
    };

    /* for decoded */
    struct ks_bc_load i_load;
    struct ks_bc_store i_store;
    struct ks_bc_call i_call;
    struct ks_bc_get i_get;
    struct ks_bc_set i_set;
    struct ks_bc_int i_int;
    struct ks_bc_new_list i_new_list;
    struct ks_bc_float i_float;
    struct ks_bc_str i_str;
    struct ks_bc_jmp i_jmp;

    // string table lookup variables
    ks_str _str;

    // top of the stack
    kso top = NULL;

    // for when searching or resolving a symbol
    kso found = NULL;

    // for constructing object
    kso new_obj = NULL;

    // list of arguments
    kso* args = NULL;


    // execute next instruction
    #define NEXT() { goto *inst_labels[*pc]; }

    #define DECODE(_type) (*(struct _type*)(pc))
    #define PASS(_type) (pc += sizeof(struct _type));

    #define INCREF_N(_list, _n) { int _i; for (_i = 0; _i < _n; ++_i) { KSO_INCREF((_list[_i])); } }
    #define DECREF_N(_list, _n) { int _i; for (_i = 0; _i < _n; ++_i) { KSO_DECREF((_list[_i])); } }

    NEXT();

    while (true) {
        do_noop:
            DECODE(ks_bc_noop);
            PASS(ks_bc_noop);
            etrace("noop");
            NEXT();

        do_load:
            // decode
            i_load = DECODE(ks_bc_load);
            PASS(ks_bc_load);
            // fetch string constant
            _str = prog->str_tbl[i_load.name_idx];
            etrace("load \"%s\" [%d]", _str._, i_load.name_idx);

            // now, look it up
            found = ks_dict_get_str(&vm->dict, _str);
            if (found == NULL) {
                ks_list_push(&vm->stk, kso_new_str_fmt("Unknown value `%s`", _str._));
                goto handle_exception;
            }

            ks_list_push(&vm->stk, found);
            KSO_INCREF(found);

            NEXT();

        do_store:
            // decode
            i_store = DECODE(ks_bc_store);
            PASS(ks_bc_store);
            // fetch string constant
            _str = prog->str_tbl[i_store.name_idx];
            etrace("store \"%s\" [%d]", _str._, i_store.name_idx);
            
            if (vm->stk.len < 1) {
                ks_list_push(&vm->stk, kso_new_str_fmt("Unknown value `%s`", _str._));
                goto handle_exception;
            }

            // get top object
            top = ks_list_pop(&vm->stk);

            // now store top->"_str"
            ks_dict_set_str(&vm->dict, _str, top);

            KSO_DECREF(top);

            NEXT();

        do_boolt:
            PASS(ks_bc);
            ks_list_push(&vm->stk, (kso)kso_V_true);
            etrace("const true");

            NEXT();
        do_boolf:
            PASS(ks_bc);
            ks_list_push(&vm->stk, (kso)kso_V_false);
            etrace("const false");

            NEXT();

        do_int:
            // decode int
            i_int = DECODE(ks_bc_int);
            PASS(ks_bc_int);
            etrace("const %lld", i_int.val);

            new_obj = kso_new_int(i_int.val);
            ks_list_push(&vm->stk, new_obj);

            KSO_INCREF(new_obj);
            NEXT();

        do_float:
            // decode float
            i_float = DECODE(ks_bc_float);
            PASS(ks_bc_float);
            etrace("const %lf", i_float.val);

            new_obj = kso_new_float(i_float.val);
            ks_list_push(&vm->stk, new_obj);

            KSO_INCREF(new_obj);
            NEXT();

        do_str:
            // decode string
            i_str = DECODE(ks_bc_str);
            PASS(ks_bc_str);
            _str = prog->str_tbl[i_str.val_idx];
            etrace("const \"%s\" [%d]", _str._, i_str.val_idx);

            new_obj = kso_new_str(_str);
            ks_list_push(&vm->stk, new_obj);

            KSO_INCREF(new_obj);
            NEXT();

        do_new_list:
            // decode new list
            i_new_list = DECODE(ks_bc_new_list);
            PASS(ks_bc_new_list);
            etrace("new_list %d", (int)i_new_list.n_items);

            if (i_new_list.n_items > vm->stk.len) {
                ks_list_push(&vm->stk, kso_new_str_fmt("Not enough items on stack for `new_list` (needed %d, but only had %d)", i_new_list.n_items, vm->stk.len));
                //ks_error("Calling something other than 'cfunc'");
                goto handle_exception;
            }

            args = &vm->stk.items[vm->stk.len -= i_new_list.n_items];
            new_obj = kso_new_list(i_new_list.n_items, args);

            // since all args are transferred from stack to the list, 
            // their reference counts are decreased, then increased, so no change
            //INCREF_N(args, i_new_list.n_items);
            KSO_INCREF(new_obj);

            ks_list_push(&vm->stk, new_obj);

            NEXT();

        /* binary operators are just function calls of length 2 */

        do_add:
            PASS(ks_bc);
            etrace("op +");
            args = &(vm->stk.items[vm->stk.len -= 2]);

            new_obj = kso_F_add->_cfunc(2, args);
            if (new_obj == NULL) goto handle_exception;

            KSO_INCREF(new_obj);
            DECREF_N(args, 2);

            ks_list_push(&vm->stk, new_obj);
            NEXT();
        do_sub:
            PASS(ks_bc);
            etrace("op -");
            args = &(vm->stk.items[vm->stk.len -= 2]);
            
            new_obj = kso_F_sub->_cfunc(2, args);
            if (new_obj == NULL) goto handle_exception;

            KSO_INCREF(new_obj);
            DECREF_N(args, 2);

            ks_list_push(&vm->stk, new_obj);
            NEXT();
        do_mul:
            PASS(ks_bc);
            etrace("op *");
            args = &(vm->stk.items[vm->stk.len -= 2]);

            new_obj = kso_F_mul->_cfunc(2, args);
            if (new_obj == NULL) goto handle_exception;

            KSO_INCREF(new_obj);
            DECREF_N(args, 2);

            ks_list_push(&vm->stk, new_obj);
            NEXT();
        do_div:
            PASS(ks_bc);
            etrace("op /");
            args = &(vm->stk.items[vm->stk.len -= 2]);
            new_obj = kso_F_div->_cfunc(2, args);
            DECREF_N(args, 2);
            if (new_obj == NULL) goto handle_exception;
            ks_list_push(&vm->stk, new_obj);
            NEXT();
        do_lt:
            PASS(ks_bc);
            etrace("op <");
            args = &(vm->stk.items[vm->stk.len -= 2]);

            new_obj = kso_F_lt->_cfunc(2, args);
            if (new_obj == NULL) goto handle_exception;

            KSO_INCREF(new_obj);
            DECREF_N(args, 2);

            ks_list_push(&vm->stk, new_obj);

            NEXT();
        do_call:
            // decode number of args
            i_call = DECODE(ks_bc_call);
            PASS(ks_bc_call);
            etrace("call %d", (int)i_call.n_args);
            // pop off the function
            top = ks_list_pop(&vm->stk);

            args = &(vm->stk.items[vm->stk.len -= i_call.n_args]);

            if (top->type == kso_T_cfunc) {
                //ks_trace("calling %p with %d", top, i_call.n_args);
                new_obj = ((kso_cfunc)top)->_cfunc(i_call.n_args, args);
                if (new_obj == NULL) {
                    if (ks_err_N() > 0) ks_list_push(&vm->stk, ks_err_pop());
                    else ks_list_push(&vm->stk, kso_new_none());
                    goto handle_exception;
                }
                KSO_INCREF(new_obj);
                DECREF_N(args, i_call.n_args);
                
                ks_list_push(&vm->stk, new_obj);
            } else {
                ks_list_push(&vm->stk, kso_new_str_fmt("Invalid type to call, tried calling on type `%s`", top->type->name._));
                //ks_error("Calling something other than 'cfunc'");
                goto handle_exception;
                return NULL;
            }

            KSO_DECREF(top);

            NEXT();

        do_get:
            // decode get
            i_get = DECODE(ks_bc_get);
            PASS(ks_bc_get);
            etrace("get %d", (int)i_get.n_args);
            args = &vm->stk.items[vm->stk.len -= i_get.n_args];

            // run the global get function
            new_obj = kso_F_get->_cfunc(i_get.n_args, args);
            if (ks_err_N() > 0) {
                ks_list_push(&vm->stk, ks_err_pop());
                goto handle_exception;
            }

            KSO_INCREF(new_obj);
            DECREF_N(args, i_get.n_args);

            ks_list_push(&vm->stk, new_obj);

            NEXT();

        do_set:
            // decode set
            i_set = DECODE(ks_bc_set);
            PASS(ks_bc_set);
            etrace("set %d", (int)i_set.n_args);
            args = &(vm->stk.items[vm->stk.len -= i_set.n_args]);

            // run the global set function
            new_obj = kso_F_set->_cfunc(i_set.n_args, args);
            if (ks_err_N() > 0) {
                ks_list_push(&vm->stk, ks_err_pop());
                goto handle_exception;
            }

            // we shouldn't add the result to the stack
            //KSO_INCREF(new_obj);
            DECREF_N(args, i_set.n_args);

            //ks_list_push(&vm->stk, new_obj);

            NEXT();
        do_jmp:
            i_jmp = DECODE(ks_bc_jmp); 
            PASS(ks_bc_jmp); 
            etrace("jmp %+d", (int)i_jmp.relamt);

            pc += i_jmp.relamt;

            NEXT();
        do_jmpt:
            i_jmp = DECODE(ks_bc_jmp); 
            PASS(ks_bc_jmp); 
            etrace("jmpt %+d", (int)i_jmp.relamt);

            top = ks_list_pop(&vm->stk);
            if (top->type == kso_T_bool && KSO_CAST(kso_bool, top)->_bool) {
                pc += i_jmp.relamt;
            }

            KSO_DECREF(top);

            NEXT();

        do_jmpf:
            i_jmp = DECODE(ks_bc_jmp); 
            PASS(ks_bc_jmp); 
            etrace("jmpf %+d", (int)i_jmp.relamt);

            top = ks_list_pop(&vm->stk);
            if (top->type == kso_T_bool && !KSO_CAST(kso_bool, top)->_bool) {
                pc += i_jmp.relamt;
            }

            KSO_DECREF(top);

            NEXT();

        do_typeof:
            PASS(ks_bc);

            etrace("typeof");

            // pop it off
            top = ks_list_pop(&vm->stk);
            ks_list_push(&vm->stk, (kso)top->type);
            NEXT();

        do_ret:
            PASS(ks_bc);
            etrace("ret");

            // pop off return value
            top = ks_list_pop(&vm->stk);
            return top;

        do_retnone:
            PASS(ks_bc);
            etrace("retnone");
        
            return NULL;

        do_discard:
            PASS(ks_bc);
            etrace("discard");
        
            top = ks_list_pop(&vm->stk);

            KSO_DECREF(top);

            NEXT();

        handle_exception:
            etrace("exception");

            ks_error("%s", KSO_CAST(kso_str, ks_list_pop(&vm->stk))->_str._);

            return NULL;



    }


}





