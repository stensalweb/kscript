/* list.c - implementation of the 'list' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"



// Construct a new list from an array of elements
// NOTE: Returns new reference, or NULL if an error was thrown
ks_list ks_list_new(ks_size_t len, ks_obj* elems) {

    ks_list self = KS_ALLOC_OBJ(ks_list);
    KS_INIT_OBJ(self, ks_T_list);


    self->len = len;
    self->elems = ks_malloc(sizeof(*self->elems) * self->len);

    ks_size_t i;
    for (i = 0; i < len; ++i) {
        self->elems[i] = elems[i];
        KS_INCREF(elems[i]);
    }

    return self;
}

// Construct a list from an iterator
// NOTE: Returns new reference, or NULL if an error was thrown
ks_list ks_list_new_iter(ks_obj iter_obj) {
    ks_list self = ks_list_new(0, NULL);

    if (!ks_list_pushall(self, iter_obj)) {
        KS_DECREF(self);
        return NULL;
    }

    return self;

}
// Clear a list out, removing any references
void ks_list_clear(ks_list self) {
    ks_size_t i;

    // release references
    for (i = 0; i < self->len; ++i) KS_DECREF(self->elems[i]);

    // now, set the length
    self->len = 0;

    // TODO: decide whether we should free/realloc the buffer to save
    //   memory
    //ks_free(self->elems); self->elems = NULL;

}


// Empty out `objs` (which must be iterable!), and add everything to `self`
// NOTE: Returns success, or false and throws an error
bool ks_list_pushall(ks_list self, ks_obj objs) {
    if (objs->type == ks_T_list) {
        ks_list_pushn(self, ((ks_list)objs)->len, ((ks_list)objs)->elems);
        return true;
    } else if (objs->type == ks_T_tuple) {
        ks_list_pushn(self, ((ks_tuple)objs)->len, ((ks_tuple)objs)->elems);
        return true;
    }



    struct ks_citer cit = ks_citer_make((ks_obj)objs);
    ks_obj ob;
    while (ob = ks_citer_next(&cit)) {
        ks_list_push(self, ob);
        KS_DECREF(ob);
    }

    ks_citer_done(&cit);

    // return success
    return !cit.threwErr;
}

// Pushes 'obj' on to the end of the list
void ks_list_push(ks_list self, ks_obj obj) {
    int idx = self->len++;

    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);

    // set the new element to the given object, and hold a reference to it
    self->elems[idx] = obj;
    KS_INCREF(obj);

}

// Push 'objs' on to the end of the list
void ks_list_pushn(ks_list self, int n, ks_obj* objs) {

    /* naive
    ks_ssize_t i;
    for (i = 0; i < n; ++i) {
        ks_list_push(self, objs[i]);
    } */

    // push up to a new start
    ks_size_t new_start = self->len;
    self->len += n;

    // ensure enough space is given
    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);

    ks_size_t i;
    for (i = 0; i < n; ++i) {
        KS_INCREF(objs[i]);
        self->elems[new_start + i] = objs[i];
    }

    /*
    // starting index
    int idx = self->len;
    self->len += n;

    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);

    ks_size_t i;
    // add new elements
    for (i = 0; i < n; ++i) {
        self->elems[i + idx] = objs[i];
        KS_INCREF(objs[i]);
    }*/

}


// Pops the last element off of list and returns its reference
// NOTE: Throws 'SizeError' if the list was empty
ks_obj ks_list_pop(ks_list self) {
    if (self->len < 1) return ks_throw(ks_T_SizeError, "'list' object had no elements to pop!");
    return self->elems[--self->len];
}

// Pop off 'n' items into 'dest'
// NOTE: Returns a reference to each object, so now each object in 'dest' now has a reference
//   you must handle!
// NOTE: Returns success, or false and throws an error
bool ks_list_popn(ks_list self, int n, ks_obj* dest) {
    if (self->len < n) {
        ks_throw(ks_T_SizeError, "'list' object didn't have enough elements to pop! (requested %i, but only had %i)", (int)self->len, (int)n);
        return false;
    }


    // take off the objects
    self->len -= n;

    // now, set 'dest' to them
    memcpy(dest, &self->elems[self->len], sizeof(*dest) * n);

    return true;
}

// Pops the last element off and throw away the reference
// NOTE: Returns success, or false and throws an error
bool ks_list_popu(ks_list self) {
    if (self->len < 1) {
        ks_throw(ks_T_SizeError, "'list' object had no elements to pop!");
        return false;
    }
    ks_obj top = self->elems[--self->len];
    KS_DECREF(top);

    return true;
}


