/* types/parser.c - represents a language parser, capable of parsing kscript source code


*/

#include "ks_common.h"

// for isdigit()/isalpha()
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

// generates an integer from the token, assuming it is an integer literal
static int64_t tok_getint(ks_tok tok) {

    if (tok.ttype != KS_TOK_INT) {
        ks_warn("tok_getint() passed a non-integer token (type %i)", tok.ttype);
        return 0;
    }
    static char tmp[100];
    int len = tok.len;
    if (len > 99) len = 99;
    memcpy(tmp, tok.v_parser->src->chr + tok.offset, len);
    tmp[len] = '\0';
    return atoll(tmp);
}


// returns a string literal value of the token, but unescaped.
// For example, 'Hello\nWorld' replaces the \\ and n with 
// a literal newline, and removes the quotes around it
static ks_str tok_getstr(ks_tok tok) {
    ks_str start = ks_str_new("");

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
            // handle literal characters
            ks_str new_str = ks_str_new_cfmt("%*s%*s", start->len, start->chr, j, src+i);
            KSO_DECREF(start);
            start = new_str;
            i += j;
        } else if (src[i] == '\\') {
            // handle an escape code, \CODE

            // by default, just handle a single character
            char c = src[++i];
            char lit = '\0';

            // wheter or not there was a valid escape code
            bool hasOne = false;

            if (c == 'n') {
                lit = '\n';
            } else if (c == '\\') {
                lit = '\\';
            }

            if (hasOne) {
                ks_str new_str = ks_str_new_cfmt("%*s%c", start->len, start->chr, lit);
                KSO_DECREF(start);
                start = new_str;
            } else {
                ks_warn("Unknown escape code: '%c'", c);
            }

            // skip the escape code
            i++;
        } else {
            // something weird happened, neither an escape code nor string literals occured,
            //   so stop
            break;
        }

    }

    // just take off our temporary reference
    start->refcnt--;

    return start;
}

/* parser methods */


// internal method to tokenize a parser into discrete tokens, populating the `self->toks` array
static void parser_tokenize(ks_parser self) {

    // get a pointer to the contents of the parser
    char* src = self->src->chr;

    // current line and column
    int line = 0, col = 0;

    // advance `n` characters in the string, incrementing the column and line values as neccessary.
    #define ADV(_n) { int _i; for (_i = 0; _i < (_n); ++_i) { char _c = src[i]; if (!_c) break; if (_c == '\n') { line++; col = 0; } else { col++; } i++; } }

    // whether or not we're still in a valid parsing position
    #define VALID() (i < self->src->len)

    // add a token to the parser, with its starting position, ending position, etc computed by the declared variables
    #define ADD_TOK(_type) { \
        self->toks = ks_realloc(self->toks, sizeof(*self->toks) * ++self->n_toks); \
        self->toks[self->n_toks - 1] = ks_tok_new(_type, self, start_i, i - start_i, start_line, start_col); \
    }

    // the current character
    #define CUR() (src[i])

    int i = 0;
    // loop through the whole source
    while (VALID()) {

        // skip whitespace
        while (VALID() && iswhite(CUR())) {
            ADV(1);
        }

        // make sure we didn't overflow
        if (!VALID()) break;

        // get the first character of this token
        char c = CUR();

        // capture where the token started parsing
        int start_i = i, start_line = line, start_col = col;

        if (c == '#') {
            // parse a comment:
            // # ... \n

            // first, skip the '#'
            ADV(1);

            // keep going until we hit a new line (or out of bounds, obviously)
            while (VALID() && CUR() != '\n') {
                ADV(1);
            }

            // now, declare all of this a comment, and append it to the output
            ADD_TOK(KS_TOK_COMMENT);

            // if the next character is a newline (it should always be), skip it
            //if (CUR() == '\n') ADV(1);

        } else if (isdigit(c)) {
            // this is either an integer or a floating point.
            // For now, we only support integers, but that will of course change

            do {
                ADV(1);
            } while (isdigit(CUR()));

            // declare the string of digits as a single integer
            ADD_TOK(KS_TOK_INT);

        } else if (isidents(c)) {
            // parse an identifier
            do {
                ADV(1);
            } while (isidentm(CUR()));

            // add as an identifier
            ADD_TOK(KS_TOK_IDENT);
        
        } else if (c == '\"') {
            // parse a string literal
            // capture starting values

            // skip opening quotes
            ADV(1);

            // keep scanning until string literal
            while (VALID() && (c = CUR()) != '"') {
                if (c == '\\') {
                    // skip an escape code, but don't actually parse it right now
                    ADV(2);
                } else {
                    ADV(1);
                }
            }

            // check and make sure we're still valid
            if (!VALID() || CUR() != '"') {
                // rewind back for the error
                i = start_i + 2;
                ADD_TOK(KS_TOK_NONE);
                kse_tok((ks_tok)self->toks[self->n_toks - 1], "Expected terminating '\"' for string literal that began here:");
                self->n_toks--;
                break;
            }


            // skip the ending quote
            ADV(1);

            // add it as a token
            ADD_TOK(KS_TOK_STR)

        }

        // macro for translating tokens which are just literals
        #define STR_TOK(_type, _str) else if (strncmp(&CUR(), _str, sizeof(_str) - 1) == 0) { \
            ADV(sizeof(_str) - 1);\
            ADD_TOK(_type);\
        }

        // grammars
        STR_TOK(KS_TOK_NEWLINE, "\n")
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
            // wind to the correct part for the error
            i = start_i + 1;
            ADD_TOK(KS_TOK_NONE);
            kse_tok(self->toks[self->n_toks - 1], "Unexpected character '%c'", c);
            self->n_toks--;
            break;
        }

    }

    // add a token representing an EOF
    int start_i = i - 1, start_line = line, start_col = col;
    ADD_TOK(KS_TOK_EOF);

    // start at the beginning
    self->tok_i = 0;

    /* UNDEFINE MACROS */

    #undef ADV
    #undef VALID
    #undef ADD_TOK
    #undef CUR

}

