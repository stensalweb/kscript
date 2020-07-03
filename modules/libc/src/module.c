/* libc/src/module.c - main implementation of C library bindings
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "libc"

// include this since this is a module.
#include "ks-module.h"

#include "../libc.h"


#include <dlfcn.h>
#include <errno.h>



/* general */

static int my_errno = 0;


// libc.errno()
static KS_TFUNC(libc, errno) {
    KS_REQ_N_ARGS(n_args, 0);

    return (ks_obj)ks_int_new(my_errno);
}


// libc.strerror(errnum)
static KS_TFUNC(libc, strerror) {
    KS_REQ_N_ARGS(n_args, 1);
    int64_t errnum;
    if (!ks_parse_params(n_args, args, "errnum%i64", &errnum)) return NULL;

    char* r = strerror(errnum);
    my_errno = errno;

    return (ks_obj)ks_str_new(r);
}


/* basic functions */


// libc.malloc(sz) -> allocates memory
static KS_TFUNC(libc, malloc) {
    KS_REQ_N_ARGS(n_args, 1);
    int64_t sz;
    if (!ks_parse_params(n_args, args, "sz%i64", &sz)) return NULL;

    void* res = malloc(sz);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(ks_type_none, res);
}

// libc.realloc(ptr, sz) -> re-allocates memory
static KS_TFUNC(libc, realloc) {
    KS_REQ_N_ARGS(n_args, 2);
    libc_pointer ptr;
    int64_t sz;
    if (!ks_parse_params(n_args, args, "ptr%any sz%i64", &ptr, &sz)) return NULL;
    
    void* ptr_c = NULL;
    if (ptr->type == ks_type_none) {
        // do nothing; assume NULL
    } else if (ks_type_issub(ptr->type, libc_type_pointer)) {
        ptr_c = ptr->val;
    } else {
        KS_ERR_CONV(ptr, libc_type_pointer);
    }

    void* res = realloc(ptr_c, sz);
    my_errno = errno;

    if (res == ptr_c && ks_type_issub(ptr->type, libc_type_pointer)) {
        return KS_NEWREF(ptr);
    } else {
        return (ks_obj)libc_make_pointer(ks_type_none, res);
    }
}

// libc.free(ptr, sz) -> frees memory
static KS_TFUNC(libc, free) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_pointer ptr;
    if (!ks_parse_params(n_args, args, "ptr%any", &ptr)) return NULL;
    
    if (ptr->type == ks_type_none) {
        // do nothing; assume NULL
        my_errno = errno;
    } else if (ks_type_issub(ptr->type, libc_type_pointer)) {
        free(ptr->val);
        my_errno = errno;

    } else {
        KS_ERR_CONV(ptr, libc_type_pointer);
    }

    return KSO_NONE;
}



/* -ldl, dynamic linking */


// libc.dlopen(fname, flags=libc.RTLD_LAZY) -> open dynamic library
static KS_TFUNC(libc, dlopen) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_str fname;
    int64_t flags = RTLD_LAZY;
    if (!ks_parse_params(n_args, args, "fname%s ?flags%i64", &fname, &flags)) return NULL;

    // TODO: add flags
    void* res = dlopen(fname->chr, flags);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(ks_type_none, res);
}


// libc.dlclose(handle) -> close dynamic library
static KS_TFUNC(libc, dlclose) {
    KS_REQ_N_ARGS(n_args, 1);
    libc_pointer handle;
    if (!ks_parse_params(n_args, args, "handle%*", &handle, libc_type_pointer)) return NULL;

    // TODO: add flags
    int res = dlclose(handle->val);
    my_errno = errno;

    return (ks_obj)ks_int_new(res);
}

// libc.dlerror() -> return error name
static KS_TFUNC(libc, dlerror) {
    KS_REQ_N_ARGS(n_args, 0);

    char* r = dlerror();
    my_errno = errno;

    return (ks_obj)ks_str_new(r);
}

// libc.dlsym(handle, symbol) -> locate symbol
static KS_TFUNC(libc, dlsym) {
    KS_REQ_N_ARGS(n_args, 2);
    libc_pointer handle;
    ks_str symbol;
    if (!ks_parse_params(n_args, args, "handle%* symbol%s", &handle, libc_type_pointer, &symbol)) return NULL;

    // TODO: add flags
    void* res = dlsym(handle->val, symbol->chr);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(ks_type_none, res);
}


