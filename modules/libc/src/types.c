/* src/types.c - implementation of the Ctypes bindings
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "../kslibc.h"


KS_TYPE_DECLFWD(libc_type_void);

KS_TYPE_DECLFWD(libc_type_char);
KS_TYPE_DECLFWD(libc_type_short);
KS_TYPE_DECLFWD(libc_type_int);
KS_TYPE_DECLFWD(libc_type_long);

KS_TYPE_DECLFWD(libc_type_uchar);
KS_TYPE_DECLFWD(libc_type_ushort);
KS_TYPE_DECLFWD(libc_type_uint);
KS_TYPE_DECLFWD(libc_type_ulong);


// templated types
KS_TYPE_DECLFWD(libc_type_pointer);
KS_TYPE_DECLFWD(libc_type_function);


// pointers
KS_TYPE_DECLFWD(libc_type_void_p);

KS_TYPE_DECLFWD(libc_type_char_p);
KS_TYPE_DECLFWD(libc_type_short_p);
KS_TYPE_DECLFWD(libc_type_int_p);
KS_TYPE_DECLFWD(libc_type_long_p);

KS_TYPE_DECLFWD(libc_type_uchar_p);
KS_TYPE_DECLFWD(libc_type_ushort_p);
KS_TYPE_DECLFWD(libc_type_uint_p);
KS_TYPE_DECLFWD(libc_type_ulong_p);


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
    if (kstype == libc_type_void || kstype == ks_type_none) {
        return &ffi_type_void;
    } else if (kstype == libc_type_int || kstype == ks_type_int) {
        return &ffi_type_sint;
    } else if (ks_type_issub(kstype, libc_type_pointer)) {
        return &ffi_type_pointer;
    } else {
        ks_throw_fmt(ks_type_TypeError, "Cannot convert type '%S' into a compatible FFI type", kstype);
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
        if (obj->type == ks_type_none) {
            return true;
        }
        
        ks_throw_fmt(ks_type_TypeError, "'%T' object cannot tbe turned into a C 'void' object", obj);
        return false;

    } else if (totype == &ffi_type_sint32) {

        int64_t v64;
        if(ks_num_get_int64(obj, &v64)) {
            *(int32_t*)dest = v64;
            return true;
        }
        
        ks_throw_fmt(ks_type_TypeError, "'%T' object cannot tbe turned into a C 'int' object", obj);
        return false;

    } else if (totype == &ffi_type_pointer) {

        if (ks_type_issub(obj->type, libc_type_pointer)) {

            *(void**)dest = ((libc_pointer)obj)->val;
            return true;
        } else if (obj->type == ks_type_str) {
            *alloc = ks_malloc(((ks_str)obj)->len + 1);

            // create NUL-terminated string
            memcpy(*alloc, ((ks_str)obj)->chr, ((ks_str)obj)->len);
            ((char*)*alloc)[((ks_str)obj)->len] = '\0';


            // return results
            *(void**)dest = *alloc;
            return true;
        }

        ks_throw_fmt(ks_type_TypeError, "'%T' object cannot tbe turned into a C pointer object", obj);
        return false;
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

    ks_obj ret = ks_dict_get(pointer_types, (ks_obj)of);

    // type to return
    ks_type ptr_type = NULL;


    if (ret == NULL) {

        // create a name
        ks_str ptr_type_name = ks_fmt_c("%S*", of->__name__);

        // we need to create type from scratch
        ptr_type = KS_ALLOC_OBJ(ks_type);
        KS_INIT_TYPE_OBJ(ptr_type, ptr_type_name->chr);
        KS_DECREF(ptr_type_name);

        // add parent type
        ks_type_add_parent(ptr_type, libc_type_pointer);

        // set a reference to the base type
        if (!ks_type_set_c(ptr_type, "_base_type", (ks_obj)of)) {
            KS_DECREF(ptr_type);
            return NULL;
        }
        
        // wrap constructor
        ks_pfunc ptr_type_new = ks_pfunc_new2((ks_obj)F_pointer_new, (ks_obj)ptr_type);

        if (!ks_type_set_c(ptr_type, "__new__", (ks_obj)ptr_type_new)) {
            KS_DECREF(ptr_type_new);
            KS_DECREF(ptr_type);
            return NULL;
        }

        KS_DECREF(ptr_type_new);

        // store it in the cache
        if (!ks_dict_set(pointer_types, (ks_obj)of, (ks_obj)ptr_type)) {
            KS_DECREF(ptr_type);
            return NULL;
        }

    } else {
        // just assign pointer type
        ptr_type = (ks_type)ret;
        if (!ks_type_issub(ptr_type, libc_type_pointer)) {
            ks_throw_fmt(ks_type_InternalError, "Internally, libc's pointer type cache contained non-type object: %S", ret);
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

        ks_str_b SB;
        ks_str_b_init(&SB);

        ks_str_b_add_fmt(&SB, "%S (*)(", argtypes[0]);

        int i;
        for (i = 1; i < n_args; ++i) {
            if (i > 1) ks_str_b_add_c(&SB, ", ");
            ks_str_b_add_fmt(&SB, "%S", argtypes[i]);
        }

        ks_str_b_add_fmt(&SB, ")");

        ks_str func_type_name = ks_str_b_get(&SB);

        ks_str_b_free(&SB);


        // we need to create type from scratch
        func_type = KS_ALLOC_OBJ(ks_type);
        KS_INIT_TYPE_OBJ(func_type, func_type_name->chr);
        KS_DECREF(func_type_name);

        // add parent type
        ks_type_add_parent(func_type, libc_type_function);

        // set a reference to the base type
        if (!ks_type_set_c(func_type, "_rtype", (ks_obj)argtypes[0])) {
            KS_DECREF(func_type);
            KS_DECREF(ofkey);
            return NULL;
        }


        ks_tuple argtypes_tuple = ks_tuple_new(n_args - 1, (ks_obj*)(argtypes + 1));
        
        // set a reference to the argument types
        if (!ks_type_set_c(func_type, "_argtypes", (ks_obj)argtypes_tuple)) {
            KS_DECREF(argtypes_tuple);
            KS_DECREF(func_type);
            KS_DECREF(ofkey);
            return NULL;
        }
        KS_DECREF(argtypes_tuple);


        // wrap constructor
        ks_pfunc func_type_new = ks_pfunc_new2((ks_obj)F_function_new, (ks_obj)func_type);

        if (!ks_type_set_c(func_type, "__new__", (ks_obj)func_type_new)) {
            KS_DECREF(func_type_new);
            KS_DECREF(func_type);
            KS_DECREF(ofkey);
            return NULL;
        }


        KS_DECREF(func_type_new);


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
            ks_throw_fmt(ks_type_InternalError, "Internal error prepping CIF for FFI");
            KS_DECREF(ofkey);
            KS_DECREF(func_type);
            ks_free(fp_meta->_ffi_types);
            return NULL;
        }
        #else
        // perhaps warn here?
        fp_meta->_ffi_types = NULL;

        #endif

        // this object can be unboxed elsewhere
        ks_obj fp_meta_obj = (ks_obj)libc_make_pointer(libc_type_void_p, (void*)fp_meta);
        if (!fp_meta_obj) {
            KS_DECREF(ofkey);
            KS_DECREF(func_type);
            ks_free(fp_meta->_ffi_types);
            KS_DECREF(fp_meta_obj);
            return NULL;
        }


        // set it on the type as an internal variable
        if (!ks_type_set_c(func_type, "_fp_meta", (ks_obj)fp_meta_obj)) {
            KS_DECREF(ofkey);
            KS_DECREF(func_type);
            ks_free(fp_meta->_ffi_types);
            KS_DECREF(fp_meta_obj);
            return NULL;
        }

        KS_DECREF(fp_meta_obj);

        // store it in the cache
        if (!ks_dict_set(function_types, (ks_obj)ofkey, (ks_obj)func_type)) {
            KS_DECREF(func_type);
            KS_DECREF(ofkey);
            return NULL;
        }



    } else {
        func_type = (ks_type)ret;
        if (!ks_type_issub(func_type, libc_type_function)) {
            ks_throw_fmt(ks_type_InternalError, "Internally, libc's function type cache contained non-type object: %S", ret);
            KS_DECREF(ret);
            return NULL;
        }
    }

    KS_DECREF(ofkey);
    return func_type;
}


// create a new pointer
libc_pointer libc_make_pointer(ks_type ptr_type, void* addr) {
    if (!ks_type_issub(ptr_type, libc_type_pointer)) return ks_throw_fmt(ks_type_TypeError, "libc_make_pointer given ptr_type that is not a pointer! (given: %S)", ptr_type);

    // create a new result
    libc_pointer self = KS_ALLOC_OBJ(libc_pointer);
    KS_INIT_OBJ(self, ptr_type);

    self->val = addr;

    return self;
}

// Create a function pointer
libc_function libc_make_function(ks_type func_type, void (*val)()) {
    if (!ks_type_issub(func_type, libc_type_function)) return ks_throw_fmt(ks_type_TypeError, "libc_make_function given func_type that is not a func! (given: %S)", func_type);

    // get meta-data
    libc_pointer fp_meta_obj = (libc_pointer)ks_dict_get_c(func_type->attr, "_fp_meta");
    if (!fp_meta_obj) {
        return ks_throw_fmt(ks_type_InternalError, "libc_make_function given func_type that does not have _fp_meta (given: %S)", func_type);
    } else if (!ks_type_issub(fp_meta_obj->type, libc_type_pointer)) {
        ks_throw_fmt(ks_type_InternalError, "libc_make_function given func_type._fp_meta that is not a pointer (given: %S)", fp_meta_obj);
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


// pointer.__new__(ptr_type, obj) -> convert 'obj' to pointer
static KS_TFUNC(pointer, new) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type ptr_type = (ks_type)args[0];
    if (!ks_type_issub(ptr_type, libc_type_pointer)) return ks_throw_fmt(ks_type_InternalError, "ptr_type was not a pointer type!");
    ks_obj obj = args[1];

    int64_t v64;
    if (ks_type_issub(obj->type, libc_type_pointer)) {
        // cast pointer types
        return (ks_obj)libc_make_pointer(ptr_type, ((libc_pointer)obj)->val);
    } else if (ks_num_get_int64(obj, &v64)) {
        return (ks_obj)libc_make_pointer(ptr_type, (void*)(intptr_t)v64);
    } else {
        ks_catch_ignore();
        KS_ERR_CONV(obj, ptr_type);
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


/* function */

