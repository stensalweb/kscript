// ast.c - implementation of the `ks_ast` type
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

ks_ast ks_ast_new_int(ks_int val) {
    ks_ast ret = (ks_ast)malloc(sizeof(struct ks_ast));
    ret->type = KS_AST_CONST_INT;
    ret->_int = val;
    return ret;
}

ks_ast ks_ast_new_float(ks_float val) {
    ks_ast ret = (ks_ast)malloc(sizeof(struct ks_ast));
    ret->type = KS_AST_CONST_FLOAT;
    ret->_float = val;
    return ret;
}

ks_ast ks_ast_new_str(ks_str val) {
    ks_ast ret = (ks_ast)malloc(sizeof(struct ks_ast));
    ret->type = KS_AST_CONST_STR;
    ret->_str = KS_STR_EMPTY;
    ks_str_copy(&ret->_str, val);
    return ret;
}

ks_ast ks_ast_new_var(ks_str name) {
    ks_ast ret = (ks_ast)malloc(sizeof(struct ks_ast));
    ret->type = KS_AST_VAR;
    ret->_str = KS_STR_EMPTY;
    ks_str_copy(&ret->_var, name);
    return ret;
}

ks_ast ks_ast_new_call(ks_ast lhs, int args_n, ks_ast* args) {
    ks_ast ret = (ks_ast)malloc(sizeof(struct ks_ast));
    ret->type = KS_AST_CALL;
    ret->_call.lhs = lhs;
    if (args_n <= 0) {
        ret->_call.args_n = 0;
        ret->_call.args = NULL;
    } else {
        // copy arguments
        ret->_call.args_n = args_n;
        ret->_call.args = malloc(sizeof(ks_ast) * args_n);
        int i;
        for (i = 0; i < args_n; ++i) {
            ret->_call.args[i] = args[i];
        }
    }
    
    return ret;
}

ks_ast ks_ast_new_assign(ks_ast lhs, ks_ast rhs) {
    ks_ast ret = (ks_ast)malloc(sizeof(struct ks_ast));
    ret->type = KS_AST_ASSIGN;
    ret->_assign.lhs = lhs;
    ret->_assign.rhs = rhs;
    return ret;
}

// frees resources (recursively)
void ks_ast_free(ks_ast ast) {

    if (ast != NULL) {
        if (ast->type == KS_AST_CONST_STR) {
            ks_str_free(&ast->_str);
        } else if (ast->type == KS_AST_VAR) {
            ks_str_free(&ast->_var);
        } else if (ast->type == KS_AST_CALL) {

            // free callee
            ks_ast_free(ast->_call.lhs);

            int i;
            // free all arguments
            for (i = 0; i < ast->_call.args_n; ++i) {
                ks_ast_free(ast->_call.args[i]);
            }
            // free the memory itself
            free(ast->_call.args);
        } else if (ast->type == KS_AST_ASSIGN) {
            ks_ast_free(ast->_assign.lhs);
            ks_ast_free(ast->_assign.rhs);
        }

        free(ast);
    }
}






