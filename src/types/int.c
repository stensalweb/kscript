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

    // original
    char* ostr = str;

    // try to calculate in a 64 bit integer
    int64_t v64 = 0;

    // check for any signs
    bool isNeg = *str == '-';
    if (isNeg || *str == '+') {
        str++;
        len--;
    }

    if (str[0] == '0' && str[1] == 'x') {
        if (base != 0 && base != 16) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%s'", base, ostr);
        base = 16;
        str+=2;
        len-=2;
    } else if (str[0] == '0' && str[1] == 'o') {
        if (base != 0 && base != 8) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%s'", base, ostr);
        base = 8;
        str+=2;
        len-=2;
    } else if (str[0] == '0' && str[1] == 'b') {
        if (base != 0 && base != 2) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%s'", base, ostr);
        base = 2;
        str+=2;
        len-=2;
    } else if (str[0] == '0' && str[1] == 'r') {
        if (base != 0 && base != KS_BASE_ROMAN) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%s'", base, ostr);
        base = KS_BASE_ROMAN;
        str+=2;
        len-=2;
    } else {
        if (base == 0) base = 10;
    }

    // check for romans
    if (base == KS_BASE_ROMAN) return ks_int_new_roman(str, base);

    int i = 0;

    // parse out main value
    while (i < len) {
        int dig = my_getdig(str[i]);
        // check for invalid/out of range digit
        if (dig < 0 || dig >= base) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%s'", base, ostr);

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
        return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: %s", base, ostr);
    }

    return self;
}


// convert roman character to digit
static int roman_dig(int romchar) {
    /**/ if (romchar == 'I') return 1;
    else if (romchar == 'V') return 5;
    else if (romchar == 'X') return 10;
    else if (romchar == 'L') return 50;
    else if (romchar == 'C') return 100;
    else if (romchar == 'D') return 500;
    else if (romchar == 'M') return 1000;
    else {
        return -1;
    }
}

