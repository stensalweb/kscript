/* object.c - implementation of the most generic type, which all
 *   other types inherit from
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"




// object.__free__(self) - free obj
static KS_TFUNC(object, free) {
    ks_obj self;
    KS_GETARGS("self:*", &self, ks_T_object);

    // most generic cleanup
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// object.__bool__(self) - convert to boolean
static KS_TFUNC(object, bool) {
    ks_obj self;
    KS_GETARGS("self:*", &self, ks_T_object);

    return KSO_TRUE;
}



// object.__eq__(L, R) - whether two objects are equal
static KS_TFUNC(object, eq) {
    ks_obj L, R;
    KS_GETARGS("L:* R:*", &L, ks_T_object, &R, ks_T_object);

    return KSO_BOOL(L == R);
}

// object.__ne__(L, R) - whether two objects are NOT equal
static KS_TFUNC(object, ne) {
    ks_obj L, R;
    KS_GETARGS("L:* R:*", &L, ks_T_object, &R, ks_T_object);

    return KSO_BOOL(L != R);
}



/* export */

KS_TYPE_DECLFWD(ks_T_object);

void ks_init_T_object() {
    ks_type_init_c(ks_T_object, "object", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(object_free_, "object.__free__(self)")},

        {"__bool__",               (ks_obj)ks_cfunc_new_c(object_bool_, "object.__bool__(self)", "Turn an object into a boolean value. This is always true")},

        {"__eq__",                 (ks_obj)ks_cfunc_new_c(object_eq_, "object.__eq__(self)", "Compare two objects; return whether they refer to the same object")},
        {"__ne__",                 (ks_obj)ks_cfunc_new_c(object_ne_, "object.__eq__(self)", "Compare two objects; return whether they refer to different objects")},
    
    ));
}
