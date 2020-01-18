/* types/module.c - represents a module-type, and module loading utilities*/

#include "ks_common.h"

// for plugin loading
#include <dlfcn.h>


// the search paths for modules, should be a list of strings
ks_list ksm_search_path = NULL;

// create a new module with a given name
ks_module ks_module_new(ks_str name) {
    ks_module self = (ks_module)ks_malloc(sizeof(*self));
    *self = (struct ks_module) {
        KSO_BASE_INIT(ks_T_module)
        .name = name,
        .__dict__ = ks_dict_new_empty()
    };

    KSO_INCREF(name);

    // return our constructed integer
    return self;
}

// create a new module with a given name, for a C module
ks_module ks_module_new_c(char* name) {
    ks_module self = (ks_module)ks_malloc(sizeof(*self));
    *self = (struct ks_module) {
        KSO_BASE_INIT(ks_T_module)
        .name = ks_str_new(name),
        .__dict__ = ks_dict_new_empty()
    };

    // return our constructed integer
    return self;
}


// internal function to attempt a load
static ks_module attempt_load(ks_str fname) {

    // first ensure the extension is correct
    char* ext = strrchr(fname->chr, '.');
    if (ext == NULL) {
        ks_debug("[LOAD_MODULE] file '%S' didn't work; not a valid extension", fname);
        return NULL;
    }

    if (strcmp(ext, ".so") != 0) {
        ks_debug("[LOAD_MODULE] '%S' is not a `.so` file", fname);
        return NULL;
    }

    // now, load it via dlopen
    void* handle = dlopen(fname->chr, RTLD_LAZY);    
    if (handle == NULL) {
        ks_debug("[LOAD_MODULE] problems opening '%S': %s", fname, dlerror());
        return NULL;
    }

    // and get the module source from it
    ks_module_init_t* mod_init = (ks_module_init_t*)dlsym(handle, "_module_init");
    if (mod_init == NULL) {
        ks_debug("[LOAD_MODULE] problems loading '%S'._module_init: %s", fname, dlerror());
        return NULL;
    }

    // it was successful
    ks_debug("[LOAD_MODULE] success using '%S', now returning its result...", fname);

    // call the init function
    return (ks_module)mod_init->f_init(0, NULL);
}
// load a module by a given name
ks_module ks_load_module(ks_str name) {
    ks_str fname = NULL;
    ks_module res = NULL;

    #define ATTEMPT(fmt, ...) { fname = ks_str_new_cfmt(fmt, __VA_ARGS__); res = attempt_load(fname); KSO_DECREF(fname); if (res) { return res; } }

    // first, just try the name itself as a .so
    ATTEMPT("%S", name);

    int i;
    for (i = 0; i < ksm_search_path->len; ++i) {
        // generate new paths from search paths
        ATTEMPT("%S/libksm_%S.so", ksm_search_path->items[i], name);

    }

    return kse_fmt("ImportError: Could not find module '%S'", name);
}


TFUNC(module, getattr) {
    #define SIG "module.__getattr__(self, attr)"
    REQ_N_ARGS(2);
    ks_module self = (ks_module)args[0];
    REQ_TYPE("self", self, ks_T_module);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);

    kso ret = NULL;

    /**/ if (KS_STR_EQ_CONST(attr, "__name__"))     ret = (kso)self->name;
    else if (KS_STR_EQ_CONST(attr, "__type__"))     ret = (kso)self->type;
    else if (KS_STR_EQ_CONST(attr, "__dict__"))     ret = (kso)self->__dict__;
    else {
        ret = ks_dict_get(self->__dict__, (kso)attr, attr->v_hash);
    }

    if (ret == NULL) {
        return kse_fmt("KeyError: %S", attr);
    } else {
        return KSO_NEWREF(ret);
    }
    #undef SIG
}


TFUNC(module, setattr) {
    #define SIG "module.__setattr__(self, attr, val)"
    REQ_N_ARGS(3);
    ks_module self = (ks_module)args[0];
    REQ_TYPE("self", self, ks_T_module);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);
    kso val = args[2];

    ks_dict_set(self->__dict__, (kso)attr, attr->v_hash, val);

    return KSO_NEWREF(val);
    #undef SIG
}

TFUNC(module, free) {
    #define SIG "module.__free__(self)"
    REQ_N_ARGS(1);
    ks_module self = (ks_module)args[0];
    REQ_TYPE("self", self, ks_T_module);

    KSO_DECREF(self->name);
    KSO_DECREF(self->__dict__);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}

/* exporting functionality */

struct ks_type T_module, *ks_T_module = &T_module;

void ks_init__module() {

    /* create the type */
    T_module = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_module, "module");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_module, "__getattr__", "module.__getattr__(self, attr)", module_getattr_);
    ADDCF(ks_T_module, "__setattr__", "module.__setattr__(self, attr, val)", module_setattr_);

    ADDCF(ks_T_module, "__free__", "module.__free__(self)", module_free_);
    // create an empty search path
    ksm_search_path = ks_list_new_empty();

    // get the ':' seperated path 
    char* ksm_path = getenv("KSM_PATH");
    if (ksm_path != NULL) {
        int slen = strlen(ksm_path);
        // some were given
        int i, _last = 0;
        for (i = 0; i < slen; ++i) {
            if (ksm_path[i] == ':') {
                // path seperator, so add it to the list
                ks_str cpath = ks_str_new_l(ksm_path+_last, i-_last-1);
                ks_list_push(ksm_search_path, (kso)cpath);
                KSO_DECREF(cpath);
                _last = i+1;
            }
        }
        // and do it for the last

        ks_str cpath = ks_str_new_l(ksm_path+_last, i-_last);
        ks_list_push(ksm_search_path, (kso)cpath);
        KSO_DECREF(cpath);
        //ks_info("KSM_PATH -> %R", ksm_search_path);
    }
}