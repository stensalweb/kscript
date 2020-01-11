/* kobj.c - kscript generic object type */

#include "ks_common.h"


TFUNC(kobj, new) {
    #define SIG "kobj.__new__()"
    REQ_N_ARGS(0);

    ks_kobj self = ks_malloc(sizeof(*self));
    *self = (struct ks_kobj) {
        KSO_BASE_INIT(ks_T_kobj)
        .attr = ks_dict_new_empty(),
    };

    return (kso)self;
    #undef SIG
}


TFUNC(kobj, getattr) {
    #define SIG "kobj.__getattr__(self, attr)"
    REQ_N_ARGS(2);
    ks_kobj self = (ks_kobj)args[0];
    REQ_TYPE("self", self, ks_T_kobj);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);

    kso ret = ks_dict_get(self->attr, (kso)attr, attr->v_hash);

    if (ret == NULL) {
        return kse_fmt("KeyError: %R", attr);
    } else {
        return KSO_NEWREF(ret);
    }

    #undef SIG
}

TFUNC(kobj, setattr) {
    #define SIG "kobj.__setattr__(self, attr, val)"
    REQ_N_ARGS(3);
    ks_kobj self = (ks_kobj)args[0];
    REQ_TYPE("self", self, ks_T_kobj);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);
    kso val = args[2];

    ks_dict_set(self->attr, (kso)attr, attr->v_hash, val);

    return KSO_NEWREF(val);
    #undef SIG
}

TFUNC(kobj, free) {
    #define SIG "kobj.__free__(self)"
    REQ_N_ARGS(1);
    ks_kobj self = (ks_kobj)args[0];
    REQ_TYPE("self", self, ks_T_kobj);

    KSO_DECREF(self->attr);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}



/* exporting functionality */

struct ks_type T_kobj, *ks_T_kobj = &T_kobj;

void ks_init__kobj() {

    /* create the type */
    T_kobj = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_kobj, "kobj");

    // add cfuncs
    #define ADDCF(_type, _name, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_kobj, "__new__", kobj_new_);
    ADDCF(ks_T_kobj, "__free__", kobj_free_);
    ADDCF(ks_T_kobj, "__getattr__", kobj_getattr_);
    ADDCF(ks_T_kobj, "__setattr__", kobj_setattr_);

}



