
#include "kscript.h"


int main(int argc, char** argv) {

    ks_set_loglvl(KS_LOGLVL_DEBUG);

    // create a new program
    ks_prog p = KS_PROG_EMPTY;
    int start = ksb_int(&p, 42);
    ksb_bool(&p, true);
    ksb_load(&p, KS_STR_CONST("print"));
    ksb_call(&p, 2);
    ksb_retnone(&p);

    ks_vm vm = KS_VM_EMPTY;

    // populate globals
    ks_dict_set_str(&vm.dict, KS_STR_CONST("print"), (kso)kso_F_print);

    ks_exec(&vm, &p, start);

    // free our resources
    ks_vm_free(&vm);
    ks_prog_free(&p);

    //printf("%s\n", kso_type_int->name._);
    return 0;
}