// create a new parser from an expression
ks_parser ks_parser_new_expr(const char* expr) {

    ks_parser self = (ks_parser)ks_malloc(sizeof(*self));
    *self = (struct ks_parser) {
        KSO_BASE_INIT(ks_T_parser)
        .src_name = ks_str_new("-e"),
        .src = ks_str_new(expr),
        .tok_i = 0,
        .n_toks = 0,
        .toks = NULL,
    };

    /* now, tokenize the source the input */
    parser_tokenize(self);

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
        KSO_BASE_INIT(ks_T_parser)
        .src_name = ks_str_new(fname),
        .src = ks_str_new_l(contents, bytes),
        .tok_i = 0,
        .n_toks = 0,
        .toks = NULL,
    };

    // since we made a new string from it
    ks_free(contents);

    /* now, tokenize the input */
    parser_tokenize(self);

    return self;
}



/*** PARSING ROUTINES ***/


// yield whether a token equals another value
#define TOK_EQ(_p, _tok, _str) (_tok.len == (sizeof(_str) - 1) && (0 == strncmp(self->src->chr + _tok.offset, _str, (sizeof(_str) - 1))))


/* expression parsing:

Essentially, the shunting yard algorithm is used, which aims to maintain a stack desrcibing the current expression.
For more info, see here: https://en.wikipedia.org/wiki/Shunting-yard_algorithm

I will denote stacks with a line starting with a `|`, so:
| 1, 2, 3,
Means the stack has 3 items, and a 3 is on top

There is an operator (Ops) stack and an output (Out) stack. The operator stack just holds which operators were encountered,
whereas the output stack has ASTs (abstract syntax trees). For example, if we have the stacks:
| 1, 2, 3
| +

And we were to 'reduce' it normally, we would get:
| 1, 5
|

We popped off the operator, and applied it to the stack. But, we want to record the addition as a syntax tree, so we turn the 
constants 2 and 3 into a tree node, which gives us:
| 1, (2+3)
|
Now, imagine we are to add a * operator:
| 1, (2+3)
| *
This would reduce to:
| (1*(2+3))
|

For basic expressions just including operators and constants, this algorithm is very simple. It begins becoming more complicated given
more and more kinds of expressions. So, this is just used to parse expressions. Statements such as function definitions, if/while/for blocks,
{} blocks, etc are handled by higher level functions (which are explained down lower in the file)

To build these stacks is quite simple: whenever encountering a value (int, string, variable), just push another node on the stack
If it is an operator, first reduce the stack (as I have shown above) for all operators that are greater precedence [1], so that now the operator
stack knows that this new operator being bushes is the higest precedence.

Before describing the internals, I will go ahead and list the precedences:

PEMDAS is the base reference, but of course is not complete: Here is their order by lowest to highest

= (assignment)
<,>,==,<=,>=,!= (comparison operators)
+,- (AS in PEMDAS)
*,/,% (MD in PEMDAS (and modulo is the same))
** (exponentiation)

And, of course, parenthesis are highest of all, as are function calls, `[]` expressions, etc. Any kind of operator that has a start (i.e. '(')
and an end (i.e. ')') will share the highest precedence. Can you think of why they couldn't have different precedences? Well, if they did,
you could have (weird) expressions such as `x = [(])`, which is obviously wrong. So, whenever a ')' is encountered, it better not be in the middle
of a `[]` expression, and vice versa, a `[]` expression should never be inside and outside a parenthesis `()` group

You can think of precedence as:

  * The lowest precedence operators apply last (so, with `A=2+3`, since the `+` has higher prec. than the `=` operator, the addition is carried
      out first, then the assignment)


So, at any point, when adding an operator, the higher prec. operators [1] are first removed, since they have to happen before the lower one being added

In this way, these stacks are well-ordered by precedence

I just want to say that Dijkstra is, of course, a genius. And I would love to thank him personally for this algorithm. It's quite beautiful.

EXAMPLES:

#1 
for example take the string:
`f(1, 2+3)`

Looking at this, we can tell it is correctly formed. Check lower in the file for details on error checking (specifically, in the shunting yard loop of code)

To parse this, start reading left to right, beginning with empty stacks:
|
|

First, we have a value `f`:
| f
|
Then, we encounter a `(`. Since this is directly after a value, that means it is a function call.
Push on a NULL (I will use `:` for these examples) to the value stack 
(I call this a 'seperator', and it will be useful for nested function calls, as we need to know what the function is)
And push a FUNC operator to the ops.:
| f :
| FUNC
Now, push the 1
| f : 1
| FUNC
Once we hit the comma, we should reduce the operator stack to the last function call. Since the top of the stack is already a FUNC, nothing is done
Now, push the 2:
| f : 1 2
| FUNC
Now, we've hit the last token, a ')'. We don't automatically know if its the end of an expression or function call, so we scan down the operator
stack until we find a FUNC or LPAR. oh look, the top is a function, so look no further!

Now that we know we're computing a function call, we scan down the stack for our seperator (see why its useful now?)
Notice the format we have for a function call: we will have (FUNCTION), a seperator, and then the rest are the arguments.
So, in this case, we pop off until we get to the seperator, skip the seperator, and then grab the function.

We have the function call parsed:


#2
TODO, I will try and add more expression parsing examples in the future


NOTES:
  [1]: Technically, this isn't always true. The stack should also be cleared left-associative operators with equal precedence, and there are
         more error checks done to make the input is correctly formed

*/

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
    
    // KS_AST_* enum values
    int bop_type, uop_type;

    // the token the operator came from
    ks_tok tok;

    // true if there was a comma associated (useful for parsing singlet tuples)
    bool had_comma;

} syop;

