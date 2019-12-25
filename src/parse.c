/* parse.c - implementation of parsing routines


includes:

  * bytecode/assembly parsing
  * normal infix notation program parsing


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
static inline char ks_parse_get(ks_parse* kp) {
    return kp->src._[kp->state.i];
}

// appends a token to ks_parse
static void ks_parse_addtok(ks_parse* kp, ks_token tok) {
    int idx = kp->tokens_n++;
    kp->tokens = ks_realloc(kp->tokens, sizeof(ks_token) * kp->tokens_n);
    kp->tokens[idx] = tok;
}

// takes a token, and outputs it as a string literal to `to`.
// Example: "Hello\nWorld" -> "Hello
// World"
static void ks_token_strlit(ks_parse* kp, ks_token tok, ks_str* to) {
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
                    ks_parse_err(kp, tok, "Invalid String literal escape code '\\%c'", c);
                }
            } else {
                ks_str_append_c(to, c);
            }
        }

    } else {
        // error
    }
}

// advances 'nchr' characters forward
static void ks_parse_adv(ks_parse* kp, int nchr) {
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

// set an error related to parsing, given a token
void ks_parse_err(ks_parse* kp, ks_token tok, const char* fmt, ...) {

    // the string to print to
    ks_str errstr = KS_STR_EMPTY;

    // string format it with what is given
    va_list ap;
    va_start(ap, fmt);
    ks_str_vcfmt(&errstr, fmt, ap);
    va_end(ap);

    // now, try and print additional information out if the token was valid
    if (tok.len >= 0) {
        int i = tok.state.i;
        int lineno = tok.state.line;
        char c;


        // rewind to the start of the line
        while (i >= 0) {
            c = kp->src._[i];

            if (c == '\n') {
                i++;
                break;
            }
            i--;
        }

        if (i < 0) i++;
        if (kp->src._[i] == '\n') i++;

        // line start i
        int lsi = i;

        while (c = kp->src._[i]) {
            if (c == '\n' || c == '\0') break;
            i++;
        }

        // line length
        int ll = i - lsi;

        // format and append a little pointer to the error
        ks_str_cfmt(&errstr, "%s\n%.*s\n%.*s^%.*s", errstr._, ll, kp->src._ + lsi, 
            tok.state.i - lsi, "                                                                                                                                                                                             ", 
            tok.len > 0 ? (tok.len - 1) : 0, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~..."
        );
        
        // add line/column information
        ks_str_cfmt(&errstr, "%s\n@ Line %d, Col %d, in '%s'", errstr._, tok.state.line + 1, tok.state.col + 1, kp->src_name._);

    } else {
        // not a valid token, so leave it off
    }

    // add the error globally
    ks_err_add_str(errstr);

    // remove the local copy we made of the string
    ks_str_free(&errstr);
}

// only call this once when the source is set, internally
static void ks_parse_tokenize(ks_parse* kp) {

    // current character in the input stream
    char c;

    // generates a token from the current state, length, etc
    #define TOK(_type) ((ks_token){ .type = (_type), .state = (stok), .len = (kp->state.i - stok.i) })

    while (c = ks_parse_get(kp)) {

        // skip whitespace/etc
        while ((c = ks_parse_get(kp)) && iswhite(c)) {
            if (!c) break;

            ks_parse_adv(kp, 1);
        }
        // make sure not EOF
        if (!c) break;

        // start of the token state
        struct ks_parse_state stok = kp->state;

        if (isdigit(c)) {
            // parse int (or float)
            // TODO: Add float support

            // keep checking digits
            do {
                ks_parse_adv(kp, 1);
            } while ((c = ks_parse_get(kp)) && isdigit(c));

            ks_parse_addtok(kp, TOK(KS_TOK_INT));
        } else if (isidents(c)) {
            // parse identifier

            // parse while its a valid identifier
            do {
                ks_parse_adv(kp, 1);
            } while ((c = ks_parse_get(kp)) && isidentm(c));

            ks_parse_addtok(kp, TOK(KS_TOK_IDENT));
        } else if (c == '"') {
            // parse a string literal
            // TODO: Support for """ literals

            // skip first '"'
            ks_parse_adv(kp, 1);

            while ((c = ks_parse_get(kp)) && c != '"') {
                if (c == '\\') {
                    // this means there will also be an escape code here,
                    //   so skip it
                    // TODO: Validate escape codes here
                    ks_parse_adv(kp, 1);
                }

                // skip over the next character
                ks_parse_adv(kp, 1);
            }

            // expect it to end with quotes
            if (ks_parse_get(kp) != '"') {
                return ks_parse_err(kp, TOK(KS_TOK_STR), "Expected terminating '\"' for string literal");
            }

            // skip the ending `"`
            ks_parse_adv(kp, 1);

            ks_parse_addtok(kp, TOK(KS_TOK_STR));
            
        } else if (c == '#') {
            // parse a comment token
            // TODO: Also support parsing multiline comments

            // keep parsing until a newline
            do {
                ks_parse_adv(kp, 1);
            } while ((c = ks_parse_get(kp)) && c != '\n');

            // since we are parsing a newline, add the token first, then skip the newline
            ks_parse_addtok(kp, TOK(KS_TOK_COMMENT));
            
            // skip the newline
            ks_parse_adv(kp, 1);
        }

    // the `else if` case for a token that is just represents a keyword/operator/string, or anything
    //   that is always the same
    #define CASE_TOK_LIT(_str, _type) else if ((strncmp((_str), kp->src._+kp->state.i, sizeof(_str) - 1) == 0)) { \
        ks_parse_adv(kp, sizeof(_str) - 1); \
        ks_parse_addtok(kp, TOK(_type)); \
    }

        /* misc grammars */

        CASE_TOK_LIT(":", KS_TOK_COLON)
        CASE_TOK_LIT(";", KS_TOK_SEMI)
        CASE_TOK_LIT(".", KS_TOK_DOT)
        CASE_TOK_LIT(",", KS_TOK_COMMA)

        CASE_TOK_LIT("(", KS_TOK_LPAREN)
        CASE_TOK_LIT(")", KS_TOK_RPAREN)

        CASE_TOK_LIT("{", KS_TOK_LBRACE)
        CASE_TOK_LIT("}", KS_TOK_RBRACE)

        CASE_TOK_LIT("[", KS_TOK_LBRACK)
        CASE_TOK_LIT("]", KS_TOK_RBRACK)

        /* somewhat special symbols */

        CASE_TOK_LIT("\n", KS_TOK_NEWLINE)

        /** operators **/

        /* unary operators */
        CASE_TOK_LIT("~", KS_TOK_U_TIL)

        /* binary/normal operators */
        CASE_TOK_LIT("+", KS_TOK_O_ADD)
        CASE_TOK_LIT("-", KS_TOK_O_SUB)
        CASE_TOK_LIT("*", KS_TOK_O_MUL)
        CASE_TOK_LIT("/", KS_TOK_O_DIV)
        CASE_TOK_LIT("%", KS_TOK_O_MOD)
        CASE_TOK_LIT("^", KS_TOK_O_POW)
        CASE_TOK_LIT("==", KS_TOK_O_EQ)
        CASE_TOK_LIT("<", KS_TOK_O_LT)
        CASE_TOK_LIT(">", KS_TOK_O_GT)

        /* special case operators */
        CASE_TOK_LIT("=", KS_TOK_O_ASSIGN)
        CASE_TOK_LIT(":=", KS_TOK_O_DEFINE)

        else {
            // there was either an unsupported character, or it didn't match the pattern
            return ks_parse_err(kp, TOK(KS_TOK_IDENT), "Invalid Syntax (unexpected '%c')", c);
        }
    }
}

