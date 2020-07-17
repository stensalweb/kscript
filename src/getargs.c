/* getargs.c - helper function for getting/parsing arguments in Cfuncs
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// maximum argument name
#define MAX_ARGNAME 256

// maximum field size
#define MAX_FIELD 256

// Attempt to parse arguments to a Cfunc, and return whether it was successful.
// Given a format string (which describes which args should be parsed and of what types), set variables given
// NOTE: Returns success, or returns false and throws an error
bool ks_getargs(int n_args, ks_obj* args, const char* fmt, ...) {
    // start varargs
    va_list ap;
    va_start(ap, fmt);

    const char* ofmt = fmt;

    // return status
    bool rst = false;

    // current argument index (cai)
    // i.e. current argument is args[cai]
    int cai = 0;

    // argument & configuration field
    char argname[MAX_ARGNAME], field[MAX_FIELD];

    // while we have more to parse
    while (*fmt) {

        // skip whitespace
        while (*fmt && *fmt == ' ') fmt++;
        if (!*fmt) break;

        // whether or not we are taking varargs
        bool isVarArg = *fmt == '*';
        if (isVarArg) fmt++;


        // pointer to argname
        int i = 0;

        while (*fmt && *fmt != ':') {
            argname[i++] = *fmt++;
        }
        // NUL-terminate
        argname[i] = '\0';


        if (isVarArg) {

            // we are going to store the rest of the arguments into the next pointer
            int* to_n_args = va_arg(ap, int*);

            ks_obj** to_args = va_arg(ap, ks_obj**);

            // set them
            *to_n_args = n_args - cai;
            *to_args = &args[cai];

            // now, done
            break;

        } else {

            // now, parse out a single argument:

            // current argument input
            ks_obj cargin = args[cai++];

            // get current argument destination (i.e. where we are going to put it)
            ks_obj* cargto = va_arg(ap, ks_obj*);

            // what is the required type?
            ks_type req_type = NULL;

            if (*fmt == ':') {
                // parse what type it should be
                fmt++;

                if (*fmt == '*') {
                    fmt++;

                    // read from varargs
                    req_type = va_arg(ap, ks_type);
                    assert (req_type->type == ks_T_type && "required type with ':*' was not type!");
                } else {
                    // for now, error
                    ks_error("ks", "Given incorrect format for ks_getargs: %s!", ofmt);
                    goto getargs_end;
                }
            }


            if (ks_type_issub(cargin->type, req_type)) {
                *cargto = cargin;
            } else {
                ks_throw(ks_T_ArgError, "Expected argument '%s' to be of type '%S', but was of type '%T'", argname, req_type, cargin);
                goto getargs_end;
            }
        }

    }


    rst = true;
    getargs_end:

    // end varargs
    va_end(ap);


    // return status
    return rst;
}
