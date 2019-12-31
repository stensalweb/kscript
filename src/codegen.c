/* codegen.c - implementation of generation of a `ks_code` object from an AST */

#include "ks.h"


// a structure describing code-generation-information (cgi)
typedef struct cgi {

    // variable keeping track of where the stack is currently
    int stk_depth;

    // variable keeping track how many ast recursion calls it is deep
    int call_depth;

    // the last AST on the main block level of the AST, useful for making sure a return is at
    //   the end of the bytecode
    ks_ast last_main;


}* cgi;





// internal method to generate code
static void codegen(ks_ast self, ks_code to, cgi geni) {
    geni->call_depth++;

    if (self == NULL) {
        kse_add("Given NULL AST in codegen()");
        return;
    }

    // loop variable
    int i;

    // capture where we started
    int stk_depth_start = geni->stk_depth;

    // grows the stack by `_n`
    #define STK_GROW(_n) { geni->stk_depth += _n; }

    // makes sure the stk grows by `_rel`, set _rel=0 for no change in stack depth
    #define STK_TO(_rel) { while (geni->stk_depth > stk_depth_start + _rel) { geni->stk_depth--; ksc_popu(to); } }

    switch (self->atype)
    {
    case KS_AST_INT:
        ksc_int(to, self->v_int->v_int);
        STK_GROW(1);
        break;
    case KS_AST_STR:
        ksc_cstr(to, self->v_str->chr);
        STK_GROW(1);
        break;
    case KS_AST_VAR:
        ksc_load(to, self->v_var->chr);
        STK_GROW(1);
        break;

    case KS_AST_BOP_ADD:
    case KS_AST_BOP_SUB:
    case KS_AST_BOP_MUL:
    case KS_AST_BOP_DIV:
        // all binary operators
        // generate both children
        codegen(self->v_bop.L, to, geni);
        codegen(self->v_bop.R, to, geni);

        /**/ if (self->atype == KS_AST_BOP_ADD) ksc_add(to);
        else if (self->atype == KS_AST_BOP_SUB) ksc_sub(to);
        else if (self->atype == KS_AST_BOP_MUL) ksc_mul(to);
        else if (self->atype == KS_AST_BOP_DIV) ksc_div(to);

        // pop 2, but push result back on
        STK_GROW(1 - 2);

        break;


    case KS_AST_CALL:

        // calculate all the children
        for (i = 0; i < self->v_call->len; ++i) {
            codegen((ks_ast)self->v_call->items[i], to, geni);
        }

        // call those items on the stack
        ksc_call(to, self->v_call->len);

        // 1 is for the result of the function, minus all the things used to call it
        STK_GROW(1 - self->v_call->len);
        break;
    case KS_AST_CODE:

        // output some literal assembly, by linking it in
        ks_code_linkin(to, self->v_code);

        break;
    
    case KS_AST_BLOCK:

        // output all the sub block items
        for (i = 0; i < self->v_block->len; ++i) {
            codegen((ks_ast)self->v_block->items[i], to, geni);

            // discard any modifications made to the stack by these items
            STK_TO(0);

        }

        break;

    default:
        kse_fmt("Given invalid AST (@%p) with type %i", self, self->atype);
        return;
        break;
    }

}


ks_code ks_ast_codegen(ks_ast self) {
    ks_list v_const = ks_list_new_empty();
    ks_code code = ks_code_new_empty(v_const);

    struct cgi geni;
    geni.last_main = NULL;
    geni.stk_depth = 0;
    geni.call_depth = 0;

    codegen(self, code, &geni);

    ksc_noop(code);

    return code;
}


