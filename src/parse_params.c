/* parse_params.c - implementation of cfunc parameter parsing helper
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


bool ks_parse_params(int n_args, ks_obj* args, const char* fmt, ...) {
    // argument list
    va_list ap;
    va_start(ap, fmt);

    // current argument place
    int args_i = 0;

    char argspec[256];

    int i = 0;
    while (fmt[i]) {

        // skip whitespace
        if (fmt[i] == ' ') {
            while (fmt[i] == ' ') i++;
            continue;
        }

        // now, parse the format:
        // NAME%ARGSPEC

        bool isQuest = fmt[i] == '?';
        if (isQuest) i++;

        int name_i = i;
        while (fmt[i] && fmt[i] != '%') i++;

        int name_len = i - name_i;

        if (fmt[i] != '%') {
            ks_error("ks", "Incorrect usage of ks_parse_params()!");
            exit(1);
            return false;
        }
        i++;

        // get the 'argspec' part
        int asi = 0;

        while (fmt[i] && fmt[i] != ' ') {
            argspec[asi++] = fmt[i++];
        }

        argspec[asi] = '\0';

        // now, we have the name & argspec

        if (strcmp(argspec, "i64") == 0) {
            // this should be a 64 bit integer
            int64_t* val_p = va_arg(ap, int64_t*);

            if (args_i >= n_args) {
                if (isQuest) {
                    // can skip
                    continue;
                } else {
                    continue;
                    ks_throw_fmt(ks_type_Error, "Ran out of arguments!");
                    return false;
                }
            }

            // claim this argument
            ks_obj arg = args[args_i++];

            // if it's not integral, throw an error
            if (!ks_num_is_integral(arg)) {
                ks_throw_fmt(ks_type_TypeError, "Incorrect type for '%.*s', expected an 'int', but got '%T'", name_len, &fmt[name_i], arg);
                return false;
            }

            // try to get it through the numerics
            if (!ks_num_get_int64(arg, val_p)) {
                ks_catch_ignore();
                ks_throw_fmt(ks_type_MathError, "Parameter '%.*s' was too large (couldn't convert to 64 bit value)", name_len, &fmt[name_i]);
                //printf("TESt\n");
                return false;
            }

        } else if (strcmp(argspec, "f") == 0) {
            // this should be a 64 bit integer
            double* val_p = va_arg(ap, double*);

            if (args_i >= n_args) {
                if (isQuest) {
                    // can skip
                    continue;
                } else {
                    continue;
                    ks_throw_fmt(ks_type_Error, "Ran out of arguments!");
                    return false;
                }
            }

            // claim this argument
            ks_obj arg = args[args_i++];

            // try to get it through the numerics
            if (!ks_num_get_double(arg, val_p)) {
                ks_catch_ignore();
                ks_throw_fmt(ks_type_MathError, "Parameter '%.*s' was too large (couldn't convert to 64 bit floating point value)", name_len, &fmt[name_i]);
                //printf("TESt\n");
                return false;
            }

        }else if (strcmp(argspec, "*") == 0) {
            // should be (val, type)
            ks_obj* val_p = va_arg(ap, ks_obj*);

            // claim type
            ks_type type = va_arg(ap, ks_type);

            if (args_i >= n_args) {
                if (isQuest) {
                    // can skip
                    continue;
                } else {
                    continue;
                    ks_throw_fmt(ks_type_Error, "Ran out of arguments!");
                    return false;
                }
            }

            // claim this argument
            ks_obj arg = args[args_i++];

            // if it's not integral, throw an error
            if (!ks_type_issub(arg->type, type)) {
                ks_throw_fmt(ks_type_TypeError, "Incorrect type for '%.*s', expected '%S', but got '%T'", name_len, &fmt[name_i], type->__name__, arg);
                return false;
            }

            *val_p = arg;
        } else if (strcmp(argspec, "s") == 0) {
            // should be (string)
            ks_obj* val_p = va_arg(ap, ks_obj*);

            if (args_i >= n_args) {
                if (isQuest) {
                    // can skip
                    continue;
                } else {
                    continue;
                    ks_throw_fmt(ks_type_Error, "Ran out of arguments!");
                    return false;
                }
            }

            // claim this argument
            ks_obj arg = args[args_i++];

            // if it's not integral, throw an error
            if (!ks_type_issub(arg->type, ks_type_str)) {
                ks_throw_fmt(ks_type_TypeError, "Incorrect type for '%.*s', expected 'str', but got '%T'", name_len, &fmt[name_i], arg);
                return false;
            }

            *val_p = arg;

        } else if (strcmp(argspec, "any") == 0) {
            // should be (any type)
            ks_obj* val_p = va_arg(ap, ks_obj*);

            if (args_i >= n_args) {
                if (isQuest) {
                    // can skip
                    continue;
                } else {
                    continue;
                    ks_throw_fmt(ks_type_Error, "Ran out of arguments!");
                    return false;
                }
            }

            // claim this argument
            ks_obj arg = args[args_i++];

            *val_p = arg;
        } else if (strcmp(argspec, "iter") == 0) {
            // should be (iterable)
            ks_obj* val_p = va_arg(ap, ks_obj*);

            if (args_i >= n_args) {
                if (isQuest) {
                    // can skip
                    continue;
                } else {
                    continue;
                    ks_throw_fmt(ks_type_Error, "Ran out of arguments!");
                    return false;
                }
            }

            // claim this argument
            ks_obj arg = args[args_i++];

            // if it's not integral, throw an error
            if (!ks_is_iterable(arg)) {
                ks_throw_fmt(ks_type_TypeError, "Incorrect type for '%.*s', expected an iterable, but got '%T'", name_len, &fmt[name_i], arg);
                return false;
            }

            *val_p = arg;

        } else {
            ks_error("ks", "Incorrect usage of ks_parse_params()! (unknown argspec)");
            exit(1);
            return false;
        }

    }

    va_end(ap);

    return true;

}

