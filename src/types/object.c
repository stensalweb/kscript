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


/* export */

KS_TYPE_DECLFWD(ks_T_object);

void ks_init_T_object() {
    ks_type_init_c(ks_T_object, "object", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(object_free_, "object.__free__(self)")},
    ));

}
