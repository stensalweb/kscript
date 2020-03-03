/* funcs.c - implementation of the built in functions
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"


// declare them here
ks_cfunc 
    ks_F_print = NULL
    
;



static KS_FUNC(print) {
    KS_REQ_N_ARGS_MIN(n_args, 0);

    ks_str_b SB;
    ks_str_b_init(&SB);

    int i;
    for (i = 0; i < n_args; ++i) {
        // add spaces between them
        if (i != 0) ks_str_b_add_c(&SB, " ");
        // add to string to item
        ks_str_b_add_str(&SB, args[i]);
    }

    ks_str_b_add_c(&SB, "\n");

    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    // now print out the buffer
    fwrite(ret->chr, 1, ret->len, stdout);


    KS_DECREF(ret);

    return KSO_NONE;

}


// initialize all the functions
void ks_init_funcs() {
    ks_F_print = ks_cfunc_new(print_);
}


