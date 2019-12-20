/* parse.c - implementation of parsing routines


*/

#include "kscript.h"
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

// gets the current character
inline char ks_parse_get(ks_parse* kp) {
    return kp->src._[kp->state.i];
}

// adds a token to the internal array
int ks_parse_addtok(ks_parse* kp, ks_token tok) {
    int idx = kp->tokens_n++;
    kp->tokens = ks_realloc(kp->tokens, sizeof(ks_token) * kp->tokens_n);
    kp->tokens[idx] = tok;
}


// outputs a string literal, unescapeing, etc:
// example: ""hello\nworld"" goes to "hello
// world"
int ks_token_strlit(ks_parse* kp, ks_token tok, ks_str* to) {
    ks_str_copy(to, KS_STR_CONST(""));

    if (tok.type == KS_TOK_STR) {
        char* s = kp->src._ + tok.state.i;
        if (*s == '"') {
            s++;
        }

        char c;
        while ((c = *s) && c != '"') {
            s++;

            if (c == '\\') {
                c = *s++;
                if (c == 'n') {
                    ks_str_append_c(to, '\n');
                } else {
                    return ks_parse_err(kp, tok, "Invalid String literal escape code '\\%c'", c);
                }
            } else {
                ks_str_append_c(to, c);
            }
        }

        return 0;
    } else {
        return 1;
    }
}

// advances 'nchr' characters forward
void ks_parse_adv(ks_parse* kp, int nchr) {
    int i;
    for (i = 0; i < nchr; ++i) {
        char c = ks_parse_get(kp);

        if (c == '\0') {
            return;
        } else if (c == '\n') {
            kp->state.line++;
            kp->state.col = 0;
        } else {
            kp->state.col++;
        }

        kp->state.i++;
    }
}

// when there is a parse error, you can return this
//#define PARSE_ERR(...) (ks_str_fmt(&kp->err, __VA_ARGS__),1)

