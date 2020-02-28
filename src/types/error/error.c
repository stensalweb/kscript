/* types/error/error.c - type representing a generic error type 
 *
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks_common.h"


ks_error ks_error_new(ks_str what) {
    ks_error self = (ks_error)ks_malloc(sizeof(*self));
    *self = (struct ks_error) {
        KSO_BASE_INIT(ks_T_error)
        .attr = ks_dict_new_empty()
    };

    ks_dict_set_cstr(self->attr, "what", (kso)what);
    return self;
}


// error.__new__(what) -> constructs a new error given a message
KS_TFUNC(error, new) {
    KS_REQ_N_ARGS(n_args, 1);
    kso what = args[0];

    ks_str what_str = kso_tostr(what);
    if (!what_str) return NULL;

    return (kso)ks_error_new(what_str);
}

// error.__str__(what) -> constructs an error-string
KS_TFUNC(error, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_error self = (ks_error)args[0];
    KS_REQ_SUBTYPE(self, ks_T_error, "self");

    return KSO_NEWREF(ks_dict_get_cstr(self->attr, "what"));
}


/* exporting functionality */

struct ks_type T_error, *ks_T_error = &T_error;

void ks_init__error() {
    T_error = KS_TYPE_INIT();

    ks_type_setname_c(ks_T_error, "Error");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    // derive from `kobj`
    ks_type_add_parent(ks_T_error, ks_T_kobj);

    ADDCF(ks_T_error, "__new__", "Error.__new__(what)", error_new_);

    ADDCF(ks_T_error, "__str__", "Error.__str__(self)", error_str_);

}


