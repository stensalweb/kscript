/* types/list.c - implementation of a resizable list for kscript
/* types/list.c - implementation of a resizable list for kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_list);

// create a kscript int from a C-style int
ks_list ks_list_new(int len, ks_obj* elems) {
    ks_list self = KS_ALLOC_OBJ(ks_list);
    KS_INIT_OBJ(self, ks_type_list);

    // initialize type-specific things
    self->len = len;
    self->elems = ks_malloc(sizeof(*self->elems) * len);

    int i;
    // and populate the array
    for (i = 0; i < len; ++i) {
        self->elems[i] = KS_NEWREF(elems[i]);
    }

    return self;
}
void ks_list_clear(ks_list self) {
    // decref all elements and remove length
    while (self->len > 0) {
        KS_DECREF(self->elems[self->len - 1]);
        self->len--;
    }
}

// push on an object to the list (recording a reference)
void ks_list_push(ks_list self, ks_obj obj) {
    ks_size_t idx = self->len++;

    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);
    self->elems[idx] = KS_NEWREF(obj);

}

// Push 'n' objects on to the end of the list, expanding the list
void ks_list_pushn(ks_list self, int n, ks_obj* objs) {
    ks_size_t idx = self->len;
    self->len += n;

    // expand
    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);

    // push new references
    int i;
    for (i = 0; i < n; ++i) self->elems[idx + i] = KS_NEWREF(objs[i]);
}

// pop off an object
ks_obj ks_list_pop(ks_list self) {
    return self->elems[--self->len];
}

void ks_list_popn(ks_list self, int n, ks_obj* dest) {
    self->len -= n;
    memcpy(dest, &self->elems[self->len], n * sizeof(ks_obj));
}

// pop off an object, without using the result, destroying the reference
void ks_list_popu(ks_list self) {
    ks_obj top = self->elems[--self->len];
    KS_DECREF(top);
}


// list.__str__(self) -> convert to string
static KS_TFUNC(list, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");
    
    assert(self->type == ks_type_list);

    ks_str_b SB;
    ks_str_b_init(&SB);

    ks_str_b_add(&SB, 1, "[");

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i > 0 && i < self->len) ks_str_b_add(&SB, 2, ", ");

        // add the item
        ks_str_b_add_repr(&SB, self->elems[i]);
    }

    ks_str_b_add(&SB, 1, "]");

    ks_str ret = ks_str_b_get(&SB);

    ks_str_b_free(&SB);

    return (ks_obj)ret;
};

// list.__free__(self) -> frees resources
static KS_TFUNC(list, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");

    // go through the buffer & delete references to elements
    int i;
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    ks_free(self->elems);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};



// initialize list type
void ks_type_list_init() {
    KS_INIT_TYPE_OBJ(ks_type_list, "list");

    ks_type_set_cn(ks_type_list, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(list_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(list_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(list_free_)},
        {NULL, NULL}   
    });
}

