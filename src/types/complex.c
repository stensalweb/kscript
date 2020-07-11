/* types/complex.c - implementation of the complex type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_complex);

// create a kscript int from a C-style int
ks_complex ks_complex_new(double complex val) {
    ks_complex self = KS_ALLOC_OBJ(ks_complex);
    KS_INIT_OBJ(self, ks_type_complex);

    // initialize type-specific things
    self->val = val;

    return self;
}

/* member functions */


// complex.__new__(real=0, imag=0) -> construct a new complex number
static KS_TFUNC(complex, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 2);

    if (n_args == 0) {
        return (ks_obj)ks_complex_new(0);
    } else if (n_args == 1) {
        double complex real_v;
        if (
            !ks_num_get_double_complex(args[0], &real_v)
        ) {
            return NULL;
        }

        // now, construct it
        return (ks_obj)ks_complex_new(real_v);
    } else {
        // n_args == 2
        double complex real_v, imag_v;
        if (
            !ks_num_get_double_complex(args[0], &real_v) || 
            !ks_num_get_double_complex(args[1], &imag_v)
        ) {
            return NULL;
        }

        // now, construct it
        return (ks_obj)ks_complex_new(real_v + I * imag_v);
    }

};


// complex.__str__(self) -> convert to a string
static KS_TFUNC(complex, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_complex self = (ks_complex)args[0];
    KS_REQ_TYPE(self, ks_type_complex, "self");

    char cstr[260];
    int len = 0;
    if (fabs(creal(self->val)) < 1e-14) {
        // just imaginary part

        snprintf(cstr, 255, "%.9lf", cimag(self->val));
        len = strlen(cstr);

        while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
            len--;
        }

        cstr[len++] = 'i';
        cstr[len] = '\0';

    } else {
        // real + imaginary, even if imaginary is 0, so it is obvious it is a complex number
        snprintf(cstr, 255, "(%.9lf", creal(self->val));

        len = strlen(cstr);
        while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
            len--;
        }

        // now, print out imaginary
        snprintf(cstr+len, 255-len, "%+.9lf", cimag(self->val));
        len = strlen(cstr);
        while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
            len--;
        }
        cstr[len++] = 'i';
        cstr[len++] = ')';
        cstr[len] = '\0';

    }

    return (ks_obj)ks_str_new_l(cstr, len);
};

// complex.__free__(self) -> free a complex object
static KS_TFUNC(complex, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_complex self = (ks_complex)args[0];
    KS_REQ_TYPE(self, ks_type_complex, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// complex.__sqig__(V) -> return complex conjugate
static KS_TFUNC(complex, sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_complex) {
        complex double vv = ((ks_complex)V)->val;
        return (ks_obj)ks_complex_new(conj(vv));
    }

    KS_ERR_UOP_UNDEF("~", V);
};

// complex.__getattr__(self, attr) -> get attribute
static KS_TFUNC(complex, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_complex self = (ks_complex)args[0];
    KS_REQ_TYPE(self, ks_type_complex, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");

    // check standards:
    if (attr->len == 4 && strncmp(attr->chr, "real", 4) == 0) {
        return (ks_obj)ks_float_new(creal(self->val));
    } else if (attr->len == 4 && strncmp(attr->chr, "imag", 4) == 0) {
        return (ks_obj)ks_float_new(cimag(self->val));
    }

    // now, try getting a member function
    ks_obj ret = ks_type_get_mf(self->type, attr, (ks_obj)self);
    if (!ret) {
        KS_ERR_ATTR(self, attr);
    }

    return ret;
};

// complex.__setattr__(self, attr, val) -> set attribute
static KS_TFUNC(complex, setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_complex self = (ks_complex)args[0];
    KS_REQ_TYPE(self, ks_type_complex, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");
    ks_obj val = args[2];

    bool isReal = false;


    // check standards:
    if (attr->len == 4 && strncmp(attr->chr, "real", 4) == 0) {
        double val_d = 0.0;
        if (!ks_num_get_double(val, &val_d)) return NULL;

        // set
        self->val = val_d + I * cimag(self->val);

        return KSO_NONE;
    } else if (attr->len == 4 && strncmp(attr->chr, "imag", 4) == 0) {
        double val_d = 0.0;
        if (!ks_num_get_double(val, &val_d)) return NULL;
        self->val = creal(self->val) + I * val_d;

        return KSO_NONE;
    }

    KS_ERR_ATTR(self, attr);
};




/** MATH FUNCS **/

static KS_TFUNC(complex, add) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_add(args[0], args[1]);
};
static KS_TFUNC(complex, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_sub(args[0], args[1]);
};
static KS_TFUNC(complex, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_mul(args[0], args[1]);
};
static KS_TFUNC(complex, div) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_div(args[0], args[1]);
};
static KS_TFUNC(complex, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_mod(args[0], args[1]);
};
static KS_TFUNC(complex, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_pow(args[0], args[1]);
};

static KS_TFUNC(complex, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    return ks_num_neg(args[0]);
};

static KS_TFUNC(complex, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    int res;
    if (!ks_num_cmp(args[0], args[1], &res)) return NULL;
    return (ks_obj)ks_int_new(res);
};

static KS_TFUNC(complex, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_lt(args[0], args[1]);
};
static KS_TFUNC(complex, le) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_le(args[0], args[1]);
};
static KS_TFUNC(complex, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_gt(args[0], args[1]);
};
static KS_TFUNC(complex, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_ge(args[0], args[1]);
};
static KS_TFUNC(complex, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    bool res;
    if (!ks_num_eq(args[0], args[1], &res)) return NULL;
    return KSO_BOOL(res);
};
static KS_TFUNC(complex, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    bool res;
    if (!ks_num_eq(args[0], args[1], &res)) return NULL;
    return KSO_BOOL(!res);
};





