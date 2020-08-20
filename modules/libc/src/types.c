/* src/types.c - implementation of the Ctypes bindings
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "../kslibc-impl.h"


KS_TYPE_DECLFWD(libc_T_void);

KS_TYPE_DECLFWD(libc_T_char);
KS_TYPE_DECLFWD(libc_T_short);
KS_TYPE_DECLFWD(libc_T_int);
KS_TYPE_DECLFWD(libc_T_long);

KS_TYPE_DECLFWD(libc_T_uchar);
KS_TYPE_DECLFWD(libc_T_ushort);
KS_TYPE_DECLFWD(libc_T_uint);
KS_TYPE_DECLFWD(libc_T_ulong);


// templated types
KS_TYPE_DECLFWD(libc_T_pointer);
KS_TYPE_DECLFWD(libc_T_function);


// pointers
KS_TYPE_DECLFWD(libc_T_void_p);

KS_TYPE_DECLFWD(libc_T_char_p);
KS_TYPE_DECLFWD(libc_T_short_p);
KS_TYPE_DECLFWD(libc_T_int_p);
KS_TYPE_DECLFWD(libc_T_long_p);

KS_TYPE_DECLFWD(libc_T_uchar_p);
KS_TYPE_DECLFWD(libc_T_ushort_p);
KS_TYPE_DECLFWD(libc_T_uint_p);
KS_TYPE_DECLFWD(libc_T_ulong_p);


// forward decl the constructors so they can be wrapped in template construction
static ks_cfunc F_pointer_new = NULL, F_function_new = NULL;


// dictionary of type -> pointer[type]
static ks_dict pointer_types = NULL;

/// dictionary of (types, ...) -> function[*types]
static ks_dict function_types = NULL;


/* BASIC INTEGERS */

// utility to paste with _ between them
#define PASTE(x, y) x##_##y

#define T_inttype(_type, _type_obj, _type_name, _c_type) \
_type PASTE(libc_make, _type_name) (_c_type val) { \
    _type self = KS_ALLOC_OBJ(_type); \
    KS_INIT_OBJ(self, _type_obj); \
    self->val = val; \
    return self; \
} \
static KS_TFUNC(_type_name, new) { \
    int64_t obj; \
    KS_GETARGS("obj:i64", &obj); \
    return (ks_obj)PASTE(libc_make, _type_name)(obj); \
} \
static KS_TFUNC(_type_name, free) { \
    _type self; \
    KS_GETARGS("self:*", &self, _type_obj); \
    KS_UNINIT_OBJ(self); \
    KS_FREE_OBJ(self); \
    return KSO_NONE; \
} \
static KS_TFUNC(_type_name, str) { \
    _type self; \
    KS_GETARGS("self:*", &self, _type_obj); \
    return (ks_obj)ks_fmt_c("%l", (int64_t)self->val); \
} \
static KS_TFUNC(_type_name, int) { \
    _type self; \
    KS_GETARGS("self:*", &self, _type_obj); \
    return (ks_obj)ks_int_new(self->val); \
} \


// template for int types
T_inttype(libc_char, libc_T_char, char, int8_t)
T_inttype(libc_short, libc_T_short, short, int16_t)
T_inttype(libc_int, libc_T_int, int, int32_t)
T_inttype(libc_long, libc_T_long, long, int64_t)

T_inttype(libc_uchar, libc_T_uchar, uchar, uint8_t)
T_inttype(libc_ushort, libc_T_ushort, ushort, uint16_t)
T_inttype(libc_uint, libc_T_uint, uint, uint32_t)
T_inttype(libc_ulong, libc_T_ulong, ulong, uint64_t)



#ifdef KS_HAVE_FFI

// TODO: add support for architectures where the size of a function pointer is not the same as a normal pointer
static ffi_type my_ffi_type_funcptr = (ffi_type) {
    .size = 0,
    .alignment = 0,

    .type = FFI_TYPE_POINTER,
    .elements = NULL,
};