// construct a basic syop
#define SYOP(_type, _tok) ((syop){.type = _type, .tok = _tok, .had_comma = false})

// construct a binary operator
#define SYBOP(_prec, _assoc, _bop_type) ((syop){ .type = SYT_BOP, .prec = _prec, .assoc = _assoc, .bop_type = _bop_type })

// construct a fully-loaded sy-operator
#define SYOP_FULL(_type, _prec, _assoc) ((syop){ .type = _type, .prec = _prec, .assoc = _assoc })

// construct a unary operator
#define SYUOP(_assoc, _uop_type) ((syop){ .type = SYT_UOP, .prec = SYP_UNARY, .assoc = _assoc, .uop_type = _uop_type })


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

/* unary operators */

static syop
    syu_neg = SYUOP(SYA_UOP_PRE, KS_AST_UOP_NEG)

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
            KSO_DECREF_N((kso*)&Sget(Out, osp), n_args+1); \
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
            KSO_DECREF_N((kso*)&Sget(Out, osp), n_args+1); \
            new_subs->tok = top.tok; \
            new_subs->tok_expr = ks_tok_combo(((ks_ast)new_subs->v_list->items[0])->tok_expr, ctok); \
            Out.len -= n_args + 2; \
            Spush(Out, new_subs);\
        } else if (top.type == SYT_BOP) { \
            if (Out.len < 2) PEXPR_ERR(top.tok, "Invalid Syntax"); \
            /* construct a binary operator from the last two values on the stack */ \
            ks_ast new_bop = ks_ast_new_bop(top.bop_type, Sget(Out, Out.len-2), Sget(Out, Out.len-1)); \
            new_bop->tok = top.tok; \
            KSO_DECREF(new_bop->v_bop.L); \
            KSO_DECREF(new_bop->v_bop.R); \
            new_bop->tok_expr = ks_tok_combo(new_bop->v_bop.L->tok_expr, new_bop->v_bop.R->tok_expr); \
            Out.len -= 2; \
            Spush(Out, new_bop); \
        } else if (top.type == SYT_UOP) { \
            /* construct a unary operator */ \
            if (Out.len < 1) PEXPR_ERR(top.tok, "Invalid Syntax"); \
            ks_ast new_uop = ks_ast_new_uop(top.uop_type, Spop(Out)); \
            new_uop->tok = top.tok; \
            KSO_DECREF(new_uop->v_uop); \
            Spush(Out, new_uop); \
        } else if (top.type == SYT_LPAR) { \
            /* just skip it */\
        } else if (top.type == SYT_LBRACK) { \
            /* we've encountered a list literal, scan down and find how many objects */ \
            int osp = Out.len - 1; \
            while (osp >= 0 && Sget(Out, osp) != NULL) osp--; \
            int n_args = Out.len - osp - 1; \
            /* now, they are contiguous in memory */ \
            ks_ast new_list = ks_ast_new_list(&Sget(Out, osp+1), n_args); \
            KSO_DECREF_N((kso*)&Sget(Out, osp+1), n_args); \
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

    // macro to cause an error given a token and formatting arguments:
    // EX: PEXPR_ERR(ctok, "Unexpected...", ...)
    #define PEXPR_ERR(...) { kse_tok(__VA_ARGS__); wasErr = true; goto parseexpr_end; }

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

    // get the starting token index, for use in case of an error
    int start_tok_i = self->tok_i;

    // current and last tokens
    ks_tok ctok = { .ttype = KS_TOK_NONE }, ltok = { .ttype = KS_TOK_NONE };

    // whether or not the current state is a valid one
    #define VALID() (self->tok_i <= self->n_toks)

    // get the current token
    #define CTOK() (self->toks[self->tok_i])

    // advance a single token
    #define ADV1() { self->tok_i++; }

    while (true) {

        // skip over comments
        while (VALID() && CTOK().ttype == KS_TOK_COMMENT) ADV1()

        // make sure we're in bounds
        if (!VALID()) goto parseexpr_end;

        // get current token
        ctok = CTOK();

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
            if (TOK_EQ(self, ctok, "if") || TOK_EQ(self, ctok, "then") 
             || TOK_EQ(self, ctok, "elif") || TOK_EQ(self, ctok, "else") 
             || TOK_EQ(self, ctok, "try") || TOK_EQ(self, ctok, "catch")) goto parseexpr_end;

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
                KSO_DECREF(var_s);
                new_var->tok_expr = new_var->tok = ctok;
                Spush(Out, new_var);
            }
        } else if (ctok.ttype == KS_TOK_DOT) {
            // the last token must be a value type, because you need one to take an attribute of
            if (!TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "Invalid Syntax")

            // save the dot token reference
            ks_tok dot_tok = ctok;
            // now advance past it
            ADV1();
            // update the current token to what the attribute will be of
            ctok = CTOK();

            // the only valid attribute is an identifier, i.e. `x.1` does not make sense
            if (ctok.ttype != KS_TOK_IDENT) PEXPR_ERR(ctok, "Attribute after a '.' must be a valid identifier")

            // take off the last
            ks_ast last = Spop(Out);
            // replace it with its attribute
            ks_str attr_name_s = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);
            ks_ast new_attr = ks_ast_new_attr(last, attr_name_s);
            KSO_DECREF(last);
            KSO_DECREF(attr_name_s);

            // set up the new tokens
            new_attr->tok = dot_tok;
            new_attr->tok_expr = ks_tok_combo(last->tok_expr, ctok);

            Spush(Out, new_attr);

        } else if (ctok.ttype == KS_TOK_COMMA) {
            // this is to prevent commas outside of operators outside of brackets
            //if (n_pars < 1 && n_bracks < 1) PEXPR_ERR(ctok, "Unexpected `,`, must have a comma within a group")

            // let someone above us handle the comma
            if (n_pars < 1 && n_bracks < 1) goto parseexpr_end;
            
            // check and make sure no double commas
            if (ltok.ttype == KS_TOK_COMMA) PEXPR_ERR(ks_tok_combo(ltok, ctok), "Invalid Syntax; expected a value between these commas");

            if (!TOKE_ISVAL(ltok.ttype)) PEXPR_ERR(ctok, "SyntaxError; did not expect this here");

            // just reduce the top of the stack until we get to a '(' or '['
            while (Ops.len > 0) {
                // look at the top of the operator stack
                syop top_op = Stop(Ops);
                // stop on any '(' operator
                if (top_op.type == SYT_FUNC || top_op.type == SYT_LPAR) break;
                // stop on any '[' operator
                if (top_op.type == SYT_SUBSCRIPT || top_op.type == SYT_LBRACK) break;

                // else, it was just some operator, so pop it
                POP_OP();
            }

            // set the flag for the operator on top, so it knew it had a comma in it
            if (Ops.len > 0) Stop(Ops).had_comma = true;

        } else if (ctok.ttype == KS_TOK_LBRACK) {
            n_bracks++;

            // we add a NULL to the output as a spacer
            Spush(Out, NULL);
                

            if (TOKE_ISVAL(ltok.ttype))  {
                // add a subscript operation, since it is like `val[`
                Spush(Ops, SYOP(SYT_SUBSCRIPT, ctok));
            } else {
                // just add a `[` which will ultimately become a list start
                Spush(Ops, SYOP(SYT_LBRACK, ctok));
            }

        } else if (ctok.ttype == KS_TOK_RBRACK) {
            n_bracks--;

            // make sure there's been an `[` for this `]`
            if (n_bracks < 0) PEXPR_ERR(ctok, "Invalid Syntax; extra ']', remove it")

            // which one was actually it actually was for
            syop used = {.type = SYT_NONE};

            // a right bracket should clear the stack, to the nearest subscript, or list literal,
            // and then that too
            while (Ops.len > 0) {
                syop top_op = Stop(Ops);

                POP_OP();
                // stop on these operators
                if (top_op.type == SYT_SUBSCRIPT || top_op.type == SYT_LBRACK) { used = top_op; break; }
            }

            if (used.type == SYT_NONE) {
                // then this means neither was found, which means there was some error
                PEXPR_ERR(ctok, "Invalid Syntax; wrong place for ']'")
            }

        } else if (ctok.ttype == KS_TOK_LPAR) {
            n_pars++;
            
            // add a spacer to the output stack
            Spush(Out, NULL);

            if (TOKE_ISVAL(ltok.ttype))  {
                // if the previous item parsed was a value, then this is a function call
                Spush(Ops, SYOP(SYT_FUNC, ctok));
            } else {
                // otherwise, this is a normal expression group, OR a tuple creation
                // add an operator denoting this
                Spush(Ops, SYOP(SYT_LPAR, ctok));
            }

        } else if (ctok.ttype == KS_TOK_RPAR) {
            n_pars--;

            // make sure its balanced
            if (n_pars < 0) PEXPR_ERR(ctok, "Invalid Syntax; extra ')', remove it")

            // check and make sure its valid
            if (!(TOKE_ISVAL(ltok.ttype) || ltok.ttype == KS_TOK_COMMA || ltok.ttype == KS_TOK_LPAR)) PEXPR_ERR(ctok, "Invalid Syntax; did not expect ')' here")

            // which one was actually it actually was for
            syop used = {.type = SYT_NONE};

            // a right parenthesis should clear the stack, to the nearest function call
            // or group
            while (Ops.len > 0) {
                syop top_op = Stop(Ops);
                POP_OP();
                // stop once we hit one that started with '('
                if (top_op.type == SYT_FUNC || top_op.type == SYT_LPAR) { used = top_op; break; }
            }

            if (used.type == SYT_FUNC) {
                // it has already been handled by `POP_OP()`, so we're done
            } else if (used.type == SYT_LPAR) {
                // it was either a normal group: `(3+4*(5+1))`
                // or, a tuple: `(1, 2, )`, `(,)`

                // first, scan down until we hit a spacer
                int osp = Out.len - 1;
                while (osp > 0 && Sget(Out, osp) != NULL) osp--;

                if (osp >= 0) {
                    // we a found NULL before hitting the bottom of the stack

                    // calculate how many items it took
                    int n_items = Out.len - osp - 1;

                    // if they entered `()`, give a syntax error.
                    // the correct 
                    if (n_items == 0 && !used.had_comma) PEXPR_ERR(ks_tok_combo(used.tok, ctok), "Invalid Syntax; given an empty group. To create an empty tuple, use a comma: `(,)`")

                    // TODO: Also check for beginnings of empty tuples that become full tuples later in parsing.
                    // EXAMPLE: `(,3)`==`(3,)`, but this should be a syntax error
                    
                    // the new AST value we are parsing out
                    ks_ast new_val = NULL;

                    if (n_items != 1 || used.had_comma) {
                        // there's definitely a tuple here, so create it
                        new_val = ks_ast_new_tuple(&Sget(Out, osp+1), n_items);
                        KSO_DECREF_N((kso*)&Sget(Out, osp+1), n_items);
                        new_val->tok = used.tok;
                        // join the first and last
                        new_val->tok_expr = ks_tok_combo(new_val->tok, ctok);
                        
                    } else {
                        // else, just yield the value as a math operation
                        new_val = Sget(Out, osp+1);
                    }

                    // remove all values
                    Out.len -= n_items;

                    // take off the seperator (NULL)
                    Spop(Out);

                    // make sure we were given a valid value
                    if (new_val != NULL) {
                        Spush(Out, new_val);
                    } else {
                        PEXPR_ERR(ctok, "Invalid Syntax")
                    }
                }
            }

        } else if (TOKE_ISOP(ctok.ttype)) {

            // try and find the current operator, store it here
            syop used = { .type = SYT_NONE };

            // if case for an operator
            #define KPE_OPCASE(_tok, _opstr, _opval) if (TOK_EQ(self, _tok, _opstr)) { used = _opval; goto kpe_op_resolve; }

            // TODO: implement unary operators
            if (TOKE_ISOP(ltok.ttype) || ltok.ttype == KS_TOK_COMMA || ltok.ttype == KS_TOK_LPAR || ltok.ttype == KS_TOK_LBRACK || ltok.ttype == KS_TOK_LBRACE) {
                // this is a unary prefix operator

                KPE_OPCASE(ctok, "-", syu_neg)

                PEXPR_ERR(ctok, "Invalid Syntax");
            } else {
                // either a binary operator, or a unary postfix operator

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
            }

            kpe_op_resolve: ;

            if (used.type == SYT_NONE) {
                // it was not found
                PEXPR_ERR(ctok, "Unexpected operator");
            }

            // set the metadata
            used.tok = ctok;

            // the precedence threshold for operators to match
            int prec_thresh = used.prec + ((used.assoc == SYA_BOP_LEFT) ? 0 : 1);

            // now, we have a valid operator, so clear the stack of anything with higher precedence than it
            // (+ some rules about equal precedence)
            while (Ops.len > 0) {

                // never go past parethesis or brackets, because those have the lowest precedence
                syop top_op = Stop(Ops);

                // check for '('-based operators, which we should never process here
                if (top_op.type == SYT_LPAR || top_op.type == SYT_FUNC) break;

                // check for '['-based operators, which we should never process here
                if (top_op.type == SYT_SUBSCRIPT || top_op.type == SYT_LBRACK) break;

                // check for the correct precedence threshold.
                // NOTE: since the stack is well ordered, we can go ahead and break here
                if (top_op.prec < prec_thresh) break;

                // if we havent broken out already, go ahead and pop off the top operator
                POP_OP();

            }

            // always push it to the op stack, after the stack has been resolved
            Spush(Ops, used);

        } else {
            if (ctok.ttype == KS_TOK_EOF) {
                PEXPR_ERR(ctok, "Unexpected EOF");
            } else {
                PEXPR_ERR(ctok, "Unexpected token for expr");
            }
        }

        // move the current token we're looking at to the last one
        ltok = CTOK();
        // and then advance another token
        ADV1();
    }

    parseexpr_end: ;

    if (wasErr) {
        ks_free(Out.base);
        ks_free(Ops.base);
        
        return NULL;
    } else {
        // reduce the rest of the operators stack
        while (Ops.len > 0) {
            POP_OP();
        }

        // check for unbalanced parenthesis
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

        // check for unbalanced brackets
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

    #undef VALID
    #undef CTOK
    #undef ADV1
}

