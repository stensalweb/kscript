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

    // construct a virtual machine to run code on
    kso_vm vm = kso_vm_new_empty();
    // hold a reference to it
    KSO_INCREF(vm);

    // macro to set a global value in the virtual machine
    #define SET_GLOBAL(_name, _val) ks_dict_set(&vm->globals, (kso)kso_str_new(KS_STR_CONST(_name)), ks_hash_str(KS_STR_CONST(_name)), (kso)_val);

    // set a few globals, the built-ins
    SET_GLOBAL("print", kso_F_print)
    SET_GLOBAL("repr", kso_F_repr)
    SET_GLOBAL("memuse", kso_F_memuse)
    SET_GLOBAL("type", kso_F_type)

    // operators
    SET_GLOBAL("add", kso_F_add)

    // types
    SET_GLOBAL("int", kso_T_int)
    SET_GLOBAL("str", kso_T_str)
    SET_GLOBAL("dict", kso_T_dict)

    // ensure there were no errors
    if (ks_err_dumpall()) return -1;

    // construct a list to work as a constant pool
    kso_list v_const = kso_list_new_empty();

    // create some 'tasks' that contain their resources related to execution
    int n_tasks = 0;
    struct ks_task {
        ks_parse kp;
        kso_code code;
    }* tasks = NULL;

    // the currently executing task
    #define this_task (tasks[n_tasks - 1])
    
    // long options for commandline parsing
    static struct option long_options[] = {
        {"expr", required_argument, NULL, 'e'},
        {"file", required_argument, NULL, 'f'},
        {"help", no_argument, NULL, 'h'},

        {NULL, 0, NULL, 0}
    };

    // extension of file
    const char* ext = NULL;
    // option argument
    int c;

    // before argument parsing, ensure its all worked so far
    if (ks_err_dumpall()) return -1;

    while ((c = getopt_long (argc, argv, "e:f:Aivh", long_options, NULL)) != -1)
    switch (c){
        case 'e':
            // do expression

            // add on another task
            tasks = ks_realloc(tasks, sizeof(*tasks) * ++n_tasks);
            tasks[n_tasks - 1] = (struct ks_task){
                .kp = KS_PARSE_EMPTY,
                .code = kso_code_new_empty(v_const)
            };

            // set source, and tokenize the expression
            ks_parse_setsrc(&this_task.kp, KS_STR_CONST("-e"), KS_STR_VIEW(optarg, strlen(optarg)));
            if (ks_err_dumpall()) return -1;

            // parse the code/expression
            ks_ast ast = ks_parse_code(&this_task.kp);
            if (ks_err_dumpall()) return -1;
            
            // do a code generation on the resulting AST
            ks_ast_codegen(ast, this_task.code);
            if (ks_err_dumpall()) return -1;

            // now, run it
            ks_debug("Running `-e`: '%s' (compiled to %db)", this_task.kp.src._, this_task.code->bc_n);

            // trace out the assembly source code
            if (ks_get_loglvl() <= KS_LOGLVL_DEBUG) {
                ks_str s_asm = KS_STR_EMPTY;
                kso_code_tostr(this_task.code, &s_asm);
                ks_debug("# -*- ASM -*-\n%s", s_asm._);
                ks_str_free(&s_asm);
            }

            // now, execute on a VM
            kso_vm_exec(vm, this_task.code);
            if (ks_err_dumpall()) return -1;

            break;
        case 'f':
            // do a file

            // add on a task
            tasks = ks_realloc(tasks, sizeof(*tasks) * ++n_tasks);
            tasks[n_tasks - 1] = (struct ks_task){
                .kp = KS_PARSE_EMPTY,
                .code = kso_code_new_empty(v_const)
            };

            // read in the source code from a file
            ks_str src_code = KS_STR_EMPTY;
            FILE* fp = fopen(optarg, "r");
            if (fp == NULL) {
                ks_error("Could not open '%s'", optarg);
                return -1;
            }
            ks_str_readfp(&src_code, fp);
            fclose(fp);

            // set source and tokenize
            ks_parse_setsrc(&this_task.kp, KS_STR_VIEW(optarg, strlen(optarg)), src_code);
            if (ks_err_dumpall()) return -1;
            
            // free our local copy, since it has been duplicated
            ks_str_free(&src_code);

            // get the file extension
            ext = strrchr(optarg, '.');
            if (strcmp(ext, ".ksasm") == 0) {
                // in this case, we want to parse an assembly code file

                ks_parse_ksasm(&this_task.kp, this_task.code);
                if (ks_err_dumpall()) return -1;

            } else if (strcmp(ext, ".kscript") == 0) {
                // in this case, we want to parse the code to an AST, then generate bytecode for it
                ks_ast ast = ks_parse_code(&this_task.kp);
                if (ks_err_dumpall()) return -1;

                // generate the code for it
                ks_ast_codegen(ast, this_task.code);
                if (ks_err_dumpall()) return -1;

            } else {
                ks_error("Unknown file type: '%s'", optarg);
                return -1;
            }

            // debug it out
            ks_debug("Running `-f`: '%s' (compiled to %db)", this_task.kp.src_name._, this_task.code->bc_n);

            // now, trace out the assembly
            if (ks_get_loglvl() <= KS_LOGLVL_DEBUG) {
                ks_str s_asm = KS_STR_EMPTY;
                kso_code_tostr(this_task.code, &s_asm);
                ks_debug("# -*- ASM -*-\n%s", s_asm._);
                ks_str_free(&s_asm);
            }

            // execute on the virtual machine
            kso_vm_exec(vm, this_task.code);

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

