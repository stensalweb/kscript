/* Enum.c - implementation of the enum class
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// Enum.__free__(self) - free string
static KS_TFUNC(Enum, free) {
    ks_Enum self;
    KS_GETARGS("self:*", &self, ks_T_Enum)

    KS_DECREF(self->name);
    KS_DECREF(self->enum_val);


    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_Enum);

void ks_init_T_Enum() {
    ks_type_init_c(ks_T_Enum, "Enum", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(Enum_free_, "Enum.__free__(self)")},
    ));

}
