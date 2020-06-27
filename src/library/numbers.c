/* numbers.c - implementation of basic numerics routine
 *
 * Essentially, helps with converting numbers and allowing liquid type conversions.
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// get hash of number
bool ks_num_hash(ks_obj self, ks_hash_t* out) {
    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1 : 1;
        return true;
    } else if (self->type == ks_type_int) {
        if (((ks_int)self)->isLong) {
            *out = ks_hash_bytes(sizeof(*((ks_int)self)->v_mpz->_mp_d) * ((ks_int)self)->v_mpz->_mp_size, (uint8_t*)((ks_int)self)->v_mpz->_mp_d);
            if (*out == 0) *out = 1;
            return true;
        } else {
            *out = (ks_hash_t)((ks_int)self)->v_i64;
            if (*out == 0) *out = 1;
            return true;
        }
    } else if (self->type == ks_type_float) {
        double vv = ((ks_float)self)->val;
        if (vv == round(vv)) *out = (ks_hash_t)vv;
        else *out = ks_hash_bytes(sizeof(vv), (uint8_t*)&vv);
        return true;
    } else if (self->type == ks_type_float) {
        double complex vv = ((ks_float)self)->val;
        if (vv == round((double)vv)) *out = (ks_hash_t)(double)vv;
        else *out = ks_hash_bytes(sizeof(vv), (uint8_t*)&vv);
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        if (*out == 0) *out = 1;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not hash '%T' object", self);
        return false;
    }
}

// get whether it fits 64 bit
bool ks_num_fits_int64(ks_obj self) {
    if (self->type == ks_type_bool) {
        return true;
    } else if (self->type == ks_type_int) {
        return !((ks_int)self)->isLong;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        return true;
    } else {
        return false;
    }
}

// get whether a type is integral
bool ks_num_is_integral(ks_obj self) {
    if (self->type == ks_type_bool) {
        return true;
    } else if (self->type == ks_type_int) {
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        return true;
    } else {
        return false;
    }
}



// get whether a type is numeric
bool ks_num_is_numeric(ks_obj self) {
    if (self->type == ks_type_bool) {
        return true;
    } else if (self->type == ks_type_int) {
        return true;
    } else if (self->type == ks_type_float) {
        return true;
    } else if (self->type == ks_type_complex) {
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        return true;
    } else {
        return false;
    }
}

// convert numeric to bool
bool ks_num_get_bool(ks_obj self, bool* out) {
    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? true : false;
        return true;
    } else if (self->type == ks_type_int) {
        return ks_int_sgn((ks_int)self) != 0;
    } else if (self->type == ks_type_float) {
        *out = round(((ks_float)self)->val);
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a boolean", self);
        return false;
    }
}


// convert object to int64_t
bool ks_num_get_int64(ks_obj self, int64_t* out) {
    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1 : 0;
        return true;
    } else if (self->type == ks_type_int) {
        if (((ks_int)self)->isLong) {
            #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
            // ensure it can fit
            if (!mpz_fits_slong_p(((ks_int)self)->v_mpz)) return false;

            // otherwise, get the signed integer
            *out = mpz_get_si(((ks_int)self)->v_mpz);
            return true;
            #else
            if (mpz_fits_slong_p(((ks_int)self)->v_mpz)) {
                *out = (int64_t)mpz_get_si(((ks_int)self)->v_mpz);
                return false;
            } else {
                size_t nbits = mpz_sizeinbase(((ks_int)self)->v_mpz, 2);

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
                        *out = -(int64_t)u_abs;
                    } else {
                        *out = (int64_t)u_abs;
                    }
                    return true;

                }
            }
            #endif
        } else {
            *out = ((ks_int)self)->v_i64;
            return true;
        }
    } else if (self->type == ks_type_float) {
        *out = round(((ks_float)self)->val);
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}

// get an MPZ from a given object
bool ks_num_get_mpz(ks_obj self, mpz_t out) {
    if (self->type == ks_type_bool) {
        mpz_set_si(out, (self == KSO_TRUE) ? 1 : 0);
        return true;
    } else if (self->type == ks_type_int) {
        if (((ks_int)self)->isLong) {
            mpz_set(out, ((ks_int)self)->v_mpz);
            return true;
        } else {
            #if SIZEOF_INT64_T == SIZEOF_SIGNED_LONG
            mpz_set_si(out, ((ks_int)self)->v_i64);
            return true;
            #else
            if (((ks_int)self)->v_i64 == (signed long)((ks_int)self)->v_i64) {
                mpz_set_si(out, (signed long)((ks_int)self)->v_i64);
                return true;
            } else {
                mpz_import(out, 1, 1, sizeof(((ks_int)self)->v_i64), 0, 0, &((ks_int)self)->v_i64);
                return true;
            }
            #endif
        }

    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        mpz_set_si(out, ((ks_Enum)self)->enum_idx);
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}
// convert object to double
bool ks_num_get_double(ks_obj self, double* out) {

    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1.0 : 0.0;
        return true;
    } else if (self->type == ks_type_int) {
        if (((ks_int)self)->isLong) {
            *out = mpz_get_d(((ks_int)self)->v_mpz);
            return true;
        } else {
            *out = ((ks_int)self)->v_i64;
            return true;
        }

    } else if (self->type == ks_type_float) {
        *out = ((ks_float)self)->val;
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}


// convert object to double complex
bool ks_num_get_double_complex(ks_obj self, double complex* out) {

    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1.0 : 0.0;
        return true;
    } else if (self->type == ks_type_int) {
        if (((ks_int)self)->isLong) {
            *out = mpz_get_d(((ks_int)self)->v_mpz);
            return true;
        } else {
            *out = ((ks_int)self)->v_i64;
            return true;
        }

    } else if (self->type == ks_type_float) {
        *out = ((ks_float)self)->val;
        return true;
    } else if (self->type == ks_type_complex) {
        *out = ((ks_complex)self)->val;
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}


// get the sign of a numerical quantity
bool ks_num_sgn(ks_obj self, int* out) {
    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1 : 0;
        return true;
    } else if (self->type == ks_type_int) {
        *out = ks_int_sgn((ks_int)self);
        return true;
    } else if (self->type == ks_type_float) {
        double v = ((ks_float)self)->val;
        *out = v == 0 ? 0 : (v > 0 ? 1 : -1);
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        int v = ((ks_Enum)self)->enum_idx;
        *out = v == 0 ? 0 : (v > 0 ? 1 : -1);
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not get the sign of a '%T' object", self);
        return false;
    }
}

// compute L <=> R
bool ks_num_cmp(ks_obj L, ks_obj R, int* out) {

    if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("<=>", L, R);
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
        mpz_inits(Lz, Rz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, NULL);
            KS_ERR_BOP_UNDEF("<=>", L, R);
        }

        int res = mpz_cmp(Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        *out = res == 0 ? 0 : (res > 0 ? 1 : -1);
        return true;

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("<=>", L, R);
}

// compare 2 numeric objects
bool ks_num_eq(ks_obj L, ks_obj R, bool* out) {
    if (L->type == ks_type_complex || R->type == ks_type_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("==", L, R);
        }
        return KSO_BOOL(Lv == Rv);
    } else if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("==", L, R);
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
        mpz_inits(Lz, Rz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, NULL);
            KS_ERR_BOP_UNDEF("==", L, R);
        }

        int res = mpz_cmp(Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        *out = res == 0;
        return true;

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("==", L, R);
}

/* MATH OPERATIONS */


