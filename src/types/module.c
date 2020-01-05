/* types/module.cc - represents a module-type */

#include "ks_common.h"

// for plugin loading
#include <dlfcn.h>

// create a new module with a name
ks_module ks_module_new(ks_str name) {
    ks_module self = (ks_module)ks_malloc(sizeof(*self));
    *self = (struct ks_module) {
        KSO_BASE_INIT(ks_T_module, KSOF_NONE)
        .name = name
    };

    KSO_INCREF(name);

    // return our constructed integer
    return self;
}


ks_module ks_module_load(const char* src_name) {

    // first ensure the extension is correct
    char* ext = strrchr(src_name, '.');
    if (ext == NULL) return kse_fmt("ImportError: C-extension module '%s' has no `.EXT`!", src_name);
    if (strcmp(ext, ".so") != 0) return kse_fmt("ImportError: C-extension module '%s' is not a `.so` file", src_name);

    // now, load it via dlopen
    void* handle = dlopen(src_name, RTLD_LAZY);    
    if (handle == NULL) return kse_fmt("ImportError: Could not open module '%s'", src_name);

    // and get the module source from it
    ks_module_init_t* mod_init = (ks_module_init_t*)dlsym(handle, "_module_init");
    if (mod_init == NULL) return kse_fmt("ImportError: Module '%s' did not have a valid initializer! (make sure to include MODULE_END())", src_name);

    // call its initializer
    return (ks_module)mod_init->f_init(0, NULL);

}

/* exporting functionality */


struct ks_type T_module, *ks_T_module = &T_module;

void ks_init__module() {

    T_module = (struct ks_type) {
        KS_TYPE_INIT("module")

    };
}

