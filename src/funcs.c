/* funcs.c - standard/builtin functions to the kscript library */

// include the common for help with standard procedures
#include "ks_common.h"

/* declarations  */

ks_cfunc
    ks_F_print = NULL,
    ks_F_dict = NULL,

    ks_F_type = NULL,
    ks_F_call = NULL,
    ks_F_hash = NULL,
    ks_F_rand = NULL,
    ks_F_import = NULL,
    
    ks_F_repr = NULL,
    
    ks_F_getattr = NULL,
    ks_F_setattr = NULL,

    ks_F_getitem = NULL,
    ks_F_setitem = NULL,

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
    ks_F_ne  = NULL
;


/* builtin default functions */

FUNC(dict) {
    #define SIG "dict()"
    REQ_N_ARGS(0)

    return (kso)ks_dict_new_empty();
    #undef SIG
}

FUNC(print) {
    #define SIG "print(args...)"

    // just print out all the arguments, then a newline
    int i;
    for (i = 0; i < n_args; ++i) {

        if (i != 0) putc(' ', stdout);
        
        kso arg_i = args[i];

        if (arg_i->type == ks_T_str) {
            // do a little optimization here which may be more efficient than printf
            fwrite(((ks_str)arg_i)->chr, 1, ((ks_str)arg_i)->len, stdout);
            //printf("%s ", ((ks_str)arg_i)->chr);
        } else if (arg_i->type == ks_T_int) {
            printf("%ld", ((ks_int)arg_i)->v_int);
        } else {
            ks_str str_arg_i = kso_tostr(arg_i);
            fwrite(str_arg_i->chr, 1, str_arg_i->len, stdout);
            KSO_DECREF(str_arg_i);
        }

    }

    putc('\n', stdout);

    // optional
    fflush(stdout);

    return KSO_NONE;
    #undef SIG
}


/* misc util */
FUNC(type) {
    #define SIG "type(obj)"
    REQ_N_ARGS(1);

    return KSO_NEWREF(args[0]->type);
    #undef SIG
}

FUNC(hash) {
    #define SIG "hash(obj)"
    REQ_N_ARGS(1);
    return (kso)ks_int_new(kso_hash(args[0]));
    #undef SIG
}

FUNC(call) {
    #define SIG "call(func, args=(,))"
    REQ_N_ARGS_RANGE(1, 2);

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
    #undef SIG
}


FUNC(rand) {
    #define SIG "rand()"
    REQ_N_ARGS(0);
    return (kso)ks_int_new(ks_random_i64());
    #undef SIG
}

FUNC(repr) {
    #define SIG "repr(obj)"
    REQ_N_ARGS(1);
    return (kso)ks_str_new_cfmt("%R", args[0]);
    #undef SIG
}

FUNC(import) {
    #define SIG "import(name)"
    REQ_N_ARGS(1);
    ks_str name = (ks_str)args[0];
    REQ_TYPE("name", name, ks_T_str);
    return (kso)ks_load_module(name);
    #undef SIG
}




/* attribute resolving */

FUNC(getattr) {
    #define SIG "getattr(obj, attr)"
    REQ_N_ARGS(2);

    kso obj = args[0], attr = args[1];

    if (obj->type == ks_T_dict) {
        return ks_dict_get((ks_dict)obj, attr, kso_hash(attr));
    }

    // try resolving this
    if (obj->type->f_getattr != NULL) return kso_call(obj->type->f_getattr, 2, args);

    return NULL;
    #undef SIG
}
FUNC(setattr) {
    #define SIG "setattr(obj, attr, val)"
    REQ_N_ARGS(3);

    kso obj = args[0], attr = args[1], val = args[2];

    // try resolving this
    if (obj->type->f_setattr != NULL) return kso_call(obj->type->f_setattr, 2, args);

    return NULL;
    #undef SIG
}


/* item getting/setting */

FUNC(getitem) {
    #define SIG "getitem(obj, *keys)"
    REQ_N_ARGS_MIN(2);

    kso obj = args[0];

    // resolve the function
    if (obj->type->f_getitem != NULL) return kso_call(obj->type->f_getitem, 2, args);

    return NULL;
    #undef SIG
}
FUNC(setitem) {
    #define SIG "setitem(obj, *keys, val)"
    REQ_N_ARGS_MIN(3);

    kso obj = args[0];

    // try resolving this
    if (obj->type->f_setitem != NULL) {
        return kso_call(obj->type->f_setitem, n_args, args);
    }

    return NULL;
    #undef SIG
}

