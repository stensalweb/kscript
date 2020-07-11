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

    // specifiers
    char spec[256];

    // and field name
    char field[256];

    int i = 0;
    while (fmt[i]) {
        while (fmt[i] == ' ') i++;

        if (fmt[i] == '%') {
            i++;
            int spec_i = i;
            // have format specifier
            while (fmt[i] && !isalpha(fmt[i])) {
                spec[i - spec_i] = fmt[i];
                i++;
            }

            spec[i - spec_i] = '\0';

            int field_i = i;

            // have format field
            while (fmt[i] && fmt[i] != ' ') {
                field[i - field_i] = fmt[i];
                i++;
            }

            field[i - field_i] = '\0';


            if (strcmp(field, "i") == 0) {
                // parse integer
                int val = va_arg(ap, int);

                ks_int new_obj = ks_int_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);
            } else if (strcmp(field, "f") == 0) {
                // parse integer
                double val = va_arg(ap, double);

                ks_float new_obj = ks_float_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);

            } else if (strcmp(field, "z") == 0) {
                // %z - ks_ssize_t
                // %+z - int len, ks_ssize_t*

                // whether or not to do an array
                bool doMult = strchr(spec, '+') != NULL;

                if (doMult) {

                    int num = va_arg(ap, int);
                    ks_ssize_t* vals = va_arg(ap, ks_ssize_t*);

                    int i;
                    for (i = 0; i < num; ++i) {

                        ks_int new_obj = ks_int_new(vals[i]);
                        ks_list_push(b_list, (ks_obj)new_obj);
                        KS_DECREF(new_obj);
                    }

                } else {
                    ks_ssize_t val = va_arg(ap, ks_ssize_t);

                    ks_int new_obj = ks_int_new(val);
                    ks_list_push(b_list, (ks_obj)new_obj);
                    KS_DECREF(new_obj);

                }
            } else if (strcmp(field, "s") == 0) {
                // parse string
                char* val = va_arg(ap, char*);

                ks_str new_obj = ks_str_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);

            } else {
                ks_error("Unknown format specifier in `ks_build_tuple`, got '%%%s'", field);
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

