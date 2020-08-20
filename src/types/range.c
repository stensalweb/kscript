/* range.c - implementation of the 'range' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"



// create a kscript int from a C-style int
ks_range ks_range_new(ks_int start, ks_int stop, ks_int step) {
    ks_range self = KS_ALLOC_OBJ(ks_range);
    KS_INIT_OBJ(self, ks_T_range);

    self->start = (ks_int)KS_NEWREF(start);
    self->stop = (ks_int)KS_NEWREF(stop);
    self->step = (ks_int)KS_NEWREF(step);

    return self;
}


// range.__new__(*args) - create new range
static KS_TFUNC(range, new) {
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("*args", &n_extra, &extra);

    if (n_extra == 1) {
        // use start=0, stop=extra[0], step=1

        ks_int t0 = ks_int_new(0);
        ks_int t1 = ks_int_new(1);
        ks_int stop = ks_num_get_int(extra[0]);

        ks_range ret = ks_range_new(t0, stop, t1);

        KS_DECREF(t0);
        KS_DECREF(t1);
        KS_DECREF(stop);

        return (ks_obj)ret;

    } else if (n_extra == 2) {
        // use start=extra[0], stop=extra[1], step=1
        ks_int t1 = ks_int_new(1);
        ks_int start = ks_num_get_int(extra[0]);
        ks_int stop = ks_num_get_int(extra[1]);

        ks_range ret = ks_range_new(start, stop, t1);

        KS_DECREF(t1);
        KS_DECREF(start);
        KS_DECREF(stop);

        return (ks_obj)ret;
    } else {
        // all 3 are given

        // use start=extra[0], stop=extra[1], step=1
        ks_int start = ks_num_get_int(extra[0]);
        ks_int stop = ks_num_get_int(extra[1]);
        ks_int step = ks_num_get_int(extra[2]);

        ks_range ret = ks_range_new(start, stop, step);

        KS_DECREF(start);
        KS_DECREF(stop);
        KS_DECREF(step);

        return (ks_obj)ret;

    }
}


// range._free__(self) - free resources
static KS_TFUNC(range, free) {
    ks_range self;
    KS_GETARGS("self:*", &self, ks_T_range)

    KS_DECREF(self->start);
    KS_DECREF(self->stop);
    KS_DECREF(self->step);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// range.__str__(self) - turn to string
static KS_TFUNC(range, str) {
    ks_range self;
    KS_GETARGS("self:*", &self, ks_T_range)
    // check short cuts
    if (ks_int_cmp_c(self->step, 1) == 0) {
        if (ks_int_cmp_c(self->start, 0) == 0) {
            return (ks_obj)ks_fmt_c("range(%S)", self->stop);
        } else {
            return (ks_obj)ks_fmt_c("range(%S, %S)", self->start, self->stop);
        }
    } else {
        return (ks_obj)ks_fmt_c("range(%S, %S, %S)", self->start, self->stop, self->step);
    }
}


/* iterator type */

// ks_range_iter - range iterator
typedef struct {
    KS_OBJ_BASE

    // the range being iterated over
    ks_range self;

    // current value
    ks_int cur;

    // precomputed value of `ks_int_cmp(self->start, self->stop)`
    int cmpsign;


}* ks_range_iter;

// declare type
KS_TYPE_DECLFWD(ks_T_range_iter);

// range_iter.__free__(self) - free obj
static KS_TFUNC(range_iter, free) {
    ks_range_iter self;
    KS_GETARGS("self:*", &self, ks_T_range_iter)

    // remove reference to string
    KS_DECREF(self->self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// range_iter.__next__(self) - return next character
static KS_TFUNC(range_iter, next) {
    ks_range_iter self;
    KS_GETARGS("self:*", &self, ks_T_range_iter)

    // out of range for sure
    if (self->cmpsign == 0) return ks_throw(ks_T_OutOfIterError, "");
    
    // compare to stopping pooint
    int ccmp = ks_int_cmp(self->cur, self->self->stop);

    // also out of range
    if ((self->cmpsign < 0 && ccmp >= 0) || (self->cmpsign > 0 && ccmp <= 0))  return ks_throw(ks_T_OutOfIterError, "");

    // return this
    ks_int ret = self->cur;

    // advance
    self->cur = (ks_int)ks_num_add((ks_obj)self->cur, (ks_obj)self->self->step);

    return (ks_obj)ret;
}

// range.__iter__(self) - return iterator
static KS_TFUNC(range, iter) {
    ks_range self;
    KS_GETARGS("self:*", &self, ks_T_range)

    ks_range_iter ret = KS_ALLOC_OBJ(ks_range_iter);
    KS_INIT_OBJ(ret, ks_T_range_iter);

    ret->self = self;
    KS_INCREF(self);

    // position at start
    ret->cur = (ks_int)KS_NEWREF(self->start);

    // precompute this
    ret->cmpsign = ks_int_cmp(self->start, self->stop);

    return (ks_obj)ret;
}


/* export */

KS_TYPE_DECLFWD(ks_T_range);

void ks_init_T_range() {
    ks_type_init_c(ks_T_range, "range", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c_old(range_new_, "range.__new__(*args)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(range_free_, "range.__free__(self)")},
        
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(range_str_, "range.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(range_str_, "range.__repr__(self)")}
        ,
        {"__iter__",               (ks_obj)ks_cfunc_new_c_old(range_iter_, "range.__iter__(self)")},
    ));

    ks_type_init_c(ks_T_range_iter, "range_iter", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(range_iter_free_, "range_iter.__free__(self)")},

        {"__next__",               (ks_obj)ks_cfunc_new_c_old(range_iter_next_, "range_iter.__next__(self)")},
    ));

}