// parses a single statement, return the result
// NOTE: if the input's next relevant token is `{`, it will recursively call itself to construct a block
ks_ast ks_parse_stmt(ks_parser self) {

    // raise a parsing error here
    #define PSTMT_ERR(...) { kse_tok(__VA_ARGS__); return NULL; }

    // external error, quit without additional message
    #define PSTMT_ERREXT() { return NULL; }

    // whether or not the parsing state is valid
    #define VALID() (self->tok_i < self->n_toks)

    // get the current token
    #define CTOK() (self->toks[self->tok_i])

    // advance a single token
    #define ADV1() { self->tok_i++; }

    // skip tokens which are irrelevant to statements
    #define SKIP_IRR_S() {  \
        ks_tok _ctok; \
        while (VALID() && (_ctok = CTOK()).ttype == KS_TOK_NEWLINE || _ctok.ttype == KS_TOK_COMMENT || _ctok.ttype == KS_TOK_SEMI) { \
            ADV1(); \
        } \
    }

    // skip tokens which are irrelevant to expressions
    #define SKIP_IRR_E() {  \
        while (VALID() && CTOK().ttype == KS_TOK_COMMENT) { \
            ADV1(); \
        } \
    }

    // make sure its valid
    if (!VALID()) PSTMT_ERR(self->toks[self->n_toks - 1], "Unexpected EOF");

    // current token
    ks_tok ctok = CTOK();

    if (ctok.ttype == KS_TOK_IDENT) {

        // check for keywords/directives
        if (TOK_EQ(self, ctok, "ret")) {
            // parse a return statement:
            // ret EXPR 
            // or
            // ret

            // skip ret
            ADV1();

            // skip things irrelavant to expressions
            SKIP_IRR_E();

            // now, get the next token
            ctok = CTOK();

            if (ctok.ttype == KS_TOK_NEWLINE || ctok.ttype == KS_TOK_EOF || ctok.ttype == KS_TOK_SEMI || ctok.ttype == KS_TOK_RBRACE) {
                // if it has hit an end of file or block, then it should be a ret none
                ks_ast nonval = ks_ast_new_none();
                ks_ast ret = ks_ast_new_ret(nonval);
                KSO_DECREF(nonval);
                return ret;
            } else {
                // otherwise, parse an expression
                ks_ast expr = ks_parse_expr(self);
                if (expr == NULL) PSTMT_ERREXT();

                ks_ast ret = ks_ast_new_ret(expr);
                KSO_DECREF(expr);
                return ret;
            }

        } else if (TOK_EQ(self, ctok, "if")) {
            // parse an if block:
            // if (COND) { BODY } ?(elif (COND1) { BODY1 })* ?(else (CONDLAST) { BODYLAST })

            // skip 'if'
            ADV1();

            // skip irrelevant bits
            SKIP_IRR_E();

            // now, expect an expression
            ks_ast cond = ks_parse_expr(self);
            if (cond == NULL) PSTMT_ERREXT();

            // skip more irrelevant bits
            SKIP_IRR_S();

            // check the next token
            ctok = CTOK();

            // if it is `then`, skip it. This is just useful for 1 liners
            // treat a comma as an equivalent shorthand for then
            if (ctok.ttype == KS_TOK_COMMA || TOK_EQ(self, ctok, "then")) ADV1();

            // recurse here, parse another statement (which may be a block)
            ks_ast body = ks_parse_stmt(self);
            if (body == NULL) PSTMT_ERREXT();


            // the entire chain of ifs, elifs, and the else
            ks_ast if_chain = ks_ast_new_if(cond, body, NULL);
            KSO_DECREF(cond); KSO_DECREF(body);

            // the last place in the linked list of elif chains,
            // where to insert the next 'else' section to
            ks_ast cur_link = if_chain;

            SKIP_IRR_S();

            // check of elif's and elses
            ctok = CTOK();

            // parse elif blocks
            while (TOK_EQ(self, ctok, "elif")) {

                // skip 'elif'
                ADV1();
                // get the conditional
                ks_ast elif_cond = ks_parse_expr(self);
                if (elif_cond == NULL) PSTMT_ERREXT();

                ctok = CTOK();

                // skip irrelevant parts
                SKIP_IRR_S();
                
                // check for a single line shorthand
                ctok = CTOK();
                if (ctok.ttype == KS_TOK_COMMA || TOK_EQ(self, ctok, "then")) ADV1();

                ctok = CTOK();

                // parse the body of the else if, and connect it
                ks_ast elif_body = ks_parse_stmt(self);
                if (elif_body == NULL) PSTMT_ERREXT();


                // the element of the chain that this elif describes
                ks_ast elif_link = ks_ast_new_if(elif_cond, elif_body, NULL);
                KSO_DECREF(elif_cond); KSO_DECREF(elif_body);

                // attach to the end of the linked list
                ks_ast_attach_else(cur_link, elif_link);
                KSO_DECREF(elif_link);
                cur_link = elif_link;

                // skip any irrelevant tokens
                SKIP_IRR_S();

                // try and keep looping
                ctok = CTOK();

            }


            // parse the optional else
            if (TOK_EQ(self, ctok, "else")) {
                // skip else
                ADV1();

                SKIP_IRR_S();
                
                // try and skip these
                ctok = CTOK();
                if (ctok.ttype == KS_TOK_COMMA || TOK_EQ(self, ctok, "then")) ADV1();

                SKIP_IRR_S();

                // parse a statement or body for the else block
                ks_ast v_else = ks_parse_stmt(self);
                if (v_else == NULL) PSTMT_ERREXT();

                ks_ast_attach_else(cur_link, v_else);
                KSO_DECREF(v_else);

            }
            
            // return the linked list of conditionals
            return if_chain;

        } else if (TOK_EQ(self, ctok, "elif")) {
            PSTMT_ERR(ctok, "SyntaxError; 'elif' without previous 'if'");
        } else if (TOK_EQ(self, ctok, "else")) {
            PSTMT_ERR(ctok, "SyntaxError; 'else' without previous 'if'");

        } else if (TOK_EQ(self, ctok, "while")) {
            // parse an while block:
            // while (COND) { BODY }

            // skip 'while'
            ADV1();

            // skip irrelevant bits
            SKIP_IRR_E();

            // now, expect an expression
            ks_ast cond = ks_parse_expr(self);
            if (cond == NULL) PSTMT_ERREXT();

            // skip more irrelevant bits
            SKIP_IRR_S();

            // check for a single line shorthand
            ctok = CTOK();
            if (ctok.ttype == KS_TOK_COMMA || TOK_EQ(self, ctok, "do")) ADV1();


            // recurse here, maybe parse a whole block
            ks_ast body = ks_parse_stmt(self);
            if (body == NULL) PSTMT_ERREXT();

            ks_ast ret_while = ks_ast_new_while(cond, body);
            KSO_DECREF(cond); 
            KSO_DECREF(body);
            return ret_while;

        } else if (TOK_EQ(self, ctok, "try")) {
            // parse try {...} (?catch {...})
            // skip try
            ADV1();

            // skip comments/etc
            SKIP_IRR_S();

            // now, parse a block for the 'try' block
            ks_ast v_try = ks_parse_stmt(self);
            if (v_try == NULL) PSTMT_ERREXT();

            SKIP_IRR_S();

            // check for a single line shorthand
            ctok = CTOK();
            if (ctok.ttype == KS_TOK_COMMA) ADV1();

            SKIP_IRR_S();

            // check for a 'catch' block
            ctok = CTOK();
            if (TOK_EQ(self, ctok, "catch")) {
                // skip 'catch'
                ADV1();

                SKIP_IRR_S();

                ks_str catch_target = NULL;

                ctok = CTOK();
                if (ctok.ttype == KS_TOK_IDENT) {
                    // we have a catch target
                    catch_target = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);
                    ADV1();
                }

                SKIP_IRR_S();

                // check for a single line shorthand
                ctok = CTOK();
                if (ctok.ttype == KS_TOK_COMMA) ADV1();
     
                SKIP_IRR_S();
                // now, read the main block
                ks_ast v_catch = ks_parse_stmt(self);
                if (v_catch == NULL) PSTMT_ERREXT();


                // set up the try/catch block
                ks_ast ret_tryc = ks_ast_new_try(v_try, v_catch, catch_target);

                KSO_DECREF(v_try);

                KSO_DECREF(v_catch);

                if (catch_target != NULL) KSO_DECREF(catch_target);

                return ret_tryc;
            

            } else {
                // no 'catch, so just return a try block

                ks_ast ret_tryc = ks_ast_new_try(v_try, NULL, NULL);

                KSO_DECREF(v_try);

                return ret_tryc;
            }




        } else if (TOK_EQ(self, ctok, "catch")) {
            PSTMT_ERR(ctok, "SyntaxError; 'catch' without previous 'try'");

        } else if (TOK_EQ(self, ctok, "func")) {

            // parse a function definition
            // func NAME(ARGS...) { BODY }

            // skip 'func'
            ADV1();

            // first, read the name
            ks_tok fd_tok = ctok = CTOK();
            if (ctok.ttype != KS_TOK_IDENT) PSTMT_ERR(ctok, "Expected a function name identifier here");

            ks_str myname = ks_str_new_l(self->src->chr+ctok.offset, ctok.len);

            // skip the name
            ADV1();

            // expect a `(`
            ctok = CTOK();
            if (ctok.ttype != KS_TOK_LPAR) PSTMT_ERR(ctok, "Expected a '(' to begin the list of function parameters here");
            ADV1();

            // create a list of parameter names
            ks_list param_names = ks_list_new_empty();

            while ((ctok = CTOK()).ttype != KS_TOK_RPAR) {
                if (ctok.ttype != KS_TOK_IDENT) PSTMT_ERR(ctok, "Expected a parameter name identifier here");

                ks_str new_param = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);
                // add it as a parameter
                ks_list_push(param_names, (kso)new_param);
                KSO_DECREF(new_param);

                // skip it
                ADV1();
                // if given a comma, skip it
                if (CTOK().ttype == KS_TOK_COMMA) ADV1();

            }

            // expect a `)`
            ctok = CTOK();
            if (ctok.ttype != KS_TOK_RPAR) PSTMT_ERR(ctok, "Expected a ')' to end the list of function parameters here");
            ADV1();

            ks_tok fdl_tok = ks_tok_combo(fd_tok, ctok);

            // skip irrelevant details
            SKIP_IRR_S();

            // expect a '{'
            ctok = CTOK();
            if (ctok.ttype != KS_TOK_LBRACE) PSTMT_ERR(ctok, "Expected a '{' to begin the function body here");

            // parse a body block from the function
            ks_ast body = ks_parse_stmt(self);
            if (body == NULL) PSTMT_ERREXT();

            // construct a new function value
            ks_ast new_func = ks_ast_new_func(myname, param_names, body);
            new_func->tok = fd_tok;
            new_func->tok_expr = fdl_tok;

            KSO_DECREF(myname);
            KSO_DECREF(param_names);
            KSO_DECREF(body);

            return new_func;

        } else if (TOK_EQ(self, ctok, "type")) {
            // parse a typedefinition
            // type NAME { ... }

            ks_tok td_tok = ctok;

            // skip 'type'
            ADV1();

            // first, read the name
            ctok = CTOK();
            if (ctok.ttype != KS_TOK_IDENT) PSTMT_ERR(ctok, "Expected a type name identifier here");
            ADV1();

            td_tok = ks_tok_combo(td_tok, ctok);
            
            ks_str myname = ks_str_new_l(self->src->chr + ctok.offset, ctok.len);

            // skip irrelevant details
            SKIP_IRR_S();

            // expect a '{'
            ctok = CTOK();
            if (ctok.ttype != KS_TOK_LBRACE) PSTMT_ERR(ctok, "Expected a '{' to begin the type definition body here");

            // now, parse the body
            ks_ast type_body = ks_parse_stmt(self);
            if (type_body == NULL) PSTMT_ERREXT();

            // now, construct the type
            ks_ast new_type = ks_ast_new_type(myname, type_body);
            KSO_DECREF(myname);
            KSO_DECREF(type_body);

            new_type->tok_expr = new_type->tok = td_tok;

            return new_type;

        } else {
            // it is just a default expression
            return ks_parse_expr(self);
        }

    } else if (ctok.ttype == KS_TOK_LBRACE) {
        // parse another block,
        // { BODY }

        // skip '{'
        ks_tok s_tok = CTOK();
        ADV1();

        // create a new block
        ks_ast res = ks_ast_new_block_empty();

        // skip irrelevant tokens
        SKIP_IRR_S();

        while (CTOK().ttype != KS_TOK_RBRACE) {
            ks_ast sub = ks_parse_stmt(self);
            if (sub == NULL) PSTMT_ERREXT();

            // add the sub statement to the list
            ks_list_push(res->v_list, (kso)sub);
            KSO_DECREF(sub);

            // skip irrelevant tokens
            SKIP_IRR_S();
        }

        // expect the ending `}`
        if (CTOK().ttype != KS_TOK_RBRACE) {
            PSTMT_ERR(s_tok, "Expected matching '}' to this");
        }

        // skip the ending `}`
        ADV1();

        return res;

    } else if (TOKE_ISVAL(ctok.ttype) || ctok.ttype == KS_TOK_LBRACK || ctok.ttype == KS_TOK_LPAR) {

        // just parse a normal expression
        ks_ast res = ks_parse_expr(self);
        if (res == NULL) PSTMT_ERREXT();
        return res;

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
            PSTMT_ERR(ctok, "Unexpected EOF after this");
        } else {
            PSTMT_ERR(ctok, "Unexpected token");
        }
    }

    return NULL;
}


