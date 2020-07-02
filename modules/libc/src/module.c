/* libc/src/module.c - main implementation of C library bindings
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "libc"

// include this since this is a module.
#include "ks-module.h"

#include "../libc.h"


/* basic functions */


// libc.malloc(sz) -> allocates memory
static KS_TFUNC(libc, malloc) {
    KS_REQ_N_ARGS(n_args, 1);
    int64_t sz;
    if (!ks_parse_params(n_args, args, "sz%i64", &sz)) return NULL;

    void* res = malloc(sz);

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
    } else if (ks_type_issub(ptr->type, libc_type_pointer)) {
        free(ptr->val);
    } else {
        KS_ERR_CONV(ptr, libc_type_pointer);
    }

    return KSO_NONE;
}



// libc.sizeof(obj) -> return size
static KS_TFUNC(libc, size) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];

    ks_ssize_t sz = libc_get_size(obj->type == ks_type_type ? (ks_type)obj : obj->type);

    if (sz < 0) return NULL;
    else return ks_int_new(sz);
}


// now, export them all
static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME);

    libc_init_types();

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]) {
        
        {"int",            (ks_obj)libc_type_int},
        {"pointer",        (ks_obj)libc_type_pointer},

        {"int_p",          (ks_obj)libc_make_pointer_type(libc_type_int)},
        
        {"sizeof",          (ks_obj)ks_cfunc_new2(libc_size_, "libc.sizeof(obj)")},


        /* memory routines */

        {"malloc",        (ks_obj)ks_cfunc_new2(libc_malloc_, "libc.malloc(sz)")},
        {"realloc",       (ks_obj)ks_cfunc_new2(libc_realloc_, "libc.realloc(ptr, sz)")},
        {"free",          (ks_obj)ks_cfunc_new2(libc_free_, "libc.free(ptr)")},

        {"NULL",           (ks_obj)libc_make_pointer(ks_type_none, NULL)},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