// Pops the last 'n' elements off and throw away the references
// NOTE: Returns success, or false and throws an error
bool ks_list_popun(ks_list self, int n) {
    if (self->len < n) {
        ks_throw(ks_T_SizeError, "'list' object didn't have enough elements to pop! (requested %i, but only had %i)", (int)self->len, (int)n);
        return false;
    }

    self->len -= n;
    ks_size_t i;

    for (i = self->len; i < self->len + n; ++i) {
        KS_DECREF(self->elems[i]);
    }

    return true;
}

// list.__new__(objs) - create new list
static KS_TFUNC(list, new) {
    ks_obj objs;
    KS_GETARGS("objs", &objs)

    // handle short cases
    if (objs->type == ks_T_tuple) {
        return (ks_obj)ks_list_new(((ks_tuple)objs)->len, ((ks_tuple)objs)->elems);
    } else if (objs->type == ks_T_list) {
        return (ks_obj)ks_list_new(((ks_list)objs)->len, ((ks_list)objs)->elems);
    }

    // create new list and populate it
    ks_list ret = ks_list_new(0, NULL);

    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob;
    while (ob = ks_citer_next(&cit)) {
        ks_list_push(ret, ob);
        KS_DECREF(ob);
    }

    ks_citer_done(&cit);

    // check error status
    if (cit.threwErr) {
        KS_DECREF(ret);
        return NULL;
    } else {
        return (ks_obj)ret;
    }

}


// list.__free__(self) - free object
static KS_TFUNC(list, free) {
    ks_list self;
    KS_GETARGS("self:*", &self, ks_T_list)

    ks_size_t i;

    // free references held to entries
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    // free allocated space
    ks_free(self->elems);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// list.__len__(self) - get length
static KS_TFUNC(list, len) {
    ks_list self;
    KS_GETARGS("self:*", &self, ks_T_list)

    return (ks_obj)ks_int_new(self->len);
}

// list.__str__(self) - to string
static KS_TFUNC(list, str) {
    ks_list self;
    KS_GETARGS("self:*", &self, ks_T_list)

    ks_size_t i;

    ks_str_builder sb = ks_str_builder_new();

    ks_str_builder_add(sb, "[", 1);

    // free references held to entries
    for (i = 0; i < self->len; ++i) {
        if (i > 0) ks_str_builder_add(sb, ", ", 2);
        ks_str_builder_add_repr(sb, self->elems[i]);
    }

    ks_str_builder_add(sb, "]", 1);
    
    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);

    return (ks_obj)ret;
}

// list.push(self, obj) - push an object
static KS_TFUNC(list, push) {
    ks_list self;
    ks_obj obj;
    KS_GETARGS("self:* obj", &self, ks_T_list, &obj)

    ks_list_push(self, obj);
    return KSO_NONE;
}

// list.pop(self) - pop off an object
static KS_TFUNC(list, pop) {
    ks_list self;
    KS_GETARGS("self:*", &self, ks_T_list)

    // bounds check
    if (self->len < 1) return ks_throw(ks_T_SizeError, "Failed to pop an item from list; list was empty");

    return ks_list_pop(self);
}

// list.__getitem__(self, idx) - get the item in a list
static KS_TFUNC(list, getitem) {
    ks_list self;
    ks_obj idx;
    KS_GETARGS("self:* idx", &self, ks_T_list, &idx)

    int64_t v64;
    if (ks_num_get_int64(idx, &v64)) {

        // ensure negative indices are wrapped once
        if (v64 < 0) v64 += self->len;

        // do bounds check
        if (v64 < 0 || v64 >= self->len) KS_THROW_INDEX_ERR(self, idx);

        // return the item specified
        return KS_NEWREF(self->elems[v64]);
    } else {
        ks_catch_ignore();
    }

    if (idx->type == ks_T_slice) {
        ks_slice slice_idx = (ks_slice)idx;

        int64_t first, last, delta;
        // convert to C for loop parameters
        if (!ks_slice_getci((ks_slice)idx, self->len, &first, &last, &delta)) return NULL;

        int64_t i;
        ks_list res = ks_list_new(0, NULL);

        // add appropriate elements
        for (i = first; i != last; i += delta) {
            ks_list_push(res, self->elems[i]);
        }

        return (ks_obj)res;

    } else {
        return ks_throw(ks_T_TypeError, "Expected 'idx' to be an integer, or a slice, but got '%T'", idx);
    }
}

