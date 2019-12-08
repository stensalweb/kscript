
#include "kscript.h"


int main(int argc, char** argv) {

    ks_set_loglvl(KS_LOGLVL_TRACE);

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

    /*
    ks_parse kp = KS_PARSE_EMPTY;

    ks_str fin = KS_STR_EMPTY;
    FILE* fp = fopen("examples/primes.ksasm", "r");
    ks_str_readfp(&fin, fp);
    fclose(fp);

    //int res = ks_parse_setsrc(&kp, KS_STR_CONST("-"), KS_STR_CONST("const \"hello world\"; const 42;"));
    int res = ks_parse_setsrc(&kp, KS_STR_CONST("examples/primes.ksasm"), fin);

    if (res != 0) {
        ks_error(kp.err._);
        return -1;
    }

    res = ks_parse_bc(&kp, &prog);

    if (res != 0) {
        ks_error(kp.err._);
        return -1;
    }*/

    ksb_int(&prog, 42);
    ksb_str(&prog, KS_STR_CONST("is the answer to everything"));

    ksb_load(&prog, KS_STR_CONST("print"));
    ksb_call(&prog, 2);
    ksb_retnone(&prog);


    /*
    ks_parse_setsrc(&kp, KS_STR_CONST("-"), KS_STR_CONST("42"));
    ks_int v = 0;
    printf("nchr: %d\n", ks_parse_int(&kp, &v));
    printf("%ld\n", v);
    */

    ks_vm vm = KS_VM_EMPTY;

    // populate globals
    ks_dict_set_str(&vm.dict, KS_STR_CONST("print"), (kso)kso_F_print);
    ks_dict_set_str(&vm.dict, KS_STR_CONST("exit"), (kso)kso_F_exit);

    //printf("EXECIN\n");
    //printf("%d\n", (int)prog.bc[0]);

    int i;
    for (i = 0; i < prog.str_tbl_n; ++i) {
        printf("[%d]: '%s'\n", i, prog.str_tbl[i]._);
    }

    ks_exec(&vm, &prog, 0);

    //ks_str tostr = ks_prog_tostr(&p);

    //printf("'%s'\n", tostr._);

    // free our resources
    ks_vm_free(&vm);
    ks_prog_free(&prog);
   // ks_parse_free(&kp);

    //printf("%s\n", kso_type_int->name._);
    return 0;
}

