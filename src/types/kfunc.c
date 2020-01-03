/* types/kfunc.c - represents callable kscript functions */

#include "ks_common.h"

// create a new kfunc
ks_kfunc ks_kfunc_new(ks_list params, ks_code code) {
    ks_kfunc self = (ks_kfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_kfunc) {
        KSO_BASE_INIT(ks_T_kfunc, KSOF_NONE)
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

    /* first create the type */
    T_kfunc = (struct ks_type) {
        KS_TYPE_INIT("kfunc")

        .f_free = (kso)ks_cfunc_newref(kfunc_free_)
    };

}




