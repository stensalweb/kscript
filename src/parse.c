/* parse.c - kscript language (& bytecode assembly) parser and tools */

#include "ks.h"

#include <ctype.h>

// whether or not a character is whitespace
static inline bool iswhite(char c) {
    return c == ' ' || c == '\t';
}

// whether or not a character is a valid start to an identifier
static inline bool isidents(char c) {
    return isalpha(c) || c == '_';
}

// whether or not a character is a valid middle part of an identifier
static inline bool isidentm(char c) {
    return isidents(c) || isdigit(c);
}

ks_tok ks_tok_new(int ttype, ks_parser kp, int offset, int len, int line, int col) {
    return (struct ks_tok) {
        .ttype = ttype,
        .v_parser = kp,
        .offset = offset, .len = len,
        .line = line, .col = col
    };
}

#define _MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define _MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))

// combine A and B to form a larger meta token
ks_tok ks_tok_combo(ks_tok A, ks_tok B) {
    return ks_tok_new(KS_TOK_COMBO, A.v_parser, _MIN(A.offset, B.offset), _MAX(A.offset+A.len, B.offset+B.len) - _MIN(A.offset, B.offset), _MIN(A.line, B.line), _MIN(A.col, B.col));
}

// gets an integer from a token, assuming its a token type
static int64_t tok_getint(ks_tok tok) {
    static char tmp[100];
    int len = tok.len;
    if (len > 99) len = 99;
    memcpy(tmp, tok.v_parser->src->chr + tok.offset, len);
    tmp[len] = '\0';
    return atoll(tmp);
}

static ks_str tok_getstr(ks_tok tok) {
    ks_str start = ks_str_new_r("");

    char* src = tok.v_parser->src->chr + tok.offset;

    // skip '"'
    int i;
    for (i = 1; i < tok.len && src[i] != '"'; ) {
        int j = 0;
        // go for as many literals as possible
        while (src[i + j] != '\0' && src[i + j] != '"' && src[i + j] != '\\') {
            j++;
        }

        if (j > 0) {
            // handle literals
            ks_str new_str = ks_str_new_cfmt("%*s%*s", start->len, start->chr, j, src+i);
            KSO_CHKREF(start);
            start = new_str;
            i += j;
        } else if (src[i] == '\\') {
            // handle an escape code
            char c = src[++i];
            char lit = '\0';
            bool hasOne = true;
            if (c == 'n') {
                lit = '\n';
            } else {
                hasOne = false;
            }

            if (hasOne) {
                ks_str new_str = ks_str_new_cfmt("%*s%c", start->len, start->chr, lit);
                KSO_CHKREF(start);
                start = new_str;
            }

            // skip the escape code
            i++;
        } else {
            // something weird happen, just stop
            return start;
        }

    }

    return start;
}


#define tok_err ks_tok_err
// have an error from a token
void* ks_tok_err(ks_tok tok, const char* fmt, ...) {

    // first, just compute the error string
    va_list ap;
    va_start(ap, fmt);
    ks_str errstr = ks_str_new_vcfmt(fmt, ap);
    va_end(ap);


    if (tok.v_parser != NULL && tok.len > 0) {
        // we have a valid token
        int i = tok.offset;
        int lineno = tok.line;
        char c;

        char* src = tok.v_parser->src->chr;

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

        // line start i
        int lsi = i;

        while (c = src[i]) {
            if (c == '\n' || c == '\0') break;
            i++;
        }

        // line length
        int ll = i - lsi;
        ks_str new_err_str = ks_str_new_cfmt("%*s\n%*s\n%*c^%*c\n@ Line %i, Col %i, in '%*s'", 
            (int)errstr->len, errstr->chr, 
            ll, src + lsi,
            tok.col, ' ',
            tok.len - 1, '~',
            tok.line + 1, tok.col + 1, 
            tok.v_parser->src_name->len, tok.v_parser->src_name->chr
        );

        KSO_CHKREF(errstr);
        errstr = new_err_str;

    }


    kse_addo(errstr);
    KSO_CHKREF(errstr);

    return NULL;
}


