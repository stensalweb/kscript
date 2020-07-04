/* types/list.c - implementation of a resizable list for kscript
/* types/list.c - implementation of a resizable list for kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_list);
KS_TYPE_DECLFWD(ks_type_list_iter);

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

// create a list by exhausting an iterable
ks_list ks_list_from_iterable(ks_obj obj) {

    // some shortcuts
    /**/ if (obj->type == ks_type_list)  return (ks_list)KS_NEWREF(obj);
    else if (obj->type == ks_type_tuple) return ks_list_new(((ks_tuple)obj)->len, ((ks_tuple)obj)->elems);

    ks_obj iter_obj = ks_F_iter->func(1, &obj);
    if (!iter_obj) return NULL;

    ks_list res = ks_list_new(0, NULL);

    while (true) {
        ks_obj cur = ks_F_next->func(1, &iter_obj);

        if (!cur) {

            // error occured
            if (ks_thread_get()->exc->type == ks_type_OutOfIterError) {
                // signals the stop of the iterator, so just return at this point
                ks_catch_ignore();
                break;
            } else {
                // unrelated error; stop
                KS_DECREF(iter_obj);
                KS_DECREF(res);
                return NULL;
            }
        }

        ks_list_push(res, cur);
        KS_DECREF(cur);
    }



    KS_DECREF(iter_obj);
    return res;

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



// list.__new__(obj) -> create list from iterable
static KS_TFUNC(list, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 1);
    if (n_args == 0) return (ks_obj)ks_list_new(0, NULL);
    else return (ks_obj)ks_list_from_iterable(args[0]);
};


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



// list.__mul__(L, R) -> multiply a list by an integer constant
static KS_TFUNC(list, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_list && ks_num_is_integral(R)) {
        ks_list lL = (ks_list)L;
        int64_t iR;
        if (!ks_num_get_int64(R, &iR)) return NULL;

        ks_list res = ks_list_new(0, NULL);
        int i;
        for (i = 0; i < (iR); ++i) {
            ks_list_pushn(res, lL->len, lL->elems);
        }

        return (ks_obj)res;
    } else if (R->type == ks_type_list && ks_num_is_integral(L)) {
        ks_list lR = (ks_list)R;
        int64_t iL;
        if (!ks_num_get_int64(L, &iL)) return NULL;

        ks_list res = ks_list_new(0, NULL);
        int i;
        for (i = 0; i < (iL); ++i) {
            ks_list_pushn(res, lR->len, lR->elems);
        }

        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("*", L, R);
};




// list.__eq__(L, R) -> check if everything is equal
static KS_TFUNC(list, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_list && R->type == ks_type_list) {
        
        ks_list lL = (ks_list)L, lR = (ks_list)R;
        if (lL->len != lR->len) return KSO_FALSE;

        int i;
        for (i = 0; i < lL->len; ++i) {
            ks_obj lreq = ks_F_eq->func(2, (ks_obj[]){ lL->elems[i], lR->elems[i] });
            if (!lreq) return NULL;
            int truthy = ks_truthy(lreq);
            KS_DECREF(lreq);
            if (!truthy) return KSO_FALSE;

        }

        // all were equal
        return KSO_TRUE;

    }

    KS_ERR_BOP_UNDEF("==", L, R);
};



// list.__ne__(L, R) -> check if anything is different
static KS_TFUNC(list, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_list && R->type == ks_type_list) {
        
        ks_list lL = (ks_list)L, lR = (ks_list)R;
        if (lL->len != lR->len) return KSO_TRUE;

        int i;
        for (i = 0; i < lL->len; ++i) {
            ks_obj lreq = ks_F_eq->func(2, (ks_obj[]){ lL->elems[i], lR->elems[i] });
            if (!lreq) return NULL;
            int truthy = ks_truthy(lreq);
            KS_DECREF(lreq);
            if (!truthy) return KSO_TRUE;

        }

        // all were equal
        return KSO_FALSE;
    }

    KS_ERR_BOP_UNDEF("!=", L, R);
};




// adjust index to within bounds
static int64_t my_adj(int64_t idx, int64_t len) {
    if (idx < 0) {
        return ((idx % len) + len) % len;
    } else {
        return idx;
    }
}

