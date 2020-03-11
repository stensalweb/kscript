/* ks.c - commandline interface to run 
 *
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks.h"

// for getopt
#include <unistd.h>
#include <getopt.h>
#include <errno.h>


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


    // now, try & initialize the library
    if (!ks_init()) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }


    // exception
    ks_obj exc = NULL;

    if (optind == argc - 1) {

        // there was 1 files given

        const char* fname = argv[optind];


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
        ks_obj ret = vm_exec(ks_vm_default, myc);
        if (exc = ks_catch()) {
            ks_error("%T: %R", exc, exc);
            return -1;
        }

    } else if (optind >= argc) {
        // no arguments left
        ks_error("%s: Too many files given to run!", argv[0]);
        return -1;
    } else {
        // no arguments left
        ks_error("%s: No files or expressions given to run!", argv[0]);
        return -1;
    }

    ks_debug("mem_max: %l", (int64_t)ks_mem_max());


    return 0;
}
