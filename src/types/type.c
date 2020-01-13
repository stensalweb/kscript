/* types/type.c - represents a type-type

Types in kscript should all typically define member functions as functions accepting the instance as the first argument

For example:

`A.append(1, 2)` becomes (most of the time) `type(A).append(A, 1, 2)`

A few exceptions to this rule are:

  * Manually setting functions unique to single objects (for example, event listeners may do this)


*/

#include "ks_common.h"


// create a new type
ks_type ks_type_new(char* name) {
    ks_type self = ks_malloc(sizeof(*self));
    *self = KS_TYPE_INIT();

    // set the name
    ks_type_setname_c(self, name);

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

// return 1 if self inherits (somewhere in its tree of dependencies) from `parent`, 0 otherwise
int ks_type_issub(ks_type self, ks_type parent) {
    if (self == parent) return 1;

    int i;
    for (i = 0; i < self->n_parents; ++i) {
        // check recursively
        if (ks_type_issub(self->parents[i], parent) != 0) return 1;
    }

    // was not found, so it does not inherit
    return 0;
}

// recursive implementation of getattr, use if not found in self->__dict__
static kso type_getattr_R(ks_type self, ks_str attr) {
    // don't try itself; since this is a breadth first search, this makes it easier to recurse
    // thus, call the dict get on `self->__dict__` before calling this
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

// gets `self.attr`, searching recursively
kso ks_type_getattr(ks_type self, ks_str attr) {

    // to avoid recursive references, allow `__dict__` to be returned as a special case
    if (KS_STR_EQ_CONST(attr, "__dict__")) {
        return KSO_NEWREF(self->__dict__);
    }

    // search through to get a generic attr
    kso ret = ks_dict_get(self->__dict__, (kso)attr, attr->v_hash);

    if (ret != NULL) {
        // it was found in the dictionary of attributes specific to this object,
        // so return it
        return KSO_NEWREF(ret);
    } else {
        // not found, so search through parent classes to see if they provide anything of the same name
        ret = type_getattr_R(self, attr);
        if (ret != NULL) {
            // a parent type had the requested attr, so return a new reference
            return KSO_NEWREF(ret);
        } else {
            // nowhere in the entire tree was `attr` found, so give an error
            return kse_fmt("AttrError: %R", attr);
        }
    }
}

// sets a given attribute on the type
void ks_type_setattr(ks_type self, ks_str attr, kso val) {

    // check if this is sets an attribute which begins with `__`
    if (attr->len > 2 && attr->chr[0] == '_' && attr->chr[1] == '_') {
        // for convenience, set the f_* or special parmeters to this value as well.
        // do not record an extra reference; this should always be shared as the one in 
        // the dictionary
        
        // while this uses slightly CPU cycles, the above short circuit makes it plenty efficient for most cases
        // and, I am counting on people only rarely setting the internal `__*` attributes,
        // so this is a very minor penalty.
        // On the other hand, built in methods/functionality may now use `type->f_add` for the operator function, 
        // for instance, and `type->f_str` for to string, instetad of having to use `ks_type_getattr(self, "__str__")`
        // in this case, it will likely save computation time, as those will be gotten far more than set

        /**/ if (KS_STR_EQ_CONST(attr, "__name__")) {
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

        // don't short circuit, as it should also be generically set in
        // the dictionary as well (that will also record the reference)

    }

    // replace the dictionary entry itself
    ks_dict_set(self->__dict__, (kso)attr, attr->v_hash, val);

}

// ease of use routine for setting from C
void ks_type_setattr_c(ks_type self, char* attr, kso val) {
    ks_str attro = ks_str_new(attr);
    ks_type_setattr(self, attro, val);
    KSO_DECREF(attro);
}

// ease of use routine for setting from C
void ks_type_setname_c(ks_type self, char* name) {
    ks_str _sn = ks_str_new(name);
    ks_type_setattr_c(self, "__name__", (kso)_sn);
    KSO_DECREF(_sn);
}

// ease of use routine for getting from C
kso ks_type_getattr_c(ks_type self, char* attr) {
    ks_str attro = ks_str_new(attr);
    kso ret = ks_type_getattr(self, attro);
    KSO_DECREF(attro);
    return ret;
}


/* generic functions */

TFUNC(type, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type self = (ks_type)args[0];
    KS_REQ_TYPE(self, ks_T_type, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_T_str, "attr");

    return ks_type_getattr(self, attr);
}

TFUNC(type, setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_type self = (ks_type)args[0];
    KS_REQ_TYPE(self, ks_T_type, "self")
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_T_str, "attr")

    // the value to set it to
    kso val = args[2];

    ks_type_setattr(self, attr, val);

    // return a new reference to what was set
    return KSO_NEWREF(val);
}

TFUNC(type, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_type self = (ks_type)args[0];
    KS_REQ_TYPE(self, ks_T_type, "self")
    
    // disregard references to the hierarchy of parents
    int i;
    for (i = 0; i < self->n_parents; ++i) {
        KSO_DECREF(self->parents[i]);
    }

    // free our list of parents
    ks_free(self->parents);

    // stop our reference to our dictionary of members
    KSO_DECREF(self->__dict__);

    // and free the pointer too
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
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_type, "__getattr__", "type.__getattr__(self, attr)", type_getattr_);
    ADDCF(ks_T_type, "__setattr__", "type.__setattr__(self, attr, val)", type_setattr_);

    ADDCF(ks_T_type, "__free__", "type.__free__(self)", type_free_);

}



