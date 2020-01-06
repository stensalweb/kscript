/* types/ast.c - reprsernts an Abstract Syntax Tree */

#include "ks_common.h"


// initializes to an empty token, with a given type
#define _AST_INIT(_type) KSO_BASE_INIT(ks_T_ast) .atype = _type, .tok = ks_tok_new(KS_TOK_NONE, NULL, 0, 0, 0, 0), .tok_expr = ks_tok_new(KS_TOK_NONE, NULL, 0, 0, 0, 0),

// create a new AST representing 'true'
ks_ast ks_ast_new_true() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_TRUE)
    };
    return self;
}

// create a new AST representing 'false'
ks_ast ks_ast_new_false() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_FALSE)

    };
    return self;
}

// create a new AST representing 'none'
ks_ast ks_ast_new_none() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_NONE)
    };
    return self;
}

// create a new AST representing a constant int
ks_ast ks_ast_new_int(int64_t v_int) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_INT)
        .v_val = (kso)ks_int_new(v_int)
    };
    return self;
}

// create a new AST representing a constant string
ks_ast ks_ast_new_str(ks_str v_str) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_STR)
        .v_val = (kso)v_str
    };
    KSO_INCREF(self->v_val);
    return self;
}


// create a new AST representing a variable reference
ks_ast ks_ast_new_var(ks_str var_name) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_VAR)
        .v_name = var_name,
    };
    KSO_INCREF(self->v_name);
    return self;
}

// create a new AST representing an attribute reference
ks_ast ks_ast_new_attr(ks_ast obj, ks_str attr) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_ATTR)
        .v_attr = {obj, attr},
    };
    KSO_INCREF(self->v_attr.obj);
    KSO_INCREF(self->v_attr.attr);
    return self;
}

// new AST representing a tuple
ks_ast ks_ast_new_tuple(ks_ast* items, int n_items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_TUPLE)
        .v_list = ks_list_new((kso*)items, n_items),
    };
    return self;
}

// new AST representing a list
ks_ast ks_ast_new_list(ks_ast* items, int n_items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_LIST)
        .v_list = ks_list_new((kso*)items, n_items),
    };
    return self;
}

// create a new AST representing a functor call, with `items[0]` being the function
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_call(ks_ast* items, int n_items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_CALL)
        .v_list = ks_list_new((kso*)items, n_items),
    };
    return self;
}

// create a new AST representing a subscript, with `items[0]` being the subscript
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_subscript(ks_ast* items, int n_items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_SUBSCRIPT)
        .v_list = ks_list_new((kso*)items, n_items),
    };
    return self;
}

ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(bop_type)
        .v_bop = { L, R }
    };
    KSO_INCREF(L);
    KSO_INCREF(R);
    return self;
}

// create a new if block AST
ks_ast ks_ast_new_if(ks_ast cond, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_IF)
        .v_if = {cond, body}
    };
    KSO_INCREF(cond);
    KSO_INCREF(body);

    return self;
}

// create a new while block AST
ks_ast ks_ast_new_while(ks_ast cond, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_WHILE)
        .v_while = {cond, body}
    };
    KSO_INCREF(cond);
    KSO_INCREF(body);

    return self;
}
// create a new AST representing a return statement
ks_ast ks_ast_new_ret(ks_ast val) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_RET)
        .v_ret = val
    };
    KSO_INCREF(val);
    return self;
}
// createa a new AST representing a function literal
ks_ast ks_ast_new_func(ks_list params, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_FUNC)
        .v_func = {params, body}
    };
    KSO_INCREF(params);
    KSO_INCREF(body);
    return self;
}

// createa a new AST representing a block of code
ks_ast ks_ast_new_code(ks_code code) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_CODE)
        .v_code = code
    };
    KSO_INCREF(code);
    return self;
}

// create a new empty block AST
ks_ast ks_ast_new_block_empty() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_BLOCK)
        .v_list = ks_list_new_empty()
    };
    return self;
}

ks_ast ks_ast_new_block(ks_ast* items, int n_items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_BLOCK)
        .v_list = ks_list_new((kso*)items, n_items)
    };
    return self;
}


TFUNC(ast, free) {
    #define SIG "ast.__free__(self)"
    REQ_N_ARGS(1);
    ks_ast self = (ks_ast)args[0];
    REQ_TYPE("self", self, ks_T_ast);

    switch (self->atype) {
    case KS_AST_TRUE:
    case KS_AST_FALSE:
    case KS_AST_NONE:
        // do nothing, they don't hold refs
        break;

    case KS_AST_INT:
    case KS_AST_STR:
        KSO_DECREF(self->v_val);
        break;

    case KS_AST_VAR:
        KSO_DECREF(self->v_name);
        break;
    case KS_AST_ATTR:
        KSO_DECREF(self->v_attr.obj);
        KSO_DECREF(self->v_attr.attr);
        break;

    case KS_AST_TUPLE:
    case KS_AST_LIST:
    case KS_AST_CALL:
    case KS_AST_SUBSCRIPT:
    case KS_AST_BLOCK:

        // all of them just use the list
        KSO_DECREF(self->v_list);
        break;

    case KS_AST_CODE:
        KSO_DECREF(self->v_code);
        break;
    case KS_AST_FUNC:
        KSO_DECREF(self->v_func.params);
        KSO_DECREF(self->v_func.body);
        break;

    case KS_AST_IF:
        KSO_DECREF(self->v_if.cond);
        KSO_DECREF(self->v_if.body);
        break;

    case KS_AST_WHILE:
        KSO_DECREF(self->v_while.cond);
        KSO_DECREF(self->v_while.body);
        break;

    case KS_AST_RET:
        KSO_DECREF(self->v_ret);
        break;

    // handle all binary operators
    case KS_AST_BOP_ADD:
    case KS_AST_BOP_SUB:
    case KS_AST_BOP_MUL:
    case KS_AST_BOP_DIV:
    case KS_AST_BOP_MOD:
    case KS_AST_BOP_POW:
    case KS_AST_BOP_LT:
    case KS_AST_BOP_LE:
    case KS_AST_BOP_GT:
    case KS_AST_BOP_GE:
    case KS_AST_BOP_EQ:
    case KS_AST_BOP_NE:
    case KS_AST_BOP_ASSIGN:
        KSO_DECREF(self->v_bop.L);
        KSO_DECREF(self->v_bop.R);
        break;

    default:
        return kse_fmt(SIG ": AST obj @ %p was of unknown type %i", self, self->atype);
        break;
    }

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}


/* exporting functionality */

struct ks_type T_ast, *ks_T_ast = &T_ast;

void ks_init__ast() {

    /* first create the type */
    T_ast = (struct ks_type) {
        KSO_BASE_INIT(ks_T_type)

        .name = ks_str_new("ast"),

        .f_free = (kso)ks_cfunc_new(ast_free_),

    };

}