// internal method to initialize the parser, tokenize it, etc
static void parser_init(ks_parser self) {

    // get a pointer to the contents
    char* contents = self->src->chr;

    // current line and column
    int line = 0, col = 0;

    // advance `n` characters 
    #define ADV(_n) { int _i; for (_i = 0; _i < (_n); ++_i) { char _c = contents[i]; if (!_c) break; if (_c == '\n') { line++; col = 0; } else { col++; } i++; } }

    int i = 0;
    while (contents[i]) {
        // skip whitespace
        while (contents[i] && iswhite(contents[i])) {
            ADV(1);
        }

        char c = contents[i];

        // check for NUL
        if (!c) break;

        // adds a token, given the internal variables start_i, and i
        #define ADD_TOK(_type) { self->toks = ks_realloc(self->toks, sizeof(*self->toks) * ++self->n_toks); self->toks[self->n_toks - 1] = ks_tok_new(_type, self, start_i, i - start_i, start_line, start_col); }

        int start_i = i, start_line = line, start_col = col;

        if (c == '#') {
            // parse a comment
            ADV(1);

            while (contents[i] && contents[i] != '\n') {
                ADV(1);

            }

            ADD_TOK(KS_TOK_COMMENT);

            // skip newline if it exists
            ADV(1);

        } else if (isdigit(c)) {
            // parse a string literal

            do {
                ADV(1);
            } while (isdigit(contents[i]));

            ADD_TOK(KS_TOK_INT);

        } else if (isidents(c)) {
            // parse an identifier
            do {
                ADV(1);
            } while (isidentm(contents[i]));

            ADD_TOK(KS_TOK_IDENT);
        
        } else if (c == '\"') {
            int st_i = i, st_line = line, st_col = col;

            // parse a string literal
            // skip opening quotes
            ADV(1);

            while ((c = contents[i]) && c != '"') {
                if (c == '\\') {
                    // skip an escape code
                    ADV(2);
                } else {
                    ADV(1);
                }
            }

            if (contents[i] != '"') {
                // rewind back for the error
                i = st_i + 2;

                ADD_TOK(KS_TOK_NONE);
                tok_err((ks_tok)self->toks[self->n_toks - 1], "Expected terminating '\"' for string literal that began here:");
                self->n_toks--;
                break;
            }

            ADV(1);

            ADD_TOK(KS_TOK_STR)

        }

        // macro for 1-1 translation of string to tokens
        #define STR_TOK(_type, _str) else if (strncmp(&contents[i], _str, sizeof(_str) - 1) == 0) { \
            ADV(sizeof(_str) - 1);\
            ADD_TOK(_type);\
        }

        // misc
        STR_TOK(KS_TOK_NEWLINE, "\n")

        // random grammars
        STR_TOK(KS_TOK_COMMA, ",")
        STR_TOK(KS_TOK_COLON, ":")
        STR_TOK(KS_TOK_SEMI, ";")

        // pairs
        STR_TOK(KS_TOK_LPAR, "(") STR_TOK(KS_TOK_RPAR, ")")
        STR_TOK(KS_TOK_LBRACK, "[") STR_TOK(KS_TOK_RBRACK, "]")
        STR_TOK(KS_TOK_LBRACE, "{") STR_TOK(KS_TOK_RBRACE, "}")

        // operators
        STR_TOK(KS_TOK_O_ADD, "+") STR_TOK(KS_TOK_O_SUB, "-")
        STR_TOK(KS_TOK_O_MUL, "*") STR_TOK(KS_TOK_O_SUB, "/")

        // special operators
        STR_TOK(KS_TOK_O_ASSIGN, "=")

        else {
            i++;
            ADD_TOK(KS_TOK_NONE);
            tok_err(self->toks[self->n_toks - 1], "Unexpected character '%c'", c);
            self->n_toks--;
            break;
        }

    }

    // add the ending token
    int start_i = i, start_line = line, start_col = col;
    ADD_TOK(KS_TOK_EOF);

    // start at the beginning
    self->tok_i = 0;

}

ks_parser ks_parser_new_expr(const char* expr) {

    ks_parser self = (ks_parser)ks_malloc(sizeof(*self));
    *self = (struct ks_parser) {
        KSO_BASE_INIT(ks_T_parser, KSOF_NONE)
        .src_name = ks_str_new_r("-e"),
        .src = ks_str_new_r(expr),
        .tok_i = 0,
        .n_toks = 0,
        .toks = NULL,
    };

    // record reference
    KSO_INCREF(self->src);
    KSO_INCREF(self->src_name);

    /* now, finalize/tokenize the input */
    parser_init(self);

    return self;

}


ks_parser ks_parser_new_file(const char* fname) {

    FILE* fp = fopen(fname, "r");
    if (fp == NULL) {
        return kse_fmt("While opening file '%s', could not open file!", fname);
    }

    fseek(fp, 0, SEEK_END);
    size_t bytes = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* contents = ks_malloc(bytes + 1);
    if (bytes != fread(contents, 1, bytes, fp)) {
        ks_warn("File being read acted oddly...");
    }
    contents[bytes] = '\0';

    fclose(fp);

    ks_parser self = (ks_parser)ks_malloc(sizeof(*self));
    *self = (struct ks_parser) {
        KSO_BASE_INIT(ks_T_parser, KSOF_NONE)
        .src_name = ks_str_new_r(fname),
        .src = ks_str_new(bytes, contents),
        .tok_i = 0,
        .n_toks = 0,
        .toks = NULL,
    };

    // record reference
    KSO_INCREF(self->src);
    KSO_INCREF(self->src_name);

    // since we made a new string from it
    ks_free(contents);

    /* now, finalize/tokenize the input */
    parser_init(self);

    return self;

}