// parse an entire kscript program file
ks_ast ks_parse_program(ks_parser self) {
    // block of contents
    ks_ast blk = ks_ast_new_block_empty();

    // we use macros defined in other functions to help out here
    SKIP_IRR_S();

    // the current token
    ks_tok ctok = {.ttype = KS_TOK_NONE};

    // keep parsing until we hit an end
    while (VALID() && (ctok = CTOK()).ttype != KS_TOK_RBRACK && ctok.ttype != KS_TOK_EOF) {
        SKIP_IRR_S();

        ks_ast sub = ks_parse_stmt(self);
        if (sub == NULL) return NULL;

        // add the statement
        ks_list_push(blk->v_list, (kso)sub);
        KSO_DECREF(sub);

        // skip more irrelevant details
        SKIP_IRR_S();
    }

    return blk;
}





/* exported type functions */

TFUNC(parser, free) {
    #define SIG "parser.__free__(self)"
    REQ_N_ARGS(1);
    ks_parser self = (ks_parser)args[0];
    REQ_TYPE("self", self, ks_T_parser);


    ks_free(self->toks);

    KSO_DECREF(self->src_name);

    KSO_DECREF(self->src);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}


/* exporting functionality */

struct ks_type T_parser, *ks_T_parser = &T_parser;

void ks_init__parser() {

    /* create the type */
    T_parser = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_parser, "parser");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_parser, "__free__", "parser.__free__(self)", parser_free_);

}




