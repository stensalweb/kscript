// kscript.c - the main commandline interface to the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


#include <unistd.h>
#include <getopt.h>


int main(int argc, char** argv) {

    // create our global context
    ks_ctx ctx = ks_ctx_new();

    ks_obj std = ks_module_load(ctx, KS_STR_CONST("std"));

    // instantiate the global values
    ks_scope globals = ks_scope_new(NULL);
    // add standard module
    ks_dict_set(&globals->locals, KS_STR_CONST("std"), std);

    // start out with globals
    ks_ctx_push(ctx, KS_STR_CONST("global"), globals);

    // long options for commandline parsing
    static struct option long_options[] = {
        {"as-file", required_argument, NULL, 'a'},
        {"help", no_argument, NULL, 'h'},

        {NULL, 0, NULL, 0}
    };

    int c;

    while ((c = getopt_long (argc, argv, "a:h", long_options, NULL)) != -1)
    switch (c){
        case 'a': ;

            FILE* fp = fopen(optarg, "r");
            if (fp == NULL) {
                ks_error("Couldn't open file '%s'", optarg);
                return 1;
            }
            ks_str src = KS_STR_EMPTY;
            ks_str_readfp(&src, fp);
            fclose(fp);

            printf("%s\n", src._);

            break;
        case 'h':
            printf("Usage: %s [-a FILE.ksasm] [-h]\n", argv[0]);
            printf("  -h,--help              Prints this help message\n");
            printf("  -a,--as-file FILE      Read and compile a kscript assembly file\n");
            return 0;
            break;
        case '?':
            if (strchr("", optopt) != NULL) {
                ks_error("Option -%c requires an argument.", optopt);
            } else {
                ks_error("Unknown option `-%c'.", optopt);
            }
            return 1;
            break;
        default:
            return 1;
            break;
    }

    if (optind < argc) {
        ks_error("Unhandled arguments!");
    }

    ks_set_loglvl(KS_LOGLVL_TRACE);

    ks_bc fm = ks_bc_new();

    ks_bc_const_str(fm, KS_STR_CONST("hello world"));
    //ks_bc_const_int(fm, 42);
    ks_bc_load(fm, KS_STR_CONST("std"));
    ks_bc_attr(fm, KS_STR_CONST("print"));
    ks_bc_call(fm, 1);
    ks_bc_ret(fm);

    ks_exec(ctx, fm, 0);

    //printf("took %d imem\n", fm->inst_n);

    return 0;
}