// initialize complex type
void ks_type_complex_init() {
    KS_INIT_TYPE_OBJ(ks_type_complex, "complex");

    // initialize type-specific things

    ks_type_set_cn(ks_type_complex, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(complex_new_, "complex.__new__(obj)")},

        {"__getattr__", (ks_obj)ks_cfunc_new2(complex_getattr_, "complex.__getattr__(self, attr)")},
        {"__setattr__", (ks_obj)ks_cfunc_new2(complex_setattr_, "complex.__setattr__(self, attr, val)")},

        {"__str__", (ks_obj)ks_cfunc_new2(complex_str_, "complex.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(complex_str_, "complex.__repr__(self)")},
        
        {"__free__", (ks_obj)ks_cfunc_new2(complex_free_, "complex.__free__(self)")},

        {"__add__",       (ks_obj)ks_cfunc_new2(complex_add_, "complex.__add__(L, R)")},
        {"__sub__",       (ks_obj)ks_cfunc_new2(complex_sub_, "complex.__sub__(L, R)")},
        {"__mul__",       (ks_obj)ks_cfunc_new2(complex_mul_, "complex.__mul__(L, R)")},
        {"__div__",       (ks_obj)ks_cfunc_new2(complex_div_, "complex.__div__(L, R)")},
        {"__mod__",       (ks_obj)ks_cfunc_new2(complex_mod_, "complex.__mod__(L, R)")},
        {"__pow__",       (ks_obj)ks_cfunc_new2(complex_pow_, "complex.__pow__(L, R)")},
        {"__neg__",       (ks_obj)ks_cfunc_new2(complex_neg_, "complex.__neg__(L)")},

        {"__cmp__",       (ks_obj)ks_cfunc_new2(complex_cmp_, "complex.__cmp__(L, R)")},
        {"__lt__",        (ks_obj)ks_cfunc_new2(complex_lt_, "complex.__lt__(L, R)")},
        {"__le__",        (ks_obj)ks_cfunc_new2(complex_le_, "complex.__le__(L, R)")},
        {"__gt__",        (ks_obj)ks_cfunc_new2(complex_gt_, "complex.__gt__(L, R)")},
        {"__ge__",        (ks_obj)ks_cfunc_new2(complex_ge_, "complex.__ge__(L, R)")},
        {"__eq__",        (ks_obj)ks_cfunc_new2(complex_eq_, "complex.__eq__(L, R)")},
        {"__ne__",        (ks_obj)ks_cfunc_new2(complex_ne_, "complex.__ne__(L, R)")},


        {"__sqig__", (ks_obj)ks_cfunc_new2(complex_sqig_, "complex.__sqig__(self)")},

        {"i", (ks_obj)ks_complex_new(I)},

        {NULL, NULL}   
    });



}

