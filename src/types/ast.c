/* ast.c - implementation of the 'ast' type (Abstract Syntax Trees)
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// construct a new AST representing a constant value
ks_ast ks_ast_new_const(ks_obj val) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_CONST;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, &val);
    self->dflag = 0;

    return self;
}
// construct a new AST representing a constant value
ks_ast ks_ast_new_var(ks_str name) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_VAR;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&name);
    self->dflag = 0;

    return self;
}

// Create an AST representing a list constructor
ks_ast ks_ast_new_list(int n_items, ks_ast* items) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_LIST;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(n_items, (ks_obj*)items);
    self->dflag = 0;

    return self;
}
// Create an AST representing a list constructor
ks_ast ks_ast_new_slice(ks_ast start, ks_ast stop, ks_ast step) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_SLICE;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(3, (ks_obj[]){ (ks_obj)start, (ks_obj)stop, (ks_obj)step });
    self->dflag = 0;

    return self;
}

// Create an AST representing a list constructor
ks_ast ks_ast_new_tuple(int n_items, ks_ast* items) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_TUPLE;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(n_items, (ks_obj*)items);
    self->dflag = 0;

    return self;
}


// Create an AST representing a list constructor
ks_ast ks_ast_new_dict(int n_items, ks_ast* items) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_DICT;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(n_items, (ks_obj*)items);
    self->dflag = 0;

    return self;
}

// Create an AST representing an attribute reference
// Type should always be string
ks_ast ks_ast_new_attr(ks_ast obj, ks_str attr) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_ATTR;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(2, (ks_obj[]){ (ks_obj)obj, (ks_obj)attr });
    self->dflag = 0;

    return self;
}

// construct a new AST representing a function call
ks_ast ks_ast_new_call(ks_ast func, int n_args, ks_ast* args) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_CALL;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&func);
    self->dflag = 0;

    // push args too
    ks_list_pushn(self->children, n_args, (ks_obj*)args);

    return self;
}

// Create an AST representing a subscript
ks_ast ks_ast_new_subscript(ks_ast obj, int n_args, ks_ast* args) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_SUBSCRIPT;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&obj);
    self->dflag = 0;

    // push args too
    ks_list_pushn(self->children, n_args, (ks_obj*)args);

    return self;
}

// construct a new AST representing a return statement
ks_ast ks_ast_new_ret(ks_ast val) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_RET;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&val);
    self->dflag = 0;

    return self;
}

// construct a new AST representing a throw statement
ks_ast ks_ast_new_throw(ks_ast val) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_THROW;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&val);
    self->dflag = 0;

    return self;
}


// construct a new AST representing a assert statement
ks_ast ks_ast_new_assert(ks_ast val) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_ASSERT;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&val);
    self->dflag = 0;

    return self;
}

// construct a new AST representing a block
ks_ast ks_ast_new_block(int num, ks_ast* elems) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_BLOCK;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(num, (ks_obj*)elems);
    self->dflag = 0;

    return self;
}


// Create an AST representing an 'if' construct
// 'else_body' may be NULL, in which case it is constructed without an 'else' body
// NOTE: Returns a new reference
ks_ast ks_ast_new_if(ks_ast cond, ks_ast if_body, ks_ast else_body) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_IF;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(2, (ks_obj[]){ (ks_obj)cond, (ks_obj)if_body });
    if (else_body) ks_list_push(self->children, (ks_obj)else_body);
    self->dflag = 0;

    return self;
}

// Create an AST representing an 'while' construct
// 'else_body' may be NULL, in which case it is constructed without an 'else' body
// NOTE: Returns a new reference
ks_ast ks_ast_new_while(ks_ast cond, ks_ast while_body, ks_ast else_body) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_WHILE;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(2, (ks_obj[]){ (ks_obj)cond, (ks_obj)while_body });
    if (else_body) ks_list_push(self->children, (ks_obj)else_body);
    self->dflag = 0;

    return self;
}


// Create an AST representing a 'try' block
// NOTE: Returns a new reference
ks_ast ks_ast_new_try(ks_ast try_body, ks_ast catch_body, ks_str catch_name) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_TRY;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj[]){ (ks_obj)try_body });
    if (catch_body) {
        ks_list_push(self->children, (ks_obj)catch_body);
        if (catch_name) ks_list_push(self->children, (ks_obj)catch_name);

    }
    self->dflag = 0;

    return self;
}

// Create an AST representing a function definition
// NOTE: Returns a new reference
ks_ast ks_ast_new_func(ks_str name, ks_list params, ks_ast body) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_FUNC;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(3, (ks_obj[]){ (ks_obj)name, (ks_obj)params, (ks_obj)body });
    self->dflag = 0;

    return self;
}

// Create an AST representing a for loop
// NOTE: Returns a new reference
ks_ast ks_ast_new_for(ks_ast iter_obj, ks_ast body, ks_str assign_to) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = KS_AST_FOR;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(3, (ks_obj[]){ (ks_obj)iter_obj, (ks_obj)body, (ks_obj)assign_to });
    self->dflag = 0;

    return self;
}

// Create a new AST represernting a binary operation on 2 objects
// NOTE: Returns a new reference
ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R) {
    assert(KS_AST_BOP__FIRST <= bop_type && bop_type <= KS_AST_BOP__LAST);
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = bop_type;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(2, (ks_obj[]){ (ks_obj)L, (ks_obj)R });
    self->dflag = 0;

    return self;
}

// Create a new AST represernting a unary operation
// NOTE: Returns a new reference
ks_ast ks_ast_new_uop(int uop_type, ks_ast V) {
    assert(KS_AST_UOP__FIRST <= uop_type && uop_type <= KS_AST_UOP__LAST);
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_T_ast);

    // set specific variables
    self->kind = uop_type;
    self->tok = (struct ks_tok){ -1, -1 };
    self->children = ks_list_new(1, (ks_obj*)&V);
    self->dflag = 0;

    return self;
}


/* Object Methods */

// ast.__free__(self) - free obj
static KS_TFUNC(ast, free) {
    ks_ast self;
    KS_GETARGS("self:*", &self, ks_T_ast)

    KS_DECREF(self->children);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_ast);

void ks_init_T_ast() {
    ks_type_init_c(ks_T_ast, "ast", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(ast_free_, "ast.__free__(self)")},
    ));
}

