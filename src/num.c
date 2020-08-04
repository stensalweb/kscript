/* num.c - implementation of `ks_num_*` routines
 *
 * Essentially, helps with converting numbers and allowing liquid type conversions.
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// convert roman character to its integer value
static int roman_val(ks_unich romchar) {
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


// Parse `len` bytes of str as a numeric type in base `base`
// If `base==KS_BASE_AUTO`, then autodetect the base, or use 10 as a default
// NOTE: Returns new reference, or NULL if there was an error (and throw it)
ks_obj ks_num_parse(const char* str, int len, int base) {
    if (len < 0) len = strlen(str);

    // original
    int olen = len;
    const char* ostr = str;

    // parse out sign
    bool isNeg = *str == '-';
    if (isNeg || *str == '+') {
        str++;
        len--;
    }

    if (base == KS_BASE_AUTO) {
        // auto detect base
        if (len > 2 && str[0] == '0') {
            // check for `0x`, etc prefixes
            char c = str[1];
            bool hasPrefix = true;
            /**/ if (c == 'x') base = KS_BASE_16;
            else if (c == 'o') base = KS_BASE_8;
            else if (c == 'b') base = KS_BASE_2;
            else if (c == 'r') base = KS_BASE_ROMAN;
            else {
                // unknown; but don't give error, it could just begin with 0s
                hasPrefix = false;
            }

            // take off prefix from parsing string
            if (hasPrefix) {
                str += 2;
                len -= 2;
            }
        }

        // assume base 10 if none is given via prefix
        if (base == KS_BASE_AUTO) base = KS_BASE_10;
    } else if (len > 2 && str[0] == '0') {

        char c = str[1];
        bool hasPrefix = false;
        /**/ if (c == 'x' && base == KS_BASE_16) hasPrefix = true;
        else if (c == 'o' && base == KS_BASE_8) hasPrefix = true;
        else if (c == 'b' && base == KS_BASE_2) hasPrefix = true;
        else if (c == 'r' && base == KS_BASE_ROMAN) hasPrefix = true;

        if (hasPrefix) {
            str += 2;
            len -= 2;
        }

    }
    

    // loop vars
    int i;

    // whether it contains '.', 'i', 
    bool hasDot = false, hasi = false;
    for (i = 0; i < len && (!hasDot || !hasi); ++i) {
        if (str[i] == '.') hasDot = true;
        if (str[i] == 'i') hasi = true;
    }

    // whether or not it is a floating point value
    bool isFloat = hasDot || hasi;

    // check for exponent in different bases
    if (base == KS_BASE_10) {
        for (i = 0; i < len && !isFloat; ++i) {
            if (str[i] == 'e') isFloat = true;
        }
    } else if (base == KS_BASE_16) {
        for (i = 0; i < len && !isFloat; ++i) {
            if (str[i] == 'p') isFloat = true;
        }
    }

    // handle different bases
    if (base == KS_BASE_ROMAN) {
        // should never be a floating point number
        if (isFloat) return ks_throw(ks_T_ArgError, "Invalid syntax for a roman numeral: %*s", olen, ostr);

        // TODO: should we accept bars over Ms or some similar syntax, for example 10000 -> X_, where every 'X_' becomes X * M**(count('_'))

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

        // create new integer
        mpz_t self;
        mpz_init(self);

        i = 0;
        // extract extra starting positions
        while (i < len) {
            // convert to value & ensure it was valid
            int dig = roman_val(str[i]);
            if (dig < 0) {
                mpz_clear(self);
                return ks_throw(ks_T_ArgError, "Invalid roman numeral: '%*s', invalid digit '%c'", len, ostr, str[i]);
            }

            if (i < len - 2) {
                // check 2 characters ahead
                int digp2 = roman_val(str[i+2]);
                if (digp2 < 0) {
                    mpz_clear(self);
                    return ks_throw(ks_T_ArgError, "Invalid roman numeral: '%*s', invalid digit '%c'", olen, ostr, str[i+2]);
                } else if (dig < digp2) {
                    mpz_clear(self);
                    return ks_throw(ks_T_ArgError, "Invalid roman numeral: '%*s', digits are in an invalid order", olen, ostr);
                }
            }


            // check for next character
            int digp1 = roman_val(str[i+1]);

            if (dig >= digp1) {
                mpz_add_ui(self, self, dig);
            } else {
                // add difference
                mpz_add_ui(self, self, digp1 - dig);
                // we comsume both here
                i++;
            }

            // advance to next character
            i++;
        }

        // return value
        if (isNeg) mpz_neg(self, self);
        return (ks_obj)ks_int_new_mpz_n(self);
    } else if (isFloat) {
        // numerical base; floating point

        // check for invalids
        if (base == 2 || base == 8) {
            return ks_throw(ks_T_ArgError, "Invalid float number: Only base 10 and 16 floats are supported, not %i", olen, ostr, base);
        }
        
        // convert to literal
        char* tmp_str = ks_malloc(olen + 1);
        memcpy(tmp_str, ostr, olen);
        tmp_str[olen] = '\0';

        char* endptr = NULL;
        double rv = strtold(tmp_str, &endptr);

        if (!endptr || endptr != tmp_str + olen) {
            return ks_throw(ks_T_ArgError, "Invalid float number: '%*s'", olen, ostr);
        }

        return (ks_obj)ks_float_new(rv);

    } else {
        // numerical base; integer
        int64_t v64 = 0;

        i = 0;
        // parse out main value
        while (i < len) {
            int dig = my_getdig(str[i]);
            // check for invalid/out of range digit
            if (dig < 0 || dig >= base) return ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%*s'", base, olen, ostr);

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
        return (ks_obj)ks_int_new(isNeg ? -v64 : v64);

        do_mpz_str:;

        // create MPZ
        mpz_t self;
        mpz_init(self);

        // convert to literal
        char* tmp_str = ks_malloc(len + 1);
        memcpy(tmp_str, str, len);
        tmp_str[len] = '\0';

        // set from string
        if (mpz_set_str(self, tmp_str, base) < 0) {
            mpz_clear(self);
            return ks_throw(ks_T_ArgError, "Invalid format for base %i integer: '%*s'", base, olen, ostr);
        }
        if (isNeg) mpz_neg(self, self);
        return (ks_obj)ks_int_new_mpz_n(self);
    
    }
}