// yield whether a token equals another value
#define TOK_EQ(_p, _tok, _str) (_tok.len == (sizeof(_str) - 1) && (0 == strncmp(self->src->chr + _tok.offset, _str, (sizeof(_str) - 1))))

// parse out an AST from assembly source file
ks_ast ks_parse_asm(ks_parser self) {

    ks_list v_const = ks_list_new_empty();
    ks_code code = ks_code_new_empty(v_const);
    ks_ast code_ast = ks_ast_new_code(code);

    bool wasErr = false;

    // for keeping tokens to strings
    ks_str tostr;

    // arguments for the instructions
    ks_tok a0, a1;

    #define PASM_ERR(...) { tok_err(__VA_ARGS__); wasErr = true; goto parseasm_end; }

    // keep track of first token
    ks_tok tok_first = {.ttype = KS_TOK_NONE };

    while (true) {
        // keep parsing in a loop

        // check if we're out
        if (self->tok_i >= self->n_toks) return code_ast;

        // get current token
        ks_tok ctok = (ks_tok)self->toks[self->tok_i];

        // check if we should skip
        if (ctok.ttype == KS_TOK_NEWLINE || ctok.ttype == KS_TOK_COMMENT) {
            self->tok_i++;
            continue;
        };
        // check if we should stop parsing
        if (ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_RBRACE) return code_ast;

        if (tok_first.ttype == KS_TOK_NONE) tok_first = ctok;

        if (ctok.ttype == KS_TOK_IDENT) {
            // we will have an assembly command

            // skip that token, but keep ctok as the name of the command
            self->tok_i++;

            if (TOK_EQ(self, ctok, "load")) {
                a0 = (ks_tok)self->toks[self->tok_i++];
                if (a0.ttype == KS_TOK_STR) {
                    // load a string literal name
                    tostr = tok_getstr(a0);
                    ksc_loado(code, (kso)tostr);
                } else {
                    PASM_ERR(a0, "Arg #0 to 'load' instruction must be a string literal");
                }
            } else if (TOK_EQ(self, ctok, "const")) {
                a0 = (ks_tok)self->toks[self->tok_i++];
                if (a0.ttype == KS_TOK_STR) {
                    // load a string literal
                    tostr = tok_getstr(a0);
                    ksc_const(code, (kso)tostr);
                } else if (a0.ttype == KS_TOK_INT) {
                    // load an int literal
                    ksc_int(code, tok_getint(a0));
                } else {
                    PASM_ERR(a0, "Arg #0 to 'const' instruction must be a int literal or string literal");
                }
            } else if (TOK_EQ(self, ctok, "call")) {
                a0 = (ks_tok)self->toks[self->tok_i++];
                if (a0.ttype == KS_TOK_INT) {
                    // call with an integer number of arguments
                    ksc_call(code, (int)tok_getint(a0));
                } else {
                    PASM_ERR(a0, "Arg #0 to 'call' instruction must be an integer");
                }
            } else if (TOK_EQ(self, ctok, "popu")) {
                ksc_popu(code);
            } else {
                PASM_ERR(ctok, "Unknown assembly instruction '%*s'", ctok.len, self->src->chr + ctok.offset);
            }

            ks_tok ntok = (ks_tok)self->toks[self->tok_i];
            // make sure it ended correctly
            if (!(ntok.ttype == KS_TOK_NEWLINE || ntok.ttype == KS_TOK_COMMENT || ntok.ttype == KS_TOK_RBRACK)) {
                PASM_ERR(ntok, "Extra argument for assembly instruction '%*s'", ctok.len, self->src->chr + ctok.offset);
            }

        } else {
            PASM_ERR(ctok, "Unexpected token for asm");
        }
    }

    parseasm_end: ;

    if (wasErr) {
        KSO_DECREF(code_ast);
        return NULL;
    } else {
        code_ast->tok = tok_first;
        code_ast->tok_expr = ks_tok_combo(tok_first, self->toks[self->tok_i]);
        return code_ast;
    }
}


/* expression parsing */

