/* ks.c - commandline interface to run 
 *
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks.h"

// for getopt
#include <unistd.h>
#include <getopt.h>

int main(int argc, char** argv) {

    // handle errors ourselves
    opterr = 0;

    // current option
    int opt = 0;

    // parse arguments 
    while ((opt = getopt(argc, argv, "vVh")) != -1) {
        if (opt == 'h') {
            // print help
            printf("Usage: %s [-h]\n\n", argv[0]);
            printf("  -h               Prints this help/usage message\n");
            printf("  -T               Run some sanity checks\n");
            const ks_version_t* ver = ks_version();
            printf("\nkscript v%i.%i.%i %s %s\n", ver->major, ver->minor, ver->patch, ver->date, ver->time);
            printf("Cade Brown <brown.cade@gmail.com>\n");
            return 0;
        } else if (opt == 'V') {
            // print version
            const ks_version_t* ver = ks_version();
            printf("kscript v%i.%i.%i %s %s\n", ver->major, ver->minor, ver->patch, ver->date, ver->time);
            return 0;
        } else if (opt == 'v') {
            // increate verbosity
            ks_log_level_set(ks_log_level() - 1);
        } else if (opt == '?') {
            fprintf(stderr, "%s: Unknown option '-%c', run with '-h' to see help message\n", argv[0], optopt);
            return -1;
        } else if (opt == ':') {
            fprintf(stderr, "%s: Option '-%c' needs a value, run with '-h' to see help message\n", argv[0], optopt);
            return -2;
        }
    }

    // these are all extraneous arguments, ignore for now
    // In the future, allow a world file
    while (optind < argc) {
        fprintf(stderr, "arg: '%s'\n", argv[optind]);
        optind++;
    }

    // now, try & initialize the library
    if (!ks_init()) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }


    // exception
    ks_obj exc = NULL;

    ks_parser p = ks_parser_new(ks_str_new("print(12)"));
    if (exc = ks_catch()) {
        ks_error("%T: %R", exc, exc);
        return -1;
    }

    ks_ast prog = ks_parser_parse_file(p);
    if (exc = ks_catch()) {
        ks_error("%T: %R", exc, exc);
        return -1;
    }


    ks_info("parser: %S, src: %R, toks: %i", p, p->src, p->tok_n);

    ks_info("prog: %S", prog);

/*
    // now, actually execute code
    ks_code code = ks_code_new(NULL);

    ksca_load_c(code, "print");
    ksca_load_c(code, "print");
    ksca_call(code, 2);
    ksca_ret(code);

    ks_info("CODE: %S", code);

    vm_exec(ks_vm_default, code);

    */

    return 0;
}