// Create a new integer from a roman-style string
ks_int ks_int_new_roman(char* rom, int len) {

    // original
    const char* rom_orig = rom;

    // structure of roman numerals (in reverse order!)
    static const struct _roman_s {
        // string of the roman numeral
        char* str;

        // numeric value
        int val;

    } romans[] = {
        {"M",     1000},
        {"D",     500},
        {"C",     100},
        {"L",     50},
        {"X",     10},
        {"V",     5},
        {"I",     1},
    };

    #define N_ROM ((sizeof(romans) / sizeof(romans[0])))

    // create new integer
    mpz_t self;
    mpz_init(self);

    int i = 0;

    // get if negative
    bool isNeg = *rom == '-';
    if (isNeg) {
        rom++;
        i++;
    }

    if (*rom == '0' && rom[1] == 'r') {
        rom++;
        rom++;
        i++;
        i++;
    }

    while (i < len) {

        int dig = roman_dig(*rom);

        // ensure there wasn't a problem
        if (dig < 0) {
            mpz_clear(self);
            return (ks_int)ks_throw(ks_T_ArgError, "Invalid roman numeral: '%*s', invalid digit '%c'", len, rom_orig, *rom);
        }

        if ((len - i) > 2) {
            int digp2 = roman_dig(rom[2]);
            if (digp2 < 0) {
                mpz_clear(self);
                return (ks_int)ks_throw(ks_T_ArgError, "Invalid roman numeral: '%*s', invalid digit '%c'", len, rom_orig, rom[2]);
            } else if (dig < digp2) {
                mpz_clear(self);
                return (ks_int)ks_throw(ks_T_ArgError, "Invalid roman numeral: '%*s', some digits are in invalid order!", len, rom_orig);
            }
        }


        int digp1 = roman_dig(rom[1]);

        if (dig >= digp1) {
            mpz_add_ui(self, self, dig);
        } else {
            mpz_add_ui(self, self, digp1 - dig);
            i++;
            rom++;
        }
        
        rom++;
        i++;
    }

    if (isNeg) mpz_neg(self, self);

    ks_int ret = ks_int_new_mpz_n(self);
    return ret;
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

// int.__new__(typ, obj, mode=none) -> convert 'obj' to a int
static KS_TFUNC(int, new) {
    ks_type typ;
    ks_obj obj;
    ks_obj mode = KSO_NONE;
    KS_GETARGS("typ:* obj ?mode", &typ, ks_T_type, &obj, &mode)
    if (!ks_type_issub(typ, ks_T_int)) ks_throw(ks_T_InternalError, "Constructor for type '%S' called given typ as '%S' (not a sub-type!)", ks_T_int, typ);

    if (mode != KSO_NONE) {
        if (obj->type != ks_T_str) return ks_throw(ks_T_ArgError, "When given parameter 'mode', 'obj' must be a string!");
        // a specific base is requested, so it must be a string
        if (mode->type == ks_T_str) {
            if (ks_str_eq_c((ks_str)mode, "roman", 5)) {
                return (ks_obj)ks_int_new_roman(((ks_str)obj)->chr, ((ks_str)obj)->len_b);
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


// int.__str__(self, mode=10) - to string
static KS_TFUNC(int, str) {
    ks_int self;
    ks_obj mode = NULL;
    KS_GETARGS("self:* ?mode", &self, ks_T_int, &mode)

    // the base
    int base = 10;

    #define _BASE_ROMAN -2

    if (!mode) {
        base = 10;
    } else if (ks_num_is_numeric(mode)) {
        int64_t v64;
        if (!ks_num_get_int64(mode, &v64)) {
            return NULL;
        }
        base = v64;
    } else if (mode->type == ks_T_str && ks_str_eq_c((ks_str)mode, "roman", 5)) {
        base = _BASE_ROMAN;
    } else {
        return ks_throw(ks_T_ArgError, "Unknown mode: %R", mode);
    }

    if (base == _BASE_ROMAN) {
        // we need to compute roman
        ks_str key = ks_str_new("toRoman");
        ks_obj toRoman = ks_type_get(ks_T_int, key);
        KS_DECREF(key);
        if (!toRoman) return NULL;

        ks_obj res = ks_obj_call(toRoman, 1, (ks_obj[]){ (ks_obj)self });
        KS_DECREF(toRoman);
        if (!res) return NULL;


        ks_str withprefix = ks_fmt_c("0r%S", res);
        KS_DECREF(res);
        return (ks_obj)withprefix;

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



// int.toRoman(self) - convert to a roman numeral string
static KS_TFUNC(int, toRoman) {
    ks_int self;
    KS_GETARGS("self:*", &self, ks_T_int)


    // structure of roman numerals (in reverse order!)
    static const struct _roman_s {
        // string of the roman numeral
        char* str;

        // numeric value
        int val;

    } romans[] = {
        {"M",     1000},
        {"D",     500},
        {"C",     100},
        {"L",     50},
        {"X",     10},
        {"V",     5},
        {"I",     1},
    };

    #define N_ROM ((sizeof(romans) / sizeof(romans[0])))

    // add a given roman 'n' times
    #define _ROM_ADD(_rom, _n) { \
        int _i; \
        for (_i = 0; _i < (_n); ++_i) { \
            ks_str_builder_add(sb, _rom.str, 1); \
        } \
    }

    // add a low, high roman pair
    #define _ROM_PRE(_lwdig, _hidig) { \
        ks_str_builder_add(sb, _lwdig.str, 1); \
        ks_str_builder_add(sb, _hidig.str, 1); \
    }
    

    if (self->isLong) {
        // TODO;
        ks_str_builder sb = ks_str_builder_new();

        mpz_t val, tmp;
        mpz_init_set(val, self->vz);
        mpz_init(tmp);


        // take out negative sign
        bool isNeg = mpz_sgn(val) < 0;
        if (isNeg) {
            ks_str_builder_add(sb, "-", 1);
            mpz_neg(val, val);
        }

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
                while (crom < N_ROM - 1) {
                    
                    if (mpz_cmp_ui(val, romans[crom].val) >= 0) {
                        
                        // threshold
                        int thres = romans[crom-1].val - romans[crom+1].val;

                        if (mpz_cmp_ui(val, thres) < 0) {
                            // just add the current roman value

                            mpz_tdiv_qr_ui(tmp, val, val, romans[0].val);
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

                if (!didBreak &&  mpz_cmp_ui(val, romans[N_ROM - 1].val) >= 0) {
                    // handle smallest case
                    int thres = romans[N_ROM - 2].val - romans[N_ROM - 1].val;
                    if (mpz_cmp_ui(val, thres) < 0) {
                        int n_iter = mpz_get_ui(val);
                        _ROM_ADD(romans[N_ROM - 1], n_iter);
                        mpz_set_ui(val, 0);
                    } else {
                        _ROM_PRE(romans[N_ROM - 1], romans[N_ROM - 2]);
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


    } else {
        // current value
        int64_t val = self->v64;

        ks_str_builder sb = ks_str_builder_new();

        // take out negative sign
        bool isNeg = val < 0;
        if (isNeg) {
            ks_str_builder_add(sb, "-", 1);
            val = -val;
        }


        while (val != 0) {
            // handle highest first
            if (val > romans[0].val) {
                _ROM_ADD(romans[0], val / romans[0].val);
                val %= romans[0].val;
            } else {
                // handle lower ones

                // current roman
                int crom = 1;

                bool didBreak = false;

                // check middle ones
                while (crom < N_ROM - 1) {
                    
                    if (val >= romans[crom].val) {
                        // threshold
                        int thres = romans[crom-1].val - romans[crom+1].val;

                        if (val < thres) {
                            // just add the current roman value
                            _ROM_ADD(romans[crom], val / romans[crom].val);
                            val %= romans[crom].val;
                        } else {
                            // add a subtractive pair
                            _ROM_PRE(romans[crom+1], romans[crom-1]);
                            val -= thres;
                        }
                        didBreak = true;
                        break;
                    }

                    crom++;
                }

                if (!didBreak && val >= romans[N_ROM - 1].val) {
                    // handle smallest case
                    int thres = romans[N_ROM - 2].val - romans[N_ROM - 1].val;
                    if (val < thres) {
                        _ROM_ADD(romans[N_ROM - 1], val);
                        val = 0;
                    } else {
                        _ROM_PRE(romans[N_ROM - 1], romans[N_ROM - 2]);
                        val -= thres;
                    }

                }

            }
        }

        ks_str res = ks_str_builder_get(sb);
        KS_DECREF(sb);
        return (ks_obj)res;
    }
    #undef _ROM_ADD
    #undef _ROM_PRE
}

// int.fromRoman(roman_str) - convert from a roman numeral string
static KS_TFUNC(int, fromRoman) {
    ks_str roman_str;
    KS_GETARGS("roman_str:*", &roman_str, ks_T_str)

    return (ks_obj)ks_int_new_roman(roman_str->chr, roman_str->len_c);
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
        {"__free__",               (ks_obj)ks_cfunc_new_c(int_free_, "int.__free__(self)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c(int_str_, "int.__str__(self, mode=10)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(int_str_, "int.__repr__(self, mode=10)")},


        {"toRoman",                (ks_obj)ks_cfunc_new_c(int_toRoman_, "int.toRoman(self)")},
        {"fromRoman",              (ks_obj)ks_cfunc_new_c(int_fromRoman_, "int.fromRoman(self)")},

        // define operators
        KST_NUM_OPKVS(int)

        
    ));

    ks_T_int->flags |= KS_TYPE_FLAGS_EQSS;

}
