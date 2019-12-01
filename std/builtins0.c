// builtins0.c - a bunch of builtin functions, including:
//
// * print
// * repr
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "ks_std.h"

MODU_FUNC(repr) {
    if (args_n != 1) {
        ctx->cexc = ks_obj_new_exception_fmt("repr(arg): requires 1 arguments, was given %d", args_n);
        return NULL;
    }

    ks_obj arg = args[0];
    ks_obj ret = ks_obj_new_str(KS_STR_EMPTY);

    if (arg->type == KS_TYPE_STR) {
        ks_str_append_c(&ret->_str, '"');
        ks_str_append(&ret->_str, arg->_str);
        ks_str_append_c(&ret->_str, '"');
    } else if (arg->type == KS_TYPE_INT) {
        ret->_str = ks_str_fmt("%ld", arg->_int);
    } else {
        ctx->cexc = ks_obj_new_exception_fmt("repr(arg): 'arg' was of unknown type '%s'", ctx->type_names[arg->type]._);
        return NULL;
    }

    return ret;
}

MODU_FUNC(print) {
    if (args_n < 0) {
        ctx->cexc = ks_obj_new_exception_fmt("print(args...): requires >=0 arguments, was given %d", args_n);
        return NULL;
    }

    int i;
    for (i = 0; i < args_n; ++i) {
        if (i != 0) printf(" ");
        ks_obj repr = MODU_FUNC_NAME(repr)(ctx, 1, &args[i]);

        printf("%s", repr->_str._);

        ks_obj_free(repr);
    }

    // end with a new line
    printf("\n");

    return NULL;
}

REGISTER_FUNC(builtins0) {
    FUNC_REGISTER(repr);
    FUNC_REGISTER(print);

    return 0;
}




