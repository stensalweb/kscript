/* tuple.c - implementation of the 'tuple' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// construct a tuple
ks_tuple ks_tuple_new(int len, ks_obj* elems) {
    // allocate enough memory for the elements
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + sizeof(ks_obj) * len);
    KS_INIT_OBJ(self, ks_T_tuple);

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
    KS_INIT_OBJ(self, ks_T_tuple);

    // initialize type-specific things
    self->len = len;

    int i;
    if (elems != NULL) {
        // and populate the array, without creating new references
        for (i = 0; i < len; ++i) {
            self->elems[i] = elems[i];
        }

    } else {
        // and populate the array, without creating new references
        for (i = 0; i < len; ++i) {
            self->elems[i] = NULL;
        }
    }

    return self;
}

// Build a tuple from C-style variables
// For example, `ks_build_tuple("%i %f", 2, 3.0)` returns (2, 3.0)
// NOTE: Returns a new reference, or NULL if there was an error thrown
ks_tuple ks_build_tuple(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    ks_list b_list = ks_list_new(0, NULL);

    // specifiers
    char spec[256];

    // and field name
    char field[256];

    int i = 0;
    while (fmt[i]) {
        while (fmt[i] == ' ') i++;

        if (fmt[i] == '%') {
            i++;
            int spec_i = i;
            // have format specifier
            while (fmt[i] && !isalpha(fmt[i])) {
                spec[i - spec_i] = fmt[i];
                i++;
            }

            spec[i - spec_i] = '\0';

            int field_i = i;

            // have format field
            while (fmt[i] && fmt[i] != ' ') {
                field[i - field_i] = fmt[i];
                i++;
            }

            field[i - field_i] = '\0';


            if (strcmp(field, "i") == 0) {
                // parse integer
                int val = va_arg(ap, int);

                ks_int new_obj = ks_int_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);
            } else if (strcmp(field, "f") == 0) {
                // parse integer
                double val = va_arg(ap, double);

                ks_float new_obj = ks_float_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);

            } else if (strcmp(field, "z") == 0) {
                // %z - ks_ssize_t
                // %+z - int len, ks_ssize_t*

                // whether or not to do an array
                bool doMult = strchr(spec, '+') != NULL;

                if (doMult) {

                    int num = va_arg(ap, int);
                    ks_ssize_t* vals = va_arg(ap, ks_ssize_t*);

                    int i;
                    for (i = 0; i < num; ++i) {

                        ks_int new_obj = ks_int_new(vals[i]);
                        ks_list_push(b_list, (ks_obj)new_obj);
                        KS_DECREF(new_obj);
                    }

                } else {
                    ks_ssize_t val = va_arg(ap, ks_ssize_t);

                    ks_int new_obj = ks_int_new(val);
                    ks_list_push(b_list, (ks_obj)new_obj);
                    KS_DECREF(new_obj);

                }
            } else if (strcmp(field, "s") == 0) {
                // parse string
                char* val = va_arg(ap, char*);

                ks_str new_obj = ks_str_new(val);
                ks_list_push(b_list, (ks_obj)new_obj);
                KS_DECREF(new_obj);

            } else {
                ks_error("ks", "Unknown format specifier in `ks_build_tuple`, got '%%%s'", field);
                exit(1);
            }
        }
    }

    va_end(ap);

    // convert to tuple and return
    ks_tuple res = ks_tuple_new(b_list->len, b_list->elems);
    KS_DECREF(b_list);
    return res;
}



// tuple.__new__(objs) - create new tuple
static KS_TFUNC(tuple, new) {
    ks_obj objs;
    KS_GETARGS("objs", &objs)

    // handle short cases
    if (objs->type == ks_T_tuple) {
        return KS_NEWREF(objs);
    } else if (objs->type == ks_T_list) {
        return (ks_obj)ks_tuple_new(((ks_list)objs)->len, ((ks_list)objs)->elems);
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
        ks_tuple ret_tuple = ks_tuple_new(ret->len, ret->elems);
        KS_DECREF(ret);
        return (ks_obj)ret_tuple;
    }
}

// tuple.__free__(self) - free object
static KS_TFUNC(tuple, free) {
    ks_tuple self;
    KS_GETARGS("self:*", &self, ks_T_tuple)

    ks_size_t i;

    // free references held to entries
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// tuple.__len__(self) - get length
static KS_TFUNC(tuple, len) {
    ks_tuple self;
    KS_GETARGS("self:*", &self, ks_T_tuple)

    return (ks_obj)ks_int_new(self->len);
}



// tuple.__hash__(self) -> return hash
static KS_TFUNC(tuple, hash) {
    ks_tuple self;
    KS_GETARGS("self:*", &self, ks_T_tuple)

    ks_hash_t hash = 0;

    int i;
    for (i = 0; i < self->len; ++i) {
        ks_hash_t chash;
        if (!ks_obj_hash(self->elems[i], &chash)) {
            return NULL;
        }

        hash = (hash ^ chash) * KS_HASH_MUL + KS_HASH_ADD;
    }

    return (ks_obj)ks_int_new(hash);
}


// tuple.__str__(self) - to string
static KS_TFUNC(tuple, str) {
    ks_tuple self;
    KS_GETARGS("self:*", &self, ks_T_tuple)

    // special cases for tuples
    /**/ if (self->len == 0) return (ks_obj)ks_str_new("(,)");
    else if (self->len == 1) return (ks_obj)ks_fmt_c("(%R,)", self->elems[0]);

    ks_size_t i;

    ks_str_builder sb = ks_str_builder_new();

    ks_str_builder_add(sb, "(", 1);

    // free references held to entries
    for (i = 0; i < self->len; ++i) {
        if (i > 0) ks_str_builder_add(sb, ", ", 2);
        ks_str_builder_add_repr(sb, self->elems[i]);
    }

    ks_str_builder_add(sb, ")", 1);
    
    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);

    return (ks_obj)ret;
}



