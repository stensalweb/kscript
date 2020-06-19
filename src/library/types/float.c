/* types/float.c - implementation of 64 bit IEE float type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


/* floating point utility functions */

// determine whether a double is an integral value
static bool my_isint(double v) {
    return v == floor(v);
}



// forward declare it
KS_TYPE_DECLFWD(ks_type_float);

// global singleton for 'NAN' values
ks_float KS_NAN = NULL;

// create a kscript int from a C-style int
ks_float ks_float_new(double val) {
    // check for NAN
    if (val != val) return (ks_float)KS_NEWREF(KS_NAN);

    ks_float self = KS_ALLOC_OBJ(ks_float);
    KS_INIT_OBJ(self, ks_type_float);

    // initialize type-specific things
    self->val = val;

    return self;
}

/* member functions */


// float.__new__(obj) -> convert 'obj' to a float
static KS_TFUNC(float, new) {
    KS_REQ_N_ARGS(n_args, 1);

    ks_obj obj = args[0];

    if (obj->type == ks_type_float) {
        return KS_NEWREF(obj);
    } else if (obj->type == ks_type_int) {
        return (ks_obj)ks_float_new(((ks_int)obj)->val);
    } else if (obj->type == ks_type_complex) {
        return (ks_obj)ks_float_new(((ks_complex)obj)->val);
    } else if (obj->type == ks_type_str) {
        // TODO: error check and see if it was a valid float
        double val = atof(((ks_str)obj)->chr);
        return (ks_obj)ks_float_new(val);
    } else {
        KS_ERR_CONV(obj, ks_type_float);
    }
};


// float.__str__(self) -> convert to a string
static KS_TFUNC(float, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_type_float, "self");

    // print it out
    char cstr[256];
    snprintf(cstr, 255, "%.9lf", (double)self->val);

    int len = strlen(cstr);
    while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
        len--;
    }

    return (ks_obj)ks_str_new_l(cstr, len);
};


// float.__hash__(self) -> return the hash of a floating point object
static KS_TFUNC(float, hash) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_type_float, "self");

    // hash it like an integer if possible
    if (self->val == 0) {
        return (ks_obj)ks_int_new(1);
    } else if (my_isint(self->val)) {
        return (ks_obj)ks_int_new(floor(self->val));
    }

    ks_hash_t res = ks_hash_bytes(sizeof(self->val), (uint8_t*)&self->val);
    if (res == 0) res = 1;

    // return a hash of the raw bytes
    return (ks_obj)ks_int_new(res);
};



// float.__free__(self) -> free an float object
static KS_TFUNC(float, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_type_float, "self");

    // don't free the NAN
    if (self == KS_NAN) {
        self->refcnt = 0xFFFF;
        return KSO_NONE;
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// standard cases (i.e. float OP float, int OP float, float OP int)
// use 'vL' and 'vR' as arguments, assign result to 'vRes'
#define T_BOP_STDCASE(...) { \
    if (L->type == ks_type_float && R->type == ks_type_float) { \
        vL = ((ks_float)L)->val; \
        vR = ((ks_float)R)->val; \
        { __VA_ARGS__; } \
        return (ks_obj)ks_float_new(vRes); \
    } else if (L->type == ks_type_float && R->type == ks_type_int) { \
        vL = ((ks_float)L)->val; \
        vR = ((ks_int)R)->val; \
        { __VA_ARGS__; } \
        return (ks_obj)ks_float_new(vRes); \
    } else if (L->type == ks_type_int && R->type == ks_type_float) { \
        vL = ((ks_int)L)->val; \
        vR = ((ks_float)R)->val; \
        { __VA_ARGS__; } \
        return (ks_obj)ks_float_new(vRes); \
    } \
}

// float.__add__(L, R) -> add 2 numbers
static KS_TFUNC(float, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL + vR);

    KS_ERR_BOP_UNDEF("+", L, R);
};

