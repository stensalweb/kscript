
#include "kscript.h"


int main(int argc, char** argv) {

    //ks_set_loglvl(KS_LOGLVL_TRACE);
    ks_set_loglvl(KS_LOGLVL_DEBUG);

    // create a new program
    /*
    ks_prog p = KS_PROG_EMPTY;
    int start = ksb_int(&p, 42);
    ksb_bool(&p, true);
    ksb_load(&p, KS_STR_CONST("print"));
    ksb_call(&p, 2);
    ksb_retnone(&p);
    */


    ks_prog prog = KS_PROG_EMPTY;

    ks_parse kp = KS_PARSE_EMPTY;

    const char* fname = "examples/hello_world.kscript";

    ks_str fin = KS_STR_EMPTY;
    FILE* fp = fopen(fname, "r");
    ks_str_readfp(&fin, fp);
    fclose(fp);

    //int res = ks_parse_setsrc(&kp, KS_STR_CONST("-"), KS_STR_CONST("const \"hello world\"; const 42;"));

    int res = ks_parse_setsrc(&kp, KS_STR_VIEW(fname, strlen(fname)), fin);


    if (kp.err.len > 0) {
        ks_error(kp.err._);
        return -1;
    }

    ks_ast call = ks_ast_new_call(ks_ast_new_var(KS_STR_CONST("print")), 1, (ks_ast[]){
        ks_ast_new_const_str(KS_STR_CONST("hello world"))
    });

    //ks_ast_codegen(call, &prog);
    //res = ks_parse_bc(&kp, &prog);
    ks_ast code = ks_parse_code(&kp);

    //ksb_retnone(&prog);

    if (kp.err.len > 0) {
        ks_error(kp.err._);
        return -1;
    }

    if (code == NULL) {
        ks_error(kp.err._);
        return -1;
    }

    res = ks_ast_codegen(code, &prog);

    if (res != 0) {
        ks_error("Codgen failed");
        return -1;
    }

    ksb_retnone(&prog);


    ks_str ns = KS_STR_EMPTY;
    ks_prog_tostr(&prog, &ns);
    printf("%s\n", ns._);

    ks_vm vm = KS_VM_EMPTY;

    // populate globals
    ks_dict_set_str(&vm.dict, KS_STR_CONST("print"), (kso)kso_F_print);
    ks_dict_set_str(&vm.dict, KS_STR_CONST("exit"), (kso)kso_F_exit);

    ks_exec(&vm, &prog, 0);

    // free our resources
    ks_vm_free(&vm);
    ks_prog_free(&prog);

    //printf("%s\n", kso_type_int->name._);
    return 0;
}

