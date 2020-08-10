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


// Create a kscript int from a string in a given base
ks_int ks_int_new_s(char* str, int base) {
    ks_obj parsed = ks_num_parse(str, -1, base);
    if (!parsed) return NULL;
    if (parsed->type != ks_T_int) {
        KS_DECREF(parsed);
        return (ks_int)ks_throw(ks_T_ArgError, "Invalid integer format: '%*s'", strlen(str));
    } else {
        return (ks_int)parsed;
    }
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
        return L->v64 == R ? 0 : (L->v64 > R ? 1 : -1);
    }
}


// Compute and return the hash of an integer
ks_hash_t ks_int_hash(ks_int self) {
    if (!self->isLong) return (ks_hash_t)self->v64;
    else {
        ks_ssize_t sz = self->vz->_mp_size;
        if (sz < 0) sz = -sz;

        // hash the bytes
        return ks_hash_bytes((const uint8_t*)self->vz->_mp_d, sz * sizeof(*self->vz->_mp_d));
    }
}

// int.__new__(obj, mode=none) -> convert 'obj' to a int
static KS_TFUNC(int, new) {
    ks_obj obj;
    ks_obj mode = KSO_NONE;
    KS_GETARGS("obj ?mode", &obj, &mode)

    if (mode != KSO_NONE) {
        if (obj->type != ks_T_str) return ks_throw(ks_T_ArgError, "When given parameter 'mode', 'obj' must be a string!");
        // a specific base is requested, so it must be a string
        if (mode->type == ks_T_str) {
            if (ks_str_eq_c((ks_str)mode, "roman", 5)) {
                return (ks_obj)ks_int_new_s(((ks_str)obj)->chr, KS_BASE_ROMAN);
            }

        } else if (ks_num_is_integral(mode)) {
            int64_t base;
            if (!ks_num_get_int64(mode, &base)) return NULL;

            return (ks_obj)ks_int_new_s(((ks_str)obj)->chr, base);
        }
        //if (base < 2 || base > MAX_BASE) return ks_throw(ks_T_ArgError, "Invalid base (%l), expected between 2 and %i", base, (int)MAX_BASE);

        return ks_throw(ks_T_ArgError, "Unknown mode: %R", mode);
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
        return (ks_obj)ks_int_new_s(((ks_str)obj)->chr, 0);
    //} else if (ks_type_issub(obj->type, ks_type_Enum)) {
    //    return (ks_obj)ks_int_new(((ks_Enum)obj)->enum_idx);
    } else if (obj->type->__int__ != NULL) {
        return ks_obj_call(obj->type->__int__, n_args-1, args+1);
    }

    // error
    KS_THROW_TYPE_ERR(obj, ks_T_int);
}