// list.__getitem__(self, idx) -> get the item in a list
static KS_TFUNC(list, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_list self = NULL;
    ks_obj idx;
    if (!ks_parse_params(n_args, args, "self%* idx%any", &self, ks_type_list, &idx)) return NULL;

    int64_t v64;
    if (ks_num_get_int64(idx, &v64)) {

        // ensure negative indices are wrapped once
        if (v64 < 0) v64 += self->len;

        // do bounds check
        if (v64 < 0 || v64 >= self->len) KS_ERR_KEY(self, args[1]);

        // return the item specified
        return KS_NEWREF(self->elems[v64]);
    } else {
        ks_catch_ignore();
    }

    if (idx->type == ks_type_slice) {
        ks_slice slice_idx = (ks_slice)idx;

        int64_t first, last, delta;
        if (!ks_slice_getci((ks_slice)idx, self->len, &first, &last, &delta)) return NULL;

        int64_t i;
        ks_list res = ks_list_new(0, NULL);

        // add appropriate elements
        for (i = first; i != last; i += delta) {
            ks_list_push(res, self->elems[i]);
        }

        return (ks_obj)res;

    } else {
        return ks_throw_fmt(ks_type_TypeError, "Expected 'idx' to be an integer, or a slice, but got '%T'", args[1]);
    }
}


// list.__setitem__(self, idx, val) -> set an item in the list
static KS_TFUNC(list, setitem) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_list self = NULL;
    int64_t idx = 0;
    ks_obj val;
    if (!ks_parse_params(n_args, args, "self%* idx%i64 val%any", &self, ks_type_list, &idx, &val)) return NULL;

    // ensure negative indices are wrapped once
    if (idx < 0) idx += self->len;

    // do bounds check
    if (idx < 0 || idx >= self->len) KS_ERR_KEY(self, args[1]);

    // remove old reference
    KS_DECREF(self->elems[idx]);
    self->elems[idx] = KS_NEWREF(val);

    // return the item specified
    return KS_NEWREF(self->elems[idx]);
};


// list.__iter__(self) -> return an iterator
static KS_TFUNC(list, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list self = (ks_list)args[0];
    KS_REQ_TYPE(self, ks_type_list, "self");

    return (ks_obj)ks_list_iter_new(self);
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


/* iterator */


ks_list_iter ks_list_iter_new(ks_list obj) {
    ks_list_iter self = KS_ALLOC_OBJ(ks_list_iter);
    KS_INIT_OBJ(self, ks_type_list_iter);

    // initialize type-specific things
    self->pos = 0;
    self->obj = (ks_list)KS_NEWREF(obj);

    return self;
}

// list_iter.__free__(self) -> free resources
static KS_TFUNC(list_iter, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list_iter self = (ks_list_iter)args[0];
    KS_REQ_TYPE(self, ks_type_list_iter, "self");

    // release reference to the list
    KS_DECREF(self->obj);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);


    return KSO_NONE;
}

// list_iter.__next__(self) -> return next item in the list, or raise a OutOfIterError if it is exhausted
static KS_TFUNC(list_iter, next) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_list_iter self = (ks_list_iter)args[0];
    KS_REQ_TYPE(self, ks_type_list_iter, "self");

    if (self->pos >= self->obj->len) {
        return ks_throw_fmt(ks_type_OutOfIterError, "");
    } else {
        // get next object
        ks_obj ret = self->obj->elems[self->pos++];
        return KS_NEWREF(ret);
    }

}


// initialize list type
void ks_type_list_init() {
    KS_INIT_TYPE_OBJ(ks_type_list, "list");

    ks_type_set_cn(ks_type_list, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(list_new_, "list.__new__(obj)")},

        {"__str__", (ks_obj)ks_cfunc_new2(list_str_, "list.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(list_str_, "list.__repr__(self)")},
        {"__free__", (ks_obj)ks_cfunc_new2(list_free_, "list.__free__(self)")},

        {"__len__", (ks_obj)ks_cfunc_new2(list_len_, "list.__len__(self)")},

        {"__add__", (ks_obj)ks_cfunc_new2(list_add_, "list.__add__(L, R)")},
        {"__mul__", (ks_obj)ks_cfunc_new2(list_mul_, "list.__mul__(L, R)")},

        {"__eq__", (ks_obj)ks_cfunc_new2(list_eq_, "list.__eq__(L, R)")},
        {"__ne__", (ks_obj)ks_cfunc_new2(list_ne_, "list.__ne__(L, R)")},

        {"__iter__", (ks_obj)ks_cfunc_new2(list_iter_, "list.__iter__(self)")},

        {"push", (ks_obj)ks_cfunc_new2(list_push_, "list.push(self, obj)")},
        {"pop", (ks_obj)ks_cfunc_new2(list_pop_, "list.pop(self)")},

        {"__getitem__", (ks_obj)ks_cfunc_new2(list_getitem_, "list.__getitem__(self, idx)")},
        {"__setitem__", (ks_obj)ks_cfunc_new2(list_setitem_, "list.__setitem__(self, idx, val)")},

        {NULL, NULL}   
    });


    KS_INIT_TYPE_OBJ(ks_type_list_iter, "list_iter");

    ks_type_set_cn(ks_type_list_iter, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new2(list_iter_free_, "list_iter.__free__(self)")},
        {"__next__", (ks_obj)ks_cfunc_new2(list_iter_next_, "list_iter.__next__(self)")},

        {NULL, NULL}   
    });
}

