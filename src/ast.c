/* ast.c - implementation of abstract syntax trees


*/

#include "kscript.h"



ks_ast ks_ast_new_const_int(ks_int val) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_CONST_INT;
    ret->_int = val;
    return ret;
}

ks_ast ks_ast_new_const_float(ks_float val) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_CONST_FLOAT;
    ret->_float = val;
    return ret;
}

ks_ast ks_ast_new_const_str(ks_str val) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_CONST_STR;
    ret->_str = ks_str_dup(val);
    return ret;
}

ks_ast ks_ast_new_var(ks_str name) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_VAR;
    ret->_str = ks_str_dup(name);
    return ret;
}
ks_ast ks_ast_new_call(ks_ast func, int args_n, ks_ast* args) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_CALL;
    ret->_call.func = func;
    ret->_call.args_n = args_n;
    if (args_n > 0) {
        ret->_call.args = ks_malloc(sizeof(*ret->_call.args) * args_n);
        memcpy(ret->_call.args, args, sizeof(*ret->_call.args) * args_n);
    } else {
        ret->_call.args = NULL;
    }
    return ret;
}

ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = bop_type;
    ret->_bop.L = L;
    ret->_bop.R = R;
    return ret;
}

ks_ast ks_ast_new_block(int sub_n, ks_ast* subs) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_BLOCK;
    ret->_block.sub_n = sub_n;
    ret->_block.subs = ks_malloc(sizeof(ks_ast) * sub_n);
    if (subs != NULL) memcpy(ret->_block.subs, subs, sizeof(ks_ast) * sub_n);
    return ret;
}

int ks_ast_block_add(ks_ast block, ks_ast sub) {
    int idx = block->_block.sub_n++;
    block->_block.subs = ks_realloc(block->_block.subs, block->_block.sub_n * sizeof(ks_ast));
    block->_block.subs[idx] = sub;
    return idx;
}

ks_ast ks_ast_new_if(ks_ast cond, ks_ast body) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_IF;
    ret->_if.cond = cond;
    ret->_if.body = body;
    return ret;
}


ks_ast ks_ast_new_while(ks_ast cond, ks_ast body) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_WHILE;
    ret->_while.cond = cond;
    ret->_while.body = body;
    return ret;
}

// code-generating clearing stack
//#define CG_CLEAR() ksb_clear(to);
#define CG_CLEAR()

int _ks_ast_codegen(ks_ast ast, ks_prog* to) {
    int rc = 0;
    if (ast->type == KS_AST_CONST_INT) {
        ksb_int64(to, ast->_int);        
    } else if (ast->type == KS_AST_CONST_FLOAT) {
        ksb_float(to, ast->_float);
    } else if (ast->type == KS_AST_CONST_STR) {
        ksb_str(to, ast->_str);
    } else if (ast->type == KS_AST_VAR) {
        ksb_load(to, ast->_str);
    } else if (ast->type == KS_AST_BOP_ADD) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_add(to);
    } else if (ast->type == KS_AST_BOP_SUB) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_sub(to);
    } else if (ast->type == KS_AST_BOP_MUL) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_mul(to);
    } else if (ast->type == KS_AST_BOP_DIV) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_div(to);
    } else if (ast->type == KS_AST_BOP_MOD) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_mod(to);
    } else if (ast->type == KS_AST_BOP_POW) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_pow(to);

    } else if (ast->type == KS_AST_BOP_LT) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_lt(to);
    } else if (ast->type == KS_AST_BOP_GT) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_gt(to);
    } else if (ast->type == KS_AST_BOP_EQ) {
        rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);
        ksb_eq(to);

    } else if (ast->type == KS_AST_BOP_ASSIGN) {
        //rc |= ks_ast_codegen(ast->_bop.L, to);
        rc |= ks_ast_codegen(ast->_bop.R, to);

        if (ast->_bop.L->type == KS_AST_VAR) {
            // assignable
            ksb_store(to, ast->_bop.L->_str);
        } else {
            ks_error("Tried assigning to wrong kind of AST (%d)", ast->type);

            rc |= 1;
        }

    } else if (ast->type == KS_AST_IF) {
        rc |= ks_ast_codegen(ast->_if.cond, to);
        // position of conditional jump
        int p_jmpf = ksb_jmpf(to, -1);

        // position of the body, fill this in later
        int p_body = to->bc_n;

        rc |= ks_ast_codegen(ast->_if.body, to);

        int32_t diff = to->bc_n - p_body;
        // now, correct it
        struct ks_bc_jmp* i_jmpf = (struct ks_bc_jmp*)(to->bc + p_jmpf);
        i_jmpf->relamt = diff;


    } else if (ast->type == KS_AST_WHILE) {
        // position of the condition/start of loop
        int p_cond = to->bc_n;
        rc |= ks_ast_codegen(ast->_while.cond, to);
        // position of conditional jump
        int p_jmpf = ksb_jmpf(to, -1);
        // the struct representing the conditional

        // position of the body, fill this in later
        int p_body = to->bc_n;

        rc |= ks_ast_codegen(ast->_while.body, to);

        // now, jump to start of loop and try again
        int32_t diff = p_cond - to->bc_n;
        ksb_jmp(to, diff - sizeof(struct ks_bc_jmp));

        // diff = to->bc_n - p_body;
        // now, replace the entry with the correct one
        struct ks_bc_jmp* i_jmpf = (struct ks_bc_jmp*)(to->bc + p_jmpf);
        i_jmpf->relamt = to->bc_n - p_body;

    } else if (ast->type == KS_AST_CALL) {
        int i;
        for (i = 0; i < ast->_call.args_n; ++i) {
            rc |= ks_ast_codegen(ast->_call.args[i], to);
        }
        rc |= ks_ast_codegen(ast->_call.func, to);
        ksb_call(to, ast->_call.args_n);

    } else if (ast->type == KS_AST_BLOCK) {

        int i;
        for (i = 0; i < ast->_block.sub_n; ++i) {
            // each should leave the stack resolved
            rc |= ks_ast_codegen(ast->_block.subs[i], to);
            CG_CLEAR();
        }

    } else {
        ks_error("Unexpected AST type %d", ast->type);
        rc |= 1;
    }

    return rc;
}

int ks_ast_codegen(ks_ast ast, ks_prog* to) {
    return _ks_ast_codegen(ast, to);
}

