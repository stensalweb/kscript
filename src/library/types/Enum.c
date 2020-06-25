/* types/Enum.c - implementation of the enum base type
 *
 * TODO: make it able to create new enumerations
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_Enum);


/* member functions */

// Enum.__new__() -> create an enum instance
static KS_TFUNC(Enum, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 0);

    if (n_args == 0) {
        // empty one
        ks_Enum self = KS_ALLOC_OBJ(ks_Enum);
        KS_INIT_OBJ(self, ks_type_Enum);

        self->name = ks_str_new("VAL");

        self->enum_idx = 2;

        return (ks_obj)self;
    }/* else {

        ks_str what = (ks_str)args[0];
        KS_REQ_TYPE(what, ks_type_str, "what");

        return (ks_obj)ks_Error_new(what);
    }*/
};


// Enum.__str__(self) -> create a string representing it
static KS_TFUNC(Enum, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Enum self = (ks_Enum)args[0];
    KS_REQ_TYPE(self, ks_type_Enum, "self");

    return (ks_obj)ks_fmt_c("%S.%S", self->type->__name__, self->name);
};

// Enum.__free__(self) -> frees resources with the enum
static KS_TFUNC(Enum, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Enum self = (ks_Enum)args[0];
    KS_REQ_TYPE(self, ks_type_Enum, "self");

    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

/*
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
*/


// initialize Enum type
void ks_type_Enum_init() {
    KS_INIT_TYPE_OBJ(ks_type_Enum, "Enum");

    ks_type_set_cn(ks_type_Enum, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(Enum_new_, "Enum.__new__()")},
        {"__free__", (ks_obj)ks_cfunc_new2(Enum_free_, "Enum.__free__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new2(Enum_str_, "Enum.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(Enum_str_, "Enum.__repr__(self)")},

        //{"__getattr__", (ks_obj)ks_cfunc_new2(Error_getattr_, "Error.__getattr__(self, attr)")},

        {NULL, NULL}   
    });


}

