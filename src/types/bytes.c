/* bytes.c - immutable byte array type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// List of global singletons representing single-byte `bytes` objects
#define KS_BYTE_MAX 255

// global singletons (+ 1 for empty constant)
static struct ks_bytes_s KS_BYTES[KS_BYTE_MAX + 1];


// Create new bytes array
ks_bytes ks_bytes_new(const uint8_t* byt, ks_size_t len_b) {
    /**/ if (len_b == 0) return &KS_BYTES[KS_BYTE_MAX];
    else if (len_b == 1) return &KS_BYTES[*byt];
    else {
        // construct it
        ks_bytes self = ks_malloc(sizeof(*self) + len_b - 1);
        KS_INIT_OBJ(self, ks_T_bytes);

        self->len_b = len_b;

        memcpy(self->byt, byt, len_b);
        self->v_hash = ks_hash_bytes(self->byt, len_b);

        return self;
    }
}

// bytes.__new__(obj) - create new bytes object
static KS_TFUNC(bytes, new) {
    ks_obj obj;
    KS_GETARGS("obj", &obj);

    if (ks_type_issub(obj->type, ks_T_bytes)) {
        return KS_NEWREF(obj);
    } else if (obj->type->__bytes__ != NULL) {
        return ks_obj_call(obj->type->__bytes__, 1, &obj);
    }

    KS_THROW_TYPE_ERR(obj, ks_T_bytes);
}

// bytes.__free__(self) - free obj
static KS_TFUNC(bytes, free) {
    ks_bytes self;
    KS_GETARGS("self:*", &self, ks_T_bytes)

    if (self >= &KS_BYTES[0] && self <= &KS_BYTES[KS_BYTE_MAX + 1]) {
        // global singleton
        self->refcnt = KS_REFS_INF;
        return KSO_NONE;
    }

    // nothing else is needed because the bytes are allocated with enough bytes for all the characters    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// bytes.__repr__(self) - get repr
static KS_TFUNC(bytes, repr) {
    ks_bytes self;
    KS_GETARGS("self:*", &self, ks_T_bytes)

    ks_str_builder sb = ks_str_builder_new();

    ks_str_builder_add_fmt(sb, "b'");

    // hex digits
    static const char bytes_hexdig[] = "012345689ABCDEF";

    ks_size_t i;
    for (i = 0; i < self->len_b; ++i) {
        uint8_t c = self->byt[i];
        ks_str_builder_add_fmt(sb, "\\x%c%c", bytes_hexdig[c / 16], bytes_hexdig[c % 16]);
    }
    ks_str_builder_add_fmt(sb, "'");

    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);
    return (ks_obj)ret;
}

// bytes.__len__(self) - get length
static KS_TFUNC(bytes, len) {
    ks_bytes self;
    KS_GETARGS("self:*", &self, ks_T_bytes)

    return (ks_obj)ks_int_new(self->len_b);
}


/* iterator type */

// ks_bytes_iter - type describing a string iterator
typedef struct {
    KS_OBJ_BASE

    // the object being iterated
    ks_bytes self;

    // current position
    ks_size_t pos;

}* ks_bytes_iter;

// declare type
KS_TYPE_DECLFWD(ks_T_bytes_iter);

// bytes_iter.__free__(self) - free obj
static KS_TFUNC(bytes_iter, free) {
    ks_bytes_iter self;
    KS_GETARGS("self:*", &self, ks_T_bytes_iter)

    // remove reference to string
    KS_DECREF(self->self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// bytes_iter.__next__(self) - return next character
static KS_TFUNC(bytes_iter, next) {
    ks_bytes_iter self;
    KS_GETARGS("self:*", &self, ks_T_bytes_iter)
    
    // check for out of bounds
    if (self->pos >= self->self->len_b) return ks_throw(ks_T_OutOfIterError, "");

    return (ks_obj)&KS_BYTES[self->self->byt[self->pos++]];
}

// bytes.__iter__(self) - return iterator
static KS_TFUNC(bytes, iter) {
    ks_bytes self;
    KS_GETARGS("self:*", &self, ks_T_bytes)

    ks_bytes_iter ret = KS_ALLOC_OBJ(ks_bytes_iter);
    KS_INIT_OBJ(ret, ks_T_bytes_iter);

    ret->self = self;
    KS_INCREF(self);

    return (ks_obj)ret;
}


/* export */

KS_TYPE_DECLFWD(ks_T_bytes);

void ks_init_T_bytes() {

    // initialize global singletons
    int i;
    for (i = 0; i < KS_BYTE_MAX; ++i) {
        ks_bytes tc = &KS_BYTES[i];
        KS_INIT_OBJ(tc, ks_T_bytes);
        tc->len_b = 1;
        tc->byt[0] = i;
        tc->v_hash = ks_hash_bytes(tc->byt, 1);
    }


    // handle size 0
    ks_bytes tc = &KS_BYTES[i];
    KS_INIT_OBJ(tc, ks_T_bytes);
    tc->len_b = 0;
    tc->v_hash = ks_hash_bytes(tc->byt, 0);

    ks_type_init_c(ks_T_bytes, "bytes", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c_old(bytes_new_, "bytes.__new__(obj, *args)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(bytes_free_, "bytes.__free__(self)")},
        {"__iter__",               (ks_obj)ks_cfunc_new_c_old(bytes_iter_, "bytes.__iter__(self)")},

        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(bytes_repr_, "bytes.__repr__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(bytes_repr_, "bytes.__str__(self)")},
        {"__len__",                (ks_obj)ks_cfunc_new_c_old(bytes_len_, "bytes.__len__(self)")},

    ));

    ks_type_init_c(ks_T_bytes_iter, "bytes_iter", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(bytes_iter_free_, "bytes_iter.__free__(self)")},
        {"__next__",               (ks_obj)ks_cfunc_new_c_old(bytes_iter_next_, "bytes_iter.__next__(self)")},
    ));

}
