/* types/tuple.c - implementation of a statically sized tuple
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_tuple);

// construct a tuple
ks_tuple ks_tuple_new(int len, ks_obj* elems) {
    // allocate enough memory for the elements
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + sizeof(ks_obj) * len);
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


// contruct a tuple with no new references
ks_tuple ks_tuple_new_n(int len, ks_obj* elems) {
    // allocate enough memory for the elements
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + sizeof(ks_obj) * len);
    KS_INIT_OBJ(self, ks_type_tuple);

    // initialize type-specific things
    self->len = len;

    int i;
    // and populate the array, without creating new references
    for (i = 0; i < len; ++i) {
        self->elems[i] = elems[i];
    }

    return self;
}


// Create a tuple representing a version
// NOTE: Returns a new reference
ks_tuple ks_tuple_new_version(int major, int minor, int patch) {
    return ks_tuple_new_n(3, (ks_obj[]){ 
        (ks_obj)ks_int_new(major),
        (ks_obj)ks_int_new(minor),
        (ks_obj)ks_int_new(patch),
    });
}


// tuple.__str__(self) -> convert to string
static KS_TFUNC(tuple, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_tuple self = (ks_tuple)args[0];
    KS_REQ_TYPE(self, ks_type_tuple, "self");

    // handle special cases for tuples
    if (self->len == 0) return (ks_obj)ks_str_new("(,)");
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

    // go through the buffer & delete references
    int i;
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    // no buffer free is needed because the tuple itself is allocated with its elements
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};



// tuple.__getitem__(self, idx) -> get the item in a tuple
static KS_TFUNC(tuple, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_tuple self = (ks_tuple)args[0];
    KS_REQ_TYPE(self, ks_type_tuple, "self");
    ks_int idx = (ks_int)args[1];
    KS_REQ_TYPE(idx, ks_type_int, "idx");

    int64_t idxi = idx->val;

    // ensure negative indices are wrapped once
    if (idxi < 0) idxi += self->len;

    // do bounds check
    if (idxi < 0 || idxi >= self->len) KS_ERR_KEY(self, idx);

    // return the item specified
    return KS_NEWREF(self->elems[idxi]);
};



// initialize tuple type
void ks_type_tuple_init() {
    KS_INIT_TYPE_OBJ(ks_type_tuple, "tuple");

    ks_type_set_cn(ks_type_tuple, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(tuple_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(tuple_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(tuple_free_)},
        
        {"__getitem__", (ks_obj)ks_cfunc_new2(tuple_getitem_, "tuple.__getitem__(self, idx)")},

        {NULL, NULL}   
    });
}

