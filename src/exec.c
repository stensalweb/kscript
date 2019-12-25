/* exec.c - execution of bytecode

This uses computed goto, essentially jumping directly to addresses

*/
#include "kscript.h"

//#define NOTRACE

// enable trace
#ifndef NOTRACE
#define etrace(...) ks_trace("EXEC: " __VA_ARGS__)
#else
#define etrace(...)
#endif

// just runs the virtual machine,
// stops when a return statement is issued at the level it started at
void _kso_vm_run(kso_vm vm) {
    // program counter
    //ks_bc* pc = code->bc;

    // instruction labels, for computed goto
    static void* inst_labels[] = {
        &&do_noop, // 0
        &&do_discard,

        &&do_const,

        &&do_load,
        &&do_store,

        &&do_call,
        &&do_get,
        &&do_set,

        &&do_add,
        &&do_sub,
        &&do_mul,
        &&do_div,
        &&do_mod,
        &&do_pow,
        &&do_lt,
        &&do_gt,
        &&do_eq,
        
        &&do_jmp,
        &&do_jmpt,
        &&do_jmpf,

        &&do_ret,
        &&do_retnone
    };

    // record where we started executing
    int call_stk_n__start = vm->call_stk_n;

    // where did we start on the stack?
    //int start_call_stk_n = vm->call_stk_n + 1;

    // pushes an item onto the call stack
    //#define PUSH_CALL_STK() { vm->call_stk_n++; }
    //#define POP_CALL_STK() { vm->call_stk_n--; }

    // creates a new call stack item (i.e. eval frame)
    #define PUSH_CALL_STK() { vm->call_stk_n++; vm->call_stk[vm->call_stk_n - 1].locals = KS_DICT_EMPTY; }
    #define POP_CALL_STK() { ks_dict_free(&vm->call_stk[vm->call_stk_n - 1].locals); vm->call_stk_n--; }

    // program counter, i.e. address of executing piece of code
    #define PC (vm->call_stk[vm->call_stk_n - 1].pc)

    // local variables as a dictionary
    #define local_vars (vm->call_stk[vm->call_stk_n - 1].locals)

    kso_str name;
    kso found, top, val, func;
    kso* args;

    int n_args, n_items;

    // the union of all instructions, for decoding
    ks_BC inst;

    // just passes without decoding
    #define PASS(_type) { PC += sizeof(struct _type); }
    // decode and pass a type
    #define DECODE(_type) { *((struct _type*)&inst) = (*(struct _type*)PC); PASS(_type) }

    // returns a constant from the constant table
    #define GET_CONST(_idx) (vm->call_stk[vm->call_stk_n - 1].v_const->v_list.items[(_idx)])

    // execute next instruction
    #define NEXT() { goto *inst_labels[*PC]; }

    // macro to increment references for a list
    #define INCREF_N(_list, _n) { int _i; for (_i = 0; _i < _n; ++_i) { KSO_INCREF((_list[_i])); } }
    // macro to decrement references for a list
    #define DECREF_N(_list, _n) { int _i; for (_i = 0; _i < _n; ++_i) { KSO_DECREF((_list[_i])); } }


    // start evaluating
    NEXT();

    while (true) {
        do_noop:
            PASS(ks_bc);
            etrace("noop");
            NEXT();

        do_discard:
            PASS(ks_bc);
            etrace("discard");
            ks_list_popu(&vm->stk);
            NEXT();

        do_const:
            // decode string
            DECODE(ks_bc_const)
            val = GET_CONST(inst.v_const.v_idx);
            #ifndef NOTRACE
            if (val->type == kso_T_int) {
                etrace("const %lld", ((kso_int)val)->v_int);
            } else if (val->type == kso_T_str) {
                etrace("const \"%s\"", ((kso_str)val)->v_str._);
            } else {
                etrace("const<%s> ... @ %p", val->type->name._, val);
            }

            #endif
            //etrace("const \"%s\" [%d]", _ostr->v_str._, i_str.val_idx);
            ks_list_push(&vm->stk, val);
            NEXT();

        /* loading/store */

        do_load:
            // decode
            DECODE(ks_bc_nameop);
            // fetch string constant
            name = (kso_str)GET_CONST(inst.nameop.name_idx);
            etrace("load \"%s\" [%d]", name->v_str._, inst.nameop.name_idx);

            // now, look it up
            //found = ks_dict_get(&vm->globals, (kso)_ostr, _ostr->v_hash);
            //found = ks_dict_get(&local_vars, (kso)_ostr, _ostr->v_hash);
            found = NULL;

            // start at the most local variable scope
            int lvl = vm->call_stk_n - 1;
            while (lvl >= 0 && found == NULL) {
                found = ks_dict_get(&vm->call_stk[lvl].locals, (kso)name, name->v_hash);
                // go down a level in scope
                lvl --;
            }

            // if all that failed, do a global lookup
            if (found == NULL) found = ks_dict_get(&vm->globals, (kso)name, name->v_hash);

            // we reached the end without finding anything
            if (found == NULL) {
                ks_err_add_str_fmt("Unknown value `%s`", name->v_str._);
                goto handle_exception;
            }

            ks_list_push(&vm->stk, found);
            NEXT();

        do_store:
            // decode
            DECODE(ks_bc_nameop);

            // fetch string constant
            name = (kso_str)GET_CONST(inst.nameop.name_idx);
            etrace("store \"%s\" [%d]", name->v_str._, inst.nameop.name_idx);

            if (vm->stk.len < 1) {
                ks_err_add_str_fmt("Not enough values on the stack");
                goto handle_exception;
            }

            // get top object
            top = ks_list_pop(&vm->stk);

            // now store top->"_str"
            //ks_dict_set(&vm->globals, (kso)_ostr, _ostr->v_hash, top);
            ks_dict_set(&local_vars, (kso)name, name->v_hash, top);

            KSO_DECREF(top);

            NEXT();


        do_call:

            // decode number of args
            DECODE(ks_bc_call);
            n_args = (int)inst.call.n_args;
            // include the function
            n_items = n_args + 1;
            etrace("call %d", n_args);

            vm->stk.len -= n_items;
            // get the function, under the arguments
            func = vm->stk.items[vm->stk.len];

            // the arguments start after the function
            args = &(vm->stk.items[vm->stk.len + 1]);

            if (func->type == kso_T_cfunc) {
                // evaluate the C function
                val = ((kso_cfunc)func)->v_cfunc(vm, n_args, args);
                if (val == NULL) {
                    goto handle_exception;
                }
                KSO_INCREF(val);
                DECREF_N(args, n_args);
                
                ks_list_push(&vm->stk, val);
            } else if (func->type == kso_T_kfunc) {

                if (n_args != ((kso_kfunc)top)->params->v_list.len) {
                    ks_err_add_str_fmt("Tried calling function that takes %d args with %d args", ((kso_kfunc)top)->params->v_list.len, n_args);
                    goto handle_exception;
                }

                // expand call stack
                PUSH_CALL_STK();

                // set the arguments as a local variable
                int i;
                for (i = 0; i < n_args; ++i) {
                    kso_str name = (kso_str)(((kso_kfunc)top)->params->v_list.items[i]);
                    ks_dict_set(&local_vars, (kso)name, name->v_hash, args[i]);
                }
                // now, the arguments should be in in the global dictionary TODO: make scopes
                DECREF_N(args, n_args);

                // set cur prog/pc
                PC = ((kso_kfunc)func)->code->bc;

            } else {
                ks_err_add_str_fmt("Invalid type to call, tried calling on type `%s`", top->type->name._);
                //ks_error("Calling something other than 'cfunc'");
                goto handle_exception;
            }

            KSO_DECREF(top);

            NEXT();

        do_get:
            // decode get
            DECODE(ks_bc_call);
            n_args = inst.call.n_args;
            etrace("get %d", n_args);
            args = &vm->stk.items[vm->stk.len -= n_args];

            // run the global get function
            //new_obj = kso_F_get->_cfunc(vm, i_get.n_args, args);
            /*if (new_obj == NULL) goto handle_exception;

            KSO_INCREF(new_obj);
            DECREF_N(args, i_get.n_args);
            ks_list_push(&vm->stk, new_obj);
*/
            NEXT();


        do_set:
            // decode get
            DECODE(ks_bc_call);
            n_args = inst.call.n_args;
            etrace("set %d", n_args);
            args = &vm->stk.items[vm->stk.len -= n_args];

            NEXT();



        /* binary operators are just function calls of length 2 */

        // binary operator macro
        #define DO_BOP(_name, _str) \
        do_##_name: \
            PASS(ks_bc); \
            etrace("op " _str); \
            //args = &(vm->stk.items[vm->stk.len -= 2]); \
            //new_obj = kso_F_##_name->_cfunc(vm, 2, args); \
            if (new_obj == NULL) goto handle_exception; \
            KSO_INCREF(new_obj); \
            DECREF_N(args, 2); \
            ks_list_push(&vm->stk, new_obj); \
            KSO_DECREF(new_obj); \
            NEXT();

        DO_BOP(add, "+")
        DO_BOP(sub, "-")
        DO_BOP(mul, "*")
        DO_BOP(div, "/")
        DO_BOP(mod, "%")
        DO_BOP(pow, "^")

        DO_BOP(lt, "<")
        DO_BOP(gt, ">")
        DO_BOP(eq, "==")



        do_jmp:
            DECODE(ks_bc_jmp); 
            etrace("jmp %+d", inst.jmp.relamt);

            PC += inst.jmp.relamt;

            NEXT();
        do_jmpt:
            DECODE(ks_bc_jmp); 
            etrace("jmpt %+d", inst.jmp.relamt);

            top = ks_list_pop(&vm->stk);
            if (top->type == kso_T_bool && KSO_CAST(kso_bool, top)->v_bool) {
                PC += inst.jmp.relamt;
            }

            KSO_DECREF(top);

            NEXT();

        do_jmpf:
            DECODE(ks_bc_jmp); 
            etrace("jmpf %+d", inst.jmp.relamt);

            top = ks_list_pop(&vm->stk);
            if (top->type == kso_T_bool && !KSO_CAST(kso_bool, top)->v_bool) {
                PC += inst.jmp.relamt;
            }

            KSO_DECREF(top);

            NEXT();


        do_ret:
            PASS(ks_bc);
            etrace("ret");

            // take off a frame
            POP_CALL_STK();

            if (vm->call_stk_n < call_stk_n__start) {
                // return, now the VM has the result at the top of the stack
                return;
            } else {
                // just keep going, there are nested layers
            }

            NEXT();

        do_retnone:
            PASS(ks_bc);

            // take off a frame
            POP_CALL_STK();

            if (vm->call_stk_n < call_stk_n__start) {
                ks_list_push(&vm->stk, (kso)kso_V_none);
                // ready to return
                return;
            } else {
                ks_list_push(&vm->stk, (kso)kso_V_none);
            }

            NEXT();

        handle_exception:
            etrace("exception");

            top = ks_err_pop();
            if (top->type == kso_T_str) {
                ks_error("%s", KSO_CAST(kso_str, top)->v_str._);
            } else {
                ks_error("Exception @ %p", top);
            }

            kso_free(top);

            return;
    }
}

