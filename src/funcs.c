/* funcs.c - standard/builtin functions to the kscript library */

// include the common for help with standard procedures
#include "ks_common.h"

/* declarations  */
ks_cfunc
    ks_F_print = NULL,
    ks_F_dict = NULL,

    ks_F_repr = NULL,
    ks_F_type = NULL,
    ks_F_call = NULL,
    ks_F_hash = NULL,
    ks_F_rand = NULL,
    ks_F_import = NULL,
    ks_F_exit = NULL,

    ks_F_new_type = NULL,
    
    ks_F_getattr = NULL,
    ks_F_setattr = NULL,

    ks_F_getitem = NULL,
    ks_F_setitem = NULL,

    ks_F_iter = NULL,
    ks_F_next = NULL,

    /* binary operators */
    ks_F_add = NULL,
    ks_F_sub = NULL,
    ks_F_mul = NULL,
    ks_F_div = NULL,
    ks_F_mod = NULL,
    ks_F_pow = NULL,
    ks_F_lt  = NULL,
    ks_F_le  = NULL,
    ks_F_gt  = NULL,
    ks_F_ge  = NULL,
    ks_F_eq  = NULL,
    ks_F_ne  = NULL,

    /* unary operators */
    ks_F_neg = NULL,
    ks_F_sqig = NULL,

    /* interop */
    ks_F_shell = NULL

;


/* builtin default functions */


/* print(*args) -> print out all 'args' (seperated by a space), and then a newline
 *
 * If a specific 'arg' is not a string, 
 * 
 */
KS_FUNC(print) {
    // just print out all the arguments, then a newline
    int i;
    for (i = 0; i < n_args; ++i) {

        // put space between them
        if (i != 0) putc(' ', stdout);
        
        kso arg_i = args[i];

        if (arg_i->type == ks_T_str) {
            // do a little optimization here which may be more efficient than printf
            fwrite(((ks_str)arg_i)->chr, 1, ((ks_str)arg_i)->len, stdout);
            //printf("%s ", ((ks_str)arg_i)->chr);
        } else if (arg_i->type == ks_T_int) {
            printf("%ld", ((ks_int)arg_i)->v_int);
        } else {
            // convert to string (i.e. str(obj)), and print that
            ks_str str_arg_i = kso_tostr(arg_i);
            if (str_arg_i == NULL) return NULL;
            if (str_arg_i->type != ks_T_str) {
                return kse_fmt("Object '%R' could not be converted to string! (str() returns a non-string object!)", arg_i);
            }
            // otherwise, output the string
            fwrite(str_arg_i->chr, 1, str_arg_i->len, stdout);
            KSO_DECREF(str_arg_i);
        }

    }

    // end with newline
    putc('\n', stdout);

    // optional
    fflush(stdout);

    return KSO_NONE;
}


/* misc util */
KS_FUNC(type) {
    KS_REQ_N_ARGS(n_args, 1);
    kso res = (kso)args[0]->type;

    return KSO_NEWREF(res);
}

KS_FUNC(hash) {
    KS_REQ_N_ARGS(n_args, 1);

    return (kso)ks_int_new(kso_hash(args[0]));
}

KS_FUNC(exit) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 1);

    if (n_args == 0) {
        exit(0);
        return NULL;
    } else if (n_args == 1) {

        kso arg = args[0];
        if (arg->type == ks_T_int) {
            exit((int)((ks_int)arg)->v_int);
        } else {
            // error
            ks_warn("exit(code) called with non-int (type '%T')", arg);
            exit(1);
        }

        return NULL;
    } else {
        return NULL;
    }
}

KS_FUNC(call) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    if (n_args == 1) {
        return kso_call(args[0], 0, NULL);
    } else {
        // return 2 arguments
        kso func = args[0], values = args[1];
        if (values->type == ks_T_tuple) {
            return kso_call(func, ((ks_tuple)values)->len, ((ks_tuple)values)->items);
        } else {
            return kse_fmt("Tried calling with args=%R (should be a tuple)", values);
        }
    }

    return NULL;
}


