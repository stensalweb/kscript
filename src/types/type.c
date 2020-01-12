/* types/type.c - represents a type-type */

#include "ks_common.h"


// create a new type
ks_type ks_type_new(char* name) {
    ks_type self = ks_malloc(sizeof(*self));
    *self = KS_TYPE_INIT();

    // set the name
    ks_type_setname_c(self, name);

    // construct a new dictionary
//    ks_dict_set_cstrl(self->__dict__, "__type__", 8, (kso)self->type);

    //KSO_DECREF(self->name);
    //KSO_DECREF(self->type);

    return self;
}
// add a parent to a type
void ks_type_add_parent(ks_type self, ks_type parent) {
    int idx = self->n_parents++;
    self->parents = ks_realloc(self->parents, sizeof(*self->parents) * self->n_parents);

    // record the parent here
    self->parents[idx] = parent;
    KSO_INCREF(parent);

    // update any builtins
    #define UBI(NAME) if (self->NAME == NULL) self->NAME = parent->NAME;

    UBI(f_new);
    UBI(f_init);
    
    UBI(f_free);

    UBI(f_str);
    UBI(f_repr);

    UBI(f_getattr);
    UBI(f_setattr);
    UBI(f_getitem);
    UBI(f_setitem);

}

// recursive implementation of getattribute
static kso type_getattr_R(ks_type self, ks_str attr) {
    // don't try itself; since this is a breadth first search, this makes it easier to recurse
    kso ret = NULL;

    // try scooping down a level
    int i;
    for (i = 0; i < self->n_parents; ++i) {
        ret = ks_dict_get(self->parents[i]->__dict__, (kso)attr, attr->v_hash);
        if (ret != NULL) return ret;
    }

    // else, try recursively
    for (i = 0; i < self->n_parents; ++i) {
        ret = type_getattr_R(self->parents[i], attr);
        if (ret != NULL) return ret;
    }

    return NULL;
}

kso ks_type_getattr(ks_type self, ks_str attr) {

    if (KS_STR_EQ_CONST(attr, "__dict__")) {
        return KSO_NEWREF(self->__dict__);
    }

    // search through to get an attribute
    kso ret = ks_dict_get(self->__dict__, (kso)attr, attr->v_hash);

    if (ret == NULL) {
        // try finding in a base class
        // TODO actually implement this
        ret = type_getattr_R(self, attr);
        if (ret != NULL) return KSO_NEWREF(ret);

        // keyerror
        return kse_fmt("KeyError: %R", attr);
    } else {
        // return a new reference to the attribute
        return KSO_NEWREF(ret);
    }
}

// sets a given attribute on the type
void ks_type_setattr(ks_type self, ks_str attr, kso val) {

    // check if this is an __* set
    if (attr->len > 2 && attr->chr[0] == '_' && attr->chr[1] == '_') {
        // for convenience, set the f_* or special parmeter to this value as well.
        // do not record an extra reference; this should always be shared as the one in 
        // the dictionary

        /**/ if (KS_STR_EQ_CONST(attr, "__name__"))     {
            // record the new name
            self->name = (ks_str)val;
        }

        else if (KS_STR_EQ_CONST(attr, "__new__")) self->f_new = val;
        else if (KS_STR_EQ_CONST(attr, "__init__")) self->f_init = val;

        else if (KS_STR_EQ_CONST(attr, "__type__")) self->type = (ks_type)val;
        else if (KS_STR_EQ_CONST(attr, "__free__") ) self->f_free = val;

        else if (KS_STR_EQ_CONST(attr, "__repr__")) self->f_repr = val;
        else if (KS_STR_EQ_CONST(attr, "__str__") ) self->f_str = val;

        else if (KS_STR_EQ_CONST(attr, "__call__") ) self->f_call = val;

        else if (KS_STR_EQ_CONST(attr, "__getattr__") ) self->f_getattr = val;
        else if (KS_STR_EQ_CONST(attr, "__setattr__") ) self->f_setattr = val;

        else if (KS_STR_EQ_CONST(attr, "__getitem__") ) self->f_getitem = val;
        else if (KS_STR_EQ_CONST(attr, "__setitem__") ) self->f_setitem = val;

        else if (KS_STR_EQ_CONST(attr, "__add__") ) self->f_add = val;
        else if (KS_STR_EQ_CONST(attr, "__sub__") ) self->f_sub = val;
        else if (KS_STR_EQ_CONST(attr, "__mul__") ) self->f_mul = val;
        else if (KS_STR_EQ_CONST(attr, "__div__") ) self->f_div = val;
        else if (KS_STR_EQ_CONST(attr, "__mod__") ) self->f_mod = val;
        else if (KS_STR_EQ_CONST(attr, "__pow__") ) self->f_pow = val;

        else if (KS_STR_EQ_CONST(attr, "__lt__") ) self->f_lt = val;
        else if (KS_STR_EQ_CONST(attr, "__le__") ) self->f_le = val;
        else if (KS_STR_EQ_CONST(attr, "__gt__") ) self->f_gt = val;
        else if (KS_STR_EQ_CONST(attr, "__ge__") ) self->f_ge = val;
        else if (KS_STR_EQ_CONST(attr, "__eq__") ) self->f_eq = val;
        else if (KS_STR_EQ_CONST(attr, "__ne__") ) self->f_ne = val;

    }

    // replace the dictionary entry itself
    ks_dict_set(self->__dict__, (kso)attr, attr->v_hash, val);

}

void ks_type_setattr_c(ks_type self, char* attr, kso val) {
    ks_str attro = ks_str_new(attr);
    ks_type_setattr(self, attro, val);
    KSO_DECREF(attro);
}

void ks_type_setname_c(ks_type self, char* name) {
    ks_str _sn = ks_str_new(name);
    ks_type_setattr_c(self, "__name__", (kso)_sn);
    KSO_DECREF(_sn);
}

kso ks_type_getattr_c(ks_type self, char* attr) {
    ks_str attro = ks_str_new(attr);
    kso ret = ks_type_getattr(self, attro);
    KSO_DECREF(attro);
    return ret;
}


TFUNC(type, getattr) {
    #define SIG "type.__getattr__(self, attr)"
    REQ_N_ARGS(2);
    ks_type self = (ks_type)args[0];
    REQ_TYPE("self", self, ks_T_type);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);

    return ks_type_getattr(self, attr);
    #undef SIG
}

TFUNC(type, setattr) {
    #define SIG "type.__setattr__(self, attr, val)"
    REQ_N_ARGS(3);
    ks_type self = (ks_type)args[0];
    REQ_TYPE("self", self, ks_T_type);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);

    // the value to set it to
    kso val = args[2];

    ks_type_setattr(self, attr, val);

    // return a new reference to what was set
    return KSO_NEWREF(val);
    #undef SIG
}

TFUNC(type, free) {
    ks_type self = (ks_type)args[0];

    int i;
    for (i = 0; i < self->n_parents; ++i) {
        KSO_DECREF(self->parents[i]);
    }

    ks_free(self->parents);

    KSO_DECREF(self->__dict__);

    ks_free(self);

    return KSO_NONE;
}

/* exporting functionality */

struct ks_type T_type, *ks_T_type = &T_type;

void ks_init__type() {

    /* create the type */
    T_type = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_type, "type");

    // add cfuncs
    #define ADDCF(_type, _name, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }


    ADDCF(ks_T_type, "__getattr__", type_getattr_);
    ADDCF(ks_T_type, "__setattr__", type_setattr_);

    ADDCF(ks_T_type, "__free__", type_free_);

}



