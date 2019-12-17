/* parse.c - implementation of parsing routines


*/

#include "kscript.h"
#include <ctype.h>

// whether or not a character is whitespace
static inline bool iswhite(char c) {
    return c == ' ' || c == '\t' || c == '\n';
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

        while (i > 0 && (c = kp->src._[i])) {
            if (c == '\n' || i == 0) {
                i++;
                break;
            }
            i--;
        }

        // line start i
        int lsi = i;

        while (c = kp->src._[i]) {
            if (c == '\n') break;
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

        /* unary */
        CASES("~", KS_TOK_U_TIL)

        /* operators */
        CASES("+", KS_TOK_O_ADD)
        CASES("-", KS_TOK_O_SUB)
        CASES("*", KS_TOK_O_MUL)
        CASES("/", KS_TOK_O_DIV)
        CASES("<", KS_TOK_O_LT)
        CASES(">", KS_TOK_O_GT)



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


    #define MATCHES(_tok, _str) (strncmp(_str, kp->src._ + _tok.state.i, _tok.len) == 0)
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
                rc = ks_parse_err(kp, kp->tokens[i], "Invalid Syntax");
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