// convert a kscript type to an FFI type (or, return NULL and throw error if it wasn't)
static ffi_type* my_get_ffitype(ks_type kstype) {
    if (kstype == libc_T_void || kstype == ks_T_none) {
        return &ffi_type_void;
    } else if (kstype == libc_T_int || kstype == ks_T_int) {
        return &ffi_type_sint;
    } else if (ks_type_issub(kstype, libc_T_pointer)) {
        return &ffi_type_pointer;
    } else {
        ks_throw(ks_T_TypeError, "Cannot convert type '%S' into a compatible FFI type", kstype);
        return NULL;
    }
}

// Convert kscript object -> FFI-compatible object
// NOTE: some types (like, for example, strings) may require additional space to be converted
// (i.e. so any C routines don't modify the data)
// In this case, `*alloc` will be set to an address that needs to be `ks_free()`'d after the function has finished calling
static bool my_to_ffitype(ffi_type* totype, ks_obj obj, void* dest, void** alloc) {

    if (totype == &ffi_type_void) {

        // only valid values for void are none
        if (obj->type == ks_T_none) {
            return true;
        }
        
        ks_throw(ks_T_TypeError, "'%T' object cannot tbe turned into a C 'void' object", obj);
        return false;

    } else if (totype == &ffi_type_sint32) {

        int64_t v64;
        if(ks_num_get_int64(obj, &v64)) {
            *(int32_t*)dest = v64;
            return true;
        }
        
        ks_throw(ks_T_TypeError, "'%T' object cannot tbe turned into a C 'int' object", obj);
        return false;

    } else if (totype == &ffi_type_pointer) {

        if (ks_type_issub(obj->type, libc_T_pointer)) {

            *(void**)dest = ((libc_pointer)obj)->val;
            return true;
        } else if (obj->type == ks_T_str) {
            *alloc = ks_malloc(((ks_str)obj)->len_b + 1);

            // create NUL-terminated string
            memcpy(*alloc, ((ks_str)obj)->chr, ((ks_str)obj)->len_b);
            ((char*)*alloc)[((ks_str)obj)->len_b] = '\0';


            // return results
            *(void**)dest = *alloc;
            return true;
        }

        ks_throw(ks_T_TypeError, "'%T' object cannot tbe turned into a C pointer object", obj);
        return false;
    } else {
        ks_throw(ks_T_TodoError, "Need to implement other ffi types");
        return false;
    }
}


// Get kscript object from ffitype and a data location
static ks_obj my_from_ffitype(ffi_type* fromtype, void* data) {
    if (fromtype == &ffi_type_sint32) {
        return (ks_obj)libc_make_int(*(int32_t*)data);
    } else {
        return ks_throw(ks_T_TodoError, "Need to implement other ffi types");
    }
}


#endif

// get base type of a pointer type
static ks_type my_gettypeof(ks_type ptr_type) {
    ks_str of = ks_str_new("of");
    ks_obj ret = ks_type_get(ptr_type, of);
    KS_DECREF(of);
    return (ks_type)ret;
}

ks_ssize_t libc_get_size(ks_type of) {
    if (of == libc_T_int) {
        return sizeof(int);
    } else if (ks_type_issub(of, libc_T_pointer)) {
        return sizeof(void*);
    } else {
        ks_throw(ks_T_TypeError, "libc cannot find the size of type '%S'", of);
        return -1;
    }
}


