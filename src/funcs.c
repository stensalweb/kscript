/* funcs.c - standard/builtin functions to the kscript library */

#include "ks.h"

/* declarations  */

ks_cfunc
    ks_F_print = NULL,
    ks_F_add = NULL,
    ks_F_sub = NULL,
    ks_F_mul = NULL,
    ks_F_div = NULL
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


#define FUNC(_name) static KS_CFUNC_DECL(_name##_)



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

FUNC(add) {
    #undef SIG
    #define SIG "add(A, B)"
    REQ_N_ARGS(2);

    // extract arguments
    kso A = args[0], B = args[1];

    return KSO_NONE;
}

FUNC(sub) {
    #undef SIG
    #define SIG "add(A, B)"
    REQ_N_ARGS(2);

    // extract arguments
    kso A = args[0], B = args[1];

    return KSO_NONE;
}

FUNC(mul) {
    #undef SIG
    #define SIG "mul(A, B)"
    REQ_N_ARGS(2);

    // extract arguments
    kso A = args[0], B = args[1];

    return KSO_NONE;
}

FUNC(div) {
    #undef SIG
    #define SIG "div(A, B)"
    REQ_N_ARGS(2);

    // extract arguments
    kso A = args[0], B = args[1];

    return KSO_NONE;
}


// initializes the default C functions
void ksf_init() {

    /* builtins */

    ks_F_print = ks_cfunc_newref(print_);

    ks_F_add = ks_cfunc_newref(add_);
    ks_F_sub = ks_cfunc_newref(sub_);
    ks_F_mul = ks_cfunc_newref(mul_);
    ks_F_div = ks_cfunc_newref(div_);

}

