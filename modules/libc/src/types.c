/* src/types.c - implementation of the Ctypes bindings
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "../libc.h"


KS_TYPE_DECLFWD(libc_type_char);
KS_TYPE_DECLFWD(libc_type_short);
KS_TYPE_DECLFWD(libc_type_int);
KS_TYPE_DECLFWD(libc_type_long);

KS_TYPE_DECLFWD(libc_type_uchar);
KS_TYPE_DECLFWD(libc_type_ushort);
KS_TYPE_DECLFWD(libc_type_uint);
KS_TYPE_DECLFWD(libc_type_ulong);


/* BASIC INTEGERS */


#define PASTE(x, y) x##_##y

#define T_inttype(_type, _type_obj, _type_name, _c_type) \
_type PASTE(libc_make, _type_name) (_c_type val) { \
    _type self = KS_ALLOC_OBJ(_type); \
    KS_INIT_OBJ(self, _type_obj); \
    self->val = val; \
    return self; \
} \
static KS_TFUNC(_type_name, new) { \
    KS_REQ_N_ARGS(n_args, 1); \
    ks_obj obj = args[0]; \
    if (obj->type == ks_type_int) { \
        int64_t val = 0; \
        /**/ if (!ks_num_get_int64(obj, &val)) return NULL; \
        else if (val > INT_MAX || val < INT_MIN) { \
            ks_throw_fmt(ks_type_ToDoError, "Decide how to cast >32 bit values to C int");  \
            return NULL; \
        } \
        return (ks_obj)PASTE(libc_make, _type_name)(val); \
    } else { \
        KS_ERR_CONV(obj, _type_obj); \
    } \
} \
static KS_TFUNC(_type_name, free) { \
    KS_REQ_N_ARGS(n_args, 1); \
    _type self = (_type)args[0]; \
    KS_REQ_TYPE(self, _type_obj, "self"); \
    KS_UNINIT_OBJ(self); \
    KS_FREE_OBJ(self); \
    return KSO_NONE; \
} \
static KS_TFUNC(_type_name, str) { \
    KS_REQ_N_ARGS(n_args, 1); \
    _type self = (_type)args[0]; \
    KS_REQ_TYPE(self, _type_obj, "self"); \
    return (ks_obj)ks_fmt_c("%l", (int64_t)self->val); \
} \
static KS_TFUNC(_type_name, int) { \
    KS_REQ_N_ARGS(n_args, 1); \
    _type self = (_type)args[0]; \
    KS_REQ_TYPE(self, _type_obj, "self"); \
    return (ks_obj)ks_int_new(self->val); \
} \


// template for int types
T_inttype(libc_char, libc_type_char, char, int8_t)
T_inttype(libc_short, libc_type_short, short, int16_t)
T_inttype(libc_int, libc_type_int, int, int32_t)
T_inttype(libc_long, libc_type_long, long, int64_t)

T_inttype(libc_uchar, libc_type_uchar, uchar, uint8_t)
T_inttype(libc_ushort, libc_type_ushort, ushort, uint16_t)
T_inttype(libc_uint, libc_type_uint, uint, uint32_t)
T_inttype(libc_ulong, libc_type_ulong, ulong, uint64_t)


KS_TYPE_DECLFWD(libc_type_pointer);
KS_TYPE_DECLFWD(libc_type_func_pointer);

// dictionary of type -> pointer[type]
static ks_dict pointer_types = NULL;

/// dictionary of (types, ...) -> func_poitner[*types]
static ks_dict func_pointer_types = NULL;



#ifdef KS_HAVE_FFI

// TODO: add support for architectures where the size of a function pointer is not the same as a normal pointer
static ffi_type my_ffi_type_funcptr = (ffi_type) {
    .size = 0,
    .alignment = 0,

    .type = FFI_TYPE_POINTER,
    .elements = NULL,
};


// convert a kscript type to an FFI type
static ffi_type* my_get_ffitype(ks_type kstype) {
    if (kstype == libc_type_int || kstype == ks_type_int) {
        return &ffi_type_sint;
    } else if (kstype == ks_type_none) {
        return &ffi_type_void;
    } else if (ks_type_issub(kstype, libc_type_pointer)) {
        return &ffi_type_pointer;
    } else {
        ks_throw_fmt(ks_type_TypeError, "Cannot convert type '%S' into a compatible FFI type", kstype);
        return NULL;
    }
}