// get hash of number
bool ks_num_hash(ks_obj self, ks_hash_t* out) {
    if (self->type == ks_T_bool) {
        *out = (self == KSO_TRUE) ? 1 : 1;
        return true;
    } else if (self->type == ks_T_int) {
        if (((ks_int)self)->isLong) {
            *out = ks_hash_bytes((uint8_t*)((ks_int)self)->vz->_mp_d, sizeof(*((ks_int)self)->vz->_mp_d) * ((ks_int)self)->vz->_mp_size);
            if (*out == 0) *out = 1;
            return true;
        } else {
            *out = (ks_hash_t)((ks_int)self)->v64;
            if (*out == 0) *out = 1;
            return true;
        }
    } else if (self->type == ks_T_float) {
        double vv = ((ks_float)self)->val;
        if (vv == round(vv)) *out = (ks_hash_t)vv;
        else *out = ks_hash_bytes((uint8_t*)&vv, sizeof(vv));
        return true;
    } else if (self->type == ks_T_float) {
        double complex vv = ((ks_float)self)->val;
        if (vv == round((double)vv)) *out = (ks_hash_t)(double)vv;
        else *out = ks_hash_bytes((uint8_t*)&vv, sizeof(vv));
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return ks_num_hash((ks_obj)((ks_Enum)self)->enum_val, out);
    } else {
        ks_throw(ks_T_MathError, "Could not hash '%T' object", self);
        return false;
    }
}

// get whether it fits 64 bit
static bool my_num_fits_int64(ks_obj self, int dep) {
    if (dep > 4) return false;
    if (self->type == ks_T_bool) {
        return true;
    } else if (self->type == ks_T_int) {
        return !((ks_int)self)->isLong;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return my_num_fits_int64((ks_obj)((ks_Enum)self)->enum_val, dep+1);
    } else if (self->type->__int__ != NULL) {
        ks_obj tmp = ks_obj_call(self->type->__int__, 1, &self);
        if (!tmp) {
            ks_catch_ignore();
            return false;
        }
        bool rstat = my_num_fits_int64(tmp, dep+1);
        KS_DECREF(tmp);
        return rstat;
    } else {
        return false;
    }
}