// function.__new__(func_type, obj) -> convert 'obj' to pointer
static KS_TFUNC(function, new) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type func_type = (ks_type)args[0];
    if (!ks_type_issub(func_type, libc_type_function)) return ks_throw_fmt(ks_type_InternalError, "func_type was not a function type!");
    ks_obj obj = args[1];

    if (ks_type_issub(obj->type, libc_type_function)) {
        return (ks_obj)libc_make_function(func_type, ((libc_function)obj)->val);
    } else if (ks_type_issub(obj->type, libc_type_pointer)) {
        // TODO: perhaps warn if this may cause an error on some platforms?
        return (ks_obj)libc_make_function(func_type, (void(*)())((libc_pointer)obj)->val);
    } else {
        ks_catch_ignore();
        KS_ERR_CONV(obj, libc_type_function);
    }
}

// function.__free__(self)
static KS_TFUNC(function, free) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_function self = (libc_function)args[0];
    KS_REQ_TYPE(self, libc_type_function, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// function.__str__(self)
static KS_TFUNC(function, str) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_function self = (libc_function)args[0];
    KS_REQ_TYPE(self, libc_type_function, "self");

    return (ks_obj)ks_fmt_c("(%S)%p", self->type->__name__, self->val);
}

// function.make(ret_type, arg_types=(,))
static KS_TFUNC(function, make) {
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
        ks_type new_fp_type = libc_make_function_type(total_n_args, all_types);

        ks_free(all_types);
        KS_DECREF(list_arg_types);

        return (ks_obj)new_fp_type;

    } else {
        // just handle return type
        return (ks_obj)libc_make_function_type(1, &ret_type);
    }
}


