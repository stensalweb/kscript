/* types/int.c - kscript's basic integer implementation (signed 64 bit) 
 *
 * Most of the actual math is based in `numbers.c`
 * 
 * One of the biggest issues with general support for normal/long integers is the fact that GMP
 *   functions with `_si` supported `signed long`, which is only 32 bits on windows.
 * So, to remedy this, we define a function (only for use in this file & 'numbers.c') that is called `my_seti64`
 *   and essentially does mpz_set_si should do
 * However, some functions are much faster with a normal signed int, or unsigned int, so we also have macros
 * 
 *  
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


/** UTIL FUNCTIONS **/

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

// set an MPZ to a 64 bit value
static void my_mpz_set_i64(mpz_t self, int64_t val) {
    #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
    mpz_set_si(self, val);
    #else
    if (val == (signed long)val) {
        mpz_set_si(self->v_mpz, (signed long)val);
    } else {
        // import
        // TODO: check endianness?
        mpz_import(self->v_mpz, 1, 1, sizeof(val), 0, 0, &val);
    }
    #endif
}

// get a 64 bit value from an MPZ,
// return whether it was successful (and if not, throw an error)
static bool my_mpz_get_i64(mpz_t self, int64_t* val) {
    #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
    if (mpz_fits_slong_p(self)) {
        *val = (int64_t)mpz_get_si(self);
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "'int' was too large to convert to signed 64 bit");
        return false;
    }
    #else

    if (mpz_fits_slong_p(self)) {
        *val = (int64_t)mpz_get_si(self);
        return true;
    } else {

        size_t nbits = mpz_sizeinbase(self, 2);

        if (nbits >= 63) {
            // can't fit
            ks_throw_fmt(ks_type_MathError, "'int' was too large to convert to signed 64 bit");
            return false;
        } else {

            
            // temporary unsigned variables
            uint64_t u, u_abs;

            // export the binary interface
            mpz_export(&u, NULL, 1, sizeof(u), 0, 0, self);
            // get absolute value
            u_abs = u < 0 ? -u : u;

            // NOTE: mpz_export treats values as unsigned so we need to analyze the sign ourselves:
            if (mpz_sgn(op) < 0) {
                *val = -(int64_t)u_abs;
            } else {
                *val = (int64_t)u_abs;
            }
            return true;

        }
    }
    #endif
}


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

    // allocate a new integer
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // not a long integer
    self->isLong = false;

    // set the 64 bit integer to that
    self->v_i64 = val;

    return self;
}

// create a kscript int from a string in a given base
ks_int ks_int_new_s(char* str, int base) {
    // calculate len if len < 0
    int len = strlen(str);

    // try to calculate in a 64 bit integer
    int64_t v64 = 0;

    // extract a sign from the integer
    bool isSigned = *str == '-';
    if (isSigned) {
        str++;
        len--;
    }

    int i = 0;

    // parse out main signifier
    while (i < len) {
        int dig = my_getdig(str[i]);
        // check for invalid/out of range digit
        if (dig < 0 || dig >= base) return ks_throw_fmt(ks_type_ArgError, "Invalid format for base %i integer: %s", base, str);

        int64_t old_v64 = v64;
        // calculate new value in 64 bits
        v64 = base * v64 + dig;

        if (v64 < old_v64) {
            // overflow
            goto do_mpz_str;
        }
        i++;
    }

    return ks_int_new(v64);

    do_mpz_str:;

    // now, we need to handle via MPZ initialization method


    // allocate a new integer
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // must be a long integer
    self->isLong = true;

    // initialize the mpz integer
    mpz_init(self->v_mpz);

    if (mpz_set_str(self->v_mpz, str, base) != 0) {
        // there was a problem
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_ArgError, "Invalid format for base %i integer: %s", base, str);
    }

    return self;
}

// Create a new integer from an MPZ
ks_int ks_int_new_mpz(mpz_t val) {
    int64_t v64;
    if (my_mpz_get_i64(val, &v64)) {
        return ks_int_new(v64);
    } else {
        ks_catch_ignore();

        ks_int self = KS_ALLOC_OBJ(ks_int);
        KS_INIT_OBJ(self, ks_type_int);

        // must be a long integer
        self->isLong = true;

        // now initialize and set it
        mpz_init(self->v_mpz);
        
        mpz_set(self->v_mpz, val);

        return self;

    }
}

// Create a new integer from an MPZ, that can use the `val` and clear if it
//   is not required
ks_int ks_int_new_mpz_n(mpz_t val) {
    int64_t v64;

    if (my_mpz_get_i64(val, &v64)) {
        // we must clear val, since we own the reference
        mpz_clear(val);
        return ks_int_new(v64);
    } else {
        ks_catch_ignore();

        ks_int self = KS_ALLOC_OBJ(ks_int);
        KS_INIT_OBJ(self, ks_type_int);

        // must be a long integer
        self->isLong = true;


        // now, we are allowed to claim 'val', and just use it as the mpz
        mpz_init(self->v_mpz);
        mpz_set(self->v_mpz, val);
        mpz_clear(val);


        return self;

    }
}


