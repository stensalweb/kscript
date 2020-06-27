/* numbers.c - implementation of basic numerics routine
 *
 * Essentially, helps with converting numbers and allowing liquid type conversions
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// convert object to int64_t
bool ks_num_getint64(ks_obj self, int64_t* out) {
    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1 : 0;
        return true;
    } else if (self->type == ks_type_int) {
        *out = ((ks_int)self)->val;
        return true;
    } else if (self->type == ks_type_float) {
        *out = round(((ks_float)self)->val);
        return true;
    } else if (self->type == ks_type_long) {
        return ks_long_getint64((ks_long)self, out);
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}

// convert object to double
bool ks_num_getdouble(ks_obj self, double* out) {

    if (self->type == ks_type_bool) {
        *out = (self == KSO_TRUE) ? 1.0 : 0.0;
        return true;
    } else if (self->type == ks_type_int) {
        *out = ((ks_int)self)->val;
        return true;
    } else if (self->type == ks_type_float) {
        *out = ((ks_float)self)->val;
        return true;
    } else if (self->type == ks_type_long) {
        *out = mpz_get_d(((ks_long)self)->val);
        return true;
    } else if (ks_type_issub(self->type, ks_type_Enum)) {
        *out = ((ks_Enum)self)->enum_idx;
        return true;
    } else {
        ks_throw_fmt(ks_type_MathError, "Could not interpret '%T' object as a double", self);
        return false;
    }
}




