/* types/float.c - implementation of the builtin `float` type 

The default float type is a double

*/

#include "ks_common.h"


/* C creation routines */

/* constructs a new integer from a 64-bit C integer */
ks_float ks_float_new(double v_float) {

    // NOTE: while we could attempt to intern integers, many tests (like Python's)
    //   have shown that this is actually not worth it, so for now we just always create a new one
    ks_float self = (ks_float)ks_malloc(sizeof(*self));
    *self = (struct ks_float) {
        KSO_BASE_INIT(ks_T_float)
        .v_float = v_float
    };

    // return our constructed integer
    return self;
}


/* type functions */

// float.__new__(obj) -> convert `obj` to an float
TFUNC(float, new) {
    KS_REQ_N_ARGS(n_args, 1);
    kso self = args[0];
    if (self->type == ks_T_float) return KSO_NEWREF(self);
    else if (self->type == ks_T_int) return (kso)ks_float_new(KS_INT_VAL((ks_int)self));
    else if (self->type == ks_T_str) return (kso)ks_float_new(strtod(((ks_str)self)->chr, NULL));
    else {
        // return an error
        KS_ERR_TYPECONV(self, ks_T_float);
    }
}


// float.__str__(self) -> return the integer as its string
TFUNC(float, str) {
    KS_REQ_N_ARGS(1, n_args);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_T_float, "self");

    // get the floating point value
    double v = self->v_float;

    // keep a string buffer for scratch space
    static char strbuf[64];

    // current position in the string buffer
    int strbuf_p = 0;

    bool is_neg = v < 0;
    if (v < 0) v = -v;

    int ct = strbuf_p;

    // number of digits to output
    int n_dig = 5, dig_base = 100000;

    int64_t frac = (int64_t)floor(v * dig_base);

    // now, output the decimal part
    while (ct++ < n_dig) {
        strbuf[strbuf_p++] = (frac % 10) + '0';
        frac /= 10;
    }

    strbuf[strbuf_p++] = '.';

    // output the whole number part
    int64_t v_i = (int64_t)(floor(v));

    do {
        strbuf[strbuf_p++] = (v_i % 10) + '0';
        v_i /= 10;
    } while (v_i > 0);

    // add a negative number
    if (is_neg) strbuf[strbuf_p++] = '-';

    // now reverse the whole thing
    int i;
    for (i = 0; 2 * i < strbuf_p; ++i) {
        char tmp = strbuf[i];
        strbuf[i] = strbuf[strbuf_p - i - 1];
        strbuf[strbuf_p - i - 1] = tmp;
    }

    // return from C string
    return (kso)ks_str_new_l(strbuf, strbuf_p);
}

// float.__repr__(self) -> return thefloat as its string representation
TFUNC(float, repr) {
    return (kso)ks_str_new("ASD");
}

// float.__add__(A, B) -> returns the sum of A and B
TFUNC(float, add) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float + ((ks_float)B)->v_float);
    }

    return NULL;
}

// float.__sub__(A, B) -> returns the difference of A and B
TFUNC(float, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float - ((ks_float)B)->v_float);
    }

    return NULL;
}

// float.__mul__(A, B) -> returns the product of A and B
TFUNC(float, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float * ((ks_float)B)->v_float);
    }

    return NULL;
}


// float.__div__(A, B) -> returns the quotient of A and B
TFUNC(float, div) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float / ((ks_float)B)->v_float);
    }

    return NULL;
}

// float.__mod__(A, B) -> returns the modulo of A and B (always positive)
TFUNC(float, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        double Af = ((ks_float)A)->v_float;
        double Bf = ((ks_float)B)->v_float;

        double res = fmod(Af, Bf);
        if (res < 0) res += Bf;

        return (kso)ks_float_new(res);
    }

    return NULL;
}

// float.__pow__(A, B) -> returns the power of A and B
TFUNC(float, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        double Af = ((ks_float)A)->v_float;
        double Bf = ((ks_float)B)->v_float;

        return (kso)ks_float_new(pow(Af, Bf));
    }

    return NULL;
}
/*


// int.__lt__(A, B) -> returns A < B
TFUNC(int, lt) {
    #define SIG "int.__lt__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int < ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__le__(A, B) -> returns A <= B
TFUNC(int, le) {
    #define SIG "int.__le__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int <= ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__gt__(A, B) -> returns A > B
TFUNC(int, gt) {
    #define SIG "int.__gt__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int > ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__ge__(A, B) -> returns A >= B
TFUNC(int, ge) {
    #define SIG "int.__ge__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int >= ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__eq__(A, B) -> returns A == B
TFUNC(int, eq) {
    #define SIG "int.__eq__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int == ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__ne__(A, B) -> returns A == B
TFUNC(int, ne) {
    #define SIG "int.__ne__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int != ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}


*/

// float.__neg__(A) -> returns -A
TFUNC(float, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float A = (ks_float)args[0];
    KS_REQ_TYPE(A, ks_T_float, "A");

    return (kso)ks_float_new(-A->v_float);
}


/* exporting functionality */

struct ks_type T_float, *ks_T_float = &T_float;

void ks_init__float() {

    /* create the type */

    T_float = KS_TYPE_INIT();

    ks_type_setname_c(ks_T_float, "float");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_float, "__new__", "float.__new__(obj)", float_new_);
    ADDCF(ks_T_float, "__str__", "float.__str__(self)", float_str_);
    ADDCF(ks_T_float, "__repr__", "float.__repr__(self)", float_repr_);

    ADDCF(ks_T_float, "__add__", "float.__add__(A, B)", float_add_);
    ADDCF(ks_T_float, "__sub__", "float.__sub__(A, B)", float_sub_);
    ADDCF(ks_T_float, "__mul__", "float.__mul__(A, B)", float_mul_);
    ADDCF(ks_T_float, "__div__", "float.__div__(A, B)", float_div_);
    ADDCF(ks_T_float, "__mod__", "float.__mod__(A, B)", float_mod_);
    ADDCF(ks_T_float, "__pow__", "float.__pow__(A, B)", float_pow_);
/*

    ADDCF(ks_T_int, "__lt__", "int.__lt__(A, B)", int_lt_);
    ADDCF(ks_T_int, "__le__", "int.__le__(A, B)", int_le_);
    ADDCF(ks_T_int, "__gt__", "int.__gt__(A, B)", int_gt_);
    ADDCF(ks_T_int, "__ge__", "int.__ge__(A, B)", int_ge_);
    ADDCF(ks_T_int, "__eq__", "int.__eq__(A, B)", int_eq_);
    ADDCF(ks_T_int, "__ne__", "int.__ne__(A, B)", int_ne_);
*/

    ADDCF(ks_T_float, "__neg__", "float.__neg__(A)", float_neg_);
}