// return sign of self
int ks_int_sgn(ks_int self) {
    if (self->isLong) {
        return mpz_sgn(self->v_mpz);
    } else {
        return self->v_i64 == 0 ? 0 : (self->v_i64 > 0 ? 1 : -1);
    }
}


// Compare 2 integers
int ks_int_cmp(ks_int L, ks_int R) {

    if (L->isLong) {
        if (R->isLong) {
            return mpz_cmp(L->v_mpz, R->v_mpz);
        } else {
            #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
            return mpz_cmp_si(L->v_mpz, R->v_i64);
            #else
            if (R->v_i64 == (signed long)R->v_i64) {
                return mpz_cmp_si(L->v_mpz, R->v_i64);
            } else {
                mpz_t tmp;
                mpz_init(tmp);
                my_mpz_set_i64(tmp, R->v_i64);
                int res = mpz_cmp(L, tmp);
                mpz_clear(tmp);
                return res;
            }
            #endif
        }
    } else {
        if (R->isLong) {
            // reverse it to be handled by above case
            return -ks_int_cmp(R, L);
        } else {
            return L->v_i64 == R->v_i64 ? 0 : (L->v_i64 > R->v_i64 ? 1 : -1);
        }
    }
}


// Compare to a C-style integer
int ks_int_cmp_c(ks_int L, int64_t R) {
    if (L->isLong) {
        #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
        return mpz_cmp_si(L->v_mpz, R);
        #else
        if (R == (signed long)R) {
            return mpz_cmp_si(L->v_mpz, R);
        } else {
            mpz_t tmp;
            mpz_init(tmp);
            my_mpz_set_i64(tmp, R);
            int res = mpz_cmp(L, tmp);
            mpz_clear(tmp);
            return res;
        }
        #endif
    } else {
        return L->v_i64 == 0 ? 0 : (L->v_i64 > R ? 1 : -1);
    }
}


/* member functions */


// int.__new__(obj, base=10) -> convert 'obj' to a int
static KS_TFUNC(int, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    // calculate the base to use
    int64_t base = 10;

    if (n_args >= 2) {
        ks_int base_o = (ks_int)args[1];
        KS_REQ_TYPE(base_o, ks_type_int, "base");
        if (base_o->isLong) {
            if (!my_mpz_get_i64(base_o->v_mpz, &base)) {
                return NULL;
            }
        } else {
            base = base_o->v_i64;
        }
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
        return (ks_obj)ks_int_new_s(((ks_str)obj)->chr, base);
    } else if (ks_type_issub(obj->type, ks_type_Enum)) {
        return (ks_obj)ks_int_new(((ks_Enum)obj)->enum_idx);
    } else if (obj->type->__int__ != NULL) {
        return ks_call(obj->type->__int__, n_args, args);
    } else {
        KS_ERR_CONV(obj, ks_type_int);
    }
};


// int.__str__(self, base=10) -> convert to a string
static KS_TFUNC(int, str) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    // calculate the base to use
    int64_t base = 10;

    if (n_args >= 2) {
        ks_int base_o = (ks_int)args[1];
        KS_REQ_TYPE(base_o, ks_type_int, "base");
        if (base_o->isLong) {
            if (!my_mpz_get_i64(base_o->v_mpz, &base)) {
                return NULL;
            }
        } else {
            base = base_o->v_i64;
        }
        if (base < 2 || base > MAX_BASE) return ks_throw_fmt(ks_type_ArgError, "Invalid base '%i' (only handles base 2 through %i)", base, MAX_BASE);
    }

    if (self->isLong) {

        size_t total_size = 16 + mpz_sizeinbase(self->v_mpz, base);

        // temporary buffer
        char* tmp = ks_malloc(total_size);
        int i = 0;


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

        // use GMP to get string
        mpz_get_str(&tmp[i], base, self->v_mpz);

        ks_str res = ks_str_new(tmp);
        ks_free(tmp);

        return (ks_obj)res;

    } else {
        // use simplified bit for C-style ints
        // temporary buffer
        char tmp[256];
        

        int64_t _val = self->v_i64;
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
    }
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

    // clear MPZ var if it is there
    if (self->isLong) {
        mpz_clear(self->v_mpz);
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// int.__hash__(self) -> return hash of object
static KS_TFUNC(int, hash) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    ks_hash_t res;
    if (!ks_num_hash((ks_obj)self, &res)) return NULL;

    return (ks_obj)ks_int_new(res);
};


