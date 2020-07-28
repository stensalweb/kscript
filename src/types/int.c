/* int.c - implementation of the 'int' type in kscript
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// All integers with abs(x) < KS_SMALL_INT_MAX are 'small' integers and kept as an interned list
#define KS_SMALL_INT_MAX 255

// global singletons of small integers
static struct ks_int_s KS_SMALL_INTS[2 * KS_SMALL_INT_MAX + 1];


// Construct a new 'int' object
// NOTE: Returns new reference, or NULL if an error was thrown
ks_int ks_int_new(int64_t val) {
    if (val <= KS_SMALL_INT_MAX && val >= -KS_SMALL_INT_MAX) return &KS_SMALL_INTS[val + KS_SMALL_INT_MAX];
    
    // now, actually create a value
    ks_int self = KS_ALLOC_OBJ(ks_int);

    KS_INIT_OBJ(self, ks_T_int);

    // int64_t's can always fit
    self->isLong = false;

    self->v64 = val;

    return self;
}


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

// Create a kscript int from a string in a given base
ks_int ks_int_new_s(char* str, int base) {
    // calculate string length    
    int len = strlen(str);

    // try to calculate in a 64 bit integer
    int64_t v64 = 0;

    // check for any signs
    bool isNeg = *str == '-';
    if (isNeg || *str == '+') {
        str++;
        len--;
    }

    int i = 0;

    // parse out main value
    while (i < len) {
        int dig = my_getdig(str[i]);
        // check for invalid/out of range digit
        if (dig < 0 || dig >= base) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: %s", base, str);

        int64_t old_v64 = v64;
        // calculate new value in 64 bits
        v64 = base * v64 + dig;

        if (v64 < old_v64) {
            // overflow
            goto do_mpz_str;
        }
        i++;
    }

    // we construct via v64 methods
    return ks_int_new(isNeg ? -v64 : v64);

    do_mpz_str:;

    // now, we need to handle via MPZ initialization method

    // allocate a new integer
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_T_int);

    // must be a long integer
    self->isLong = true;

    // initialize the mpz integer
    mpz_init(self->vz);

    if (mpz_set_str(self->vz, str, base) != 0) {
        // there was a problem
        KS_DECREF(self);
        return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: %s", base, str);
    }

    return self;
}


// get a 64 bit value from an MPZ,
// return whether it was successful (and if not, throw an error)
static bool my_mpz_get_i64(mpz_t self, int64_t* val) {
    #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
    if (mpz_fits_slong_p(self)) {
        *val = (int64_t)mpz_get_si(self);
        return true;
    } else {
        ks_throw(ks_T_MathError, "'int' was too large to convert to signed 64 bit");
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
            ks_throw(ks_T_MathError, "'int' was too large to convert to signed 64 bit");
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


// Create a new integer from an MPZ
ks_int ks_int_new_mpz(mpz_t val) {
    int64_t v64;
    if (my_mpz_get_i64(val, &v64)) {
        return ks_int_new(v64);
    } else {
        ks_catch_ignore();

        ks_int self = KS_ALLOC_OBJ(ks_int);
        KS_INIT_OBJ(self, ks_T_int);

        // must be a long integer
        self->isLong = true;

        // now initialize and set it
        mpz_init(self->vz);
        
        mpz_set(self->vz, val);

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
        KS_INIT_OBJ(self, ks_T_int);

        // must be a long integer
        self->isLong = true;


        // now, we are allowed to claim 'val', and just use it as the mpz
        mpz_init(self->vz);
        mpz_set(self->vz, val);
        mpz_clear(val);


        return self;

    }
}

// return sign of self
int ks_int_sgn(ks_int self) {
    if (self->isLong) {
        return mpz_sgn(self->vz);
    } else {
        return self->v64 == 0 ? 0 : (self->v64 > 0 ? 1 : -1);
    }
}


// Compare 2 integers
int ks_int_cmp(ks_int L, ks_int R) {

    if (L->isLong) {
        if (R->isLong) {
            return mpz_cmp(L->vz, R->vz);
        } else {
            #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
            return mpz_cmp_si(L->vz, R->v64);
            #else
            if (R->v64 == (signed long)R->v64) {
                return mpz_cmp_si(L->vz, R->v64);
            } else {
                mpz_t tmp;
                mpz_init(tmp);
                my_mpz_set_i64(tmp, R->v64);
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
            return L->v64 == R->v64 ? 0 : (L->v64 > R->v64 ? 1 : -1);
        }
    }
}


// Compare to a C-style integer
int ks_int_cmp_c(ks_int L, int64_t R) {
    if (L->isLong) {
        #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
        return mpz_cmp_si(L->vz, R);
        #else
        if (R == (signed long)R) {
            return mpz_cmp_si(L->vz, R);
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
        return L->v64 == 0 ? 0 : (L->v64 > R ? 1 : -1);
    }
}



// int.__new__(typ, obj, base=none) -> convert 'obj' to a int
static KS_TFUNC(int, new) {
    ks_type typ;
    ks_obj obj;
    int64_t base = 10;
    if (!ks_getargs(n_args, args, "typ:* obj ?base:i64", &typ, ks_T_type, &obj, &base)) return NULL;
    if (!ks_type_issub(typ, ks_T_int)) ks_throw(ks_T_InternalError, "Constructor for type '%S' called given typ as '%S' (not a sub-type!)", ks_T_int, typ);

    if (n_args >= 3) {
        // a specific base is requested, so it must be a string
        if (obj->type != ks_T_str) return ks_throw(ks_T_ArgError, "When given parameter 'base', 'obj' must be a string!");
        if (base < 2 || base > MAX_BASE) return ks_throw(ks_T_ArgError, "Invalid base (%l), expected between 2 and %i", base, (int)MAX_BASE);
    }

    if (obj->type == ks_T_int) {
        return KS_NEWREF(obj);
    } else if (obj->type == ks_T_bool) {
        return (ks_obj)ks_int_new(obj == KSO_TRUE ? 1 : 0);
    } else if (obj->type == ks_T_float) {
        return (ks_obj)ks_int_new(round(((ks_float)obj)->val));
    } else if (obj->type == ks_T_complex) {
        return (ks_obj)ks_int_new(round(((ks_complex)obj)->val));
    } else if (obj->type == ks_T_str) {
        return (ks_obj)ks_int_new_s(((ks_str)obj)->chr, base);
    //} else if (ks_type_issub(obj->type, ks_type_Enum)) {
    //    return (ks_obj)ks_int_new(((ks_Enum)obj)->enum_idx);
    } else if (obj->type->__int__ != NULL) {
        return ks_obj_call(obj->type->__int__, n_args, args);
    }

    // error
    KS_THROW_TYPE_ERR(obj, ks_T_int);

}



// int.__str__(self) - to string
static KS_TFUNC(int, str) {
    ks_int self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_int)) return NULL;


    int base = 10;

    if (self->isLong) {

        // do mpz to string
        size_t total_size = 16 + mpz_sizeinbase(self->vz, base);

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
            return ks_throw(ks_T_ArgError, "Invalid base '%i' for int->str conversion", base);
        }

        // use GMP to get string
        mpz_get_str(&tmp[i], base, self->vz);

        ks_str res = ks_str_new_c(tmp, -1);
        ks_free(tmp);

        return (ks_obj)res;

    } else {

        char tmp[256];
        int ct = snprintf(tmp, sizeof(tmp) - 1, "%lli", (long long int)self->v64);

        return (ks_obj)ks_str_new_c(tmp, ct);

    }

}


// int.__free__(self) - free string
static KS_TFUNC(int, free) {
    ks_int self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_int)) return NULL;

    // check for global singletons
    if (self >= &KS_SMALL_INTS[0] && self <= &KS_SMALL_INTS[2 * KS_SMALL_INT_MAX + 1]) {
        self->refcnt = KS_REFS_INF;
        return KSO_NONE;
    }

    // clear MPZ var if it is there
    if (self->isLong) {
        mpz_clear(self->vz);
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// operators
KST_NUM_OPFS(int)


/* export */

KS_TYPE_DECLFWD(ks_T_int);

void ks_init_T_int() {

    // initialize singletons
    int64_t i;
    for (i = -KS_SMALL_INT_MAX; i <= KS_SMALL_INT_MAX; ++i) {
        ks_int self = &KS_SMALL_INTS[KS_SMALL_INT_MAX + i];
        KS_INIT_OBJ(self, ks_T_int);

        self->isLong = false;
        self->v64 = i;

    }
    
    ks_type_init_c(ks_T_int, "int", ks_T_obj, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c(int_new_, "int.__new__(typ, obj, base=none)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c(int_str_, "int.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(int_free_, "int.__free__(self)")},

        KST_NUM_OPKVS(int)

        
    ));

    ks_T_int->flags |= KS_TYPE_FLAGS_EQSS;

}