KS_FUNC(rand) {
    KS_REQ_N_ARGS(n_args, 0);

    return (kso)ks_int_new(ks_random_i64());
}

KS_FUNC(repr) {
    KS_REQ_N_ARGS(n_args, 1);

    return (kso)ks_str_new_cfmt("%R", args[0]);
}

KS_FUNC(import) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str name = (ks_str)args[0];
    KS_REQ_TYPE(name, ks_T_str, "name");

    kso ret = (kso)ks_load_module(name);
    
    return ret;
}

KS_FUNC(new_type) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str name = (ks_str)args[0];
    KS_REQ_TYPE(name, ks_T_str, "name");

    // construct a new type
    ks_type new_type = ks_type_new(name->chr);
    
    // always parent to the kobj
    ks_type_add_parent(new_type, ks_T_kobj);

    return (kso)new_type;
}



/* attribute resolving */

KS_FUNC(getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    kso obj = args[0], attr = args[1];

    // loop up type(obj).__getattr__() function to call
    if (obj->type->f_getattr != NULL) {
        // if it exists, we try and call that with all arguments (args[0] becomes 'self')
        kso res = kso_call(obj->type->f_getattr, n_args, args);

        // if it was successful, return it
        if (res != NULL) return res;

        // otherwise, clear the error stack & try specific to that object
        kse_clear();
    }

    // otherwise, look up specific attributes about its type, and try and resolve the reference
    //   to a member function
    kso Tattr = ks_type_getattr(obj->type, (ks_str)attr);
    if (Tattr != NULL) {
        // it was a valid attribute
        // construct a partial function 
        ks_pfunc mem_func = ks_pfunc_new(Tattr);
        KSO_DECREF(Tattr);

        // now, set argument #0 to be filled as 'obj' (i.e. the 'self')
        ks_pfunc_fill(mem_func, 0, obj);

        // return the member function
        return (kso)mem_func;
    } else {
        // it was not found, and an error should have been added
        return NULL;
    }


    return NULL;
}
KS_FUNC(setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    kso obj = args[0], attr = args[1], val = args[2];

    // if the type supports setting the attribute, do it
    if (obj->type->f_setattr != NULL) {
        return kso_call(obj->type->f_setattr, n_args, args);
    }

    return NULL;
}


/* item getting/setting */

KS_FUNC(getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 2);

    kso obj = args[0];

    // try and call type(obj).__getitem__()
    if (obj->type->f_getitem != NULL) return kso_call(obj->type->f_getitem, n_args, args);

    return kse_fmt("No '[]' method for '%T' type", args[0]);
}

KS_FUNC(setitem) {
    KS_REQ_N_ARGS_MIN(n_args, 3);

    kso obj = args[0];

    // try calling type(obj).__setitem__()
    if (obj->type->f_setitem != NULL) return kso_call(obj->type->f_setitem, n_args, args);

    return kse_fmt("No '[]=' method for '%T' type", args[0]);
}

/* iterators */


/* iter(obj) -> return an iterator for a given object
 */
KS_FUNC(iter) {
    KS_REQ_N_ARGS(n_args, 1);
    kso obj = args[0];

    if (obj->type->f_iter == NULL) {
        return kse_fmt("'%T' object is not iterable!", obj);
    } else {
        // call the iterator
        return kso_call(obj->type->f_iter, 1, &obj);
    }
}


/* next(iter_obj) -> return the next object in an iterator, incrementing th eiterator
 */
KS_FUNC(next) {
    KS_REQ_N_ARGS(n_args, 1);
    kso iter_obj = args[0];

    if (iter_obj->type->f_next == NULL) {
        return kse_fmt("'%T' object is not next()-able!", iter_obj);
    } else {
        // call the iterator
        return kso_call(iter_obj->type->f_next, 1, &iter_obj);
    }
}


/* binary operators */

