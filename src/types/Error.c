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

// Error.__new__() - create new error
static KS_TFUNC(Error, new) {
    KS_GETARGS("")

    ks_Error self = KS_ALLOC_OBJ(ks_Error);
    KS_INIT_OBJ(self, ks_T_Error);

    self->attr = ks_dict_new(0, NULL);
    self->what = ks_str_new("");

    return (ks_obj)self;
}


// Error.__init__(self, what)
static KS_TFUNC(Error, init) {
    ks_Error self;
    ks_str what;
    KS_GETARGS("self:* what:*", &self, ks_T_Error, &what, ks_T_str)


    ks_dict_set_c(self->attr, KS_KEYVALS(
        {"what",                   KS_NEWREF(what)},
    ));

    self->what = what;

    return KSO_NONE;

}


// Error.__str__(self) -> to string
static KS_TFUNC(Error, str) {
    ks_Error self;
    KS_GETARGS("self:*", &self, ks_T_Error)

    return KS_NEWREF(self->what);
}

// Error.__free__(self) -> free object
static KS_TFUNC(Error, free) {
    ks_Error self = NULL;
    KS_GETARGS("self:*", &self, ks_T_Error)

    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_Error);



KS_TYPE_DECLFWD(ks_T_InternalError)
KS_TYPE_DECLFWD(ks_T_ImportError)
KS_TYPE_DECLFWD(ks_T_SyntaxError)
KS_TYPE_DECLFWD(ks_T_IOError)
KS_TYPE_DECLFWD(ks_T_TodoError)    
KS_TYPE_DECLFWD(ks_T_KeyError)
KS_TYPE_DECLFWD(ks_T_AttrError)
KS_TYPE_DECLFWD(ks_T_TypeError)
KS_TYPE_DECLFWD(ks_T_ArgError)
KS_TYPE_DECLFWD(ks_T_SizeError)
KS_TYPE_DECLFWD(ks_T_OpError)
KS_TYPE_DECLFWD(ks_T_OutOfIterError)
KS_TYPE_DECLFWD(ks_T_OutOfMemError)
KS_TYPE_DECLFWD(ks_T_MathError)
KS_TYPE_DECLFWD(ks_T_AssertError)



void ks_init_T_Error() {
    // initialize singletons

    ks_type_init_c(ks_T_Error, "Error", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c_old(Error_new_, "Error.__new__()")},
        {"__init__",               (ks_obj)ks_cfunc_new_c_old(Error_init_, "Error.__init__(what)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(Error_str_, "Error.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(Error_free_, "Error.__free__(self)")},
    ));

    #define SUBTYPE(_name) ks_type_init_c(ks_T_##_name, #_name, ks_T_Error, KS_KEYVALS());


    SUBTYPE(InternalError)
    SUBTYPE(ImportError)
    SUBTYPE(SyntaxError)
    SUBTYPE(IOError)
    SUBTYPE(TodoError)    
    SUBTYPE(KeyError)
    SUBTYPE(AttrError)
    SUBTYPE(TypeError)
    SUBTYPE(ArgError)
    SUBTYPE(SizeError)
    SUBTYPE(OpError)
    SUBTYPE(OutOfIterError)
    SUBTYPE(OutOfMemError)
    SUBTYPE(MathError)
    SUBTYPE(AssertError)


    #undef SUBTYPE
}
