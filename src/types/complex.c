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
        ks_obj real = args[0];

        if (real->type == ks_type_complex) {
            return KS_NEWREF(real);
        } else if (real->type == ks_type_float) {
            return (ks_obj)ks_complex_new(((ks_float)real)->val);
        } else if (real->type == ks_type_int) {
            return (ks_obj)ks_complex_new(((ks_int)real)->val);
        } else {
            KS_ERR_CONV(real, ks_type_complex);
        }
    } else {
        // n_args == 2
        ks_obj real = args[0], imag = args[1];

        double real_d = 0.0, imag_d = 0.0;


        // add 1 * real

        if (real->type == ks_type_complex) {
            // add together
            real_d += creal(((ks_complex)real)->val);
            imag_d += cimag(((ks_complex)real)->val);
        } else if (real->type == ks_type_float) {
            real_d += ((ks_float)real)->val;
        } else if (real->type == ks_type_int) {
            real_d += ((ks_int)real)->val;
        } else {
            KS_ERR_CONV(real, ks_type_complex);
        }

        // add 1i * imag

        if (real->type == ks_type_complex) {
            // add together
            real_d -= cimag(((ks_complex)imag)->val);
            imag_d += creal(((ks_complex)imag)->val);
        } else if (imag->type == ks_type_float) {
            imag_d += ((ks_float)imag)->val;
        } else if (imag->type == ks_type_int) {
            imag_d += ((ks_int)imag)->val;
        } else {
            KS_ERR_CONV(imag, ks_type_complex);
        }

        // now, construct it
        return (ks_obj)ks_complex_new(real_d + I * imag_d);

    }

};


// complex.__str__(self) -> convert to a string
static KS_TFUNC(complex, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_complex self = (ks_complex)args[0];
    KS_REQ_TYPE(self, ks_type_complex, "self");


    char cstr[260];
    snprintf(cstr, 255, "(%.9lf", creal(self->val));

    int len = strlen(cstr);
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


// standard cases (i.e. TL <op> TR)
// each operand is converted to a complex first
// use 'vL' and 'vR' as arguments, assign result to 'vRes'
#define T_BOP_STDCASE(...) { \
    if (L->type == ks_type_complex) { \
        vL = ((ks_complex)L)->val; \
    } else if (L->type == ks_type_float) { \
        vL = ((ks_float)L)->val; \
    } else if (L->type == ks_type_int) { \
        vL = ((ks_int)L)->val; \
    } else { \
        goto _err; \
    } \
    if (R->type == ks_type_complex) { \
        vR = ((ks_complex)R)->val; \
    } else if (R->type == ks_type_float) { \
        vR = ((ks_float)R)->val; \
    } else if (R->type == ks_type_int) { \
        vR = ((ks_int)R)->val; \
    } else { \
        goto _err; \
    } \
    { __VA_ARGS__; }; \
    return (ks_obj)ks_complex_new(vRes); \
    _err: ; \
}

// complex.__add__(L, R) -> add 2 numbers
static KS_TFUNC(complex, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double complex vL, vR, vRes;


    T_BOP_STDCASE(vRes = vL + vR);


    KS_ERR_BOP_UNDEF("+", L, R);
};

// complex.__sub__(L, R) -> subtract 2 numbers
static KS_TFUNC(complex, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double complex vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL - vR);

    KS_ERR_BOP_UNDEF("-", L, R);
};


// complex.__mul__(L, R) -> multiply 2 numbers
static KS_TFUNC(complex, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double complex vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL * vR);

    KS_ERR_BOP_UNDEF("*", L, R);
};

// complex.__div__(L, R) -> divide 2 numbers
static KS_TFUNC(complex, div) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double complex vL, vR, vRes;

    T_BOP_STDCASE(vRes = vL / vR);

    KS_ERR_BOP_UNDEF("/", L, R);
};

/*
// complex.__mod__(L, R) -> modular
static KS_TFUNC(complex, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double complex vL, vR, vRes;

    T_BOP_STDCASE(vRes = fmod(vL, vR));

    KS_ERR_BOP_UNDEF("%", L, R);
};
*/

// complex.__pow__(L, R) -> exponent
static KS_TFUNC(complex, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double complex vL, vR, vRes;

    T_BOP_STDCASE(vRes = cpow(vL, vR));

    KS_ERR_BOP_UNDEF("**", L, R);
};



// complex.__eq__(L, R) -> cmp 2 floats
static KS_TFUNC(complex, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_STDCASE(vRes = vL == vR; return KSO_BOOL(vRes); );

    KS_ERR_BOP_UNDEF("==", L, R);
};


// complex.__ne__(L, R) -> cmp 2 floats
static KS_TFUNC(complex, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];
    double vL, vR;
    bool vRes;

    T_BOP_STDCASE(vRes = vL != vR; return KSO_BOOL(vRes); );

    KS_ERR_BOP_UNDEF("!=", L, R);
};


// complex.__neg__(V) -> negative float
static KS_TFUNC(complex, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_complex) {
        return (ks_obj)ks_complex_new(-((ks_complex)V)->val);
    }

    KS_ERR_UOP_UNDEF("-", V);
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
        if (val->type == ks_type_int) {
            val_d = ((ks_int)val)->val;
        } else if (val->type == ks_type_float) {
            val_d = ((ks_float)val)->val;
        } else {
            KS_REQ_TYPE(val, ks_type_float, "val");
        }

        // set
        self->val = val_d +  I * cimag(self->val);

        return KSO_NONE;
    } else if (attr->len == 4 && strncmp(attr->chr, "imag", 4) == 0) {
        double val_d = 0.0;
        if (val->type == ks_type_int) {
            val_d = ((ks_int)val)->val;
        } else if (val->type == ks_type_float) {
            val_d = ((ks_float)val)->val;
        } else {
            KS_REQ_TYPE(val, ks_type_float, "val");
        }

        // set
        self->val = creal(self->val) +  I * val_d;

        return KSO_NONE;
    }

    KS_ERR_ATTR(self, attr);
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
 
        {"__add__", (ks_obj)ks_cfunc_new2(complex_add_, "complex.__add__(L, R)")},
        {"__sub__", (ks_obj)ks_cfunc_new2(complex_sub_, "complex.__sub__(L, R)")},
        {"__mul__", (ks_obj)ks_cfunc_new2(complex_mul_, "complex.__mul__(L, R)")},
        {"__div__", (ks_obj)ks_cfunc_new2(complex_div_, "complex.__div__(L, R)")},
        {"__pow__", (ks_obj)ks_cfunc_new2(complex_pow_, "complex.__pow__(L, R)")},
 
        {"__eq__", (ks_obj)ks_cfunc_new2(complex_eq_, "complex.__eq__(L, R)")},
        {"__ne__", (ks_obj)ks_cfunc_new2(complex_ne_, "complex.__ne__(L, R)")},

        {"__neg__", (ks_obj)ks_cfunc_new2(complex_neg_, "complex.__neg__(self)")},
        {"__sqig__", (ks_obj)ks_cfunc_new2(complex_sqig_, "complex.__sqig__(self)")},

        {"i", (ks_obj)ks_complex_new(I)},

        {NULL, NULL}   
    });



}