// Create a pointer type
ks_type libc_make_pointer_type(ks_type of) {
    if (of != ks_T_none) {
        ks_str key = ks_str_new("_is_ctype");
        ks_obj _isct = ks_type_get(of, key);
        KS_DECREF(key);
        if (!_isct) {
            ks_catch_ignore();
            return (ks_type)ks_throw(ks_T_TypeError, "Attempted to create a pointer type to a type that wasn't a C-type (%S)", of);
        }
        KS_DECREF(_isct);
    }

    ks_obj ret = ks_dict_get(pointer_types, (ks_obj)of);

    // type to return
    ks_type ptr_type = NULL;


    if (ret == NULL) {

        // create a name
        ks_str ptr_type_name = ks_fmt_c("%S*", of->__name__);

        // we need to create type from scratch
        ptr_type = KS_ALLOC_OBJ(ks_type);
        ks_type_init_c(ptr_type, ptr_type_name->chr, libc_T_pointer, KS_KEYVALS(
        ));

        KS_DECREF(ptr_type_name);


        // set a reference to the base type
        /*if (!ks_type_set_c(ptr_type, "_base_type", (ks_obj)of)) {
            KS_DECREF(ptr_type);
            return NULL;
        }*/

        // store it in the cache
        if (!ks_dict_set(pointer_types, (ks_obj)of, (ks_obj)ptr_type)) {
            KS_DECREF(ptr_type);
            return NULL;
        }

    } else {
        // just assign pointer type
        ptr_type = (ks_type)ret;
        if (!ks_type_issub(ptr_type, libc_T_pointer)) {
            ks_throw(ks_T_InternalError, "Internally, libc's pointer type cache contained non-type object: %S", ret);
            KS_DECREF(ret);
            return NULL;
        }
    }

    return ptr_type;
}


// create a function type
// NOTE: argtypes[0] is return type, so argument types start at index #1
ks_type libc_make_function_type(int n_args, ks_type* argtypes) {

    // construct 'key' of the function type
    ks_tuple ofkey = ks_tuple_new(n_args, (ks_obj*)argtypes);

    // attempt to find in cache
    ks_obj ret = ks_dict_get(function_types, (ks_obj)ofkey);
    ks_type func_type = NULL;


    if (ret == NULL) {
        // create a name

        ks_str_builder sb = ks_str_builder_new();

        ks_str_builder_add_fmt(sb, "%S (*)(", argtypes[0]);

        int i;
        for (i = 1; i < n_args; ++i) {
            if (i > 1) ks_str_builder_add_fmt(sb, ", ");
            ks_str_builder_add_fmt(sb, "%S", argtypes[i]);
        }
        ks_str_builder_add_fmt(sb, ")");


        ks_str func_type_name = ks_str_builder_get(sb);
        KS_DECREF(sb);


        // we need to create type from scratch
        func_type = KS_ALLOC_OBJ(ks_type);
        ks_type_init_c(func_type, func_type_name->chr, libc_T_function, KS_KEYVALS(

        ));
        KS_DECREF(func_type_name);


        ks_tuple argtypes_tuple = ks_tuple_new(n_args - 1, (ks_obj*)(argtypes + 1));
        
        /* FFI (Foreign Function Interface) */

        // now, create meta-data
        struct libc_fp_meta* fp_meta = ks_malloc(sizeof(*fp_meta));

        #ifdef KS_HAVE_FFI

        fp_meta->_ffi_n = n_args;
        fp_meta->_ffi_types = ks_malloc(sizeof(*fp_meta->_ffi_types) * n_args);

        // TODO: actually convert things over to relevant data types
        for (i = 0; i < n_args; ++i) {
            //fp_meta->_ffi_types[i] = &ffi_type_sint;
            fp_meta->_ffi_types[i] = my_get_ffitype((ks_type)argtypes[i]);
            if (!fp_meta->_ffi_types[i]) {
                KS_DECREF(ofkey);
                KS_DECREF(func_type);
                ks_free(fp_meta->_ffi_types);
                return NULL;
            }
        }

        /* Initialize the cif */
        if (ffi_prep_cif(&fp_meta->_ffi_cif, FFI_DEFAULT_ABI, n_args - 1, fp_meta->_ffi_types[0], &fp_meta->_ffi_types[1]) != FFI_OK) {
            ks_throw(ks_T_InternalError, "Internal error prepping CIF for FFI");
            KS_DECREF(ofkey);
            KS_DECREF(func_type);
            ks_free(fp_meta->_ffi_types);
            return NULL;
        }
        #else
        // perhaps warn here?
        fp_meta->_ffi_types = NULL;

        #endif

        ks_type_set_c(func_type, KS_KEYVALS(
            {"_rtype",             (ks_obj)argtypes[0]},
            {"_argtypes",          (ks_obj)argtypes_tuple},
            {"_fp_meta",           (ks_obj)libc_make_pointer(libc_T_void_p, (void*)fp_meta)},
        ));


        // store it in the cache
        if (!ks_dict_set(function_types, (ks_obj)ofkey, (ks_obj)func_type)) {
            KS_DECREF(func_type);
            KS_DECREF(ofkey);
            return NULL;
        }


    } else {
        func_type = (ks_type)ret;
        if (!ks_type_issub(func_type, libc_T_function)) {
            ks_throw(ks_T_InternalError, "Internally, libc's function type cache contained non-type object: %S", ret);
            KS_DECREF(ret);
            return NULL;
        }
    }

    KS_DECREF(ofkey);
    return func_type;
}