// cast a kscript object into a ffi type
// NOTE: dest may be reallocated, so either give NULL, or something allocated with ks_malloc()
static bool my_to_ffitype(ffi_type* totype, ks_obj obj, void* dest) {

    if (totype == &ffi_type_sint32) {

        int64_t v64;
        if(!ks_num_get_int64(obj, &v64)) return false;

        *(int32_t*)dest = v64;

        // success
        return true;
    } else if (totype == &ffi_type_pointer) {

        if (obj->type == ks_type_str) {
            *(char**)dest = ((ks_str)obj)->chr;
        } else {
            ks_throw_fmt(ks_type_TypeError, "'%T' object canno tbe turned into an FFI pointer!", obj);
            return false;
        }

        return true;
    } else {

        ks_throw_fmt(ks_type_ToDoError, "Need to implement other ffi types");
        return false;
    }

}



// Get kscript object from ffitype and a data location
static ks_obj my_from_ffitype(ffi_type* fromtype, void* data) {
    if (fromtype == &ffi_type_sint32) {
        return (ks_obj)libc_make_int(*(int32_t*)data);
    } else {
        return ks_throw_fmt(ks_type_ToDoError, "Need to implement other ffi types");
    }
}


#endif


// forward decl
static KS_TFUNC(pointer, new);
static KS_TFUNC(func_pointer, new);

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


// Create a function pointer type
ks_type libc_make_func_pointer_type(int n_args, ks_type* argtypes) {

    // construct key
    ks_tuple of = ks_tuple_new(n_args, (ks_obj*)argtypes);

    ks_obj ret = ks_dict_get(func_pointer_types, 0, (ks_obj)of);
    ks_type p_T = NULL;
    if (ret == NULL) {
        // create type
        p_T = KS_ALLOC_OBJ(ks_type);

        ks_str_b SB;
        ks_str_b_init(&SB);

        ks_str_b_add_fmt(&SB, "%S (*)(", argtypes[0]);

        int i;
        for (i = 1; i < n_args; ++i) {
            if (i > 1) ks_str_b_add_c(&SB, ", ");
            ks_str_b_add_fmt(&SB, "%S", argtypes[i]);
        }

        ks_str_b_add_fmt(&SB, ")");


        ks_str tname = ks_str_b_get(&SB);
        ks_str_b_free(&SB);

        KS_INIT_TYPE_OBJ(p_T, tname->chr);
        KS_DECREF(tname);

        ks_type_add_parent(p_T, libc_type_func_pointer);

        ks_type_set_c(p_T, "of", (ks_obj)of);

        ks_obj p_get__base_ = (ks_obj)ks_cfunc_new2(func_pointer_new_, "func_pointer.__new__(obj)");
        ks_pfunc p_get_ = ks_pfunc_new(p_get__base_);
        KS_DECREF(p_get__base_);
        ks_pfunc_fill(p_get_, 0, (ks_obj)p_T);

        ks_type_set_c(p_T, "__new__", (ks_obj)p_get_);

        KS_DECREF(p_get_);


        // now, create meta-data
        struct libc_fp_meta* fp_meta = ks_malloc(sizeof(*fp_meta));


        fp_meta->_ffi_n = n_args;
        fp_meta->_ffi_types = ks_malloc(sizeof(*fp_meta->_ffi_types) * n_args);


        // TODO: actually convert things over to relevant data types
        for (i = 0; i < n_args; ++i) {
            //fp_meta->_ffi_types[i] = &ffi_type_sint;
            fp_meta->_ffi_types[i] = my_get_ffitype((ks_type)argtypes[i]);
            if (!fp_meta->_ffi_types[i]) {
                KS_DECREF(of);
                KS_DECREF(p_T);
                ks_free(fp_meta->_ffi_types);
                return NULL;
            }
        }

        /* Initialize the cif */
        if (ffi_prep_cif(&fp_meta->_ffi_cif, FFI_DEFAULT_ABI, n_args - 1, fp_meta->_ffi_types[0], &fp_meta->_ffi_types[1]) != FFI_OK) {
            ks_throw_fmt(ks_type_InternalError, "Internal error prepping CIF for FFI");
            KS_DECREF(of);
            KS_DECREF(p_T);
            ks_free(fp_meta->_ffi_types);
            return NULL;
        }

        // this object can be unboxed elsewhere
        ks_obj fp_meta_obj = (ks_obj)libc_make_pointer(ks_type_none, (void*)fp_meta);
        if (!fp_meta_obj) {
            KS_DECREF(of);
            KS_DECREF(p_T);
            ks_free(fp_meta->_ffi_types);
            return NULL;
        }
        ks_type_set_c(p_T, "_fp_meta", (ks_obj)fp_meta_obj);
        KS_DECREF(fp_meta_obj);


        int r = ks_dict_set(func_pointer_types, 0, (ks_obj)of, (ks_obj)p_T);



    } else {
        // just assign pointer type
        p_T = (ks_type)ret;
    }

    KS_DECREF(of);
    return p_T;
}

