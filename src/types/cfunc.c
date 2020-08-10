/* cfunc.c - implementation of C-functions
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// Construct a new Cfunc with given name and signature
// NOTE: Returns new reference, or NULL if an error was thrown
ks_cfunc ks_cfunc_new(ks_cfunc_f func, ks_str name_hr, ks_str sig_hr) {
    ks_cfunc self = KS_ALLOC_OBJ(ks_cfunc);
    KS_INIT_OBJ(self, ks_T_cfunc);

    self->func = func;

    self->name_hr = name_hr;
    KS_INCREF(name_hr);
    
    self->sig_hr = sig_hr;
    KS_INCREF(sig_hr);

    return self;
}

// Construct a new Cfunc with given signature (the function name is taken as the substring up to '(')
// NOTE: Returns new reference, or NULL if an error was thrown
ks_cfunc ks_cfunc_new_c(ks_cfunc_f func, const char* sig) {
    char* first_lpar = strchr(sig, '(');
    assert (first_lpar != NULL && "sig did not contain '('");

    // construct objects
    ks_str name_hr = ks_str_new_c(sig, first_lpar - sig);
    ks_str sig_hr = ks_str_new_c(sig, -1);

    ks_cfunc self = ks_cfunc_new(func, name_hr, sig_hr);

    KS_DECREF(name_hr);
    KS_DECREF(sig_hr);

    return self;
}

// cfunc.__free__(self) -> free obj
static KS_TFUNC(cfunc, free) {
    ks_cfunc self;
    KS_GETARGS("self:*", &self, ks_T_cfunc)

    KS_DECREF(self->name_hr);
    KS_DECREF(self->sig_hr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// cfunc.__str__(self) -> to string
static KS_TFUNC(cfunc, str) {
    ks_cfunc self;
    KS_GETARGS("self:*", &self, ks_T_cfunc)
    
    return (ks_obj)ks_fmt_c("<'%T' : %S>", self, self->sig_hr);
};

/* export */

KS_TYPE_DECLFWD(ks_T_cfunc);

void ks_init_T_cfunc() {
    ks_type_init_c(ks_T_cfunc, "cfunc", ks_T_func, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(cfunc_free_, "cfunc.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c(cfunc_str_, "cfunc.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(cfunc_str_, "cfunc.__repr__(self)")},
    ));

}
