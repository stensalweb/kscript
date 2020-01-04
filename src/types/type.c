/* types/type.c - represents a type-type */

#include "ks_common.h"


TFUNC(type, getattr) {
    #define SIG "type.__getattr__(self, attr)"
    REQ_N_ARGS(2);
    ks_type self = (ks_type)args[0];
    REQ_TYPE("self", self, ks_T_type);
    ks_str attr = (ks_str)args[1];
    REQ_TYPE("attr", attr, ks_T_str);

    kso ret = NULL;

    /**/ if (KS_STR_EQ_CONST(attr, "__name__")) ret = (kso)self->name;
    else if (KS_STR_EQ_CONST(attr, "__type__")) ret = (kso)self->type;
    else if (KS_STR_EQ_CONST(attr, "__repr__")) ret = self->f_repr;
    else if (KS_STR_EQ_CONST(attr, "__str__") ) ret = self->f_str;
    else if (KS_STR_EQ_CONST(attr, "__getitem__") ) ret = self->f_getitem;
    else if (KS_STR_EQ_CONST(attr, "__setitem__") ) ret = self->f_setitem;

    if (ret == NULL) {
        // keyerror
        return kse_fmt("KeyError: %R", attr);
    } else {
        return ret;
    }
    #undef SIG
}


/* exporting functionality */


struct ks_type T_type, *ks_T_type = &T_type;

void ks_init__type() {

    T_type = (struct ks_type) {
        KS_TYPE_INIT("type")

        .f_getattr = (kso)ks_cfunc_newref(type_getattr_)

    };
}



