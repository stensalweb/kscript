/* types/tuple.c - implementation of a statically sized tuple
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_tuple);

// create a kscript int from a C-style int
ks_tuple ks_new_tuple(int len, ks_obj* elems) {
    // allocate enough memory for the elements
    ks_tuple self = (ks_tuple)malloc(sizeof(*self) + sizeof(ks_obj) * len);
    KS_INIT_OBJ(self, ks_type_tuple);

    // initialize type-specific things
    self->len = len;

    int i;
    // and populate the array
    for (i = 0; i < len; ++i) {
        self->elems[i] = KS_NEWREF(elems[i]);
    }

    return self;
}


// free a kscript list
void ks_free_tuple(ks_tuple self) {

    // go through the buffers & delete references
    int i;
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
}

// tuple.__str__(self) -> convert to string
static KS_TFUNC(tuple, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_tuple self = (ks_tuple)args[0];
    KS_REQ_TYPE(self, ks_type_tuple, "self");

    // handle special cases for tuples
    if (self->len == 0) return (ks_obj)ks_new_str("(,)");
    if (self->len == 1) return (ks_obj)ks_fmt_c("(%R,)", self->elems[0]);
    
    ks_str_b SB;
    ks_str_b_init(&SB);

    ks_str_b_add(&SB, 1, "(");

    int i;
    for (i = 0; i < self->len; ++i) {
        if (i > 0 && i < self->len) ks_str_b_add(&SB, 2, ", ");

        // add the item
        ks_str_b_add_repr(&SB, self->elems[i]);
    }

    ks_str_b_add(&SB, 1, ")");

    ks_str ret = ks_str_b_get(&SB);

    ks_str_b_free(&SB);

    return (ks_obj)ret;
};

// tuple.__free__(self) -> frees resources
static KS_TFUNC(tuple, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_tuple self = (ks_tuple)args[0];
    KS_REQ_TYPE(self, ks_type_tuple, "self");

    ks_free_tuple(self);

    return KSO_NONE;
};



// initialize tuple type
void ks_type_tuple_init() {
    KS_INIT_TYPE_OBJ(ks_type_tuple, "tuple");

    ks_type_set_cn(ks_type_tuple, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_new_cfunc(tuple_str_)},
        {"__free__", (ks_obj)ks_new_cfunc(tuple_free_)},
        {NULL, NULL}   
    });
}

