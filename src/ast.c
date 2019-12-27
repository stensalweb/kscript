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

ks_ast ks_ast_new_subscript(ks_ast obj, int args_n, ks_ast* args) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_SUBSCRIPT;
    ret->_subscript.obj = obj;
    ret->_subscript.args_n = args_n;
    if (args_n > 0) {
        ret->_subscript.args = ks_malloc(sizeof(*ret->_subscript.args) * args_n);
        memcpy(ret->_subscript.args, args, sizeof(*ret->_subscript.args) * args_n);
    } else {
        ret->_subscript.args = NULL;
    }
    return ret;
}

ks_ast ks_ast_new_list(int items_n, ks_ast* items) {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_LIST;
    ret->_list.items_n = items_n;
    if (items_n > 0) {
        ret->_list.items = ks_malloc(sizeof(*ret->_list.items) * items_n);
        memcpy(ret->_list.items, items, sizeof(*ret->_list.items) * items_n);
    } else {
        ret->_list.items = NULL;
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

ks_ast ks_ast_new_func() {
    ks_ast ret = (ks_ast)ks_malloc(sizeof(*ret));
    ret->type = KS_AST_FUNC;
    ret->_func.n_params = 0;
    ret->_func.param_names = NULL;
    ret->_func.body = NULL;
    return ret;
}

void ks_ast_func_add_param(ks_ast func, ks_str param_name) {
    func->_func.param_names = ks_realloc(func->_func.param_names, ++func->_func.n_params * sizeof(*func->_func.param_names));
    func->_func.param_names[func->_func.n_params - 1] = ks_str_dup(param_name);
}


/* tostring */

static void _pprint(ks_ast parent, ks_ast ast, ks_str* to) {

    if (ast->type == KS_AST_CONST_TRUE) {
        ks_str_cfmt(to, "%s%s", to->_, "true");
    } else if (ast->type == KS_AST_CONST_FALSE) {
        ks_str_cfmt(to, "%s%s", to->_, "false");
    } else if (ast->type == KS_AST_CONST_INT) {
        ks_str_cfmt(to, "%s%lld", to->_, ast->_int);
    } else if (ast->type == KS_AST_CONST_STR) {
        ks_str_cfmt(to, "%s\"%s\"", to->_, ast->_str._);
    } else if (ast->type == KS_AST_VAR) {
        ks_str_cfmt(to, "%s%s", to->_, ast->_str._);

    } else if (ast->type == KS_AST_BOP_ASSIGN) {
        ks_str_cfmt(to, "%s(", to->_);
        _pprint(ast, ast->_bop.L, to);
        ks_str_cfmt(to, "%s=", to->_);
        _pprint(ast, ast->_bop.R, to);
        ks_str_cfmt(to, "%s)", to->_);

    } else if (ast->type == KS_AST_FUNC) {
        ks_str_cfmt(to, "%s(", to->_);
        int i;
        for (i = 0; i < ast->_func.n_params; ++i) {
            if (i != 0) ks_str_cfmt(to, "%s, ", to->_);
            ks_str_cfmt(to, "%s%s", to->_, ast->_func.param_names[i]._);
        }
        ks_str_cfmt(to, "%s)->{", to->_);
        _pprint(ast, ast->_func.body, to);
        ks_str_cfmt(to, "%s}", to->_);
    } else if (ast->type == KS_AST_BLOCK) {
        ks_str_cfmt(to, "%s{", to->_);
        int i;
        for (i = 0; i < ast->_block.sub_n; ++i) {
            if (i != 0) ks_str_cfmt(to, "%s; ", to->_);
            _pprint(ast, ast->_block.subs[i], to);

        }
        ks_str_cfmt(to, "%s}", to->_);
    } else if (ast->type == KS_AST_CALL) {

        _pprint(ast, ast->_call.func, to);
        ks_str_cfmt(to, "%s(", to->_);
        int i;
        for (i = 0; i < ast->_call.args_n; ++i) {
            if (i != 0) ks_str_cfmt(to, "%s, ", to->_);
            _pprint(ast, ast->_call.args[i], to);
        }

        ks_str_cfmt(to, "%s)", to->_);
        
    } else {
        ks_str_cfmt(to, "%s<err>", to->_);
    }

}


void ks_ast_pprint(ks_ast ast, ks_str* to) {
    ks_str_cfmt(to, "");

    _pprint(NULL, ast, to);
}


/* CODE GENERATION */


// code-generation information
typedef struct {
    int stk_depth;
    ks_ast last;
} cgi;
// code-generating clearing stack
//#define CG_CLEAR() ksb_clear(to);
#define CG_CLEAR()

void _ks_ast_codegen(ks_ast ast, kso_code to, cgi* geni) {

    if (ast == NULL) {
        ks_err_add_str(KS_STR_CONST("given NULL ast"));
        return ;
    }

    #define DEPINC(_n) { geni->stk_depth += _n; }
    #define RESET_STK(rel) { while (geni->stk_depth > start_depth + rel) { geni->stk_depth--; ksc_discard(to); } }
    //printf("HERE:%d\n", 5);

    // record the starting depth
    int start_depth = geni->stk_depth;

    if (ast->type == KS_AST_CONST_TRUE) {
        ksc_const_true(to);
        DEPINC(1);
    } else if (ast->type == KS_AST_CONST_FALSE) {
        ksc_const_false(to);
        DEPINC(1);
    } else if (ast->type == KS_AST_CONST_INT) {
        ksc_const_int(to, ast->_int);
        DEPINC(1);
    } else if (ast->type == KS_AST_CONST_STR) {
        ksc_const_str(to, ast->_str);
        DEPINC(1);
    } else if (ast->type == KS_AST_VAR) {
        ksc_load(to, ast->_str);
        DEPINC(1);
    }

    // generate code for a binary operator

    //#define CODEGEN_BOP(_boptype, _bopfunc) 
    #define CODEGEN_BOP(_boptype, _bopfunc) else if (ast->type == _boptype) { \
        _ks_ast_codegen(ast->_bop.L, to, geni); \
        _ks_ast_codegen(ast->_bop.R, to, geni); \
        _bopfunc(to); \
        DEPINC(-1); \
    }

    CODEGEN_BOP(KS_AST_BOP_ADD, ksc_add)
    CODEGEN_BOP(KS_AST_BOP_SUB, ksc_sub)
    CODEGEN_BOP(KS_AST_BOP_MUL, ksc_mul)
    CODEGEN_BOP(KS_AST_BOP_DIV, ksc_div)
    CODEGEN_BOP(KS_AST_BOP_MOD, ksc_mod)
    CODEGEN_BOP(KS_AST_BOP_POW, ksc_pow)
    CODEGEN_BOP(KS_AST_BOP_LT, ksc_lt)
    CODEGEN_BOP(KS_AST_BOP_GT, ksc_gt)
    CODEGEN_BOP(KS_AST_BOP_EQ, ksc_eq)


    else if (ast->type == KS_AST_BOP_ASSIGN) {
        if (ast->_bop.L->type == KS_AST_VAR) {

            _ks_ast_codegen(ast->_bop.R, to, geni);

            // assignable
            ksc_store(to, ast->_bop.L->_str);

            // this is 0, since the destination is not actually an object, but encoded into the instruction
            DEPINC(0);


        } else if (ast->_bop.L->type == KS_AST_SUBSCRIPT) {
            // do a []=
            _ks_ast_codegen(ast->_bop.L->_subscript.obj, to, geni);

            int i;
            for (i = 0; i < ast->_bop.L->_subscript.args_n; ++i) {
                _ks_ast_codegen(ast->_bop.L->_subscript.args[i], to, geni);
            }

            _ks_ast_codegen(ast->_bop.R, to, geni);

            ksc_set(to, ast->_bop.L->_subscript.args_n + 2);

            DEPINC(-(ast->_bop.L->_subscript.args_n + 1));


        } else {
            ks_err_add_str(KS_STR_CONST("Tried assigning to the wrong kind of AST (expected a var)"));
        }

    } else if (ast->type == KS_AST_LIST) {
        int i;
        for (i = 0; i < ast->_list.items_n; ++i) {
            _ks_ast_codegen(ast->_list.items[i], to, geni);
        }

        ksc_list(to, ast->_list.items_n);

        DEPINC(1-ast->_list.items_n);

    } else if (ast->type == KS_AST_IF) {
        // compute the conditional, so it is at the top of the stack
        _ks_ast_codegen(ast->_if.cond, to, geni);

        // now, we will jump forward if it is false
        int p_jmpf = to->bc_n;
        ksc_jmpf(to, -1);
        DEPINC(-1);

        // position of the body, fill this in later
        int p_body = to->bc_n;
        _ks_ast_codegen(ast->_if.body, to, geni);

        int32_t diff = to->bc_n - p_body;

        // now, correct it
        struct ks_bc_jmp* i_jmpf = (struct ks_bc_jmp*)(to->bc + p_jmpf);
        i_jmpf->relamt = diff;


    } else if (ast->type == KS_AST_WHILE) {
        // position of the condition/start of loop
        int p_cond = to->bc_n;
        _ks_ast_codegen(ast->_while.cond, to, geni);

        // jump ahead if it is false
        int p_jmpf = to->bc_n;
        ksc_jmpf(to, -1);
        DEPINC(-1);

        // position of the body, fill this in later
        int p_body = to->bc_n;
        _ks_ast_codegen(ast->_while.body, to, geni);

        // now, jump to start of loop and try again
        int32_t diff = p_cond - to->bc_n;
        ksc_jmp(to, diff - sizeof(struct ks_bc_jmp));

        // now, replace the entry with the correct one
        struct ks_bc_jmp* i_jmpf = (struct ks_bc_jmp*)(to->bc + p_jmpf);
        i_jmpf->relamt = to->bc_n - p_body;

    } else if (ast->type == KS_AST_CALL) {
        _ks_ast_codegen(ast->_call.func, to, geni);

        int i;
        for (i = 0; i < ast->_call.args_n; ++i) {
            _ks_ast_codegen(ast->_call.args[i], to, geni);
        }
        ksc_call(to, ast->_call.args_n + 1);
        DEPINC(-ast->_call.args_n);

    } else if (ast->type == KS_AST_SUBSCRIPT) {
        _ks_ast_codegen(ast->_subscript.obj, to, geni);

        int i;
        for (i = 0; i < ast->_subscript.args_n; ++i) {
            _ks_ast_codegen(ast->_subscript.args[i], to, geni);
        }
        ksc_get(to, ast->_subscript.args_n + 1);
        DEPINC(-ast->_subscript.args_n);

    } else if (ast->type == KS_AST_BLOCK) {

        int i;
        for (i = 0; i < ast->_block.sub_n; ++i) {
            // each should leave the stack resolved
            _ks_ast_codegen(ast->_block.subs[i], to, geni);

            // unless, its returning something, reset the stack back
            while (geni->stk_depth > start_depth) {
                ksc_discard(to);
                DEPINC(-1);
            }
            //RESET_STK(start_depth);
        }
    } else if (ast->type == KS_AST_FUNC) {
        kso_code func_code = kso_code_new_empty(to->v_const);

        // create parameters
        kso_list f_params = kso_list_new_empty();
        int i;
        for (i = 0; i < ast->_func.n_params; ++i) {
            ks_list_push(&f_params->v_list, (kso)kso_str_new(ast->_func.param_names[i]));
        }

        // generate the code here
        ks_ast_codegen(ast->_func.body, func_code);
        // TODO: make sure it returns with none if requested
        ksc_retnone(func_code);

        kso_kfunc kfunc = kso_kfunc_new(f_params, func_code);

        // pop it on as a function literal
        ksc_const(to, (kso)kfunc);
        DEPINC(1);

    } else if (ast->type == KS_AST_RETURN) {
        if (ast->_val == NULL) {
            DEPINC(0);
            ksc_retnone(to);
        } else {
            _ks_ast_codegen(ast->_val, to, geni);
            ksc_ret(to);
            DEPINC(-1);
        }
    } else {

        ks_err_add_str_fmt("Unexpected AST type %d", ast->type);
    }

    geni->last = ast;

    return;
}

void ks_ast_codegen(ks_ast ast, kso_code to) {
    cgi geni;
    geni.stk_depth = 0;
    geni.last = NULL;
    _ks_ast_codegen(ast, to, &geni);
}