// create a new pointer
libc_pointer libc_make_pointer(ks_type ptr_type, void* addr) {
    if (!ks_type_issub(ptr_type, libc_T_pointer)) return ks_throw(ks_T_TypeError, "libc_make_pointer given ptr_type that is not a pointer! (given: %S)", ptr_type);

    // create a new result
    libc_pointer self = KS_ALLOC_OBJ(libc_pointer);
    KS_INIT_OBJ(self, ptr_type);

    self->val = addr;

    return self;
}

// Create a function pointer
libc_function libc_make_function(ks_type func_type, void (*val)()) {
    if (!ks_type_issub(func_type, libc_T_function)) return ks_throw(ks_T_TypeError, "libc_make_function given func_type that is not a func! (given: %S)", func_type);

    // get meta-data
    libc_pointer fp_meta_obj = (libc_pointer)ks_dict_get_c(func_type->attr, "_fp_meta");
    if (!fp_meta_obj) {
        return ks_throw(ks_T_InternalError, "libc_make_function given func_type that does not have _fp_meta (given: %S)", func_type);
    } else if (!ks_type_issub(fp_meta_obj->type, libc_T_pointer)) {
        ks_throw(ks_T_InternalError, "libc_make_function given func_type._fp_meta that is not a pointer (given: %S)", fp_meta_obj);
        KS_DECREF(fp_meta_obj);
        return NULL;
    }

    // convert to C pointer
    struct libc_fp_meta* fp_meta = (struct libc_fp_meta*)fp_meta_obj->val;


    // create a new result
    libc_function self = KS_ALLOC_OBJ(libc_function);
    KS_INIT_OBJ(self, func_type);

    self->val = val;

    self->fp_meta = fp_meta;

    return self;
}



/* libc.pointer */


// pointer.__new__(obj) -> convert 'obj' to pointer
static KS_TFUNC(pointer, new) {
    ks_type typ;
    ks_obj obj;
    KS_GETARGS("typ:* obj", &typ, ks_T_type, &obj)
    if (!ks_type_issub(typ, libc_T_pointer)) return ks_throw(ks_T_InternalError, "'typ' was not a pointer type!");

    int64_t v64;
    if (ks_type_issub(obj->type, libc_T_pointer)) {
        // cast pointer types
        return (ks_obj)libc_make_pointer(typ, ((libc_pointer)obj)->val);
    } else if (ks_num_get_int64(obj, &v64)) {
        return (ks_obj)libc_make_pointer(typ, (void*)(intptr_t)v64);
    } else {
        ks_catch_ignore();
        KS_THROW_TYPE_ERR(obj, typ);
    }
}