// int.__fmt__(self, fstr) - format an integer according to a format string
// ints have the following syntaxes:
// %[]
static KS_TFUNC(int, fmt) {
    ks_int self;
    ks_str fstr;
    KS_GETARGS("self:* fstr:*", &self, ks_T_int, &fstr, ks_T_str);

    // ensure it's ASCII
    if (!KS_STR_ISASCII(fstr)) return ks_throw(ks_T_ArgError, "Invalid format string: %R", fstr);

    // format string in C
    const char* fstrc = fstr->chr;


    // sign component, whether '+', or ' ' (\0 for default)
    char f_sign = '\0';

    // whether or not certain characters were given
    bool f_has_0 = false, f_has_neg = false;

    // width & precision fields
    int64_t f_width = 0, f_prec = -1;
    
    // the type, i.e. `i, d, x`, etc
    char f_typ = 'i';

    // temporary char
    char c;
    

    // parse flags
    while ((c = *fstrc) == '+' || c == '-' || c == ' ' || c == '0') {
        if (c == '+' || c == ' ') {
            if (f_sign != '\0') return ks_throw(ks_T_ArgError, "Invalid format string: %R, given the 'sign' field multiple times");
            f_sign = c;
        } else if (c == '-') {
            if (f_has_neg) return ks_throw(ks_T_ArgError, "Invalid format string: %R, given the '-' flag multiple times");
            f_has_neg = true;
        } else if (c == '0') {
            if (f_has_0) return ks_throw(ks_T_ArgError, "Invalid format string: %R, given the '0' flag multiple times");
            f_has_0 = true;
        }
        fstrc++;
    }

    // parse width
    while (isdigit(c = *fstrc)) {
        f_width = f_width * 10 + (c - '0');
        fstrc++;
    }

    // parse precision
    if (*fstrc == '.') {
        fstrc++;
        f_prec = 0;
        while (isdigit(c = *fstrc)) {
            f_prec = f_prec * 10 + (c - '0');
            fstrc++;
        }
    }

    // parse type
    if (*fstrc) {
        f_typ = *fstrc++;
        if (!(f_typ == 'i' || f_typ == 'd' || f_typ == 'x')) {
            if (*fstrc) return ks_throw(ks_T_ArgError, "Invalid format string: %R, unknown type '%c'", f_typ);
        }
    }

    if (*fstrc) return ks_throw(ks_T_ArgError, "Invalid format string: %R, given extra characters");


    // TODO: handle roman here as well

    int base = -1;
    /**/ if (f_typ == 'i' || f_typ == 'd') base = 10;
    else if (f_typ == 'x') base = 16;

    // now, actually format it
    // temporary variable
    mpz_t tz;
    if (self->isLong) {
        *tz = *self->vz;
    } else {
        mpz_init(tz);
        if (!ks_num_get_mpz((ks_obj)self, tz)) {
            mpz_clear(tz);
            return NULL;
        }
    }

    // first, calculate sizes
    size_t num_size = mpz_sizeinbase(tz, base);

    // whether tz >= 0
    bool tz_nz = mpz_cmp_ui(tz, 0) >= 0;

    if (tz_nz) {
        if (f_sign == '+' || f_sign == ' ') {
            // add a negative size in these cases
            num_size++;
        }
    } else {
        // always a negative sign
        num_size++;
        // we want the absolute value, so invert it (we handle the sign ourself)
        mpz_neg(tz, tz);
    }


    // total size of the output
    size_t total_size = num_size > f_width ? num_size : f_width;

    // temporary buffer to create the string from
    char* tmp = ks_malloc(total_size + 1);
    tmp[total_size] = '\0';

    // left v right aligned
    int i = f_has_neg ? 0 : total_size - num_size;

    if (!f_has_neg) {
        int j;
        for (j = 0; j < i; ++j) tmp[j] = f_has_0 ? '0' : ' ';
    }

    // now, build the string
    if (tz_nz) {
        /**/ if (f_sign == '+') tmp[i++] = '+';
        else if (f_sign == ' ') tmp[i++] = ' ';
    } else {
        tmp[i++] = '-';
    }
/*
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
        return ks_throw(ks_T_ArgError, "Invalid base '%i' for 'int' to 'str' conversion", base);
    }
    */
    // add the 'meats'
    mpz_get_str(&tmp[i], base, tz);

    if (!self->isLong) mpz_clear(tz);


    if (f_has_neg) {
        i = total_size - num_size;
        while (i < total_size) tmp[i++] = ' ';
    }

    ks_str res = ks_str_new_c(tmp, total_size);
    ks_free(tmp);

    return (ks_obj)res;
}