// get whether it fits 64 bit
bool ks_num_fits_int64(ks_obj self) {
    return my_num_fits_int64(self, 0);
}


// get whether a type is integral
bool ks_num_is_integral(ks_obj self) {
    if (self->type == ks_T_bool) {
        return true;
    } else if (self->type == ks_T_int) {
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return true;
    } else if (self->type->__int__ != NULL) {
        return true;
    } else {
        return false;
    }
}


// get whether a type is numeric
bool ks_num_is_numeric(ks_obj self) {
    if (self->type == ks_T_bool) {
        return true;
    } else if (self->type == ks_T_int) {
        return true;
    } else if (self->type == ks_T_float) {
        return true;
    } else if (self->type == ks_T_complex) {
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return true;
    } else if (self->type->__int__ != NULL) {
        return true;
    } else if (self->type->__float__ != NULL) {
        return true;
    } else {
        return false;
    }
}

// convert numeric to bool
bool ks_num_get_bool(ks_obj self, bool* out) {
    if (self->type == ks_T_bool) {
        *out = (self == KSO_TRUE) ? true : false;
        return true;
    } else if (self->type == ks_T_int) {
        return ks_int_sgn((ks_int)self) != 0;
    } else if (self->type == ks_T_float) {
        *out = round(((ks_float)self)->val);
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return ks_num_get_bool((ks_obj)((ks_Enum)self)->enum_val, out);
    } else {
        ks_throw(ks_T_MathError, "Could not interpret '%T' object as a boolean", self);
        return false;
    }
}


// convert object to int64_t
static bool my_num_get_int64(ks_obj self, int64_t* out, int dep) {
    if (self->type == ks_T_bool) {
        *out = (self == KSO_TRUE) ? 1 : 0;
        return true;
    } else if (self->type == ks_T_int) {
        if (((ks_int)self)->isLong) {
            #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
            // ensure it can fit
            if (!mpz_fits_slong_p(((ks_int)self)->vz)) return false;

            // otherwise, get the signed integer
            *out = mpz_get_si(((ks_int)self)->vz);
            return true;
            #else
            if (mpz_fits_slong_p(((ks_int)self)->vz)) {
                *out = (int64_t)mpz_get_si(((ks_int)self)->vz);
                return false;
            } else {
                size_t nbits = mpz_sizeinbase(((ks_int)self)->vz, 2);

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
                        *out = -(int64_t)u_abs;
                    } else {
                        *out = (int64_t)u_abs;
                    }
                    return true;

                }
            }
            #endif
        } else {
            *out = ((ks_int)self)->v64;
            return true;
        }
    } else if (self->type == ks_T_float) {
        *out = round(((ks_float)self)->val);
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return my_num_get_int64((ks_obj)((ks_Enum)self)->enum_val, out, dep+1);
    } else if (self->type->__int__ != NULL) {
        ks_obj tmp = ks_obj_call(self->type->__int__, 1, &self);
        if (!tmp) {
            ks_catch_ignore();
            return false;
        }
        bool rstat = my_num_get_int64(tmp, out, dep+1);
        KS_DECREF(tmp);
        return rstat;
    } else {
        ks_throw(ks_T_MathError, "Could not interpret '%T' object as an int64", self);
        return false;
    }
}

bool ks_num_get_int64(ks_obj self, int64_t* out) {
    return my_num_get_int64(self, out, 0);
}


// Attempt to convert 'self' to an 'int'
// NOTE: If there was a problem, return false and throw an error
ks_int ks_num_get_int(ks_obj self) {
    if (self->type == ks_T_int) {
        return (ks_int)KS_NEWREF(self);
    } else if (self->type == ks_T_bool) {
        return (ks_int)ks_int_new(self == KSO_TRUE ? 1 : 0);
    } else if (self->type->__int__ != NULL) {
        ks_obj tmp = ks_obj_call(self->type->__int__, 1, &self);
        if (!tmp) {
            ks_catch_ignore();
            return false;
        }
        ks_int ret = ks_num_get_int(tmp);
        KS_DECREF(tmp);
        return ret;
    }

    KS_THROW_TYPE_ERR(self, ks_T_int);
}



