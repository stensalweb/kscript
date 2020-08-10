/* slice.c - implementation of the 'slice' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"



// create a kscript int from a C-style int
ks_slice ks_slice_new(ks_obj start, ks_obj stop, ks_obj step) {
    ks_slice self = KS_ALLOC_OBJ(ks_slice);
    KS_INIT_OBJ(self, ks_T_slice);

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
        ks_throw(ks_T_ArgError, "Slices cannot have step==0");
        return false;
    }

    *first = start;
    *last = stop;
    
    if (step >= len || step <= -len) {
        // only 1 iteration
        *delta = 1;
        *last = *first + *delta; 
        return true;
    } else if ((step > 0 && *last < *first) || (step < 0 && *last > *first)) {
        // no objects
        *delta = 1;
        *last = *first;
    } else {
        // otherwise, just set to step
        *delta = step;
    }



    // difference
    int64_t diff = *last - *first;

    if (diff % *delta != 0) {
        // get the index after the last one
        *last = *first + *delta * (diff / *delta + 1);
    }

    return true;
}




// slice.__free__(self) - free object
static KS_TFUNC(slice, free) {
    ks_slice self;
    KS_GETARGS("self:*", &self, ks_T_slice)

    KS_DECREF(self->start);
    KS_DECREF(self->stop);
    KS_DECREF(self->step);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_slice);

void ks_init_T_slice() {
    ks_type_init_c(ks_T_slice, "slice", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(slice_free_, "slice.__free__(self)")},
    ));

}