// pointer.__free__(self)
static KS_TFUNC(pointer, free) {
    libc_pointer self;
    KS_GETARGS("self:*", &self, libc_T_pointer)

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// pointer.create(type_of)
static KS_TFUNC(pointer, create) {
    ks_type typ;
    KS_GETARGS("type_of:*", ks_T_type)

    return (ks_obj)libc_make_pointer_type(typ);
}

// pointer.__str__(self)
static KS_TFUNC(pointer, str) {
    libc_pointer self;
    KS_GETARGS("self:*", &self, libc_T_pointer)

    return (ks_obj)ks_fmt_c("(%S)%p", self->type->__name__, self->val);
}

// pointer.__bool__(self)
static KS_TFUNC(pointer, bool) {
    libc_pointer self;
    KS_GETARGS("self:*", &self, libc_T_pointer)
    return (ks_obj)KSO_BOOL(self->val != NULL);
}

// pointer.__int__(self)
static KS_TFUNC(pointer, int) {
    libc_pointer self;
    KS_GETARGS("self:*", &self, libc_T_pointer)

    return (ks_obj)ks_int_new((intptr_t)self->val);
}

// pointer.__getitem__(self, idx)
static KS_TFUNC(pointer, getitem) {
    libc_pointer self;
    int64_t idx;
    KS_GETARGS("self:* idx:i64", &self, libc_T_pointer, &idx)

    // now, switch based on type
    ks_type typ = my_gettypeof(self->type);

    if (typ == libc_T_int) {
        int* addr = &((int*)self->val)[idx];
        KS_DECREF(typ);
        return (ks_obj)libc_make_int(0);
    } else {
        KS_DECREF(typ);
        return ks_throw(ks_T_TodoError, "Need to handle setting pointers of type '%T'", self);
    }
}

// pointer.__setitem__(self, idx, val)
static KS_TFUNC(pointer, setitem) {
    libc_pointer self;
    int64_t idx;
    ks_obj val;
    KS_GETARGS("self:* idx:i64 val", &self, libc_T_pointer, &idx, &val)

    // now, switch based on type
    ks_type typ = my_gettypeof(self->type);

    if (typ == libc_T_int) {
        int* addr = &((int*)self->val)[idx];
        int64_t val64;
        if (!ks_num_get_int64(val, &val64)) {
            KS_DECREF(typ);
            return NULL;
        }
        *addr = (int)val64;

    } else {
        KS_DECREF(typ);
        return ks_throw(ks_T_TodoError, "Need to handle setting pointers of type '%T'", self);
    }

    KS_DECREF(typ);
    return (ks_obj)KS_NEWREF(val);
}


/* function */

// function.__new__(obj) -> convert 'obj' to pointer
static KS_TFUNC(function, new) {
    ks_type typ;
    ks_obj obj;
    KS_GETARGS("typ:* obj", &typ, ks_T_type, &obj)
    if (!ks_type_issub(typ, libc_T_function)) return ks_throw(ks_T_InternalError, "'typ' was not a function type!");


    if (ks_type_issub(obj->type, libc_T_function)) {
        return (ks_obj)libc_make_function(typ, ((libc_function)obj)->val);
    } else if (ks_type_issub(obj->type, libc_T_pointer)) {
        // TODO: perhaps warn if this may cause an error on some platforms?
        return (ks_obj)libc_make_function(typ, (void(*)())((libc_pointer)obj)->val);
    } else {
        ks_catch_ignore();
        KS_THROW_TYPE_ERR(obj, libc_T_function);
    }
}

// function.__free__(self)
static KS_TFUNC(function, free) {
    libc_pointer self;
    KS_GETARGS("self:*", &self, libc_T_pointer)

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// function.__str__(self)
static KS_TFUNC(function, str) {
    libc_pointer self;
    KS_GETARGS("self:*", &self, libc_T_function)

    return (ks_obj)ks_fmt_c("(%S)%p", self->type->__name__, self->val);
}

// function.create(ret_type, arg_types=(,))
static KS_TFUNC(function, create) {
    ks_type ret_type;
    ks_obj arg_types = NULL;
    KS_GETARGS("ret_type:* ?arg_types:iter", &ret_type, ks_T_type, &arg_types)

    if (arg_types != NULL) {
        // handle return & argument types

        ks_list total_types = ks_list_new(1, (ks_obj*)&ret_type);
        if (!ks_list_pushall(total_types, arg_types)) {
            KS_DECREF(total_types);
            return NULL;
        }

        // now, create a new type
        ks_type new_fp_type = libc_make_function_type(total_types->len, (ks_type*)total_types->elems);
        KS_DECREF(total_types);

        return (ks_obj)new_fp_type;

    } else {
        // just handle return type
        return (ks_obj)libc_make_function_type(1, &ret_type);
    }
}


// function.__call__(self, *args)
static KS_TFUNC(function, call) {
    libc_function self;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("self:* *args", &self, libc_T_function, &n_extra, &extra)

    #ifdef KS_HAVE_FFI

    // get meta-data
    struct libc_fp_meta* fp_meta = self->fp_meta;


    // +1,-1 cancel out for including 'self', but not including the return value
    //KS_REQ_N_ARGS(n_args, fp_meta->_ffi_n);

    if (fp_meta->_ffi_n - 1 != n_extra) {
        return ks_throw(ks_T_ArgError, "Given incorrect number of arguments for Cfunc, expected %i, but got %i", fp_meta->_ffi_n - 1, n_extra);
    }


    // now, allocate temporary arrays
    int total_sz = 0, i;
    for (i = 0; i < fp_meta->_ffi_n; ++i) {
        total_sz += fp_meta->_ffi_types[i]->size;
    }

    // heap to allocate buffers off of
    void* f_argdata = ks_malloc(total_sz);

    // pointers to argdata
    void** f_args = ks_malloc(sizeof(void*) * fp_meta->_ffi_n);

    // extra allocation space for temporary sizes
    void** f_args_alloc = ks_malloc(sizeof(void*) * fp_meta->_ffi_n);


    int c_off = 0;

    for (i = 0; i < fp_meta->_ffi_n; ++i) {
        f_args[i] = (void*)((intptr_t)f_argdata + c_off);
        f_args_alloc[i] = NULL;
        c_off += fp_meta->_ffi_types[i]->size;
    }


    // iterate through arguments, and convert each one
    for (i = 1; i < fp_meta->_ffi_n; ++i) {
        if (!my_to_ffitype(fp_meta->_ffi_types[i], args[i], f_args[i], &f_args_alloc[i])) {
            return NULL;
        }
    }


    // actually call it through FFI
    ffi_call(&fp_meta->_ffi_cif, self->val, f_args[0], &f_args[1]);

    // now, free any allocated arguments

    for (i = 0; i < fp_meta->_ffi_n; ++i) {
        if (f_args_alloc[i]) ks_free(f_args_alloc[i]);
    }


    ks_free(f_argdata);
    ks_free(f_args);
    ks_free(f_args_alloc);

    // get return value
    return my_from_ffitype(fp_meta->_ffi_types[0], f_args[0]);

    #else

    return ks_throw(ks_T_InternalError, "Tried to call C function pointer (%S), but `libc` was not compiled with libffi (use `./configure --with-ffi`)", self);

    #endif
}


void libc_init_types() {

    // emit a warning
    if (sizeof(void*) != sizeof(void (*)())) {
        ks_warn("ks", "Size of normal pointers & function pointers differs (%i vs %i)", (int)sizeof(void*), (int)sizeof(void (*)()));
    }

    ks_type_init_c(libc_T_void, "libc.void", ks_T_object, KS_KEYVALS(
        {"_ctype_size",    (ks_obj)ks_int_new(0)},
        {"_is_ctype",      KSO_TRUE},
    ));

    ks_type_init_c(libc_T_pointer, "libc.pointer", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)(F_pointer_new = ks_cfunc_new_c_old(pointer_new_, "libc.pointer.__new__(ptr_type, obj)"))},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(pointer_free_, "libc.pointer.__free__(obj)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c_old(pointer_str_, "libc.pointer.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(pointer_str_, "libc.pointer.__repr__(self)")},
        {"__bool__",               (ks_obj)ks_cfunc_new_c_old(pointer_bool_, "libc.pointer.__bool__(self)")},
        {"__int__",                (ks_obj)ks_cfunc_new_c_old(pointer_int_, "libc.pointer.__int__(self)")},

        {"__getitem__",            (ks_obj)ks_cfunc_new_c_old(pointer_getitem_, "libc.pointer.__getitem__(self, idx)")},
        {"__setitem__",            (ks_obj)ks_cfunc_new_c_old(pointer_setitem_, "libc.pointer.__setitem__(self, idx, val)")},

        {"create",                 (ks_obj)ks_cfunc_new_c_old(pointer_create_, "libc.pointer.create(type_of)")},

        {"_ctype_size",            (ks_obj)ks_int_new(sizeof(void*))},
        {"_is_ctype",              KSO_TRUE},

    ));
    ks_type_init_c(libc_T_function, "libc.function", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)(F_function_new = ks_cfunc_new_c_old(function_new_, "libc.function.__new__(func_type, obj)"))},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(function_free_, "libc.function.__free__(self)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c_old(function_str_, "libc.function.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(function_str_, "libc.function.__repr__(self)")},

        {"__call__",               (ks_obj)ks_cfunc_new_c_old(function_call_, "libc.function.__call__(self, *args)")},

        {"create",                 (ks_obj)ks_cfunc_new_c_old(function_create_, "libc.function.create(ret_type, arg_types=(,))")},

        {"_ctype_size",            (ks_obj)ks_int_new(sizeof(void(*)()))},
        {"_is_ctype",              KSO_TRUE},

    ));


    #define T_setupinttype(_type_name, _ctype) \
    ks_type_init_c(PASTE(libc_T, _type_name), "libc." #_type_name, ks_T_object, KS_KEYVALS( \
        {"__new__",        (ks_obj)ks_cfunc_new_c_old(PASTE(_type_name, new_), "libc." #_type_name ".__new__(obj)")}, \
        {"__free__",       (ks_obj)ks_cfunc_new_c_old(PASTE(_type_name, free_), "libc." #_type_name ".__free__(obj)")}, \
        {"__str__",        (ks_obj)ks_cfunc_new_c_old(PASTE(_type_name, str_), "libc." #_type_name ".__str__(self)")}, \
        {"__repr__",       (ks_obj)ks_cfunc_new_c_old(PASTE(_type_name, str_), "libc." #_type_name ".__repr__(self)")}, \
        {"__int__",        (ks_obj)ks_cfunc_new_c_old(PASTE(_type_name, int_), "libc." #_type_name ".__int__(self)")}, \
        {"_ctype_size",    (ks_obj)ks_int_new(sizeof(_ctype))},\
        {"_is_ctype",      KSO_TRUE}, \
    ));

    T_setupinttype(char, int8_t);
    T_setupinttype(short, int16_t);
    T_setupinttype(int, int32_t);
    T_setupinttype(long, int64_t);

    T_setupinttype(uchar, uint8_t);
    T_setupinttype(ushort, uint16_t);
    T_setupinttype(uint, uint32_t);
    T_setupinttype(ulong, uint64_t);


    // dictionary containing pointer types
    pointer_types = ks_dict_new(0, NULL);
    function_types = ks_dict_new(0, NULL);

    // create common types
    libc_T_void_p = libc_make_pointer_type(libc_T_void);

    libc_T_char_p = libc_make_pointer_type(libc_T_char);
    libc_T_short_p = libc_make_pointer_type(libc_T_short);
    libc_T_int_p = libc_make_pointer_type(libc_T_int);
    libc_T_long_p = libc_make_pointer_type(libc_T_long);

    libc_T_uchar_p = libc_make_pointer_type(libc_T_uchar);
    libc_T_ushort_p = libc_make_pointer_type(libc_T_ushort);
    libc_T_uint_p = libc_make_pointer_type(libc_T_uint);
    libc_T_ulong_p = libc_make_pointer_type(libc_T_ulong);

}