// set the source code, and a name
void ks_parse_setsrc(ks_parse* kp, ks_str src_name, ks_str src) {
    ks_str_copy(&kp->src_name, src_name);
    ks_str_copy(&kp->src, src);
    ks_str_copy(&kp->err, KS_STR_EMPTY);
    kp->state = KS_PARSE_STATE_BEGINNING;

    // now, tokenize it
    ks_parse_tokenize(kp);

}

// free resources of the parser
void ks_parse_free(ks_parse* kp) {
    ks_str_free(&kp->src);
    ks_str_free(&kp->src_name);
    ks_free(&kp->tokens);

    *kp = KS_PARSE_EMPTY;
}


/* SPECIFIC LANGUAGE PARSERS */

void ks_parse_ksasm(ks_parse* kp, kso_code to) {
    // arguments for assembly directives
    // example:
    // cmd a0 a1 a2 ...
    ks_token a0, a1, a2;

    // number of labels defined
    int lbls_n = 0;

    // a structure describing a named label
    struct ksalbl {
        // the token that defined the label
        ks_token tok;

        // the string (as a view string, so should not be freed) of the label name
        ks_str lbl_name;

        // the pointer in the bytecode where the label points
        int bc_p;

    }* lbls = NULL;

    // number of links that need to be performed after parsing
    int links_n = 0;

    // a structure describing a link that references a label name,
    //   which will ultimately be resolved after the label has been added
    struct ksalink {
        // the token that defined the link
        ks_token tok;

        // the string (as a view-string, so should not be freed) of the label
        //   which the link references
        ks_str lbl_name;

        // index in to->bc where the jump will be occuring from.
        // most of the time this is the current bc + sizeof(instruction)
        int from_p;

        // index in to->bc where the relative amount of the jump needs to be written
        int relamt_p;

    }* links = NULL;

    // helper string
    ks_str st = KS_STR_EMPTY;

    // return whether or not a token is equal to a literal
    #define TOK_EQ(_tok, _str) ((sizeof(_str) - 1) == _tok.len && strncmp(_str, kp->src._ + _tok.state.i, (sizeof(_str) - 1)) == 0)


    for (; kp->tok_i < kp->tokens_n; ++kp->tok_i) {

        switch (kp->tokens[kp->tok_i].type) {
            // these don't matter, skip them
            case KS_TOK_COMMENT:
            case KS_TOK_SEMI:
            case KS_TOK_NEWLINE:
                break;

            case KS_TOK_RBRACE:
                // this means that we are out of scope, and a parent has called us, so just return
                goto _kspas_end;
                break;

            case KS_TOK_IDENT:
                // parse off a command, or a label:
                if (TOK_EQ(kp->tokens[kp->tok_i], "const")) {
                    a0 = kp->tokens[++kp->tok_i];
                    // now, have `const $a0`, and deduce which kind of constant it is
                    if (a0.type == KS_TOK_INT) {
                        // const [int], so add an int constant instruction
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksc_const_int(to, (ks_int)atoll(st._));
                    } else if (a0.type == KS_TOK_IDENT && TOK_EQ(a0, "true")) {
                        // constr true, add boolean
                        ksc_const_true(to);
                    } else if (a0.type == KS_TOK_IDENT && TOK_EQ(a0, "false")) {
                        // constr false, add boolean
                        ksc_const_false(to);
                    } else if (a0.type == KS_TOK_STR) {
                        // generate the string literal, and add as a string constant
                        ks_token_strlit(kp, a0, &st);
                        ksc_const_str(to, st);
                    } else if (a0.type == KS_TOK_LPAREN) {

                        // now, parse a function
                        // should be like:
                        // const (arg0, arg1, ...) > {
                        //   BODY
                        // }
                        // skip the '('
                        ++kp->tok_i;
                        kso_list param_list = kso_list_new_empty();
                        while (kp->tokens[kp->tok_i].type != KS_TOK_RPAREN) {
                            // parse name,
                            if (kp->tokens[kp->tok_i].type != KS_TOK_IDENT) {
                                ks_parse_err(kp, kp->tokens[kp->tok_i], "Invalid Syntax, expected an identifier for the name of a function parameter");
                                goto _kspas_end;
                            }
                            ks_list_push(&param_list->v_list, (kso)kso_str_new(KS_STR_VIEW(kp->src._ + kp->tokens[kp->tok_i].state.i, kp->tokens[kp->tok_i].len)));
                            kp->tok_i++;
                            // now, assume a comma or RPAREN
                            if (kp->tokens[kp->tok_i].type == KS_TOK_COMMA) {
                                kp->tok_i++;
                            } else if (kp->tokens[kp->tok_i].type == KS_TOK_RPAREN) {
                                // do nothing
                            } else {
                                ks_parse_err(kp, kp->tokens[kp->tok_i], "Invalid Syntax, expected either a ',' to continue parameter list, or a ')' to end the parameter list");
                                goto _kspas_end;
                            }
                        }

                        // expect/skip ')'
                        if (kp->tokens[kp->tok_i].type != KS_TOK_RPAREN) {
                            ks_parse_err(kp, kp->tokens[kp->tok_i], "Invalid Syntax, expected a ')' to end the parameter list");
                            goto _kspas_end;
                        }
                        ++kp->tok_i;


                        // expect/skip '{'
                        if (kp->tokens[kp->tok_i].type != KS_TOK_LBRACE) {
                            ks_parse_err(kp, kp->tokens[kp->tok_i], "Invalid Syntax, expected a '{' to start the function body");
                            goto _kspas_end;
                        }
                        ++kp->tok_i;


                        // create another code object
                        kso_code new_func_code = kso_code_new_empty(to->v_const);
                        // create another function
                        kso_kfunc new_func = kso_kfunc_new(param_list, new_func_code);
                        // parse into the new function
                        ks_parse_ksasm(kp, new_func_code);

                        // now, add the instruction
                        //ksc_func_lit(to, new_func);
                        ksc_const(to, (kso)new_func);

                        // expect/skip '}'
                        if (kp->tokens[kp->tok_i].type != KS_TOK_RBRACE) {
                            ks_parse_err(kp, kp->tokens[kp->tok_i], "Invalid Syntax, expected a '}' to end the function body");
                            goto _kspas_end;
                        }
                        ++kp->tok_i;
                        
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `const` instruction, it should be a `int`, `bool`, `none`, `float`, or `str`");
                        goto _kspas_end;
                    }
                    
                } else if (TOK_EQ(kp->tokens[kp->tok_i], "bop")) {
                    // handle a binary operator

                    // get the specific operator
                    a0 = kp->tokens[++kp->tok_i];

                    /*if (TOK_EQ(a0, "+"))      ksc_add(to);
                    else if (TOK_EQ(a0, "-")) ksc_sub(to);
                    else if (TOK_EQ(a0, "*")) ksc_mul(to);
                    else if (TOK_EQ(a0, "/")) ksc_div(to);
                    else if (TOK_EQ(a0, "<")) ksc_lt(to);
                    else*/ {
                        ks_parse_err(kp, a0, "Unknown operator");
                        goto _kspas_end;
                    }

                } else if (TOK_EQ(kp->tokens[kp->tok_i], "load")) {
                    // load variable
                    // load $a0

                    // should take a string
                    a0 = kp->tokens[++kp->tok_i];

                    if (a0.type == KS_TOK_STR) {
                        // valid
                        ks_token_strlit(kp, a0, &st);
                        ksc_load(to, st);
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `load` instruction, it should be a `str` (try adding '\"' around it)");
                        goto _kspas_end;
                    }
                } else if (TOK_EQ(kp->tokens[kp->tok_i], "store")) {
                    // store top of stack into variable name
                    // store $a0

                    // should take string
                    a0 = kp->tokens[++kp->tok_i];

                    if (a0.type == KS_TOK_STR) {
                        ks_token_strlit(kp, a0, &st);;
                        ksc_store(to, st);
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `store` instruction, it should be a `str` (try adding '\"' around it)");
                        goto _kspas_end;
                    }

                } else if (TOK_EQ(kp->tokens[kp->tok_i], "call")) {
                    // call [n_args]

                    a0 = kp->tokens[++kp->tok_i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksc_call(to, (int)atoll(st._));
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `call` instruction, it should be a `int`");
                        goto _kspas_end;
                    }
                /*
                } else if (TOK_EQ(kp->tokens[i], "get")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_get(to, (int16_t)atol(st._));
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `get` instruction, it should be a `int`");
                        goto _kspas_end;
                    }
                } else if (TOK_EQ(kp->tokens[i], "set")) {
                    a0 = kp->tokens[++i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksb_set(to, (int16_t)atol(st._));
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `set` instruction, it should be `int`");
                        goto _kspas_end;
                    }
                */
                } else if (TOK_EQ(kp->tokens[kp->tok_i], "create_list")) {
                    // create_list [num]
                    a0 = kp->tokens[++kp->tok_i];

                    if (a0.type == KS_TOK_INT) {
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        //ksc_create_list(to, (int16_t)atol(st._));
                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `create_list` instruction, it should be `int`");
                        goto _kspas_end;
                    }

                /* simple instructions */

                } else if (TOK_EQ(kp->tokens[kp->tok_i], "retnone")) {
                    ksc_retnone(to);

                } else if (TOK_EQ(kp->tokens[kp->tok_i], "discard")) {
                    ksc_discard(to);


                /* jumping/conditionals */

                } else if (TOK_EQ(kp->tokens[kp->tok_i], "jmpt")) {
                    a0 = kp->tokens[++kp->tok_i];

                    if (a0.type == KS_TOK_INT) {
                        // interpreted as relative amt, literally
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksc_jmpt(to, (int)atol(st._));
                    } else if (a0.type == KS_TOK_U_TIL && kp->tokens[kp->tok_i+1].type == KS_TOK_IDENT) {
                        // read the a0 as the label name
                        a0 = kp->tokens[++kp->tok_i];

                        // add a delayed link, so it will be linked at the end
                        links = ks_realloc(links, sizeof(*links) * ++links_n);
                        links[links_n - 1] = (struct ksalink) { 
                            .tok = a0,
                            .lbl_name = KS_STR_VIEW(kp->src._ + a0.state.i, a0.len),
                            .from_p = to->bc_n + sizeof(struct ks_bc_jmp),
                            .relamt_p = to->bc_n + offsetof(struct ks_bc_jmp, relamt)
                        };

                        // this '0' will be filled in later by the link
                        ksc_jmpt(to, 0);

                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `jmpt` instruction, it should be a `int`, or a `~label`");
                        goto _kspas_end;
                    }
                } else if (TOK_EQ(kp->tokens[kp->tok_i], "jmpf")) {
                    a0 = kp->tokens[++kp->tok_i];

                    if (a0.type == KS_TOK_INT) {
                        // interpreted as relative amt
                        ks_str_copy_cp(&st, kp->src._ + a0.state.i, a0.len);
                        ksc_jmpf(to, (int)atol(st._));
                    } else if (a0.type == KS_TOK_U_TIL && kp->tokens[kp->tok_i+1].type == KS_TOK_IDENT) {
                        // read the a0 as the label name
                        a0 = kp->tokens[++kp->tok_i];

                      // add a delayed link, so it will be linked at the end
                        links = ks_realloc(links, sizeof(*links) * ++links_n);
                        links[links_n - 1] = (struct ksalink) { 
                            .tok = a0,
                            .lbl_name = KS_STR_VIEW(kp->src._ + a0.state.i, a0.len),
                            .from_p = to->bc_n + sizeof(struct ks_bc_jmp),
                            .relamt_p = to->bc_n + offsetof(struct ks_bc_jmp, relamt)
                        };

                        // this '0' will be filled in later by the link
                        ksc_jmpf(to, 0);

                    } else {
                        ks_parse_err(kp, a0, "Invalid argument for `jmpf` instruction, it should be a `int`, or a `~label`");
                        goto _kspas_end;
                    }

                } else if (kp->tok_i < kp->tokens_n - 1 && kp->tokens[kp->tok_i+1].type == KS_TOK_COLON) {
                    // a0 is the label name to define
                    a0 = kp->tokens[kp->tok_i];

                    //ks_info("label: %.*s", a0.len, kp->src._ + a0.state.i);

                    // we are defining a label, so add it to the list
                    lbls = ks_realloc(lbls, sizeof(*lbls) * ++lbls_n);
                    lbls[lbls_n - 1] = (struct ksalbl) { 
                        .tok = a0,
                        .lbl_name = KS_STR_VIEW(kp->src._ + a0.state.i, a0.len),
                        .bc_p = to->bc_n
                    };

                    ++kp->tok_i;

                } else {
                    ks_parse_err(kp, kp->tokens[kp->tok_i], "Unknown instruction '%.*s'", kp->tokens[kp->tok_i].len, kp->src._ + kp->tokens[kp->tok_i].state.i);
                    goto _kspas_end;
                }

                if (kp->tok_i+1 < kp->tokens_n) {
                    ks_token next_tok = kp->tokens[kp->tok_i+1];
                    // check for extra arguments
                    if (!(next_tok.type == KS_TOK_NEWLINE || next_tok.type == KS_TOK_SEMI || next_tok.type == KS_TOK_COMMENT)) {
                        ks_parse_err(kp, next_tok, "Extra argument '%.*s'", next_tok.len, kp->src._ + next_tok.state.i);
                        goto _kspas_end;
                    }
                }

                break;

            default:
                ks_parse_err(kp, kp->tokens[kp->tok_i], "Invalid Syntax: Unexpected token type (expected an identifier)");
                goto _kspas_end;
        }
    }

    // now, run back and link the program
    int i;
    for (i = 0; i < links_n; ++i) {
        // get the current link
        struct ksalink klink = links[i];

        // locate the current label
        struct ksalbl klbl = (struct ksalbl){ .bc_p = -1 };

        // search through
        int j;
        for (j = 0; j < lbls_n; ++j) {
            if (ks_str_eq(klink.lbl_name, lbls[j].lbl_name)) {
                klbl = lbls[j];
                break;
            }
        }

        if (klbl.bc_p < 0) {
            ks_parse_err(kp, klink.tok, "Unknown label: `%.*s`", klink.lbl_name.len, klink.lbl_name._);
            goto _kspas_end; 
        }

        // now, compute the relative amount off
        int32_t relamt = klbl.bc_p - klink.from_p;

        // and finish the link
        *(int32_t*)(to->bc + klink.relamt_p) = relamt;
    }


    // go here when ready to clean up, or in case of error
    _kspas_end: ;
    ks_free(links);
    ks_free(lbls);
    ks_str_free(&st);
}


