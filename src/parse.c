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
    ks_str start = ks_str_new("");
    KSO_INCREF(start);

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
            KSO_INCREF(new_str);
            KSO_DECREF(start);
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
                KSO_INCREF(new_str);
                KSO_DECREF(start);
                start = new_str;
            }

            // skip the escape code
            i++;
        } else {
            // something weird happen, just stop
            return start;
        }

    }

    // just take off our temporary reference
    start->refcnt--;

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
        STR_TOK(KS_TOK_DOT, ".")
        STR_TOK(KS_TOK_COMMA, ",")
        STR_TOK(KS_TOK_COLON, ":")
        STR_TOK(KS_TOK_SEMI, ";")

        // pairs
        STR_TOK(KS_TOK_LPAR, "(") STR_TOK(KS_TOK_RPAR, ")")
        STR_TOK(KS_TOK_LBRACK, "[") STR_TOK(KS_TOK_RBRACK, "]")
        STR_TOK(KS_TOK_LBRACE, "{") STR_TOK(KS_TOK_RBRACE, "}")

        // operators
        STR_TOK(KS_TOK_O_ADD, "+") STR_TOK(KS_TOK_O_SUB, "-")
        STR_TOK(KS_TOK_O_MOD, "%") STR_TOK(KS_TOK_O_POW, "**")
        STR_TOK(KS_TOK_O_MUL, "*") STR_TOK(KS_TOK_O_SUB, "/")

        // comp operators
        STR_TOK(KS_TOK_O_LE, "<=") STR_TOK(KS_TOK_O_LT, "<") 
        STR_TOK(KS_TOK_O_GE, ">=") STR_TOK(KS_TOK_O_GT, ">") 
        STR_TOK(KS_TOK_O_EQ, "==") STR_TOK(KS_TOK_O_NE, "!=")

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
    int start_i = i - 1, start_line = line, start_col = col;
    ADD_TOK(KS_TOK_EOF);

    // start at the beginning
    self->tok_i = 0;

}

