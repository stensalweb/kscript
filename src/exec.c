/* exec.c - execution of bytecode

This uses computed goto, essentially jumping directly to addresses

*/
#include "kscript.h"

#define NOTRACE

// enable trace
#ifndef NOTRACE
#define etrace(...) ks_debug("EXEC: " __VA_ARGS__)
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

        &&do_list,
        &&do_call,

        &&do_getattr,
        &&do_setattr,

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
        &&do_retnone,

        &&do_scope,
        &&do_new_type
    };

    // record where we started executing, so we know when to stop
    int call_stk_n__start = vm->call_stk_n;

    // creates a new call stack item (i.e. eval frame)
    #define PUSH_CALL_STK() { vm->call_stk_n++; vm->call_stk[vm->call_stk_n - 1].locals = KS_DICT_EMPTY; }
    #define POP_CALL_STK() { ks_dict_free(&vm->call_stk[vm->call_stk_n - 1].locals); vm->call_stk_n--; }

    // program counter, i.e. address of executing piece of code
    #define PC (vm->call_stk[vm->call_stk_n - 1].pc)

    // local variables as a dictionary
    #define local_vars (vm->call_stk[vm->call_stk_n - 1].locals)

    // variables to define once, and use in multiple dispatches
    // so that no allocas happen while executing
    kso_str name;
    kso found, top, val, func;

    // a pointer to arguments, useful when no reallocation would be neccessary
    kso* args_p = NULL;
    // operator arguments, for things that take a small and fixed number of args
    kso op_args[8];

    // array of arguments
    kso* args = NULL;

    // number the args has been allocated for
    int __args_len_max = 0;

    #define ENSURE_ARGS_HAS(_len) if (_len > __args_len_max) { __args_len_max = _len; args = ks_realloc(args, _len * sizeof(*args)); }
    #define ARGS_COPY(_n, _from) memcpy(args, _from, sizeof(*args) * _n)

    // common variable for number of args or items in an operation
    int n_args;

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
            top = ks_list_pop(&vm->stk);
            KSO_DECREF(top);
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

            // get top object, just peeking, because it should stay on the stack
            top = vm->stk.items[vm->stk.len - 1];

            // now store top->"_str"
            //ks_dict_set(&vm->globals, (kso)_ostr, _ostr->v_hash, top);
            ks_dict_set(&local_vars, (kso)name, name->v_hash, top);

            //KSO_DECREF(top);

            NEXT();

        do_list:

            // decode number of args/items
            DECODE(ks_bc_call);
            n_args = (int)inst.call.n_args;
            etrace("list %d", n_args);

            // pluck of arguments
            args_p = &vm->stk.items[vm->stk.len -= n_args];

            // add a new list
            ks_list_push(&vm->stk, (kso)kso_list_new(n_args, args_p));

            //DECREF_N(args_p, n_args);

            NEXT();

        do_call:

            // decode number of args
            DECODE(ks_bc_call);
            n_args = (int)inst.call.n_args;
            // include the function
            etrace("call %d", n_args);

            // arguments (including function)
            args_p = &(vm->stk.items[vm->stk.len -= n_args]);
            // just take out the function
            func = *args_p++;
            n_args--;

            kso_obj new_obj = NULL;

            if (func->type == kso_T_type) {
                if (((kso_type)func)->f_call != NULL) {
                    // just do the call
                    func = ((kso_type)func)->f_call;
                    //   for example by calling a kfunc from the C func
                    ENSURE_ARGS_HAS(n_args);
                    ARGS_COPY(n_args, args_p);
                } else if (((kso_type)func)->f_init != NULL) {

                    kso_type ftype = (kso_type)func;

                    // we will construct it
                    new_obj = kso_obj_new();
                    new_obj->type = ftype;
                    KSO_INCREF(new_obj);
                    
                    // copy arguments
                    ++n_args;
                    ENSURE_ARGS_HAS(n_args);
                    args[0] = (kso)new_obj;

                    memcpy(&args[1], args_p, (n_args - 1) * sizeof(*args));
                    func = ((kso_type)func)->f_init;
                } else {
                    ks_err_add_str_fmt("Tried calling type '%s', but did not have .init() or .call()", ((kso_type)func)->name._);
                    goto handle_exception;

                }

            } else {
                // copy arguments, in case the stack gets modified by the C func,
                //   for example by calling a kfunc from the C func
                ENSURE_ARGS_HAS(n_args);
                ARGS_COPY(n_args, args_p);
            }

            if (func == NULL) {
                ks_err_add_str_fmt("Tried calling uncallable object");
                goto handle_exception;

            } else if (func->type == kso_T_cfunc) {

                // evaluate the C function
                val = ((kso_cfunc)func)->v_cfunc(vm, n_args, args);
                if (val == NULL) {
                    goto handle_exception;
                }

                // update reference counts
                ks_list_push(&vm->stk, val);

            } else if (func->type == kso_T_kfunc) {

                // ensure correct size
                if (n_args != ((kso_kfunc)func)->params->v_list.len) {
                    ks_err_add_str_fmt("Tried calling function that takes %d args with %d args", ((kso_kfunc)func)->params->v_list.len, n_args);
                    goto handle_exception;
                }

                // expand call stack
                PUSH_CALL_STK();

                // set the arguments as a local variable in the inner most scope
                int i;
                for (i = 0; i < n_args; ++i) {
                    kso_str name = (kso_str)(((kso_kfunc)func)->params->v_list.items[i]);
                    ks_dict_set(&local_vars, (kso)name, name->v_hash, args[i]);
                    //KSO_DECREF(new_obj);
                }

                // set cur prog/pc
                PC = ((kso_kfunc)func)->code->bc;
                // set the constant pool
                vm->call_stk[vm->call_stk_n - 1].v_const = ((kso_kfunc)func)->code->v_const;

            } else {

                ks_err_add_str_fmt("Invalid type to call, tried calling on type `%s`", func->type->name._);
                //ks_error("Calling something other than 'cfunc'");
                goto handle_exception;
            }

            //KSO_DECREF(func);
            // now, the arguments should be in in the locals dictionary,
            // so decrement them
            DECREF_N(args, n_args);

            //if (new_obj != NULL) KSO_DECREF(new_obj);

            NEXT();


        do_getattr:
            // decode get
            DECODE(ks_bc_nameop);
            name = (kso_str)GET_CONST(inst.nameop.name_idx);
            etrace("getattr \"%s\" [%d]", name->v_str._, inst.nameop.name_idx);

            // set up arguments
            args_p = &vm->stk.items[vm->stk.len -= 1];
            op_args[0] = args_p[0];
            op_args[1] = (kso)name;

            // run the global get function
            val = kso_F_getattr->v_cfunc(vm, 2, op_args);
            if (val == NULL) goto handle_exception;

            ks_list_push(&vm->stk, val);

            KSO_DECREF(op_args[0]);

            NEXT();

        do_setattr:
            // decode set
            DECODE(ks_bc_nameop);
            name = (kso_str)GET_CONST(inst.nameop.name_idx);
            etrace("setattr \"%s\" [%d]", name->v_str._, inst.nameop.name_idx);

            // set up arguments
            args_p = &vm->stk.items[vm->stk.len -= 2];
            op_args[0] = args_p[0];
            op_args[1] = (kso)name;
            op_args[2] = args_p[1];

            // run the global get function
            val = kso_F_setattr->v_cfunc(vm, 3, op_args);
            if (val == NULL) goto handle_exception;

            ks_list_push(&vm->stk, val);
            KSO_DECREF(op_args[0]);
            KSO_DECREF(op_args[2]);

            NEXT();

        do_get:
            // decode get
            DECODE(ks_bc_call);
            n_args = inst.call.n_args;
            etrace("get %d", n_args);

            // get arguments into the array
            args_p = &vm->stk.items[vm->stk.len -= n_args];
            ENSURE_ARGS_HAS(n_args);
            ARGS_COPY(n_args, args_p);

            // run the global get function
            val = kso_F_get->v_cfunc(vm, n_args, args);
            if (val == NULL) goto handle_exception;

            ks_list_push(&vm->stk, val);
            DECREF_N(args, n_args);

            NEXT();

        do_set:
            // decode set
            DECODE(ks_bc_call);
            n_args = inst.call.n_args;
            etrace("set %d", n_args);
            
            // get arguments into the array
            args_p = &vm->stk.items[vm->stk.len -= n_args];
            ENSURE_ARGS_HAS(n_args);
            ARGS_COPY(n_args, args_p);

            // run the global get function
            val = kso_F_set->v_cfunc(vm, n_args, args);
            if (val == NULL) goto handle_exception;

            ks_list_push(&vm->stk, val);
            DECREF_N(args, n_args);

            NEXT();


        /* binary operators are just function calls of length 2 */

        // binary operator macro
        #define DO_BOP(_name, _str) \
        do_##_name: \
            PASS(ks_bc); \
            etrace("bop " _str); \
            args_p = &(vm->stk.items[vm->stk.len -= 2]); \
            op_args[0] = args_p[0]; op_args[1] = args_p[1]; \
            val = ((kso_cfunc)kso_F_##_name)->v_cfunc(vm, 2, op_args); \
            if (val == NULL) goto handle_exception; \
            ks_list_push(&vm->stk, val); \
            DECREF_N(op_args, 2); \
            NEXT(); \
            break;

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
            if (top == (kso)KSO_TRUE) {
                PC += inst.jmp.relamt;
            }

            KSO_DECREF(top);

            NEXT();

        do_jmpf:
            DECODE(ks_bc_jmp); 
            etrace("jmpf %+d", inst.jmp.relamt);

            top = ks_list_pop(&vm->stk);
            if (top != (kso)KSO_TRUE) {
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
                if (args != NULL) ks_free(args);
                return;
            } else {
                // just keep going, there are nested layers
            }

            NEXT();

        do_retnone:
            PASS(ks_bc);
            etrace("retnone");

            // take off a frame
            POP_CALL_STK();

            if (vm->call_stk_n < call_stk_n__start) {
                ks_list_push(&vm->stk, (kso)kso_V_none);
                // ready to return
                if (args != NULL) ks_free(args);
                return;
            } else {
                ks_list_push(&vm->stk, (kso)kso_V_none);
            }

            NEXT();

        do_scope:
            PASS(ks_bc);
            etrace("scope");

            // put on a new scope
            PUSH_CALL_STK();
            vm->call_stk[vm->call_stk_n - 1] = KSO_VM_CALL_STK_ITEM_EMPTY;
            vm->call_stk[vm->call_stk_n - 1].pc = vm->call_stk[vm->call_stk_n - 2].pc;
            vm->call_stk[vm->call_stk_n - 1].v_const = vm->call_stk[vm->call_stk_n - 2].v_const;

            NEXT();


        do_new_type:

            PASS(ks_bc);
            etrace("new_type");

            int type_mem_idx;
            kso_type new_type = kso_type_new();
            new_type->f_init = NULL;
            new_type->f_free = kso_T_obj->f_free;
            new_type->name = KS_STR_CREATE("CUSTOM");
            new_type->f_str = kso_T_obj->f_str;
            new_type->f_setattr = kso_T_obj->f_setattr;
            new_type->f_getattr = kso_T_obj->f_getattr;

            for (type_mem_idx = 0; type_mem_idx < local_vars.n_buckets; ++type_mem_idx) {
                struct ks_dict_entry bucket = local_vars.buckets[type_mem_idx];
                if (bucket.val != NULL) {
                    // we have a used bucket, add it to type
                    if (ks_hash_str(KS_STR_CONST("str")) == bucket.hash) {
                        new_type->f_str = bucket.val;
                        KSO_INCREF(bucket.val);
                    } else if (ks_hash_str(KS_STR_CONST("init")) == bucket.hash) {
                        new_type->f_init = bucket.val;
                        KSO_INCREF(bucket.val);
                    } else if (ks_hash_str(KS_STR_CONST("add")) == bucket.hash) {
                        new_type->f_add = bucket.val;
                        KSO_INCREF(bucket.val);
                    } else if (ks_hash_str(KS_STR_CONST("mul")) == bucket.hash) {
                        new_type->f_mul = bucket.val;
                        KSO_INCREF(bucket.val);
                    } else if (ks_hash_str(KS_STR_CONST("get")) == bucket.hash) {
                        new_type->f_get = bucket.val;
                        KSO_INCREF(bucket.val);
                    } else if (ks_hash_str(KS_STR_CONST("set")) == bucket.hash) {
                        new_type->f_set = bucket.val;
                        KSO_INCREF(bucket.val);
                    }
                }
            }

            // first, fall the PC down
            vm->call_stk[vm->call_stk_n - 2].pc = vm->call_stk[vm->call_stk_n - 1].pc;

            // remove the stack
            POP_CALL_STK();

            ks_list_push(&vm->stk, (kso)new_type);

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

        kso ret = ks_list_pop(&vm->stk);
        ret->refcnt--;
    
        // the last item on the stack should be the one
        return ret;
    }
}