// tuple.__getitem__(self, idx) - get the 'idx'th item
static KS_TFUNC(tuple, getitem) {
    ks_tuple self;
    ks_obj idx;
    KS_GETARGS("self:* idx", &self, ks_T_tuple, &idx)

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

        int64_t i, ct = 0;
        // create new tuple with no references
        ks_tuple res = ks_tuple_new_n((last - first) / delta, NULL);

        // add appropriate elements
        for (i = first; i != last; i += delta, ct++) {
            res->elems[ct] = self->elems[i];
            KS_INCREF(self->elems[i]);
        }

        return (ks_obj)res;

    } else {
        return ks_throw(ks_T_TypeError, "Expected 'idx' to be an integer, or a slice, but got '%T'", idx);
    }
}

// tuple.__add__(L, R)
static KS_TFUNC(tuple, add) {
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

// tuple.__mul__(L, R)
static KS_TFUNC(tuple, mul) {
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

        ks_tuple res_tuple = ks_tuple_new(res->len, res->elems);
        KS_DECREF(res);
        return (ks_obj)res_tuple;

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

        ks_tuple res_tuple = ks_tuple_new(res->len, res->elems);
        KS_DECREF(res);
        return (ks_obj)res_tuple;
    }

    KS_THROW_BOP_ERR("*", L, R);
}



/* iterator type */

// ks_tuple_iter - tuple iterable type
typedef struct {
    KS_OBJ_BASE

    // the tuple it is iterating over
    ks_tuple self;

    // current position in the array
    ks_ssize_t pos;


}* ks_tuple_iter;

// declare type
KS_TYPE_DECLFWD(ks_T_tuple_iter);

// tuple_iter.__free__(self) - free obj
static KS_TFUNC(tuple_iter, free) {
    ks_tuple_iter self;
    KS_GETARGS("self:*", &self, ks_T_tuple_iter)

    // remove reference to string
    KS_DECREF(self->self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// tuple_iter.__next__(self) - return next character
static KS_TFUNC(tuple_iter, next) {
    ks_tuple_iter self;
    KS_GETARGS("self:*", &self, ks_T_tuple_iter)
    
    // check if the iterator is done
    if (self->pos >= self->self->len) return ks_throw(ks_T_OutOfIterError, "");


    // get next element
    ks_obj ret = self->self->elems[self->pos++];
    return KS_NEWREF(ret);
}

// tuple.__iter__(self) - return iterator
static KS_TFUNC(tuple, iter) {
    ks_tuple self;
    KS_GETARGS("self:*", &self, ks_T_tuple)

    ks_tuple_iter ret = KS_ALLOC_OBJ(ks_tuple_iter);
    KS_INIT_OBJ(ret, ks_T_tuple_iter);

    ret->self = self;
    KS_INCREF(self);

    // position at start
    ret->pos = 0;

    return (ks_obj)ret;
}




/* export */

KS_TYPE_DECLFWD(ks_T_tuple);

void ks_init_T_tuple() {
    ks_type_init_c(ks_T_tuple, "tuple", ks_T_obj, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c(tuple_new_, "tuple.__new__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(tuple_free_, "tuple.__free__(self)")},
        {"__len__",                (ks_obj)ks_cfunc_new_c(tuple_len_, "tuple.__len__(self)")},
        {"__hash__",               (ks_obj)ks_cfunc_new_c(tuple_hash_, "tuple.__hash__(self)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c(tuple_str_, "tuple.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(tuple_str_, "tuple.__repr__(self)")},

        {"__getitem__",            (ks_obj)ks_cfunc_new_c(tuple_getitem_, "tuple.__getitem__(self, idx)")},
        
        {"__add__",                (ks_obj)ks_cfunc_new_c(tuple_add_, "tuple.__add__(L, R)")},
        {"__mul__",                (ks_obj)ks_cfunc_new_c(tuple_mul_, "tuple.__mul__(L, R)")},
        
        {"__iter__",               (ks_obj)ks_cfunc_new_c(tuple_iter_, "tuple.__iter__(self)")},

    ));
    ks_type_init_c(ks_T_tuple_iter, "tuple_iter", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(tuple_iter_free_, "tuple_iter.__free__(self)")},

        {"__next__",               (ks_obj)ks_cfunc_new_c(tuple_iter_next_, "tuple_iter.__next__(self)")},
    ));
}