// get an MPZ from a given object
static bool my_num_get_mpz(ks_obj self, mpz_t out, int dep) {
    if (self->type == ks_T_bool) {
        mpz_set_si(out, (self == KSO_TRUE) ? 1 : 0);
        return true;
    } else if (self->type == ks_T_int) {
        if (((ks_int)self)->isLong) {
            mpz_set(out, ((ks_int)self)->vz);
            return true;
        } else {
            #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
            mpz_set_si(out, ((ks_int)self)->v64);
            return true;
            #else
            if (((ks_int)self)->v64 == (signed long)((ks_int)self)->v64) {
                mpz_set_si(out, (signed long)((ks_int)self)->v64);
                return true;
            } else {
                mpz_import(out, 1, 1, sizeof(((ks_int)self)->v64), 0, 0, &((ks_int)self)->v64);
                return true;
            }
            #endif
        }

    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return my_num_get_mpz((ks_obj)((ks_Enum)self)->enum_val, out, dep+1);
        //mpz_set_si(out, ((ks_Enum)self)->enum_idx);
        //return true;
    } else if (self->type->__int__ != NULL) {
        ks_obj tmp = ks_obj_call(self->type->__int__, 1, &self);
        if (!tmp) {
            ks_catch_ignore();
            return false;
        }
        bool rstat = my_num_get_mpz(tmp, out, dep+1);
        KS_DECREF(tmp);
        return rstat;
    } else {
        ks_throw(ks_T_MathError, "Could not interpret '%T' object as a mpz", self);
        return false;
    }
}

bool ks_num_get_mpz(ks_obj self, mpz_t out) {
    return my_num_get_mpz(self, out, 0);
}

// convert object to double
bool ks_num_get_double(ks_obj self, double* out) {

    if (self->type == ks_T_bool) {
        *out = (self == KSO_TRUE) ? 1.0 : 0.0;
        return true;
    } else if (self->type == ks_T_int) {
        if (((ks_int)self)->isLong) {
            *out = mpz_get_d(((ks_int)self)->vz);
            return true;
        } else {
            *out = ((ks_int)self)->v64;
            return true;
        }

    } else if (self->type == ks_T_float) {
        *out = ((ks_float)self)->val;
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return ks_num_get_double((ks_obj)((ks_Enum)self)->enum_val, out);
    } else {
        ks_throw(ks_T_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}


// convert object to double complex
bool ks_num_get_double_complex(ks_obj self, double complex* out) {

    if (self->type == ks_T_bool) {
        *out = (self == KSO_TRUE) ? 1.0 : 0.0;
        return true;
    } else if (self->type == ks_T_int) {
        if (((ks_int)self)->isLong) {
            *out = mpz_get_d(((ks_int)self)->vz);
            return true;
        } else {
            *out = ((ks_int)self)->v64;
            return true;
        }
    } else if (self->type == ks_T_float) {
        *out = ((ks_float)self)->val;
        return true;
    } else if (self->type == ks_T_complex) {
        *out = ((ks_complex)self)->val;
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return ks_num_get_double_complex((ks_obj)((ks_Enum)self)->enum_val, out);
    } else {
        ks_throw(ks_T_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}


// get the sign of a numerical quantity
bool ks_num_sgn(ks_obj self, int* out) {
    if (self->type == ks_T_bool) {
        *out = (self == KSO_TRUE) ? 1 : 0;
        return true;
    } else if (self->type == ks_T_int) {
        *out = ks_int_sgn((ks_int)self);
        return true;
    } else if (self->type == ks_T_float) {
        double v = ((ks_float)self)->val;
        *out = v == 0 ? 0 : (v > 0 ? 1 : -1);
        return true;
    } else if (ks_type_issub(self->type, ks_T_Enum)) {
        return ks_num_sgn((ks_obj)((ks_Enum)self)->enum_val, out);
    } else {
        ks_throw(ks_T_MathError, "Could not get the sign of a '%T' object", self);
        return false;
    }
}

// compute L <=> R
bool ks_num_cmp(ks_obj L, ks_obj R, int* out) {

    if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("<=>", L, R);
        }
        *out = Lv == Rv ? 0 : (Lv > Rv ? 1 : -1);
        return true;
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            *out = Lv == Rv ? 0 : (Lv > Rv ? 1 : -1);
            return true;
        }

        mpz_t Lz, Rz;
        mpz_init(Lz);
        mpz_init(Rz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            KS_THROW_BOP_ERR("<=>", L, R);
        }

        int res = mpz_cmp(Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        *out = res == 0 ? 0 : (res > 0 ? 1 : -1);
        return true;

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("<=>", L, R);
}

// compare 2 numeric objects
bool ks_num_eq(ks_obj L, ks_obj R, bool* out) {
    if (L->type == ks_T_complex || R->type == ks_T_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("==", L, R);
        }
        return KSO_BOOL(Lv == Rv);
    } else if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("==", L, R);
        }
        *out = Lv == Rv;
        return true;
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            *out = Lv == Rv;
            return true;
        }

        mpz_t Lz, Rz;
        mpz_init(Lz);
        mpz_init(Rz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            KS_THROW_BOP_ERR("==", L, R);
        }

        int res = mpz_cmp(Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        *out = res == 0;
        return true;

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("==", L, R);
}

/* MATH OPERATIONS */


// compute L+R
ks_obj ks_num_add(ks_obj L, ks_obj R) {

    if (L->type == ks_T_complex || R->type == ks_T_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("+", L, R);
        }
        return (ks_obj)ks_complex_new(Lv + Rv);
    } else if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("+", L, R);
        }
        return (ks_obj)ks_float_new(Lv + Rv);
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            if (Lv > INT64_MAX - Rv || Lv < INT64_MIN + Rv) {
                // overflow; let it flow through and it end up using mpz types

            } else {
                return (ks_obj)ks_int_new(Lv + Rv);
            }
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("+", L, R);
        }

        mpz_add(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("+", L, R);
}


