/* types/tuple.c - the tuple of values type */

#include "ks_common.h"



// create a tuple from a list of objects
ks_tuple ks_tuple_new(kso* items, int n_items) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + n_items * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple)
        .len = n_items
    };

    int i;
    for (i = 0; i < n_items; ++i) {
        self->items[i] = items[i];
        // record this tuple's reference to it
        KSO_INCREF(items[i]);
    }
    return self;
}

// creates a tuple, without creating new references to the objects
ks_tuple ks_tuple_new_norefs(kso* items, int n_items) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + n_items * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple)
        .len = n_items
    };

    int i;
    for (i = 0; i < n_items; ++i) {
        self->items[i] = items[i];
        // don't record references
    }
    return self;

}

// create a new empty tuple
ks_tuple ks_tuple_new_empty() {
    return ks_tuple_new(NULL, 0);
}


TFUNC(tuple, free) {
    #define SIG "tuple.__free__(self)"
    REQ_N_ARGS(1);
    ks_tuple self = (ks_tuple)args[0];
    REQ_TYPE("self", self, ks_T_tuple);

    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}

TFUNC(tuple, repr) {
    #define SIG "tuple.__repr__(self)"
    REQ_N_ARGS(1);
    ks_tuple self = (ks_tuple)args[0];
    REQ_TYPE("self", self, ks_T_tuple);

    if (self->len == 0) {
        return (kso)ks_str_new("(,)");
    } else if (self->len == 1) {
        return (kso)ks_str_new_cfmt("(%R,)", self->items[0]);
    }

    ks_strB ksb = ks_strB_create();

    ks_strB_add(&ksb, "(", 1);

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i != 0) ks_strB_add(&ksb, ", ", 2);
        ks_strB_add_repr(&ksb, self->items[i]);
    }

    ks_strB_add(&ksb, ")", 1);

    return (kso)ks_strB_finish(&ksb);
    #undef SIG
}

TFUNC(tuple, str) {
    #define SIG "tuple.__str__(self)"
    REQ_N_ARGS(1);
    ks_tuple self = (ks_tuple)args[0];
    REQ_TYPE("self", self, ks_T_tuple);

    if (self->len == 0) {
        return (kso)ks_str_new("(,)");
    } else if (self->len == 1) {
        return (kso)ks_str_new_cfmt("(%R,)", self->items[0]);
    }

    ks_strB ksb = ks_strB_create();

    ks_strB_add(&ksb, "(", 1);

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i != 0) ks_strB_add(&ksb, ", ", 2);
        ks_strB_add_repr(&ksb, self->items[i]);
    }

    ks_strB_add(&ksb, ")", 1);

    return (kso)ks_strB_finish(&ksb);
    #undef SIG
}


TFUNC(tuple, getitem) {
    #define SIG "tuple.__str__(self)"
    REQ_N_ARGS(2);
    ks_tuple self = (ks_tuple)args[0];
    REQ_TYPE("self", self, ks_T_tuple);
    ks_int idx = (ks_int)args[1];
    REQ_TYPE("idx", idx, ks_T_int);

    if (idx->v_int < 0 || idx->v_int >= self->len) return kse_fmt("KeyError: %R (out of range)", idx);

    // return the value
    return KSO_NEWREF(self->items[idx->v_int]);
    #undef SIG
}


/* exporting functionality */

struct ks_type T_tuple, *ks_T_tuple = &T_tuple;

void ks_init__tuple() {

    /* create the type */
    T_tuple = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_tuple, "tuple");

    // add cfuncs
    #define ADDCF(_type, _name, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_tuple, "__str__", tuple_str_);
    ADDCF(ks_T_tuple, "__repr__", tuple_repr_);

    ADDCF(ks_T_tuple, "__getitem__", tuple_getitem_);

    ADDCF(ks_T_tuple, "__free__", tuple_free_);

}