/* binary operators */

// prints out/handles an operator mismatch
#define BOP_MISMATCH(_A, _B, _ops) { \
    ks_type tA = (_A)->type, tB = (_B)->type; \
    return kse_fmt("Type mismatch for operator '" _ops "': '%*s' and '%*s'", tA->name->len, tA->name->chr, tB->name->len, tB->name->chr);\
}
// tries and resolves a binary operator, given the name of the type member
// ex: BOP_RESOLVE(A, B, f_add)
#define BOP_RESOLVE(_A, _B, _opf, _ops) { \
    kso oA = (_A), oB = (_B), oR = NULL; \
    ks_type tA = oA->type, tB = oB->type; \
    if (tA->_opf != NULL) { \
        oR = kso_call(tA->_opf, 2, ((kso[]){oA, oB})); \
        if (oR) return oR; \
    } else if (tB->_opf != NULL) { \
        oR = kso_call(tB->_opf, 2, ((kso[]){oA, oB})); \
        if (oR) return oR; \
    } \
    BOP_MISMATCH(_A, _B, _ops); \
}

/* define binary operator functions  */
FUNC(add) {
    #define SIG "add(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_add, "+");
    #undef SIG
}
FUNC(sub) {
    #define SIG "sub(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_sub, "-");
    #undef SIG
}
FUNC(mul) {
    #define SIG "mul(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_mul, "*");
    #undef SIG
}
FUNC(div) {
    #define SIG "div(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_div, "/");
    #undef SIG
}
FUNC(mod) {
    #define SIG "mod(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_mod, "%%");
    #undef SIG
}
FUNC(pow) {
    #define SIG "pow(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_pow, "**");
    #undef SIG
}
FUNC(lt) {
    #define SIG "lt(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_lt, "<");
    #undef SIG
}
FUNC(le) {
    #define SIG "le(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_le, "<=");
    #undef SIG
}
FUNC(gt) {
    #define SIG "gt(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_gt, ">");
    #undef SIG
}
FUNC(ge) {
    #define SIG "ge(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_ge, ">=");
    #undef SIG
}
FUNC(eq) {
    #define SIG "eq(A, B)"
    REQ_N_ARGS(2);
    // shortcut, the same object is always equal to itself
    if (args[0] == args[1]) return KSO_TRUE;
    BOP_RESOLVE(args[0], args[1], f_eq, "==");
    #undef SIG
}
FUNC(ne) {
    #define SIG "ne(A, B)"
    REQ_N_ARGS(2);
    // shortcut, the same object is never not equal to itself
    if (args[0] == args[1]) return KSO_FALSE;
    BOP_RESOLVE(args[0], args[1], f_ne, "!=");
    #undef SIG
}



// initializes the default C functions
void ksf_init() {

    /* builtins */

    ks_F_print = ks_cfunc_new(print_);
    ks_F_dict = ks_cfunc_new(dict_);

    ks_F_type = ks_cfunc_new(type_);
    ks_F_call = ks_cfunc_new(call_);
    ks_F_hash = ks_cfunc_new(hash_);
    ks_F_rand = ks_cfunc_new(rand_);
    
    ks_F_import = ks_cfunc_new(import_);

    ks_F_repr = ks_cfunc_new(repr_);

    ks_F_getattr = ks_cfunc_new(getattr_);
    ks_F_setattr = ks_cfunc_new(setattr_);

    ks_F_getitem = ks_cfunc_new(getitem_);
    ks_F_setitem = ks_cfunc_new(setitem_);

    ks_F_add = ks_cfunc_new(add_);
    ks_F_sub = ks_cfunc_new(sub_);
    ks_F_mul = ks_cfunc_new(mul_);
    ks_F_div = ks_cfunc_new(div_);
    ks_F_mod = ks_cfunc_new(mod_);
    ks_F_pow = ks_cfunc_new(pow_);
    ks_F_lt  = ks_cfunc_new(lt_);
    ks_F_le  = ks_cfunc_new(le_);
    ks_F_gt  = ks_cfunc_new(gt_);
    ks_F_ge  = ks_cfunc_new(ge_);
    ks_F_eq  = ks_cfunc_new(eq_);
    ks_F_ne  = ks_cfunc_new(ne_);

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

