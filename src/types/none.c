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

// none.__free__(self) -> do nothing, as nones should never be freed
static KS_TFUNC(none, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_none self = (ks_none)args[0];
    KS_REQ_TYPE(self, ks_type_none, "self");


    // up the refcnt


    return KSO_NONE;
};


// initialize none type
void ks_type_none_init() {
    KS_INIT_TYPE_OBJ(ks_type_none, "none");

    ks_type_set_cn(ks_type_none, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_new_cfunc(none_free_)},
        {NULL, NULL}
    });

    // initialize global singleton
    KS_NONE = KS_ALLOC_OBJ(ks_none);

    KS_INIT_OBJ(KS_NONE, ks_type_none);

}

