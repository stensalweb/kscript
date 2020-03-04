/* types/Error.c - implementation of the base class for an error
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_Error);

// create a kscript int from a string
ks_Error ks_Error_new(ks_str what) {
    ks_Error self = KS_ALLOC_OBJ(ks_Error);
    KS_INIT_OBJ(self, ks_type_Error);

    // initialize the dictionary
    self->attr = ks_dict_new_cn((ks_dict_ent_c[]){
        {"what", KS_NEWREF(what)},
        {NULL, NULL}
    });

    return self;
}

// create a kscript int from a C-style string
ks_Error ks_Error_new_c(char* what) {
    ks_str what_str = ks_str_new(what);
    ks_Error ret = ks_Error_new(what_str);
    KS_DECREF(what_str);
    return ret;
}


/* member functions */

// Error.__new__(self, what) -> create a new error from a string reason
static KS_TFUNC(Error, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str what = (ks_str)args[0];
    KS_REQ_TYPE(what, ks_type_str, "what");

    return (ks_obj)ks_Error_new(what);
};


// Error.__str__(self) -> create a string representing it
static KS_TFUNC(Error, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Error self = (ks_Error)args[0];
    KS_REQ_TYPE(self, ks_type_Error, "self");

    return ks_dict_get_c(self->attr, "what");
};

// Error.__free__(self) -> frees resources with the error
static KS_TFUNC(Error, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Error self = (ks_Error)args[0];
    KS_REQ_TYPE(self, ks_type_Error, "self");

    // go through the buffers & delete references
    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// Error.__getattr__(self, attr) -> get an attribute of an error
static KS_TFUNC(Error, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_Error self = (ks_Error)args[0];
    KS_REQ_TYPE(self, ks_type_Error, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");

    // first try and get the attribute
    ks_obj ret = ks_dict_get(self->attr, attr->v_hash, (ks_obj)attr);
    if (ret) return ret;

    // now, try getting a member function
    ret = ks_type_get_mf(self->type, attr, (ks_obj)self);
    if (!ret) return NULL;

    // return member function
    return ret;
};


// initialize Error type
void ks_type_Error_init() {
    KS_INIT_TYPE_OBJ(ks_type_Error, "Error");

    ks_type_set_cn(ks_type_Error, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new(Error_new_)},
        {"__free__", (ks_obj)ks_cfunc_new(Error_free_)},

        {"__str__", (ks_obj)ks_cfunc_new(Error_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(Error_str_)},

        {"__getattr__", (ks_obj)ks_cfunc_new(Error_getattr_)},

        {NULL, NULL}   
    });
}