/* infix parsing */

// shunting yard operator
typedef struct syop {

    // string representing the representation of the operator
    const char* str;

    // what kind of association (+,-,... are left), (^ is right), etc,
    //   and unary operators are either pre or post
    // ++i -> pre
    // i++ -> post
    enum syassoc {
        SYA_NONE = 0,

        SYA_LEFT,
        SYA_RIGHT,

        // unary associations
        SYA_UNARY_PRE,
        SYA_UNARY_POST,

        SYA__END
    } assoc;

    // the precedence of the operator, which should be sorted from lowest to highest.
    // low precedence means its the last to evaluate,
    // and high precedence means very close arguments are the one used:
    // low precdence: =, like:
    // a = 3 + 4 * 5, because +,* are evaluated first (they have higher precedence)
    // but * has highest in that example
    enum syprec {
        SYP_NONE = 0,

        // unary has the highest precedence, unconditionallity
        SYP_UNARY,
        
        // assignment, i.e. A=B, because it should be the largest in scope
        SYP_ASSIGN,

        // define `:=` should be slightly lower than assignment
        SYP_DEFINE,

        // comparison operators, i.e. ==, <=, <, >, .etc
        SYP_CMP,

        // +,-
        SYP_ADDSUB,
        // *,/ , but also % (modulo)
        SYP_MULDIV,
        // ^, exponentiaion
        SYP_POW,

        SYP_N

    } prec;

    // the type of shunting yard operator
    enum sytype { 
        SYT_NONE = 0,

        // binary operator, i.e. infixed between two expressions
        SYT_BOP,

        // unary operator, i.e. takes an argument. Can be post or pre
        SYT_UOP,
        
        // psuedo-operators, i.e. aren't operators, but are similar
        SYT_FUNC,
        SYT_LPAREN,

        SYT_N
    } type;

    // what type to construct an AST via?
    int ast_type;

} syop;

