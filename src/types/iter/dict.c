/* types/iter/dict.c - implementation of dict iteration object,
 *   which supports the iteration over entries in a dictionary, in (key, val) tuples
 * 
 */

#include "ks_common.h"

// construct a new dict iterator from a dict
ks_dict_iter ks_dict_iter_new(ks_dict dict_obj) {
    ks_dict_iter self = (ks_dict_iter)ks_malloc(sizeof(*self));
    *self = (struct ks_dict_iter) {
        KSO_BASE_INIT(ks_T_dict_iter)
        .dict_obj = dict_obj,
        .pos = 0, // start at beginning
    };

    KSO_INCREF(dict_obj);
    return self;
}

// kscript function to construct a dict iterator
TFUNC(dict_iter, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict dict_obj = (ks_dict)args[0];
    KS_REQ_TYPE(dict_obj, ks_T_dict, "dict_obj");

    return (kso)ks_dict_iter_new(dict_obj);
}

// grab the next value from the iterator
TFUNC(dict_iter, next) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict_iter self = (ks_dict_iter)args[0];
    KS_REQ_TYPE(self, ks_T_dict_iter, "self");


    int idx = self->pos;

    bool found = false;

    while (idx < self->dict_obj->n_buckets) {
        if (self->dict_obj->buckets[idx].val != NULL) {
            // we've found one
            found = true;
            break;
        }

        // didn't find it, increase
        idx++;
    }

    if (!found) return kse_fmt("Iterator is exhausted!");

    // else, update position for next time
    self->pos = idx + 1;

    // construct a tuple from key, val
    return ks_tuple_new((kso[]){ self->dict_obj->buckets[idx].key, self->dict_obj->buckets[idx].val }, 2);
}


// free the dict iterator object
TFUNC(dict_iter, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict_iter self = (ks_dict_iter)args[0];
    KS_REQ_TYPE(self, ks_T_dict_iter, "self");

    // don't free the dict, unless this was the last reference
    KSO_DECREF(self->dict_obj);

    ks_free(self);

    return KSO_NONE;
}


/* exporting functionality */

struct ks_type T_dict_iter, *ks_T_dict_iter = &T_dict_iter;

void ks_init__dict_iter() {

    /* create the type */
    T_dict_iter = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_dict_iter, "dict_iter");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_dict_iter, "__new__", "dict_iter.__new__(dict_iter)", dict_iter_new_);
    ADDCF(ks_T_dict_iter, "__next__", "dict_iter.__next__(self)", dict_iter_next_);
    ADDCF(ks_T_dict_iter, "__free__", "dict_iter.__free__(self)", dict_iter_free_);
    

}