// float.__sub__(L, R) -> subtract 2 numbers
static KS_TFUNC(float, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL - vR);

    KS_ERR_BOP_UNDEF("-", L, R);
};


// float.__mul__(L, R) -> multiply 2 numbers
static KS_TFUNC(float, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL * vR);

    KS_ERR_BOP_UNDEF("*", L, R);
};

// float.__div__(L, R) -> divide 2 numbers
static KS_TFUNC(float, div) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL / vR);

    KS_ERR_BOP_UNDEF("/", L, R);
};

// float.__mod__(L, R) -> modular
static KS_TFUNC(float, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR, vRes;

    T_BOP_STDCASE(vRes = fmod(vL, vR); if (vRes < 0) vRes += vR;);

    KS_ERR_BOP_UNDEF("%", L, R);
};

// float.__pow__(L, R) -> exponent
static KS_TFUNC(float, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR, vRes;

    T_BOP_STDCASE(if (!my_isint(vR) && vL < 0) { return ks_throw_fmt(ks_type_MathError, "Cannot raise negative number to a fractional power"); } vRes = pow(vL, vR));

    KS_ERR_BOP_UNDEF("**", L, R);
};



// comparison cases (i.e. float OP float, int OP float, float OP int)
// use 'vL' and 'vR' as arguments, assign boolean result to 'vRes'
#define T_BOP_CMPCASE(...) { \
    if (L->type == ks_type_float && R->type == ks_type_float) { \
        vL = ((ks_float)L)->val; \
        vR = ((ks_float)R)->val; \
        { __VA_ARGS__; } \
        return KSO_BOOL(vRes); \
    } else if (L->type == ks_type_int && R->type == ks_type_float) { \
        vL = ((ks_int)L)->val; \
        vR = ((ks_float)R)->val; \
        { __VA_ARGS__; } \
        return KSO_BOOL(vRes); \
    } else if (L->type == ks_type_float && R->type == ks_type_int) { \
        vL = ((ks_float)L)->val; \
        vR = ((ks_int)R)->val; \
        { __VA_ARGS__; } \
        return KSO_BOOL(vRes); \
    } \
}

// float comparison
static int my_cmp(double L, double R) {
    return L > R ? 1 : (L < R ? -1 : 0);
}


// float.__cmp__(L, R) -> cmp 2 floats
static KS_TFUNC(float, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    int64_t vRes;

    if (L->type == ks_type_float && R->type == ks_type_float) {
        return (ks_obj)ks_int_new(my_cmp(((ks_float)L)->val, ((ks_float)R)->val));
    } else if (L->type == ks_type_int && R->type == ks_type_float) {
        return (ks_obj)ks_int_new(my_cmp(((ks_int)L)->val, ((ks_float)R)->val));
    } else if (L->type == ks_type_float && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(my_cmp(((ks_float)L)->val, ((ks_int)R)->val));
    }

    KS_ERR_BOP_UNDEF("<=>", L, R);
};


// float.__lt__(L, R) -> cmp 2 floats
static KS_TFUNC(float, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_CMPCASE(vRes = vL < vR);

    KS_ERR_BOP_UNDEF("<", L, R);
};

// float.__le__(L, R) -> cmp 2 floats
static KS_TFUNC(float, le) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_CMPCASE(vRes = vL <= vR);

    KS_ERR_BOP_UNDEF("<=", L, R);
};


// float.__gt__(L, R) -> cmp 2 floats
static KS_TFUNC(float, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_CMPCASE(vRes = vL > vR);

    KS_ERR_BOP_UNDEF(">", L, R);
};


// float.__ge__(L, R) -> cmp 2 floats
static KS_TFUNC(float, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_CMPCASE(vRes = vL >= vR);

    KS_ERR_BOP_UNDEF(">=", L, R);
};


// float.__eq__(L, R) -> cmp 2 floats
static KS_TFUNC(float, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_CMPCASE(vRes = vL == vR);

    KS_ERR_BOP_UNDEF("==", L, R);
};