/** MATH FUNCS **/

static KS_TFUNC(int, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj ret = ks_num_add(args[0], args[1]);
    return ret;
};
static KS_TFUNC(int, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_sub(args[0], args[1]);
};
static KS_TFUNC(int, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_mul(args[0], args[1]);
};
static KS_TFUNC(int, div) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_div(args[0], args[1]);
};
static KS_TFUNC(int, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_mod(args[0], args[1]);
};
static KS_TFUNC(int, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_pow(args[0], args[1]);
};

static KS_TFUNC(int, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    return ks_num_neg(args[0]);
};

static KS_TFUNC(int, binor) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_binor(args[0], args[1]);
};
static KS_TFUNC(int, binand) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_binand(args[0], args[1]);
};

static KS_TFUNC(int, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    int res;
    if (!ks_num_cmp(args[0], args[1], &res)) return NULL;
    return (ks_obj)ks_int_new(res);
};

static KS_TFUNC(int, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_lt(args[0], args[1]);
};
static KS_TFUNC(int, le) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_le(args[0], args[1]);
};
static KS_TFUNC(int, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_gt(args[0], args[1]);
};
static KS_TFUNC(int, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_ge(args[0], args[1]);
};
static KS_TFUNC(int, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    bool res;
    if (!ks_num_eq(args[0], args[1], &res)) return NULL;
    return KSO_BOOL(res);
};
static KS_TFUNC(int, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    bool res;
    if (!ks_num_eq(args[0], args[1], &res)) return NULL;
    return KSO_BOOL(!res);
};


// initialize int type
void ks_type_int_init() {
    KS_INIT_TYPE_OBJ(ks_type_int, "int");

    int i;
    for (i = -KS_SMALL_INT_MAX; i <= KS_SMALL_INT_MAX; ++i) {
        KS_INIT_OBJ(&KS_SMALL_INTS[i + KS_SMALL_INT_MAX], ks_type_int);


        KS_SMALL_INTS[i + KS_SMALL_INT_MAX].isLong = false;
        KS_SMALL_INTS[i + KS_SMALL_INT_MAX].v_i64 = i;
    }


    ks_type_set_cn(ks_type_int, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(int_new_, "int.__new__(obj)")},
        {"__str__", (ks_obj)ks_cfunc_new2(int_str_, "int.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(int_str_, "int.__repr__(self)")},
        
        {"__hash__", (ks_obj)ks_cfunc_new2(int_hash_, "int.__hash__(self)")},
        
        {"__free__", (ks_obj)ks_cfunc_new2(int_free_, "int.__free__(self)")},

        {"__add__",       (ks_obj)ks_cfunc_new2(int_add_, "int.__add__(L, R)")},
        {"__sub__",       (ks_obj)ks_cfunc_new2(int_sub_, "int.__sub__(L, R)")},
        {"__mul__",       (ks_obj)ks_cfunc_new2(int_mul_, "int.__mul__(L, R)")},
        {"__div__",       (ks_obj)ks_cfunc_new2(int_div_, "int.__div__(L, R)")},
        {"__mod__",       (ks_obj)ks_cfunc_new2(int_mod_, "int.__mod__(L, R)")},
        {"__pow__",       (ks_obj)ks_cfunc_new2(int_pow_, "int.__pow__(L, R)")},

        {"__neg__",       (ks_obj)ks_cfunc_new2(int_neg_, "int.__neg__(L)")},

        {"__cmp__",       (ks_obj)ks_cfunc_new2(int_cmp_, "int.__cmp__(L, R)")},
        {"__lt__",        (ks_obj)ks_cfunc_new2(int_lt_, "int.__lt__(L, R)")},
        {"__le__",        (ks_obj)ks_cfunc_new2(int_le_, "int.__le__(L, R)")},
        {"__gt__",        (ks_obj)ks_cfunc_new2(int_gt_, "int.__gt__(L, R)")},
        {"__ge__",        (ks_obj)ks_cfunc_new2(int_ge_, "int.__ge__(L, R)")},
        {"__eq__",        (ks_obj)ks_cfunc_new2(int_eq_, "int.__eq__(L, R)")},
        {"__ne__",        (ks_obj)ks_cfunc_new2(int_ne_, "int.__ne__(L, R)")},
 /*

        {"__neg__", (ks_obj)ks_cfunc_new2(int_neg_, "int.__neg__(self)")},
        {"__sqig__", (ks_obj)ks_cfunc_new2(int_sqig_, "int.__sqig__(self)")},
 */


        {"__binor__",       (ks_obj)ks_cfunc_new2(int_binor_, "int.__binor__(L, R)")},
        {"__binand__",     (ks_obj)ks_cfunc_new2(int_binand_, "int.__binand__(L, R)")},
 

        {NULL, NULL}   
    });
}

