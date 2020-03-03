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



// initialize cfunc type
void ks_type_cfunc_init() {
    KS_INIT_TYPE_OBJ(ks_type_cfunc, "cfunc");

}