// compute L+R
ks_obj ks_num_add(ks_obj L, ks_obj R) {

    if (L->type == ks_type_complex || R->type == ks_type_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("+", L, R);
        }
        return (ks_obj)ks_complex_new(Lv + Rv);
    } else if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("+", L, R);
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
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("+", L, R);
        }

        mpz_add(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("+", L, R);
}


// compute L-R
ks_obj ks_num_sub(ks_obj L, ks_obj R) {

    if (L->type == ks_type_complex || R->type == ks_type_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("-", L, R);
        }
        return (ks_obj)ks_complex_new(Lv - Rv);
    } else if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("-", L, R);
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
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("-", L, R);
        }


        mpz_sub(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);

        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("-", L, R);
}

// compute L*R
ks_obj ks_num_mul(ks_obj L, ks_obj R) {
    if (L->type == ks_type_complex || R->type == ks_type_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("*", L, R);
        }


        return (ks_obj)ks_complex_new(Lv * Rv);
    } else if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("*", L, R);
        }

        return (ks_obj)ks_float_new(Lv * Rv);
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Lf && Rf) {
            if (Lv > INT64_MAX / Rv || Lv < INT64_MIN / Rv) {
                // overflow; let it flow through and it end up using mpz types

            } else {
                return (ks_obj)ks_int_new(Lv * Rv);
            }
        }

        mpz_t Lz, Rz, Vz;
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("*", L, R);
        }

        mpz_mul(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("*", L, R);
}