// prints out/handles an operator mismatch
#define BOP_MISMATCH(_A, _B, _ops) { \
    ks_type tA = (_A)->type, tB = (_B)->type; \
    return kse_fmt("Type mismatch for operator '" _ops "': '%*s' and '%*s'", tA->name->len, tA->name->chr, tB->name->len, tB->name->chr);\
}
// tries and resolves a binary operator, given the name of the type member
// ex: BOP_RESOLVE(A, B, f_add)
// TODO: handle exceptions
#define BOP_RESOLVE(_A, _B, _opf, _ops) { \
    kso oA = (_A), oB = (_B), oR = NULL; \
    ks_type tA = oA->type, tB = oB->type; \
    if (tA->_opf != NULL) { \
        oR = kso_call(tA->_opf, 2, ((kso[]){oA, oB})); \
        if (oR || kse_N() != 0) return oR; \
    } \
    if (tB->_opf != NULL) { \
        oR = kso_call(tB->_opf, 2, ((kso[]){oA, oB})); \
        if (oR || kse_N() != 0) return oR; \
    } \
    BOP_MISMATCH(_A, _B, _ops); \
}

/* define binary operator functions  */
KS_FUNC(add) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_add, "+");
}
KS_FUNC(sub) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_sub, "-");
}
KS_FUNC(mul) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_mul, "*");
}
KS_FUNC(div) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_div, "/");
}
KS_FUNC(mod) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_mod, "%%");
}
KS_FUNC(pow) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_pow, "**");
}
KS_FUNC(lt) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_lt, "<");
}
KS_FUNC(le) {
    KS_REQ_N_ARGS(n_args, 2);
    // we introduce a shortcut; if the two objects are the same pointer, they must be equal
    if (args[0] == args[1]) return KSO_TRUE;
    // otherwise, resolve the operator
    BOP_RESOLVE(args[0], args[1], f_le, "<=");
}
KS_FUNC(gt) {
    KS_REQ_N_ARGS(n_args, 2);
    BOP_RESOLVE(args[0], args[1], f_gt, ">");
}
KS_FUNC(ge) {
    KS_REQ_N_ARGS(n_args, 2);
    // we introduce a shortcut; if the two objects are the same pointer, they must be equal
    if (args[0] == args[1]) return KSO_TRUE;
    // otherwise, resolve the operator
    BOP_RESOLVE(args[0], args[1], f_ge, ">=");
}
KS_FUNC(eq) {
    KS_REQ_N_ARGS(n_args, 2);
    // we introduce a shortcut; if the two objects are the same pointer, they must be equal
    if (args[0] == args[1]) return KSO_TRUE;
    // otherwise, resolve the operator
    BOP_RESOLVE(args[0], args[1], f_eq, "==");
}
KS_FUNC(ne) {
    KS_REQ_N_ARGS(n_args, 2);
    // introduce a shortcut; if the two objects are the same pointer, they must be equal, so they are not not-equal
    if (args[0] == args[1]) return KSO_FALSE;
    // otherwise, resolve the operator
    BOP_RESOLVE(args[0], args[1], f_ne, "!=");
}


/* unary operators */

// unary operator resolving
#define UOP_RESOLVE(_A, _opf, _ops) { \
    kso oA = (_A), oR = NULL; \
    ks_type tA = oA->type; \
    if (tA->_opf != NULL) { \
        oR = kso_call(tA->_opf, 1, &oA); \
        if (oR) return oR; \
    } \
    kse_fmt("Unary operator '" _ops "' not defined for type '%T'", oA); \
}

KS_FUNC(neg) {
    KS_REQ_N_ARGS(n_args, 1);
    UOP_RESOLVE(args[0], f_neg, "-");
}

KS_FUNC(sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    UOP_RESOLVE(args[0], f_sqig, "~");
}


/* system interop */

// shell(cmd) -> run a command
KS_FUNC(shell) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str cmd = (ks_str)args[0];
    KS_REQ_TYPE(cmd, ks_T_str, "cmd");

    int res = system(cmd->chr);

    if (res == -1) return kse_fmt("system() called failed!");

    // run the command with `system()`
    return (kso)ks_int_new(res >> 8);
}



