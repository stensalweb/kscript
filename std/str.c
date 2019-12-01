// str.c - implementation of the main `str` type in kscript
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//


#include "ks_std.h"

// declare a list type
TYPE_DECL(str);


// constructor, takes its target as the first argument
TYPE_FUNC(str, __init__) {
    if (args_n < 1 || args_n > 2) {
        ctx->cexc = ks_obj_new_exception_fmt("str.__init__(self, val=\"\"): requires (1 or 2) arguments, was given %d", args_n);
        return NULL;
    }

    ks_obj self = args[0];
    self->type = str_typeid;

    if (args_n == 2) {
        ks_obj val = args[1];
        if (val->type == KS_TYPE_INT) {
            // format it
            self->_str = ks_str_fmt("%ld", val->_int);
        } else if (val->type == KS_TYPE_STR) {
            // duplicate
            self->_str = ks_str_dup(val->_str);
        } else {
            ctx->cexc = ks_obj_new_exception_fmt("str.__init__(self, val): type of 'val' was invalid (got '%s')", ctx->type_names[val->type]._);
            return NULL;
        }
    } else {
        // default to empty string
        self->_str = KS_STR_EMPTY;
    }
    // return self from the constructor
    return self;
}


// free, frees all resources
TYPE_FUNC(str, __free__) {
    if (args_n != 1) {
        ctx->cexc = ks_obj_new_exception_fmt("str.__free__(self): requires 1 arguments, was given %d", args_n);
        return NULL;
    }

    ks_obj self = args[0];
    
    // free the string
    ks_str_free(&self->_str);

    return NULL;
}
REGISTER_FUNC(str) {
    TYPE_REGISTER(str, 2, (struct ks_dict_kvp_cp[]){
        { "__name__", ks_obj_new_str(KS_STR_CONST("str")) },
        { "__init__", ks_obj_new_cfunc(TYPE_FUNC_NAME(str, __init__)) },
        { "__free__", ks_obj_new_cfunc(TYPE_FUNC_NAME(str, __free__)) }
    });

    return 0;
}



