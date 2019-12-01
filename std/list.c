// list.c - implements the 'list' type for kscript
//
// internally, this uses the `ks_stk` type to do memory management.
//
// This contains the methods to actually initialize everything
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "ks_std.h"

// declare a list type
TYPE_DECL(list);

// constructor, takes its target as the first argument
TYPE_FUNC(list, __init__) {
    if (args_n < 1) {
        ctx->cexc = ks_obj_new_exception_fmt("list.__init__(self, args...): requires >=%d arguments, was given %d", 1, args_n);
        return NULL;
    }

    ks_obj self = args[0];
    self->type = list_typeid;
    self->_ptr = malloc(sizeof(ks_stk));

    // push all args (not the first)
    ks_stk stk = KS_STK_EMPTY;
    int i;
    for (i = 1; i < args_n; ++i) {
        ks_stk_push(&stk, args[i]);
    }

    // set the pointer to it
    *(ks_stk*)self->_ptr = stk;

    // return self from the constructor
    return self;
}

// getter, takes self, index
TYPE_FUNC(list, __get__) {
    if (args_n != 2) {
        ctx->cexc = ks_obj_new_exception_fmt("list.__get__(self, idx): requires %d arguments, was given %d", 2, args_n);
        return NULL;
    }

    ks_obj self = args[0], idx = args[1];

    if (self->type != list_typeid) {
        ctx->cexc = ks_obj_new_exception_fmt("list.__get__(self, idx): 'self' should be of type 'list', but is actually of type '%s'", ctx->type_names[self->type]._);
        return NULL;
    }

    if (idx->type == KS_TYPE_INT) {
        return ((ks_stk*)self->_ptr)->vals[idx->_int];
    } else {
        // only integer indexes are supported at the moment.
        // TODO: slices
        ctx->cexc = ks_obj_new_exception_fmt("list.__get__(self, idx): idx must be 'int', but got '%s'", ctx->type_names[idx->type]._);
        return NULL;
    }
}

// free, frees the list
TYPE_FUNC(list, __free__) {
    if (args_n != 1) {
        ctx->cexc = ks_obj_new_exception_fmt("list.__free__(self): requires %d arguments, was given %d", 1, args_n);
        return NULL;
    }

    ks_obj self = args[0];

    if (self->type != list_typeid) {
        ctx->cexc = ks_obj_new_exception_fmt("list.__free__(self): 'self' should be of type 'list', but is actually of type '%s'", ctx->type_names[self->type]._);
        return NULL;
    }

    ks_stk_free((ks_stk*)self->_ptr);
    free(self->_ptr);

    return NULL;
}

REGISTER_FUNC(list) {
    TYPE_REGISTER(list, 4, (struct ks_dict_kvp_cp[]){
        { "__name__", ks_obj_new_str(KS_STR_CONST("list")) },
        { "__init__", ks_obj_new_cfunc(TYPE_FUNC_NAME(list, __init__)) },
        { "__get__", ks_obj_new_cfunc(TYPE_FUNC_NAME(list, __get__)) },
        { "__free__", ks_obj_new_cfunc(TYPE_FUNC_NAME(list, __free__)) }
    });

    return 0;
}

