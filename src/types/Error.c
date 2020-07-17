/* Error.c - implementation of standard error types in kscript
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"




// Construct a new error
ks_Error ks_Error_new(ks_type errtype, ks_str what) {
    ks_Error self = KS_ALLOC_OBJ(ks_Error);
    KS_INIT_OBJ(self, errtype);

    self->attr = ks_dict_new_c(KS_KEYVALS(
        {"what",                   KS_NEWREF(what)},
    ));
    self->what = what;

    return self;
}


// Error.__str__(self) -> to string
static KS_TFUNC(Error, str) {
    ks_Error self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_Error)) return NULL;

    return KS_NEWREF(self->what);
}

// Error.__free__(self) -> free object
static KS_TFUNC(Error, free) {
    ks_Error self = NULL;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_Error)) return NULL;

    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_Error);
KS_TYPE_DECLFWD(ks_T_InternalError);
KS_TYPE_DECLFWD(ks_T_ArgError);
KS_TYPE_DECLFWD(ks_T_KeyError);

void ks_init_T_Error() {
    // initialize singletons

    ks_type_init_c(ks_T_Error, "Error", ks_T_obj, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_cfunc_new_c(Error_str_, "Error.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(Error_free_, "Error.__free__(self)")},
    ));

    #define SUBTYPE(_name) ks_type_init_c(ks_T_##_name, #_name, ks_T_Error, KS_KEYVALS());

    SUBTYPE(InternalError);
    SUBTYPE(ArgError);
    SUBTYPE(KeyError);


}