// binary operator left associative
#define MAKE_BOPL(_str, _prec, _ast_type) ((syop){ _str, SYA_LEFT, _prec, SYT_BOP, .ast_type = _ast_type })

// built in operators
// binary operators
syop
    sybop_add = MAKE_BOPL("+", SYP_ADDSUB, KS_AST_BOP_ADD), sybop_sub = MAKE_BOPL("-", SYP_ADDSUB, KS_AST_BOP_SUB),
    sybop_mul = MAKE_BOPL("*", SYP_MULDIV, KS_AST_BOP_MUL), sybop_div = MAKE_BOPL("/", SYP_MULDIV, KS_AST_BOP_DIV),
    sybop_mod = MAKE_BOPL("%", SYP_MULDIV, KS_AST_BOP_MUL), sybop_pow = MAKE_BOPL("^", SYP_POW, KS_AST_BOP_POW),
    sybop_lt = MAKE_BOPL("<", SYP_CMP, KS_AST_BOP_LT), 
    sybop_gt = MAKE_BOPL(">", SYP_CMP, KS_AST_BOP_GT), 
    sybop_eq = MAKE_BOPL("==", SYP_CMP, KS_AST_BOP_EQ),
    sybop_assign = MAKE_BOPL("=", SYP_ASSIGN, KS_AST_BOP_ASSIGN),
    sybop_define = MAKE_BOPL(":=", SYP_DEFINE, KS_AST_BOP_DEFINE)
    