ks_parser ks_parser_new_expr(const char* expr) {

    ks_parser self = (ks_parser)ks_malloc(sizeof(*self));
    *self = (struct ks_parser) {
        KSO_BASE_INIT(ks_T_parser, KSOF_NONE)
        .src_name = ks_str_new("-e"),
        .src = ks_str_new(expr),
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
        .src_name = ks_str_new(fname),
        .src = ks_str_new_l(contents, bytes),
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
        if (self->tok_i >= self->n_toks) goto parseasm_end;

        // get current token
        ks_tok ctok = (ks_tok)self->toks[self->tok_i];

        // check if we should skip
        if (ctok.ttype == KS_TOK_NEWLINE || ctok.ttype == KS_TOK_COMMENT) {
            self->tok_i++;
            continue;
        }
        
        // check if we should stop parsing
        if (ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_RBRACE) goto parseasm_end;

        if (tok_first.ttype == KS_TOK_NONE) tok_first = ctok;

        if (ctok.ttype == KS_TOK_IDENT) {
            // we will have an assembly command

            // skip that token, but keep ctok as the name of the command
            self->tok_i++;

            if (TOK_EQ(self, ctok, "load")) {
                a0 = self->toks[self->tok_i++];
                if (a0.ttype == KS_TOK_STR) {
                    // load a string literal name
                    tostr = tok_getstr(a0);
                    ksc_loado(code, (kso)tostr);
                    KSO_CHKREF(tostr);
                } else {
                    PASM_ERR(a0, "Arg #0 to 'load' instruction must be a string literal");
                }
            } else if (TOK_EQ(self, ctok, "const")) {
                a0 = self->toks[self->tok_i++];
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
                a0 = self->toks[self->tok_i++];
                if (a0.ttype == KS_TOK_INT) {
                    // call with an integer number of arguments
                    ksc_call(code, (int)tok_getint(a0));
                } else {
                    PASM_ERR(a0, "Arg #0 to 'call' instruction must be an integer");
                }
            } else if (TOK_EQ(self, ctok, "popu")) {
                ksc_popu(code);

            } else if (TOK_EQ(self, ctok, "ret_none")) {
                ksc_ret_none(code);
            } else if (TOK_EQ(self, ctok, "ret")) {
                ksc_ret(code);

            } else if (TOK_EQ(self, ctok, "bop")) {
                a0 = self->toks[self->tok_i++];

                // check for binary operators
                /**/ if (a0.ttype == KS_TOK_O_ADD) ksc_add(code);
                else {
                    PASM_ERR(a0, "Arg #0 must be a valid binary operator!");
                }

            } else {
                PASM_ERR(ctok, "Unknown assembly instruction '%*s'", ctok.len, self->src->chr + ctok.offset);
            }

            ks_tok ntok = self->toks[self->tok_i];
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

        // comparison operators, like <,>,==
        SYP_CMP,

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
    syb_mod = SYBOP(SYP_MULDIV, SYA_BOP_LEFT, KS_AST_BOP_MOD), syb_pow = SYBOP(SYP_POW, SYA_BOP_RIGHT, KS_AST_BOP_POW),
    syb_lt = SYBOP(SYP_CMP, SYA_BOP_LEFT, KS_AST_BOP_LT), syb_le = SYBOP(SYP_CMP, SYA_BOP_LEFT, KS_AST_BOP_LE), 
    syb_gt = SYBOP(SYP_CMP, SYA_BOP_LEFT, KS_AST_BOP_GT), syb_ge = SYBOP(SYP_CMP, SYA_BOP_LEFT, KS_AST_BOP_GE), 
    syb_eq = SYBOP(SYP_CMP, SYA_BOP_LEFT, KS_AST_BOP_EQ), syb_ne = SYBOP(SYP_CMP, SYA_BOP_LEFT, KS_AST_BOP_NE),

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
            ks_ast new_call = ks_ast_new_call(&Sget(Out, osp), n_args+1); \
            new_call->tok = top.tok; \
            new_call->tok_expr = ks_tok_combo(((ks_ast)new_call->v_list->items[0])->tok_expr, ctok); \
            Out.len -= n_args + 2; \
            Spush(Out, new_call);\
        } else if (top.type == SYT_SUBSCRIPT) { \
            /* basically: start at the top of the stack, scanning down for our NULL we added to the output 
            to signify the start of the object */ \
            int osp = Out.len - 1; \
            while (osp >= 0 && Sget(Out, osp) != NULL) osp--; \
            int n_args = Out.len - osp - 1; \
            Sget(Out, osp) = Sget(Out, osp - 1); /* effectively swaps the NULL and the actual function call */ \
            /* now, they are contiguous in memory */ \
            ks_ast new_subs = ks_ast_new_subscript(&Sget(Out, osp), n_args+1); \
            new_subs->tok = top.tok; \
            new_subs->tok_expr = ks_tok_combo(((ks_ast)new_subs->v_list->items[0])->tok_expr, ctok); \
            Out.len -= n_args + 2; \
            Spush(Out, new_subs);\
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
            ks_ast new_list = ks_ast_new_list(&Sget(Out, osp+1), n_args); \
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
    #define TOKE_ISVAL(_type) (_type == KS_TOK_INT || _type == KS_TOK_STR || _type == KS_TOK_IDENT || _type == KS_TOK_RPAR || _type == KS_TOK_RBRACK)

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

        // check if we should skip a comment
        if (ctok.ttype == KS_TOK_COMMENT) {
            self->tok_i++;
            continue;
        };

        // check if we should stop parsing due to being at the end
        if (ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_NEWLINE || 
            ctok.ttype == KS_TOK_SEMI || 
            ctok.ttype == KS_TOK_LBRACE || ctok.ttype == KS_TOK_RBRACE) goto parseexpr_end;

        if (ctok.ttype == KS_TOK_INT) {
            // can't have 2 value types in a row
            if (TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax");
    
            // just push this onto the value stack
            ks_ast new_int = ks_ast_new_int(tok_getint(ctok));
            new_int->tok_expr = new_int->tok = ctok;
            Spush(Out, new_int);

        } else if (ctok.ttype == KS_TOK_STR) {
            // can't have 2 value types in a row
            if (TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax");

            // just push this onto the value stack
            ks_str strval = tok_getstr(ctok);
            ks_ast new_str = ks_ast_new_str(strval);
            new_str->tok_expr = new_str->tok = ctok;
            Spush(Out, new_str);

        } else if (ctok.ttype == KS_TOK_IDENT) {
            // we need to skip a keyword, because it is not part of the expression
            if (TOK_EQ(self, ctok, "then")) goto parseexpr_end;

            // can't have 2 value types in a row
            if (TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax");

            // first, check for keywords
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
                ks_str var_s = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);
                ks_ast new_var = ks_ast_new_var(var_s);
                KSO_CHKREF(var_s);
                new_var->tok_expr = new_var->tok = ctok;
                Spush(Out, new_var);
            }
        } else if (ctok.ttype == KS_TOK_DOT) {
            // the last token must be a value type, because you need one to take an attribute of
            if (!TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax")

            // save the dot token reference
            ks_tok dot_tok = ctok;
            ctok = self->toks[++self->tok_i];

            // the only valid attribute is an identifier, i.e. `x.1` does not make sense
            if (ctok.ttype != KS_TOK_IDENT) PEXPR_ERR(ctok, "Attribute after a '.' must be a valid identifier")

            // take off the last
            ks_ast last = Spop(Out);
            // replace it with its attribute
            ks_str attr_name_s = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);
            ks_ast new_attr = ks_ast_new_attr(last, attr_name_s);
            KSO_CHKREF(attr_name_s);

            // set up the new tokens
            new_attr->tok = dot_tok;
            new_attr->tok_expr = ks_tok_combo(last->tok_expr, ctok);

            Spush(Out, new_attr);

        } else if (ctok.ttype == KS_TOK_COMMA) {
            // this is to prevent commas outside of operators outside of brackets
            if (n_pars < 1 && n_bracks < 1) PEXPR_ERR(ctok, "Unexpected `,`, must have a comma within a group")

            // just reduce the top of the stack until we get to a '(' or '['
            while (Ops.len > 0 && Stop(Ops).type != SYT_FUNC && Stop(Ops).type != SYT_LPAR && Stop(Ops).type != SYT_SUBSCRIPT && Stop(Ops).type != SYT_LBRACK) {
                POP_OP();
            }

            // set the flag for the operator on top, so it knew it had a comma in it
            if (Ops.len > 0) Stop(Ops).had_comma = true;

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

            if (!(TOKE_ISVAL(ltok.ttype) || ltok.ttype == KS_TOK_COMMA || ltok.ttype == KS_TOK_LPAR)) PEXPR_ERR(ctok, "Invalid Syntax; did not expect ')' here")

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
                        new_val = ks_ast_new_tuple(&Sget(Out, osp+1), n_items);
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
                KPE_OPCASE(ctok, "+",  syb_add)
                KPE_OPCASE(ctok, "-",  syb_sub)
                KPE_OPCASE(ctok, "*",  syb_mul)
                KPE_OPCASE(ctok, "/",  syb_div)
                KPE_OPCASE(ctok, "%",  syb_mod)
                KPE_OPCASE(ctok, "**", syb_pow)
                KPE_OPCASE(ctok, "<",  syb_lt)
                KPE_OPCASE(ctok, "<=", syb_le)
                KPE_OPCASE(ctok, ">",  syb_gt)
                KPE_OPCASE(ctok, ">=", syb_ge)
                KPE_OPCASE(ctok, "==", syb_eq)
                KPE_OPCASE(ctok, "!=", syb_ne)
                KPE_OPCASE(ctok, "=",  syb_assign)
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
            if (ctok.ttype == KS_TOK_EOF) {
                PEXPR_ERR(ctok, "Unexpected EOF");
            } else {
                PEXPR_ERR(ctok, "Unexpected token for expr");
            }
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
                PEXPR_ERR(last_lpar, "Invalid Syntax; have an extra '('");
            } else {
                // just use the last one
                PEXPR_ERR(ctok, "Invalid Syntax; have an extra '(', remove it");
            }
        }
        if (n_bracks > 0) {
            int i, encountered = 0;
            ks_tok last_lbrack;
            // try and find the parenthesis that caused the error
            for (i = start_tok_i; i < self->tok_i && encountered < n_bracks; ++i) {
                if (self->toks[i].ttype == KS_TOK_LBRACK) {
                    encountered++;
                    last_lbrack= self->toks[i];
                }
            }

            if (encountered > 0) {
                // we found one to use for the error message
                PEXPR_ERR(last_lbrack, "Invalid Syntax; have an extra '['");
            } else {
                // just use the last one
                PEXPR_ERR(ctok, "Invalid Syntax; have an extra '[', remove it");
            }
        }


        if (Out.len > 1) {
            // this means there are just multiple statements, seperated by commas:
            // x = 2, y = 3, so just return them as a block
            ks_ast ret = ks_ast_new_block(Out.base, Out.len);
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


// generic parser, parses single statements out, but can include blocks
ks_ast ks_parse_all(ks_parser self) {

    // raise a parsing error here
    #define PALL_ERR(...) { tok_err(__VA_ARGS__); return NULL; }

    // external error, quit without additional message
    #define PALL_ERREXT() { return NULL; }

    // skips the irrelevant tokens/details
    #define PALL_SKIPIRR() { \
        while (self->tok_i < self->n_toks && ((ctok = self->toks[self->tok_i]).ttype == KS_TOK_NEWLINE || ctok.ttype == KS_TOK_COMMENT || ctok.ttype == KS_TOK_SEMI)) { \
            self->tok_i++; \
        } \
    }

    // skips the REALLY irrelevant bits
    #define PALL_SKIPRIRR() { \
        while (self->tok_i < self->n_toks && ((ctok = self->toks[self->tok_i]).ttype == KS_TOK_COMMENT)) { \
            self->tok_i++; \
        } \
    }

    // current token
    ks_tok ctok;

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
            ctok = self->toks[self->tok_i++];
            if (ctok.ttype != KS_TOK_LBRACE) PALL_ERR(ctok, "Expected '{' to start the assembly block");

            ks_ast res = ks_parse_asm(self);
            if (res == NULL) PALL_ERREXT();

            ctok = self->toks[self->tok_i++];
            if (ctok.ttype != KS_TOK_RBRACE) PALL_ERR(ctok, "Expected '}' to end the assembly block");

            return res;

        } else if (TOK_EQ(self, ctok, "ret")) {
            // parse a return statement:
            // ret EXPR 
            // or
            // ret

            // skip ret
            self->tok_i++;


            // skip comments/whitespca
            PALL_SKIPRIRR();

            // if the next one is a semi colon, newline, etc, then we know the return should be a return none

            ctok = self->toks[self->tok_i];
            ks_ast expr = NULL;
            if (ctok.ttype == KS_TOK_NEWLINE || ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_SEMI || ctok.ttype == KS_TOK_RBRACE) {
                expr = ks_ast_new_none();
            } else {
                expr = ks_parse_expr(self);
            }

            // check for errors
            if (expr == NULL) PALL_ERREXT();

            return ks_ast_new_ret(expr);

        } else if (TOK_EQ(self, ctok, "if")) {
            // parse an if block:
            // if (COND) { BODY } ?(elif (COND1) { BODY1 })* ?(else (CONDLAST) { BODYLAST })
            self->tok_i++;

            PALL_SKIPIRR();

            // now, expect an expression
            ks_ast cond = ks_parse_expr(self);
            if (cond == NULL) PALL_ERREXT();

            PALL_SKIPIRR();

            // skip the optional 'then'
            ctok = self->toks[self->tok_i];
            if (TOK_EQ(self, ctok, "then")) self->tok_i++;

            // recurse here, parse a whole block
            ks_ast body = ks_parse_all(self);
            if (body == NULL) PALL_ERREXT();

            return ks_ast_new_if(cond, body);

        } else if (TOK_EQ(self, ctok, "while")) {
            // parse an while block:
            // while (COND) { BODY }
            self->tok_i++;

            PALL_SKIPIRR();

            // now, expect an expression
            ks_ast cond = ks_parse_expr(self);
            if (cond == NULL) PALL_ERREXT();

            PALL_SKIPIRR();

            // recurse here, parse a whole block
            ks_ast body = ks_parse_all(self);
            if (body == NULL) PALL_ERREXT();

            return ks_ast_new_while(cond, body);

        } else if (TOK_EQ(self, ctok, "func")) {

            // parse a function definition
            // func NAME(ARGS...) { BODY }
            self->tok_i++;

            if ((ctok = self->toks[self->tok_i++]).ttype != KS_TOK_IDENT) PALL_ERR(ctok, "Expected a function name identifier here");

            ks_str func_name_s = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);
            ks_ast func_name = ks_ast_new_var(func_name_s);
            KSO_CHKREF(func_name_s);

            if ((ctok = self->toks[self->tok_i++]).ttype != KS_TOK_LPAR) PALL_ERR(ctok, "Expected a '(' to begin the list of function parameters here");
            
            // TODO: parse argument names
            ks_list param_names = ks_list_new_empty();

            while ((ctok = self->toks[self->tok_i]).ttype != KS_TOK_RPAR) {
                if (ctok.ttype != KS_TOK_IDENT) PALL_ERR(ctok, "Expected a parameter name identifier here");

                // add it as a parameter
                ks_list_push(param_names, (kso)ks_str_new_l(self->src->chr + ctok.offset, ctok.len));

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
            return ks_ast_new_bop(KS_AST_BOP_ASSIGN, func_name, func_val);

        } else {
            //printf("HERE:%s\n", self->src->chr + self->toks[self->tok_i].offset);
            return ks_parse_expr(self);
        }

    } else if (ctok.ttype == KS_TOK_LBRACE) {
        // parse another block,
        // { BODY }
        // skip '{'
        ks_tok s_tok = self->toks[self->tok_i++];

        // create a new block
        ks_ast res = ks_ast_new_block_empty();

        PALL_SKIPIRR();

        while (self->toks[self->tok_i].ttype != KS_TOK_RBRACE) {
            ks_ast sub = ks_parse_all(self);
            if (sub == NULL) PALL_ERREXT();
            ks_list_push(res->v_list, (kso)sub);

            PALL_SKIPIRR();
        }

        // expect/skip '}'
        if (self->toks[self->tok_i++].ttype != KS_TOK_RBRACE) {
            PALL_ERR(s_tok, "Expected matching '}' to this");
        }

        return res;

    } else if (TOKE_ISVAL(ctok.ttype) || ctok.ttype == KS_TOK_LBRACK) {
        // just parse a normal expression
        return ks_parse_expr(self);
    } else {
        if (ctok.ttype == KS_TOK_EOF) {
            int i = self->tok_i;
            while (i >= 0) {
                ctok = self->toks[i];
                if (!(ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_NEWLINE)) {
                    break;
                }
                i--;
            }
            PALL_ERR(ctok, "Unexpected EOF after this");
        } else {
            PALL_ERR(ctok, "Unexpected token");
        }
    }

    return NULL;
}


ks_ast ks_parse_general(ks_parser self) {
    ks_ast blk = ks_ast_new_block_empty();

    ks_tok ctok;

    PALL_SKIPIRR();

    while (self->tok_i < self->n_toks && (self->toks[self->tok_i].ttype != KS_TOK_RBRACK && self->toks[self->tok_i].ttype != KS_TOK_EOF)) {
        ks_ast sub = ks_parse_all(self);
        if (sub == NULL) return NULL;
        ks_list_push(blk->v_list, (kso)sub);
        PALL_SKIPIRR();
    }

    return blk;
}

