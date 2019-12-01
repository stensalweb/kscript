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


// return (string) representation of a single argument
ks_obj ks_std_repr(ks_ctx ctx, int args_n, ks_obj* args) {
    if (args_n != 1) {
        ks_error("repr takes %d args, was given %d", 1, args_n);
        return NULL;
    }

    // get the only argument
    ks_obj A = args[0];
    // the result we'll return
    ks_obj ret = ks_obj_new_str(KS_STR_EMPTY);

    // check type
    if (A->type == KS_TYPE_NONE) {
        ks_str_copy(&ret->_str, KS_STR_CONST("NONE"));
    } else if (A->type == KS_TYPE_INT) {
        char tmp[100];
        sprintf(tmp, "%ld", A->_int);
        ks_str_copy_cp(&ret->_str, tmp, strlen(tmp));
    } else if (A->type == KS_TYPE_FLOAT) {
        char tmp[100];
        sprintf(tmp, "%lf", A->_float);
        ks_str_copy_cp(&ret->_str, tmp, strlen(tmp));
    } else if (A->type == KS_TYPE_STR) {
        ks_str_append(&ret->_str, A->_str);
    } else if (A->type == KS_TYPE_EXCEPTION) {
        ks_str_append(&ret->_str, A->_exception);

    } else if (A->type >= KS_TYPE_CUSTOM) {
        ks_str_append_c(&ret->_str, '{');

        // add all entries of dictionary
        int i;
        for (i = 0; i < A->_dict.len; ++i) {
            if (i != 0) { 
                ks_str_append(&ret->_str, KS_STR_CONST(", "));
            }

            ks_str_append(&ret->_str, A->_dict.keys[i]);
            ks_str_append(&ret->_str, KS_STR_CONST(": "));

            // get repr of subobject
            ks_obj subrepr = ks_std_repr(ctx, 1, &A->_dict.vals[i]);
            ks_str_append(&ret->_str, subrepr->_str);
            ks_obj_free(subrepr);

        }

        ks_str_append_c(&ret->_str, '}');
    } else {
        ctx->cexc = ks_obj_new_exception_fmt("'repr': Argument was unknown type (id: %d)", A->type);
        return NULL;
    }

    return ret;
}

// print all arguments as string representations, joined by spaces
ks_obj ks_std_print(ks_ctx ctx, int args_n, ks_obj* args) {


    int i;
    for (i = 0; i < args_n; ++i) {
        ks_obj repr = ks_std_repr(ctx, 1, &args[i]);

        if (i != 0) printf(" ");
        if (repr->type == KS_TYPE_STR) {
            printf("%s", repr->_str._);
        } else {
            ks_obj_free(repr);
            ctx->cexc = ks_obj_new_exception_fmt("InternalError: 'repr' didn't give a 'str'");
            return NULL;
            //return ks_obj_new_none();
        }

        ks_obj_free(repr);
    }

    // end with a newline
    printf("\n");

    return ks_obj_new_none();
}

