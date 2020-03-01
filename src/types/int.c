/* types/int.c - kscript's basic integer implementation (signed 64 bit) 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_int);

// create a kscript int from a C-style int
ks_int ks_new_int(int64_t val) {
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // initialize type-specific things
    self->val = val;

    return self;
}


// free a kscript int
void ks_free_int(ks_int self) {
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
}



// initialize int type
void ks_type_int_init() {
    KS_INIT_TYPE_OBJ(ks_type_int, "int");

}