// int.__str__(self, base=none) - to string
static KS_TFUNC(int, str) {
    ks_int self;
    ks_obj mode = NULL;
    KS_GETARGS("self:* ?mode", &self, ks_T_int, &mode)

    // the base
    int base = 10;

    if (!mode) {
        base = 10;
    } else if (ks_num_is_numeric(mode)) {
        int64_t v64;
        if (!ks_num_get_int64(mode, &v64)) {
            return NULL;
        }
        base = v64;
    } else if (mode->type == ks_T_str && ks_str_eq_c((ks_str)mode, "roman", 5)) {
        base = KS_BASE_ROMAN;
    } else {
        return ks_throw(ks_T_ArgError, "Unknown base: %R", mode);
    }

    if (base == KS_BASE_ROMAN) {
        // convert to romans
        // structure of roman numerals (in reverse order!)
        static const struct _roman_s {
            // string of the roman numeral
            char c;

            // numeric value
            int val;

        } romans[] = {
            {'M',     1000},
            {'D',     500},
            {'C',     100},
            {'L',     50},
            {'X',     10},
            {'V',     5},
            {'I',     1},
        };

        // number of roman characters
        static const int romans_n = sizeof(romans) / sizeof(*romans);

        // add a given roman 'n' times
        #define _ROM_ADD(_rom, _n) { \
            int _i; \
            for (_i = 0; _i < (_n); ++_i) { \
                ks_str_builder_add(sb, (void*)&_rom.c, 1); \
            } \
        }

        // add a low, high roman pair
        #define _ROM_PRE(_lwdig, _hidig) { \
            ks_str_builder_add(sb, (void*)&_lwdig.c, 1); \
            ks_str_builder_add(sb, (void*)&_hidig.c, 1); \
        }
    
        // create a string builder
        ks_str_builder sb = ks_str_builder_new();

        // the value being converted
        mpz_t val, tmp;
        mpz_init(val);
        mpz_init(tmp);
        if (!ks_num_get_mpz((ks_obj)self, val)) return NULL;

        // take out negative sign
        bool isNeg = mpz_sgn(val) < 0;
        if (isNeg) {
            ks_str_builder_add(sb, "-", 1);
            mpz_neg(val, val);
        }

        ks_str_builder_add(sb, "0r", 2);

        while (mpz_sgn(val) != 0) {

            // handle highest first
            if (mpz_cmp_ui(val, romans[0].val) >= 0) {
                mpz_tdiv_qr_ui(tmp, val, val, romans[0].val);

                if (!mpz_fits_slong_p(tmp)) {
                    mpz_clear(val); mpz_clear(tmp);
                    KS_DECREF(sb);
                    return ks_throw(ks_T_InternalError, "Value '%S' too large to convert to roman numeral!", self);
                }

                int n_iter = mpz_get_si(tmp);
                _ROM_ADD(romans[0], n_iter);
            } else {
                // handle lower ones

                // current roman
                int crom = 1;

                bool didBreak = false;

                // check middle ones
                while (crom < romans_n - 1) {
                    
                    if (mpz_cmp_ui(val, romans[crom].val) >= 0) {
                        
                        // threshold
                        int thres = romans[crom-1].val - romans[crom+1].val;

                        if (mpz_cmp_ui(val, thres) < 0) {
                            // just add the current roman value

                            mpz_tdiv_qr_ui(tmp, val, val, romans[crom].val);
                            if (!mpz_fits_slong_p(tmp)) {
                                mpz_clear(val); mpz_clear(tmp);
                                KS_DECREF(sb);
                                return ks_throw(ks_T_InternalError, "Value '%S' too large to convert to roman numeral!", self);
                            }

                            int n_iter = mpz_get_si(tmp);
                            _ROM_ADD(romans[crom], n_iter);
                        } else {

                            // add a subtractive pair
                            _ROM_PRE(romans[crom+1], romans[crom-1]);
                            mpz_sub_ui(val, val, thres);
                        }
                        didBreak = true;
                        break;
                    }

                    crom++;
                }

                if (!didBreak &&  mpz_cmp_ui(val, romans[romans_n - 1].val) >= 0) {

                    // handle smallest case
                    int thres = romans[romans_n - 2].val - romans[romans_n - 1].val;
                    if (mpz_cmp_ui(val, thres) < 0) {
                        int n_iter = mpz_get_ui(val);
                        _ROM_ADD(romans[romans_n - 1], n_iter);
                        mpz_set_ui(val, 0);
                    } else {
                        _ROM_PRE(romans[romans_n - 1], romans[romans_n - 2]);
                        mpz_sub_ui(val, val, thres);
                    }

                }
            }
        }

        mpz_clear(val);
        mpz_clear(tmp);

        ks_str res = ks_str_builder_get(sb);
        KS_DECREF(sb);
        return (ks_obj)res;


    } else if (base == 10 && !self->isLong) {
        // simple base 10
        char tmp[256];
        int ct = snprintf(tmp, sizeof(tmp) - 1, "%lli", (long long int)self->v64);

        return (ks_obj)ks_str_new_c(tmp, ct);

    } else {
        // get temporary variable
        mpz_t tz;
        if (self->isLong) {
            *tz = *self->vz;
        } else {
            mpz_init(tz);
            if (!ks_num_get_mpz((ks_obj)self, tz)) {
                mpz_clear(tz);
                return NULL;
            }
        }

        // do mpz to string
        size_t total_size = 16 + mpz_sizeinbase(tz, base);

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
            return ks_throw(ks_T_ArgError, "Invalid base '%i' for 'int' to 'str' conversion", base);
        }

        // use MPZ to convert to string
        mpz_get_str(&tmp[i], base, tz);

        if (!self->isLong) mpz_clear(tz);

        ks_str res = ks_str_new_c(tmp, -1);
        ks_free(tmp);

        return (ks_obj)res;

    }
}


// int.__free__(self) - free string
static KS_TFUNC(int, free) {
    ks_int self;
    KS_GETARGS("self:*", &self, ks_T_int)

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
    
    ks_type_init_c(ks_T_int, "int", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c(int_new_, "int.__new__(obj, base=none)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(int_free_, "int.__free__(self)")},

        {"__fmt__",                (ks_obj)ks_cfunc_new_c(int_fmt_, "int.__fmt__(self, fstr)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c(int_str_, "int.__str__(self, base=none)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(int_str_, "int.__repr__(self, base=none)")},

        // define operators
        KST_NUM_OPKVS(int)

        
    ));

    ks_T_int->flags |= KS_TYPE_FLAGS_EQSS;

}
