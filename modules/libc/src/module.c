/* libc/src/module.c - main implementation of C library bindings
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "libc"

// include this since this is a module.
#include "ks-module.h"

#include "../kslibc-impl.h"


#include <dlfcn.h>
#include <errno.h>



/* general */

static int my_errno = 0;


// libc.errno()
static KS_TFUNC(libc, errno) {
    KS_GETARGS("")

    return (ks_obj)ks_int_new(my_errno);
}


// libc.strerror(errnum)
static KS_TFUNC(libc, strerror) {
    int64_t errnum;
    KS_GETARGS("errnum:i64", &errnum)

    char* r = strerror(errnum);
    my_errno = errno;

    return (ks_obj)ks_str_new(r);
}


/* basic functions */


// libc.malloc(sz) -> allocates memory
static KS_TFUNC(libc, malloc) {
    int64_t sz;
    KS_GETARGS("sz:i64", &sz)

    void* res = malloc(sz);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(libc_T_void_p, res);
}

// libc.realloc(ptr, sz) -> re-allocates memory
static KS_TFUNC(libc, realloc) {
    libc_pointer ptr;
    int64_t sz;
    KS_GETARGS("ptr:* sz:i64", &ptr, libc_T_pointer, &sz)

    void* res = realloc(ptr->val, sz);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(libc_T_void_p, res);
}

// libc.free(ptr, sz) -> frees memory
static KS_TFUNC(libc, free) {
    libc_pointer ptr;
    KS_GETARGS("ptr:*", &ptr, libc_T_pointer)

    free(ptr->val);
    my_errno = errno;

    return KSO_NONE;
}



/* -ldl, dynamic linking */


// libc.dlopen(fname, flags=libc.RTLD_LAZY) -> open dynamic library
static KS_TFUNC(libc, dlopen) {
    ks_str fname;
    int64_t flags = RTLD_LAZY;
    KS_GETARGS("fname:* ?flags:i64", &fname, ks_T_str, &flags);

    // TODO: add flags
    void* res = dlopen(fname->chr, flags);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(libc_T_void_p, res);
}


// libc.dlclose(handle) -> close dynamic library
static KS_TFUNC(libc, dlclose) {
    libc_pointer handle;
    KS_GETARGS("handle:*", &handle, libc_T_pointer);

    // TODO: add flags
    int res = dlclose(handle->val);
    my_errno = errno;

    return (ks_obj)ks_int_new(res);
}

// libc.dlerror() -> return error name
static KS_TFUNC(libc, dlerror) {
    KS_GETARGS("")

    char* r = dlerror();
    my_errno = errno;

    return (ks_obj)ks_str_new(r);
}

// libc.dlsym(handle, symbol) -> locate symbol
static KS_TFUNC(libc, dlsym) {
    libc_pointer handle;
    ks_str symbol;
    KS_GETARGS("handle:* symbol:*", &handle, libc_T_pointer, &symbol, ks_T_str);

    // TODO: add flags
    void* res = dlsym(handle->val, symbol->chr);
    my_errno = errno;

    return (ks_obj)libc_make_pointer(libc_T_void_p, res);
}


/* misc */

// libc.sizeof(obj) -> return size
static KS_TFUNC(libc, size) {
    ks_obj obj;
    KS_GETARGS("obj", &obj);

    ks_ssize_t sz = libc_get_size(obj->type == ks_T_type ? (ks_type)obj : obj->type);

    if (sz < 0) return NULL;
    else return (ks_obj)ks_int_new(sz);
}



// now, export them all
static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME);

    libc_init_types();

    ks_type E_rtld = ks_Enum_create_c("RtldFlags", KS_ENUMVALS(

        KS_EEF(RTLD_LAZY),
        KS_EEF(RTLD_NOW),
        
        #if !defined(KS__WINDOWS) && !defined(KS__CYGWIN)
        KS_EEF(RTLD_BINDING_MASK),
        #endif

        KS_EEF(RTLD_NOLOAD),
        KS_EEF(RTLD_DEEPBIND),
        KS_EEF(RTLD_GLOBAL),
        KS_EEF(RTLD_LOCAL),
        KS_EEF(RTLD_NODELETE),

    ));

    
    ks_dict_set_c(mod->attr, KS_KEYVALS(
        
        /* simple C types */

        {"void",                     (ks_obj)libc_T_void},

        {"char",                     (ks_obj)libc_T_char},
        {"short",                    (ks_obj)libc_T_short},
        {"int",                      (ks_obj)libc_T_int},
        {"long",                     (ks_obj)libc_T_long},
   
        {"uchar",                    (ks_obj)libc_T_uchar},
        {"ushort",                   (ks_obj)libc_T_ushort},
        {"uint",                     (ks_obj)libc_T_uint},
        {"ulong",                  (ks_obj)libc_T_ulong},


        /* common pointer types */

        {"void_p",                   (ks_obj)libc_T_void_p},

        {"char_p",                   (ks_obj)libc_T_char_p},
        {"short_p",                  (ks_obj)libc_T_short_p},
        {"int_p",                    (ks_obj)libc_T_int_p},
        {"long_p",                   (ks_obj)libc_T_long_p},

        {"uchar_p",                  (ks_obj)libc_T_uchar_p},
        {"ushort_p",                 (ks_obj)libc_T_ushort_p},
        {"uint_p",                   (ks_obj)libc_T_uint_p},
        {"ulong_p",                  (ks_obj)libc_T_ulong_p},

        /* templates */

        {"pointer",              (ks_obj)libc_T_pointer},
        {"function",             (ks_obj)libc_T_function},

        
        /* misc/extra functions */

        {"sizeof",              (ks_obj)ks_cfunc_new_c_old(libc_size_, "libc.sizeof(obj)")},


        /* enums */

        {"RtldFlags",        (ks_obj)E_rtld},


        /* general */

        {"errno",           (ks_obj)ks_cfunc_new_c_old(libc_errno_, "libc.errno()")},
        {"strerror",        (ks_obj)ks_cfunc_new_c_old(libc_strerror_, "libc.strerr(errnum)")},


        /* memory routines */

        {"malloc",        (ks_obj)ks_cfunc_new_c_old(libc_malloc_, "libc.malloc(sz)")},
        {"realloc",       (ks_obj)ks_cfunc_new_c_old(libc_realloc_, "libc.realloc(ptr, sz)")},
        {"free",          (ks_obj)ks_cfunc_new_c_old(libc_free_, "libc.free(ptr)")},


        /* dynamic linking */

        {"dlopen",         (ks_obj)ks_cfunc_new_c_old(libc_dlopen_, "libc.dlopen(fname, flags=libc.RTLD_LAZY)")},
        {"dlclose",         (ks_obj)ks_cfunc_new_c_old(libc_dlclose_, "libc.dlclose(handle)")},
        {"dlsym",          (ks_obj)ks_cfunc_new_c_old(libc_dlsym_, "libc.dlsym(handle, symbol)")},
        {"dlerror",        (ks_obj)ks_cfunc_new_c_old(libc_dlerror_, "libc.dlerror()")},


        {"NULL",           (ks_obj)libc_make_pointer(libc_T_void_p, NULL)},

    ));


    ks_dict_merge_enum(mod->attr, E_rtld);


    //ks_module_add_enum_members(mod, E_rtld);

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
