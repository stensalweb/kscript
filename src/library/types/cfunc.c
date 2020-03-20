/* types/cfunc.c - C-style function wrapper class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_cfunc);

// create a kscript function
ks_cfunc ks_cfunc_new(ks_obj (*func)(int n_args, ks_obj* args)) {
    ks_cfunc self = KS_ALLOC_OBJ(ks_cfunc);
    KS_INIT_OBJ(self, ks_type_cfunc);

    // initialize type-specific things
    self->func = func;
    self->name_hr = ks_fmt_c("...", self);

    return self;
}


// create a functino with a hrname
ks_cfunc ks_cfunc_new2(ks_obj (*func)(int n_args, ks_obj* args), char* hrname) {
    ks_cfunc self = KS_ALLOC_OBJ(ks_cfunc);
    KS_INIT_OBJ(self, ks_type_cfunc);

    // initialize type-specific things
    self->func = func;
    self->name_hr = ks_str_new(hrname);

    return self;
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

// cfunc.__str__(self) -> to string
static KS_TFUNC(cfunc, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_cfunc self = (ks_cfunc)args[0];
    KS_REQ_TYPE(self, ks_type_cfunc, "self");
    
    return (ks_obj)ks_fmt_c("<'%T' : %S>", self, self->name_hr);
};


// initialize cfunc type
void ks_type_cfunc_init() {
    KS_INIT_TYPE_OBJ(ks_type_cfunc, "cfunc");

    ks_type_set_cn(ks_type_cfunc, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new2(cfunc_free_, "cfunc.__free__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new2(cfunc_str_, "cfunc.__str__(self)")},

        {NULL, NULL}   
    });

}

