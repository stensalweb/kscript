/* ks.c - source code for the binary executable `ks`
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

#include "ks_getopt.h"

int main(int argc, char** argv) {
    // handle errors ourselves
    ks_opterr = 0;

    // current option
    int opt = 0;
    const ks_version_t* ver = ks_version();

    // the file to run
    char* fname = NULL;

    // the expression to run
    char* expr = NULL;

    // option argument error
    #define OPT_ERR(...) { \
        fprintf(stderr, COL_BOLD COL_RED "ERROR" COL_RESET ": " __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return -1; \
    }

    // the options
    static struct ks_option long_options[] = {
        { "help",    no_argument,       0, 'h' },
        { "verbose", no_argument,       0, 'v' },
        { "version", no_argument,       0, 'V' },
        { "expr",    required_argument, 0, 'e' },
        { NULL, 0, NULL, 0}
    };

    // verbositity
    int vdiff = 0;
    ks_optind = 0;
    
    while ((opt = ks_getopt_long(argc, argv, "+e:hvV", long_options, NULL)) != -1) {
        if (opt == 'h') {
            // print help
            printf("Usage: %s [options] FILE [args...]\n", argv[0]);
            printf("       %s [options] -e 'EXPR' [args...]\n", argv[0]);
            printf("       %s [options] - [args...]\n", argv[0]);
            printf("\nOptions:\n");
            printf("  -h,--help             Prints this help/usage message\n");
            printf("  -e,--expr [EXPR]      Run an inline expression, instead of a file\n");
            printf("  -                     Start an interactive REPL shell\n");
            printf("  -v[vv]                Increase verbosity (use '-vvv' for 'TRACE' level)\n");
            printf("  -V,--version          Print out just the version information for kscript\n");
            printf("\nkscript v%i.%i.%i %s %s %s\n", ver->major, ver->minor, ver->patch, ver->build_type, ver->date, ver->time);
            printf("Cade Brown <brown.cade@gmail.com>\n");
            return 0;
        } else if (opt == 'V') {
            // print version
            printf("kscript v%i.%i.%i %s %s %s\n", ver->major, ver->minor, ver->patch, ver->build_type, ver->date, ver->time);
            return 0;
        } else if (opt == 'v') {
            // increate verbosity
            //ks_log_level_set(ks_log_level() - 1);
            vdiff ++;
        } else if (opt == 'e') {
            expr = ks_optarg;
            // stop parsing arguments
            break;

        } else if (opt == '?') {
            //OPT_ERR("%s: Unknown option '-%c', run with '-h' to see help message", argv[0], optopt);
            if (!ks_optopt || ks_optopt == '-') {
                OPT_ERR("%s: Unknown option '%s', run '%s -h' to see help message", argv[0], argv[ks_optind - 1], argv[0]);
            } else {
                OPT_ERR("%s: Unknown option '-%c', run '%s -h' to see help message", argv[0], ks_optopt, argv[0]);
            }
        } else if (opt == ':') {
            if (ks_optopt == '-') {
                OPT_ERR("%s: Option '%s' needs a value, run '%s -h' to see help message", argv[0], argv[ks_optind - 1], argv[0]);
            } else {
                OPT_ERR("%s: Option '-%c' needs a value, run '%s -h' to see help message", argv[0], ks_optopt, argv[0]);
            }
        } else {
            OPT_ERR("%s: Unknown options passed ('%s')!, run '%s -h' to see help message", argv[0], argv[ks_optind-1], argv[0]);
        }
    }

    // now, try & initialize the library
    if (!ks_init(KS_LOG_WARN - vdiff)) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }

    ks_debug("ks", "argc: %i, argv[0]: %s", argc, argv[0]);
    //printf("argv[0]: %s\n", argv[0]);
    // ensure a maximum of one is given 
    if (fname != NULL && expr != NULL) {
        OPT_ERR("%s: Given a file AND an expression (with '-e'), but only expected one", argv[0]);
    }

    // do a little version check to make sure it was compiled with the same dynamic library version
    if (ver->major != KS_VERSION_MAJOR || ver->minor != KS_VERSION_MINOR || ver->patch != KS_VERSION_PATCH) {
        fprintf(stderr, COL_YLW "WARN " COL_RESET ": ks_version() gave different result than header (%i.%i.%i v %i.%i.%i)\n", 
            ver->major, ver->minor, ver->patch, 
            KS_VERSION_MAJOR, KS_VERSION_MINOR, KS_VERSION_PATCH
        );
    }


    // check if given an expression
    if (ks_optind < argc && !expr) {
        // take the file name as the first positional argument
        fname = argv[ks_optind++];
    }

    // default to nothing (i.e. open session)
    if (fname && *fname == '-') fname = NULL;

    /*if (fname == NULL && expr == NULL) {
        OPT_ERR("%s: Not given a file or expression (with '-e'), run '%s -h' for help", argv[0], argv[0]);
    }*/


    // move to the arguments given to the program
    ks_list prog_args = ks_list_new(0, NULL);

    // temporary string
    ks_str tmps = NULL;

    // construct a new thread
    ks_thread main_thread = NULL;

    // update globals
    ks_dict_set_c(ks_globals, KS_KEYVALS(
        ///{"__globals__", KS_NEWREF(ks_globals)},
        
        //{"__path__", KS_NEWREF(ks_paths)},
        {"__argv__", KS_NEWREF(prog_args)},
    ));


    if (fname != NULL) {
        // executing a file
        tmps = ks_str_new(fname);
        ks_list_push(prog_args, (ks_obj)tmps);
        //main_thread = ks_thread_new("main", (ks_obj)ks_F_run_file, 1, (ks_obj*)&tmps);
        KS_DECREF(tmps);

    } else if (expr != NULL) {
        // executing an expression
        tmps = ks_fmt_c("-e %s", expr);
        ks_list_push(prog_args, (ks_obj)tmps);
        KS_DECREF(tmps);

        tmps = ks_str_new(expr);
        //main_thread = ks_thread_new("main", (ks_obj)ks_F_run_expr, 1, (ks_obj*)&tmps);
        KS_DECREF(tmps);
    } else {
        // live session
        tmps = ks_str_new("-");
        ks_list_push(prog_args, (ks_obj)tmps);
        KS_DECREF(tmps);

        //main_thread = ks_thread_new("main", (ks_obj)ks_F_run_interactive, 0, NULL);
    }

   /*
    if (main_thread == NULL) {
        OPT_ERR("Nothing given to do!");
        return -1;
    }
*/

    // the number of arguments passed to the program
    int prog_argc = argc - ks_optind;
    char** prog_argv = argv + ks_optind;


    int i;
    for (i = 0; i < prog_argc; ++i) {
        tmps = ks_str_new(prog_argv[i]);
        ks_list_push(prog_args, (ks_obj)tmps);
        KS_DECREF(tmps);
    }

    // execute the code
    //ks_thread_start(main_thread);
    //ks_thread_join(main_thread);

    ks_obj ret = NULL;

    if (fname != NULL) {
        // run filename
        ret = ks_obj_call((ks_obj)ks_F_exec_file, 1, (ks_obj[]){ (ks_obj)prog_args->elems[0] });

    } else if (expr != NULL) {
        // run single expression
        ks_str expr_str = ks_str_new(expr);
        ret = ks_obj_call((ks_obj)ks_F_exec_expr, 1, (ks_obj[]){ (ks_obj)expr_str });
        KS_DECREF(expr_str);

    } else {
        // live session
        ret = ks_obj_call((ks_obj)ks_F_exec_interactive, 0, NULL);
    }

    if (!ret) ks_exit_if_err();
    KS_DECREF(ret);

    // return success
    return 0;
}