// add(A, B) == A+B
ks_obj ks_std_add(ks_ctx ctx, int args_n, ks_obj* args) {
    if (args_n != 2) {
        ctx->cexc = ks_obj_new_exception_fmt("'add' given %d arguments, expected %d", args_n, 2);
        return NULL;
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_int(args[0]->_int + args[1]->_int);
}

// mul(A, B) == A*B
ks_obj ks_std_mul(ks_ctx ctx, int args_n, ks_obj* args) {
    if (args_n != 2) {
        ctx->cexc = ks_obj_new_exception_fmt("'mul' given %d arguments, expected %d", args_n, 2);
        return NULL;
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_int(args[0]->_int * args[1]->_int);
}
// pow(A, B) == A**B
ks_obj ks_std_pow(ks_ctx ctx, int args_n, ks_obj* args) {
    if (args_n != 2) {
        ctx->cexc = ks_obj_new_exception_fmt("'pow' given %d arguments, expected %d", args_n, 2);
        return NULL;
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_int((ks_int)(pow(args[0]->_int, args[1]->_int)));
}

int main(int argc, char** argv) {
/*
    // numbers
    ks_ast n2 = ks_ast_new_int(2);
    ks_ast n3 = ks_ast_new_int(3);
    ks_ast n5 = ks_ast_new_int(5);

    // our other variables (but here, represented as ASTs, not objects)
    ks_ast b = ks_ast_new_int(2);
    ks_ast c = ks_ast_new_int(3);

    // the functions we use (we wrap them as an AST, but as a constant, so they just
    //   evaluate to the function object when `eval` is called on the AST)
    ks_ast func_add = ks_ast_new_const(ks_obj_new_cfunc(ks_std_add));
    ks_ast func_mul = ks_ast_new_const(ks_obj_new_cfunc(ks_std_mul));
    ks_ast func_pow = ks_ast_new_const(ks_obj_new_cfunc(ks_std_pow));
    ks_ast func_print = ks_ast_new_const(ks_obj_new_cfunc(ks_std_print));

    // construct expression manually... we'll get to parsing in a bit
    ks_ast expr = ks_ast_new_call(func_print, 1, (ks_ast[]){
        ks_ast_new_call(func_add, 2, (ks_ast[]){ 
            ks_ast_new_call(func_mul, 2, (ks_ast[]){
                n3,
                ks_ast_new_call(func_pow, 2, (ks_ast[]){
                    b,
                    n5
                })
            }),
            c
        }
    )});

    // evaluate it
    ks_eval(expr);

    // since this expression contains all others, the `free` works on all at once
    ks_ast_free(expr);

*/
    // create our global context
    ks_ctx ctx = ks_ctx_new();

    void* lib_ptr = dlopen("libMOD_std.so", RTLD_NOW | RTLD_GLOBAL);

    if (lib_ptr == NULL) {
        printf("Couldn't load lib! (%s)\n", dlerror());
    }

    struct ks_module_loader* kml = dlsym(lib_ptr, "module_loader");

    int status = kml->f_load(ctx);

    if (status != 0) {
        printf("couldn't load!\n");
    } else {
        printf("loaded!\n");
    }

    printf("%s\n", ctx->type_names[1]._);

    // instantiate the global values
    ks_scope globals = ks_scope_new(NULL);

    // add builtins
    ks_dict_set(&globals->locals, KS_STR_CONST("__stacktrace"), ks_obj_new_cfunc(ks_std_stacktrace));
    ks_dict_set(&globals->locals, KS_STR_CONST("print"), ks_obj_new_cfunc(ks_std_print));
    ks_dict_set(&globals->locals, KS_STR_CONST("add"), ks_obj_new_cfunc(ks_std_add));
    ks_dict_set(&globals->locals, KS_STR_CONST("mul"), ks_obj_new_cfunc(ks_std_mul));
    ks_dict_set(&globals->locals, KS_STR_CONST("pow"), ks_obj_new_cfunc(ks_std_pow));

    // start out with globals
    ks_ctx_push(ctx, KS_STR_CONST("global"), globals);


    ks_bc f_bc = KS_BC_EMPTY, f_main = KS_BC_EMPTY;

    // define a function (f)
    // like: f(a, b) -> print(a+b)
    int f_i = ks_bc_add(&f_bc, ks_bc_new_load(KS_STR_CONST("a")));
    ks_bc_add(&f_bc, ks_bc_new_load(KS_STR_CONST("b")));
    ks_bc_add(&f_bc, ks_bc_new_load(KS_STR_CONST("add")));
    ks_bc_add(&f_bc, ks_bc_new_call(2));
    ks_bc_add(&f_bc, ks_bc_new_load(KS_STR_CONST("print")));
    ks_bc_add(&f_bc, ks_bc_new_call(1));
    ks_bc_add(&f_bc, ks_bc_new_ret_none());

    // define the function
    ks_kfunc f = KS_KFUNC_EMPTY;
    ks_kfunc_add_param(&f, KS_STR_CONST("a"));
    ks_kfunc_add_param(&f, KS_STR_CONST("b"));
    f.inst = &f_bc.inst[f_i];


    // add it to the globals
    ks_dict_set(&globals->locals, KS_STR_CONST("f"), ks_obj_new_kfunc(f));

    // now, define our 'main' function, it just calls `f` a few times

    int fmain_i = ks_bc_add(&f_main, ks_bc_new_int(2));
    ks_bc_add(&f_main, ks_bc_new_load(KS_STR_CONST("print"))); 
    ks_bc_add(&f_main, ks_bc_new_call(1));

/*
    ks_bc_add(&f_main, ks_bc_new_int(4));
    ks_bc_add(&f_main, ks_bc_new_int(5));
    ks_bc_add(&f_main, ks_bc_new_load(KS_STR_CONST("f")));
    ks_bc_add(&f_main, ks_bc_new_call(2));
*/
    // now, throw(42)
    //ks_bc_add(&f_main, ks_bc_new_int(42));
    //ks_bc_add(&f_main, ks_bc_new_throw());

    ks_bc_add(&f_main, ks_bc_new_ret_none());

    // now, construct the function
    ks_kfunc fmain = KS_KFUNC_EMPTY;
    fmain.inst = &f_main.inst[fmain_i];


    //ks_exec_kfunc(ctx, fmain, 0, NULL);

    ks_exec(ctx, f_main.inst);

    //ks_obj lit = ks_obj_new_int(32);

    //ks_dict_get(&globals->locals, KS_STR_CONST("print"))->_cfunc(1, &lit);

    //printf("SIZE: %lu\n", sizeof(struct ks_obj));

    return 0;
}