;

// true if the token type is an operator type
#define KS_TOK_ISOP(_toktype) ((_toktype) == KS_TOK_O_ADD || (_toktype) == KS_TOK_O_SUB || (_toktype) == KS_TOK_O_MUL || (_toktype) == KS_TOK_O_DIV || (_toktype) == KS_TOK_O_MOD || (_toktype) == KS_TOK_O_POW || (_toktype) == KS_TOK_O_LT || (_toktype) == KS_TOK_O_GT || (_toktype) == KS_TOK_O_EQ || (_toktype) == KS_TOK_O_ASSIGN)

// parse an infix expression, return the AST
ks_ast ks_parse_expr(ks_parse* kp) {

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


    // current and last token
    ks_token ctok = KS_TOK_EMPTY, ltok = KS_TOK_EMPTY;

    // the last operator parsed
    syop last_op = { .type = SYT_NONE };

    // a helper string
    ks_str str = KS_STR_EMPTY;

    // for constructing ASTs
    ks_ast new_ast = NULL;

    // for keeping track of parenthesis, ensuring its correct
    int n_lparens = 0, n_rparens = 0;


    bool is_func = true;

    int took_off = 0;

    // processes through a single operator,
    //   basically taking the operator off the stack, and then altering the output stack,
    //   scooping up values and forming the ASTs
    #define PROCESS_OPERATOR { \
        syop top = Spop(op_stk); \
        is_func = false; \
        if (top.type == SYT_BOP) { \
            if (out_stk.p+1 < 2) { \
                ks_parse_err(kp, ctok, "Invalid Syntax; incorrect usage of operator"); \
                goto _kspco_end; \
            } \
            ks_ast R = Spop(out_stk); \
            ks_ast L = Spop(out_stk); \
            ks_ast node = ks_ast_new_bop(top.ast_type, L, R); \
            Spush(out_stk, node); \
        } else if (top.type == SYT_FUNC) { \
            /* scan down until we hit the NULL, which tells us when the function call started */ \
            int outsp = out_stk.p, nargs = 0; \
            while (outsp >= 0 && Sget(out_stk, outsp) != NULL) { \
                outsp--; \
                nargs++; \
            } \
            /* now, out_stk_ptr should point to NULL */ \
            ks_ast node = ks_ast_new_call(Sget(out_stk, outsp - 1), nargs, &Sget(out_stk, outsp + 1)); \
            out_stk.p = outsp; \
            if (Spop(out_stk) != NULL) { \
                ks_parse_err(kp, ctok, "Invalid Syntax; incorrect usage of function call"); \
                goto _kspco_end; \
            } \
            out_stk.p--; \
            /* push out the function call */ \
            Spush(out_stk, node); \
            is_func = true; \
            break; \
        } else if (top.type == SYT_LPAREN) { \
            continue; \
        } else { \
            ks_parse_err(kp, ctok, "InternalError; Did not pop off operator correctly (got %d)", top.type); \
            goto _kspco_end; \
        } \
    }


    #define ESC_LOOP { ++kp->tok_i; break; }

    for (; kp->tok_i < kp->tokens_n; ++kp->tok_i) {
        ctok = kp->tokens[kp->tok_i];

        if (ctok.type == KS_TOK_NEWLINE) {
            ctok = ltok;
            break;
        } else if (ctok.type == KS_TOK_INT) {
            if (ltok.type == KS_TOK_INT || ltok.type == KS_TOK_STR || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                ks_parse_err(kp, ctok, "Invalid Syntax; didn't expect 2 values in a row like this");
                goto _kspco_end;
            }

            ks_str_copy(&str, KS_STR_VIEW(kp->src._+ctok.state.i, ctok.len));
            new_ast = ks_ast_new_const_int(atoll(str._));
            Spush(out_stk, new_ast);
        } else if (ctok.type == KS_TOK_STR) {
            if (ltok.type == KS_TOK_INT || ltok.type == KS_TOK_STR || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                ks_parse_err(kp, ctok, "Invalid Syntax; didn't expect 2 values in a row like this");
                goto _kspco_end;
            }

            ks_token_strlit(kp, ctok, &str);
            new_ast = ks_ast_new_const_str(str);
            Spush(out_stk, new_ast);

        } else if (ctok.type == KS_TOK_IDENT) {


            if (ltok.type == KS_TOK_INT || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                ks_parse_err(kp, ctok, "Invalid Syntax; didn't expect 2 values in a row like this");
                goto _kspco_end;
            }
            /* first, check for keywords */
            if (TOK_EQ(ctok, "true")) {
                // boolean constant
                new_ast = ks_ast_new_const_true();
                Spush(out_stk, new_ast);
            } else if (TOK_EQ(ctok, "false")) {
                // boolean constant
                new_ast = ks_ast_new_const_false();
                Spush(out_stk, new_ast);
            } else {
                // otherwise, default to variable reference
                ks_str_copy(&str, KS_STR_VIEW(kp->src._+ctok.state.i, ctok.len));
                new_ast = ks_ast_new_var(str);
                Spush(out_stk, new_ast);
            }

        } else if (ctok.type == KS_TOK_COMMA) {
            // Comma, which should be used in a function call, condense the output
            if (ltok.type == KS_TOK_COMMA) {
                ks_parse_err(kp, ctok, "Invalid Syntax; can't have multiple commas like this");
                
                goto _kspco_end;
            }
            if (ltok.type == KS_TOK_LPAREN) {
                ks_parse_err(kp, ltok, "Invalid Syntax; expected some value after this");
                goto _kspco_end;
            }

            if (KS_TOK_ISOP(ltok.type)) {
                ks_parse_err(kp, ctok, "Invalid Syntax; didn't expect a `,` after an operator");
                goto _kspco_end;
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
                    ks_parse_err(kp, ctok, "InternalError; Unhandled operator");
                    goto _kspco_end;
                }

                Spush(op_stk, cur_op);


            } else if (ltok.type == KS_TOK_RPAREN || ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_INT || ltok.type == KS_TOK_STR || (KS_TOK_ISOP(ltok.type) && last_op.type == SYT_UOP)) {
                // this can be either a binary infix operator or a unary postfix operator, handle unary postfix first

                //KPE_OC(ctok, "++", syuop_postinc) else 
                //{
                    // do nothing, as it should now be a binary operator
                //}

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
                    KPE_OC(ctok, "%" , sybop_mod) else
                    KPE_OC(ctok, "^" , sybop_pow) else
                    KPE_OC(ctok, "<" , sybop_lt) else
                    KPE_OC(ctok, ">" , sybop_gt) else
                    KPE_OC(ctok, "==" , sybop_eq) else
                    {
                        ks_parse_err(kp, ctok, "InternalError; Unhandled operator");
                        goto _kspco_end;
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
                ks_parse_err(kp, ctok, "Invalid Syntax; not a valid syntax");
                goto _kspco_end;

            }


        } else if (ctok.type == KS_TOK_LPAREN) {
            n_lparens++;

            if (ltok.type == KS_TOK_IDENT || ltok.type == KS_TOK_RPAREN) {
                // do a function call
                // op_stk = ... func
                // out_stk = ... NULL


                Spush(out_stk, NULL);

                // this is like a 'start group', so the parser knows where the function started
                Spush(op_stk, (syop) { .type = SYT_FUNC });
            } else {
                // just do a normal group start
                Spush(op_stk, (syop) { .type = SYT_LPAREN });
            }

        } else if (ctok.type == KS_TOK_RPAREN) {
            n_rparens++;

            if (ltok.type == KS_TOK_COMMA || (KS_TOK_ISOP(ltok.type) && last_op.type == SYT_UOP && last_op.assoc == SYA_UNARY_PRE)) {
                ks_parse_err(kp, ltok, "Invalid Syntax; expected some value after this");
                goto _kspco_end;
            }

            //took_off = 0;
            while (op_stk.p >= 0 && Stop(op_stk).type != SYT_LPAREN) {
                PROCESS_OPERATOR
                //took_off++;
            }

            if (is_func) {
                // if we parsed off a function, we already handled it
                //ks_parse_err(kp, ctok, "Function call");

            } else {

                if (ltok.type == KS_TOK_LPAREN) {
                    ks_parse_err(kp, ltok, "Invalid Syntax; this is an empty group");
                    goto _kspco_end;
                }
            
                // if we parsed off a normal group, expect it to end like this
                if (Stop(op_stk).type == SYT_LPAREN) {
                    // pop it off, continue
                    Spop_unused(op_stk);
                } else {
                    // when encountering another parentheses
                    ks_parse_err(kp, ctok, "Invalid Syntax; unexpected ')' : %d");
                    goto _kspco_end;
                    break;
                }
            }

        } else if (ctok.type == KS_TOK_LBRACE || ctok.type == KS_TOK_RBRACE || ctok.type == KS_TOK_SEMI) {
            // this means the expression has stopped
            //ctok = ltok;
            break;

        } else {
            ks_parse_err(kp, ctok, "Invalid Syntax; unexpected token here");
            goto _kspco_end;
        }

        ltok = ctok;
    }

    if (n_lparens > n_rparens) {
        ks_parse_err(kp, ltok, "SyntaxError: Mismatched parentheses, expected another ')' after this");
        goto _kspco_end;
    } else if (n_lparens < n_rparens) {
        ks_parse_err(kp, ltok, "SyntaxError: Mismatched parentheses, have an extra ')'");
        goto _kspco_end;
    }

    // while operators on the stack, should only be binary or unary, because
    //   all functions/things should have already been popped off
    while (op_stk.p >= 0) {
        PROCESS_OPERATOR
    }

    if (out_stk.p != 0) {
        ks_parse_err(kp, ctok, "SyntaxError: Malformed expression (ended with out:%d, ops:%d)", out_stk.p + 1, op_stk.p + 1);
        goto _kspco_end;
    }

    //if (ks_ast_codegen(out_stk.base[0], to) != 0) {
    //    rc = ks_parse_err(kp, ctok, "Internal Error in ks_ast_codegen()");
    //    goto _kspas_end;
    //}

    _kspco_end: ;
    int p = out_stk.p;
    ks_str_free(&str);
    out_stk.p = -1;
    op_stk.p = -1;
    return p == 0 ? out_stk.base[0] : NULL;
}

