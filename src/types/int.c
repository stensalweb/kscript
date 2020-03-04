/* types/int.c - kscript's basic integer implementation (signed 64 bit) 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_int);

// create a kscript int from a C-style int
ks_int ks_int_new(int64_t val) {
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // initialize type-specific things
    self->val = val;

    return self;
}

/* member functions */


// int.__str__(self) -> free an int object
static KS_TFUNC(int, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    // print it out
    char cstr[256];
    snprintf(cstr, 255, "%lli", (long long int)self->val);

    return (ks_obj)ks_str_new(cstr);

};


// int.__free__(self) -> free an int object
static KS_TFUNC(int, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// initialize int type
void ks_type_int_init() {
    KS_INIT_TYPE_OBJ(ks_type_int, "int");

    ks_type_set_cn(ks_type_int, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(int_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(int_str_)},
        
        {"__free__", (ks_obj)ks_cfunc_new(int_free_)},
        {NULL, NULL}   
    });
}

