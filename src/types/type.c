/* types/type.c - represents a type-type */

#include "ks_common.h"


// create a new type
ks_type ks_type_new(char* name) {
    ks_type self = ks_malloc(sizeof(*self));
    *self = KS_TYPE_INIT();

    // set the name
    ks_type_set_namec(self, name);

    // construct a new dictionary
    ks_dict_set_cstrl(self->__dict__, "__name__", 8, (kso)self->name);
    ks_dict_set_cstrl(self->__dict__, "__type__", 8, (kso)self->type);

    //KSO_DECREF(self->name);
    //KSO_DECREF(self->type);

    return self;
}

TFUNC(type, getattr) {
    #define SIG "type.__getattr__(self, attr)"
    REQ_N_ARGS(2);
    ks_type self = (ks_type)args[0];
    REQ_TYPE("self", self, ks_T_type);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);

    // search through to get an attribute
    kso ret = ks_dict_get(self->__dict__, (kso)attr, attr->v_hash);

    if (ret == NULL) {
        // keyerror
        return kse_fmt("KeyError: %R", attr);
    } else {
        // return a new reference to the attribute
        return KSO_NEWREF(ret);
    }
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

    // check if this is an __* set
    if (attr->len > 2 && attr->chr[0] == '_' && attr->chr[1] == '_') {
        // for convenience, set the f_* or special parmeter to this value as well.
        // do not record an extra reference; this should always be shared as the one in 
        // the dictionary

        /**/ if (KS_STR_EQ_CONST(attr, "__name__"))     {
            if (val->type != ks_T_str) {
                return kse_fmt("KeyError: %R must be set to a str", attr);
            }
            // otherwise, record it
            self->name = (ks_str)val;
        }
        else if (KS_STR_EQ_CONST(attr, "__type__")) return kse_fmt("KeyError: %R can not be set");
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

    // return a new reference to what was set
    return KSO_NEWREF(val);
    #undef SIG
}



// set a special string value
#define __TYPESET(_typeself, _name, _charp, _val) { (_typeself)->_name = _val; ks_dict_set_cstrl((_typeself)->__dict__, (_charp), sizeof(_charp) - 1, (kso)_val); }
void ks_type_set_namec(ks_type self, char* name) {
    ks_str mys = ks_str_new(name); 
    self->name = mys;
    ks_dict_set_cstrl(self->__dict__, "__name__", 8, mys);
    KSO_DECREF(mys);
}
void ks_type_set_name(ks_type self, ks_str name) __TYPESET(self, name, "__name__", name)

void ks_type_set_str (ks_type self, kso val)  __TYPESET(self, f_str, "__str__", val)
void ks_type_set_repr(ks_type self, kso val)  __TYPESET(self, f_repr, "__repr__", val)
void ks_type_set_free(ks_type self, kso val)  __TYPESET(self, f_free, "__free__", val)
void ks_type_set_call(ks_type self, kso val)  __TYPESET(self, f_call, "__call__", val)

void ks_type_set_getattr(ks_type self, kso val)  __TYPESET(self, f_getattr, "__getattr__", val)
void ks_type_set_setattr(ks_type self, kso val)  __TYPESET(self, f_setattr, "__setattr__", val)

void ks_type_set_getitem(ks_type self, kso val)  __TYPESET(self, f_getitem, "__getitem__", val)
void ks_type_set_setitem(ks_type self, kso val)  __TYPESET(self, f_setitem, "__setitem__", val)

void ks_type_set_add(ks_type self, kso val)  __TYPESET(self, f_add, "__add__", val)
void ks_type_set_sub(ks_type self, kso val)  __TYPESET(self, f_sub, "__sub__", val)
void ks_type_set_mul(ks_type self, kso val)  __TYPESET(self, f_mul, "__mul__", val)
void ks_type_set_div(ks_type self, kso val)  __TYPESET(self, f_div, "__div__", val)
void ks_type_set_mod(ks_type self, kso val)  __TYPESET(self, f_mod, "__mod__", val)
void ks_type_set_pow(ks_type self, kso val)  __TYPESET(self, f_pow, "__pow__", val)

void ks_type_set_lt(ks_type self, kso val)  __TYPESET(self, f_lt, "__lt__", val)
void ks_type_set_le(ks_type self, kso val)  __TYPESET(self, f_le, "__le__", val)
void ks_type_set_gt(ks_type self, kso val)  __TYPESET(self, f_gt, "__gt__", val)
void ks_type_set_ge(ks_type self, kso val)  __TYPESET(self, f_ge, "__ge__", val)
void ks_type_set_eq(ks_type self, kso val)  __TYPESET(self, f_eq, "__eq__", val)
void ks_type_set_ne(ks_type self, kso val)  __TYPESET(self, f_ne, "__ne__", val)


TFUNC(type, free) {
    ks_type self = (ks_type)args[0];

    KSO_DECREF(self->__dict__);

    ks_free(self);

    return KSO_NONE;
}

/* exporting functionality */

struct ks_type T_type, *ks_T_type = &T_type;

void ks_init__type() {
    T_type = (struct ks_type) {
        KSO_BASE_INIT(ks_T_type)
        .__dict__ = ks_dict_new_empty(),
    };

    ks_type_set_namec(ks_T_type, "type");

    ks_dict_set_cstrl(ks_T_type->__dict__, "__name__", 8, (kso)ks_T_type->name);
    ks_dict_set_cstrl(ks_T_type->__dict__, "__type__", 8, (kso)ks_T_type->type);

    kso _myf = (kso)ks_cfunc_new(type_getattr_);
    ks_dict_set_cstrl(ks_T_type->__dict__, "__getattr__", 11, _myf);
    ks_T_type->f_getattr = _myf;
    KSO_DECREF(_myf);
    _myf = (kso)ks_cfunc_new(type_setattr_);
    ks_dict_set_cstrl(ks_T_type->__dict__, "__setattr__", 11, _myf);
    ks_T_type->f_setattr = _myf;
    KSO_DECREF(_myf);

    _myf = (kso)ks_cfunc_new(type_free_);
    ks_dict_set_cstrl(ks_T_type->__dict__, "__free__", 8, _myf);
    ks_T_type->f_free = _myf;
    KSO_DECREF(_myf);

}



