/* types/list.c - implementation of a resizable list for kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_list);

// create a kscript int from a C-style int
ks_list ks_new_list(int len, ks_obj* elems) {
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


// free a kscript list
void ks_free_list(ks_list self) {

    // go through the buffers & delete references
    int i;
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    ks_free(self->elems);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
}

// push on an object to the list (recording a reference)
void ks_list_push(ks_list self, ks_obj obj) {
    ks_size_t idx = self->len++;

    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);
    self->elems[idx] = KS_NEWREF(obj);

}


// pop off an object
ks_obj ks_list_pop(ks_list self) {
    return self->elems[--self->len];
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

    ks_str_builder SB;
    ks_str_builder_init(&SB);

    ks_str_builder_add(&SB, 1, "[");

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i > 0 && i < self->len) ks_str_builder_add(&SB, 2, ", ");

        // add the item
        ks_str_builder_add_repr(&SB, self->elems[i]);
    }

    ks_str_builder_add(&SB, 1, "]");

    ks_str ret = ks_str_builder_get(&SB);

    ks_str_builder_free(&SB);

    return (ks_obj)ret;
};


// initialize list type
void ks_type_list_init() {
    KS_INIT_TYPE_OBJ(ks_type_list, "list");

    ks_type_set_cn(ks_type_list, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_new_cfunc(list_str_)},
        {"__repr__", (ks_obj)ks_new_cfunc(list_str_)},
        {NULL, NULL}   
    });
}

