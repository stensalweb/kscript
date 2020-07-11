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
ks_dict ks_globals = NULL;

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
    ks_type_namespace_init();
    ks_type_Error_init();
    ks_type_Enum_init();

    ks_type_thread_init();
    ks_type_iostream_init();

    ks_type_slice_init();

    ks_type_cfunc_init();
    ks_type_kfunc_init();
    ks_type_pfunc_init();
    ks_type_code_init();
    ks_type_ast_init();
    ks_type_parser_init();
    ks_type_module_init();
    ks_type_blob_init();


    ks_init_funcs();


    // construct a mutex to lock the global interpreter
    ks_GIL = ks_mutex_new();

    // read in paths to search for modules 
    // ORDER:
    //   Literal paths
    //   KS_PATH variable
    //   Relative to executable
    ks_paths = ks_list_new(0, NULL);

    /* Literal paths */

    // literal paths
    const char* path_lits[] = {
        ".",
        NULL,
    };

    const char** this_path = &path_lits[0];

    // add literals
    while (*this_path) {
        ks_str tmp_path = ks_str_new((char*)*this_path);
        ks_list_push(ks_paths, (ks_obj)tmp_path);
        KS_DECREF(tmp_path);

        this_path++;
    }

    /* KS_PATH paths */

    // get the environment variable (if it exists),
    //   add those paths
    char* env_ksp = getenv("KS_PATH");
    if (env_ksp != NULL) {
        // make a copy of the `KS_PATH` variable,
        //   since strtok() is destructive
        int slen = strlen(env_ksp);
        char* dup_ksp = ks_malloc(slen + 1);
        strncpy(dup_ksp, env_ksp, slen);

        // iterate through tokens seperated by ':',
        //   the seperator
        char* tok = strtok(dup_ksp, ":");

        while (tok != NULL) {
            // add to paths variable
            ks_str tmp_path = ks_str_new(tok);
            ks_list_push(ks_paths, (ks_obj)tmp_path);
            KS_DECREF(tmp_path);

            // grab next token
            tok = strtok(NULL, ":");
        }

        // free temporary, mutated version
        ks_free(dup_ksp);
    }


    /* Relative-To-Executable paths */


    // find out the full path of 'argv[0]'
    int length = wai_getExecutablePath(NULL, 0, NULL);
    char* full_path = ks_malloc(length + 1);
    int dir_length = 0;
    wai_getExecutablePath(full_path, length, &dir_length);
    full_path[dir_length] = '\0';


    ks_debug("wai_getExecutablePath: %s", full_path);

    // relative paths 
    const char* path_rels[] = {
        "/../lib/ksm",
        NULL,
    };

    // buffer for resolving paths
    char* tmpbuf = NULL;


    // loop through relative paths
    this_path = &path_rels[0];
    while (*this_path) {
        int sl = strlen(*this_path);
        tmpbuf = ks_realloc(tmpbuf, sl + length + 4);
        snprintf(tmpbuf, sl + length + 4, "%s%s", full_path, *this_path);

        // TODO: replace ../ and symbolic links?
        ks_str tmp_path = ks_str_new(tmpbuf);
        ks_list_push(ks_paths, (ks_obj)tmp_path);
        KS_DECREF(tmp_path);


        this_path++;
    }

    ks_free(tmpbuf);

    /*
    // initialize internal globals
    ks_internal_globals = ks_dict_new(0, NULL);
    // set it in the internal global dictionary
    ks_dict_set_cn(ks_internal_globals, (ks_dict_ent_c[]){
        {"KSCRIPT_BINARY_DIR",   (ks_obj)full_path_o},
        {"KSCRIPT_LIB_DIR",      (ks_obj)lib_path_o},
        {NULL, NULL}
    });
    */

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
        
        {"blob",           KS_NEWREF(ks_type_blob)},
        {"str",            KS_NEWREF(ks_type_str)},
        
        {"list",           KS_NEWREF(ks_type_list)},
        {"tuple",          KS_NEWREF(ks_type_tuple)},
        {"dict",           KS_NEWREF(ks_type_dict)},
        {"namespace",      KS_NEWREF(ks_type_namespace)},

        {"slice",          KS_NEWREF(ks_type_slice)},

        {"Error",          KS_NEWREF(ks_type_Error)},
        {"SyntaxError",    KS_NEWREF(ks_type_SyntaxError)},
        {"MathError",      KS_NEWREF(ks_type_MathError)},
        {"AttrError",      KS_NEWREF(ks_type_AttrError)},
        {"KeyError",       KS_NEWREF(ks_type_KeyError)},
        {"OpError",        KS_NEWREF(ks_type_OpError)},
        {"ToDoError",      KS_NEWREF(ks_type_ToDoError)},
        
        {"Enum",           KS_NEWREF(ks_type_Enum)},


        {"iostream",       KS_NEWREF(ks_type_iostream)},

        {"thread",         KS_NEWREF(ks_type_thread)},

        /* functions */

        {"hash",           KS_NEWREF(ks_F_hash)},
        {"repr",           KS_NEWREF(ks_F_repr)},
        {"exit",           KS_NEWREF(ks_F_exit)},
        {"print",          KS_NEWREF(ks_F_print)},
        {"len",            KS_NEWREF(ks_F_len)},
        {"time",          KS_NEWREF(ks_F_time)},
        {"sleep",          KS_NEWREF(ks_F_sleep)},
        {"typeof",         KS_NEWREF(ks_F_typeof)},
        {"__import__",     KS_NEWREF(ks_F_import)},
        {"iter",           KS_NEWREF(ks_F_iter)},
        {"next",           KS_NEWREF(ks_F_next)},
        {"open",           KS_NEWREF(ks_F_open)},
        {"sort",           KS_NEWREF(ks_F_sort)},
        {"filter",         KS_NEWREF(ks_F_filter)},
        {"any",            KS_NEWREF(ks_F_any)},
        {"all",            KS_NEWREF(ks_F_all)},
        {"sum",            KS_NEWREF(ks_F_sum)},
        {"map",            KS_NEWREF(ks_F_map)},
        {"range",          KS_NEWREF(ks_F_range)},

        /* operators */

        /* math constants */

        {"PI",             (ks_obj)ks_float_new(KS_M_PI)},
        {"E",              (ks_obj)ks_float_new(KS_M_E)},
        {"PHI",            (ks_obj)ks_float_new(KS_M_PHI)},

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

