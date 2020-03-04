/* types/cfunc.c - C-style function wrapper class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_cfunc);

// create a kscript int from a C-style int
ks_cfunc ks_cfunc_new(ks_obj (*func)(int n_args, ks_obj* args)) {
    ks_cfunc self = KS_ALLOC_OBJ(ks_cfunc);
    KS_INIT_OBJ(self, ks_type_cfunc);

    // initialize type-specific things
    self->func = func;

    return self;
}


// free a kscript cfunc
void ks_free_cfunc(ks_cfunc self) {
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
}


/* member functions */

// cfunc.__free__(self) -> free a cfunc
static KS_TFUNC(cfunc, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_cfunc self = (ks_cfunc)args[0];
    KS_REQ_TYPE(self, ks_type_cfunc, "self");
    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// initialize cfunc type
void ks_type_cfunc_init() {
    KS_INIT_TYPE_OBJ(ks_type_cfunc, "cfunc");

    ks_type_set_cn(ks_type_cfunc, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new(cfunc_free_)},

        {NULL, NULL}   
    });

}