// Create a function pointer
libc_func_pointer libc_make_func_pointer(int n_args, ks_type* argtypes, void (*val)()) {
    ks_type p_T = libc_make_func_pointer_type(n_args, argtypes);

    // create a new result
    libc_func_pointer self = KS_ALLOC_OBJ(libc_func_pointer);
    KS_INIT_OBJ(self, p_T);

    self->val = val;

    KS_DECREF(p_T);

    return self;
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

// pointer.__bool__(self)
static KS_TFUNC(pointer, bool) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_pointer self = (libc_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_pointer, "self");

    return (ks_obj)KSO_BOOL(self->val != NULL);
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


/* func_pointer */

// func_pointer.__new__(fp_type, obj) -> convert 'obj' to pointer
static KS_TFUNC(func_pointer, new) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type fp_type = (ks_type)args[0];
    ks_obj obj = args[1];
    if (!ks_type_issub(fp_type, libc_type_func_pointer)) return ks_throw_fmt(ks_type_TypeError, "Expected 'fp_type' to be of type '%S', but got %T", libc_type_func_pointer, fp_type);


    // now, get fp_meta pointer
    libc_pointer fp_type_meta = (libc_pointer)ks_dict_get_c(fp_type->attr, "_fp_meta");

    if (!fp_type_meta) {
        return ks_throw_fmt(ks_type_InternalError, "fp_type._fp_meta did not exist!");
    } else if (!ks_type_issub(fp_type_meta->type, libc_type_pointer)) {
        return ks_throw_fmt(ks_type_InternalError, "fp_type._fp_meta was not a pointer (got type %T)!", fp_type_meta);
    }

    // now, we have a pointer to the meta-data
    struct libc_fp_meta* fp_meta = (struct libc_fp_meta*)fp_type_meta->val;

    // free temporary resource
    KS_DECREF(fp_type_meta);


    if (ks_type_issub(obj->type, libc_type_func_pointer)) {
        // copy value, but change type
        libc_func_pointer self = KS_ALLOC_OBJ(libc_func_pointer);
        KS_INIT_OBJ(self, fp_type);

        // copy address of function
        self->val = ((libc_func_pointer)obj)->val;

        // store a copy to the meta-data
        self->fp_meta = fp_meta;

        int total_sz = 0, i;
        for (i = 0; i < fp_meta->_ffi_n; ++i) {
            total_sz += fp_meta->_ffi_types[i]->size;
        }

        // allocate enough size for the entire argument
        self->argdata = ks_malloc(total_sz);
        self->args = ks_malloc(sizeof(void*) * fp_meta->_ffi_n);

        int c_off = 0;

        for (i = 0; i < fp_meta->_ffi_n; ++i) {
            self->args[i] = (void*)((intptr_t)self->argdata + c_off);
            c_off += fp_meta->_ffi_types[i]->size;
        }

        return (ks_obj)self;

    } else if (ks_type_issub(obj->type, libc_type_pointer)) {
        // act as though its a void function pointer
        libc_func_pointer self = (libc_func_pointer)libc_make_func_pointer_type(1, &ks_type_none);

        // copy address of function
        self->val = (void (*)())((libc_pointer)obj)->val;

        // store a copy to the meta-data
        self->fp_meta = fp_meta;

        int total_sz = 0, i;
        for (i = 0; i < fp_meta->_ffi_n; ++i) {
            total_sz += fp_meta->_ffi_types[i]->size;
        }

        // allocate enough size for the entire argument
        self->argdata = ks_malloc(total_sz);
        self->args = ks_malloc(sizeof(void*) * fp_meta->_ffi_n);

        int c_off = 0;

        for (i = 0; i < fp_meta->_ffi_n; ++i) {
            self->args[i] = (void*)((intptr_t)self->argdata + c_off);
            c_off += fp_meta->_ffi_types[i]->size;
        }

        return (ks_obj)self;

    } else {
        ks_catch_ignore();
        KS_ERR_CONV(obj, libc_type_func_pointer);
    }
}

