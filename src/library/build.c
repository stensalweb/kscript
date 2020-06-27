/* build.c - value building functions
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// Build a tuple from C-style variables
// For example, `ks_build_tuple("%i %f", 2, 3.0)` returns (2, 3.0)
// NOTE: Returns a new reference, or NULL if there was an error thrown
KS_API ks_tuple ks_build_tuple(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ks_list b_list = ks_list_new(0, NULL);

    // field specifier
    char spec[256];

    int i;
    while (fmt[i]) {
        while (fmt[i] == ' ') i++;

        if (fmt[i] == '%') {
            i++;
            int spec_i = i;
            // have format specifier
            while (fmt[i] && fmt[i] != ' ') {
                spec[i - spec_i] = fmt[i];
                i++;
            }

            spec[spec_i] = '\0';

            if (strcmp(spec, "i") == 0) {
                // parse integer
                int val = va_arg(ap, int);

                ks_int new_obj = ks_int_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);
            } else if (strcmp(spec, "f") == 0) {
                // parse integer
                double val = va_arg(ap, double);

                ks_float new_obj = ks_float_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);
            } else {
                ks_error("Unknown format specifier in `ks_build_tuple`, got '%%%s'", spec);
                exit(1);
            }
        
        }

    }

    va_end(ap);

    // convert to tuple and return
    ks_tuple res = ks_tuple_new(b_list->len, b_list->elems);
    KS_DECREF(b_list);
    return res;
}