// compute L/R
ks_obj ks_num_div(ks_obj L, ks_obj R) {
    if (L->type == ks_type_complex || R->type == ks_type_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("/", L, R);
        }
        return (ks_obj)ks_complex_new(Lv / Rv);
    } else if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("/", L, R);
        }
        return (ks_obj)ks_float_new(Lv / Rv);
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Rf && Rv == 0) return ks_throw_fmt(ks_type_MathError, "Division by 0");

        if (Lf && Rf) {
            return (ks_obj)ks_int_new(Lv / Rv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("/", L, R);
        }

        mpz_tdiv_q(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("*", L, R);
}

// compute L%R
ks_obj ks_num_mod(ks_obj L, ks_obj R) {
    if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("%", L, R);
        }
        if (Rv == 0) return ks_throw_fmt(ks_type_MathError, "Modulo by 0");

        return (ks_obj)ks_float_new(fmod(Lv, Rv));
    } else if (ks_num_is_integral(L) && ks_num_is_integral(R)) {

        // values for left and right
        int64_t Lv, Rv;

        // get whether either fits
        bool Lf = ks_num_get_int64(L, &Lv);
        bool Rf = ks_num_get_int64(R, &Rv);

        if (Rf && Rv == 0) return ks_throw_fmt(ks_type_MathError, "Modulo by 0");

        if (Lf && Rf) {

            int64_t Vv = Lv % Rv;
            if (Vv < 0) Vv += Rv;
            return (ks_obj)ks_int_new(Vv);
        }

        mpz_t Lz, Rz, Vz;
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("%", L, R);
        }

        mpz_mod(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("%", L, R);
}

// compute L**R
ks_obj ks_num_pow(ks_obj L, ks_obj R) {
    if (L->type == ks_type_complex || R->type == ks_type_complex) {
        double complex Lv, Rv;
        if (
            !ks_num_get_double_complex(L, &Lv) ||
            !ks_num_get_double_complex(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("**", L, R);
        }
        return (ks_obj)ks_complex_new(cpow(Lv, Rv));
    } else if (L->type == ks_type_float || R->type == ks_type_float) {
        double Lv, Rv;
        if (
            !ks_num_get_double(L, &Lv) ||
            !ks_num_get_double(R, &Rv)
        ) {
            ks_catch_ignore();
            KS_ERR_BOP_UNDEF("**", L, R);
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
        if (Lsgn < 0) return ks_throw_fmt(ks_type_MathError, "Negative number in base: %S", L);

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
            return ks_throw_fmt(ks_type_MathError, "Overflow in pow(), exponent too large: %S", R);
        }


        // otherwise, compute this way:
        mpz_t Lz, Vz;
        mpz_inits(Lz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clears(Lz, Vz, NULL);
            KS_ERR_BOP_UNDEF("**", L, R);
        }


        mpz_pow_ui(Vz, Lz, Rv);
        mpz_clears(Lz, NULL);

        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("**", L, R);
}



// compute -L
ks_obj ks_num_neg(ks_obj L) {
    if (L->type == ks_type_complex) {
        return (ks_obj)ks_complex_new(-((ks_complex)L)->val);
    } else if (L->type == ks_type_float) {
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
        mpz_inits(Lz, NULL);

        if (
            !ks_num_get_mpz(L, Lz)
        ) {
            // problem converting
            mpz_clears(Lz, NULL);
            KS_ERR_UOP_UNDEF("-", L);
        }

        mpz_neg(Lz, Lz);
        return (ks_obj)ks_int_new_mpz_n(Lz);
    } else {

    }

    // default: undefined
    KS_ERR_UOP_UNDEF("-", L);
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
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("|", L, R);
        }

        mpz_ior(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("|", L, R);
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
        mpz_inits(Lz, Rz, Vz, NULL);

        if (
            !ks_num_get_mpz(L, Lz) || 
            !ks_num_get_mpz(R, Rz)
        ) {
            // problem converting
            mpz_clears(Lz, Rz, Vz, NULL);
            KS_ERR_BOP_UNDEF("&", L, R);
        }

        mpz_and(Vz, Lz, Rz);
        mpz_clears(Lz, Rz, NULL);
        return (ks_obj)ks_int_new_mpz_n(Vz);

    } else {

    }

    // default: undefined
    KS_ERR_BOP_UNDEF("&", L, R);
}



