/* types/module.c - the module class, which can be created via the C API or constructed
 *   via a .ks file
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_module);

// create a kscript function
ks_module ks_module_new(char* mname) {
    ks_module self = KS_ALLOC_OBJ(ks_module);
    KS_INIT_OBJ(self, ks_type_module);

    // initialize type-specific things
    self->attr = ks_dict_new_cn((ks_dict_ent_c[]){
        {"__name__", (ks_obj)ks_str_new(mname)},
        {NULL, NULL}  
    });

    return self;
}


// attempt to load a single file, without any extra paths
// Do not raise error, just return NULL if not successful
static ks_module attempt_load(char* cname) {
    ks_debug("[import] trying %s...", cname);

    char* ext = strrchr(cname, '.');

    if (ext != NULL && strcmp(&ext[1], KS_SHARED_END) == 0) {
        // load the library as a C API

        // attempt to load linux shared library
        // now, load it via dlopen
        void* handle = dlopen(cname, RTLD_LAZY | RTLD_GLOBAL);    
        if (handle == NULL) {
            ks_debug("[import] '%s' failed: dlerror(): %s", cname, dlerror());
            return NULL;
        }
        
        struct ks_module_cext_init* cext_init = (struct ks_module_cext_init*)dlsym(handle, "__C_module_init__");
        
        if (cext_init != NULL) {
            // call the function, and return its result
            ks_module mod = cext_init->init_func();
            if (!mod) {
                ks_debug("[import] '%s' failed: Exception was thrown by '__C_module_init__->init_func()'", cname);
                return NULL;
            }

            ks_debug("[import] '%s' succeeded!", cname);
            return mod;
        } else {
            ks_debug("[import] '%s' failed: No '__C_module_init__' symbol!", cname);
            return NULL;
        }

    }

    // not found
    return NULL;
}


// cache of modules
static ks_dict mod_cache = NULL;


// attempt a module with a given name
ks_module ks_module_import(char* mname) {
    //__C_module_init__
    ks_module mod = NULL;

    ks_str mod_key = ks_str_new(mname);
    
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

        ctry = ks_fmt_c("%S/modules/%s/libksm_%s.%s", ks_paths->elems[i], mname, mname, KS_SHARED_END);

        mod = attempt_load(ctry->chr);
        KS_DECREF(ctry);
        if (mod != NULL) goto finish;

    }


    finish:;

    if (mod == NULL) {
        // not found, throw error
        KS_DECREF(mod_key);
        if (ks_thread_get()->exc) return NULL;
        else return ks_throw_fmt(ks_type_Error, "Failed to import module '%s': No such module!", mname);
    } else {
        // add it to the dictionary, and return
        ks_dict_set_h(mod_cache, (ks_obj)mod_key, mod_key->v_hash, (ks_obj)mod);
        KS_DECREF(mod_key);
        return mod;
    }


}

// add member types
void ks_module_add_enum_members(ks_module self, ks_type enumtype) {
    if (!ks_type_issub(enumtype, ks_type_Enum)) {
        ks_throw_fmt(ks_type_TypeError, "Attempted to add a type that was not an enum with a module!");
        return;
    }

    // get the enum keys array
    ks_list e_keys = (ks_list)ks_type_get_c(enumtype, "_enum_keys");
    if (!e_keys) return;
    if (e_keys->type != ks_type_list) {
        ks_throw_fmt(ks_type_TypeError, "While adding enum to module: '_enum_keys' (type: %T) was not a list!", e_keys);
        KS_DECREF(e_keys);
        return;
    }

    int i;
    for (i = 0; i < e_keys->len; ++i) {
        ks_str e_key = (ks_str)e_keys->elems[i];
        if (e_key->type != ks_type_str) {
            KS_DECREF(e_keys);
            ks_throw_fmt(ks_type_TypeError, "Internal problem with enum; '_enum_keys' (which was %R) contained a non-string", e_keys);
            return;
        }

        // add a reference in the module
        ks_obj e_elem = ks_dict_get_h(enumtype->attr, (ks_obj)e_key, e_key->v_hash);
        ks_dict_set_h(self->attr, (ks_obj)e_key, e_key->v_hash, e_elem);
        KS_DECREF(e_elem);
    }
    KS_DECREF(e_keys);

}


// add member types
void ks_dict_add_enum_members(ks_dict self, ks_type enumtype) {
    // get the enum keys array
    ks_list e_keys = (ks_list)ks_dict_get_c(enumtype->attr, "_enum_keys");
    if (!e_keys) return;
    if (e_keys->type != ks_type_list) {
        ks_throw_fmt(ks_type_TypeError, "While adding enum to dict: '_enum_keys' (type: %T) was not a list!", e_keys);
        KS_DECREF(e_keys);
        return;

    }



    int i;
    for (i = 0; i < e_keys->len; ++i) {
        ks_str e_key = (ks_str)e_keys->elems[i];
        if (e_key->type != ks_type_str) {
            KS_DECREF(e_keys);
            ks_throw_fmt(ks_type_TypeError, "Internal problem with dict; '_enum_keys' (which was %R) contained a non-string", e_keys);
            return;
        }

        // add a reference in the module
        ks_obj e_elem = ks_dict_get_h(enumtype->attr, (ks_obj)e_key, e_key->v_hash);
        ks_dict_set_h(self, (ks_obj)e_key, e_key->v_hash, e_elem);
        KS_DECREF(e_elem);
    }
    KS_DECREF(e_keys);

}


/* member functions */

// module.__free__(self) -> free a module
static KS_TFUNC(module, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_module self = (ks_module)args[0];
    KS_REQ_TYPE(self, ks_type_module, "self");
    
    // only thing is the attribute dictionary
    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// module.__str__(self) -> to string
static KS_TFUNC(module, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_module self = (ks_module)args[0];
    KS_REQ_TYPE(self, ks_type_module, "self");
    
    ks_str name = (ks_str)ks_dict_get_c(self->attr, "__name__");
    ks_str ret = ks_fmt_c("<'%T' : %S>", self, name);
    KS_DECREF(name);

    return (ks_obj)ret;
};

// module.__getattr__(self, attr) -> get attribute
static KS_TFUNC(module, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_module self = (ks_module)args[0];
    KS_REQ_TYPE(self, ks_type_module, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");

    // special case
    if (*attr->chr == '_' && strncmp(attr->chr, "__dict__", 8) == 0) {
        return KS_NEWREF(self->attr);
    }

    ks_obj ret = ks_dict_get_h(self->attr, (ks_obj)attr, attr->v_hash);

    if (!ret) {
        KS_ERR_ATTR(self, attr);
    }

    return ret;
};

// module.__setattr__(self, attr, val) -> set attribute
static KS_TFUNC(module, setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_module self = (ks_module)args[0];
    KS_REQ_TYPE(self, ks_type_module, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");
    ks_obj val = args[2];

    ks_dict_set_h(self->attr, (ks_obj)attr, attr->v_hash, val);

    return KS_NEWREF(val);
};


// initialize cfunc type
void ks_type_module_init() {
    KS_INIT_TYPE_OBJ(ks_type_module, "module");

    ks_type_set_cn(ks_type_module, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new2(module_free_, "module.__free__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new2(module_str_, "module.__str__(self)")},

        {"__getattr__", (ks_obj)ks_cfunc_new2(module_getattr_, "module.__getattr__(self, attr)")},
        {"__setattr__", (ks_obj)ks_cfunc_new2(module_setattr_, "module.__setattr__(self, attr, val)")},

        {NULL, NULL}   
    });

    // construct cache
    mod_cache = ks_dict_new(0, NULL);

}

