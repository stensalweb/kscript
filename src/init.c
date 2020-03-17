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

// the paths to search
ks_list ks_paths = NULL;

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
    ks_type_complex_init();
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
    ks_type_module_init();

    ks_init_funcs();


    // construct a mutex to lock the global interpreter
    ks_GIL = ks_mutex_new();

    // read in paths
    ks_paths = ks_list_new(0, NULL);

    ks_str myp = ks_str_new(".");
    ks_list_push(ks_paths, (ks_obj)myp);
    KS_DECREF(myp);

    // get the environment variable
    char* env_ksp = getenv("KS_PATH");

    if (env_ksp != NULL) {
        int slen = strlen(env_ksp);
        char* dup = ks_malloc(slen + 1);
        strncpy(dup, env_ksp, slen);

        char* tok = strtok(dup, ":");

        while (tok != NULL) {
            // add to the paths
            myp = ks_str_new(tok);
            ks_list_push(ks_paths, (ks_obj)myp);
            KS_DECREF(myp);

            // grab next token
            tok = strtok(NULL, ":");
        }

        ks_free(dup);
    }

    // initialize globals
    ks_globals = ks_dict_new(0, NULL);
    ks_dict_set_cn(ks_globals, (ks_dict_ent_c[]){

        /* types */

        {"bool",           KS_NEWREF(ks_type_bool)},
        {"int",            KS_NEWREF(ks_type_int)},
        {"float",          KS_NEWREF(ks_type_float)},
        {"complex",        KS_NEWREF(ks_type_complex)},
        
        {"str",            KS_NEWREF(ks_type_str)},
        {"list",           KS_NEWREF(ks_type_list)},
        
        {"tuple",          KS_NEWREF(ks_type_tuple)},
        {"dict",           KS_NEWREF(ks_type_dict)},

        {"Error",          KS_NEWREF(ks_type_Error)},
        {"SyntaxError",    KS_NEWREF(ks_type_SyntaxError)},
        {"MathError",      KS_NEWREF(ks_type_MathError)},
        {"AttrError",      KS_NEWREF(ks_type_AttrError)},
        {"KeyError",       KS_NEWREF(ks_type_KeyError)},

        {"thread",         KS_NEWREF(ks_type_thread)},

        /* functions */

        {"hash",           KS_NEWREF(ks_F_hash)},
        {"repr",           KS_NEWREF(ks_F_repr)},
        {"exit",           KS_NEWREF(ks_F_exit)},
        {"print",          KS_NEWREF(ks_F_print)},
        {"len",            KS_NEWREF(ks_F_len)},
        {"sleep",          KS_NEWREF(ks_F_sleep)},
        {"typeof",         KS_NEWREF(ks_F_typeof)},
        {"__import__",     KS_NEWREF(ks_F_import)},

        /* math constants */

        {"PI",             (ks_obj)ks_float_new(3.141592653589793238462643383279502884)},
        {"E",              (ks_obj)ks_float_new(2.7182818284590452353602874713526625)},
        {"PHI",            (ks_obj)ks_float_new(1.6180339887498948482045868343656381177)},

        /* misc constants */
        {"NONE",           KS_NEWREF(KSO_NONE)},
        {"NaN",            KS_NEWREF(KS_NAN)},
        {"TRUE",           KS_NEWREF(KSO_TRUE)},
        {"FALSE",          KS_NEWREF(KSO_FALSE)},

        // end
        {NULL, NULL}
    });



    // success
    return true;
}

