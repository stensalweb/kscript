/* parser.c - implementation of the builtin parser
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"

#include <ctype.h>

// forward declare it
KS_TYPE_DECLFWD(ks_type_parser);


// true if the character is a valid start to an identifier
static bool is_ident_s(char c) {
    return isalpha(c) || c == '_';
}

// true if the character is a valid middle part of an identifier
static bool is_ident_m(char c) {
    return is_ident_s(c) || isdigit(c);
}

// true if the character is whitespace
static bool is_white(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

// tokenize a parser; only should be called at initialization
static void* tokenize(ks_parser self) {

    // add a token to the parser, using defined local variables
    #define ADDTOK(_toktype) { \
        self->tok = ks_realloc(self->tok, sizeof(*self->tok) * ++self->tok_n); \
        self->tok[self->tok_n - 1] = (ks_tok){ .parser = self, .pos = start_i, .len = i - start_i, .line = start_line, .col = start_col }; \
    }

    // advance a single character, updating variables as needed
    #define ADV() { \
        char c = self->src->chr[i++]; \
        if (c == '\n') { \
            line++; \
            col = 0; \
        } else { \
            col++; \
        } \
    }

    // advnace '_n' characters
    #define ADVn(_n) { \
        int ni = (_n); \
        while (ni-- > 0) ADV(); \
    }

    int line = 0, col = 0;

    int i;
    for (i = 0; i < self->src->len; ) {

        // skip whitespace
        while (self->src->chr[i] && is_white(self->src->chr[i])) ADV();

        // done
        if (i >= self->src->len) break;

        // capture where we started so the ADDTOK macro constructs them correctly
        int start_i = i;
        int start_line = line, start_col = col;

        char c = self->src->chr[i];

        // case for a string literal mapping directly to a type
        #define CASE_S(_type, _str) else if (strncmp(&self->src->chr[i], _str, sizeof(_str) - 1) == 0) { ADVn(sizeof(_str) - 1); ADDTOK(_type); }

        // handle the basics
        if (is_ident_s(c)) {
            // read all we can
            do {
                ADV();
            } while (self->src->chr[i] && is_ident_m(self->src->chr[i]));

            ADDTOK(KS_TOK_IDENT);
        } else if (isdigit(c) || (c == '.' && isdigit(self->src->chr[i+1]))) {
            // parse out a numerical constant
            // it could be 

            // go through all the digits we can
            while (isdigit(self->src->chr[i])) {
                ADV();
            }

            if (self->src->chr[i] == '.') {
                // we are parsing some sort of float
                return ks_throw(ks_new_str("No floats allowed!"));


            } else {
                // we are parsing an integer, so we're fininshed
                ADDTOK(KS_TOK_INT);
            }


        }
        CASE_S(KS_TOK_DOT, ".")
        CASE_S(KS_TOK_COL, ":")
        CASE_S(KS_TOK_SEMI, ";")
        CASE_S(KS_TOK_LPAR, "(")
        CASE_S(KS_TOK_RPAR, ")")
        CASE_S(KS_TOK_LBRK, "[")
        CASE_S(KS_TOK_RBRK, "]")
        CASE_S(KS_TOK_LBRC, "{")
        CASE_S(KS_TOK_RBRC, "}")
        else {
            // internal error
            ks_throw((ks_obj)ks_new_str("Invalid Character!"));
            return NULL;
        }


        #undef CASE_S
    }

    int start_i = i;
    int start_line = line, start_col = col;

    // add a buffer of 1 EOF so you can always check the next token without having to worry about seg faults
    ADDTOK(KS_TOK_EOF);

    // non-null
    return self;
}

// construct a new parser
ks_parser ks_new_parser(ks_str src_code) {
    ks_parser self = KS_ALLOC_OBJ(ks_parser);
    KS_INIT_OBJ(self, ks_type_parser);

    // set specific variables
    self->src = src_code;
    KS_INCREF(src_code);

    // start with empty token array
    self->tok_n = 0;
    self->tok = NULL;

    // ensure we can tokenize the input
    if (!tokenize(self)) {
        KS_DECREF(self);
        return NULL;
    }


    return self;
}


// Parse a single expression out of 'p'
// NOTE: Returns a new reference
ks_ast ks_parser_parse_expr(ks_parser p) {

}

// Parse a single statement out of 'p'
// NOTE: Returns a new reference
ks_ast ks_parser_parse_stmt(ks_parser p) {

}

// Parse the entire file out of 'p', returning the AST of the program
// Or, return NULL if there was an error (and 'throw' the exception)
// NOTE: Returns a new reference
ks_ast ks_parser_parse_file(ks_parser p) {
    
}


/* member function */

// parser.__free__(self) -> free a parser object
static KS_TFUNC(parser, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_parser self = (ks_parser)args[0];
    KS_REQ_TYPE(self, ks_type_parser, "self");
    
    // free misc here
    KS_DECREF(self->src);

    ks_free(self->tok);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
    return KSO_NONE;
};


// initialize parser type
void ks_type_parser_init() {
    KS_INIT_TYPE_OBJ(ks_type_parser, "parser");

    ks_type_set_cn(ks_type_parser, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_new_cfunc(parser_free_)},
        {NULL, NULL}   
    });
}



