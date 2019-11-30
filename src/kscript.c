// kscript.c - the main commandline interface to the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


// return (string) representation of a single argument
ks_obj ks_std_repr(int args_n, ks_obj* args) {
    if (args_n != 1) {
        ks_error("repr takes %d args, was given %d", 1, args_n);
        return ks_obj_new_none();
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
        ks_str_append_c(&ret->_str, '"');
        ks_str_append(&ret->_str, A->_str);
        ks_str_append_c(&ret->_str, '"');

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
            ks_obj subrepr = ks_std_repr(1, &A->_dict.vals[i]);
            ks_str_append(&ret->_str, subrepr->_str);
            ks_obj_free(subrepr);

        }

        ks_str_append_c(&ret->_str, '}');
    } else {
        ks_error("repr given unknown type (id: %d)", A->type);
    }

    return ret;
}

// print all arguments as string representations, joined by spaces
ks_obj ks_std_print(int args_n, ks_obj* args) {

    int i;
    for (i = 0; i < args_n; ++i) {
        ks_obj repr = ks_std_repr(1, &args[i]);

        if (i != 0) printf(" ");
        if (repr->type == KS_TYPE_STR) {
            printf("%s", repr->_str._);
        } else {
            ks_error("Internal error; `repr` gave a non-string");
            ks_obj_free(repr);
            return ks_obj_new_none();
        }

        ks_obj_free(repr);
    }

    // end with a newline
    printf("\n");

    return ks_obj_new_none();
}

// add(A, B) == A+B
ks_obj ks_std_add(int args_n, ks_obj* args) {
    if (args_n != 2) {
        ks_error("add takes %d args, was given %d", 2, args_n);
        return ks_obj_new_none();
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_int(args[0]->_int + args[1]->_int);
}

// mul(A, B) == A*B
ks_obj ks_std_mul(int args_n, ks_obj* args) {
    if (args_n != 2) {
        ks_error("add takes %d args, was given %d", 2, args_n);
        return ks_obj_new_none();
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_int(args[0]->_int * args[1]->_int);
}
// pow(A, B) == A**B
ks_obj ks_std_pow(int args_n, ks_obj* args) {
    if (args_n != 2) {
        ks_error("add takes %d args, was given %d", 2, args_n);
        return ks_obj_new_none();
    }

    // for now, just implement integers, and assume all arguments are integers
    return ks_obj_new_int((ks_int)(pow(args[0]->_int, args[1]->_int)));
}


ks_obj ks_eval(ks_ast ast) {
    if (ast->type == KS_AST_CONST_INT) {
        return ks_obj_new_int(ast->_int);
    } else if (ast->type == KS_AST_CALL) {
        // evaluate arguments
        ks_obj* args = malloc(sizeof(ks_obj) * ast->_call.args_n);

        // compute all arguments first, then call the function
        int i;
        for (i = 0; i < ast->_call.args_n; ++i) {
            args[i] = ks_eval(ast->_call.args[i]);
        }

        // evaluate the function
        ks_obj func = ks_eval(ast->_call.lhs);

        // call the function
        ks_obj res = func->_cfunc(ast->_call.args_n, args);

        free(args);
        return res;
    } else if (ast->type == KS_AST_CONST) {
        return ast->_const;
    } else {
        ks_error("Unknown type!");
    }
}

int main(int argc, char** argv) {

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

    return 0;
}