// func_pointer.__free__(self)
static KS_TFUNC(func_pointer, free) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_func_pointer self = (libc_func_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_func_pointer, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// func_pointer.__str__(self)
static KS_TFUNC(func_pointer, str) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_func_pointer self = (libc_func_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_func_pointer, "self");

    return (ks_obj)ks_fmt_c("(%S)%p", self->type->__name__, self->val);
}

// func_pointer.make(ret_type, arg_types=(,))
static KS_TFUNC(func_pointer, make) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type ret_type;
    ks_obj arg_types = NULL;
    if (!ks_parse_params(n_args, args, "ret_type%* ?arg_types%iter", &ret_type, ks_type_type, &arg_types)) return NULL;

    if (arg_types != NULL) {
        // handle return & argument types


        // convert to a list
        ks_list list_arg_types = ks_list_from_iterable(arg_types);
        if (!list_arg_types) return NULL;

        // collect list of types 
        int total_n_args = 1 + list_arg_types->len;
        ks_type* all_types = ks_malloc(sizeof(ks_type) * total_n_args);

        all_types[0] = ret_type;
        int i;
        for (i = 0; i < list_arg_types->len; ++i) {
            all_types[i + 1] = (ks_type)list_arg_types->elems[i];
            if (all_types[i + 1]->type != ks_type_type) {
                ks_throw_fmt(ks_type_TypeError, "Function pointer arg_types included non-type object '%S'", all_types[i + 1]);
                KS_DECREF(list_arg_types);
                ks_free(all_types);
                return NULL;
            }
        }

        // now, create a new type
        ks_type new_fp_type = libc_make_func_pointer_type(total_n_args, all_types);

        ks_free(all_types);
        KS_DECREF(list_arg_types);

        return (ks_obj)new_fp_type;

    } else {
        // just handle return type
        return (ks_obj)libc_make_func_pointer_type(1, &ret_type);
    }
}


// func_pointer.__call__(self, *args)
static KS_TFUNC(func_pointer, call) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    libc_func_pointer self = (libc_func_pointer)args[0];
    KS_REQ_TYPE(self, libc_type_func_pointer, "self");

    // get meta-data
    struct libc_fp_meta* fp_meta = self->fp_meta;

    // +1,-1 cancel out for including 'self', but not including the return value
    KS_REQ_N_ARGS(n_args, fp_meta->_ffi_n);

    int i;
    // iterate through arguments, and convert each one
    for (i = 1; i < fp_meta->_ffi_n; ++i) {
        if (!my_to_ffitype(fp_meta->_ffi_types[i], args[i], self->args[i])) {
            return NULL;
        }
    }

    // actually call it through FFI
    ffi_call(&fp_meta->_ffi_cif, self->val, self->args[0], &self->args[1]);

    // get return value
    return my_from_ffitype(fp_meta->_ffi_types[0], self->args[0]);
}


