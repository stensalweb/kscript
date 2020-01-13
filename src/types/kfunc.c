/* types/kfunc.c - represents callable kscript functions */

#include "ks_common.h"

// create a new kfunc
ks_kfunc ks_kfunc_new(ks_list params, ks_code code) {
    ks_kfunc self = (ks_kfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_kfunc) {
        KSO_BASE_INIT(ks_T_kfunc)
        .code = code,
        .params = params
    };

    KSO_INCREF(params);
    KSO_INCREF(code);
    return self;
}


TFUNC(kfunc, free) {
    #define SIG "kfunc.__free__(self)"
    REQ_N_ARGS(1);
    ks_kfunc self = (ks_kfunc)args[0];
    REQ_TYPE("self", self, ks_T_kfunc);

    KSO_DECREF(self->params);
    KSO_DECREF(self->code);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}

/* exporting functionality */

struct ks_type T_kfunc, *ks_T_kfunc = &T_kfunc;

void ks_init__kfunc() {

    /* create the type */
    T_kfunc = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_kfunc, "kfunc");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_kfunc, "__free__", "kfunc.__free__(self)", kfunc_free_);

}


