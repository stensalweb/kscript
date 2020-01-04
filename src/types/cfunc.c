/* types/cfunc.c - represents callable C functions */

#include "ks_common.h"


// create a new C-function wrapper
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc) {
    ks_cfunc self = (ks_cfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_cfunc) {
        KSO_BASE_INIT(ks_T_cfunc, KSOF_NONE)
        .v_cfunc = v_cfunc
    };
    return self;
}

// create a new C-function wrapper with a reference
ks_cfunc ks_cfunc_newref(ks_cfunc_sig v_cfunc) {
    ks_cfunc self = (ks_cfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_cfunc) {
        KSO_BASE_INIT_R(ks_T_cfunc, KSOF_NONE, 1)
        .v_cfunc = v_cfunc
    };
    return self;
}


/* exporting functionality */

struct ks_type T_cfunc, *ks_T_cfunc = &T_cfunc;

void ks_init__cfunc() {

    /* first create the type */
    T_cfunc = (struct ks_type) {
        KS_TYPE_INIT("cfunc")

    };

}



