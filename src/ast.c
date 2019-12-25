/* ast.c - implementation of abstract syntax trees

Mostly, this just records computation as a hierarchy of operations. This is not very efficient to execute,
  so, we can generate bytecode from the AST, and then execute that


*/

#include "kscript.h"


/* constructing */

ks_ast ks_ast_new_const_true() {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_CONST_TRUE;
    return ret;
}
ks_ast ks_ast_new_const_false() {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_CONST_FALSE;
    return ret;
}

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

ks_ast ks_ast_new_ret(ks_ast val) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_RETURN;
    ret->_val = val;
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

/*
ks_ast ks_ast_new_funcdef(ks_str name) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_FUNCDEF;
    ret->_funcdef.name = ks_str_dup(name);
    ret->_funcdef.n_params = 0;
    ret->_funcdef.param_names = NULL;
    ret->_funcdef.body = NULL;
    return ret;
}*/

void ks_ast_funcdef_add_param(ks_ast funcdef, ks_str param_name) {
    funcdef->_funcdef.param_names = ks_realloc(funcdef->_funcdef.param_names, ++funcdef->_funcdef.n_params * sizeof(*funcdef->_funcdef.param_names));
    funcdef->_funcdef.param_names[funcdef->_funcdef.n_params - 1] = ks_str_dup(param_name);
}



/* CODE GENERATION */

// code-generating clearing stack
//#define CG_CLEAR() ksb_clear(to);
#define CG_CLEAR()

void _ks_ast_codegen(ks_ast ast, kso_code to) {

    //printf("HERE:%d\n", 5);

    if (ast->type == KS_AST_CONST_TRUE) {
        ksc_const_true(to);
    } else if (ast->type == KS_AST_CONST_FALSE) {
        ksc_const_false(to);
    } else if (ast->type == KS_AST_CONST_INT) {
        ksc_const_int(to, ast->_int);
    } else if (ast->type == KS_AST_CONST_STR) {
        ksc_const_str(to, ast->_str);
    } else if (ast->type == KS_AST_VAR) {
        ksc_load(to, ast->_str);
    }

    // generate code for a binary operator

    //#define CODEGEN_BOP(_boptype, _bopfunc) 
    #define CODEGEN_BOP(_boptype, _bopfunc) else if (ast->type == _boptype) { \
        ks_ast_codegen(ast->_bop.L, to); \
        ks_ast_codegen(ast->_bop.R, to); \
        _bopfunc(to); \
    }
    CODEGEN_BOP(KS_AST_BOP_ADD, ksc_add)
    /*CODEGEN_BOP(KS_AST_BOP_SUB, ksc_sub)
    CODEGEN_BOP(KS_AST_BOP_MUL, ksc_mul)
    CODEGEN_BOP(KS_AST_BOP_DIV, ksc_div)
    CODEGEN_BOP(KS_AST_BOP_MOD, ksc_mod)
    CODEGEN_BOP(KS_AST_BOP_POW, ksc_pow)
    CODEGEN_BOP(KS_AST_BOP_LT, ksc_lt)
    CODEGEN_BOP(KS_AST_BOP_GT, ksc_gt)
    CODEGEN_BOP(KS_AST_BOP_EQ, ksc_eq)*/


    else if (ast->type == KS_AST_BOP_ASSIGN) {
        //rc |= ks_ast_codegen(ast->_bop.L, to);
        ks_ast_codegen(ast->_bop.R, to);

        if (ast->_bop.L->type == KS_AST_VAR) {
            // assignable
            ksc_store(to, ast->_bop.L->_str);
        } else {
            ks_err_add_str(KS_STR_CONST("Tried assigning to the wrong kind of AST (expected a var)"));
        }
    } else if (ast->type == KS_AST_IF) {
        // compute the conditional, so it is at the top of the stack
        ks_ast_codegen(ast->_if.cond, to);

        // now, we will jump forward if it is false
        int p_jmpf = to->bc_n;
        ksc_jmpf(to, -1);

        // position of the body, fill this in later
        int p_body = to->bc_n;
        ks_ast_codegen(ast->_if.body, to);

        int32_t diff = to->bc_n - p_body;
        printf("%d\n", diff);
        // now, correct it
        struct ks_bc_jmp* i_jmpf = (struct ks_bc_jmp*)(to->bc + p_jmpf);
        i_jmpf->relamt = diff;

/*

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
*/
    } else if (ast->type == KS_AST_CALL) {
        ks_ast_codegen(ast->_call.func, to);

        int i;
        for (i = 0; i < ast->_call.args_n; ++i) {
            ks_ast_codegen(ast->_call.args[i], to);
        }
        ksc_call(to, ast->_call.args_n);

    } else if (ast->type == KS_AST_BLOCK) {

        int i;
        for (i = 0; i < ast->_block.sub_n; ++i) {
            // each should leave the stack resolved
            ks_ast_codegen(ast->_block.subs[i], to);
            CG_CLEAR();
        }
        /*
    } else if (ast->type == KS_AST_FUNCDEF) {
        // position of body
        int pos = to->bc_n;
        rc |= ks_ast_codegen(ast->_funcdef.body, to);

        ks_prog_lbl_add(to, ast->_funcdef.name, pos);

    } */
    } else {

        ks_err_add_str_fmt("Unexpected AST type %d", ast->type);
    }

    return;
}

void ks_ast_codegen(ks_ast ast, kso_code to) {
    return _ks_ast_codegen(ast, to);
}