// list.__setitem__(self, idx, val) - set items in list
static KS_TFUNC(list, setitem) {
    ks_list self;
    ks_obj idx, val;
    KS_GETARGS("self:* idx val", &self, ks_T_list, &idx, &val)

    int64_t v64;
    if (ks_num_get_int64(idx, &v64)) {

        // ensure negative indices are wrapped once
        if (v64 < 0) v64 += self->len;

        // do bounds check
        if (v64 < 0 || v64 >= self->len) KS_THROW_INDEX_ERR(self, idx);


        // swap it out
        KS_INCREF(val);
        KS_DECREF(self->elems[v64]);
        self->elems[v64] = val;

        // return new reference
        KS_INCREF(val);
        return val;

    } else {
        ks_catch_ignore();
    }

    if (idx->type == ks_T_slice) {
        ks_slice slice_idx = (ks_slice)idx;

        // integer variables
        int64_t first, last, delta, i;

        // convert to C for loop parameters
        if (!ks_slice_getci((ks_slice)idx, self->len, &first, &last, &delta)) return NULL;

        if (ks_obj_is_iterable(val)) {
            // strided assign
            struct ks_citer cit = ks_citer_make(val);
            ks_obj ob = NULL;

            i = first;
            int64_t ct = 0;

            while ((ob = ks_citer_next(&cit)) != NULL && (i != last)) {
                // strided assign

                // swap out at this index
                KS_INCREF(ob);
                KS_DECREF(self->elems[i]);
                self->elems[i] = ob;
                ct++;

                // prepare for next loop
                i += delta;
                KS_DECREF(ob);
            }
            ks_citer_done(&cit);
        
            // check for wrong sequence length
            if (cit.threwErr) {
                return NULL;
            } else if (i != last) {
                return ks_throw(ks_T_SizeError, "Assigning to list via slice failed; the iterator had too few values! (expected %l, and it only had %l)", (last - first) / delta, ct);
            } else if (!cit.done) {
                return ks_throw(ks_T_SizeError, "Assigning to list via slice failed; the iterator had too many values! (expected %l)", (last - first) / delta);
            }


            return KSO_NONE;
        } else {
            // copied assign
            // add appropriate elements
            for (i = first; i != last; i += delta) {
                KS_INCREF(val);
                KS_DECREF(self->elems[i]);
                self->elems[i] = val;
            }

            return KSO_NONE;
        }

    } else {
        return ks_throw(ks_T_TypeError, "Expected 'idx' to be an integer, or a slice, but got '%T'", idx);
    }
}

// list.__eq__(L, R) - check if all elements are equal
static KS_TFUNC(list, eq) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    if (L->type == ks_T_list && R->type == ks_T_list) {
        
        ks_list lL = (ks_list)L, lR = (ks_list)R;
        if (lL->len != lR->len) return KSO_FALSE;

        int i;
        for (i = 0; i < lL->len; ++i) {
            ks_obj lreq = ks_F_eq->func(2, (ks_obj[]){ lL->elems[i], lR->elems[i] });
            if (!lreq) return NULL;
            int truthy = ks_obj_truthy(lreq);
            KS_DECREF(lreq);
            if (truthy < 0) return NULL;
            if (!truthy) return KSO_FALSE;

        }

        // all were equal
        return KSO_TRUE;

    }

    KS_THROW_BOP_ERR("==", L, R);
}


// list.__ne__(L, R) - check if any elements differ
static KS_TFUNC(list, ne) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    if (L->type == ks_T_list && R->type == ks_T_list) {
        
        ks_list lL = (ks_list)L, lR = (ks_list)R;
        if (lL->len != lR->len) return KSO_TRUE;

        int i;
        for (i = 0; i < lL->len; ++i) {
            ks_obj lreq = ks_F_eq->func(2, (ks_obj[]){ lL->elems[i], lR->elems[i] });
            if (!lreq) return NULL;
            int truthy = ks_obj_truthy(lreq);
            KS_DECREF(lreq);
            if (truthy < 0) return NULL;
            if (!truthy) return KSO_TRUE;

        }

        // all were equal
        return KSO_FALSE;

    }

    KS_THROW_BOP_ERR("==", L, R);
}

// list.__add__(L, R)
static KS_TFUNC(list, add) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    if (ks_obj_is_iterable(L) && ks_obj_is_iterable(R)) {

        // result list
        ks_list res = ks_list_new(0, NULL);

        // try adding them
        if (!ks_list_pushall(res, L)) {
            KS_DECREF(res);
            return NULL;
        }
        if (!ks_list_pushall(res, R)) {
            KS_DECREF(res);
            return NULL;
        }

        return (ks_obj)res;

    }

    KS_THROW_BOP_ERR("+", L, R);
}

