/* ks.c - commandline interface to run 
 *
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"


// for getopt
#include <unistd.h>
#include <getopt.h>

#include <errno.h>



void my_getopt() {

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
    
    ks_debug("prog_args: %S", prog_args);

    ks_thread main_thread = ks_thread_cur();
    assert(main_thread != NULL);

    ks_dict_set_cn(ks_globals, (ks_dict_ent_c[]){
        {"__argv__", KS_NEWREF(prog_args)},

        {NULL, NULL}
    });



    // exception handling
    ks_obj exc = NULL;


    FILE* fp = fopen(fname, "r");
    if (!fp) {
        ks_error("Failed to open file '%s': %s", fname, strerror(errno));
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* csrc = ks_malloc(len + 1);
    fread(csrc, 1, len, fp);
    csrc[len] = '\0';

    ks_str src = ks_str_new(csrc);
    ks_free(csrc);
    ks_str src_name = ks_str_new((char*)fname);

    ks_parser p = ks_parser_new(src, src_name);
    KS_DECREF(src_name);
    if (exc = ks_catch()) {
        ks_error("%T: %R", exc, exc);
        return -1;
    }

    ks_ast prog = ks_parser_parse_file(p);
    if (exc = ks_catch()) {
        ks_error("%T: %R", exc, exc);
        return -1;
    }

    ks_code myc = ks_codegen(prog);
    if (exc = ks_catch()) {
        ks_error("%T: %R", exc, exc);
        return -1;
    }

    ks_debug("CODE: %S", myc);

    // execute it
    //ks_obj ret = vm_exec(ks_vm_default, myc);
    ks_obj ret = ks_thread_call_code(main_thread, myc);
    if (exc = ks_catch()) {
        ks_error("%T: %R", exc, exc);
        return -1;
    }

    /*} else if (optind >= argc) {
        // no arguments left
        ks_error("%s: Too many files given to run!", argv[0]);
        return -1;
    } else {
        // no arguments left
        ks_error("%s: No files or expressions given to run!", argv[0]);
        return -1;
    }*/

    ks_debug("mem_max: %l", (int64_t)ks_mem_max());


    // end the main thread
    KS_DECREF(ks_thread_cur());

    return 0;
}