// initializes the default C functions
void ksf_init() {

    /* builtins */

    ks_F_type = ks_cfunc_new(type_, "type(obj)");
    ks_F_hash = ks_cfunc_new(hash_, "hash(obj)");
    ks_F_repr = ks_cfunc_new(repr_, "repr(obj)");

    ks_F_call = ks_cfunc_new(call_, "call(func, args=(,))");

    ks_F_rand = ks_cfunc_new(rand_, "rand()");
    ks_F_print = ks_cfunc_new(print_, "print(*args)");

    ks_F_import = ks_cfunc_new(import_, "import(module_name)");

    ks_F_shell = ks_cfunc_new(shell_, "shell(cmd)");
    ks_F_exit = ks_cfunc_new(exit_, "exit(code=0)");

    ks_F_new_type = ks_cfunc_new(new_type_, "__new_type__(name)");

    ks_F_getattr = ks_cfunc_new(getattr_, "getattr(obj, attr)");
    ks_F_setattr = ks_cfunc_new(setattr_, "setattr(obj, attr, val)");

    ks_F_getitem = ks_cfunc_new(getitem_, "getitem(obj, *keys)");
    ks_F_setitem = ks_cfunc_new(setitem_, "setitem(obj, *keys, val)");


    ks_F_iter = ks_cfunc_new(iter_, "__iter__(obj)");
    ks_F_next = ks_cfunc_new(next_, "__next__(iter_obj)");

    ks_F_add = ks_cfunc_new(add_, "__add__(A, B)");
    ks_F_sub = ks_cfunc_new(sub_, "__sub__(A, B)");
    ks_F_mul = ks_cfunc_new(mul_, "__mul__(A, B)");
    ks_F_div = ks_cfunc_new(div_, "__div__(A, B)");
    ks_F_mod = ks_cfunc_new(mod_, "__mod__(A, B)");
    ks_F_pow = ks_cfunc_new(pow_, "__pow__(A, B)");
    ks_F_lt  = ks_cfunc_new(lt_, "__lt__(A, B)");
    ks_F_le  = ks_cfunc_new(le_, "__le__(A, B)");
    ks_F_gt  = ks_cfunc_new(gt_, "__gt__(A, B)");
    ks_F_ge  = ks_cfunc_new(ge_, "__ge__(A, B)");
    ks_F_eq  = ks_cfunc_new(eq_, "__eq__(A, B)");
    ks_F_ne  = ks_cfunc_new(ne_, "__ne__(A, B)");

    ks_F_neg  = ks_cfunc_new(neg_, "__neg__(A)");
    ks_F_sqig  = ks_cfunc_new(sqig_, "__sqig__(A)");


    /* set type functions */

    /* integer functions */
    /*
    ks_T_int->f_str = (kso)ks_cfunc_new(int_str_);
    ks_T_int->f_repr = (kso)ks_cfunc_new(int_repr_);
    ks_T_int->f_add = (kso)ks_cfunc_new(int_add_);
    ks_T_int->f_sub = (kso)ks_cfunc_new(int_sub_);
    ks_T_int->f_mul = (kso)ks_cfunc_new(int_mul_);
    ks_T_int->f_div = (kso)ks_cfunc_new(int_div_);
    ks_T_int->f_mod = (kso)ks_cfunc_new(int_mod_);
    ks_T_int->f_pow = (kso)ks_cfunc_new(int_pow_);
    ks_T_int->f_lt  = (kso)ks_cfunc_new(int_lt_);
    ks_T_int->f_le  = (kso)ks_cfunc_new(int_le_);
    ks_T_int->f_gt  = (kso)ks_cfunc_new(int_gt_);
    ks_T_int->f_ge  = (kso)ks_cfunc_new(int_ge_);
    ks_T_int->f_eq  = (kso)ks_cfunc_new(int_eq_);
    ks_T_int->f_ne  = (kso)ks_cfunc_new(int_ne_);
*/

}