// float.__ne__(L, R) -> cmp 2 floats
static KS_TFUNC(float, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_CMPCASE(vRes = vL != vR);

    KS_ERR_BOP_UNDEF("!=", L, R);
};


// float.__neg__(V) -> negative float
static KS_TFUNC(float, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_float) {
        return (ks_obj)ks_float_new(-((ks_float)V)->val);
    }

    KS_ERR_UOP_UNDEF("-", V);
};

// float.__sqig__(V) -> round to nearest
static KS_TFUNC(float, sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_float) {
        double vv = ((ks_float)V)->val;
        return (ks_obj)ks_int_new(round(vv));
    }

    KS_ERR_UOP_UNDEF("~", V);
};


// float.isnan(self) -> return if it is NAN
static KS_TFUNC(float, isnan) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_type_float, "self");

    return KSO_BOOL(self->val != self->val);
}

// float.isint(self) -> return if it is an integer value
static KS_TFUNC(float, isint) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_type_float, "self");

    return KSO_BOOL(my_isint(self->val));
}



// initialize float type
void ks_type_float_init() {
    KS_INIT_TYPE_OBJ(ks_type_float, "float");

    // construct NAN value
    KS_NAN = KS_ALLOC_OBJ(ks_float);
    KS_INIT_OBJ(KS_NAN, ks_type_float);
    KS_NAN->val = nan("0");

    // initialize type-specific things

    ks_type_set_cn(ks_type_float, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(float_new_, "float.__new__(obj)")},

        {"__str__", (ks_obj)ks_cfunc_new2(float_str_, "float.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(float_str_, "float.__repr__(self)")},
        {"__hash__", (ks_obj)ks_cfunc_new2(float_hash_, "float.__hash__(self)")},
        
        {"__free__", (ks_obj)ks_cfunc_new2(float_free_, "float.__free__(self)")},
 
        {"__add__", (ks_obj)ks_cfunc_new2(float_add_, "float.__add__(L, R)")},
        {"__sub__", (ks_obj)ks_cfunc_new2(float_sub_, "float.__sub__(L, R)")},
        {"__mul__", (ks_obj)ks_cfunc_new2(float_mul_, "float.__mul__(L, R)")},
        {"__div__", (ks_obj)ks_cfunc_new2(float_div_, "float.__div__(L, R)")},
        {"__mod__", (ks_obj)ks_cfunc_new2(float_mod_, "float.__mod__(L, R)")},
        {"__pow__", (ks_obj)ks_cfunc_new2(float_pow_, "float.__pow__(L, R)")},
 
        {"__cmp__", (ks_obj)ks_cfunc_new2(float_cmp_, "float.__cmp__(L, R)")},
        {"__lt__", (ks_obj)ks_cfunc_new2(float_lt_, "float.__lt__(L, R)")},
        {"__le__", (ks_obj)ks_cfunc_new2(float_le_, "float.__le__(L, R)")},
        {"__gt__", (ks_obj)ks_cfunc_new2(float_gt_, "float.__gt__(L, R)")},
        {"__ge__", (ks_obj)ks_cfunc_new2(float_ge_, "float.__ge__(L, R)")},
        {"__eq__", (ks_obj)ks_cfunc_new2(float_eq_, "float.__eq__(L, R)")},
        {"__ne__", (ks_obj)ks_cfunc_new2(float_ne_, "float.__ne__(L, R)")},

        {"__neg__", (ks_obj)ks_cfunc_new2(float_neg_, "float.__neg__(self)")},
        {"__sqig__", (ks_obj)ks_cfunc_new2(float_sqig_, "float.__sqig__(self)")},

        {"isnan", (ks_obj)ks_cfunc_new2(float_isnan_, "float.isnan(self)")},
        {"isint", (ks_obj)ks_cfunc_new2(float_isint_, "float.isint(self)")},


        {"nan", KS_NEWREF(KS_NAN)},
 
        {NULL, NULL}   
    });



}

