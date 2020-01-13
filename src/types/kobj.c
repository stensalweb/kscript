/* kobj.c - kscript generic object type 

This is the base class of all kscript-defined types, and is basically just a dictionary

The `->__dict__` member holds all of the member variables for a kobj (except member functions)

And, all the function associated with it require the value to be a subclass of kobj,
since it requires the __dict__

*/

#include "ks_common.h"

TFUNC(kobj, new) {
    KS_REQ(n_args == 0, "Given wrong number of args, expected 0, but got %i", n_args);

    ks_kobj self = ks_malloc(sizeof(*self));
    *self = (struct ks_kobj) {
        KSO_BASE_INIT(ks_T_kobj)
        .attr = ks_dict_new_empty(),
    };

    return (kso)self;
}

TFUNC(kobj, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_kobj self = (ks_kobj)args[0];
    KS_REQ_SUBTYPE(self, ks_T_kobj, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_T_str, "attr");

    // to avoid recursive references, allow `__attr__` to be returned as a special case
    // not included generically
    if (KS_STR_EQ_CONST(attr, "__attr__")) {
        return KSO_NEWREF(self->attr);
    }

    // try and find a given attribute
    kso ret = ks_dict_get(self->attr, (kso)attr, attr->v_hash);

    if (ret == NULL) {
        // search through the type, recursively
        ret = ks_dict_get(self->type->__dict__, (kso)attr, attr->v_hash);
        if (ret == NULL) {
            return kse_fmt("KeyError: %R", attr);
        } else if (ret->type == ks_T_kfunc || ret->type == ks_T_cfunc) {
            // do member function, as a pfunc
            ks_pfunc mem_func = ks_pfunc_new(ret);
            KSO_DECREF(ret);
            // now, let mem_func(*args) == type.func(self, *args),
            // by filling in the 0th arg
            ks_pfunc_fill(mem_func, 0, (kso)self);
            return (kso)mem_func;
        } else {
            // TODO: maybe allow member references to resolve to statics?
            return kse_fmt("KeyError: %R", attr);
        }
    } else {
        return KSO_NEWREF(ret);
    }
}

TFUNC(kobj, setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_kobj self = (ks_kobj)args[0];
    KS_REQ_SUBTYPE(self, ks_T_kobj, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_T_str, "attr");

    // get the value
    kso val = args[2];

    ks_dict_set(self->attr, (kso)attr, attr->v_hash, val);

    return KSO_NEWREF(val);
}

TFUNC(kobj, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_kobj self = (ks_kobj)args[0];
    KS_REQ_SUBTYPE(self, ks_T_kobj, "self");

    // record a reference to the dictionary
    KSO_DECREF(self->attr);

    // and free the actual pointer itself
    ks_free(self);

    return KSO_NONE;
}



/* exporting functionality */

struct ks_type T_kobj, *ks_T_kobj = &T_kobj;

void ks_init__kobj() {

    /* create the type */
    T_kobj = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_kobj, "kobj");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_kobj, "__new__", "kobj.__new__(self)", kobj_new_);
    ADDCF(ks_T_kobj, "__free__", "kobj.__free__(self)", kobj_free_);
    ADDCF(ks_T_kobj, "__getattr__", "kobj.__getattr__(self, attr)", kobj_getattr_);
    ADDCF(ks_T_kobj, "__setattr__", "kobj.__setattr__(self, attr, val)", kobj_setattr_);

}



