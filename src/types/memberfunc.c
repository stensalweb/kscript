/* memberfunc.c - implementation of member function wrappers
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// create new member func
ks_memberfunc ks_memberfunc_new(ks_obj func, ks_obj member_inst) {
    ks_memberfunc self = KS_ALLOC_OBJ(ks_memberfunc);
    KS_INIT_OBJ(self, ks_T_memberfunc);

    self->func = func;
    KS_INCREF(func);

    self->member_inst = member_inst;
    KS_INCREF(member_inst);

    return self;
}


// memberfunc.__free__(self) -> free obj
static KS_TFUNC(memberfunc, free) {
    ks_memberfunc self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_memberfunc)) return NULL;

    KS_DECREF(self->func);
    KS_DECREF(self->member_inst);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_memberfunc);

void ks_init_T_memberfunc() {
    ks_type_init_c(ks_T_memberfunc, "memberfunc", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(memberfunc_free_, "memberfunc.__free__(self)")},
    ));

}
