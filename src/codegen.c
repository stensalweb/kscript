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

        // now, add additional metadata about the error, including in-source markup
        ks_str_b_add_fmt(&SB, "\n%.*s" RED BOLD "%.*s" RESET "%.*s\n%*c" RED "^%*c" RESET "\n@ Line %i, Col %i, in '%S'",
            tok.pos - lsi, src + lsi,
            tok.len, src + tok.pos,
            ll - tok.len - tok.pos, src + tok.pos + tok.len,
            tok.col, ' ',
            tok.len - 1, '~',
            tok.line + 1, tok.col + 1,
            tok.parser->src
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

    if (self->kind == KS_AST_VAR) {
        // push on a variable reference (which shoud be the 0th item)
        ksca_load(to, (ks_str)self->children->elems[0]);
        st->stk_len++;
    } else if (self->kind == KS_AST_CONST) {
        // push on a constant
        ksca_push(to, self->children->elems[0]);
        st->stk_len++;

    } else if (self->kind == KS_AST_CALL) {
        // do a function call

        int start_len = st->stk_len;
        // first, calculate the function
        if (!ast_emit((ks_ast)self->children->elems[0], st, to)) return false;
        
        // then, calculate the objects
        int i;
        for (i = 1; i < self->children->len; ++i) {
            if (!ast_emit((ks_ast)self->children->elems[i], st, to)) return false;
        }

        // now, call 'n' items
        ksca_call(to, self->children->len);

        // this will consume this many
        st->stk_len -= self->children->len;

        // internal error if this is not true
        assert(start_len == st->stk_len);


    } else if (self->kind == KS_AST_BLOCK) {
        // emit all the children as ASTs
        int start_len = st->stk_len;

        int i;
        // try emitting all children
        for (i = 0; i < self->children->len; ++i) {
            if (!ast_emit((ks_ast)self->children->elems[i], st, to)) {
                return false;
            }
        }
        // ensure it ends at the same place it starts (blocks do not yield a value)
        while (st->stk_len > start_len) {
            ksca_popu(to);
            st->stk_len--;
        }

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