// set the error
int ks_parse_err(ks_parse* kp, ks_token tok, const char* fmt, ...) {

    // string format it
    va_list ap;
    va_start(ap, fmt);
    ks_str_vfmt(&kp->err, fmt, ap);
    va_end(ap);

    // now, try and rewind from the token
    if (tok.len >= 0) {
        int i = tok.state.i;
        int lineno = tok.state.line;
        char c;

        while (i > 0) {
            c = kp->src._[i];

            if (c == '\n') {
                i++;
                break;
            }
            i--;
        }

        // line start i
        int lsi = i;

        while (c = kp->src._[i]) {
            if (c == '\n' || c == '\0') break;
            i++;
        }

        // line length
        int ll = i - lsi;


        ks_str_fmt(&kp->err, "%s\n%.*s\n%.*s^%.*s", kp->err._, ll, kp->src._ + lsi, 
            tok.state.i - lsi, "                                                                                                                                                                                             ", 
            tok.len > 0 ? (tok.len - 1) : 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~...");

    } else {
        // not even a valid-half token
    }

    ks_str_fmt(&kp->err, "%s\n@ Line %d, Col %d, in '%s'", kp->err._, tok.state.line + 1, tok.state.col + 1, kp->src_name._);

    //ks_debug("PARSE_ERR: %s", kp->err._);

    // return the error code
    return 1;

}

// only call this once, internally
int ks_parse_tokenize(ks_parse* kp) {
    ks_str ident = KS_STR_EMPTY;

    char c;

    // generates a token from the current variables
    #define TOK(_type) ((ks_token){ .type = (_type), .state = (stok), .len = (kp->state.i - stok.i) })

    while ((c = ks_parse_get(kp)) && c != '\0') {

        // skip whitespace
        while ((c = ks_parse_get(kp)) && iswhite(c)) {
            if (!c) break;

            ks_parse_adv(kp, 1);
        }
        if (!c) break;

        // start of the token state
        struct ks_parse_state stok = kp->state;

        if (isdigit(c)) {
            // parse int (or float)

            // while it is a digit
            do {
                ks_parse_adv(kp, 1);
            } while (isdigit(ks_parse_get(kp)));

            ks_parse_addtok(kp, TOK(KS_TOK_INT));
        } else if (isidents(c)) {
            // parse an identifier
            do {

                ks_parse_adv(kp, 1);
            } while (isidentm(ks_parse_get(kp)));

            ks_parse_addtok(kp, TOK(KS_TOK_IDENT));
        } else if (c == '"') {
            // parse a string literal
            // skip first '"'
            ks_parse_adv(kp, 1);


            while ((c = ks_parse_get(kp)) && c != '"') {
                if (c == '\\') {
                    // this means there will also be an escape code here
                    ks_parse_adv(kp, 1);
                }

                // skip over the next character
                ks_parse_adv(kp, 1);
            }

            // expect ending quotes as well
            if (ks_parse_get(kp) != '"') {
                return ks_parse_err(kp, TOK(KS_TOK_STR), "Expected terminating '\"' after str literal");
            }

            ks_parse_adv(kp, 1);

            ks_parse_addtok(kp, TOK(KS_TOK_STR));
            
        } else if (c == '#') {
            // coment, ignore until end of line
            ks_parse_adv(kp, 1);

            while ((c = ks_parse_get(kp)) && c != '\n') {
                ks_parse_adv(kp, 1);
            }

            ks_parse_addtok(kp, TOK(KS_TOK_COMMENT));
            
            ks_parse_adv(kp, 1);
        }

    // case for a literal string
    #define CASES(_str, _tok) else if (strncmp((_str), kp->src._+kp->state.i, strlen(_str)) == 0) { \
        ks_parse_adv(kp, strlen(_str)); \
        ks_parse_addtok(kp, TOK(_tok)); \
    }

        /* 1-1s */
        CASES(":", KS_TOK_COLON)
        CASES(";", KS_TOK_SEMI)
        CASES(".", KS_TOK_DOT)
        CASES("\n", KS_TOK_NEWLINE)

        CASES("(", KS_TOK_LPAREN)
        CASES(")", KS_TOK_RPAREN)

        CASES("{", KS_TOK_LBRACE)
        CASES("}", KS_TOK_RBRACE)

        CASES(",", KS_TOK_COMMA)

        /* unary */
        CASES("~", KS_TOK_U_TIL)

        /* operators */
        CASES("+", KS_TOK_O_ADD)
        CASES("-", KS_TOK_O_SUB)
        CASES("*", KS_TOK_O_MUL)
        CASES("/", KS_TOK_O_DIV)
        CASES("==", KS_TOK_O_EQ)
        CASES("<", KS_TOK_O_LT)
        CASES(">", KS_TOK_O_GT)

        CASES("=", KS_TOK_O_ASSIGN)


        else {

            return ks_parse_err(kp, TOK(KS_TOK_STR), "Invalid syntax (unexpected '%c')", c);
        }

    }

    return 0;
}

int ks_parse_setsrc(ks_parse* kp, ks_str src_name, ks_str src) {
    ks_str_copy(&kp->src_name, src_name);
    ks_str_copy(&kp->src, src);
    ks_str_copy(&kp->err, KS_STR_EMPTY);
    kp->state = KS_PARSE_STATE_BEGINNING;

    // now, tokenize it
    return ks_parse_tokenize(kp);
}

void ks_parse_free(ks_parse* kp) {
    ks_str_free(&kp->src);
    ks_str_free(&kp->src_name);

    *kp = KS_PARSE_EMPTY;
}



/* specific output parsers */