/* misc */

// libc.sizeof(obj) -> return size
static KS_TFUNC(libc, size) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];

    ks_ssize_t sz = libc_get_size(obj->type == ks_type_type ? (ks_type)obj : obj->type);

    if (sz < 0) return NULL;
    else return (ks_obj)ks_int_new(sz);
}





// now, export them all
static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME);

    libc_init_types();


    ks_type E_rtld = ks_Enum_create_c("RtldFlags", (struct ks_enum_entry_c[]){

        KS_EEF(RTLD_LAZY),
        KS_EEF(RTLD_NOW),
        KS_EEF(RTLD_BINDING_MASK),
        KS_EEF(RTLD_NOLOAD),
        KS_EEF(RTLD_DEEPBIND),
        KS_EEF(RTLD_GLOBAL),
        KS_EEF(RTLD_LOCAL),
        KS_EEF(RTLD_NODELETE),

        {NULL, -1}
    });

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]) {
        
        /* int types */
        {"char",                  (ks_obj)libc_type_char},
        {"short",                 (ks_obj)libc_type_short},
        {"int",                   (ks_obj)libc_type_int},
        {"long",                  (ks_obj)libc_type_long},

        {"uchar",                 (ks_obj)libc_type_uchar},
        {"ushort",                (ks_obj)libc_type_ushort},
        {"uint",                  (ks_obj)libc_type_uint},
        {"ulong",                 (ks_obj)libc_type_ulong},


        /* int* types */

        {"char_p",                (ks_obj)libc_make_pointer_type(libc_type_char)},
        {"short_p",               (ks_obj)libc_make_pointer_type(libc_type_short)},
        {"int_p",                 (ks_obj)libc_make_pointer_type(libc_type_int)},
        {"long_p",                (ks_obj)libc_make_pointer_type(libc_type_long)},

        {"uchar_p",                (ks_obj)libc_make_pointer_type(libc_type_uchar)},
        {"ushort_p",               (ks_obj)libc_make_pointer_type(libc_type_ushort)},
        {"uint_p",                 (ks_obj)libc_make_pointer_type(libc_type_uint)},
        {"ulong_p",                (ks_obj)libc_make_pointer_type(libc_type_ulong)},

        /* templates */

        {"pointer",             (ks_obj)libc_type_pointer},
        {"func_pointer",        (ks_obj)libc_type_func_pointer},

        
        /* misc/extra functions */

        {"sizeof",              (ks_obj)ks_cfunc_new2(libc_size_, "libc.sizeof(obj)")},


        /* enums */

        {"RtldFlags",        (ks_obj)E_rtld},


        /* general */

        {"errno",           (ks_obj)ks_cfunc_new2(libc_errno_, "libc.errno()")},
        {"strerror",        (ks_obj)ks_cfunc_new2(libc_strerror_, "libc.strerr(errnum)")},


        /* memory routines */

        {"malloc",        (ks_obj)ks_cfunc_new2(libc_malloc_, "libc.malloc(sz)")},
        {"realloc",       (ks_obj)ks_cfunc_new2(libc_realloc_, "libc.realloc(ptr, sz)")},
        {"free",          (ks_obj)ks_cfunc_new2(libc_free_, "libc.free(ptr)")},


        /* dynamic linking */

        {"dlopen",         (ks_obj)ks_cfunc_new2(libc_dlopen_, "libc.dlopen(fname, flags=libc.RTLD_LAZY)")},
        {"dlclose",         (ks_obj)ks_cfunc_new2(libc_dlclose_, "libc.dlclose(handle)")},
        {"dlsym",          (ks_obj)ks_cfunc_new2(libc_dlsym_, "libc.dlsym(handle, symbol)")},
        {"dlerror",        (ks_obj)ks_cfunc_new2(libc_dlerror_, "libc.dlerror()")},


        {"NULL",           (ks_obj)libc_make_pointer(ks_type_none, NULL)},

        {NULL, NULL}
    });


    ks_module_add_enum_members(mod, E_rtld);

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
