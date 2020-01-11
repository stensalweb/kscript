/* types/cfunc.c - represents callable C functions */

#include "ks_common.h"


// create a new C-function wrapper
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc) {
    ks_cfunc self = (ks_cfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_cfunc) {
        KSO_BASE_INIT(ks_T_cfunc)
        .v_cfunc = v_cfunc
    };
    return self;
}

/* exporting functionality */

struct ks_type T_cfunc, *ks_T_cfunc = &T_cfunc;

void ks_init__cfunc() {

    /* create the type */
    T_cfunc = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_cfunc, "cfunc");

}



