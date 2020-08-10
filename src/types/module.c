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

    self->parent = NULL;
    self->children = ks_list_new(0, NULL);

    return self;
}

// set a module's parent
void ks_module_parent(ks_module self, ks_module par) {
    if (par != NULL) KS_INCREF(par);
    if (self->parent != NULL) KS_DECREF(self->parent);
    self->parent = par;
}

// attempt to load a single file, without any extra paths
// Do not raise error, just return NULL if not successful
static ks_module attempt_load(const char* mname, char* cname) {

    char* ext = strrchr(cname, '.');

    if (ext != NULL && strcmp(&ext[1], KS_SHARED_END) == 0) {
        // load the library as a C API

        // attempt to load linux shared library
        // now, load it via dlopen

        void* handle = dlopen(cname, RTLD_LAZY | RTLD_GLOBAL);    
        if (handle == NULL) {
            char* dlmsg = dlerror();
            ks_debug("ks", "[import] file '%s' failed: dlerror(): %s", cname, dlmsg);

            if (dlmsg) {
                int sl = strlen(dlmsg);
                static const char spec_msg[] = "undefined symbol";
                int i;
                for (i = 0; i < sl - sizeof(spec_msg); ++i) {
                    if (strncmp(dlmsg + i, spec_msg, sizeof(spec_msg) - 1) == 0) {
                        ks_throw(ks_T_ImportError, "Failed to import module '%s': %*s (while loading '%s')", mname, sl - i, dlmsg + i, cname);
                        break;
                    }
                }
            }

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

        mod = attempt_load(mname, ctry->chr);
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


// module.__str__(self) - to string
static KS_TFUNC(module, str) {
    ks_module self;
    KS_GETARGS("self:*", &self, ks_T_module)
    
    ks_str name = (ks_str)ks_dict_get_c(self->attr, "__name__");
    ks_str ret = ks_fmt_c("<'%T' : %S>", self, name);
    KS_DECREF(name);

    return (ks_obj)ret;
}

// module.__getattr__(self, attr) -> get attribute
static KS_TFUNC(module, getattr) {
    ks_module self;
    ks_str attr;
    KS_GETARGS("self:* attr:*", &self, ks_T_module, &attr, ks_T_str)

    // special case
    if (*attr->chr == '_' && strncmp(attr->chr, "__dict__", 8) == 0) {
        return KS_NEWREF(self->attr);
    }

    ks_obj ret = ks_dict_get_h(self->attr, (ks_obj)attr, attr->v_hash);

    if (!ret) {
        KS_THROW_ATTR_ERR(self, attr);
    }

    return ret;
}

// module.__setattr__(self, attr, val) -> set attribute
static KS_TFUNC(module, setattr) {
    ks_module self;
    ks_str attr;
    ks_obj val;
    KS_GETARGS("self:* attr:* val", &self, ks_T_module, &attr, ks_T_str, &val)

    ks_dict_set_h(self->attr, (ks_obj)attr, attr->v_hash, val);

    return KS_NEWREF(val);
}




/* export */

KS_TYPE_DECLFWD(ks_T_module);

void ks_init_T_module() {
    ks_type_init_c(ks_T_module, "module", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(module_free_, "module.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c(module_str_, "module.__str__(self)")},
        {"__getattr__",            (ks_obj)ks_cfunc_new_c(module_getattr_, "module.__getattr__(self, attr)")},
        {"__setattr__",            (ks_obj)ks_cfunc_new_c(module_setattr_, "module.__setattr__(self, attr, val)")},
    ));

    mod_cache = ks_dict_new(0, NULL);
}