// function.__call__(self, *args)
static KS_TFUNC(function, call) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    libc_function self = (libc_function)args[0];
    KS_REQ_TYPE(self, libc_type_function, "self");
    #ifdef KS_HAVE_FFI

    // get meta-data
    struct libc_fp_meta* fp_meta = self->fp_meta;



    // +1,-1 cancel out for including 'self', but not including the return value
    KS_REQ_N_ARGS(n_args, fp_meta->_ffi_n);


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

    return ks_throw_fmt(ks_type_InternalError, "Tried to call C function pointer (%S), but `libc` was not compiled with libffi (use `./configure --with-ffi`)", self);

    #endif
}


void libc_init_types() {

    // emit a warning
    if (sizeof(void*) != sizeof(void (*)())) {
        ks_warn("Size of normal pointers & function pointers differs (%i vs %i)", (int)sizeof(void*), (int)sizeof(void (*)()));
    }


    KS_INIT_TYPE_OBJ(libc_type_void, "libc.void");

    KS_INIT_TYPE_OBJ(libc_type_char, "libc.char");
    KS_INIT_TYPE_OBJ(libc_type_short, "libc.short");
    KS_INIT_TYPE_OBJ(libc_type_int, "libc.int");
    KS_INIT_TYPE_OBJ(libc_type_long, "libc.long");
    KS_INIT_TYPE_OBJ(libc_type_uchar, "libc.uchar");
    KS_INIT_TYPE_OBJ(libc_type_ushort, "libc.ushort");
    KS_INIT_TYPE_OBJ(libc_type_uint, "libc.uint");
    KS_INIT_TYPE_OBJ(libc_type_ulong, "libc.ulong");

    KS_INIT_TYPE_OBJ(libc_type_pointer, "libc.pointer");
    KS_INIT_TYPE_OBJ(libc_type_function, "libc.function");


    ks_type_set_cn(libc_type_void, (ks_dict_ent_c[]){

        {"_ctype_size",    (ks_obj)ks_int_new(0)},
        {"_is_ctype",      KSO_TRUE},

        {NULL, NULL},
    });




    #define T_setupinttype(_type_name, _ctype) \
    ks_type_set_cn(PASTE(libc_type, _type_name), (ks_dict_ent_c[]){ \
        {"__new__",        (ks_obj)ks_cfunc_new2(PASTE(_type_name, new_), "libc." #_type_name ".__new__(obj)")}, \
        {"__free__",       (ks_obj)ks_cfunc_new2(PASTE(_type_name, free_), "libc." #_type_name ".__free__(obj)")}, \
        {"__str__",        (ks_obj)ks_cfunc_new2(PASTE(_type_name, str_), "libc." #_type_name ".__str__(self)")}, \
        {"__repr__",       (ks_obj)ks_cfunc_new2(PASTE(_type_name, str_), "libc." #_type_name ".__repr__(self)")}, \
        {"__int__",        (ks_obj)ks_cfunc_new2(PASTE(_type_name, int_), "libc." #_type_name ".__int__(self)")}, \
        {"_ctype_size",    (ks_obj)ks_int_new(sizeof(_ctype))},\
        {"_is_ctype",      KSO_TRUE}, \
        {NULL, NULL}, \
    });

    T_setupinttype(char, int8_t);
    T_setupinttype(short, int16_t);
    T_setupinttype(int, int32_t);
    T_setupinttype(long, int64_t);

    T_setupinttype(uchar, uint8_t);
    T_setupinttype(ushort, uint16_t);
    T_setupinttype(uint, uint32_t);
    T_setupinttype(ulong, uint64_t);



    ks_type_set_cn(libc_type_pointer, (ks_dict_ent_c[]){

        {"__new__",          (ks_obj)(F_pointer_new = ks_cfunc_new2(pointer_new_, "libc.pointer.__new__(ptr_type, obj)"))},
        {"__free__",        (ks_obj)ks_cfunc_new2(pointer_free_, "libc.pointer.__free__(obj)")},

        {"__str__",        (ks_obj)ks_cfunc_new2(pointer_str_, "libc.pointer.__str__(self)")},
        {"__repr__",       (ks_obj)ks_cfunc_new2(pointer_str_, "libc.pointer.__repr__(self)")},
        {"__bool__",        (ks_obj)ks_cfunc_new2(pointer_bool_, "libc.pointer.__bool__(self)")},
        {"__int__",        (ks_obj)ks_cfunc_new2(pointer_int_, "libc.pointer.__int__(self)")},

        {"__getitem__",        (ks_obj)ks_cfunc_new2(pointer_getitem_, "libc.pointer.__getitem__(self, idx)")},
        {"__setitem__",        (ks_obj)ks_cfunc_new2(pointer_setitem_, "libc.pointer.__setitem__(self, idx, val)")},

        {"create",             (ks_obj)ks_cfunc_new2(pointer_create_, "libc.pointer.create(type_of)")},

        {"_ctype_size",      (ks_obj)ks_int_new(sizeof(void*))},
        {"_is_ctype",        KSO_TRUE},

        {NULL, NULL},
    });

    ks_type_set_cn(libc_type_function, (ks_dict_ent_c[]){

        {"__new__",          (ks_obj)(F_function_new = ks_cfunc_new2(function_new_, "libc.function.__new__(func_type, obj)"))},
        {"__free__",         (ks_obj)ks_cfunc_new2(function_free_, "libc.function.__free__(self)")},

        {"__str__",          (ks_obj)ks_cfunc_new2(function_str_, "libc.function.__str__(self)")},
        {"__repr__",         (ks_obj)ks_cfunc_new2(function_str_, "libc.function.__repr__(self)")},

        {"__call__",         (ks_obj)ks_cfunc_new2(function_call_, "libc.function.__call__(self, *args)")},

        {"make",             (ks_obj)ks_cfunc_new2(function_make_, "libc.function.make(ret_type, arg_types=(,))")},

        {"_ctype_size",      (ks_obj)ks_int_new(sizeof(void(*)()))},
        {"_is_ctype",        KSO_TRUE},

        {NULL, NULL},
    });



    // dictionary containing pointer types
    pointer_types = ks_dict_new(0, NULL);
    function_types = ks_dict_new(0, NULL);

    // create common types
    libc_type_void_p = libc_make_pointer_type(libc_type_void);

    libc_type_char_p = libc_make_pointer_type(libc_type_char);
    libc_type_short_p = libc_make_pointer_type(libc_type_short);
    libc_type_int_p = libc_make_pointer_type(libc_type_int);
    libc_type_long_p = libc_make_pointer_type(libc_type_long);

    libc_type_uchar_p = libc_make_pointer_type(libc_type_uchar);
    libc_type_ushort_p = libc_make_pointer_type(libc_type_ushort);
    libc_type_uint_p = libc_make_pointer_type(libc_type_uint);
    libc_type_ulong_p = libc_make_pointer_type(libc_type_ulong);

}


