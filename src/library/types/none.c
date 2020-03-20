/* types/none.c - the none-type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_none);

ks_none KS_NONE;

/* member functions */


// none.__str__(self) -> convert to string
static KS_TFUNC(none, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_none self = (ks_none)args[0];
    KS_REQ_TYPE(self, ks_type_none, "self");

    // static globals so it only calculates once
    static ks_str s_none = NULL;

    // only update once
    if (!s_none) {
        s_none = ks_str_new("none");
    }

    // new reference
    return KS_NEWREF(s_none);
};



// none.__free__(self) -> do nothing, as nones should never be freed
static KS_TFUNC(none, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_none self = (ks_none)args[0];
    KS_REQ_TYPE(self, ks_type_none, "self");

    // up the refcnt so it won't be freed for a while
    self->refcnt = INT32_MAX;

    return KSO_NONE;
};


// initialize none type
void ks_type_none_init() {
    KS_INIT_TYPE_OBJ(ks_type_none, "none");

    ks_type_set_cn(ks_type_none, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new2(none_str_, "none.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(none_str_, "none.__repr__(self)")},

        {"__free__", (ks_obj)ks_cfunc_new2(none_free_, "none.__free__(self)")},
        {NULL, NULL}
    });

    // initialize global singleton
    KS_NONE = KS_ALLOC_OBJ(ks_none);

    KS_INIT_OBJ(KS_NONE, ks_type_none);

}

