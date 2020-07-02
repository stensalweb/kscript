/* src/types.c - implementation of the Ctypes bindings
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "../libc.h"


KS_TYPE_DECLFWD(libc_type_int);
KS_TYPE_DECLFWD(libc_type_pointer);


// dictionary of type -> pointer[type]
static ks_dict pointer_types = NULL;



// forward decl
static KS_TFUNC(pointer, new);

// get base type of a pointer type
static ks_type my_gettypeof(ks_type ptr_type) {
    ks_obj of = ks_type_get_c(ptr_type, "of");
    return (ks_type)of;
}

ks_ssize_t libc_get_size(ks_type of) {
    if (of == libc_type_int) {
        return sizeof(int);
    } else if (ks_type_issub(of, libc_type_pointer)) {
        return sizeof(void*);
    } else {
        ks_throw_fmt(ks_type_TypeError, "libc cannot find the size of type '%S'", of);
        return -1;
    }
}

/* 'make' routines */

// Create a libc_int
// NOTE: Returns a new reference
libc_int libc_make_int(int val) {
    // create a new result
    libc_int self = KS_ALLOC_OBJ(libc_int);
    KS_INIT_OBJ(self, libc_type_int);

    self->val = val;

    return self;
}


// Create a pointer type
ks_type libc_make_pointer_type(ks_type of) {
    if (of != ks_type_none) {
        ks_obj _isct = ks_type_get_c(of, "_is_ctype");
        if (!_isct) {
            return ks_throw_fmt(ks_type_TypeError, "Attempted to create a pointer type to a type that wasn't a C-type (%S)", of);
        }
        KS_DECREF(_isct);
    }

    ks_obj ret = ks_dict_get(pointer_types, 0, (ks_obj)of);
    ks_type p_T = NULL;
    if (ret == NULL) {
        // create type
        p_T = KS_ALLOC_OBJ(ks_type);
        ks_str tname = of == ks_type_none ? ks_fmt_c("void*") : ks_fmt_c("%S*", of->__name__);
        KS_INIT_TYPE_OBJ(p_T, tname->chr);
        KS_DECREF(tname);

        ks_type_add_parent(p_T, libc_type_pointer);

        ks_type_set_c(p_T, "of", (ks_obj)of);

        ks_obj p_get__base_ = (ks_obj)ks_cfunc_new2(pointer_new_, "pointer.__new__(obj)");
        ks_pfunc p_get_ = ks_pfunc_new(p_get__base_);
        KS_DECREF(p_get__base_);
        ks_pfunc_fill(p_get_, 0, (ks_obj)p_T);

        ks_type_set_c(p_T, "__new__", (ks_obj)p_get_);

        KS_DECREF(p_get_);

        int r = ks_dict_set(pointer_types, 0, (ks_obj)of, (ks_obj)p_T);

    } else {
        // just assign pointer type
        p_T = (ks_type)ret;
    }

    return p_T;
}

// Create a libc_int
// NOTE: Returns a new reference
libc_pointer libc_make_pointer(ks_type of, void* addr) {
    ks_type p_T = libc_make_pointer_type(of);

    // create a new result
    libc_pointer self = KS_ALLOC_OBJ(libc_pointer);
    KS_INIT_OBJ(self, p_T);

    self->val = addr;

    KS_DECREF(p_T);

    return self;
}





/* libc.int */

// int.__new__(obj) -> convert 'obj' to int
static KS_TFUNC(int, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];

    if (obj->type == ks_type_int) {
        int64_t val = 0;
        /**/ if (!ks_num_get_int64(obj, &val)) return NULL;
        else if (val > INT_MAX || val < INT_MIN) {
            ks_throw_fmt(ks_type_ToDoError, "Decide how to cast >32 bit values to C int");
            return NULL;
        }
        return (ks_obj)libc_make_int(val);
    } else {
        KS_ERR_CONV(obj, libc_type_int);
    }
}

// int.__free__(self) -> free
static KS_TFUNC(int, free) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_int self = (libc_int)args[0];
    KS_REQ_TYPE(self, libc_type_int, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// int.__str__(self)
static KS_TFUNC(int, str) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_int self = (libc_int)args[0];
    KS_REQ_TYPE(self, libc_type_int, "self");

    return (ks_obj)ks_fmt_c("%i", self->val);
}

// int.__int__(self)
static KS_TFUNC(int, int) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_int self = (libc_int)args[0];
    KS_REQ_TYPE(self, libc_type_int, "self");

    return (ks_obj)ks_int_new(self->val);
}


/* libc.pointer */


// pointer.__new__(type_of, obj) -> convert 'obj' to pointer
static KS_TFUNC(pointer, new) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type type_of = (ks_type)args[0];
    /*if (!(type_of->type == ks_type_type) || !ks_type_issub(type_of, libc_type_pointer)) {
        return ks_throw_fmt(ks_type_TypeError, "Incorrect type for 'type_of', expected 'libc.pointer', but got '%T'", type_of);
    }*/
    ks_obj obj = args[1];
    int64_t v64;

    if (ks_type_issub(obj->type, libc_type_pointer)) {

        ks_type type_of_call = my_gettypeof(type_of);
        if (!type_of_call) return NULL;
        //libc_pointer ret = libc_make_pointer(type_of_call, ((libc_pointer)obj)->val);
        KS_DECREF(type_of_call);
        //return (ks_obj)ret;
        return KS_NEWREF(obj);
    } else if (ks_num_get_int64(obj, &v64)) {

        ks_type type_of_call = my_gettypeof(type_of);
        libc_pointer ret = libc_make_pointer(type_of_call, (void*)(intptr_t)v64);
        KS_DECREF(type_of_call);
        return (ks_obj)ret;
    } else {
        ks_catch_ignore();
        KS_ERR_CONV(obj, libc_type_int);
    }
}


