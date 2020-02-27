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
KS_TFUNC(float, new) {
    KS_REQ_N_ARGS(n_args, 1);
    kso self = args[0];
    if (self->type == ks_T_float) return KSO_NEWREF(self);
    else if (self->type == ks_T_int) return (kso)ks_float_new(floor(((ks_int)self)->v_int));
    else if (self->type == ks_T_str) return (kso)ks_float_new(strtod(((ks_str)self)->chr, NULL));
    else {
        // return an error
        KS_ERR_TYPECONV(self, ks_T_float);
    }
}


// float.__str__(self) -> return the integer as its string
KS_TFUNC(float, str) {
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
KS_TFUNC(float, repr) {
    return (kso)ks_str_new("ASD");
}

// float.__add__(A, B) -> returns the sum of A and B
KS_TFUNC(float, add) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float + ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_int)A)->v_int + ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return (kso)ks_float_new(((ks_float)A)->v_float + ((ks_int)B)->v_int);
    }

    return NULL;
}

// float.__sub__(A, B) -> returns the difference of A and B
KS_TFUNC(float, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float - ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_int)A)->v_int - ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return (kso)ks_float_new(((ks_float)A)->v_float - ((ks_int)B)->v_int);
    }

    return NULL;
}

// float.__mul__(A, B) -> returns the product of A and B
KS_TFUNC(float, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float * ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_int)A)->v_int * ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return (kso)ks_float_new(((ks_float)A)->v_float * ((ks_int)B)->v_int);
    }

    return NULL;
}


// float.__div__(A, B) -> returns the quotient of A and B
KS_TFUNC(float, div) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_float)A)->v_float / ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return (kso)ks_float_new(((ks_int)A)->v_int / ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return (kso)ks_float_new(((ks_float)A)->v_float / ((ks_int)B)->v_int);
    }

    return NULL;
}

// float.__mod__(A, B) -> returns the modulo of A and B (always positive)
KS_TFUNC(float, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        double Af = ((ks_float)A)->v_float;
        double Bf = ((ks_float)B)->v_float;

        double res = fmod(Af, Bf);
        if (res < 0) res += Bf;

        return (kso)ks_float_new(res);
    }else if (A->type == ks_T_int && B->type == ks_T_float) {
        int64_t Ai = ((ks_int)A)->v_int;
        double Bf = ((ks_float)B)->v_float;

        double Rf = fmod(Ai, Bf);

        // by convention, modulo should always be positive
        if (Rf < 0) Rf += Bf;

        return (kso)ks_float_new(Rf);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        double Af = ((ks_float)A)->v_float;
        int64_t Bi = ((ks_int)B)->v_int;

        double Rf = fmod(Af, Bi);

        // by convention, modulo should always be positive
        if (Rf < 0) Rf += Bi;

        return (kso)ks_float_new(Rf);
    }

    return NULL;
}

// float.__pow__(A, B) -> returns the power of A and B
KS_TFUNC(float, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        double Af = ((ks_float)A)->v_float;
        double Bf = ((ks_float)B)->v_float;

        return (kso)ks_float_new(pow(Af, Bf));
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        int64_t Ai = ((ks_int)A)->v_int;
        double Bf = ((ks_float)B)->v_float;

        return (kso)ks_float_new(pow(Ai, Bf));
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        double Af = ((ks_float)A)->v_float;
        int64_t Bi = ((ks_int)B)->v_int;

        return (kso)ks_float_new(pow(Af, Bi));
    }

    return NULL;
}

// float.__lt__(A, B) -> returns A < B
KS_TFUNC(float, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return KSO_BOOL(((ks_float)A)->v_float < ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return KSO_BOOL(((ks_int)A)->v_int < ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return KSO_BOOL(((ks_float)A)->v_float < ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// float.__le__(A, B) -> returns A <= B
KS_TFUNC(float, le) {
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return KSO_BOOL(((ks_float)A)->v_float <= ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return KSO_BOOL(((ks_int)A)->v_int <= ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return KSO_BOOL(((ks_float)A)->v_float <= ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// float.__gt__(A, B) -> returns A > B
KS_TFUNC(float, gt) {
    #define SIG "int.__gt__(A, B)"
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return KSO_BOOL(((ks_float)A)->v_float > ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return KSO_BOOL(((ks_int)A)->v_int > ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return KSO_BOOL(((ks_float)A)->v_float > ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// float.__ge__(A, B) -> returns A >= B
KS_TFUNC(float, ge) {
    #define SIG "int.__ge__(A, B)"
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return KSO_BOOL(((ks_float)A)->v_float >= ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return KSO_BOOL(((ks_int)A)->v_int >= ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return KSO_BOOL(((ks_float)A)->v_float >= ((ks_int)B)->v_int);
    }
    return NULL;
    #undef SIG
}

// float.__eq__(A, B) -> returns A == B
KS_TFUNC(float, eq) {
    #define SIG "int.__eq__(A, B)"
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_float && B->type == ks_T_float) {
        return KSO_BOOL(((ks_float)A)->v_float == ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return KSO_BOOL(((ks_int)A)->v_int == ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return KSO_BOOL(((ks_float)A)->v_float == ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// float.__ne__(A, B) -> returns A == B
KS_TFUNC(float, ne) {
    #define SIG "int.__ne__(A, B)"
    KS_REQ_N_ARGS(n_args, 2);
    kso A = args[0], B = args[1];
    
    if (A->type == ks_T_float && B->type == ks_T_float) {
        return KSO_BOOL(((ks_float)A)->v_float != ((ks_float)B)->v_float);
    } else if (A->type == ks_T_int && B->type == ks_T_float) {
        return KSO_BOOL(((ks_int)A)->v_int != ((ks_float)B)->v_float);
    } else if (A->type == ks_T_float && B->type == ks_T_int) {
        return KSO_BOOL(((ks_float)A)->v_float != ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}



// float.__neg__(A) -> returns -A
KS_TFUNC(float, neg) {
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

    ADDCF(ks_T_float, "__lt__", "float.__lt__(A, B)", float_lt_);
    ADDCF(ks_T_float, "__le__", "float.__le__(A, B)", float_le_);
    ADDCF(ks_T_float, "__gt__", "float.__gt__(A, B)", float_gt_);
    ADDCF(ks_T_float, "__ge__", "float.__ge__(A, B)", float_ge_);
    ADDCF(ks_T_float, "__eq__", "float.__eq__(A, B)", float_eq_);
    ADDCF(ks_T_float, "__ne__", "float.__ne__(A, B)", float_ne_);

    ADDCF(ks_T_float, "__neg__", "float.__neg__(A)", float_neg_);
}


