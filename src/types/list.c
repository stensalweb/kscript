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
    // starting index
    int idx = self->len;
    self->len += n;

    self->elems = ks_realloc(self->elems, sizeof(*self->elems) * self->len);

    ks_size_t i;
    // add new elements
    for (i = idx; i < self->len; ++i) {
        self->elems[i] = objs[i - idx];
        KS_INCREF(self->elems[i]);
    }

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



// list.__free__(self) - free object
static KS_TFUNC(list, free) {
    ks_list self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_list)) return NULL;

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




// list.__str__(self) - to string
static KS_TFUNC(list, str) {
    ks_list self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_list)) return NULL;

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


/* export */

KS_TYPE_DECLFWD(ks_T_list);

void ks_init_T_list() {
    ks_type_init_c(ks_T_list, "list", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(list_free_, "list.__free__(self)")},
        
        {"__str__",                (ks_obj)ks_cfunc_new_c(list_str_, "list.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(list_str_, "list.__repr__(self)")},

    ));

}


