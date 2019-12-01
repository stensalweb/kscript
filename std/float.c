// float.c - implementation of `float` type
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//


#include "ks_std.h"

TYPE_DECL(float);

// constructor, takes its target as the first argument
TYPE_FUNC(float, __init__) {
    if (args_n < 1 || args_n > 2) {
        ctx->cexc = ks_obj_new_exception_fmt("float.__init__(self, val=0.0): requires (1 or 2) arguments, was given %d", args_n);
        return NULL;
    }

    ks_obj self = args[0];
    self->type = float_typeid;

    if (args_n == 2) {
        ks_obj val = args[1];
        if (val->type == KS_TYPE_INT) {
            self->_float = (ks_float)val->_int;
        } else if (val->type == KS_TYPE_STR) {
            // parse a string
            self->_int = (ks_float)atof(val->_str._);
        } else {
            ctx->cexc = ks_obj_new_exception_fmt("float.__init__(self, val): type of 'val' was invalid (got '%s')", ctx->type_names[val->type]._);
            return NULL;
        }
    } else {
        // default to zero
        self->_float = 0.0;
    }
    // return self from the constructor
    return self;
}

REGISTER_FUNC(float) {
    TYPE_REGISTER(float, 2, (struct ks_dict_kvp_cp[]){
        { "__name__", ks_obj_new_str(KS_STR_CONST("float")) },
        { "__init__", ks_obj_new_cfunc(TYPE_FUNC_NAME(float, __init__)) }
    });

    return 0;
}



