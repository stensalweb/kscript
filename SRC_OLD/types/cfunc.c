/* types/cfunc.c - represents callable C functions */

#include "ks_common.h"


// create a new C-function wrapper
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc, char* sig_s) {
    ks_cfunc self = (ks_cfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_cfunc) {
        KSO_BASE_INIT(ks_T_cfunc)
        .sig_s   = ks_str_new(sig_s),
        .v_cfunc = v_cfunc,
    };
    return self;
}

// free a function
KS_TFUNC(cfunc, free) {
    ks_cfunc self = (ks_cfunc)args[0];
    KS_REQ_TYPE(self, ks_T_cfunc, "self");

    KSO_DECREF(self->sig_s);

    ks_free(self);

    return KSO_NONE;
}

/* exporting functionality */

struct ks_type T_cfunc, *ks_T_cfunc = &T_cfunc;

void ks_init__cfunc() {

    /* create the type */
    T_cfunc = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_cfunc, "cfunc");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_cfunc, "__free__", "cfunc.__free__(self)", cfunc_free_);
}