// pointer.__free__(self)
static KS_TFUNC(pointer, free) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_pointer self = (libc_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_pointer, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// pointer.create(type_of)
static KS_TFUNC(pointer, create) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_type type_of = (ks_type)args[0];
    if ((ks_obj)type_of != KSO_NONE) KS_REQ_TYPE(type_of, ks_type_type, "type_of")
    else type_of = ks_type_none;

    return (ks_obj)libc_make_pointer_type(type_of);
}

// pointer.__str__(self)
static KS_TFUNC(pointer, str) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_pointer self = (libc_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_pointer, "self");

    return (ks_obj)ks_fmt_c("(%S)%p", self->type->__name__, self->val);
}

// pointer.__int__(self)
static KS_TFUNC(pointer, int) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_pointer self = (libc_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_pointer, "self");

    return (ks_obj)ks_int_new((intptr_t)self->val);
}


// pointer.__getitem__(self, idx)
static KS_TFUNC(pointer, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    libc_pointer self;
    int64_t idx;
    if (!ks_parse_params(n_args, args, "self%* idx%i64", &self, libc_type_pointer, &idx)) return NULL;

    // now, switch based on type
    ks_type typ = my_gettypeof(self->type);

    if (typ == libc_type_int) {
        int* addr = &((int*)self->val)[idx];
        KS_DECREF(typ);
        return (ks_obj)libc_make_int(0);
    } else {
        KS_DECREF(typ);
        return ks_throw_fmt(ks_type_ToDoError, "Need to handle setting pointers of type '%T'", self);
    }
}

// pointer.__setitem__(self, idx, val)
static KS_TFUNC(pointer, setitem) {
    KS_REQ_N_ARGS(n_args, 3);
    libc_pointer self;
    int64_t idx;
    ks_obj val;
    if (!ks_parse_params(n_args, args, "self%* idx%i64 val%any", &self, libc_type_pointer, &idx, &val)) return NULL;

    // now, switch based on type
    ks_type typ = my_gettypeof(self->type);


    if (typ == libc_type_int) {
        int* addr = &((int*)self->val)[idx];
        int64_t val64;
        if (!ks_num_get_int64(val, &val64)) {
            KS_DECREF(typ);
            return NULL;
        }
        *addr = (int)val64;

    } else {
        KS_DECREF(typ);
        return ks_throw_fmt(ks_type_ToDoError, "Need to handle setting pointers of type '%T'", self);
    }

    KS_DECREF(typ);
    return (ks_obj)KS_NEWREF(val);
}


void libc_init_types() {

    KS_INIT_TYPE_OBJ(libc_type_int, "libc.int");
    KS_INIT_TYPE_OBJ(libc_type_pointer, "libc.pointer");

    ks_type_set_cn(libc_type_int, (ks_dict_ent_c[]){
        {"__new__",        (ks_obj)ks_cfunc_new2(int_new_, "libc.int.__new__(obj)")},
        {"__free__",        (ks_obj)ks_cfunc_new2(int_free_, "libc.int.__free__(obj)")},

        {"__str__",        (ks_obj)ks_cfunc_new2(int_str_, "libc.int.__str__(self)")},
        {"__repr__",       (ks_obj)ks_cfunc_new2(int_str_, "libc.int.__repr__(self)")},
        {"__int__",        (ks_obj)ks_cfunc_new2(int_int_, "libc.int.__int__(self)")},

        {"_is_ctype",      KSO_TRUE},

        {NULL, NULL},
    });

    ks_type_set_cn(libc_type_pointer, (ks_dict_ent_c[]){

        {"__free__",        (ks_obj)ks_cfunc_new2(pointer_free_, "libc.pointer.__free__(obj)")},

        {"__str__",        (ks_obj)ks_cfunc_new2(pointer_str_, "libc.pointer.__str__(self)")},
        {"__repr__",       (ks_obj)ks_cfunc_new2(pointer_str_, "libc.pointer.__repr__(self)")},
        {"__int__",        (ks_obj)ks_cfunc_new2(pointer_int_, "libc.pointer.__int__(self)")},

        {"__getitem__",        (ks_obj)ks_cfunc_new2(pointer_getitem_, "libc.pointer.__getitem__(self, idx)")},
        {"__setitem__",        (ks_obj)ks_cfunc_new2(pointer_setitem_, "libc.pointer.__setitem__(self, idx, val)")},

        {"create",             (ks_obj)ks_cfunc_new2(pointer_create_, "libc.pointer.create(type_of)")},

        {"_is_ctype",      KSO_TRUE},

        {NULL, NULL},
    });

    /*
    ks_obj p_get__base_ = (ks_obj)ks_cfunc_new2(Enum_getitem_, "Enum.__getitem__(self, item)");
    ks_pfunc p_get_ = ks_pfunc_new(p_get__base_);
    KS_DECREF(p_get__base_);
    ks_pfunc_fill(p_get_, 0, (ks_obj)enumtype);
    */


    // dictionary containing pointer types
    pointer_types = ks_dict_new(0, NULL);

}


