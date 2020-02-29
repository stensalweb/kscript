/* types/ast.c - represents an Abstract Syntax Tree
 *
 * Generally, a program can be thought of a control flow diagram and/or a computational graph/tree.
 *
 * This is a high level representation of that tree, and how it can be traversed, read, etc from C.
 *
 * These are typically generated from a parser (see `types/parser.c` for how they are constructed programmatically)
 *
 *
 * Specifics:
 *
 * The `self->atype` attribute tells what kind of AST it is respectively.
 *
 * If it is KS_AST_BOP_*, then it is a binary operator, KS_AST_UOP_*, a unary operator, etc
 *
 * There are specific attributes in the union of the AST that are used by certain types of ASTs.
 *
 * These are documented in `ks_types.h`, for more complete information.
 *
 *
 * TODO:
 *   * Traversal algorithms/visitor functions
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks_common.h"

// visits an ast recursively
void ks_ast_visit(ks_ast self, ks_ast_visit_f func, void* data) {

    // call the function
    int res = func(self, data);

    // now, handle the children of the function
    int i;
    switch (self->atype) {
    case KS_AST_TRUE:
    case KS_AST_FALSE:
    case KS_AST_NONE:
    case KS_AST_INT:
    case KS_AST_STR:
    case KS_AST_VAR:
    case KS_AST_CODE:
        // do nothing, they don't have child nodes
        break;

    case KS_AST_ATTR:
        ks_ast_visit(self->v_attr.obj, func, data);
        break;

    case KS_AST_TUPLE:
    case KS_AST_LIST:
    case KS_AST_CALL:
    case KS_AST_SUBSCRIPT:
    case KS_AST_BLOCK:
        for (i = 0; i < self->v_list->len; ++i) {
            ks_ast_visit((ks_ast)self->v_list->items[i], func, data);
        }
        break;

    case KS_AST_FUNC:
        ks_ast_visit(self->v_func.body, func, data);
        break;

    case KS_AST_TYPE:
        ks_ast_visit(self->v_type.body, func, data);
        break;

    case KS_AST_IF:
        ks_ast_visit(self->v_if.cond, func, data);
        ks_ast_visit(self->v_if.body, func, data);
        if (self->v_if.has_else) ks_ast_visit(self->v_if.v_else, func, data);
        break;

    case KS_AST_WHILE:
        ks_ast_visit(self->v_while.cond, func, data);
        ks_ast_visit(self->v_while.body, func, data);
        break;

    case KS_AST_TRY:
        ks_ast_visit(self->v_try.v_try, func, data);
        if (self->v_try.v_catch != NULL) ks_ast_visit(self->v_try.v_catch, func, data);
        break;

    case KS_AST_RET:
        if (self->v_ret != NULL) ks_ast_visit(self->v_ret, func, data);
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
        ks_ast_visit(self->v_bop.L, func, data);
        ks_ast_visit(self->v_bop.R, func, data);
        break;

    // handle all unary operators
    case KS_AST_UOP_NEG:
    case KS_AST_UOP_SQIG:
        ks_ast_visit(self->v_uop, func, data);
        break;
    

    default:
        break;
    }

}

// optimizes an AST given a function
ks_ast ks_ast_fopt(ks_ast self, ks_ast_opt_f func, void* data) {

    // first, handle the children of the function
    int i;
    switch (self->atype) {
    case KS_AST_TRUE:
    case KS_AST_FALSE:
    case KS_AST_NONE:
    case KS_AST_INT:
    case KS_AST_STR:
    case KS_AST_VAR:
    case KS_AST_CODE:
        // do nothing, they don't have child nodes
        break;

    case KS_AST_ATTR:
        self->v_attr.obj = ks_ast_fopt(self->v_attr.obj, func, data);
        break;

    case KS_AST_TUPLE:
    case KS_AST_LIST:
    case KS_AST_CALL:
    case KS_AST_SUBSCRIPT:
    case KS_AST_BLOCK:
        for (i = 0; i < self->v_list->len; ++i) {
            self->v_list->items[i] = (kso)ks_ast_fopt((ks_ast)self->v_list->items[i], func, data);
        }
        break;

    case KS_AST_FUNC:
        self->v_func.body = ks_ast_fopt(self->v_func.body, func, data);
        break;

    case KS_AST_TYPE:
        self->v_type.body = ks_ast_fopt(self->v_type.body, func, data);
        break;

    case KS_AST_IF:
        self->v_if.cond = ks_ast_fopt(self->v_if.cond, func, data);
        self->v_if.body = ks_ast_fopt(self->v_if.body, func, data);
        if (self->v_if.has_else) self->v_if.v_else = ks_ast_fopt(self->v_if.v_else, func, data);
        break;

    case KS_AST_WHILE:
        self->v_while.cond = ks_ast_fopt(self->v_while.cond, func, data);
        self->v_while.body = ks_ast_fopt(self->v_while.body, func, data);
        break;

    case KS_AST_TRY:
        self->v_try.v_try = ks_ast_fopt(self->v_try.v_try, func, data);
        if (self->v_try.v_catch != NULL) self->v_try.v_catch = ks_ast_fopt(self->v_try.v_catch, func, data);
        break;

    case KS_AST_RET:
        if (self->v_ret != NULL) self->v_ret = ks_ast_fopt(self->v_ret, func, data);
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
        self->v_bop.L = ks_ast_fopt(self->v_bop.L, func, data);
        self->v_bop.R = ks_ast_fopt(self->v_bop.R, func, data);
        break;

    // handle all unary operators
    case KS_AST_UOP_NEG:
    case KS_AST_UOP_SQIG:
        self->v_uop = ks_ast_fopt(self->v_uop, func, data);
        break;
    

    default:
        break;
    }

    // now, optimize the current node with the given function and return the result
    return func(self, data);
}


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

// create a new AST representing a constant float
ks_ast ks_ast_new_float(double v_float) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_FLOAT)
        .v_val = (kso)ks_float_new(v_float)
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


// create a new AST representing a unary operator, assumes `uop_type` is a valid unary operator
ks_ast ks_ast_new_uop(int uop_type, ks_ast V) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(uop_type)
        .v_uop = V
    };
    KSO_INCREF(V);
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
ks_ast ks_ast_new_if(ks_ast cond, ks_ast body, ks_ast v_else) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_IF)
        .v_if = {cond, body, .has_else = v_else != NULL, .v_else = v_else}
    };
    KSO_INCREF(cond);
    KSO_INCREF(body);

    if (v_else != NULL) KSO_INCREF(v_else);

    return self;
}

// attach an 'else' block to an AST (which must be an 'if' AST)
void ks_ast_attach_else(ks_ast self, ks_ast v_else) {
    assert(self->atype == KS_AST_IF);
    if (self->v_if.has_else) KSO_DECREF(self->v_if.v_else);

    self->v_if.has_else = v_else != NULL;
    self->v_if.v_else = v_else;
    if (v_else != NULL) KSO_INCREF(v_else);
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

// create a new try/catch block
ks_ast ks_ast_new_try(ks_ast v_try, ks_ast v_catch, ks_str v_catch_target) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_TRY)
        .v_try = {v_try, v_catch, v_catch_target}
    };
    KSO_INCREF(v_try);
    if (v_catch != NULL) KSO_INCREF(v_catch);
    if (v_catch_target != NULL) KSO_INCREF(v_catch_target);

    return self;
}

// create a new throw expression
ks_ast ks_ast_new_throw(ks_ast v_throw) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_THROW)
        .v_throw = v_throw
    };
    KSO_INCREF(v_throw);

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

// createa a new AST representing a function literal
ks_ast ks_ast_new_func(ks_str name, ks_list params, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_FUNC)
        .v_func = {name, params, body}
    };
    KSO_INCREF(name);
    KSO_INCREF(params);
    KSO_INCREF(body);
    return self;
}
// create a new type AST
ks_ast ks_ast_new_type(ks_str name, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        _AST_INIT(KS_AST_TYPE)
        .v_type.name = name,
        .v_type.body = body
    };
    KSO_INCREF(name);
    KSO_INCREF(body);
    return self;
}

// free an AST
KS_TFUNC(ast, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_ast self = (ks_ast)args[0];
    KS_REQ_TYPE(self, ks_T_ast, "self");

    switch (self->atype) {
    case KS_AST_TRUE:
    case KS_AST_FALSE:
    case KS_AST_NONE:
        // do nothing, they don't hold refs
        break;

    case KS_AST_INT:
    case KS_AST_FLOAT:
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
        KSO_DECREF(self->v_func.name);
        KSO_DECREF(self->v_func.params);
        KSO_DECREF(self->v_func.body);
        break;
    case KS_AST_TYPE:
        KSO_DECREF(self->v_type.name);
        KSO_DECREF(self->v_type.body);

        break;

    case KS_AST_IF:
        KSO_DECREF(self->v_if.cond);
        KSO_DECREF(self->v_if.body);
        if (self->v_if.has_else) KSO_DECREF(self->v_if.v_else);
        break;

    case KS_AST_WHILE:
        KSO_DECREF(self->v_while.cond);
        KSO_DECREF(self->v_while.body);
        break;
    case KS_AST_TRY:
        KSO_DECREF(self->v_try.v_try);
        if (self->v_try.v_catch != NULL) KSO_DECREF(self->v_try.v_catch);
        if (self->v_try.v_catch_target != NULL) KSO_DECREF(self->v_try.v_catch_target);
        break;
    case KS_AST_THROW:
        KSO_DECREF(self->v_throw);
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

    // handle all unary operators
    case KS_AST_UOP_NEG:
    case KS_AST_UOP_SQIG:
        KSO_DECREF(self->v_uop);
        break;
    

    default:
        return kse_fmt("AST obj @ %p was of unknown type %i", self, self->atype);
        break;
    }

    ks_free(self);

    return KSO_NONE;
}


/* exporting functionality */

struct ks_type T_ast, *ks_T_ast = &T_ast;

void ks_init__ast() {

    /* create the type */
    T_ast = KS_TYPE_INIT();

    ks_type_setname_c(ks_T_ast, "ast");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_ast, "__free__", "ast.free(self)", ast_free_);

}
