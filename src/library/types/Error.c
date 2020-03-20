/* types/Error.c - implementation of the base class for an error
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_Error);
KS_TYPE_DECLFWD(ks_type_MathError);
KS_TYPE_DECLFWD(ks_type_SyntaxError);
KS_TYPE_DECLFWD(ks_type_SizeError);
KS_TYPE_DECLFWD(ks_type_KeyError);
KS_TYPE_DECLFWD(ks_type_AttrError);
KS_TYPE_DECLFWD(ks_type_TypeError);
KS_TYPE_DECLFWD(ks_type_OpError);
KS_TYPE_DECLFWD(ks_type_ToDoError);
KS_TYPE_DECLFWD(ks_type_OutOfIterError);

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

// Error.__new__() -> allocate an error
static KS_TFUNC(Error, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 1);

    if (n_args == 0) {
        // empty one
        ks_Error self = KS_ALLOC_OBJ(ks_Error);
        KS_INIT_OBJ(self, ks_type_Error);

        // initialize the dictionary
        self->attr = ks_dict_new_cn((ks_dict_ent_c[]){
            {NULL, NULL}
        });


        return (ks_obj)self;
    } else {

        ks_str what = (ks_str)args[0];
        KS_REQ_TYPE(what, ks_type_str, "what");

        return (ks_obj)ks_Error_new(what);
    }
};

// Error.__init__(self, what) -> construct an error
static KS_TFUNC(Error, init) {

    KS_REQ_N_ARGS(n_args, 2);
    ks_Error self = (ks_Error)args[0];
    KS_REQ_TYPE(self, ks_type_Error, "self");

    ks_str what = (ks_str)args[1];
    KS_REQ_TYPE(what, ks_type_str, "what");

    ks_dict_set_cn(self->attr, (ks_dict_ent_c[]){
        {"what", KS_NEWREF(what)},
        {NULL, NULL}
    });

    // done initializing
    return KSO_NONE;
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
        {"__new__", (ks_obj)ks_cfunc_new2(Error_new_, "Error.__new__()")},
        {"__init__", (ks_obj)ks_cfunc_new2(Error_init_, "Error.__init__(self, what)")},
        {"__free__", (ks_obj)ks_cfunc_new2(Error_free_, "Error.__free__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new2(Error_str_, "Error.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(Error_str_, "Error.__repr__(self)")},

        {"__getattr__", (ks_obj)ks_cfunc_new2(Error_getattr_, "Error.__getattr__(self, attr)")},

        {NULL, NULL}   
    });

    // sub types
    KS_INIT_TYPE_OBJ(ks_type_SyntaxError, "SyntaxError");
    ks_type_add_parent(ks_type_SyntaxError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_MathError, "MathError");
    ks_type_add_parent(ks_type_MathError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_SizeError, "SizeError");
    ks_type_add_parent(ks_type_SizeError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_KeyError, "KeyError");
    ks_type_add_parent(ks_type_KeyError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_AttrError, "AttrError");
    ks_type_add_parent(ks_type_AttrError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_TypeError, "TypeError");
    ks_type_add_parent(ks_type_TypeError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_OpError, "OpError");
    ks_type_add_parent(ks_type_OpError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_ToDoError, "ToDoError");
    ks_type_add_parent(ks_type_ToDoError, ks_type_Error);

    KS_INIT_TYPE_OBJ(ks_type_OutOfIterError, "OutOfIterError");
    ks_type_add_parent(ks_type_OutOfIterError, ks_type_Error);

}