// parses out a byte code program, appending to `to`.
// if there was an error, return nonzero, and set `kp->err`
int ks_parse_bc(ks_parse* kp, ks_prog* to) {
    // arg 0 (for commands)
    ks_token a0;

    // return code
    int rc = 0;

    // hold links, for example, for jumping ahead ina file
    int links_n = 0;
    struct jlink {
        // the name of the label it is referencing
        ks_token lbl_name;

        // this is the point from which it should be based.
        // so it should be the first byte after the current is decoded,
        // bc_n + sizeof(struct inst)
        int bc_from;

        // the start of the location where the signed 32 bit integer
        //   of the relative offset should be written
        // for example, the second byte of a `jmpf` instruction, so it will be updated at the end
        int bc_i32;

    }* links = NULL;

    // helper string
    ks_str st = KS_STR_EMPTY;


    #define MATCHES(_tok, _str) (strlen(_str) == _tok.len && strncmp(_str, kp->src._ + _tok.state.i, _tok.len) == 0)
    int i;
    for (i = 0; i < kp->tokens_n; ++i) {

        switch (kp->tokens[i].type) {
            case KS_TOK_COMMENT:
            case KS_TOK_NEWLINE:
                break;

            case KS_TOK_IDENT:
                // this should be a command
                if (MATCHES(kp->tokens[i], "const")) {
                    // parse constant out
                    a0 = kp->tokens[++i];
                    if (a0.type == KS_TOK_INT) {
                        // const [int]
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_int(to, (ks_int)atoll(st._));
                    } else if (a0.type == KS_TOK_IDENT && (MATCHES(a0, "true") || MATCHES(a0, "false"))) {
                        ksb_bool(to, (ks_bool)MATCHES(a0, "true"));
                    } else if (a0.type == KS_TOK_STR) {
                        ks_token_strlit(kp, a0, &st);
                        ksb_str(to, st);                        
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `const` instruction, it should be `int`, `bool`, `none`, `float`, or `str`");
                        goto kb_pend;
                    }
                    
                } else if (MATCHES(kp->tokens[i], "op")) {
                    // handle an operators
                    a0 = kp->tokens[++i];

                    if (MATCHES(a0, "+")) ksb_add(to);
                    else if (MATCHES(a0, "-")) ksb_sub(to);
                    else if (MATCHES(a0, "*")) ksb_mul(to);
                    else if (MATCHES(a0, "/")) ksb_div(to);
                    else if (MATCHES(a0, "<")) ksb_lt(to);
                    else {
                        rc = ks_parse_err(kp, a0, "Unknown operator");
                        goto kb_pend;
                    }

                } else if (MATCHES(kp->tokens[i], "load")) {
                    // should take string
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_STR) {
                        if (ks_token_strlit(kp, a0, &st) != 0) return 1;
                        ksb_load(to, st);
                        //printf("%.*s\n", a0.len, kp->src._ + a0.state.i);
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `load` instruction, it should be `str`");
                        goto kb_pend;
                    }
                } else if (MATCHES(kp->tokens[i], "store")) {
                    // should take string
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_STR) {
                        ks_token_strlit(kp, a0, &st);;
                        ksb_store(to, st);
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `store` instruction, it should be `str`");
                        goto kb_pend;
                    }

                } else if (MATCHES(kp->tokens[i], "call")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_call(to, (int16_t)atol(st._));
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `new_list` instruction, it should be `int`");
                        goto kb_pend;
                    }
                } else if (MATCHES(kp->tokens[i], "get")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_get(to, (int16_t)atol(st._));
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `get` instruction, it should be `int`");
                        goto kb_pend;
                    }
                } else if (MATCHES(kp->tokens[i], "set")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_set(to, (int16_t)atol(st._));
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `set` instruction, it should be `int`");
                        goto kb_pend;
                    }
                } else if (MATCHES(kp->tokens[i], "new_list")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_new_list(to, (int16_t)atol(st._));
                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `new_list` instruction, it should be `int`");
                        goto kb_pend;
                    }

                /* simple instructions */

                } else if (MATCHES(kp->tokens[i], "retnone")) {
                    ksb_retnone(to);

                } else if (MATCHES(kp->tokens[i], "discard")) {
                    ksb_discard(to);


                /* jumping/conditionals */

                } else if (MATCHES(kp->tokens[i], "jmpt")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        // interpreted as relative amt
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_jmpt(to, (int)atol(st._));
                    } else if (a0.type == KS_TOK_U_TIL && kp->tokens[i+1].type == KS_TOK_IDENT) {
                        // read the a0 as the label name
                        a0 = kp->tokens[++i];

                        // add a delayed link, so it will be linked at the end
                        links = ks_realloc(links, sizeof(*links) * ++links_n);
                        links[links_n - 1] = (struct jlink) { 
                            .lbl_name = a0, 
                            .bc_from = to->bc_n + sizeof(struct ks_bc_jmp), 
                            .bc_i32 = to->bc_n + offsetof(struct ks_bc_jmp, relamt) 
                        };

                        // this '0' will be filled in later by the link
                        ksb_jmpt(to, 0);

                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `jmpt` instruction, it should be `int`, or a `~label`");
                        goto kb_pend;
                    }
                } else if (MATCHES(kp->tokens[i], "jmpf")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        // interpreted as relative amt
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_jmpf(to, (int)atol(st._));
                    } else if (a0.type == KS_TOK_U_TIL && kp->tokens[i+1].type == KS_TOK_IDENT) {
                        // read the a0 as the label name
                        a0 = kp->tokens[++i];

                        // add a delayed link, so it will be linked at the end
                        links = ks_realloc(links, sizeof(*links) * ++links_n);
                        links[links_n - 1] = (struct jlink) { 
                            .lbl_name = a0, 
                            .bc_from = to->bc_n + sizeof(struct ks_bc_jmp), 
                            .bc_i32 = to->bc_n + offsetof(struct ks_bc_jmp, relamt) 
                        };

                        // this '0' will be filled in later by the link
                        ksb_jmpf(to, 0);

                    } else {
                        rc = ks_parse_err(kp, a0, "Invalid argument for `jmpf` instruction, it should be `int`, or a `~label`");
                        goto kb_pend;
                    }

                } else if (i < kp->tokens_n - 1 && kp->tokens[i+1].type == KS_TOK_COLON) {
                    // we are defining a label
                    //ks_debug("label: %.*s", kp->tokens[i].len, kp->src._ + kp->tokens[i].state.i);
                    ks_prog_lbl_add(to, KS_STR_VIEW(kp->src._ + kp->tokens[i].state.i, kp->tokens[i].len), to->bc_n);
                    ++i;


                } else {
                    rc = ks_parse_err(kp, kp->tokens[i], "Unknown instruction '%.*s'", kp->tokens[i].len, kp->src._ + kp->tokens[i].state.i);
                    goto kb_pend;
                }


                break;

            default:
                rc = ks_parse_err(kp, kp->tokens[i], "Invalid Syntax (#1)");
                goto kb_pend;
        }
    }


    // now, actually link things

    for (i = 0; i < links_n; ++i) {

        struct jlink jl = links[i];

        // compute where the label is
        int32_t lbl_off = ks_prog_lbl_i(to, KS_STR_VIEW(kp->src._ + jl.lbl_name.state.i, jl.lbl_name.len));
        if (lbl_off == -1) {
            rc = ks_parse_err(kp, jl.lbl_name, "Unknown label: `%.*s`", jl.lbl_name.len, kp->src._ + jl.lbl_name.state.i);
            goto kb_pend; 
        }

        // now, we're making it a relative jump, so calculate the difference
        int32_t relamt = lbl_off - jl.bc_from;
        // and write it
        *(int32_t*)(to->bc + jl.bc_i32) = relamt;
    }


    // exception while parsing, or just the end
    kb_pend:
    ks_free(links);
    ks_str_free(&st);
    return rc;
}



