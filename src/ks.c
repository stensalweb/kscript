/* ks.c - the main commandline interface to kscript */

#include "ks.h"

/* for getopt */
#include <getopt.h>
#include <unistd.h>

int main(int argc, char** argv) {

    ks_init();
    ks_log_level_set(KS_LOG_INFO);

    // TODO: parse the options before executing everything, so this point will be debugged/traced if
    // given -v

    ks_int a = ks_int_new(2342);

    // get the global virtual machine
    ks_dict globals = ks_get_globals();

    #define SET_GLOBAL(_key, _val) { \
        ks_str skey = ks_str_new(_key); \
        ks_dict_set(globals, (kso)skey, skey->v_hash, (kso)(_val)); \
        KSO_DECREF(skey); \
    }

    /* utility built ins */
    SET_GLOBAL("type",   ks_F_type);
    SET_GLOBAL("repr",   ks_F_repr);

    SET_GLOBAL("getattr", ks_F_getattr);
    SET_GLOBAL("setattr", ks_F_setattr);

    SET_GLOBAL("import", ks_F_import);
    SET_GLOBAL("call",   ks_F_call);

    /* useful std functions */
    SET_GLOBAL("print",  ks_F_print);
    SET_GLOBAL("hash",   ks_F_hash);
    SET_GLOBAL("rand",   ks_F_rand);

    /* builtin types */
    SET_GLOBAL("bool",  ks_T_bool);
    SET_GLOBAL("int",   ks_T_int);
    SET_GLOBAL("float",   ks_T_float);
    SET_GLOBAL("str",   ks_T_str);
    SET_GLOBAL("tuple", ks_T_tuple);
    SET_GLOBAL("list",  ks_T_list);
    SET_GLOBAL("dict",  ks_T_dict);
    SET_GLOBAL("cfunc", ks_T_cfunc);
    SET_GLOBAL("kfunc", ks_T_kfunc);
    SET_GLOBAL("pfunc", ks_T_pfunc);
    SET_GLOBAL("kobj", ks_T_kobj);

    /* internal helpers */
    SET_GLOBAL("__new_type__", ks_F_new_type);

    /* operators */
    SET_GLOBAL("__add__", ks_F_add);
    SET_GLOBAL("__sub__", ks_F_sub);
    SET_GLOBAL("__mul__", ks_F_mul);
    SET_GLOBAL("__div__", ks_F_div);
    SET_GLOBAL("__mod__", ks_F_mod);
    SET_GLOBAL("__pow__", ks_F_pow);

    SET_GLOBAL("__lt__", ks_F_lt);
    SET_GLOBAL("__le__", ks_F_le);
    SET_GLOBAL("__gt__", ks_F_gt);
    SET_GLOBAL("__ge__", ks_F_ge);
    SET_GLOBAL("__eq__", ks_F_eq);
    SET_GLOBAL("__ne__", ks_F_ne);

    SET_GLOBAL("__neg__", ks_F_neg);
    SET_GLOBAL("__sqig__", ks_F_sqig);

    /* misc. builtin functions */
    SET_GLOBAL("shell", ks_F_shell);

    // check for errors so far
    if (kse_dumpall()) return -1;

    // long options for commandline parsing
    static struct option long_options[] = {
        { "expr", required_argument, NULL, 'e' },
        { "file", required_argument, NULL, 'f' },
        { "help", no_argument,       NULL, 'h' },

        { NULL,   0,                 NULL, 0   }
    };

    // a parser, usable to parse expressions or files
    ks_parser par = NULL;

    // the AST for the whole program
    ks_ast prog_ast = NULL;

    // the generated bytecode for the program
    ks_code prog_bc = NULL;

    // getopt character
    int c;

    int64_t MU = ks_memuse();

    // error check
    if (kse_dumpall()) return -1;

    // now, handle arguments
    while ((c = getopt_long (argc, argv, "e:f:vih", long_options, NULL)) != -1)
    switch (c){
        case 'e':
            // do an expression given via commandline

            // construct a parser
            par = ks_parser_new_expr(optarg);
            if (kse_dumpall()) return -1;

            // parse out the whole expression            
            prog_ast = ks_parse_program(par);
            if (kse_dumpall()) return -1;

            // optimize it
            prog_ast = ks_ast_fopt(prog_ast, ks_ast_opt_propconst, NULL);

            // generate the bytecode
            prog_bc = ks_ast_codegen(prog_ast, NULL);
            if (kse_dumpall()) return -1;

            ks_debug("Running `-e`: '%s' (compiled to %ib)", par->src->chr, prog_bc->bc_n);
            // TODO: maybe output the assembly here


            // now, execute on the VM
            ks_vm_exec(prog_bc);
            if (kse_dumpall()) return -1;

            // check refcnt
            KSO_DECREF(par);
            KSO_DECREF(prog_ast);
            KSO_DECREF(prog_bc);

            break;
        case 'f': ;
            // do a file  given via commandline

            // construct a parser
            par = ks_parser_new_file(optarg);
            if (kse_dumpall()) return -1;

            // parse out the whole expression            
            prog_ast = ks_parse_program(par);
            if (kse_dumpall()) return -1;

            // generate the bytecode
            prog_bc = ks_ast_codegen(prog_ast, NULL);
            if (kse_dumpall()) return -1;

            ks_debug("Running `-f`: '%s' (compiled to %ib)", par->src_name->chr, prog_bc->bc_n);
            // TODO: maybe output the assembly here

            // now, execute on the VM
            ks_vm_exec(prog_bc);
            if (kse_dumpall()) return -1;

            // check refcnt
            KSO_DECREF(par);
            KSO_DECREF(prog_ast);
            KSO_DECREF(prog_bc);

            break;
        case 'v':
            // increase verbosity, decrease log level
            ks_log_level_set(ks_log_level() - 1);
            break;
        case 'i':
            printf("kscript v0.0, built at " __DATE__ " " __TIME__ "\n");
            printf("Authors:\n");
            printf("  Cade Brown <brown.cade@gmail.com>\n");

            return 0;
            break;
        case 'h':
            printf("Usage: %s [-e expr | -f file] [--h]\n", argv[0]);
            printf("  -h,--help              Prints this help message, then exits\n");
            printf("  -e,--expr [EXPR]       Compiles [EXPR], then executes it\n");
            printf("  -f,--file [FILE]       Reads [FILE], compiles it, then executes it\n");
            printf("  -i,--info              Prints build information, then exits\n");
            printf("  -v                     Increases the verbosity by one level (use -v -v for more verbosity)\n");
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


    // clean up memory, etc
    if (kse_dumpall()) return -1;

    int64_t total_diff = (int64_t)ks_memuse() - MU;
    //if (total_diff != 0) ks_warn("possible leak of %i bytes detected", (int)total_diff);

    ks_debug("[MEM] maximum: %l", ks_memuse_max());

    return 0;
}

