/* types/kfunc.c - kscript functino class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_kfunc);

// create a kscript function
ks_kfunc ks_kfunc_new(ks_code code, ks_str hr_name) {
    ks_kfunc self = KS_ALLOC_OBJ(ks_kfunc);
    KS_INIT_OBJ(self, ks_type_kfunc);

    // initialize type-specific things
    self->name_hr = hr_name;
    KS_INCREF(hr_name);

    self->code = code;
    KS_INCREF(code);


    // list of closures
    self->closures = ks_list_new(0, NULL);

    self->n_param = 0;
    self->params = NULL;

    return self;
}


// create copy
ks_kfunc ks_kfunc_new_copy(ks_kfunc func) {
    ks_kfunc self = KS_ALLOC_OBJ(ks_kfunc);
    KS_INIT_OBJ(self, ks_type_kfunc);

    self->name_hr = (ks_str)KS_NEWREF(func->name_hr);
    self->code = (ks_code)KS_NEWREF(func->code);

    // list of closures
    self->closures = ks_list_new(func->closures->len, func->closures->elems);

    self->n_param = 0;
    self->params = NULL;

    int i;
    for (i = 0; i < func->n_param; ++i) {
        ks_kfunc_addpar(self, func->params[i].name);
    }

    return self;
}



// add a parametyer
void ks_kfunc_addpar(ks_kfunc self, ks_str name) {
    int idx = self->n_param++;
    self->params = ks_realloc(self->params, sizeof(*self->params) * self->n_param);
    self->params[idx] = (struct ks_kfunc_param) {
        .name = (ks_str)KS_NEWREF(name)
    };
}


/* member functions */

// kfunc.__free__(self) -> free a kfunc
static KS_TFUNC(kfunc, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_kfunc self = (ks_kfunc)args[0];
    KS_REQ_TYPE(self, ks_type_cfunc, "self");

    // no need for the closures anymore
    KS_DECREF(self->closures);

    KS_DECREF(self->name_hr);
    KS_DECREF(self->code);
    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


/*
// cfunc.__str__(self) -> to string
static KS_TFUNC(cfunc, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_cfunc self = (ks_cfunc)args[0];
    KS_REQ_TYPE(self, ks_type_cfunc, "self");
    
    return (ks_obj)ks_fmt_c("<'%T' : %S>", self, self->name_hr);
};*/




// initialize cfunc type
void ks_type_kfunc_init() {
    KS_INIT_TYPE_OBJ(ks_type_kfunc, "kfunc");

    ks_type_set_cn(ks_type_kfunc, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new(kfunc_free_)},

        {NULL, NULL}   
    });

}