// shunting yard operator
typedef struct syop {
    const char* str;

    // what kind of association (+,-,... are left), (^ is right),
    //   and unary operators are unary
    enum syassoc {
        SYA_NONE = 0,
        SYA_LEFT,
        SYA_RIGHT,

        // unary associations
        SYA_UNARY_PRE,
        SYA_UNARY_POST,
        SYA_N
    } assoc;

    // the precedence of the operator, which should be sorted from lowest to highest.
    enum syprec {
        SYP_NONE = 0,

        SYP_UNARY, // unary has greatest precedence, other than groups (like '()')
        
        SYP_ASSIGN,
        SYP_CMP, // comparison operators, like ==, <, etc
        SYP_ADDSUB,
        SYP_MULDIV,

        SYP_N

    } prec;

    // the type of shunting yard operator
    enum sytype { 
        SYT_NONE = 0,
        // actual operators
        SYT_BOP,
        SYT_UOP,
        
        // psuedo-operators
        SYT_FUNC,
        SYT_LPAREN,

        SYT_N
    } type;

    
    int ast_type;

} syop;

/* _kp_ops[] = {
    { "==", ASSOC_LEFT, PREC_EQ, _OP_BOP, KAST_BOP_EQ, NULL },
    { "=",  ASSOC_LEFT, PREC_ASSIGN, _OP_BOP, KAST_BOP_ASSIGN, NULL },
    { "+",  ASSOC_LEFT, PREC_AS, _OP_BOP, KAST_BOP_ADD, NULL },
    { "-",  ASSOC_LEFT, PREC_AS, _OP_BOP, KAST_BOP_SUB, NULL },
    { "*",  ASSOC_LEFT, PREC_MD, _OP_BOP, KAST_BOP_MUL, NULL },
    { "/",  ASSOC_LEFT, PREC_MD, _OP_BOP, KAST_BOP_DIV, NULL },
};*/

