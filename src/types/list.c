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


// list.__len__(self) -> get the length
static KS_TFUNC(list, len) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");

    return (ks_obj)ks_int_new(self->len);
};



// list.__add__(L, R) -> append 2 lists
// TODO: add iterable support
static KS_TFUNC(list, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_list && R->type == ks_type_list) {
        ks_list res = ks_list_new(((ks_list)L)->len, ((ks_list)L)->elems);
        ks_list_pushn(res, ((ks_list)R)->len, ((ks_list)R)->elems);
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("+", L, R);
};

// list.__getitem__(self, idx) -> get the item in a list
static KS_TFUNC(list, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");
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

// list.push(self, obj) -> push an item to a list
static KS_TFUNC(list, push) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");
    ks_obj obj = (ks_obj)args[1];

    ks_list_push(self, obj);

    // return itself
    return KS_NEWREF(self);
};


// list.pop(self) -> return last item popped off 
static KS_TFUNC(list, pop) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");

    // ensure there were enough items
    if (self->len <= 0) ks_throw_fmt(ks_type_Error, "'%T' object is empty; nothing to pop off", self);

    // return last item
    return ks_list_pop(self);
};


// initialize list type
void ks_type_list_init() {
    KS_INIT_TYPE_OBJ(ks_type_list, "list");

    ks_type_set_cn(ks_type_list, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(list_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(list_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(list_free_)},

        {"__len__", (ks_obj)ks_cfunc_new(list_len_)},

        {"__add__", (ks_obj)ks_cfunc_new(list_add_)},

        {"push", (ks_obj)ks_cfunc_new(list_push_)},
        {"pop", (ks_obj)ks_cfunc_new(list_pop_)},

        {"__getitem__", (ks_obj)ks_cfunc_new2(list_getitem_, "list.__getitem__(self, idx)")},

        {NULL, NULL}   
    });
}

