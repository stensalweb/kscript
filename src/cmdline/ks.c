/* ks.c - commandline interface to run 
 *
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"

#include "ks_getopt.h"

// for error printing
#include <errno.h>

// Cfunc wrapper to call
static KS_FUNC(cfunc_main) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_str fname = (ks_str)args[0];
    KS_REQ_TYPE(fname, ks_type_str, "fname");

    ks_str src_code = NULL;

    // 1. read the source code from the file
    if (n_args == 2) {
        // use a source code given
        src_code = (ks_str)args[1];
        KS_INCREF(src_code);
    } else {

        // just read an entire file
        src_code = ks_readfile(fname->chr);
        if (!src_code) return NULL;
    }

    // 2. construct a parser
    ks_parser p = ks_parser_new(src_code, fname);
    if (!p) return NULL;

    // 3. parse out the entire module (which will also syntax validate)
    ks_ast prog = ks_parser_parse_file(p);
    if (!prog) {
        KS_DECREF(p);    
        return NULL;
    }

    // 4. generate a bytecode object reprsenting the module
    ks_code myc = ks_codegen(prog);
    if (!myc) {
        KS_DECREF(p);    
        KS_DECREF(prog);    
        return NULL;
    }

    // 5. (optional) attempt to set some metadata for the code, if asked
    if (myc->meta_n > 0 && myc->meta[0].tok.parser != NULL) {
        if (myc->name_hr) KS_DECREF(myc->name_hr);
        myc->name_hr = ks_fmt_c("%S", myc->meta[0].tok.parser->src_name);
    }


    // debug the code it is going to run
    ks_debug("CODE: %S", myc);

    // now, call the code object with no arguments, and return the result
    // If there is an error, it will return NULL, and the thread will call ks_errend(),
    //   which will print out a stack trace and terminate the program for us
    return ks_call((ks_obj)myc, 0, NULL);
}


int main(int argc, char** argv) {

    // now, try & initialize the library
    if (!ks_init()) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }

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
        fprintf(stderr, BOLD RED "ERROR" RESET ": " __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return -1; \
    }

    // the options
    static struct ks_option long_options[] = {
        { "help",    no_argument,       0, 'h' },
        { "verbose", no_argument,       0, 'v' },
        { "expr",    required_argument, 0, 'e' },
        { NULL, 0, NULL, 0}
    };


    ks_optind = 0;
    
    while ((opt = ks_getopt_long(argc, argv, "+e:hvV", long_options, NULL)) != -1) {
        if (opt == 'h') {
            // print help
            printf("Usage: %s [options] FILE [args...]\n", argv[0]);
            printf("       %s [options] -e 'EXPR' [args...]\n", argv[0]);
            printf("\nOptions:\n");
            printf("  -h, --help            Prints this help/usage message\n");
            printf("  -e [EXPR]             Run an inline expression, instead of a file\n");
            printf("  -v[vv]                Increase verbosity (use '-vvv' for 'TRACE' level)\n");
            printf("  -V, --version         Print out just the version information for kscript\n");
            printf("\nkscript v%i.%i.%i %s %s %s\n", ver->major, ver->minor, ver->patch, ver->build_type, ver->date, ver->time);
            printf("Cade Brown <brown.cade@gmail.com>\n");
            return 0;
        } else if (opt == 'V') {
            // print version
            printf("kscript v%i.%i.%i %s %s %s\n", ver->major, ver->minor, ver->patch, ver->build_type, ver->date, ver->time);
            return 0;
        } else if (opt == 'v') {
            // increate verbosity
            ks_log_level_set(ks_log_level() - 1);

        } else if (opt == 'e') {
            expr = ks_optarg;
            break;

        } else if (opt == '?') {
            //OPT_ERR("%s: Unknown option '-%c', run with '-h' to see help message", argv[0], optopt);
            OPT_ERR("%s: Unknown option '%s', run '%s -h' to see help message", argv[0], argv[ks_optind-1], argv[0]);
        } else if (opt == ':') {
            OPT_ERR("%s: Option '-%c' needs a value, run '%s -h' to see help message", argv[0], ks_optopt, argv[0]);
        } else {
            OPT_ERR("%s: Unknown options passed ('%s')!, run '%s -h' to see help message", argv[0], argv[ks_optind-1], argv[0]);
        }
    }

    // check if given an expression
    if (ks_optind < argc && !expr) {
        // take the file name as the first positional argument
        fname = argv[ks_optind++];
    }

    if (fname == NULL && expr == NULL) {
        OPT_ERR("%s: Not given a file or expression (with '-e'), run '%s -h' for help", argv[0], argv[0]);
    } else if (fname != NULL && expr != NULL) {
        OPT_ERR("%s: Given a file AND an expression (with '-e'), but only expected one", argv[0]);
    }

    // the number of arguments passed to the program
    int prog_argc = argc - ks_optind;
    char** prog_argv = argv + ks_optind;


    ks_str arg0 = NULL;
    if (fname != NULL) {
        arg0 = ks_str_new(fname);
    } else {
        arg0 = ks_fmt_c("-e %s", expr);
    }
    // start with program
    ks_list prog_args = ks_list_new(1, (ks_obj*)&arg0);
    KS_DECREF(arg0);

    int i;
    for (i = 0; i < prog_argc; ++i) {
        ks_str argi = ks_str_new(prog_argv[i]);
        ks_list_push(prog_args, (ks_obj)argi);
        KS_DECREF(argi);
    }

    if (ver->major != KS_VERSION_MAJOR || ver->minor != KS_VERSION_MINOR || ver->patch != KS_VERSION_PATCH) {
        fprintf(stderr, YELLOW "WARN " RESET ": ks_version() gave different result than header (%i.%i.%i v %i.%i.%i)\n", 
            ver->major, ver->minor, ver->patch, 
            KS_VERSION_MAJOR, KS_VERSION_MINOR, KS_VERSION_PATCH
        );
    }

    //ks_debug("prog_args: %S", prog_args);

    // update globals
    ks_dict_set_cn(ks_globals, (ks_dict_ent_c[]){
        {"__globals__", KS_NEWREF(ks_globals)},
        
        {"__path__", KS_NEWREF(ks_paths)},
        {"__argv__", KS_NEWREF(prog_args)},

        {NULL, NULL}
    });

    // construct a main function
    ks_cfunc my_main = ks_cfunc_new2(cfunc_main_, "__std.main(fname)");
    ks_thread main_thread = NULL;

    if (fname != NULL) {
        ks_str fname_o = ks_str_new(fname);
        main_thread = ks_thread_new("main", (ks_obj)my_main, 1, (ks_obj*)&fname_o);
        KS_DECREF(fname_o);
    } else if (expr != NULL) {
        ks_str fname_o = ks_str_new("'-e'");
        ks_str expr_o = ks_str_new(expr);
        main_thread = ks_thread_new("main", (ks_obj)my_main, 2, (ks_obj[]){(ks_obj)fname_o, (ks_obj)expr_o});
        KS_DECREF(fname_o);
        KS_DECREF(expr_o)
    } else {
        assert (false && "invalid state; 'fname' and 'expr' are NULL, but I already checked!");
    }

    // stop using our reference
    KS_DECREF(my_main);

    if (!main_thread) {
        OPT_ERR("Thread creation failed for thread 'main'");
    }


    // start executing the thread
    ks_thread_start(main_thread);

    // ensure the thread is done
    ks_thread_join(main_thread);
    //KS_DECREF(main_thread);

    //ks_debug("mem_max: %l", (int64_t)ks_mem_max());

    return 0;
}
