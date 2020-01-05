/* codegen.c - implementation of generation of a `ks_code` object from an AST

See `types/code.c` for the actual byte-code production, that is where the actual generation routines lie

*/

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
    /* strategy for code generation:

    We want to keep the stack balanced so that, say, recursive functions can work without the stack becoming
      corrupted, so we store the current stack depth in `geni` (generation information)
    
    Some operations produce values on the stack (like loading a constant), and some take them off (function calls),
      so we sum these up to get our current stack position.

    Some ASTs, like blocks, must destroy any left over values on the top of the stack, because they 'yield' nothing to the stack

    */

    geni->call_depth++;

    if (self == NULL) {
        kse_add("Given NULL AST in codegen()");
        return;
    }

    // just testing the errors
    //kse_tok(self->tok_expr, "test...%i", self->atype);

    // loop variable
    int i;

    // capture where we started
    int stk_depth_start = geni->stk_depth;

    // grows the stack by `_n`
    #define STK_GROW(_n) { geni->stk_depth += _n; }

    // makes sure the stk only grows by `_rel`, set _rel=0 for no change in stack depth, i.e. remove all additional things generated by children
    #define STK_TO(_rel) { while (geni->stk_depth > stk_depth_start + _rel) { geni->stk_depth--; ksc_popu(to); } }

    if (self->atype == KS_AST_INT) {
        ks_code_add_meta(to, self);
        ksc_int(to, ((ks_int)self->v_val)->v_int);
        STK_GROW(1);
    } else if (self->atype == KS_AST_STR) {
        ks_code_add_meta(to, self);
        ksc_cstr(to, ((ks_str)self->v_val)->chr);
        STK_GROW(1);
    } else if (self->atype == KS_AST_TRUE) {
        ks_code_add_meta(to, self);
        ksc_const_true(to);
        STK_GROW(1);
    } else if (self->atype == KS_AST_FALSE) {
        ks_code_add_meta(to, self);
        ksc_const_false(to);
        STK_GROW(1);
    } else if (self->atype == KS_AST_NONE) {
        ks_code_add_meta(to, self);
        ksc_const_none(to);
        STK_GROW(1);
    } else if (self->atype == KS_AST_VAR) {
        ks_code_add_meta(to, self);
        ksc_loado(to, (kso)self->v_name);
        STK_GROW(1);
    } else if (self->atype == KS_AST_ATTR) {
        // generate the object
        codegen(self->v_attr.obj, to, geni);

        // now, get its attribute
        ks_code_add_meta(to, self);
        ksc_load_ao(to, (kso)self->v_attr.attr);
        
        // the object is replaced by its attribute, so no growth
        STK_GROW(0);

    } else if (self->atype >= KS_AST_BOP_ADD && self->atype < KS_AST_BOP_ASSIGN) {
        // all binary operators
        // generate both children
        codegen(self->v_bop.L, to, geni);
        codegen(self->v_bop.R, to, geni);

        // add meta after arguments
        ks_code_add_meta(to, self);

        /**/ if (self->atype == KS_AST_BOP_ADD) ksc_add(to);
        else if (self->atype == KS_AST_BOP_SUB) ksc_sub(to);
        else if (self->atype == KS_AST_BOP_MUL) ksc_mul(to);
        else if (self->atype == KS_AST_BOP_DIV) ksc_div(to);
        else if (self->atype == KS_AST_BOP_MOD) ksc_mod(to);
        else if (self->atype == KS_AST_BOP_POW) ksc_pow(to);
        else if (self->atype == KS_AST_BOP_LT)  ksc_lt(to);
        else if (self->atype == KS_AST_BOP_LE)  ksc_le(to);
        else if (self->atype == KS_AST_BOP_GT)  ksc_gt(to);
        else if (self->atype == KS_AST_BOP_GE)  ksc_ge(to);
        else if (self->atype == KS_AST_BOP_EQ)  ksc_eq(to);
        else if (self->atype == KS_AST_BOP_NE)  ksc_ne(to);

        // pop 2, but push result back on
        STK_GROW(1 - 2);
    } else if (self->atype == KS_AST_BOP_ASSIGN) {
        // assignment is special, because only certain kinds of ASTs are assignable


        if (self->v_bop.L->atype == KS_AST_VAR) {
            // assign to a name, i.e. NAME = val
            // first generate the value
            codegen(self->v_bop.R, to, geni);

            // add meta
            ks_code_add_meta(to, self);

            // now, just store
            ksc_storeo(to, (kso)self->v_bop.L->v_name);

            // nothing changes
            STK_GROW(0);
        } else if (self->v_bop.L->atype == KS_AST_ATTR) {
            // generate the object we are setting its attribute of
            codegen(self->v_bop.L->v_attr.obj, to, geni);

            // generate the value as well
            codegen(self->v_bop.R, to, geni);
            
            // add meta
            ks_code_add_meta(to, self);

            ks_str myattr = self->v_bop.L->v_attr.attr;
            // now, just store the attribute
            ksc_store_ao(to, (kso)myattr);

            // 1 less items on the stack now
            STK_GROW(-1);
        } else if (self->v_bop.L->atype == KS_AST_SUBSCRIPT) {

            // do a `x[v] = y` call,
            // or: `x[a0, a1, 2] = y` will be in `v_list` as `x, a0, a1, 2`
            // so, generate all those, then Y, then call a setitem
            // calculate all the children
            for (i = 0; i < self->v_bop.L->v_list->len; ++i) {
                codegen((ks_ast)self->v_bop.L->v_list->items[i], to, geni);
            }

            // calculate the value too

            // generate the value as well
            codegen(self->v_bop.R, to, geni);

            // add meta
            ks_code_add_meta(to, self);

            // +1 for the value on the RHS
            int n_items = self->v_bop.L->v_list->len + 1;


            ksc_setitem(to, n_items);


            // push one on, but take off n_items
            STK_GROW(1 - n_items);

        } else {
            kse_tok(ks_tok_combo(self->v_bop.L->tok_expr, self->tok), "Invalid left-hand side of `=` operator, must have a variable or an attribute!");
        }

    } else if (self->atype == KS_AST_CALL) {

        // calculate all the children
        for (i = 0; i < self->v_list->len; ++i) {
            codegen((ks_ast)self->v_list->items[i], to, geni);
        }

        // add meta
        ks_code_add_meta(to, self);

        // call those items on the stack
        ksc_call(to, self->v_list->len);

        // 1 is for the result of the function, minus all the things used to call it
        STK_GROW(1 - self->v_list->len);


    } else if (self->atype == KS_AST_SUBSCRIPT) {

        // calculate all the children
        for (i = 0; i < self->v_list->len; ++i) {
            codegen((ks_ast)self->v_list->items[i], to, geni);
        }

        // add meta
        ks_code_add_meta(to, self);

        // call those items on the stack
        ksc_getitem(to, self->v_list->len);

        // 1 is for the result of the function, minus all the things used to call it
        STK_GROW(1 - self->v_list->len);

    } else if (self->atype == KS_AST_FUNC) {
        // generate the function code
        ks_code new_code = ks_ast_codegen(self->v_func.body, NULL);

        // create a function
        ks_kfunc new_kfunc = ks_kfunc_new(self->v_func.params, new_code);

        // add meta
        ks_code_add_meta(to, self);

        // load the object
        ksc_const(to, (kso)new_kfunc);
        // we add on one object
        STK_GROW(1);
    } else if (self->atype == KS_AST_RET) {

        // generate the value
        codegen(self->v_ret, to, geni);

        // add meta
        ks_code_add_meta(to, self);

        ksc_ret(to);

        STK_GROW(0);

    } else if (self->atype == KS_AST_TUPLE) {
        // calculate all the elements (use the same thing as list)
        for (i = 0; i < self->v_list->len; ++i) {
            codegen((ks_ast)self->v_list->items[i], to, geni);
        }

        // add meta
        ks_code_add_meta(to, self);

        // call those items on the stack
        ksc_tuple(to, self->v_list->len);

        // 1 is for the result of the function, minus all the things used to call it
        STK_GROW(1 - self->v_list->len);
    } else if (self->atype == KS_AST_LIST) {

        // calculate all the elements
        for (i = 0; i < self->v_list->len; ++i) {
            codegen((ks_ast)self->v_list->items[i], to, geni);
        }

        // add meta
        ks_code_add_meta(to, self);

        // call those items on the stack
        ksc_list(to, self->v_list->len);

        // 1 is for the result of the function, minus all the things used to call it
        STK_GROW(1 - self->v_list->len);
    } else if (self->atype == KS_AST_CODE) {

        // add meta
        ks_code_add_meta(to, self);
        
        // output some literal assembly, by linking it in using the method provided in kso.c
        // this will also merge the constants list
        ks_code_linkin(to, self->v_code);

    } else if (self->atype == KS_AST_BLOCK) {
    
        // output all the sub block items
        for (i = 0; i < self->v_list->len; ++i) {
            codegen((ks_ast)self->v_list->items[i], to, geni);

            // discard any modifications made to the stack by these items
            STK_TO(0);
        }

        STK_GROW(0);

    } else if (self->atype == KS_AST_IF) {

        // first, generate the conditional
        // TODO: generate short-circuit jumping code
        codegen(self->v_if.cond, to, geni);

        // capture the position where the jump instruction begins
        int cond_jmpf_p = to->bc_n;
        // this will be filled in later
        ksc_jmpf(to, -1);
        // jmpf consumes one argument
        STK_GROW(-1);
        // capture the position where the jump shall be made from
        int cond_jmpf_a_p = to->bc_n;

        // next, generate the body
        codegen(self->v_if.body, to, geni);
        STK_TO(0);
        // capture the position after the body
        int body_a_p = to->bc_n;

        // now, get the actual instruction
        ksbc_i32* jmpf_i = (ksbc_i32*)(to->bc + cond_jmpf_p);

        // fill in the bytecode
        jmpf_i->i32 = body_a_p - cond_jmpf_a_p;

    } else if (self->atype == KS_AST_WHILE) {

        // capture the start of the loop
        int start_p = to->bc_n;

        // first, generate the conditional
        // TODO: generate short-circuit jumping code
        codegen(self->v_while.cond, to, geni);

        // capture the position where the jump instruction begins
        int cond_jmpf_p = to->bc_n;
        // this will be filled in later
        ksc_jmpf(to, -1);
        // jmpf consumes one argument
        STK_GROW(-1);
        // capture the position where the jump shall be made from
        int cond_jmpf_a_p = to->bc_n;

        // next, generate the body
        codegen(self->v_if.body, to, geni);
        STK_TO(0);
        // capture the position after the body
        int body_a_p = to->bc_n;

        // now, always jump back to the start of the loop
        ksc_jmp(to, -1);

        // capture position after the jump
        int jmp_a_p = to->bc_n;


        // now, get the actual instruction to jump forward and skip the loop
        ksbc_i32* jmpf_i = (ksbc_i32*)(to->bc + cond_jmpf_p);
        // get the instruction to jump back and loop again
        ksbc_i32* jmp_i = (ksbc_i32*)(to->bc + body_a_p);

        // fill in the bytecode
        jmpf_i->i32 = jmp_a_p - cond_jmpf_a_p;
        jmp_i->i32 = start_p - jmp_a_p;


    } else {
        kse_fmt("Given invalid AST (@%p) with type %i", self, self->atype);
        return;
    }

    geni->call_depth--;

    //set the main level call depth last value
    if (geni->call_depth == 1) {
        geni->last_main = self;
    }

}

ks_code ks_ast_codegen(ks_ast self, ks_list v_const) {

    if (v_const == NULL) {
        v_const = ks_list_new_empty();
        v_const->refcnt--;
    }
    
    ks_code code = ks_code_new_empty(v_const);

    struct cgi geni;
    geni.last_main = NULL;
    geni.stk_depth = 0;
    geni.call_depth = 0;

    codegen(self, code, &geni);

    // end with a return none, if it hasn't been filled out yet
    if (geni.last_main == NULL || (geni.last_main->atype != KS_AST_RET)) ksc_ret_none(code);
    
    return code;
}


