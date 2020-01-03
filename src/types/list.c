/* types/tuple.c - the tuple of values type */

#include "ks_common.h"



// create a list from a list of objects
ks_list ks_list_new(kso* items, int n_items) {
    ks_list self = (ks_list)ks_malloc(sizeof(*self));
    *self = (struct ks_list) {
        KSO_BASE_INIT(ks_T_list, KSOF_NONE)
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

// create a new empty tuple
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



TFUNC(list, free) {
    #define SIG "list.__free__(self)"
    REQ_N_ARGS(1);
    ks_list self = (ks_list)args[0];
    REQ_TYPE("self", self, ks_T_list);


    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    ks_free(self->items);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}


/* exporting functionality */

struct ks_type T_list, *ks_T_list = &T_list;

void ks_init__list() {

    /* first create the type */
    T_list = (struct ks_type) {
        KS_TYPE_INIT("list")

        .f_free = (kso)ks_cfunc_newref(list_free_)

    };

}




