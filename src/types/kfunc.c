/* kfunc.c - kscript function class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// create a kscript function
ks_kfunc ks_kfunc_new(ks_code code, ks_str hr_name) {
    ks_kfunc self = KS_ALLOC_OBJ(ks_kfunc);
    KS_INIT_OBJ(self, ks_T_kfunc);

    // initialize type-specific things
    self->name_hr = hr_name;
    KS_INCREF(hr_name);

    self->code = code;
    KS_INCREF(code);


    // list of closures
    self->closures = ks_list_new(0, NULL);

    self->isVarArg = false;

    self->n_param = 0;
    self->params = NULL;

    self->n_defa = 0;
    self->defa_start_idx = -1;
    self->defas = NULL;

    return self;
}


// create copy
ks_kfunc ks_kfunc_new_copy(ks_kfunc func) {
    ks_kfunc self = KS_ALLOC_OBJ(ks_kfunc);
    KS_INIT_OBJ(self, ks_T_kfunc);

    self->name_hr = (ks_str)KS_NEWREF(func->name_hr);
    self->code = func->code;
    KS_INCREF(func->code);

    // list of closures
    self->closures = ks_list_new(func->closures->len, func->closures->elems);

    self->isVarArg = func->isVarArg;

    self->n_param = 0;
    self->params = NULL;

    self->n_defa = 0;
    self->defa_start_idx = -1;
    self->defas = NULL;

    int i;
    for (i = 0; i < func->n_param; ++i) {
        ks_kfunc_addpar(self, func->params[i].name, func->params[i].defa);
    }

    return self;
}



// add a parametyer
void ks_kfunc_addpar(ks_kfunc self, ks_str name, ks_obj defa) {
    int idx = self->n_param++;
    self->params = ks_realloc(self->params, sizeof(*self->params) * self->n_param);
    self->params[idx] = (struct ks_kfunc_param) {
        .name = (ks_str)KS_NEWREF(name),
        .defa = defa,
    };

}


// kfunc.__free__(self) -> free a kfunc
static KS_TFUNC(kfunc, free) {
    ks_kfunc self;
    KS_GETARGS("self:*", &self, ks_T_kfunc)

    int i;
    for (i = 0; i < self->n_param; ++i) {
        KS_DECREF(self->params[i].name);
        if (self->params[i].defa) KS_DECREF(self->params[i].defa);
    }

    ks_free(self->params);

    // no need for the closures anymore
    KS_DECREF(self->closures);

    KS_DECREF(self->name_hr);
    KS_DECREF(self->code);
    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};





/* export */

KS_TYPE_DECLFWD(ks_T_kfunc);

void ks_init_T_kfunc() {
    ks_type_init_c(ks_T_kfunc, "kfunc", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(kfunc_free_, "kfunc.__free__(self)")},
    ));

}