void kso_vm_exec(kso_vm vm, kso_code code) {

    // add call frame
    int idx = vm->call_stk_n++;
    vm->call_stk[idx] = KSO_VM_CALL_STK_ITEM_EMPTY;
    vm->call_stk[idx].pc = code->bc;
    vm->call_stk[idx].v_const = code->v_const;

    // run it
    _kso_vm_run(vm);
}


kso kso_vm_call(kso_vm vm, kso func, int n_args, kso* args) {
    if (func->type == kso_T_cfunc) {
        return ((kso_cfunc)func)->v_cfunc(vm, n_args, args);
    } else if (func->type == kso_T_kfunc) {
        kso_kfunc kfunc = (kso_kfunc)func;

        if (kfunc->params->v_list.len != n_args) {
            ks_err_add_str_fmt("Tried to call function requiring %d arguments with %d", kfunc->params->v_list.len, n_args);
            return NULL;
        }

        int idx = vm->call_stk_n++;
        vm->call_stk[idx] = KSO_VM_CALL_STK_ITEM_EMPTY;
        int i;
        for (i = 0; i < n_args; ++i) {
            ks_dict_set(&vm->call_stk[idx].locals, (kso)kfunc->params->v_list.items[i], ((kso_str)kfunc->params->v_list.items[i])->v_hash, args[i]);
        }

        vm->call_stk[idx].pc = kfunc->code->bc;
        vm->call_stk[idx].v_const = kfunc->code->v_const;

        // now, execute
        _kso_vm_run(vm);

        return ks_list_pop(&vm->stk);
    }
}




