/* init.c - holds code for the initialization of the kscript library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"

#include "whereami.h"


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

// global interpreter lock
ks_mutex ks_GIL = NULL;

// global vars
ks_dict ks_globals = NULL, ks_internal_globals = NULL;

// the paths to search
ks_list ks_paths = NULL;


/* signal handlers */

static void handle_signal(int sig_num) {

    // do nothing
    return;
    
}


// initialize the whole library
bool ks_init() {

    // handle signals
    //signal(SIGINT, handle_signal);

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
    ks_type_complex_init();

    ks_type_tuple_init();
    ks_type_list_init();
    ks_type_dict_init();
    ks_type_Error_init();

    ks_type_thread_init();
    ks_type_iostream_init();

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


    // find out the full path of 'argv[0]'
    int length = wai_getExecutablePath(NULL, 0, NULL);
    char* full_path = ks_malloc(length + 1);
    int dir_length = 0;
    wai_getExecutablePath(full_path, length, &dir_length);
    full_path[dir_length] = '\0';

    ks_str full_path_o = ks_str_new(full_path);
    ks_str lib_path_o = ks_fmt_c("%s/../lib", full_path);

    // add a module lookup path to it
    ks_str module_path_o = ks_fmt_c("%S/kscript/modules", lib_path_o);
    ks_list_push(ks_paths, (ks_obj)module_path_o);
    KS_DECREF(module_path_o);

    module_path_o = ks_fmt_c("%S/kscript", lib_path_o);
    ks_list_push(ks_paths, (ks_obj)module_path_o);
    KS_DECREF(module_path_o);

    ks_str tmpstr = NULL;
    //#if defined(KS__LINUX) || defined(KS__CYGWIN)
    // add some default places to look
    tmpstr = ks_str_new(KS_PREFIX "/lib/kscript/modules");
    ks_list_push(ks_paths, (ks_obj)tmpstr);
    KS_DECREF(tmpstr);
    //#endif
    
    // initialize internal globals
    ks_internal_globals = ks_dict_new(0, NULL);
    // set it in the internal global dictionary
    ks_dict_set_cn(ks_internal_globals, (ks_dict_ent_c[]){
        {"KSCRIPT_BINARY_DIR", (ks_obj)full_path_o},
        {"KSCRIPT_LIB_DIR", (ks_obj)lib_path_o},
        {NULL, NULL}
    });

    ks_iostream ks__stdin  = ks_iostream_new_extern(stdin, "r");
    ks_iostream ks__stdout = ks_iostream_new_extern(stdout, "w");
    ks_iostream ks__stderr = ks_iostream_new_extern(stderr, "w");

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
        {"OpError",        KS_NEWREF(ks_type_OpError)},
        {"ToDoError",      KS_NEWREF(ks_type_ToDoError)},
        
        {"iostream",       KS_NEWREF(ks_type_iostream)},

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
        {"iter",           KS_NEWREF(ks_F_iter)},
        {"next",           KS_NEWREF(ks_F_next)},
        {"open",           KS_NEWREF(ks_F_open)},
        {"sort",           KS_NEWREF(ks_F_sort)},
        {"filter",         KS_NEWREF(ks_F_filter)},
        {"map",            KS_NEWREF(ks_F_map)},
        {"range",          KS_NEWREF(ks_F_range)},

        /* math constants */

        {"PI",             (ks_obj)ks_float_new(3.141592653589793238462643383279502884)},
        {"E",              (ks_obj)ks_float_new(2.7182818284590452353602874713526625)},
        {"PHI",            (ks_obj)ks_float_new(1.6180339887498948482045868343656381177)},

        /* misc constants */
        {"none",           KS_NEWREF(KSO_NONE)},
        {"NaN",            KS_NEWREF(KS_NAN)},
        {"true",           KS_NEWREF(KSO_TRUE)},
        {"false",          KS_NEWREF(KSO_FALSE)},

        /* other globals */

        {"__stdin__",      (ks_obj)ks__stdin},
        {"__stdout__",     (ks_obj)ks__stdout},
        {"__stderr__",     (ks_obj)ks__stderr},

        {"__globals__",    KS_NEWREF(ks_globals)},

        // end
        {NULL, NULL}
    });



    // success
    return true;
}

