/* module.c - implementation of the kscript module system
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"



// create a kscript function
ks_module ks_module_new(const char* mname) {
    ks_module self = KS_ALLOC_OBJ(ks_module);
    KS_INIT_OBJ(self, ks_T_module);

    // initialize type-specific things
    self->attr = ks_dict_new_c(KS_KEYVALS(
        {"__name__", (ks_obj)ks_str_new(mname)},
    ));

    return self;
}


// attempt to load a single file, without any extra paths
// Do not raise error, just return NULL if not successful
static ks_module attempt_load(char* cname) {

    char* ext = strrchr(cname, '.');

    if (ext != NULL && strcmp(&ext[1], KS_SHARED_END) == 0) {
        // load the library as a C API

        // attempt to load linux shared library
        // now, load it via dlopen
        void* handle = dlopen(cname, RTLD_LAZY | RTLD_GLOBAL);    
        if (handle == NULL) {
            ks_debug("ks", "[import] file '%s' failed: dlerror(): %s", cname, dlerror());
            return NULL;
        }
        
        struct ks_module_cinit* mod_cinit = (struct ks_module_cinit*)dlsym(handle, "__ks_module_cinit__");
        
        if (mod_cinit != NULL) {
            // call the function, and return its result
            ks_module mod = mod_cinit->load_func();
            if (!mod) {
                ks_debug("ks", "[import] file '%s' failed: Exception was thrown by '__ks_module_cinit__->load_func()'", cname);
                return NULL;
            }

            ks_debug("ks", "[import] file '%s' succeeded!", cname);
            return mod;
        } else {
            ks_debug("ks", "[import] file '%s' failed: No '__C_module_init__' symbol!", cname);
            return NULL;
        }

    }

    // not found
    return NULL;
}

// cache of modules
static ks_dict mod_cache = NULL;

// attempt a module with a given name
ks_module ks_module_import(const char* mname) {
    //__C_module_init__
    ks_module mod = NULL;

    ks_str mod_key = ks_str_new(mname);
    ks_debug("ks", "[import] trying to import '%s'...", mname);
    
    // check the cache for quick return
    mod = (ks_module)ks_dict_get_h(mod_cache, (ks_obj)mod_key, mod_key->v_hash);
    if (mod != NULL) {
        KS_DECREF(mod_key);
        return mod;
    }

    int i;
    for (i = 0; i < ks_paths->len; ++i) {
        // current path we are trying
        ks_str ctry = ks_fmt_c("%S/%s/libksm_%s.%s", ks_paths->elems[i], mname, mname, KS_SHARED_END);

        mod = attempt_load(ctry->chr);
        KS_DECREF(ctry);

        if (mod != NULL) goto finish;
        if (ks_thread_get()->exc) goto finish;
    }


    finish:;

    if (mod == NULL) {
        // not found, throw error
        KS_DECREF(mod_key);
        if (ks_thread_get()->exc) return NULL;
        else return (ks_module)ks_throw(ks_T_ImportError, "Failed to import module '%s': No such module!", mname);
    } else {
        // add it to the dictionary, and return
        ks_dict_set_h(mod_cache, (ks_obj)mod_key, mod_key->v_hash, (ks_obj)mod);
        KS_DECREF(mod_key);
        return mod;
    }

}


// module.__free__(self) - free obj
static KS_TFUNC(module, free) {
    ks_module self;
    KS_GETARGS("self:*", &self, ks_T_module)

    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_module);

void ks_init_T_module() {
    ks_type_init_c(ks_T_module, "module", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(module_free_, "module.__free__(self)")},
    ));

}
