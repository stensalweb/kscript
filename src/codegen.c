/* codegen.c - Code generation routines for translating AST -> bytecode
 *
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// define a structure type to hold current code generation values
typedef struct {

    // the current stack length, starting at 0
    int stk_len;

} em_state;


// give a code error
static void* code_error(ks_tok tok, char* fmt, ...) {

    // do string formatting
    va_list ap;
    va_start(ap, fmt);
    ks_str what = ks_fmt_vc(fmt, ap);
    va_end(ap);

    ks_str_b SB;
    ks_str_b_init(&SB);

    ks_str_b_add_str(&SB, (ks_obj)what);
    
    if (tok.parser != NULL && tok.len >= 0) {
        // we have a valid token
        int i = tok.pos;
        int lineno = tok.line;
        char c;

        char* src = tok.parser->src->chr;

        // rewind to the start of the line
        while (i >= 0) {
            c = src[i];

            if (c == '\n') {
                i++;
                break;
            }
            i--;
        }



        if (i < 0) i++;
        if (src[i] == '\n') i++;

        // line start index
        int lsi = i;

        while (c = src[i]) {
            if (c == '\n' || c == '\0') break;
            i++;
        }


        // line length
        int ll = i - lsi;

        // the start of the line
        char* sl = src + lsi;

        // now, add additional metadata about the error, including in-source markup
        ks_str_b_add_fmt(&SB, "\n%.*s" RED BOLD "%.*s" RESET "%.*s\n%*c" RED "^%*c" RESET "\n@ Line %i, Col %i, in '%S'",
            tok.col, sl,
            tok.len, sl + tok.col,
            ll - tok.col - tok.len, sl + tok.col + tok.len,
            tok.col, ' ',
            tok.len - 1, '~',
            tok.line + 1, tok.col + 1,
            tok.parser->src_name
        );
    }

    ks_str full_what = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    ks_Error errval = ks_Error_new(full_what);
    KS_DECREF(full_what);

    // throw our error
    ks_throw((ks_obj)errval);
    KS_DECREF(errval);

    return NULL;
}



// actually emit bytecode for an AST
static bool ast_emit(ks_ast self, em_state* st, ks_code to) {

    assert(self->type == ks_type_ast);
    // capture the starting length
    int start_len = st->stk_len;

    // Reset the stack to a relative position after where it started
    #define RESET_STK(_rel) {                        \
        while (st->stk_len > start_len + (_rel)) {   \
            ksca_popu(to);                           \
            st->stk_len--;                           \
        }                                            \
    }
    if (self->kind == KS_AST_VAR) {
        
        // variable name
        ks_str vname = (ks_str)self->children->elems[0];

        // check for builtins
        if (strcmp(vname->chr, "true") == 0) {
            ksca_push(to, KSO_TRUE);
        } else if (strcmp(vname->chr, "false") == 0) {
            ksca_push(to, KSO_FALSE);
        } else if (strcmp(vname->chr, "none") == 0) {
            ksca_push(to, KSO_NONE);
        } else {
            // generic variable, not a builtin
            ksca_load(to, vname);
        }

        // add meta data
        ks_code_add_meta(to, self->tok_expr);


        // record the new item on the stack
        st->stk_len++;

    } else if (self->kind == KS_AST_CONST) {

        // add meta data
        ks_code_add_meta(to, self->tok_expr);

        // push on a constant
        ksca_push(to, self->children->elems[0]);
        st->stk_len++;

    } else if (self->kind == KS_AST_ATTR) {

        // first, emit the object we are taking the attribute of
        if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;

        // make sure everything was good
        assert(start_len + 1 == st->stk_len && "Attribute object node was not emitted correctly!");

        // now,  attribute calculate it
        ksca_load_attr(to, (ks_str)self->children->elems[1]);

        // add meta data
        ks_code_add_meta(to, self->tok_expr);

        // don't increase the stack length, since it will just replace TOS


    } else if (self->kind == KS_AST_CALL) {
        // do a function call

        // first, calculate the function
        if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;

        // ensure the function emitted one 
        assert(start_len + 1 == st->stk_len && "Function node was not emitted correctly!");

        // then, calculate the objects
        int i;
        for (i = 1; i < self->children->len; ++i) {
            if (!ast_emit((ks_ast)self->children->elems[i], st, to)) return false;
        }

        // ensure correct number of arguments were emitted
        assert(start_len + self->children->len == st->stk_len && "Function arguments was not emitted correctly!");

        // now, call 'n' items
        ksca_call(to, self->children->len);

        // add meta data
        ks_code_add_meta(to, self->tok_expr);

        // this will consume this many
        st->stk_len -= self->children->len;
        
        // but push on the result
        st->stk_len++;

        // internal error if this is not true
        assert(st->stk_len == start_len + 1 && "Function output was not emitted correctly!");

    } else if (self->kind == KS_AST_THROW) {

        // do a throw

        // first, calculate the function
        if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;

        // ensure the function emitted one 
        assert(start_len + 1 == st->stk_len && "'throw' expr was not emitted correctly!");

        // throw the value
        ksca_throw(to);

        // add meta data
        ks_code_add_meta(to, self->tok_expr);

        // the stack gets rewound, so don't touch this here
        RESET_STK(0);


    } else if (self->kind == KS_AST_BOP_ASSIGN) {
        // assignment operator is a special case
        ks_ast L = (ks_ast)self->children->elems[0], R = (ks_ast)self->children->elems[1];

        if (L->kind == KS_AST_VAR) {
            // assign to a variable via a 'store'

            // first, calculate the result
            if (!ast_emit(R, st, to)) return false;

            // then store it to the given name
            ksca_store(to, (ks_str)L->children->elems[0]);

            // add meta data
            ks_code_add_meta(to, self->tok_expr);


        } else if (L->kind == KS_AST_ATTR) {
            // do a setattr call

            // compute the base object that is being set
            if (!ast_emit((ks_ast)L->children->elems[0], st, to)) return false;

            // then calculate the value
            if (!ast_emit(R, st, to)) return false;

            // then store it to the given name
            ksca_store_attr(to, (ks_str)L->children->elems[1]);

            // add meta data
            ks_code_add_meta(to, self->tok_expr);

            // the total number shrinks by 1
            st->stk_len--;

        } else {

            //printf("self: %i\n", L->tok.pos);

            code_error(L->tok, "Cannot assign to LHS! Must be a variable!");
            return false;
        }


    } else if (KS_AST_BOP__FIRST <= self->kind && self->kind <= KS_AST_BOP__LAST) {
        // handle general binary operators
        ks_ast L = (ks_ast)self->children->elems[0], R = (ks_ast)self->children->elems[1];
        // have stack like | L R
        if (!ast_emit(L, st, to) || !ast_emit(R, st, to)) return false;

        assert(st->stk_len == start_len + 2 && "Binary operator did not emit 2 operands!");


        // actually do binary operation, translate
        ksca_bop(to, (self->kind - KS_AST_BOP__FIRST) + KSB_BOP_ADD);

        // add meta data
        ks_code_add_meta(to, self->tok_expr);
        

        // both arguments are consumed
        st->stk_len -= 2;
        // one result is poppped back on
        st->stk_len++;


    } else if (self->kind == KS_AST_BLOCK) {
        // emit all the children as ASTs

        int i;
        // try emitting all children
        for (i = 0; i < self->children->len; ++i) {
            if (!ast_emit((ks_ast)self->children->elems[i], st, to)) return false;

            // Ensure nothing 'dipped' below the starting stack length for the block
            assert(st->stk_len >= start_len && "Block's children did not emit correctly (they dipped below)");

            // ensure it ends at the same place it starts (blocks do not yield a value)
            RESET_STK(0);

        }
    } else if (self->kind == KS_AST_IF) {

        // first, generate the conditional
        if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;

        assert(st->stk_len == start_len + 1 && "'if' conditional did not produce a value!");

        // else block (or NULL if it doesnt exist)
        ks_ast else_blk = (ks_ast)(self->children->len > 2 ? self->children->elems[2] : NULL);


        // now, insert a conditional jump, and fill it in later
        // where to store jump information
        int cj_i = to->bc_n;

        // temporary value
        // this should pop off the starting length
        ksca_jmpf(to, -1);
        st->stk_len--;

        // record the location it will be jumping from
        int cj_a = to->bc_n;

        // now, generate the main body
        if (!ast_emit((ks_ast)self->children->elems[1], st, to)) return false;
        RESET_STK(0);


        // now, handle 'else' if it iexists
        if (else_blk) {

            // if there's an else, the 'if' block should jump to after it, so create
            //   an unconditional jump

            // record the position it is encoded
            int ij_i = to->bc_n;

            // this does not affect stack, and will be filled in later
            ksca_jmp(to, -1);

            // where it will be jumping from
            int ij_a = to->bc_n;

            // generate the 'else' block
            if (!ast_emit(else_blk, st, to)) return false;
            RESET_STK(0);


            // now, fill in both the conditional jump and the inner if jump

            // capture the 'afterward' position
            int after_i = to->bc_n;

            ksb_i32* cj_p = (ksb_i32*)(&to->bc[cj_i]);

            // store local offset
            cj_p->arg = (int32_t)(ij_a - cj_a);

            // fill in the inner jump

            ksb_i32* ij_p = (ksb_i32*)(&to->bc[ij_i]);

            // store local offset
            ij_p->arg = (int32_t)(after_i - ij_a);


        } else {
            // no else block, so just fill in the original jump

            // capture the 'afterward' position
            int after_i = to->bc_n;

            ksb_i32* cj_p = (ksb_i32*)(&to->bc[cj_i]);

            // store local offset
            cj_p->arg = (int32_t)(after_i - cj_a);

        }
    } else if (self->kind == KS_AST_WHILE) {
        // generate a while loop

        // the start of the while loop (where it must jump back to later)
        int ws_i = to->bc_n;

        // first, generate the conditional
        if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;

        assert(st->stk_len == start_len + 1 && "'while' conditional did not produce a value!");


        // get position of the jump
        int wj_i = to->bc_n;

        // fill it in later
        ksca_jmpf(to, -1);
        st->stk_len--;

        // get position it is jumping from
        int wj_a = to->bc_n;


        // else block (or NULL if it doesnt exist)
        ks_ast else_blk = (ks_ast)(self->children->len > 2 ? self->children->elems[2] : NULL);

        if (else_blk) {
            // in this case, we want to redirect the first one to the 'else' block, but the other ones to afterwards,
            //   since the loop had ran


            // generate the body
            if (!ast_emit((ks_ast)self->children->elems[1], st, to)) return false;
            RESET_STK(0);

            // now, generate another expression to tell if we should jump back
            if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;


            // have a jump from the body back to the start of the body

            // get position of the jump
            int bj_i = to->bc_n;

            // fill it in later
            ksca_jmpt(to, -1);
            st->stk_len--;

            // get position it is jumping from
            int bj_a = to->bc_n;

            // create another jump
            int bj_ai = bj_a;

            // if it was false, end up linking  to after the else block, since it would have
            //   ran at least once
            ksca_jmp(to, -1);

            int bj_aa = to->bc_n;

            // now link the body's jump
            ksb_i32* bj_p = (ksb_i32*)(&to->bc[bj_i]);

            // set it to the difference
            bj_p->arg = (int)(wj_a - bj_a);


            // now, set the original jump to jump after all this
            ksb_i32* wj_p = (ksb_i32*)(&to->bc[wj_i]);

            // set the relative amount to right before the else block
            wj_p->arg = (int)(to->bc_n - wj_a);

            // now, emit the other block
            // now, generate another expression to tell if we should jump back
            if (!ast_emit(else_blk, st, to)) return false;
            RESET_STK(0);

            // now link the body's jump
            ksb_i32* bj_ap = (ksb_i32*)(&to->bc[bj_ai]);

            // set it to the difference
            bj_ap->arg = (int)(to->bc_n - bj_aa);



        } else {
            // no else block, standard routine

            // generate the body
            if (!ast_emit((ks_ast)self->children->elems[1], st, to)) return false;
            RESET_STK(0);
            
            // now, generate another expression to tell if we should jump back
            if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;


            // have a jump from the body back up to after the original conditional

            // get position of the jump
            int bj_i = to->bc_n;

            // fill it in later
            ksca_jmpt(to, -1);
            st->stk_len--;

            // get position it is jumping from
            int bj_a = to->bc_n;

            ksb_i32* bj_p = (ksb_i32*)(&to->bc[bj_i]);

            // set it to the difference
            bj_p->arg = (int)(wj_a - bj_a);


            // now, set the original jump to jump after all this
            ksb_i32* wj_p = (ksb_i32*)(&to->bc[wj_i]);

            wj_p->arg = (int)(bj_a - wj_a);

        }
    } else if (self->kind == KS_AST_TRY) {
        // execute a try catch block

        ks_ast b_try = (ks_ast)self->children->elems[0], b_catch = (ks_ast)self->children->elems[1];


        // position the instruction is at
        int tj_i = to->bc_n;

        // first, pop on a new catch block
        ksca_try_start(to, -1);

        // position jumping from
        int tj_a = to->bc_n;

        // now, generate the body
        if (!ast_emit(b_try, st, to)) return false;
        RESET_STK(0);

        
        int ej_i = to->bc_n;

        // end the try, and jump to the location afterwards
        ksca_try_end(to, -1);

        int ej_a = to->bc_n;


        // there will be an item on the stack, pushed by the exception handler, so keep track of that here:
        st->stk_len++;

        // TODO: add an assignment, right now just reset it
        RESET_STK(0);

        // now, start generating the 'catch' body
        if (!ast_emit(b_catch, st, to)) return false;
        RESET_STK(0);
        

        // after catch location
        int after_catch = to->bc_n;

        // now, fill in the jumps

        ksb_i32* tj_p = (ksb_i32*)(&to->bc[tj_i]);

        // the try jump needs to jump to where the exception handler starts
        tj_p->arg = (int)(ej_a - tj_a);


        ksb_i32* ej_p = (ksb_i32*)(&to->bc[ej_i]);

        // the end jump needs to jump to after the entire thing
        ej_p->arg = (int)(after_catch - ej_a);


    } else {
        code_error(self->tok, "Unknown AST Type! (type:%i)", self->kind);
        return false;
    }

    // success
    return true;

}




// Generate corresponding bytecode for a given AST
// NOTE: Returns a new reference
// See `codegen.c` for more info
ks_code ks_codegen(ks_ast self) {

    // construct an empty bytecode here
    ks_code to = ks_code_new(NULL);

    // the current state
    em_state st = (em_state) { .stk_len = 0 };


    // emit the main function
    if (!ast_emit(self, &st, to)) {
        // if there was an error
        KS_DECREF(to);
        return NULL;
    }

    // return none at the end
    ksca_push(to, KSO_NONE);
    ksca_ret(to);

    return to;
}


