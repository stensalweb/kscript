/* types/int.c - kscript's basic integer implementation (signed 64 bit) 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_int);

// create a kscript int from a C-style int
ks_int ks_int_new(int64_t val) {
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // initialize type-specific things
    self->val = val;

    return self;
}

/* member functions */


// int.__str__(self) -> free an int object
static KS_TFUNC(int, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    // print it out
    char cstr[256];
    snprintf(cstr, 255, "%lli", (long long int)self->val);

    return (ks_obj)ks_str_new(cstr);

};

// int.__free__(self) -> free an int object
static KS_TFUNC(int, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// int.__add__(L, R) -> add 2 integers
static KS_TFUNC(int, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val + ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("+", L, R);
};

// int.__sub__(L, R) -> subtract 2 integers
static KS_TFUNC(int, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val - ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("-", L, R);
};

// int.__mul__(L, R) -> multiply 2 integers
static KS_TFUNC(int, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val * ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("*", L, R);
};

// int.__div__(L, R) -> divide 2 integers
static KS_TFUNC(int, div) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val / ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("/", L, R);
};

// int.__mod__(L, R) -> modulo 2 integers
static KS_TFUNC(int, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        int64_t res = ((ks_int)L)->val % ((ks_int)R)->val;
        // ensure it is above 0
        if (res < 0) res += ((ks_int)R)->val;
        return (ks_obj)ks_int_new(res);
    }

    KS_ERR_BOP_UNDEF("%", L, R);
};


// int.__pow__(L, R) -> exponentiate 2 integers
static KS_TFUNC(int, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        int64_t Li = ((ks_int)L)->val, Ri = ((ks_int)R)->val;
        // X^0 == 1
        if (Ri == 0) return (ks_obj)ks_int_new(1);
        // 0^X == 0 (negative X's return 0s too)
        if (Li == 0) return (ks_obj)ks_int_new(0);
        // X^-n == 0, for integers
        if (Ri < 0) return (ks_obj)ks_int_new(0);
        // X^1==X
        if (Ri == 1) return KS_NEWREF(L);

        // the sign to be applied
        bool neg = (Li < 0 && Ri & 1 == 1);
        if (Li < 0) Li = -Li;

        // now, Ri>1 and Li is positive, with sign extracted

        // now, calculate it
        int64_t res = 1;

        while (Ri != 0) {
            if (Ri & 1) res *= Li;
            Ri >>= 1;
            Li *= Li;
        }

        return (ks_obj)ks_int_new(neg ? -res : res);
    }

    KS_ERR_BOP_UNDEF("**", L, R);
};

// int.__lt__(L, R) -> cmp 2 integers
static KS_TFUNC(int, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return KSO_BOOL(((ks_int)L)->val < ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("<", L, R);
};

// int.__le__(L, R) -> cmp 2 integers
static KS_TFUNC(int, le) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return KSO_BOOL(((ks_int)L)->val <= ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("<=", L, R);
};

// int.__gt__(L, R) -> cmp 2 integers
static KS_TFUNC(int, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return KSO_BOOL(((ks_int)L)->val > ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF(">", L, R);
};

// int.__ge__(L, R) -> cmp 2 integers
static KS_TFUNC(int, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return KSO_BOOL(((ks_int)L)->val >= ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF(">=", L, R);
};

// int.__eq__(L, R) -> cmp 2 integers
static KS_TFUNC(int, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return KSO_BOOL(((ks_int)L)->val == ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("==", L, R);
};

// int.__ne__(L, R) -> cmp 2 integers
static KS_TFUNC(int, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return KSO_BOOL(((ks_int)L)->val != ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("!=", L, R);
};



// int.__neg__(V) -> negative int
static KS_TFUNC(int, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_int) {
        return (ks_obj)ks_int_new(-((ks_int)V)->val);
    }

    KS_ERR_UOP_UNDEF("-", V);
};




// int.__sqig__(V) -> sqig int
static KS_TFUNC(int, sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_int) {
        return (ks_obj)ks_int_new(~((ks_int)V)->val);
    }

    KS_ERR_UOP_UNDEF("~", V);
};


// initialize int type
void ks_type_int_init() {
    KS_INIT_TYPE_OBJ(ks_type_int, "int");

    ks_type_set_cn(ks_type_int, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(int_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(int_str_)},
        
        {"__free__", (ks_obj)ks_cfunc_new(int_free_)},
 
        {"__add__", (ks_obj)ks_cfunc_new(int_add_)},
        {"__sub__", (ks_obj)ks_cfunc_new(int_sub_)},
        {"__mul__", (ks_obj)ks_cfunc_new(int_mul_)},
        {"__div__", (ks_obj)ks_cfunc_new(int_div_)},
        {"__mod__", (ks_obj)ks_cfunc_new(int_mod_)},
        {"__pow__", (ks_obj)ks_cfunc_new(int_pow_)},
 
        {"__lt__", (ks_obj)ks_cfunc_new(int_lt_)},
        {"__le__", (ks_obj)ks_cfunc_new(int_le_)},
        {"__gt__", (ks_obj)ks_cfunc_new(int_gt_)},
        {"__ge__", (ks_obj)ks_cfunc_new(int_ge_)},
        {"__eq__", (ks_obj)ks_cfunc_new(int_eq_)},
        {"__ne__", (ks_obj)ks_cfunc_new(int_ne_)},

        {"__neg__", (ks_obj)ks_cfunc_new(int_neg_)},
        {"__sqig__", (ks_obj)ks_cfunc_new(int_sqig_)},
 
        {NULL, NULL}   
    });
}

