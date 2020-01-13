/* types/pfunc.c - represents partial function wrappers */

#include "ks_common.h"


// create a new partial function wrapper
ks_pfunc ks_pfunc_new(kso func) {
    ks_pfunc self = (ks_pfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_pfunc) {
        KSO_BASE_INIT(ks_T_pfunc)
        .func = func
    };
    KSO_INCREF(func);

    self->n_fillin = 0;
    self->fillin_args = NULL;

    return self;
}

// fill a given argument index
void ks_pfunc_fill(ks_pfunc self, int arg_idx, kso val) {

    // hold a reference to it
    KSO_INCREF(val);

    int i;
    for (i = 0; i < self->n_fillin; ++i) {
        if (self->fillin_args[i].idx == arg_idx) {
            KSO_DECREF(self->fillin_args[i].val);
            self->fillin_args[i].val = val;
            return;

        } else if (self->fillin_args[i].idx > arg_idx) {
            // insert it here
            int j = self->n_fillin++;
            self->fillin_args = ks_realloc(self->fillin_args, sizeof(*self->fillin_args) * self->n_fillin);

            while (j > i) {
                self->fillin_args[j] = self->fillin_args[j - 1];
                j--;
            }

            self->fillin_args[i].idx = arg_idx;
            self->fillin_args[i].val = val;

            return;
        }
    }

    // add it afterwards
    i = self->n_fillin++;
    self->fillin_args = ks_realloc(self->fillin_args, sizeof(*self->fillin_args) * self->n_fillin);

    self->fillin_args[i].idx = arg_idx;
    self->fillin_args[i].val = val;


}

TFUNC(pfunc, free) {
    ks_pfunc self = (ks_pfunc)args[0];

    int i;
    for (i = 0; i < self->n_fillin; ++i) {
        KSO_DECREF(self->fillin_args[i].val);
    }

    ks_free(self->fillin_args);

    ks_free(self);
    return KSO_NONE;
}

/* exporting functionality */

struct ks_type T_pfunc, *ks_T_pfunc = &T_pfunc;

void ks_init__pfunc() {

    /* create the type */
    T_pfunc = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_pfunc, "pfunc");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_pfunc, "__free__", "pfunc.__free__(self)", pfunc_free_);

}



