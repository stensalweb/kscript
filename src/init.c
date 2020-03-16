/* init.c - holds code for the initialization of the kscript library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"


// the static version information
static ks_version_t this_version = (ks_version_t){

    .major = KS_VERSION_MAJOR,
    .minor = KS_VERSION_MINOR,
    .patch = KS_VERSION_PATCH,

    .date = __DATE__,
    .time = __TIME__

};

// return version info
const ks_version_t* ks_version() {
    return &this_version;
}

// global interpreter lock
ks_mutex ks_GIL = NULL;

// global vars
ks_dict ks_globals = NULL;

// initialize the whole library
bool ks_init() {
    ks_util_init();

    ks_mem_init();

    ks_log_init();

    // initialize the builtin types
    ks_type_type_init();
    ks_type_none_init();
    ks_type_bool_init();
    ks_type_int_init();
    ks_type_float_init();
    ks_type_str_init();
    ks_type_tuple_init();
    ks_type_list_init();
    ks_type_dict_init();
    ks_type_Error_init();

    ks_type_thread_init();

    ks_type_cfunc_init();
    ks_type_kfunc_init();
    ks_type_pfunc_init();
    ks_type_code_init();
    ks_type_ast_init();
    ks_type_parser_init();

    ks_init_funcs();


    // construct a mutex
    ks_GIL = ks_mutex_new();


//    ks_type_vm_init();


    // initialize globals

    ks_globals = ks_dict_new(0, NULL);
    ks_dict_set_cn(ks_globals, (ks_dict_ent_c[]){

        /* types */

        {"bool",       KS_NEWREF(ks_type_bool)},
        {"int",        KS_NEWREF(ks_type_int)},
        {"float",      KS_NEWREF(ks_type_float)},
        
        {"str",        KS_NEWREF(ks_type_str)},
        {"list",       KS_NEWREF(ks_type_list)},
        
        {"tuple",      KS_NEWREF(ks_type_tuple)},
        {"dict",       KS_NEWREF(ks_type_dict)},

        {"Error",      KS_NEWREF(ks_type_Error)},

        {"thread",     KS_NEWREF(ks_type_thread)},

        /* functions */

        {"hash",       KS_NEWREF(ks_F_hash)},
        {"repr",       KS_NEWREF(ks_F_repr)},
        {"exit",       KS_NEWREF(ks_F_exit)},
        {"print",      KS_NEWREF(ks_F_print)},
        {"len",        KS_NEWREF(ks_F_len)},
        {"sleep",      KS_NEWREF(ks_F_sleep)},
        {"typeof",     KS_NEWREF(ks_F_typeof)},

        /* math constants */

        {"PI",         (ks_obj)ks_float_new(3.141592653589793238462643383279502884)},
        {"E",          (ks_obj)ks_float_new(2.7182818284590452353602874713526625)},
        {"PHI",        (ks_obj)ks_float_new(1.6180339887498948482045868343656381177)},


        // end
        {NULL, NULL}
    });



    // success
    return true;
}