// list.__mul__(L, R)
static KS_TFUNC(list, mul) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);


    if (ks_obj_is_iterable(L) && ks_num_is_integral(R)) {
        
        // get how many times
        int64_t times;
        if (!ks_num_get_int64(R, &times)) {
            return NULL;
        }

        // result list
        ks_list res = ks_list_new(0, NULL);


        // now, add copies
        ks_size_t i;
        for (i = 0; i < times; ++i) {
            // try adding them
            if (!ks_list_pushall(res, L)) {
                KS_DECREF(res);
                return NULL;
            }
        }

        return (ks_obj)res;

    } else if (ks_num_is_integral(L) && ks_num_is_numeric(R)) {
        
        // get how many times
        int64_t times;
        if (!ks_num_get_int64(L, &times)) {
            return NULL;
        }

        // result list
        ks_list res = ks_list_new(0, NULL);


        // now, add copies
        ks_size_t i;
        for (i = 0; i < times; ++i) {
            // try adding them
            if (!ks_list_pushall(res, R)) {
                KS_DECREF(res);
                return NULL;
            }
        }

        return (ks_obj)res;
    }

    KS_THROW_BOP_ERR("*", L, R);
}



/* iterator type */

// ks_list_iter - list iterable type
typedef struct {
    KS_OBJ_BASE

    // the list it is iterating over
    ks_list self;

    // current position in the array
    ks_ssize_t pos;


}* ks_list_iter;

// declare type
KS_TYPE_DECLFWD(ks_T_list_iter);

// list_iter.__free__(self) - free obj
static KS_TFUNC(list_iter, free) {
    ks_list_iter self;
    KS_GETARGS("self:*", &self, ks_T_list_iter)

    // remove reference to string
    KS_DECREF(self->self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// list_iter.__next__(self) - return next character
static KS_TFUNC(list_iter, next) {
    ks_list_iter self;
    KS_GETARGS("self:*", &self, ks_T_list_iter)
    
    // check if the iterator is done
    if (self->pos >= self->self->len) return ks_throw(ks_T_OutOfIterError, "");


    // get next element
    ks_obj ret = self->self->elems[self->pos++];
    return KS_NEWREF(ret);
}

// list.__iter__(self) - return iterator
static KS_TFUNC(list, iter) {
    ks_list self;
    KS_GETARGS("self:*", &self, ks_T_list)

    ks_list_iter ret = KS_ALLOC_OBJ(ks_list_iter);
    KS_INIT_OBJ(ret, ks_T_list_iter);

    ret->self = self;
    KS_INCREF(self);

    // position at start
    ret->pos = 0;

    return (ks_obj)ret;
}



/* export */

KS_TYPE_DECLFWD(ks_T_list);

void ks_init_T_list() {
    ks_type_init_c(ks_T_list, "list", ks_T_object, KS_KEYVALS(
        {"__new__",               (ks_obj)ks_cfunc_new_c_old(list_new_, "list.__new__(objs)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(list_free_, "list.__free__(self)")},
        {"__len__",                (ks_obj)ks_cfunc_new_c_old(list_len_, "list.__len__(self)")},
        
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(list_str_, "list.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(list_str_, "list.__repr__(self)")},

        {"__iter__",               (ks_obj)ks_cfunc_new_c_old(list_iter_, "list.__iter__(self)")},

        {"__getitem__",            (ks_obj)ks_cfunc_new_c_old(list_getitem_, "list.__getitem__(self, idx)")},
        {"__setitem__",            (ks_obj)ks_cfunc_new_c_old(list_setitem_, "list.__setitem__(self, idx, val)")},

        {"__add__",                (ks_obj)ks_cfunc_new_c_old(list_add_, "list.__add__(L, R)")},
        {"__mul__",                (ks_obj)ks_cfunc_new_c_old(list_mul_, "list.__mul__(L, R)")},

        {"__eq__",                 (ks_obj)ks_cfunc_new_c_old(list_eq_, "list.__eq__(L, R)")},
        {"__ne__",                 (ks_obj)ks_cfunc_new_c_old(list_ne_, "list.__ne__(L, R)")},

        {"push",                   (ks_obj)ks_cfunc_new_c_old(list_push_, "list.push(self, obj)")},
        {"pop",                    (ks_obj)ks_cfunc_new_c_old(list_pop_, "list.pop(self)")},

    ));
    ks_type_init_c(ks_T_list_iter, "list_iter", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(list_iter_free_, "list_iter.__free__(self)")},

        {"__next__",               (ks_obj)ks_cfunc_new_c_old(list_iter_next_, "list_iter.__next__(self)")},
    ));
}


