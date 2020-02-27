/* types/error.c - type representing a generic error type */

#include "ks.h"







/* exporting functionality */

struct ks_type T_error, *ks_T_error = &T_error;

void ks_init__error() {
    T_error = KS_TYPE_INIT();

    ks_type_set_namec(ks_T_error, "Error");
}


