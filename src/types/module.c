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

    if (strcmp(ext, ".so") == 0) {
        // attempt to load linux shared library
        // now, load it via dlopen
        void* handle = dlopen(cname, RTLD_LAZY);    
        if (handle == NULL) return NULL;
        
        struct ks_module_cext_init* cext_init = (struct ks_module_cext_init*)dlsym(handle, "__C_module_init__");
        
        if (cext_init != NULL) {
            // call the function, and return its result
            return cext_init->init_func();   
        }

    }

    // not found
    return NULL;

}

// attempt a module with a given name
ks_module ks_module_import(char* mname) {
    //__C_module_init__
    ks_module mod = NULL;

    int i;
    for (i = 0; i < ks_paths->len; ++i) {
        // current path we are trying
        ks_str ctry = ks_fmt_c("%S/%s/libksm_%s.so", ks_paths->elems[i], mname, mname);

        mod = attempt_load(ctry->chr);
        KS_DECREF(ctry);
        if (mod != NULL) return mod;

    }

    return ks_throw_fmt(ks_type_Error, "Failed to import module '%s'", mname);
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

    return ks_dict_get(self->attr, attr->v_hash, (ks_str)attr);
};

// module.__setattr__(self, attr, val) -> set attribute
static KS_TFUNC(module, setattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_module self = (ks_module)args[0];
    KS_REQ_TYPE(self, ks_type_module, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");
    ks_obj val = args[2];

    ks_dict_set(self->attr, attr->v_hash, (ks_str)attr, val);

    return KS_NEWREF(val);
};


// initialize cfunc type
void ks_type_module_init() {
    KS_INIT_TYPE_OBJ(ks_type_module, "module");

    ks_type_set_cn(ks_type_module, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new(module_free_)},

        {"__str__", (ks_obj)ks_cfunc_new(module_str_)},

        {"__getattr__", (ks_obj)ks_cfunc_new(module_getattr_)},
        {"__setattr__", (ks_obj)ks_cfunc_new(module_setattr_)},

        {NULL, NULL}   
    });

}

