/* init.c - handles initialization of the kscript library
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// include the `whereami` function to tell where the binary is located
#include "whereami.h"


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

// list of paths to search through when importing
ks_list ks_paths = NULL;

// global variables
ks_dict ks_globals = NULL;



// Lock the GIL for operation
void ks_GIL_lock() {

}

// Unock the GIL for operation
void ks_GIL_unlock() {

}

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
    ks_init_T_range();
    ks_init_T_dict();
    ks_init_T_namespace();
    ks_init_T_cfunc();
    ks_init_T_pfunc();
    ks_init_T_thread();
    ks_init_T_parser();
    ks_init_T_ast();
    ks_init_T_code();
    ks_init_T_stack_frame();
    ks_init_T_kfunc();
    ks_init_T_module();

    // initialize others
    ks_init_funcs();


    // initialize paths
    ks_paths = ks_list_new(0, NULL);

    // Literal paths; just search through these as written (even though they may be
    //   relative in nature, they are just string literals)
    const char* path_lits[] = {
        ".",
        NULL,
    };

    // iterator through paths
    const char** this_path = &path_lits[0];

    // add literals
    while (*this_path) {
        ks_str tmp_path = ks_str_new(*this_path);
        ks_list_push(ks_paths, (ks_obj)tmp_path);
        KS_DECREF(tmp_path);

        this_path++;
    }


    // Paths from `$KS_PATH`

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


    // find out the full path of 'argv[0]'
    int length = wai_getExecutablePath(NULL, 0, NULL);
    char* full_path = ks_malloc(length + 1);
    int dir_length = 0;
    wai_getExecutablePath(full_path, length, &dir_length);
    full_path[dir_length] = '\0';


    ks_debug("ks", "wai_getExecutablePath: %s", full_path);

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
        {"range",                  KS_NEWREF(ks_T_range)},
        {"dict",                   KS_NEWREF(ks_T_dict)},
        {"namespace",              KS_NEWREF(ks_T_namespace)},

        {"type",                   KS_NEWREF(ks_T_type)},
        {"obj",                    KS_NEWREF(ks_T_obj)},
        {"thread",                 KS_NEWREF(ks_T_thread)},

        {"Enum",                   KS_NEWREF(ks_T_Enum)},


        /* Errors */

        {"Error",                  KS_NEWREF(ks_T_Error)},
        {"MathError",              KS_NEWREF(ks_T_MathError)},


        /* Functions */

        {"__import__",             KS_NEWREF(ks_F_import)},
        {"__recurse__",            KS_NEWREF(ks_F_recurse)},
        {"eval",                   KS_NEWREF(ks_F_eval)},
        
        {"print",                  KS_NEWREF(ks_F_print)},

        {"typeof",                 KS_NEWREF(ks_F_typeof)},

        {"truthy",                 KS_NEWREF(ks_F_truthy)},
        {"repr",                   KS_NEWREF(ks_F_repr)},
        {"len",                    KS_NEWREF(ks_F_len)},
        {"abs",                    KS_NEWREF(ks_F_abs)},

        {"chr",                    KS_NEWREF(ks_F_chr)},
        {"ord",                    KS_NEWREF(ks_F_ord)},

        {"any",                    KS_NEWREF(ks_F_any)},
        {"all",                    KS_NEWREF(ks_F_all)},

        {"sum",                    KS_NEWREF(ks_F_sum)},
        {"map",                    KS_NEWREF(ks_F_map)},
        {"filter",                 KS_NEWREF(ks_F_filter)},


        /* Misc. Variables */

        {"__path__",               KS_NEWREF(ks_paths)},

    ));


    // success
    return hasInit = true;
}