// compute L-R
ks_obj ks_num_sub(ks_obj L, ks_obj R) {

    if (L->type == ks_T_complex || R->type == ks_T_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("-", L, R);
        }
        return (ks_obj)ks_complex_new(Lv - Rv);
    } else if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("-", L, R);
        }
        return (ks_obj)ks_float_new(Lv - Rv);
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            if (Lv > INT64_MAX + Rv || Lv < INT64_MIN - Rv) {
                // overflow; let it flow through and it end up using mpz types

            } else {
                return (ks_obj)ks_int_new(Lv - Rv);
            }
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("-", L, R);
        }


        mpz_sub(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);

        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("-", L, R);
}

// compute L*R
ks_obj ks_num_mul(ks_obj L, ks_obj R) {
    if (L->type == ks_T_complex || R->type == ks_T_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("*", L, R);
        }


        return (ks_obj)ks_complex_new(Lv * Rv);
    } else if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("*", L, R);
        }

        return (ks_obj)ks_float_new(Lv * Rv);
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            if ((Rv > 0) && (Lv > INT64_MAX / Rv || Lv < INT64_MIN / Rv)) {
                // overflow; let it flow through and it end up using mpz types

            } else {
                return (ks_obj)ks_int_new(Lv * Rv);
            }
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("*", L, R);
        }

        mpz_mul(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("*", L, R);
}

