// bool.c - implementation of the main `bool` type in kscript
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//


#include "ks_std.h"

TYPE_DECL(bool);

// constructor, takes its target as the first argument
TYPE_FUNC(bool, __init__) {
    if (args_n < 1 || args_n > 2) {
        ctx->cexc = ks_obj_new_exception_fmt("bool.__init__(self, val=0): requires (1 or 2) arguments, was given %d", args_n);
        return NULL;
    }

    ks_obj self = args[0];
    self->type = bool_typeid;

    if (args_n == 2) {
        ks_obj val = args[1];
        if (val->type == KS_TYPE_INT) {
            self->_bool = val->_int == 0 ? false : true;
        } else if (val->type == KS_TYPE_STR) {
            self->_bool = ks_str_eq(KS_STR_CONST("true"), val->_str) ? true : false;
        } else {
            ctx->cexc = ks_obj_new_exception_fmt("bool.__init__(self, val): type of 'val' was invalid (got '%s')", ctx->type_names[val->type]._);
            return NULL;
        }
    } else {
        // default to false
        self->_bool = false;
    }
    // return self from the constructor
    return self;
}

REGISTER_FUNC(bool) {
    TYPE_REGISTER(bool, 2, (struct ks_dict_kvp_cp[]){
        { "__name__", ks_obj_new_str(KS_STR_CONST("bool")) },
        { "__init__", ks_obj_new_cfunc(TYPE_FUNC_NAME(bool, __init__)) }
    });

    return 0;
}



