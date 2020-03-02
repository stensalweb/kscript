
#include "ks.h"


int main(int argc, char** argv) {

    if (!ks_init()) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }

    int64_t smem = ks_mem_cur();

    // create a code with a constant array
    ks_list v_const = ks_new_list(0, NULL);
    ks_code c = ks_new_code(v_const);
    KS_DECREF(v_const);


    // push an integer on to the stack
    ks_int myval = ks_new_int(43);
    ks_str mystr = ks_new_str("Testingasdfasdfji ioasdfjio\n\n aijodsfjias");
    ksca_push(c, (ks_obj)myval);
    ksca_push(c, (ks_obj)myval);
    KS_DECREF(myval);
    ksca_push(c, (ks_obj)mystr);
    KS_DECREF(mystr);

    ksca_popu(c);

    ksca_jmp(c, -5);

    // return TOS
    ksca_ret(c);

    if (!ks_code_tofile(c, "mycode.ksbc")) {
        ks_error("Error writing to '%s'","mycode.ksbc");
    }

    ks_code read_back = ks_code_fromfile("mycode.ksbc");

    KS_DECREF(read_back);

    // now, execute that code
    vm_exec(c);

    // we are now done with the code
    KS_DECREF(c);
    
    ks_info("start: %l, cur: %l, max: %l", smem, ks_mem_cur(), ks_mem_max());

    return 0;
}