// compute L/R
ks_obj ks_num_div(ks_obj L, ks_obj R) {
    if (L->type == ks_T_complex || R->type == ks_T_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("/", L, R);
        }
        return (ks_obj)ks_complex_new(Lv / Rv);
    } else if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("/", L, R);
        }
        return (ks_obj)ks_float_new(Lv / Rv);
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Rf && Rv == 0) return ks_throw(ks_T_MathError, "Division by 0");

        if (Lf && Rf) {
            return (ks_obj)ks_int_new(Lv / Rv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("/", L, R);
        }

        mpz_tdiv_q(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("*", L, R);
}

// compute L%R
ks_obj ks_num_mod(ks_obj L, ks_obj R) {
    if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("%", L, R);
        }
        if (Rv == 0) return ks_throw(ks_T_MathError, "Modulo by 0");

        return (ks_obj)ks_float_new(fmod(Lv, Rv));
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Rf && Rv == 0) return ks_throw(ks_T_MathError, "Modulo by 0");

        if (Lf && Rf) {

            int64_t Vv = Lv % Rv;
            if (Vv < 0) Vv += Rv;
            return (ks_obj)ks_int_new(Vv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("%", L, R);
        }

        mpz_mod(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("%", L, R);
}

// compute L**R
ks_obj ks_num_pow(ks_obj L, ks_obj R) {
    if (L->type == ks_T_complex || R->type == ks_T_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("**", L, R);
        }
        return (ks_obj)ks_complex_new(cpow(Lv, Rv));
    } else if (L->type == ks_T_float || R->type == ks_T_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_THROW_BOP_ERR("**", L, R);
        }
        return (ks_obj)ks_float_new(pow(Lv, Rv));
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {


        int Lsgn, Rsgn;
        if (!ks_num_sgn(L, &Lsgn) || !ks_num_sgn(R, &Rsgn)) {
            return NULL;
        }

        // x^0 == 1
        if (Rsgn == 0) return (ks_obj)ks_int_new(1);
        // x^-y == 0
        if (Lsgn > 0 && Rsgn < 0) return (ks_obj)ks_int_new(0);
        //if (Lsgn < 0) return ks_throw(ks_T_MathError, "Negative number in base: %S", L);

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        // 1^x == 1
        if (Lf && Lv == 1) {
            return (ks_obj)ks_int_new(1);
        }

        if (!Rf) {
            return ks_throw(ks_T_MathError, "Overflow in pow(), exponent too large: %S", R);
        }


        // otherwise, compute this way:
        mpz_t Lz, Vz;
        mpz_init(Lz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("**", L, R);
        }


        mpz_pow_ui(Vz, Lz, Rv);
        mpz_clear(Lz);

        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("**", L, R);
}


// compute -L
ks_obj ks_num_neg(ks_obj L) {
    if (L->type == ks_T_complex) {
        return (ks_obj)ks_complex_new(-((ks_complex)L)->val);
    } else if (L->type == ks_T_float) {
        return (ks_obj)ks_float_new(-((ks_float)L)->val);
    } else if (ks_num_is_integral(L)) {


        // see if it can fit in a 64 bit integer
        int64_t Lv;
        bool Lf = ks_num_get_int64(L, &Lv);
        if (Lf) {
            return (ks_obj)ks_int_new(-Lv);
        }

        // otherwise, declare mpz and set it, then negate it
        mpz_t Lz;
        mpz_init(Lz);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clear(Lz);
            KS_THROW_UOP_ERR("-", L);
        }

        mpz_neg(Lz, Lz);
        return (ks_obj)ks_int_new_mpz_n(Lz);
    } else {

    }

    // default: undefined
    KS_THROW_UOP_ERR("-", L);
}


// compute ~L
ks_obj ks_num_sqig(ks_obj L) {
    if (ks_num_is_integral(L)) {

        // see if it can fit in a 64 bit integer
        int64_t Lv;
        bool Lf = ks_num_get_int64(L, &Lv);
        if (Lf) {
            return (ks_obj)ks_int_new(~Lv);
        }

        // otherwise, declare mpz and set it, then negate it
        mpz_t Lz;
        mpz_init(Lz);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clear(Lz);
            KS_THROW_UOP_ERR("~", L);
        }

        // return 2s complement
        mpz_ui_sub(Lz, 1, Lz);
        return (ks_obj)ks_int_new_mpz_n(Lz);
    } else {

    }

    // default: undefined
    KS_THROW_UOP_ERR("~", L);
}



