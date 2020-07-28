/* init.c - handles initialization of the kscript library
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// whether or not kscript has been initialized
static bool hasInit = false;

// the static version information
static ks_version_t this_version = (ks_version_t){

    .major = KS_VERSION_MAJOR,
    .minor = KS_VERSION_MINOR,
    .patch = KS_VERSION_PATCH,

    .build_type = KS_BUILD_STR,

    .date = __DATE__,
    .time = __TIME__

};

// return version info
const ks_version_t* ks_version() {
    return &this_version;
}

// globals
ks_dict ks_globals = NULL;

// initialize library
bool ks_init(int verbose) {
    if (hasInit) return true;


    // First, initialize types
    ks_init_T_logger();

    // set up the kscript logger to use the given verbosity
    ks_logger _ks = ks_logger_get("ks", true);
    ks_log_c_set("ks", verbose);
    KS_DECREF(_ks);

    ks_init_T_obj();
    ks_init_T_type();
    ks_init_T_Error();

    ks_init_T_none();
    ks_init_T_bool();
    ks_init_T_int();
    ks_init_T_float();
    ks_init_T_complex();
    ks_init_T_str();
    ks_init_T_Enum();

    ks_init_T_str_builder();


    ks_init_T_list();
    ks_init_T_tuple();
    ks_init_T_slice();
    ks_init_T_dict();
    ks_init_T_cfunc();
    ks_init_T_memberfunc();
    ks_init_T_thread();
    ks_init_T_parser();
    ks_init_T_ast();
    ks_init_T_code();
    ks_init_T_stack_frame();
    ks_init_T_kfunc();


    // initialize others
    ks_init_funcs();


    // set up globals
    ks_globals = ks_dict_new_c(KS_KEYVALS(

        /* Types */
        {"none",                   KS_NEWREF(ks_T_none)},
        {"bool",                   KS_NEWREF(ks_T_bool)},
        {"int",                    KS_NEWREF(ks_T_int)},
        {"float",                  KS_NEWREF(ks_T_float)},
        {"complex",                KS_NEWREF(ks_T_complex)},

        {"str",                    KS_NEWREF(ks_T_str)},

        {"logger",                 KS_NEWREF(ks_T_logger)},

        {"list",                   KS_NEWREF(ks_T_list)},
        {"tuple",                  KS_NEWREF(ks_T_tuple)},
        {"slice",                  KS_NEWREF(ks_T_slice)},
        {"dict",                   KS_NEWREF(ks_T_dict)},

        {"type",                   KS_NEWREF(ks_T_type)},
        {"obj",                    KS_NEWREF(ks_T_obj)},
        {"thread",                 KS_NEWREF(ks_T_thread)},

        /* Errors */

        {"Error",                  KS_NEWREF(ks_T_Error)},
        {"MathError",              KS_NEWREF(ks_T_MathError)},


        /* Functions */

        {"print",                  KS_NEWREF(ks_F_print)},

        {"typeof",                 KS_NEWREF(ks_F_typeof)},

        {"repr",                   KS_NEWREF(ks_F_repr)},
        {"len",                    KS_NEWREF(ks_F_len)},

        {"chr",                    KS_NEWREF(ks_F_chr)},
        {"ord",                    KS_NEWREF(ks_F_ord)},

    ));


    // success
    return hasInit = true;
}