// binary operator left associative
#define MAKE_BOPL(_str, _prec, _ast_type) ((syop){ _str, SYA_LEFT, _prec, SYT_BOP, .ast_type = _ast_type })

// built in operators

// binary operators
syop
    sybop_add = MAKE_BOPL("+", SYP_ADDSUB, KS_AST_BOP_ADD), sybop_sub = MAKE_BOPL("-", SYP_ADDSUB, KS_AST_BOP_SUB),
    sybop_mul = MAKE_BOPL("*", SYP_MULDIV, KS_AST_BOP_MUL), sybop_div = MAKE_BOPL("/", SYP_MULDIV, KS_AST_BOP_DIV),
    sybop_lt = MAKE_BOPL("<", SYP_CMP, KS_AST_BOP_LT), 
    sybop_gt = MAKE_BOPL(">", SYP_CMP, KS_AST_BOP_GT), 
    sybop_eq = MAKE_BOPL("==", SYP_CMP, KS_AST_BOP_EQ),
    sybop_assign = MAKE_BOPL("=", SYP_ASSIGN, KS_AST_BOP_ASSIGN)
    
    
    ;


#define KS_TOK_ISOP(_toktype) ((_toktype) == KS_TOK_O_ADD || (_toktype) == KS_TOK_O_SUB || (_toktype) == KS_TOK_O_MUL || (_toktype) == KS_TOK_O_DIV || (_toktype) == KS_TOK_O_LT || (_toktype) == KS_TOK_O_GT || (_toktype) == KS_TOK_O_EQ || (_toktype) == KS_TOK_O_ASSIGN)