void libc_init_types() {

    // emit a warning
    if (sizeof(void*) != sizeof(void (*)())) {
        ks_warn("Size of normal pointers & function pointers differs (%i vs %i)", (int)sizeof(void*), (int)sizeof(void (*)()));
    }

    KS_INIT_TYPE_OBJ(libc_type_char, "libc.char");
    KS_INIT_TYPE_OBJ(libc_type_short, "libc.short");
    KS_INIT_TYPE_OBJ(libc_type_int, "libc.int");
    KS_INIT_TYPE_OBJ(libc_type_long, "libc.long");
    KS_INIT_TYPE_OBJ(libc_type_uchar, "libc.uchar");
    KS_INIT_TYPE_OBJ(libc_type_ushort, "libc.ushort");
    KS_INIT_TYPE_OBJ(libc_type_uint, "libc.uint");
    KS_INIT_TYPE_OBJ(libc_type_ulong, "libc.ulong");


    KS_INIT_TYPE_OBJ(libc_type_pointer, "libc.pointer");
    KS_INIT_TYPE_OBJ(libc_type_func_pointer, "libc.func_pointer");

    #define T_setupinttype(_type_name) \
    ks_type_set_cn(PASTE(libc_type, _type_name), (ks_dict_ent_c[]){ \
        {"__new__",        (ks_obj)ks_cfunc_new2(PASTE(_type_name, new_), "libc." #_type_name ".__new__(obj)")}, \
        {"__free__",       (ks_obj)ks_cfunc_new2(PASTE(_type_name, free_), "libc." #_type_name ".__free__(obj)")}, \
        {"__str__",        (ks_obj)ks_cfunc_new2(PASTE(_type_name, str_), "libc." #_type_name ".__str__(self)")}, \
        {"__repr__",       (ks_obj)ks_cfunc_new2(PASTE(_type_name, str_), "libc." #_type_name ".__repr__(self)")}, \
        {"__int__",        (ks_obj)ks_cfunc_new2(PASTE(_type_name, int_), "libc." #_type_name ".__int__(self)")}, \
        {"_is_ctype",      KSO_TRUE}, \
        {NULL, NULL}, \
    });


    T_setupinttype(char);
    T_setupinttype(short);
    T_setupinttype(int);
    T_setupinttype(long);

    T_setupinttype(uchar);
    T_setupinttype(ushort);
    T_setupinttype(uint);
    T_setupinttype(ulong);


    ks_type_set_cn(libc_type_pointer, (ks_dict_ent_c[]){

        {"__free__",        (ks_obj)ks_cfunc_new2(pointer_free_, "libc.pointer.__free__(obj)")},

        {"__str__",        (ks_obj)ks_cfunc_new2(pointer_str_, "libc.pointer.__str__(self)")},
        {"__repr__",       (ks_obj)ks_cfunc_new2(pointer_str_, "libc.pointer.__repr__(self)")},
        {"__bool__",        (ks_obj)ks_cfunc_new2(pointer_bool_, "libc.pointer.__bool__(self)")},
        {"__int__",        (ks_obj)ks_cfunc_new2(pointer_int_, "libc.pointer.__int__(self)")},

        {"__getitem__",        (ks_obj)ks_cfunc_new2(pointer_getitem_, "libc.pointer.__getitem__(self, idx)")},
        {"__setitem__",        (ks_obj)ks_cfunc_new2(pointer_setitem_, "libc.pointer.__setitem__(self, idx, val)")},

        {"create",             (ks_obj)ks_cfunc_new2(pointer_create_, "libc.pointer.create(type_of)")},

        {"_is_ctype",      KSO_TRUE},

        {NULL, NULL},
    });

    ks_type_set_cn(libc_type_func_pointer, (ks_dict_ent_c[]){

        {"__new__",        (ks_obj)ks_cfunc_new2(func_pointer_new_, "libc.func_pointer.__new__(obj)")},
        {"__free__",        (ks_obj)ks_cfunc_new2(func_pointer_free_, "libc.func_pointer.__free__(self)")},

        {"__str__",        (ks_obj)ks_cfunc_new2(func_pointer_str_, "libc.func_pointer.__str__(self)")},
        {"__repr__",        (ks_obj)ks_cfunc_new2(func_pointer_str_, "libc.func_pointer.__repr__(self)")},

        {"__call__",        (ks_obj)ks_cfunc_new2(func_pointer_call_, "libc.func_pointer.__call__(self, *args)")},

        {"make",             (ks_obj)ks_cfunc_new2(func_pointer_make_, "libc.func_pointer.make(ret_type, arg_types=(,))")},

        {NULL, NULL},
    });


    // dictionary containing pointer types
    pointer_types = ks_dict_new(0, NULL);
    func_pointer_types = ks_dict_new(0, NULL);

}


