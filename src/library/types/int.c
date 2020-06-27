/* types/int.c - kscript's basic integer implementation (signed 64 bit) 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_int);

// global singletons
struct ks_int KS_SMALL_INTS[2 * KS_SMALL_INT_MAX + 1];


// create a kscript int from a C-style int
ks_int ks_int_new(int64_t val) {
    if (val <= KS_SMALL_INT_MAX && val >= -KS_SMALL_INT_MAX) {
        // return singleton
        return (ks_int)KS_NEWREF(&KS_SMALL_INTS[val + KS_SMALL_INT_MAX]);
    }

    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // initialize type-specific things
    self->val = val;

    return self;
}


// convert to 64 bit integer
bool ks_int_geti64(ks_int self, int64_t* out) {
    *out = self->val;
    return true;
}

/* member functions */

// the maximum base supported
#define MAX_BASE (my_getdig('z') + 1)

// Return the digit value (irrespective of base), or -1 if there was a problem
static int my_getdig(char c) {
    if (isdigit(c)) {
        return c - '0';
    } else if (c >= 'a' && c <= 'z') {
        return (c - 'a') + 10;
    } else if (c >= 'A' && c <= 'Z') {
        return (c - 'A') + 10;
    } else {
        // errro: invalid digit
        return -1;
    }
}


// return the character representing the given digit, or NUL if there was a problem
static char my_getdigchar(int dig) {
    if (dig >= 0 && dig < 10) {
        return dig + '0';
    } else if (dig < MAX_BASE) {
        // use lowercase letters
        return (dig - 10) + 'a';
    } else {
        // errro: invalid digit
        return 0;
    }
}

// int.__new__(obj, base=10) -> convert 'obj' to a int
static KS_TFUNC(int, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    int base = 10;

    if (n_args >= 2) {
        ks_int base_o = (ks_int)args[1];
        KS_REQ_TYPE(base_o, ks_type_int, "base");
        base = base_o->val;
        if (base < 2 || base > MAX_BASE) return ks_throw_fmt(ks_type_ArgError, "Invalid base '%i' (only handles base 2 through %i)", base, MAX_BASE);
    }

    ks_obj obj = args[0];
    if (obj->type == ks_type_int) {
        return KS_NEWREF(obj);
    } else if (obj->type == ks_type_float) {
        return (ks_obj)ks_int_new(round(((ks_float)obj)->val));
    } else if (obj->type == ks_type_complex) {
        return (ks_obj)ks_int_new(round(((ks_complex)obj)->val));
    } else if (obj->type == ks_type_str) {
        // TODO:
        // Support other bases

        // read formats:
        // DIGITS+
        // DIGITS+(eE)(+-)?DIGITS+

        char* cstr = ((ks_str)obj)->chr;
        int len = ((ks_str)obj)->len;

        // current value
        int64_t c_val = 0;

        int i = 0;

        // parse out main signifier
        while (i < len) {
            int dig = my_getdig(cstr[i]);
            // check for invalid/out of range digit
            if (dig < 0 || dig >= base) return ks_throw_fmt(ks_type_ArgError, "Invalid format for base %i integer: %R", base, obj);
            c_val = base * c_val + dig;
            i++;
        }

        // parse out the first amount of digits
        while (i < len && isdigit(cstr[i])) {
            c_val = 10 * c_val + (cstr[i] - '0');
            i++;
        }

        // incorrect: extra non-digit character
        if (cstr[i]) return ks_throw_fmt(ks_type_ArgError, "Invalid format for base %i integer: %R", base, obj);


        return (ks_obj)ks_int_new(c_val);
    } else if (ks_type_issub(obj->type, ks_type_Enum)) {
        return (ks_obj)ks_int_new(((ks_Enum)obj)->enum_idx);
    } else {
        KS_ERR_CONV(obj, ks_type_int);
    }
};


// int.__str__(self, base=10) -> convert to a string
static KS_TFUNC(int, str) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    int base = 10;
    if (n_args > 1) {
        ks_int base_o = (ks_int)args[1];
        KS_REQ_TYPE(base_o, ks_type_int, "base");
        base = base_o->val;
        if (base < 2 || base > MAX_BASE) return ks_throw_fmt(ks_type_ArgError, "Invalid base '%i' (only handles base 2 through %i)", base, MAX_BASE);
    }

    // temporary buffer
    char tmp[256];

    int64_t _val = self->val;
    bool doSign = _val < 0;
    uint64_t val = doSign ? -_val : _val;

    int i = 0;

    // add string
    if (doSign) tmp[i++] = '-';

    // add prefix specifier
    if (base == 10) {
        // do nothing
    } else if (base == 16) {
        tmp[i++] = '0';
        tmp[i++] = 'x';
    } else if (base == 8) {
        tmp[i++] = '0';
        tmp[i++] = 'o';
    } else if (base == 2) {
        tmp[i++] = '0';
        tmp[i++] = 'b';
    } else {
        return ks_throw_fmt(ks_type_ArgError, "Invalid base '%i' for int->str conversion", base);
    }


    int startRev = i;

    do {
        char myc = my_getdigchar(val % base);

        if (!myc) return ks_throw_fmt(ks_type_ArgError, "Internal invalid format for base %i integer", base);
        val /= base;

        tmp[i++] = myc;

    } while (i < 250 && val > 0);


    // NUL-terminate
    tmp[i] = '\0';

    int len = i;
    // now, reverse it
    for (i = startRev; 2 * (i - startRev) < len - startRev; ++i) {
        char t = tmp[i];
        tmp[i] = tmp[len - i - 1 + startRev];
        tmp[len - i - 1 + startRev] = t;
    }

    return (ks_obj)ks_str_new_l(tmp, len);

};

