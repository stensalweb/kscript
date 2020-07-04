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

// get C iteration values
// if getci(start, stop, step, len) = (first, last, delta), then
// getci(stop, start, -len) = (last, first, delta)
// getci(2, 8, 2) -> [2, 4, 6]
// getci(6, 0, -2)
bool ks_slice_getci(ks_slice self, int64_t len, int64_t* first, int64_t* last, int64_t* delta) {
    // translate self's params to integers
    int64_t start, stop, step;

    // convert step
    if (self->step == KSO_NONE) {
        step = 1;
    } else {
        if (!ks_num_get_int64(self->step, &step)) return false;
    }

    if (self->start == KSO_NONE) {
        start = step > 0 ? 0 : len - 1;
    } else {
        if (!ks_num_get_int64(self->start, &start)) return false;
        start = ((start % len) + len) % len;
    }

    if (self->stop == KSO_NONE) {
        stop = step > 0 ? len : -1;
    } else {
        if (!ks_num_get_int64(self->stop, &stop)) return false;
        stop = ((stop % len) + len) % len;
    }

    if (step == 0) {
        ks_throw_fmt(ks_type_ArgError, "Slices cannot have step==0");
        return false;
    }


    *first = start;
    *last = stop;
    
    if (step >= len || step <= -len) {
        // only 1 iteration
        *delta = 1;
        *last = *first + *delta; 
        return true;
    }


    if ((step > 0 && *last < *first) || (step < 0 && *last > *first)) {
        // no objects
        *delta = 1;
        *last = *first;
    }

    // otherwise, just set to step
    *delta = step;

    // difference
    int64_t diff = *last - *first;

    if (diff % *delta != 0) {
        // get the index after the last one
        *last = *first + *delta * (diff / *delta + 1);
    }

    return true;


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