// compute abs(L)
ks_obj ks_num_abs(ks_obj L) {
    if (L->type == ks_T_complex) {
        return (ks_obj)ks_float_new(cabs(((ks_complex)L)->val));
    } else if (L->type == ks_T_float) {
        return (ks_obj)ks_float_new(fabs(((ks_float)L)->val));
    } else if (ks_num_is_integral(L)) {

        // see if it can fit in a 64 bit integer
        int64_t Lv;
        bool Lf = ks_num_get_int64(L, &Lv);
        if (Lf) {
            return Lv >= 0 ? KS_NEWREF(L) : (ks_obj)ks_int_new(-Lv);
        }

        // otherwise, declare mpz and set it, then take abs
        mpz_t Lz;
        mpz_init(Lz);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clear(Lz);
            return NULL;
        }

        // check for positive integer
        if (mpz_cmp_ui(Lz, 0) >= 0) {
            mpz_clear(Lz);
            return KS_NEWREF(Lz);
        }

        // otherwise, we need to negate it
        mpz_neg(Lz, Lz);
        return (ks_obj)ks_int_new_mpz_n(Lz);
    } else if (L->type->__abs__ != NULL) {
        return (ks_obj)ks_obj_call(L->type->__abs__, 1, &L);
    }

    // default: undefined
    KS_THROW_METH_ERR(L, "__abs__");
}


// compute L<R
ks_obj ks_num_lt(ks_obj L, ks_obj R) {
    int cmp;
    if (!ks_num_cmp(L, R, &cmp)) return NULL;
    return KSO_BOOL(cmp < 0);
}
// compute L<=R
ks_obj ks_num_le(ks_obj L, ks_obj R) {
    int cmp;
    if (!ks_num_cmp(L, R, &cmp)) return NULL;
    return KSO_BOOL(cmp <= 0);
}
// compute L>R
ks_obj ks_num_gt(ks_obj L, ks_obj R) {
    int cmp;
    if (!ks_num_cmp(L, R, &cmp)) return NULL;
    return KSO_BOOL(cmp > 0);
}
// compute L>=R
ks_obj ks_num_ge(ks_obj L, ks_obj R) {
    int cmp;
    if (!ks_num_cmp(L, R, &cmp)) return NULL;
    return KSO_BOOL(cmp >= 0);
}



// compute L|R
ks_obj ks_num_binor(ks_obj L, ks_obj R) {

    if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            return (ks_obj)ks_int_new(Lv | Rv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("|", L, R);
        }

        mpz_ior(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("|", L, R);
}

// compute L&R
ks_obj ks_num_binand(ks_obj L, ks_obj R) {

    if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            return (ks_obj)ks_int_new(Lv & Rv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("&", L, R);
        }

        mpz_and(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("&", L, R);
}



// compute L^R
ks_obj ks_num_binxor(ks_obj L, ks_obj R) {

    if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            return (ks_obj)ks_int_new(Lv ^ Rv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_init(Lz);
        mpz_init(Rz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Rz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("^", L, R);
        }

        mpz_xor(Vz, Lz, Rz);
        mpz_clear(Lz);
        mpz_clear(Rz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("^", L, R);
}



// compute L<<R
ks_obj ks_num_lshift(ks_obj L, ks_obj R) {

    if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (!Rf) {
            return ks_throw(ks_T_MathError, "Right-hand side of '<<' must fit in a C-style integer! (got %S)", R);
        }
        
        if (Rv < 0) return ks_throw(ks_T_MathError, "Right-hand side of '<<' must be >= 0! (got %S)", R);

        mpz_t Lz, Vz;
        mpz_init(Lz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR("<<", L, R);
        }

        mpz_mul_2exp(Vz, Lz, Rv);
        mpz_clear(Lz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR("<<", L, R);
}


// compute L>>R
ks_obj ks_num_rshift(ks_obj L, ks_obj R) {

    if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (!Rf) {
            return ks_throw(ks_T_MathError, "Right-hand side of '<<' must fit in a C-style integer! (got %S)", R);
        }
        if (Rv < 0) return ks_throw(ks_T_MathError, "Right-hand side of '<<' must be >= 0! (got %S)", R);

        mpz_t Lz, Vz;
        mpz_init(Lz);
        mpz_init(Vz);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clear(Lz);
            mpz_clear(Vz);
            KS_THROW_BOP_ERR(">>", L, R);
        }

        mpz_tdiv_q_2exp(Vz, Lz, Rv);
        mpz_clear(Lz);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_THROW_BOP_ERR(">>", L, R);
}