// shunting yard operator structure
typedef struct syop {

    // what type of operator is it?
    enum {
        // err/empty type
        SYT_NONE = 0,

        // unary operator, like -A, ~A
        SYT_UOP,

        // a binary operator, like A+B, A-B
        SYT_BOP,


        /* psuedo-operators */
        // just a left parenthesis (not a function call)
        SYT_LPAR,

        // a function call (including the left parenthesis)
        SYT_FUNC,

        // just a left bracket (not a subscript operation )
        SYT_LBRACK,

        // a subscript operation, i.e. `a[b]`
        SYT_SUBSCRIPT,

        SYT__END

    } type;

    // what is the precedence of the operator
    enum {

        SYP_NONE = 0,


        // assignment i.e. A=B, should always be highest other than that
        SYP_ASSIGN,

        // +,-, in PEMDAS order
        SYP_ADDSUB,

        // *,/ (and %), second highest in PEMDAS
        SYP_MULDIV,

        // ^ exponetentiation, highest in PEMDAS
        SYP_POW,

        // unary operators always have the highest precedence
        SYP_UNARY,


        SYP__END

    } prec;

    // what is the associativity of the operator. left means A+B+C==(A+B)+C, right means A+B+C=A+(B+C)
    enum {

        SYA_NONE = 0,

        // binary operator, left associative
        // this handles most cases, like +,-,*,/,...
        SYA_BOP_LEFT,

        // binary operator, right associative
        // some things are a bit weird, like exponentiation; they are associative from the right
        // essentially, this is the power, as well as the assignment operator
        SYA_BOP_RIGHT,

        // unary prefix operator, like ++x, --x
        SYA_UOP_PRE,

        // unary postfix operator, like x++, x--
        SYA_UOP_POST,


        SYA__END

    } assoc;

    int bop_type;

    // the token the operator came from
    ks_tok tok;

    // true if there was a comma associated (useful for parsing singlet tuples)
    bool had_comma;

} syop;

// construct a basic syop
#define SYOP(_type) ((syop){.type = _type, .had_comma = false})

// construct a binary operator
#define SYBOP(_prec, _assoc, _bop_type) ((syop){ .type = SYT_BOP, .prec = _prec, .assoc = _assoc, .bop_type = _bop_type })

// construct a fully-loaded sy-operator
#define SYOP_FULL(_type, _prec, _assoc) ((syop){ .type = _type, .prec = _prec, .assoc = _assoc })



/* operator definitions */
static syop
    // binary operators
    syb_add = SYBOP(SYP_ADDSUB, SYA_BOP_LEFT, KS_AST_BOP_ADD), syb_sub = SYBOP(SYP_ADDSUB, SYA_BOP_LEFT, KS_AST_BOP_SUB),
    syb_mul = SYBOP(SYP_MULDIV, SYA_BOP_LEFT, KS_AST_BOP_MUL), syb_div = SYBOP(SYP_MULDIV, SYA_BOP_LEFT, KS_AST_BOP_DIV),

    // special case
    syb_assign = SYBOP(SYP_ASSIGN, SYA_BOP_RIGHT, KS_AST_BOP_ASSIGN)

;



