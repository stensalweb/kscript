/* types/list.c - implementation of the standard list type
 *
 * Essentially, it holds a list of references to other kscript objects,
 *   and can be resized dynamically
 * 
 */

#include "ks_common.h"

// create a list from a pointer to items
ks_list ks_list_new(kso* items, int n_items) {
    ks_list self = (ks_list)ks_malloc(sizeof(*self));
    *self = (struct ks_list) {
        KSO_BASE_INIT(ks_T_list)
        .len = n_items,
        .items = (kso*)ks_malloc(n_items * sizeof(kso))
    };

    int i;
    for (i = 0; i < n_items; ++i) {
        self->items[i] = items[i];
        // record this tuple's reference to it
        KSO_INCREF(items[i]);
    }
    return self;
}

// create a new empty list (i.e. has no elements)
ks_list ks_list_new_empty() {
    return ks_list_new(NULL, 0);
}

// push an object onto the list, returning the index
// NOTE: This adds a reference to the object
int ks_list_push(ks_list self, kso obj) {
    int idx = self->len++;
    self->items = ks_realloc(self->items, sizeof(kso) * self->len);
    self->items[idx] = obj;
    KSO_INCREF(obj);
    return idx;
}


// push objects onto the list, returning the index of the first one added
// NOTE: This adds a reference to each object
int ks_list_pushN(ks_list self, kso* objs, int len) {
    int idx = self->len;
    self->len += len;
    self->items = ks_realloc(self->items, sizeof(kso) * self->len);

    int i;
    for (i = 0; i < len; ++i) {
        self->items[idx + i] = objs[i];
        KSO_INCREF(objs[i]);
    }

    return idx;
}

// pops an object off of the list, transfering the reference to the caller
// NOTE: Call KSO_DECREF when the object reference is dead
kso ks_list_pop(ks_list self) {
    return self->items[--self->len];
}

// pops off an object from the list, with it not being used
// NOTE: This decrements the reference originally added with the push function
void ks_list_popu(ks_list self) {
    kso obj = self->items[--self->len];
    KSO_DECREF(obj);
}

// clears the list, setting it back to empty
void ks_list_clear(ks_list self) {
    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    self->len = 0;

    ks_free(self->items);
    self->items = NULL;
}



// list.__new__(val) -> constructs a new list from 'val', which in general
//   creates a list from all the elements of 'val', which can be a tuple/collection type
KS_TFUNC(list, new) {
    KS_REQ_N_ARGS(n_args, 1);
    kso val = args[0];


    if (val->type == ks_T_list) {
        return val;
    } else if (val->type == ks_T_tuple) {
        // create a new list
        return (kso)ks_list_new(((ks_tuple)val)->items, ((ks_tuple)val)->len);
    } else {
        KS_ERR_TYPECONV(val, ks_T_list);
    }
    return NULL;
}

// list.__free__(self) -> frees the list's resources & de-records references
KS_TFUNC(list, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");


    // de-record references to the objects
    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    // free our allocated buffers
    ks_free(self->items);
    ks_free(self);

    return KSO_NONE;
}

KS_TFUNC(list, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");

    if (self->len == 0) {
        return (kso)ks_str_new("[]");
    } else if (self->len == 1) {
        return (kso)ks_str_new_cfmt("[%R]", self->items[0]);
    }

    ks_strB ksb = ks_strB_create();

    ks_strB_add(&ksb, "[", 1);

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i != 0) ks_strB_add(&ksb, ", ", 2);
        ks_strB_add_repr(&ksb, self->items[i]);
    }

    ks_strB_add(&ksb, "]", 1);

    return (kso)ks_strB_finish(&ksb);
}

KS_TFUNC(list, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");

    if (self->len == 0) {
        return (kso)ks_str_new("[]");
    } else if (self->len == 1) {
        return (kso)ks_str_new_cfmt("[%R]", self->items[0]);
    }

    ks_strB ksb = ks_strB_create();

    ks_strB_add(&ksb, "[", 1);

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i != 0) ks_strB_add(&ksb, ", ", 2);
        ks_strB_add_repr(&ksb, self->items[i]);
    }

    ks_strB_add(&ksb, "]", 1);

    return (kso)ks_strB_finish(&ksb);
}

KS_TFUNC(list, hash) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");

    return kse_fmt("'list' objects are not hashable!");
}

KS_TFUNC(list, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");
    ks_int key = (ks_int)args[1];
    KS_REQ_TYPE(key, ks_T_int, "key");

    int64_t idx = key->v_int;
    if (idx < 0 || idx >= self->len) return kse_fmt("KeyError: %l (out of range)", idx);
    return KSO_NEWREF(self->items[idx]);
}

KS_TFUNC(list, setitem) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");
    ks_int key = (ks_int)args[1];
    KS_REQ_TYPE(key, ks_T_int, "key");
    kso val = args[2];

    int64_t idx = key->v_int;
    if (idx < 0 || idx >= self->len) return kse_fmt("KeyError: %l (out of range)", idx);

    kso old_val = self->items[idx];
    KSO_INCREF(val);
    KSO_INCREF(val);
    KSO_DECREF(old_val);

    return self->items[idx] = val;
}



// list.__iter__(self) -> return a list iterator object
KS_TFUNC(list, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");

    return (kso)ks_list_iter_new(self);
}


KS_TFUNC(list, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");
    kso other = args[1];

    if (other->type == ks_T_list) {
        // create a copy of the list
        ks_list ret = ks_list_new(self->items, self->len);

        // push N values
        ks_list_pushN(ret, ((ks_list)other)->items, ((ks_list)other)->len);

        return (kso)ret;

    }

    return NULL;
}





// list.__neg__(self) -> returns the list in reverse order
KS_TFUNC(list, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_T_list, "self");

    ks_list res = ks_list_new_empty();

    // process in reverse order
    int i;
    for (i = self->len - 1; i >= 0; --i) {
        ks_list_push(res, self->items[i]);
    }

    return (kso)res;
}


/* exporting functionality */

struct ks_type T_list, *ks_T_list = &T_list;

void ks_init__list() {

    /* create the type */
    T_list = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_list, "list");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_list, "__new__", "list.__new__(val)", list_new_);
    
    ADDCF(ks_T_list, "__str__", "list.__str__(self)", list_str_);
    ADDCF(ks_T_list, "__repr__", "list.__repr__(self)", list_repr_);

    ADDCF(ks_T_list, "__getitem__", "list.__getitem__(self, key)", list_getitem_);
    ADDCF(ks_T_list, "__setitem__", "list.__setitem__(self, key, val)", list_setitem_);

    ADDCF(ks_T_list, "__iter__", "list.__iter__(self)", list_iter_);

    ADDCF(ks_T_list, "__add__", "list.__add__(self, key)", list_add_);
    ADDCF(ks_T_list, "__neg__", "list.__neg__(self)", list_neg_);

    ADDCF(ks_T_list, "__free__", "list.__free__(self)", list_free_);

}




