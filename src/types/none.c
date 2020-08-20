/* none.c - implementation of the 'none'-type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


static struct ks_none_s KS_NONE_s;
ks_none KS_NONE = &KS_NONE_s;




// none.__free__(self) -> free object
static KS_TFUNC(none, free) {
    ks_none self = NULL;
    KS_GETARGS("self:*", &self, ks_T_none)

    // reset references
    self->refcnt = KS_REFS_INF;

    return KSO_NONE;
}



// none.__str__(self) 
static KS_TFUNC(none, str) {
    ks_none self = NULL;
    KS_GETARGS("self:*", &self, ks_T_none)

 
    static ks_str s_none = NULL;

    // 1 time initialization
    if (!s_none) {
        s_none = ks_str_new("none");
    }

    return KS_NEWREF(s_none);

}


/* export */

KS_TYPE_DECLFWD(ks_T_none);

void ks_init_T_none() {
    KS_INIT_OBJ(KS_NONE, ks_T_none)

    ks_type_init_c(ks_T_none, "type(none)", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(none_free_, "none.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(none_str_, "none.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(none_str_, "none.__repr__(self)")},
    ));

    ks_T_none->flags |= KS_TYPE_FLAGS_EQSS;
}