ks_ast ks_parse_expr(ks_parse* kp, int* kpi) {

    int rc = 0;

    // stack macros
    #define Spush(_stk, _val) { if (++_stk.p >= _stk.p_max - 2) { _stk.base = ks_realloc(_stk.base, (_stk.p + 1) * sizeof(_stk.base[0])); } _stk.base[_stk.p] = _val; }
    #define Spop(_stk) _stk.base[_stk.p--]
    #define Spop_unused(_stk) _stk.p--
    #define Stop(_stk) _stk.base[_stk.p]
    #define Sget(_stk, _idx) _stk.base[_idx]

    static struct {
        int p, p_max;
        syop* base;
    } op_stk = { -1, 0, NULL };

    static struct {
        int p, p_max;
        ks_ast* base;
    } out_stk = { -1, 0, NULL };


    ks_token ctok = KS_TOK_EMPTY, ltok = KS_TOK_EMPTY;

    syop last_op = { .type = SYT_NONE };

    ks_str str = KS_STR_EMPTY;

    ks_ast new_ast = NULL;

    bool is_func = true;

    int n_lparens = 0, n_rparens = 0;

    int took_off = 0;

    // process an operator
    #define PROCESS_OPERATOR { \
        syop top = Spop(op_stk); \
        is_func = false; \
        if (top.type == SYT_BOP) { \
            if (out_stk.p < 2 - 1) { \
                rc = ks_parse_err(kp, ctok, "SyntaxError: Malformed expression"); \
                goto kc_pend; \
            } \
            ks_ast R = Spop(out_stk); \
            ks_ast L = Spop(out_stk); \
            ks_ast node = ks_ast_new_bop(top.ast_type, L, R); \
            Spush(out_stk, node); \
        } else if (top.type == SYT_FUNC) { \
            /* scan down until we hit the NULL, which tells us when the function call started */  \
            int outsp = out_stk.p, nargs = 0; \
            while (outsp >= 0 && Sget(out_stk, outsp) != NULL) { \
                outsp--; \
                nargs++; \
            } \
            /* now, out_stk_ptr should point to NULL */  \
            ks_ast node = ks_ast_new_call(Sget(out_stk, outsp - 1), nargs, &Sget(out_stk, outsp + 1)); \
            out_stk.p = outsp; \
            if (Spop(out_stk) != NULL) { \
                rc = ks_parse_err(kp, ctok, "SyntaxError: Malformed function call"); \
                goto kc_pend; \
            } \
            out_stk.p--; \
            /* push out the function call */  \
            Spush(out_stk, node); \
            is_func = true; \
            break; \
        } else { \
            rc = ks_parse_err(kp, ctok, "InternalError: Did not pop off operator correctly (got %d)", top.type); \
            goto kc_pend; \
        } \
    }


    for (; *kpi < kp->tokens_n; ++*kpi) {
        ctok = kp->tokens[*kpi];
        
        if (ctok.type == KS_TOK_NEWLINE) {
            ctok = ltok;
            break;
        } else if (ctok.type == KS_TOK_INT) {
            if (ltok.type == KS_TOK_INT || ltok.type == KS_TOK_STR || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                rc = ks_parse_err(kp, ctok, "SyntaxError: This was not expected (#1)");
                goto kc_pend;
            }

            ks_str_copy(&str, KS_STR_VIEW(kp->src._+ctok.state.i, ctok.len));
            new_ast = ks_ast_new_const_int(atoll(str._));
            Spush(out_stk, new_ast);
        } else if (ctok.type == KS_TOK_STR) {
            if (ltok.type == KS_TOK_INT || ltok.type == KS_TOK_STR || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                rc = ks_parse_err(kp, ctok, "SyntaxError: This was not expected (#2)");
                goto kc_pend;
            }

            ks_token_strlit(kp, ctok, &str);
            new_ast = ks_ast_new_const_str(str);
            Spush(out_stk, new_ast);

        } else if (ctok.type == KS_TOK_IDENT) {
            if (ltok.type == KS_TOK_INT || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                rc = ks_parse_err(kp, ctok, "SyntaxError: This was not expected (#3)");
                goto kc_pend;
            }

            ks_str_copy(&str, KS_STR_VIEW(kp->src._+ctok.state.i, ctok.len));
            new_ast = ks_ast_new_var(str);
            Spush(out_stk, new_ast);

        } else if (ctok.type == KS_TOK_COMMA) {
            // Comma, which should be used in a function call, condense the output
            if (ltok.type == KS_TOK_COMMA) {
                rc = ks_parse_err(kp, ctok, "SyntaxError: Can't have multiple commas like this");
                goto kc_pend;
            }
            if (ltok.type == KS_TOK_LPAREN) {
                rc = ks_parse_err(kp, ltok, "SyntaxError: Expected some value after this");
                goto kc_pend;
            }

            if (KS_TOK_ISOP(ltok.type)) {
                rc = ks_parse_err(kp, ctok, "SyntaxError: Invalid Syntax");
                goto kc_pend;
            }
            // reduce top of stack

            while (op_stk.p >= 0 && Stop(op_stk).type != SYT_FUNC) {
                PROCESS_OPERATOR
            }

        } else if (KS_TOK_ISOP(ctok.type)) {

            // current operator
            syop cur_op = { .type = SYT_NONE };

            #define KPE_OC(_in, _opstr, _opval) if (strlen(_opstr) == _in.len && strncmp(kp->src._ + _in.state.i, _opstr, _in.len) == 0) { cur_op = _opval; }


            if (ltok.type == KS_TOK_NONE || ltok.type == KS_TOK_COMMA || ltok.type == KS_TOK_LPAREN || (KS_TOK_ISOP(ltok.type) && last_op.type == SYT_BOP)) {
                // unary prefix operator

                //KPE_OC(ctok, "++", syuop_preinc) else 
                {
                    rc = ks_parse_err(kp, ctok, "Unhandled operator '%.*s'", ctok.len, kp->src._ + ctok.state.i);
                    goto kc_pend;
                }

                Spush(op_stk, cur_op);


            } else if (ltok.type == KS_TOK_RPAREN || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_INT || ltok.type == KS_TOK_STR || (KS_TOK_ISOP(ltok.type) && last_op.type == SYT_UOP)) {
                // this can be either a binary infix operator or a unary postfix operator, handle unary postfix first

                /*KPE_OC(ctok, "++", syuop_postinc) else 
                {
                    // do nothing, as it should now be a binary operator
                }*/

                if (cur_op.type != SYT_NONE) {
                    // if not none, then it was a post-fix unary operator, so pop stuff off
                    //PROCESS_OPERATOR
                    //Spush(op_stk, cur_op);
                    Spush(op_stk, cur_op);
                    PROCESS_OPERATOR

                    //kast_p node = kast_new();
                    ////kast_init_uop(node, cur_op.uop_type, Spop(out_stk));
                    //Spush(out_stk, node);

                } else {
                    // other than that, it should be a normal binary operator
                    //kparse_error(kp, &ctok, "SyntaxError: This operator was not expected");

                    KPE_OC(ctok, "=" , sybop_assign) else
                    KPE_OC(ctok, "+" , sybop_add) else
                    KPE_OC(ctok, "-" , sybop_sub) else
                    KPE_OC(ctok, "*" , sybop_mul) else
                    KPE_OC(ctok, "/" , sybop_div) else
                    KPE_OC(ctok, "<" , sybop_lt) else
                    KPE_OC(ctok, ">" , sybop_gt) else
                    KPE_OC(ctok, "==" , sybop_eq) else
                    {
                        rc = ks_parse_err(kp, ctok, "Unhandled operator (internal error)");
                        goto kc_pend;
                    }

                    // process all greater precedence operators:
                    while (op_stk.p >= 0) {
                        bool do_op = Stop(op_stk).type == SYT_FUNC;
                        do_op = do_op || Stop(op_stk).prec >= cur_op.prec + ((cur_op.assoc == SYA_LEFT) ? 0 : 1);
                        do_op = do_op && Stop(op_stk).type != SYT_LPAREN && Stop(op_stk).type != SYT_FUNC;
                        if (do_op) {
                            PROCESS_OPERATOR
                        } else {
                            break;
                        }
                    }

                    Spush(op_stk, cur_op);
                }
            } else {
                rc = ks_parse_err(kp, ctok, "Invalid Syntax (#2)");
                goto kc_pend;

            }


        } else if (ctok.type == KS_TOK_LPAREN) {
            n_lparens++;

            if (ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                // do a function call
                // op_stk = ... func
                // out_stk = ... NULL

                // this is like a 'start group', so the parser knows where the function started
                Spush(op_stk, (syop) { .type = SYT_FUNC });
                Spush(out_stk, NULL);
            } else {
                // just do a normal group start
                Spush(op_stk, (syop) { .type = SYT_LPAREN });
            }

        } else if (ctok.type == KS_TOK_RPAREN) {
            n_rparens++;

            if (ltok.type == KS_TOK_COMMA || (KS_TOK_ISOP(ltok.type) && last_op.type == SYT_UOP && last_op.assoc == SYA_UNARY_PRE)) {
                rc = ks_parse_err(kp, ltok, "SyntaxError: Expected some value after this");
                goto kc_pend;
            }

            took_off = 0;
            while (op_stk.p >= 0 && Stop(op_stk).type != SYT_LPAREN) {
                PROCESS_OPERATOR
                took_off++;
            }

            if (is_func) {
                // if we parsed off a function, we already handled it

            } else {

                if (ltok.type == KS_TOK_LPAREN) {
                    rc = ks_parse_err(kp, ctok, "SyntaxError: Empty group here");
                    goto kc_pend;
                }
            
                // if we parsed off a normal group, expect it to end like this
                if (Stop(op_stk).type == SYT_LPAREN) {
                    // pop it off, continue
                    Spop_unused(op_stk);
                } else {
                    // when encountering another parentheses
                    //rc = ks_parse_err(kp, ctok, "SyntaxError: Extra ')'");
                    //goto kc_pend;
                    break;
                }
            }

        } else if (ctok.type == KS_TOK_LBRACE || ctok.type == KS_TOK_RBRACE) {
            // this means the expression has stopped
            //ctok = ltok;
            break;

        } else {
            rc = ks_parse_err(kp, ctok, "SyntaxError: Unexpected token");
            goto kc_pend;
        }

        ltok = ctok;
    }

    if (n_lparens > n_rparens) {
        rc = ks_parse_err(kp, ltok, "SyntaxError: Mismatched parentheses, expected another ')' after this");
        goto kc_pend;
    } else if (n_lparens < n_rparens) {
        rc = ks_parse_err(kp, ltok, "SyntaxError: Mismatched parentheses, have an extra ')'");
        goto kc_pend;
    }

    // while operators on the stack, should only be binary or unary, because
    //   all functions/things should have already been popped off
    while (op_stk.p >= 0) {
        PROCESS_OPERATOR
    }

    //kast_pprint(Sget(out_stk, 0));

    if (out_stk.p != 0) {
        rc = ks_parse_err(kp, ctok, "SyntaxError: Malformed expression (ended with out:%d, ops:%d)", out_stk.p + 1, op_stk.p + 1);
        goto kc_pend;
    }

    /*if (ks_ast_codegen(out_stk.base[0], to) != 0) {
        rc = ks_parse_err(kp, ctok, "Internal Error in ks_ast_codegen()");
        goto kc_pend;
    }*/

    kc_pend:
    ks_str_free(&str);
    out_stk.p = -1;
    op_stk.p = -1;
    return rc == 0 ? out_stk.base[0] : NULL;
}

