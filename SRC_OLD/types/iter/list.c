/* types/iter/list.c - implementation of list iteration object,
 *   which supports the iteration over elements in a list
 * 
 */

#include "ks_common.h"

// construct a new list iterator from a list
ks_list_iter ks_list_iter_new(ks_list list_obj) {
    ks_list_iter self = (ks_list_iter)ks_malloc(sizeof(*self));
    *self = (struct ks_list_iter) {
        KSO_BASE_INIT(ks_T_list_iter)
        .list_obj = list_obj,
        .pos = 0, // start at beginning
    };

    KSO_INCREF(list_obj);
    return self;
}

// kscript function to construct a list iterator
KS_TFUNC(list_iter, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list list_obj = (ks_list)args[0];
    KS_REQ_TYPE(list_obj, ks_T_list, "list_obj");

    return (kso)ks_list_iter_new(list_obj);
}

// grab the next value from the iterator
KS_TFUNC(list_iter, next) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list_iter self = (ks_list_iter)args[0];
    KS_REQ_TYPE(self, ks_T_list_iter, "self");

    // make sure its
    if (self->pos >= self->list_obj->len) return kse_fmt("Iterator is exhausted!");

    // increment position
    int idx = self->pos++;

    return KSO_NEWREF(self->list_obj->items[idx]);
}


// free the list iterator object
KS_TFUNC(list_iter, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list_iter self = (ks_list_iter)args[0];
    KS_REQ_TYPE(self, ks_T_list_iter, "self");

    // don't free the list, unless this was the last reference
    KSO_DECREF(self->list_obj);

    ks_free(self);

    return KSO_NONE;
}


/* exporting functionality */

struct ks_type T_list_iter, *ks_T_list_iter = &T_list_iter;

void ks_init__list_iter() {

    /* create the type */
    T_list_iter = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_list_iter, "list_iter");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_list_iter, "__new__", "list_iter.__new__(list_iter)", list_iter_new_);
    ADDCF(ks_T_list_iter, "__next__", "list_iter.__next__(self)", list_iter_next_);
    ADDCF(ks_T_list_iter, "__free__", "list_iter.__free__(self)", list_iter_free_);
    

}