// int.__free__(self) -> free an int object
static KS_TFUNC(int, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    // check for global singletons
    if (self >= &KS_SMALL_INTS[0] && self <= &KS_SMALL_INTS[2 * KS_SMALL_INT_MAX + 1]) {
        self->refcnt = 0xFFFF;
        return KSO_NONE;
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// int.__hash__(self) -> hash an integer
static KS_TFUNC(int, hash) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");


    if (self->val == 0) {
        return (ks_obj)ks_int_new(1);
    } else {
        return KS_NEWREF(self);
    }
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
        if (((ks_int)R)->val == 0) {
            return ks_throw_fmt(ks_type_MathError, "Division by 0!");
        }
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


// int.__binor__(L, R) -> bitwise-or 2 integers
static KS_TFUNC(int, binor) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        int64_t res = ((ks_int)L)->val | ((ks_int)R)->val;
        // ensure it is above 0
        if (res < 0) res += ((ks_int)R)->val;
        return (ks_obj)ks_int_new(res);
    }

    KS_ERR_BOP_UNDEF("|", L, R);
};

// int.__binand__(L, R) -> bitwise-or 2 integers
static KS_TFUNC(int, binand) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        int64_t res = ((ks_int)L)->val & ((ks_int)R)->val;
        // ensure it is above 0
        if (res < 0) res += ((ks_int)R)->val;
        return (ks_obj)ks_int_new(res);
    }

    KS_ERR_BOP_UNDEF("&", L, R);
};



// int.__cmp__(L, R) -> cmp 2 integers
static KS_TFUNC(int, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        int64_t res = ((ks_int)L)->val - ((ks_int)R)->val;
        return (ks_obj)ks_int_new(res > 0 ? 1 : (res < 0 ? -1 : 0));
    }

    KS_ERR_BOP_UNDEF("<=>", L, R);
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

    int i;
    for (i = -KS_SMALL_INT_MAX; i <= KS_SMALL_INT_MAX; ++i) {
        KS_INIT_OBJ(&KS_SMALL_INTS[i + KS_SMALL_INT_MAX], ks_type_int);

        KS_SMALL_INTS[i + KS_SMALL_INT_MAX].val = i;
    }


    ks_type_set_cn(ks_type_int, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(int_new_, "int.__new__(obj)")},
        {"__str__", (ks_obj)ks_cfunc_new2(int_str_, "int.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(int_str_, "int.__repr__(self)")},
        
        {"__hash__", (ks_obj)ks_cfunc_new2(int_hash_, "int.__hash__(self)")},
        
        {"__free__", (ks_obj)ks_cfunc_new2(int_free_, "int.__free__(self)")},
 
        {"__add__", (ks_obj)ks_cfunc_new2(int_add_, "int.__add__(L, R)")},
        {"__sub__", (ks_obj)ks_cfunc_new2(int_sub_, "int.__sub__(L, R)")},
        {"__mul__", (ks_obj)ks_cfunc_new2(int_mul_, "int.__mul__(L, R)")},
        {"__div__", (ks_obj)ks_cfunc_new2(int_div_, "int.__div__(L, R)")},
        {"__mod__", (ks_obj)ks_cfunc_new2(int_mod_, "int.__mod__(L, R)")},
        {"__pow__", (ks_obj)ks_cfunc_new2(int_pow_, "int.__pow__(L, R)")},

        {"__binor__", (ks_obj)ks_cfunc_new2(int_binor_, "int.__binor__(L, R)")},
        {"__binand__", (ks_obj)ks_cfunc_new2(int_binand_, "int.__binand__(L, R)")},
 
        {"__cmp__", (ks_obj)ks_cfunc_new2(int_cmp_, "int.__cmp__(L, R)")},
        {"__lt__", (ks_obj)ks_cfunc_new2(int_lt_, "int.__lt__(L, R)")},
        {"__le__", (ks_obj)ks_cfunc_new2(int_le_, "int.__le__(L, R)")},
        {"__gt__", (ks_obj)ks_cfunc_new2(int_gt_, "int.__gt__(L, R)")},
        {"__ge__", (ks_obj)ks_cfunc_new2(int_ge_, "int.__ge__(L, R)")},
        {"__eq__", (ks_obj)ks_cfunc_new2(int_eq_, "int.__eq__(L, R)")},
        {"__ne__", (ks_obj)ks_cfunc_new2(int_ne_, "int.__ne__(L, R)")},

        {"__neg__", (ks_obj)ks_cfunc_new2(int_neg_, "int.__neg__(self)")},
        {"__sqig__", (ks_obj)ks_cfunc_new2(int_sqig_, "int.__sqig__(self)")},
 
        {NULL, NULL}   
    });
}