ks_ast ks_parse_block(ks_parse* kp, int* kpi) {

    ks_token ctok = kp->tokens[*kpi];

    // skip newlines
    while (ctok.type == KS_TOK_NEWLINE) {
        ctok = kp->tokens[++*kpi];
    }


    if (ctok.type == KS_TOK_LBRACE) {
        // parse list
        // skip '{'
        ++*kpi;

        ks_ast body = ks_ast_new_block(0, NULL);

        do {
            ctok = kp->tokens[*kpi];

            if (ctok.type == KS_TOK_RBRACE) {
                break;
            }

            if (ctok.type == KS_TOK_NEWLINE) {
                ++*kpi;
                continue;
            }

            ks_ast body_sub = ks_parse_block(kp, kpi);
            if (body_sub == NULL) return NULL;
            // recursively parse it
            ks_ast_block_add(body, body_sub);

        } while (true);

        // expect '}'
        if (ctok.type != KS_TOK_RBRACE) {
            ks_parse_err(kp, ctok, "Expected '}'");
            return NULL;
        }
        ++*kpi;

        return body;

        
    } else if (ctok.type == KS_TOK_IDENT && (MATCHES(ctok, "if"))) {
        // parse if (COND) { BODY }
        // skip `if`
        ++*kpi;

        //ctok = kp->tokens[*kpi];
        //printf(":%s\n", kp->src._ + ctok.state.i);
        ks_ast cond = ks_parse_expr(kp, kpi);
        if (cond == NULL) return NULL;

        ks_ast body = ks_parse_block(kp, kpi);
        if (body == NULL) return NULL;
        
        return ks_ast_new_if(cond, body);

    } else if (ctok.type == KS_TOK_IDENT && (MATCHES(ctok, "while"))) {
        // parse while (COND) { BODY }
        // skip `while`
        ++*kpi;

        //ctok = kp->tokens[*kpi];
        ks_ast cond = ks_parse_expr(kp, kpi);
        if (cond == NULL) return NULL;

        ks_ast body = ks_parse_block(kp, kpi);
        if (body == NULL) return NULL;
        
        return ks_ast_new_while(cond, body);


    } else if (ctok.type == KS_TOK_RBRACE) {
        // this just means its the end
        return NULL;
    } else if (ctok.type == KS_TOK_IDENT) {
        // assums its just a normal expression, add it
        return ks_parse_expr(kp, kpi);
    } else {
        ks_parse_err(kp, ctok, "Invalid Syntax");
        return NULL;
    }

    return NULL;
}

ks_ast ks_parse_code(ks_parse* kp) {

    ks_ast body = ks_ast_new_block(0, NULL);

    int i, rc;
    for (i = 0; i < kp->tokens_n; ++i) {
        if (kp->tokens[i].type == KS_TOK_NEWLINE) {
            continue;
        }
        rc = 0;

        ks_token ctok = kp->tokens[i];
        // by default, just treat it as a block expression
        ks_ast expr = ks_parse_block(kp, &i);
        if (expr == NULL) return NULL;
        ks_ast_block_add(body, expr);
    }

    return body;
    
}


