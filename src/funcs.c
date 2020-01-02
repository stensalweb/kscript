/* funcs.c - standard/builtin functions to the kscript library */

#include "ks.h"

/* declarations  */

ks_cfunc
    ks_F_print = NULL,
    ks_F_add = NULL,
    ks_F_sub = NULL,
    ks_F_mul = NULL,
    ks_F_div = NULL,
    ks_F_lt  = NULL,
    ks_F_gt  = NULL,
    ks_F_eq  = NULL
;



/* helper macros for standard errors */


// define SIG as the C-string descibing the signature of the function. This will help with errors
#define SIG


// require a specific number of arguments
#define REQ_N_ARGS(_num) if (n_args != _num) { return kse_fmt(SIG ": Expected %i args, but got %i", _num, n_args); }

// require a minimum number of arguments
#define REQ_N_ARGS_MIN(_min) if (n_args < _min) { return kse_fmt(SIG ": Expected >=%i args, but got %i", _min, n_args); }

// require a maximum number of arguments
#define REQ_N_ARGS_MAX(_max) if (n_args > _max) { return kse_fmt(SIG ": Expected <=%i args, but got %i", _max, n_args); }

// require a range of number of arguments
#define REQ_N_ARGS_RANGE(_min, _max) if (n_args > _max || n_args < _min) { return kse_fmt(SIG ": Expected %i<=args<=%i, but got %i", _min, _max, n_args); }


// require an object _obj to be of type _type, or print an error. The object is referred to as its C-string style name `_objname`
#define REQ_TYPE(_objname, _obj, _type) if (!(_obj) || (_obj)->type != (_type)) { return kse_fmt(SIG ": Expected %*s to be of type '%*s', but it was of type '%*s'", sizeof(_objname) - 1, objname, (_type)->name->len, (_type)->name->chr, (_obj)->type->name->len, (_obj)->type->name->chr); }


// define a C-function with a given name
// (the actual name has an _ appended)
#define FUNC(_name) static KS_CFUNC_DECL(_name##_)

// define a C-function with a given type and name
// (the actual name has an _ appended)
#define TFUNC(_type, _name) static KS_CFUNC_DECL(_type##_##_name##_)


/* builtin default functions */

FUNC(print) {
    #undef SIG
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
            KSO_CHKREF(str_arg_i);
        }

    }

    putc('\n', stdout);

    // optional
    fflush(stdout);

    return KSO_NONE;
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


FUNC(add) {
    #undef SIG
    #define SIG "add(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_add, "+");
}

FUNC(sub) {
    #undef SIG
    #define SIG "sub(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_sub, "-");
}

FUNC(mul) {
    #undef SIG
    #define SIG "mul(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_mul, "*");
}

FUNC(div) {
    #undef SIG
    #define SIG "div(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_div, "/");
}


FUNC(lt) {
    #undef SIG
    #define SIG "lt(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_lt, "<");
}
FUNC(gt) {
    #undef SIG
    #define SIG "gt(A, B)"
    REQ_N_ARGS(2);
    BOP_RESOLVE(args[0], args[1], f_gt, ">");
}
FUNC(eq) {
    #undef SIG
    #define SIG "eq(A, B)"
    REQ_N_ARGS(2);
    // shortcut, objects of the same type should always be true
    if (args[0] == args[1]) return KSO_TRUE;
    BOP_RESOLVE(args[0], args[1], f_eq, "==");
}



/* type specific functions */


/* integer functions */

TFUNC(int, str) {
    #undef SIG
    #define SIG "int.str(A)"
    REQ_N_ARGS(1);

    ks_int A = (ks_int)args[0];

    return (kso)ks_str_new_cfmt("%l", A->v_int);
}

TFUNC(int, repr) {
    #undef SIG
    #define SIG "int.repr(A)"
    REQ_N_ARGS(1);

    ks_int A = (ks_int)args[0];

    return (kso)ks_str_new_cfmt("%l", A->v_int);
}

TFUNC(int, add) {
    #undef SIG
    #define SIG "int.add(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int + ((ks_int)B)->v_int);
    }

    return NULL;
}
TFUNC(int, sub) {
    #undef SIG
    #define SIG "int.sub(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int - ((ks_int)B)->v_int);
    }

    return NULL;
}
TFUNC(int, mul) {
    #undef SIG
    #define SIG "int.mul(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];


    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int * ((ks_int)B)->v_int);
    }

    return NULL;
}
TFUNC(int, div) {
    #undef SIG
    #define SIG "int.div(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int / ((ks_int)B)->v_int);
    }

    return NULL;
}

TFUNC(int, lt) {
    #undef SIG
    #define SIG "int.lt(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int < ((ks_int)B)->v_int);
    }

    return NULL;
}

TFUNC(int, gt) {
    #undef SIG
    #define SIG "int.gt(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int > ((ks_int)B)->v_int);
    }

    return NULL;
}

TFUNC(int, eq) {
    #undef SIG
    #define SIG "int.eq(A, B)"
    REQ_N_ARGS(2);
    // extract arguments
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int == ((ks_int)B)->v_int);
    }

    return NULL;
}

/* string functions */


TFUNC(str, add) {
    #undef SIG
    #define SIG "str.add(A, B)"
    REQ_N_ARGS(2);

    // just use the formatter
    return ks_str_new_cfmt("%V%V", args[0], args[1]);
}



// initializes the default C functions
void ksf_init() {

    /* builtins */

    ks_F_print = ks_cfunc_newref(print_);

    ks_F_add = ks_cfunc_newref(add_);
    ks_F_sub = ks_cfunc_newref(sub_);
    ks_F_mul = ks_cfunc_newref(mul_);
    ks_F_div = ks_cfunc_newref(div_);
    ks_F_lt = ks_cfunc_newref(lt_);
    ks_F_gt = ks_cfunc_newref(gt_);
    ks_F_eq = ks_cfunc_newref(eq_);


    /* set type functions */

    /* integer functions */
    ks_T_int->f_str = (kso)ks_cfunc_newref(int_str_);
    ks_T_int->f_repr = (kso)ks_cfunc_newref(int_repr_);
    ks_T_int->f_add = (kso)ks_cfunc_newref(int_add_);
    ks_T_int->f_sub = (kso)ks_cfunc_newref(int_sub_);
    ks_T_int->f_mul = (kso)ks_cfunc_newref(int_mul_);
    ks_T_int->f_div = (kso)ks_cfunc_newref(int_div_);
    ks_T_int->f_lt  = (kso)ks_cfunc_newref(int_lt_);
    ks_T_int->f_gt  = (kso)ks_cfunc_newref(int_gt_);
    ks_T_int->f_eq  = (kso)ks_cfunc_newref(int_eq_);


    /* str funcs */
    ks_T_str->f_add  = (kso)ks_cfunc_newref(str_add_);

}

