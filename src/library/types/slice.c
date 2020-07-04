/* types/slice.c - slice object
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_slice);

// create a kscript int from a C-style int
ks_slice ks_slice_new(ks_obj start, ks_obj stop, ks_obj step) {
    ks_slice self = KS_ALLOC_OBJ(ks_slice);
    KS_INIT_OBJ(self, ks_type_slice);

    self->start = KS_NEWREF(start);
    self->stop = KS_NEWREF(stop);
    self->step = KS_NEWREF(step);

    return self;
}



// slice.__new__(start=none, stop=none, step=none)
static KS_TFUNC(slice, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 3);

    return (ks_obj)ks_slice_new(n_args>0?args[0]:KSO_NONE, n_args>1?args[1]:KSO_NONE, n_args>2?args[2]:KSO_NONE);
}


// slice.__str__(self) -> convert to string
static KS_TFUNC(slice, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_slice self = (ks_slice)args[0];
    KS_REQ_TYPE(self, ks_type_slice, "self");

    return (ks_obj)ks_fmt_c("slice(%S, %S, %S)", self->start, self->stop, self->step);
}

// slice.__free__(self) -> frees resources
static KS_TFUNC(slice, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_slice self = (ks_slice)args[0];
    KS_REQ_TYPE(self, ks_type_slice, "self");

    KS_DECREF(self->start);
    KS_DECREF(self->stop);
    KS_DECREF(self->step);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}



// initialize slice type
void ks_type_slice_init() {
    KS_INIT_TYPE_OBJ(ks_type_slice, "slice");

    ks_type_set_cn(ks_type_slice, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(slice_new_, "slice.__new__(start=none, stop=none, step=none)")},

        {"__str__", (ks_obj)ks_cfunc_new2(slice_str_, "slice.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(slice_str_, "slice.__repr__(self)")},
        {"__free__", (ks_obj)ks_cfunc_new2(slice_free_, "slice.__free__(self)")},


        {NULL, NULL}   
    });

}

