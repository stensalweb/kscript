/* ks.c - commandline interface to run 
 *
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"


// for getopt
#include <unistd.h>
#include <getopt.h>

#include <errno.h>

// Cfunc wrapper to call
static KS_FUNC(cfunc_main) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    ks_str fname = (ks_str)args[0];
    KS_REQ_TYPE(fname, ks_type_str, "fname");


    // 1. read the source code from the file
    // allowing for any errors
    ks_str src_code = ks_readfile(fname->chr);
    if (!src_code) return NULL;

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
    opterr = 0;

    // current option
    int opt = 0;
    const ks_version_t* ver = ks_version();

    // the file to run
    char* fname = NULL;

    // option argument error
    #define OPT_ERR(...) { \
        fprintf(stderr, BOLD RED "ERROR" RESET ": " __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        return -1; \
    }

    // parse arguments 
    while ((opt = getopt(argc, argv, "+vVh")) != -1) {
        if (opt == 'h') {
            // print help
            printf("Usage: %s [options] file <prog_args>\n\n", argv[0]);
            printf("Options:\n");
            printf("  -h               Prints this help/usage message\n");
            printf("  -v[vv]           Increase verbosity (use '-vvv' for 'TRACE' level)\n");
            printf("\nkscript v%i.%i.%i %s %s\n", ver->major, ver->minor, ver->patch, ver->date, ver->time);
            printf("Cade Brown <brown.cade@gmail.com>\n");
            return 0;
        } else if (opt == 'V') {
            // print version
            printf("kscript v%i.%i.%i %s %s\n", ver->major, ver->minor, ver->patch, ver->date, ver->time);
            return 0;
        } else if (opt == 'v') {
            // increate verbosity
            ks_log_level_set(ks_log_level() - 1);
        } else if (opt == '?') {
            OPT_ERR("%s: Unknown option '-%c', run with '-h' to see help message", argv[0], optopt);
        } else if (opt == ':') {
            OPT_ERR("%s: Option '-%c' needs a value, run with '-h' to see help message", argv[0], optopt);
            return -2;
        }
    }

    if (optind < argc) {
        // there was 1 files given
        fname = argv[optind];
    } else {
        OPT_ERR("Expected file, but got no arguments!");
    }

    // the number of arguments passed to the program
    int prog_argc = argc - optind;
    char** prog_argv = argv + optind;

    ks_list prog_args = ks_list_new(0, NULL);
    int i;
    for (i = 0; i < prog_argc; ++i) {
        ks_str argi = ks_str_new(prog_argv[i]);
        ks_list_push(prog_args, (ks_obj)argi);
        KS_DECREF(argi);
    }
    
    //ks_debug("prog_args: %S", prog_args);

    // update globals
    ks_dict_set_cn(ks_globals, (ks_dict_ent_c[]){
        {"__argv__", KS_NEWREF(prog_args)},

        {NULL, NULL}
    });

    // construct a main function
    ks_cfunc my_main = ks_cfunc_new2(cfunc_main_, "__std.main(fname)");

    ks_str fname_o = ks_str_new(fname);
    ks_thread main_thread = ks_thread_new("main", (ks_obj)my_main, 1, (ks_obj*)&fname_o);
    KS_DECREF(fname_o);
    KS_DECREF(my_main);

    // start executing the thread
    ks_thread_start(main_thread);

    // ensure the thread is done
    ks_thread_join(main_thread);
    //KS_DECREF(main_thread);

    //ks_debug("mem_max: %l", (int64_t)ks_mem_max());

    return 0;
}