ks_ast ks_parse_block(ks_parse* kp) {

    ks_token ctok = kp->tokens[kp->tok_i];

    // skip newlines
    while (ctok.type == KS_TOK_NEWLINE) {
        ctok = kp->tokens[++kp->tok_i];
    }

    if (ctok.type == KS_TOK_LBRACE) {
        // parse list
        // skip '{'
        ++kp->tok_i;

        ks_ast body = ks_ast_new_block(0, NULL);

        do {
            ctok = kp->tokens[kp->tok_i];

            if (ctok.type == KS_TOK_RBRACE) {
                break;
            }

            if (ctok.type == KS_TOK_NEWLINE || ctok.type == KS_TOK_SEMI) {
                ++kp->tok_i;
                continue;
            }

            ks_ast body_sub = ks_parse_block(kp);
            if (body_sub == NULL) return NULL;
            // recursively parse it
            ks_ast_block_add(body, body_sub);

        } while (true);

        // expect '}'
        if (ctok.type != KS_TOK_RBRACE) {
            ks_parse_err(kp, ctok, "Expected '}'");
            return NULL;
        }
        ++kp->tok_i;

        return body;

    } else if (ctok.type == KS_TOK_IDENT && (TOK_EQ(ctok, "ret"))) {
        // parse ret val
        // skip `ret`
        ++kp->tok_i;

        ks_ast val = ks_parse_expr(kp);
        if (val == NULL) return NULL;

        return ks_ast_new_ret(val);

    } else if (ctok.type == KS_TOK_IDENT && (TOK_EQ(ctok, "if"))) {
        // parse if (COND) { BODY }
        // skip `if`
        ++kp->tok_i;

        //ctok = kp->tokens[*kpi];
        ks_ast cond = ks_parse_expr(kp);
        if (cond == NULL) return NULL;

        ks_ast body = ks_parse_block(kp);
        if (body == NULL) return NULL;
        
        return ks_ast_new_if(cond, body);

    } else if (ctok.type == KS_TOK_IDENT && (TOK_EQ(ctok, "while"))) {
        // parse while (COND) { BODY }
        // skip `while`
        ++kp->tok_i;

        //ctok = kp->tokens[*kpi];
        ks_ast cond = ks_parse_expr(kp);
        if (cond == NULL) return NULL;

        ks_ast body = ks_parse_block(kp);
        if (body == NULL) return NULL;
        
        return ks_ast_new_while(cond, body);


    } else if (ctok.type == KS_TOK_RBRACE) {
        // this just means its the end
        return NULL;
    } else if (ctok.type == KS_TOK_IDENT || ctok.type == KS_TOK_LPAREN) {
        // assums its just a normal expression, add it
        return ks_parse_expr(kp);
    } else {
        ks_parse_err(kp, ctok, "Invalid Syntax");
        return NULL;
    }

    return NULL;
}

ks_ast ks_parse_code(ks_parse* kp) {

    ks_ast body = ks_ast_new_block(0, NULL);

    for (; kp->tok_i < kp->tokens_n; ++kp->tok_i) {
        ks_token ctok = kp->tokens[kp->tok_i];

        if (ctok.type == KS_TOK_NEWLINE || ctok.type == KS_TOK_SEMI) {
            continue;
        }

        // by default, just treat it as a block expression
        ks_ast expr = ks_parse_block(kp);
        if (expr == NULL) return NULL;
        ks_ast_block_add(body, expr);
    }

    return body;
    
}

