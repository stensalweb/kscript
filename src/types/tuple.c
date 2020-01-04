/* types/tuple.c - the tuple of values type */

#include "ks_common.h"



// create a tuple from a list of objects
ks_tuple ks_tuple_new(kso* items, int n_items) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + n_items * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple, KSOF_NONE)
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

    ks_str built = ks_str_new("(");
    KSO_INCREF(built);

    int i;
    for (i = 0; i < self->len; ++i) {
        // valid entry
        ks_str next_built = ks_str_new_cfmt(i == 0 ? "%V%R" : "%V, %R", built, self->items[i]);
        KSO_INCREF(next_built);
        KSO_DECREF(built);
        built = next_built;
    }

    ks_str result = ks_str_new_cfmt("%V)", built);
    KSO_DECREF(built);

    return (kso)result;
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

    ks_str built = ks_str_new("(");
    KSO_INCREF(built);

    int i;
    for (i = 0; i < self->len; ++i) {
        // valid entry
        ks_str next_built = ks_str_new_cfmt(i == 0 ? "%V%R" : "%V, %R", built, self->items[i]);
        KSO_INCREF(next_built);
        KSO_DECREF(built);
        built = next_built;
    }

    ks_str result = ks_str_new_cfmt("%V)", built);
    KSO_DECREF(built);

    return (kso)result;
    #undef SIG
}


/* exporting functionality */

struct ks_type T_tuple, *ks_T_tuple = &T_tuple;

void ks_init__tuple() {

    /* first create the type */
    T_tuple = (struct ks_type) {
        KS_TYPE_INIT("tuple")

        .f_free = (kso)ks_cfunc_newref(tuple_free_),

        .f_repr = (kso)ks_cfunc_newref(tuple_repr_),
        .f_str  = (kso)ks_cfunc_newref(tuple_str_)

    };

}