// parses a normal infix expression from 'self', using a hybrid shunting yard algorithm
// It seems efficient, but I've been wanting to do larger tests with it
ks_ast ks_parse_expr(ks_parser self) {

    // the output stack, for values
    struct {
        // current length of the stack
        int len;
        // the pointer to the elements
        ks_ast* base;
    } Out = {0, NULL};

    // the operator stack
    struct {
        // current length of the stack
        int len;
        // the pointer to the elements
        syop* base;
    } Ops = {0, NULL};

    /* stack manipulation macros */

    // pushes an item onto the stack
    #define Spush(_stk, _val) { int _idx = (_stk).len++; (_stk).base = ks_realloc((_stk).base, sizeof(*(_stk).base) * _stk.len); (_stk).base[_idx] = (_val); }
    // pops off an item from the stack, yielding the value
    #define Spop(_stk) (_stk).base[--(_stk).len]
    // yields the top value of the stack
    #define Stop(_stk) (_stk).base[(_stk).len - 1]
    // pops off an unused object from the stack (just use this one on Out, not Ops)
    #define Spopu(_stk) { kso _obj = Spop(_stk); KSO_CHKREF(_obj); }
    // gets `idx`'th item of `_stk`
    #define Sget(_stk, _idx) (_stk).base[_idx]

    // pops an operator from `Ops`, takes arguments from `Out`, and then replaces that output in `Out`
    #define POP_OP() { \
        syop top = Stop(Ops); \
        if (top.type == SYT_FUNC) { \
            /* basically: start at the top of the stack, scanning down for our NULL we added to the output 
            to signify the start of the function call */ \
            int osp = Out.len - 1; \
            while (osp >= 0 && Sget(Out, osp) != NULL) osp--; \
            int n_args = Out.len - osp - 1; \
            Sget(Out, osp) = Sget(Out, osp - 1); /* effectively swaps the NULL and the actual function call */ \
            /* now, they are contiguous in memory */ \
            ks_ast new_call = ks_ast_new_call(n_args + 1, &Sget(Out, osp)); \
            new_call->tok = top.tok; \
            new_call->tok_expr = ks_tok_combo(((ks_ast)new_call->v_call->items[0])->tok_expr, ctok); \
            Out.len -= n_args + 2; \
            Spush(Out, new_call);\
        } else if (top.type == SYT_BOP) { \
            /* construct a binary operator from the last two values on the stack */ \
            ks_ast new_bop = ks_ast_new_bop(top.bop_type, Sget(Out, Out.len-2), Sget(Out, Out.len-1)); \
            new_bop->tok = top.tok; \
            new_bop->tok_expr = ks_tok_combo(new_bop->v_bop.L->tok_expr, new_bop->v_bop.R->tok_expr); \
            Out.len -= 2; \
            Spush(Out, new_bop); \
        } else if (top.type == SYT_LPAR) { \
            /* just skip it */\
        } else if (top.type == SYT_LBRACK) { \
            /* we've encountered a list literal, scan down and find how many objects */ \
            int osp = Out.len - 1; \
            while (osp >= 0 && Sget(Out, osp) != NULL) osp--; \
            int n_args = Out.len - osp - 1; \
            /* now, they are contiguous in memory */ \
            ks_ast new_list = ks_ast_new_list(n_args, &Sget(Out, osp+1)); \
            new_list->tok = top.tok; \
            new_list->tok_expr = ks_tok_combo(top.tok, ctok); \
            Out.len -= n_args + 1; \
            Spush(Out, new_list);\
        } else { \
            PEXPR_ERR(ctok, "Internal Operator Error (%i)", top.type); \
        } \
        Spop(Ops); \
    }

    // whether or not there was an error
    bool wasErr = false;

    // macro to cause a token error
    #define PEXPR_ERR(...) { tok_err(__VA_ARGS__); wasErr = true; goto parseexpr_end; }

    // current and last tokens
    ks_tok ctok = { .ttype = KS_TOK_NONE }, ltok = { .ttype = KS_TOK_NONE };



    // tells you whether or not a given token type means that the last value was a value, or an operator
    #define TOKE_ISVAL(_type) (_type == KS_TOK_INT || _type == KS_TOK_STR || _type == KS_TOK_IDENT || _type == KS_TOK_RPAR)

    // tells you whether or not a given token is a binary operator
    #define TOKE_ISBOP(_type) (_type >= KS_TOK_O_ADD && _type <= KS_TOK_O_ASSIGN)

    // tells you whether ot not a given token is a unary operator
    #define TOKE_ISUOP(_type) (false)

    // tells you whether or not a given token is any kind of operator
    #define TOKE_ISOP(_type) (TOKE_ISBOP(_type) || TOKE_ISUOP(_type))


    // number of parenthesis, left - right, this number should always be >= 0
    int n_pars = 0;

    // number of brackets, left - right, this number should always be >= 0
    int n_bracks = 0;

    // get the starting token
    int start_tok_i = self->tok_i;


    while (true) {
        // make sure we're in bounds
        if (self->tok_i >= self->n_toks) goto parseexpr_end;

        // get current token
        ctok = self->toks[self->tok_i];

        // check if we should skip
        if (ctok.ttype == KS_TOK_COMMENT) {
            self->tok_i++;
            continue;
        };

        // check if we should stop parsing
        if (ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_NEWLINE || 
            ctok.ttype == KS_TOK_SEMI || 
            ctok.ttype == KS_TOK_LBRACE || ctok.ttype == KS_TOK_RBRACE) goto parseexpr_end;

        if (ctok.ttype == KS_TOK_INT) {
            if (TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax");
    
            // just push this onto the value stack
            ks_ast new_int = ks_ast_new_int(tok_getint(ctok));
            new_int->tok_expr = new_int->tok = ctok;
            Spush(Out, new_int);

        } else if (ctok.ttype == KS_TOK_STR) {
            if (TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax");

            // just push this onto the value stack
            ks_str strval = tok_getstr(ctok);
            ks_ast new_str = ks_ast_new_stro(strval);
            new_str->tok_expr = new_str->tok = ctok;
            Spush(Out, new_str);

        } else if (ctok.ttype == KS_TOK_IDENT) {
            if (TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax");

            // first, check for key words
            if (TOK_EQ(self, ctok, "true")) {
                // treat it as a constant value
                ks_ast new_true = ks_ast_new_true();
                new_true->tok_expr = new_true->tok = ctok;
                Spush(Out, new_true);
            } else if (TOK_EQ(self, ctok, "false")) {
                // treat it as a constant value
                ks_ast new_false = ks_ast_new_false();
                new_false->tok_expr = new_false->tok = ctok;
                Spush(Out, new_false);
            } else if (TOK_EQ(self, ctok, "none")) {
                // treat it as a constant value
                ks_ast new_none = ks_ast_new_none();
                new_none->tok_expr = new_none->tok = ctok;
                Spush(Out, new_none);
            } else {
                // do a variable reference
                ks_ast new_var = ks_ast_new_varl(ctok.len, self->src->chr + ctok.offset);
                new_var->tok_expr = new_var->tok = ctok;
                Spush(Out, new_var);
            }

        } else if (ctok.ttype == KS_TOK_COMMA) {

            if (n_pars < 1 && n_bracks < 1) PEXPR_ERR(ctok, "Unexpected `,`, must have a comma within a group")

            while (Ops.len > 0 && Stop(Ops).type != SYT_FUNC && Stop(Ops).type != SYT_LPAR && Stop(Ops).type != SYT_SUBSCRIPT && Stop(Ops).type != SYT_LBRACK) {
                POP_OP();
            }

            // set it
            if (Ops.len > 0) {
                Stop(Ops).had_comma = true;
            }

        } else if (ctok.ttype == KS_TOK_LBRACK) {
            n_bracks++;

            if (TOKE_ISVAL(ltok.ttype))  {

                // we add a 'NULL', so we know where the subscript starts
                Spush(Out, NULL);
                // add a subscript operation, since it is like `val[`
                syop new_op = SYOP(SYT_SUBSCRIPT);
                new_op.tok = ctok;
                Spush(Ops, new_op);

            } else {
                // this is a list literal
                // output a spacer
                Spush(Out, NULL);

                // and just a token
                syop new_op = SYOP(SYT_LBRACK);
                new_op.tok = ctok;
                Spush(Ops, new_op);
            }

        } else if (ctok.ttype == KS_TOK_RBRACK) {
            n_bracks--;
            if (n_bracks < 0) PEXPR_ERR(ctok, "Invalid Syntax; extra ']', remove it")

            // a right bracket should clear the stack, to the nearest subscript, or list literal
            while (Ops.len > 0) {
                syop cur = Stop(Ops);
                POP_OP();
                if (cur.type == SYT_SUBSCRIPT || cur.type == SYT_LBRACK) break;
            }

        } else if (ctok.ttype == KS_TOK_LPAR) {
            n_pars++;
            
            if (TOKE_ISVAL(ltok.ttype))  {
                // start a function call
                // we add a 'NULL', so we know where the function call starts
                Spush(Out, NULL);

                // then the previous item parsed was a 'value' type, so this is a function call
                syop new_op = SYOP(SYT_FUNC);
                new_op.tok = ctok;
                Spush(Ops, new_op);

            } else {
                // either a normal expression or a tuple
                // output a spacer
                Spush(Out, NULL);

                // add an operator denoting this
                syop new_op = SYOP(SYT_LPAR);
                new_op.tok = ctok;
                Spush(Ops, new_op);
            }

        } else if (ctok.ttype == KS_TOK_RPAR) {
            n_pars--;
            if (n_pars < 0) PEXPR_ERR(ctok, "Invalid Syntax; extra ')', remove it")

            bool is_func = false;

            syop cur;

            // a right parenthesis should clear the stack, to the nearest function call
            // or group
            while (Ops.len > 0) {
                cur = Stop(Ops);
                POP_OP();
                if ((is_func = cur.type == SYT_FUNC) || cur.type == SYT_LPAR) break;
            }

            if (is_func) {
                // it has already been handled, we're done
            } else {
                // handle a group, which could be a tuple
                int osp = Out.len - 1;

                while (osp > 0 && Sget(Out, osp) != NULL) osp--;

                if (osp >= 0) {
                    // we found NULL
                    int n_items = Out.len - osp - 1;

                    // construct a value
                    ks_ast new_val = NULL;

                    if (n_items > 1 || n_items == 0 || cur.had_comma) {
                        // there's definitely a tuple here, so create it
                        new_val = ks_ast_new_tuple(n_items, &Sget(Out, osp+1));
                        new_val->tok = cur.tok;
                        // join the first and last
                        new_val->tok_expr = ks_tok_combo(new_val->tok, ctok);
                        
                    } else {
                        // else, just yield the value as a math operation
                        new_val = Sget(Out, osp+1);
                    }

                    // remove all values
                    Out.len -= n_items;

                    // take off the NULL
                    Spop(Out);

                    if (new_val != NULL) {
                        Spush(Out, new_val);
                    }
                }
            }

        } else if (TOKE_ISOP(ctok.ttype)) {

            // try and find the current operator, store it here
            syop cur_op = { .type = SYT_NONE };

            // if case for an operator
            #define KPE_OPCASE(_tok, _opstr, _opval) if (TOK_EQ(self, _tok, _opstr)) { cur_op = _opval; goto kpe_op_resolve; }

            // TODO: implement unary operators
            if (TOKE_ISOP(ltok.ttype) || ltok.ttype == KS_TOK_COMMA || ltok.ttype == KS_TOK_LPAR) PEXPR_ERR(ctok, "Invalid Syntax");

            if (TOKE_ISVAL(ltok.ttype)) {
                // since the last token was a value, this must either be a unary postfix or a binary infix
                KPE_OPCASE(ctok, "+", syb_add)
                KPE_OPCASE(ctok, "-", syb_sub)
                KPE_OPCASE(ctok, "*", syb_mul)
                KPE_OPCASE(ctok, "/", syb_div)
                KPE_OPCASE(ctok, "=", syb_assign)
            } else {
                PEXPR_ERR(ctok, "Invalid Syntax; Unexpected operator");
            }

            kpe_op_resolve: ;

            if (cur_op.type == SYT_NONE) {
                // it was not found
                PEXPR_ERR(ctok, "Unexpected operator");
            }

            // set the metadata
            cur_op.tok = ctok;

            // now, we have a valid operator, so clear the stack of anything with higher precedence than it

            while (Ops.len > 0) {
                bool do_op = false;
                // check if the precedence makes sense
                do_op = (Stop(Ops).prec >= cur_op.prec + ((cur_op.assoc == SYA_BOP_LEFT) ? 0 : 1));

                // make sure we're obeying PEMDAS and always putting parenthesis at the highest level
                do_op = do_op && (Stop(Ops).type != SYT_LPAR && Stop(Ops).type != SYT_FUNC);

                if (do_op) {
                    // process the operator
                    POP_OP();
                } else {
                    // stop
                    break;
                }
            }

            // always push it to the op stack
            Spush(Ops, cur_op);

        } else {

            PEXPR_ERR(ctok, "Unexpected token for expr");
        }


        // transfer the reference to the last token
        ltok = ctok;

        // increment counter
        self->tok_i++;
    }

    parseexpr_end: ;

    if (wasErr) {
        ks_free(Out.base);
        ks_free(Ops.base);
        
        return NULL;
    } else {
        while (Ops.len > 0) {
            POP_OP();
        }
        if (n_pars > 0) {
            int i, encountered = 0;
            ks_tok last_lpar;
            // try and find the parenthesis that caused the error
            for (i = start_tok_i; i < self->tok_i && encountered < n_pars; ++i) {
                if (self->toks[i].ttype == KS_TOK_LPAR) {
                    encountered++;
                    last_lpar = self->toks[i];
                }
            }

            if (encountered > 0) {
                // we found one to use for the error message
                PEXPR_ERR(last_lpar, "Invalid Syntax; have an extra '(', remove it");
            } else {
                // just use the last one
                PEXPR_ERR(ctok, "Invalid Syntax; have an extra '(', remove it");
            }
        }

        if (Out.len > 1) {
            // this means there are just multiple statements, seperated by commas:
            // x = 2, y = 3, so just return them as a block
            ks_ast ret = ks_ast_new_block(Out.len, Out.base);
            ks_free(Out.base);
            ks_free(Ops.base);
            return ret;
        } else if (Out.len == 1) {
            ks_ast ret = Sget(Out, 0);
            ks_free(Out.base);
            ks_free(Ops.base);
            return ret;
        } else {
            // empty expression
            PEXPR_ERR(ctok, "Invalid Syntax; empty expression");
        }
    }
}


// generic parser, which is the general one used
ks_ast ks_parse_all(ks_parser self) {

    ks_ast block = ks_ast_new_block_empty();
    bool wasErr = false;

    // raise a parsing error here
    #define PALL_ERR(...) { tok_err(__VA_ARGS__); wasErr = true; goto parseall_end; }

    // external error, quit without additional message
    #define PALL_ERREXT() { wasErr = true; goto parseall_end; }

    
    // skips the irrelevant tokens/details
    #define PALL_SKIPIRR() { \
        while (self->tok_i < self->n_toks && ((ctok = self->toks[self->tok_i]).ttype == KS_TOK_NEWLINE || ctok.ttype == KS_TOK_COMMENT || ctok.ttype == KS_TOK_SEMI)) { \
            self->tok_i++; \
        } \
        if (self->tok_i >= self->n_toks || ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_RBRACK) goto parseall_end; \
    }

    ks_tok ctok;

    while (true) {

        // skip irrevalnt
        PALL_SKIPIRR();

        if (ctok.ttype == KS_TOK_IDENT) {

            // check for keywords/directives
            if (TOK_EQ(self, ctok, "asm")) {
                // parse a bytecode/assembly block:
                // asm { BODY }
                self->tok_i++;

                PALL_SKIPIRR();

                // now, expect an opening brace '{'
                ctok = (ks_tok)self->toks[self->tok_i++];
                if (ctok.ttype != KS_TOK_LBRACE) PALL_ERR(ctok, "Expected '{' to start the assembly block");

                ks_ast asm_block = ks_parse_asm(self);
                if (asm_block == NULL) PALL_ERREXT();

                ks_list_push(block->v_block, (kso)asm_block);

                ctok = (ks_tok)self->toks[self->tok_i++];
                if (ctok.ttype != KS_TOK_RBRACE) PALL_ERR(ctok, "Expected '}' to end the assembly block");

            } else if (TOK_EQ(self, ctok, "ret")) {
                // parse a return statement:
                // ret EXPR 
                // or
                // ret

                // skip ret
                self->tok_i++;

                ks_ast expr = ks_parse_expr(self);
                if (expr == NULL) PALL_ERREXT();

                ks_list_push(block->v_block, (kso)ks_ast_new_ret(expr));


            } else if (TOK_EQ(self, ctok, "if")) {
                // parse an if block:
                // if (COND) { BODY } ?(elif (COND1) { BODY1 })* ?(else (CONDLAST) { BODYLAST })
                self->tok_i++;

                PALL_SKIPIRR();

                // now, expect an expression
                ks_ast cond = ks_parse_expr(self);
                if (cond == NULL) PALL_ERREXT();

                PALL_SKIPIRR();

                // recurse here, parse a whole block
                ks_ast body = ks_parse_all(self);
                if (body == NULL) PALL_ERREXT();

                ks_list_push(block->v_block, (kso)ks_ast_new_if(cond, body));
            } else if (TOK_EQ(self, ctok, "func")) {

                // parse a function definition
                // func NAME(ARGS...) { BODY }
                self->tok_i++;

                if ((ctok = self->toks[self->tok_i++]).ttype != KS_TOK_IDENT) PALL_ERR(ctok, "Expected a function name identifier here");

                ks_ast func_name = ks_ast_new_varl(ctok.len, self->src->chr + ctok.offset);

                if ((ctok = self->toks[self->tok_i++]).ttype != KS_TOK_LPAR) PALL_ERR(ctok, "Expected a '(' to begin the list of function parameters here");
                
                // TODO: parse argument names
                ks_list param_names = ks_list_new_empty();

                while ((ctok = self->toks[self->tok_i]).ttype != KS_TOK_RPAR) {
                    if (ctok.ttype != KS_TOK_IDENT) PALL_ERR(ctok, "Expected a parameter name identifier here");

                    // add it as a parameter
                    ks_list_push(param_names, (kso)ks_str_new(ctok.len, self->src->chr + ctok.offset));

                    ctok = self->toks[++self->tok_i];
                    if (ctok.ttype == KS_TOK_COMMA) self->tok_i++;

                }

                if ((ctok = self->toks[self->tok_i++]).ttype != KS_TOK_RPAR) PALL_ERR(ctok, "Expected a ')' to end the list of function parameters here");

                PALL_SKIPIRR();

                if ((ctok = self->toks[self->tok_i]).ttype != KS_TOK_LBRACE) PALL_ERR(ctok, "Expected a '{' to begin the function body here");

                // parse a body block from the function
                ks_ast body = ks_parse_all(self);
                if (body == NULL) PALL_ERREXT();

                ks_ast func_val = ks_ast_new_func(param_names, body);

                // add it to the current block
                ks_list_push(block->v_block, (kso)ks_ast_new_bop(KS_AST_BOP_ASSIGN, func_name, func_val));

            } else {
                ks_ast expr = ks_parse_expr(self);
                if (expr == NULL) PALL_ERREXT();
                ks_list_push(block->v_block, (kso)expr);
            }
        } else if (ctok.ttype == KS_TOK_RBRACE) {
            // some higher order function has called us and we should end now
            goto parseall_end;
        } else if (ctok.ttype == KS_TOK_LBRACE) {
            // parse another block,
            // { BODY }
            // skip '{'
            ks_tok s_tok = self->toks[self->tok_i++];

            // recursively call it, to get the inner block
            ks_ast body = ks_parse_all(self);
            if (body == NULL) PALL_ERREXT();

            // expect/skip '}'
            if (self->toks[self->tok_i++].ttype != KS_TOK_RBRACE) {
                PALL_ERR(s_tok, "Expected matching '}' to this");
            }

            // good to go
            ks_list_push(block->v_block, (kso)body);
            goto parseall_end;

        } else if (TOKE_ISVAL(ctok.ttype) || ctok.ttype == KS_TOK_LBRACK) {
            // just parse a normal expression
            ks_ast expr = ks_parse_expr(self);
            if (expr == NULL) PALL_ERREXT();
            ks_list_push(block->v_block, (kso)expr);
        } else {
            PALL_ERR(ctok, "Unexpected token");
        }
    }


    parseall_end: ;

    if (wasErr) {
        KSO_CHKREF(block);
        return NULL;
    } else {
        return block;
    }
}


