/* kscript.c - the main compiler


*/

#include "kscript.h"

/* for getopt */
#include <getopt.h>
#include <unistd.h>


int main(int argc, char** argv) {

    ks_init();

    ks_set_loglvl(KS_LOGLVL_DEBUG);
    //ks_set_loglvl(KS_LOGLVL_TRACE);

    // the global VM
    ks_vm vm = KS_VM_EMPTY;

    // tasks to run
    int n_tasks = 0;
    struct ks_task {
        ks_parse kp;
        ks_prog prog;
    }* tasks = NULL;

    struct ks_task* this_task;

    // set a global variable
    #define SET_GLOBAL(_name, _val) { ks_dict_set_str(&vm.dict, KS_STR_CONST(_name), ks_hash_str(KS_STR_CONST(_name)), (kso)_val); }

    SET_GLOBAL("print", kso_F_print);
    SET_GLOBAL("exit" , kso_F_exit);

    // check errors
    if (ks_err_dumpall()) return -1;

    
    // long options for commandline parsing
    static struct option long_options[] = {
        {"expr", required_argument, NULL, 'e'},
        {"file", required_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'},

        {NULL, 0, NULL, 0}
    };

    // extension of file
    const char* ext = NULL;
    int stat = 0;

    int c;

    while ((c = getopt_long (argc, argv, "e:f:Aivh", long_options, NULL)) != -1)
    switch (c){
        case 'e':
            // do expression
            tasks = ks_realloc(tasks, sizeof(*tasks) * ++n_tasks);
            tasks[n_tasks - 1] = (struct ks_task){
                .kp = KS_PARSE_EMPTY,
                .prog = KS_PROG_EMPTY
            };
            this_task = &tasks[n_tasks - 1];
            ks_parse_setsrc(&this_task->kp, KS_STR_CONST("-e"), KS_STR_VIEW(optarg, strlen(optarg)));
            if (ks_err_dumpall()) return -1;

            ks_ast ast = ks_parse_code(&this_task->kp);
            if (ks_err_dumpall()) return -1;
            
            ks_ast_codegen(ast, &this_task->prog);
            ksb_retnone(&this_task->prog);
            if (ks_err_dumpall()) return -1;

            // now, run it
            ks_debug("Running `-e`: '%s' (compiled to %db)", this_task->kp.src._, this_task->prog.bc_n);
            ks_str ns = KS_STR_EMPTY;
            ks_prog_tostr(&this_task->prog, &ns);
            printf("%s\n", ns._);
            ks_str_free(&ns);
            
            ks_exec(&vm, &this_task->prog, 0);
            if (ks_err_dumpall()) return -1;

            break;
        case 'f':
            // do a file
            tasks = ks_realloc(tasks, sizeof(*tasks) * ++n_tasks);
            tasks[n_tasks - 1] = (struct ks_task){
                .kp = KS_PARSE_EMPTY,
                .prog = KS_PROG_EMPTY
            };
            this_task = &tasks[n_tasks - 1];

            ks_str src_code = KS_STR_EMPTY;
            FILE* fp = fopen(optarg, "r");
            if (fp == NULL) {
                ks_error("Could not open '%s'", optarg);
                return -1;
            }
            ks_str_readfp(&src_code, fp);
            fclose(fp);

            ks_parse_setsrc(&this_task->kp, KS_STR_VIEW(optarg, strlen(optarg)), src_code);
            if (ks_err_dumpall()) return -1;
            
            // free our local copy, since it has been duplicated
            ks_str_free(&src_code);

            ext = strrchr(optarg, '.');
            if (strcmp(ext, ".ksasm") == 0) {

                // parse assembly
                ks_parse_bc(&this_task->kp, &this_task->prog);
                ksb_retnone(&this_task->prog);
                if (ks_err_dumpall()) return -1;

            } else if (strcmp(ext, ".kscript") == 0) {
                ks_ast ast = ks_parse_code(&this_task->kp);
                if (ks_err_dumpall()) return -1;

                stat = ks_ast_codegen(ast, &this_task->prog);
                ksb_retnone(&this_task->prog);
                if (ks_err_dumpall()) return -1;

            } else {
                ks_error("Unknown file type: '%s'", optarg);
                return -1;
            }
            
            /* print out assembly
            ks_str ns = KS_STR_EMPTY;
            ks_prog_tostr(&this_task->prog, &ns);
            printf("%s\n", ns._);
            ks_str_free(&ns);
            */

            // now, run
            ks_debug("Running `-f`: '%s' (compiled to %db)", this_task->kp.src_name._, this_task->prog.bc_n);
            ks_exec(&vm, &this_task->prog, 0);

            break;
        case 'h':
            printf("Usage: %s [-e expr | -f file] [--h]\n", argv[0]);
            printf("  -h,--help              Prints this help message\n");
            printf("  -e,--expr [EXPR]       Compiles [EXPR], then executes it\n");
            printf("  -f,--file [FILE]       Reads [FILE], compiles it, then executes it\n");
            return 0;
            break;
        case '?':
            if (strchr("e", optopt) != NULL) {
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

    return 0;
}

