/* types/module.c - represents a module-type, and module loading utilities*/

#include "ks_common.h"

// for plugin loading
#include <dlfcn.h>

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


// load a module by a given name
ks_module ks_load_module(ks_str name) {

    int i;
    for (i = 0; i < 1; ++i) {
        // generate new paths

        ks_str libfile = ks_str_new_cfmt("%V", name);

        ks_trace("[LOAD_MODULE] trying file %V...", libfile);

        // first ensure the extension is correct
        char* ext = strrchr(libfile->chr, '.');
        if (ext == NULL) {
            ks_trace("[LOAD_MODULE] file '%V' didn't work; not a valid extension", libfile);
            KSO_DECREF(libfile);
            continue;
        }
        if (strcmp(ext, ".so") != 0) {
            ks_trace("[LOAD_MODULE] '%V' is not a `.so` file", libfile);
            KSO_DECREF(libfile);
            continue;
        }

        // now, load it via dlopen
        void* handle = dlopen(libfile->chr, RTLD_LAZY);    
        if (handle == NULL) {
            ks_trace("[LOAD_MODULE] problems opening '%V': %s", libfile, dlerror());
            KSO_DECREF(libfile);
            continue;
        }

        // and get the module source from it
        ks_module_init_t* mod_init = (ks_module_init_t*)dlsym(handle, "_module_init");
        if (mod_init == NULL) {
            ks_trace("[LOAD_MODULE] problems loading '%V'._module_init: %s", libfile, dlerror());
            KSO_DECREF(libfile);
            continue;
        }

        // it was successful
        ks_trace("[LOAD_MODULE] sucess using '%V', now returning its result...", libfile);

        KSO_DECREF(libfile);

        return (ks_module)mod_init->f_init(0, NULL);
    }

    return kse_fmt("ImportError: Could not open module '%V'", name);
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
        return kse_fmt("KeyError: %V", attr);
    } else {
        return KSO_NEWREF(ret);
    }
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

    T_module = (struct ks_type) {
        KSO_BASE_INIT(ks_T_type)

        .name = ks_str_new("module"),

        .f_getattr = (kso)ks_cfunc_new(module_getattr_),

        .f_free = (kso)ks_cfunc_new(module_free_),

    };
}

