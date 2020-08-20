/* cfunc.c - implementation of C-functions
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// Construct a new Cfunc with given name and signature
// NOTE: Returns new reference, or NULL if an error was thrown
ks_cfunc ks_cfunc_new(ks_cfunc_f func, ks_str name_hr, ks_str sig_hr, ks_str doc) {
    ks_cfunc self = KS_ALLOC_OBJ(ks_cfunc);
    KS_INIT_OBJ(self, ks_T_cfunc);

    self->func = func;

    self->name_hr = name_hr;
    KS_INCREF(name_hr);
    
    self->sig_hr = sig_hr;
    KS_INCREF(sig_hr);

    self->doc = doc;
    KS_INCREF(doc);

    return self;
}

// Construct a new Cfunc with given signature (the function name is taken as the substring up to '(')
// NOTE: Returns new reference, or NULL if an error was thrown
// TODO: implement docstrings
ks_cfunc ks_cfunc_new_c(ks_cfunc_f func, const char* sig, const char* doc) {
    char* first_lpar = strchr(sig, '(');
    assert (first_lpar != NULL && "sig did not contain '('");

    // construct objects
    ks_str name_hr = ks_str_new_c(sig, first_lpar - sig);
    ks_str sig_hr = ks_str_new_c(sig, -1);
    ks_str doc_obj = ks_str_new(doc);

    ks_cfunc self = ks_cfunc_new(func, name_hr, sig_hr, doc_obj);

    KS_DECREF(name_hr);
    KS_DECREF(sig_hr);
    KS_DECREF(doc_obj);

    return self;
}
// Construct a new Cfunc with given signature (the function name is taken as the substring up to '(')
// NOTE: Returns new reference, or NULL if an error was thrown
ks_cfunc ks_cfunc_new_c_old(ks_cfunc_f func, const char* sig) {
    return ks_cfunc_new_c(func, sig, "");
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
}


// cfunc.__getattr__(self, attr) - get attributes
static KS_TFUNC(cfunc, getattr) {
    ks_cfunc self;
    ks_str attr;
    KS_GETARGS("self:* attr:*", &self, ks_T_cfunc, &attr, ks_T_str)
    
    if (ks_str_eq_c(attr, "__doc__", 7)) {
        return KS_NEWREF(self->doc);
    } else {
        KS_THROW_ATTR_ERR(self, attr);
    }
};


/* export */

KS_TYPE_DECLFWD(ks_T_cfunc);

void ks_init_T_cfunc() {
    ks_type_init_c(ks_T_cfunc, "cfunc", ks_T_func, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(cfunc_free_, "cfunc.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(cfunc_str_, "cfunc.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(cfunc_str_, "cfunc.__repr__(self)")},

        {"__getattr__",            (ks_obj)ks_cfunc_new_c_old(cfunc_getattr_, "cfunc.__getattr__(self, attr)")},

    ));

}
