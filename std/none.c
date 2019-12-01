// none.c - implementation of the none-type for kscript
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//


#include "ks_std.h"

// declare a list type
TYPE_DECL(None);


// constructor, takes its target as the first argument
TYPE_FUNC(None, __init__) {
    if (args_n != 1) {
        ctx->cexc = ks_obj_new_exception_fmt("None.__init__(self): requires 1 arguments, was given %d", args_n);
        return NULL;
    }

    ks_obj self = args[0];
    self->type = None_typeid;

    // return self from the constructor
    return self;
}

REGISTER_FUNC(None) {
    TYPE_REGISTER(None, 2, (struct ks_dict_kvp_cp[]){
        { "__name__", ks_obj_new_str(KS_STR_CONST("None")) },
        { "__init__", ks_obj_new_cfunc(TYPE_FUNC_NAME(None, __init__)) }
    });

    return 0;
}



