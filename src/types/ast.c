/* ast.c - implementation of abstract syntax trees for kscript
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_ast);

// construct a new AST representing a constant value
ks_ast ks_ast_new_const(ks_obj val) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = KS_AST_CONST;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(1, &val);

    return self;
}

// construct a new AST representing a constant value
ks_ast ks_ast_new_var(ks_str name) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = KS_AST_VAR;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(1, (ks_obj*)&name);

    return self;
}

// Create an AST representing an attribute reference
// Type should always be string
ks_ast ks_ast_new_attr(ks_ast obj, ks_str attr) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = KS_AST_ATTR;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(2, (ks_obj[]){ (ks_obj)obj, (ks_obj)attr });

    return self;
}

// construct a new AST representing a function call
ks_ast ks_ast_new_call(ks_ast func, int n_args, ks_ast* args) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = KS_AST_CALL;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(1, (ks_obj*)&func);

    // push args too
    ks_list_pushn(self->children, n_args, (ks_obj*)args);

    return self;
}

// construct a new AST representing a return statement
ks_ast ks_ast_new_ret(ks_ast val) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = KS_AST_RET;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(1, (ks_obj*)&val);

    return self;
}


// construct a new AST representing a block
ks_ast ks_ast_new_block(int num, ks_ast* elems) {
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = KS_AST_BLOCK;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(num, (ks_obj*)elems);

    return self;
}
// Create a new AST represernting a binary operation on 2 objects
// NOTE: Returns a new reference
ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R) {
    assert(KS_AST_BOP__FIRST <= bop_type && bop_type <= KS_AST_BOP__LAST);
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = bop_type;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(2, (ks_obj[]){ (ks_obj)L, (ks_obj)R });

    return self;
}

// Create a new AST represernting a unary operation
// NOTE: Returns a new reference
ks_ast ks_ast_new_uop(int uop_type, ks_ast V) {
    assert(KS_AST_UOP__FIRST <= uop_type && uop_type <= KS_AST_UOP__LAST);
    ks_ast self = KS_ALLOC_OBJ(ks_ast);
    KS_INIT_OBJ(self, ks_type_ast);

    // set specific variables
    self->kind = uop_type;
    self->tok = self->tok_expr = (ks_tok){NULL};
    self->children = ks_list_new(1, (ks_obj*)&V);

    return self;
}




/* member functions */

// ast.__free__(self) -> free an AST object
static KS_TFUNC(ast, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_ast self = (ks_ast)args[0];
    KS_REQ_TYPE(self, ks_type_ast, "self");
    
    // recursively dereference the children
    KS_DECREF(self->children);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// initialize ast type
void ks_type_ast_init() {
    KS_INIT_TYPE_OBJ(ks_type_ast, "ast");

    ks_type_set_cn(ks_type_ast, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new(ast_free_)},
        {NULL, NULL}   
    });
}



