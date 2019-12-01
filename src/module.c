// module.c - implementation of module semantics for the kscript language
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

// here, we grow a cache list of modules
static struct {

    int n;
    ks_str* names;
    ks_obj* modules;

} mcache = { 0, NULL, NULL };


ks_obj ks_module_load(ks_ctx ctx, ks_str name) {
    int i;
    for (i = 0; i < mcache.n; ++i) {
        if (ks_str_eq(name, mcache.names[i])) {
            return mcache.modules[i];
        }
    }

    // else, we need to load it in for real
    ks_str file_name = ks_str_fmt("libMOD_%s.so", name._);

    // use dlopen to load it in. Note, RTLD_NOW makes sure everything can be linked right now,
    //   and RTLD_GLOBAL allows the current running process to provide symbols
    void* ptr = dlopen(file_name._, RTLD_NOW | RTLD_GLOBAL);

    if (ptr == NULL) {
        ks_warn("dlopen(%s, RTLD_NOW | RTLD_GLOBAL) failed, reason: %s", file_name._, dlerror());
        ks_str_free(&file_name);
        return NULL;
    }

    // this is a special name that should be in each one
    struct ks_module_loader* kml = (struct ks_module_loader*)dlsym(ptr, "module_loader");

    if (kml == NULL) {
        ks_warn("dlopen(%s).module_loader was not found!", file_name._);
        ks_str_free(&file_name);
        return NULL;
    }

    // actually load the module
    ks_obj mod = ks_obj_new_module();
    int status = kml->f_load(ctx, mod);

    if (status != 0) {
        ks_warn("dlopen(%s).module_loader(ctx, module) return status code %d", file_name._, status);
        ks_str_free(&file_name);
        return NULL;
    }

    // add it to the cache
    int idx = mcache.n++;
    mcache.names = realloc(mcache.names, sizeof(ks_str) * mcache.n);
    mcache.modules = realloc(mcache.modules, sizeof(ks_obj) * mcache.n);
    mcache.names[idx] = ks_str_dup(name);
    mcache.modules[idx] = mod;

    // free tmp string
    ks_str_free(&file_name);

    return mod;
}

