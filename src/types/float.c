/* types/float.c - implementation of 64 bit IEE float type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_float);

// create a kscript int from a C-style int
ks_float ks_float_new(double val) {
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

// float.__free__(self) -> free an float object
static KS_TFUNC(float, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_float self = (ks_float)args[0];
    KS_REQ_TYPE(self, ks_type_float, "self");

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

    T_BOP_STDCASE(vRes = pow(vL, vR));

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
    } \
}


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


// initialize float type
void ks_type_float_init() {
    KS_INIT_TYPE_OBJ(ks_type_float, "float");

    ks_type_set_cn(ks_type_float, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new(float_new_)},

        {"__str__", (ks_obj)ks_cfunc_new(float_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(float_str_)},
        
        {"__free__", (ks_obj)ks_cfunc_new(float_free_)},
 
        {"__add__", (ks_obj)ks_cfunc_new(float_add_)},
        {"__sub__", (ks_obj)ks_cfunc_new(float_sub_)},
        {"__mul__", (ks_obj)ks_cfunc_new(float_mul_)},
        {"__div__", (ks_obj)ks_cfunc_new(float_div_)},
        {"__mod__", (ks_obj)ks_cfunc_new(float_mod_)},
        {"__pow__", (ks_obj)ks_cfunc_new(float_pow_)},
 
        {"__lt__", (ks_obj)ks_cfunc_new(float_lt_)},
        {"__le__", (ks_obj)ks_cfunc_new(float_le_)},
        {"__gt__", (ks_obj)ks_cfunc_new(float_gt_)},
        {"__ge__", (ks_obj)ks_cfunc_new(float_ge_)},
        {"__eq__", (ks_obj)ks_cfunc_new(float_eq_)},
        {"__ne__", (ks_obj)ks_cfunc_new(float_ne_)},

        {"__neg__", (ks_obj)ks_cfunc_new(float_neg_)},
        {"__sqig__", (ks_obj)ks_cfunc_new(float_sqig_)},
 
        {NULL, NULL}   
    });
}

