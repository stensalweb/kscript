/* pfunc.c - implementation of partial function wrappers
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// create new member func
ks_pfunc ks_pfunc_new(ks_obj func, ks_obj member_inst) {
    ks_pfunc self = KS_ALLOC_OBJ(ks_pfunc);
    KS_INIT_OBJ(self, ks_T_pfunc);

    self->func = func;
    KS_INCREF(func);

    self->member_inst = member_inst;
    KS_INCREF(member_inst);

    return self;
}


// pfunc.__free__(self) -> free obj
static KS_TFUNC(pfunc, free) {
    ks_pfunc self;
    KS_GETARGS("self:*", &self, ks_T_pfunc)

    KS_DECREF(self->func);
    KS_DECREF(self->member_inst);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_pfunc);

void ks_init_T_pfunc() {
    ks_type_init_c(ks_T_pfunc, "pfunc", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(pfunc_free_, "pfunc.__free__(self)")},
    ));

}
