// kscript.c - the main commandline interface to the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


// prints the stack trace out
ks_obj ks_std_stacktrace(ks_ctx ctx, int args_n, ks_obj* args) {
    if (args_n != 0) {
        ctx->cexc = ks_obj_new_exception_fmt("'__stacktrace' given %d arguments, expected %d", args_n, 0);
        return NULL;
    }

    int i;
    for (i = 0; i < ctx->call_stk_n; ++i) {
        int j;
        for (j = 0; j < 2 * i; ++j) {
            printf(" ");
        }

        printf("%s\n", ctx->call_stk_names[i]._);
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_none();
}


int main(int argc, char** argv) {

    // create our global context
    ks_ctx ctx = ks_ctx_new();

    ks_obj std = ks_module_load(ctx, KS_STR_CONST("std"));

    // instantiate the global values
    ks_scope globals = ks_scope_new(NULL);
    // add standard module
    ks_dict_set(&globals->locals, KS_STR_CONST("std"), std);

    // start out with globals
    ks_ctx_push(ctx, KS_STR_CONST("global"), globals);


    // example main method: std.print(42, 45)
    ks_bc f_main = KS_BC_EMPTY;
    ks_bc_add(&f_main, ks_bc_new_int(42));
    ks_bc_add(&f_main, ks_bc_new_int(45));
    ks_bc_add(&f_main, ks_bc_new_load(KS_STR_CONST("std")));
    ks_bc_add(&f_main, ks_bc_new_attr(KS_STR_CONST("print")));
    ks_bc_add(&f_main, ks_bc_new_call(2));
    ks_bc_add(&f_main, ks_bc_new_ret_none());

    // execute it
    ks_exec(ctx, f_main.inst);

    return 0;
}


