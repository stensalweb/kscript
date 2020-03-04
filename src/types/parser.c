/* parser.c - implementation of the builtin parser
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"

#include <ctype.h>

// forward declare it
KS_TYPE_DECLFWD(ks_type_parser);


/* CHARACTER SPECIFICATION */

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

/* UTIL FUNCTIONS */

// give a syntax error at a given token
static void* syntax_error(ks_tok tok, char* fmt, ...) {

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



/* TOKENS */

// tokenize a parser; only should be called at initialization
static void* tokenize(ks_parser self) {

    // add a token to the parser, using defined local variables
    #define ADDTOK(_toktype) { \
        self->tok = ks_realloc(self->tok, sizeof(*self->tok) * ++self->tok_n); \
        self->tok[self->tok_n - 1] = (ks_tok){ .parser = self, .type = _toktype, .pos = start_i, .len = i - start_i, .line = start_line, .col = start_col }; \
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
                return ks_throw_fmt(NULL, "No float support yet!");

            } else {
                // we are parsing an integer, so we're fininshed
                ADDTOK(KS_TOK_INT);
            }
        }
        CASE_S(KS_TOK_NEWLINE, "\n")

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
            // unexpected character

            // construct a dummy character with just 1 char
            ADV();
            ks_tok badtok = (ks_tok){ 
                .parser = self, .type = KS_TOK_NONE, 
                .pos = start_i, .len = i - start_i, 
                .line = start_line, .col = start_col 
            };

            // give a syntax error
            return syntax_error(badtok, "Unexpected Character: '%c'", c);
            //return ks_throw_fmt(NULL, "Invalid Character: %c", c);
        }

        #undef CASE_S
    }

    #undef ADV
    #undef ADVn
    #undef ADD_TOK

    int start_i = i;
    int start_line = line, start_col = col;

    // add a buffer of 1 EOF so you can always check the next token without having to worry about seg faults
    ADDTOK(KS_TOK_EOF);

    // non-null
    return self;
}

// construct a new parser
ks_parser ks_parser_new(ks_str src_code) {
    ks_parser self = KS_ALLOC_OBJ(ks_parser);
    KS_INIT_OBJ(self, ks_type_parser);

    // set specific variables
    self->src = src_code;
    KS_INCREF(src_code);

    // start with empty token array
    self->toki = 0;
    self->tok_n = 0;
    self->tok = NULL;

    // ensure we can tokenize the input
    if (!tokenize(self)) {
        KS_DECREF(self);
        return NULL;
    }

    return self;
}


/* PARSING UTILITY MACROS */


// Get a boolean expression telling whether the parser is in a valid parsing state
// (i.e. has not run out of bounds)
// NOTE: Assumes the parser is named 'self'
#define VALID() (self->toki < self->tok_n && self->tok[self->toki].type != KS_TOK_EOF)

// Get the current token for a parser
// NOTE: Assumes the parser is named 'self'
#define CTOK() (self->tok[self->toki])

// Advance a single token in the input
// NOTE: Assumes the parser is named 'self'
#define ADV_1() (self->toki++)

// Use this macro to skip tokens that are irrelevant to expressions
// NOTE: Assumes the parser is named 'self'
#define SKIP_IRR_E() {  \
    while (VALID() && (CTOK().type == KS_TOK_COMMENT)) { \
        ADV_1(); \
    } \
}

// Use this macro to skip tokens that are irrelevant to statements
// NOTE: Assumes the parser is named 'self'
#define SKIP_IRR_S() {  \
    ks_tok _ctok; \
    while (VALID() && ((_ctok = CTOK()).type == KS_TOK_NEWLINE || _ctok.type == KS_TOK_COMMENT || _ctok.type == KS_TOK_SEMI)) { \
        ADV_1(); \
    } \
}

// Parse a single expression out of 'p'
// NOTE: Returns a new reference
ks_ast ks_parser_parse_expr(ks_parser self) {

    return NULL;
}

// Parse a single statement out of 'p'
// NOTE: Returns a new reference
ks_ast ks_parser_parse_stmt(ks_parser self) {

    // skip irrelevant characters
    SKIP_IRR_S();

    // ensure we are still in range
    if (!VALID()) {
        syntax_error(CTOK(), "Unexpected EOF");
        goto kpps_err;
    }


    // capture current token
    ks_tok ctok = CTOK();
    // capture starting token
    ks_tok start_tok = ctok;

    if (ctok.type == KS_TOK_LBRC) {
        // { STMT... }
        // parse a block of other statements out
        ADV_1();

        // create a block of statements
        ks_ast blk = ks_ast_new_block(0, NULL);

        // skip anything irrelevant
        SKIP_IRR_S();

        // until we reach matching '}'
        while (VALID() && CTOK().type != KS_TOK_RBRC) {
            // try and parse a statement
            ks_ast stmt = ks_parser_parse_stmt(self);
            if (!stmt) {
                KS_DECREF(blk);
                goto kpps_err;
            }

            // take off irrelevant
            SKIP_IRR_S();

        }
        // error; we got to the end without getting a '}'
        if (CTOK().type != KS_TOK_RBRC) {
            KS_DECREF(blk);
            syntax_error(start_tok, "Failed to find ending '}' for this block");
            goto kpps_err;
        }


        return blk;
    }



    // no pattern was found, error out
    return syntax_error(start_tok, "Unexpected start of statement");

    kpps_err:
    return NULL;
}

// Parse the entire file out of 'p', returning the AST of the program
// Or, return NULL if there was an error (and 'throw' the exception)
// NOTE: Returns a new reference
ks_ast ks_parser_parse_file(ks_parser self) {
    // declare allocated variables
    ks_ast blk = NULL;

    // the current token
    ks_tok ctok = { .type = KS_TOK_NONE };

    // block of contents
    blk = ks_ast_new_block(0, NULL);

    // skip irrelevant characters
    SKIP_IRR_S();

    // keep parsing until we hit an end
    while (VALID()) {

        // try and parse a single statement
        ks_ast sub = ks_parser_parse_stmt(self);
        if (sub == NULL) goto kppf_err;

        // add to the block
        ks_list_push(blk->children, (ks_obj)sub);
        KS_DECREF(sub);

        // skip more irrelevant details
        SKIP_IRR_S();
    }

    // return the entire block
    return blk;

    // label to handle an error 
    kppf_err:

    // handle any variables that have been allocated
    if (blk) KS_DECREF(blk);

    return NULL;
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
        {"__free__", (ks_obj)ks_cfunc_new(parser_free_)},
        {NULL, NULL}   
    });
}



