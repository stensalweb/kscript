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

#define _MIN(_a, _b) ((_a) < (_b) ? (_a) : (_b))
#define _MAX(_a, _b) ((_a) > (_b) ? (_a) : (_b))

// combine A and B to form a larger meta token
ks_tok ks_tok_combo(ks_tok A, ks_tok B) {
    return (ks_tok) {
        .parser = A.parser, .type = KS_TOK_COMBO,
        .pos = _MIN(A.pos, B.pos), .len = _MAX(A.pos+A.len, B.pos+B.len) - _MIN(A.pos, B.pos),
        .line = _MIN(A.line, B.line), .col = _MIN(A.col, B.col)
    };
}

// return whether a given token type is a valid yielding type
static bool tok_isval(int type) {
    return type == KS_TOK_RPAR || type == KS_TOK_IDENT || type == KS_TOK_INT || type == KS_TOK_STR || type == KS_TOK_RBRK;
}

// return whether a given token type is a valid operator
static bool tok_isop(int type) {
    return false;
}


// generates an integer from the token, assuming it is an integer literal
static int64_t tok_getint(ks_tok tok) {

    if (tok.type != KS_TOK_INT) {
        ks_warn("tok_getint() passed a non-integer token (type %i)", tok.type);
        return 0;
    }
    static char tmp[100];
    int len = tok.len;
    if (len > 99) len = 99;
    memcpy(tmp, tok.parser->src->chr + tok.pos, len);
    tmp[len] = '\0';
    return atoll(tmp);
}

// returns a string literal value of the token, but unescaped.
// For example, 'Hello\nWorld' replaces the \\ and n with 
// a literal newline, and removes the quotes around it
static ks_str tok_getstr(ks_tok tok) {
    ks_str inside_quotes = ks_str_new_l(tok.parser->src->chr + tok.pos + 1, tok.len - 2);

    ks_str res = ks_str_unescape(inside_quotes);
    KS_DECREF(inside_quotes);
    return res;
}

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
        } else if (c == '\'' || c == '"') {
            char s_c = c;

            ADV();
            while (self->src->chr[i] && self->src->chr[i] != s_c && self->src->chr[i] != '\n') {
                if (self->src->chr[i] == '\\') {
                    // escape code; skip it
                    ADV();
                }
                ADV();
            }


            if (self->src->chr[i] != s_c) {

                ks_tok badtok = (ks_tok){ 
                    .parser = self, .type = KS_TOK_NONE, 
                    .pos = start_i, .len = i - start_i, 
                    .line = start_line, .col = start_col 
                };
                return syntax_error(badtok, "Didn't find ending quote for string literal");
            }

            ADV();

            // add the token
            ADDTOK(KS_TOK_STR);

        }
        
        CASE_S(KS_TOK_NEWLINE, "\n")

        CASE_S(KS_TOK_DOT, ".")
        CASE_S(KS_TOK_COMMA, ",")
        CASE_S(KS_TOK_COL, ":")
        CASE_S(KS_TOK_SEMI, ";")

        CASE_S(KS_TOK_LPAR, "(")
        CASE_S(KS_TOK_RPAR, ")")
        CASE_S(KS_TOK_LBRK, "[")
        CASE_S(KS_TOK_RBRK, "]")
        CASE_S(KS_TOK_LBRC, "{")
        CASE_S(KS_TOK_RBRC, "}")

        // operators
        CASE_S(KS_TOK_OP, "+") CASE_S(KS_TOK_OP, "-")
        CASE_S(KS_TOK_OP, "*") CASE_S(KS_TOK_OP, "/")
        CASE_S(KS_TOK_OP, "%") CASE_S(KS_TOK_OP, "**")
        
        CASE_S(KS_TOK_OP, "<") CASE_S(KS_TOK_OP, "<=")
        CASE_S(KS_TOK_OP, ">") CASE_S(KS_TOK_OP, ">=")
        CASE_S(KS_TOK_OP, "==") CASE_S(KS_TOK_OP, "!=")

        CASE_S(KS_TOK_OP, "=")

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

// yield whether a token equals a string constant too
// NOTE: Assumes the parser is named 'self'

#define TOK_EQ(_tok, _str) (_tok.len == (sizeof(_str) - 1) && (0 == strncmp(self->src->chr + _tok.pos, _str, (sizeof(_str) - 1))))


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

        // unary operators should override most operators except power
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
    syu_neg = SYUOP(SYA_UOP_PRE, KS_AST_UOP_NEG),
    syu_sqig = SYUOP(SYA_UOP_PRE, KS_AST_UOP_SQIG)
    

;

#ifndef KS_DECREF_N
#define KS_DECREF_N(_objs, _n) { \
    int i, n = _n;               \
    for (i = 0; i < n; ++i) {    \
        KS_DECREF((_objs)[i]);   \
    }                            \
}
#endif

// Parse a single expression out of 'p'
// NOTE: Returns a new reference
ks_ast ks_parser_parse_expr(ks_parser self) {

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

    // pushes an item onto the stack
    #define Spush(_stk, _val) { int _idx = (_stk).len++; (_stk).base = ks_realloc((_stk).base, sizeof(*(_stk).base) * _stk.len); (_stk).base[_idx] = (_val); }
    // pops off an item from the stack, yielding the value
    #define Spop(_stk) (_stk).base[--(_stk).len]
    // yields the top value of the stack
    #define Stop(_stk) (_stk).base[(_stk).len - 1]
    // pops off an unused object from the stack (just use this one on Out, not Ops)
    #define Spopu(_stk) { kso _obj = Spop(_stk); KS_INCREF(_obj); KS_DECREF(_obj); }
    // gets `idx`'th item of `_stk`
    #define Sget(_stk, _idx) (_stk).base[_idx]

    // raise an error
    #define KPPE_ERR(...) {  syntax_error(__VA_ARGS__); goto kppe_err; }


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
            ks_ast new_call = ks_ast_new_call(Sget(Out, osp), n_args, (ks_ast*)&Sget(Out, osp+1)); \
            KS_DECREF_N(&Sget(Out, osp), n_args+1); \
            new_call->tok = top.tok; \
            new_call->tok_expr = ks_tok_combo(((ks_ast)new_call->children->elems[0])->tok_expr, ctok); \
            Out.len -= n_args + 2; \
            Spush(Out, new_call);\
        } else if (top.type == SYT_BOP) { \
            ks_ast R = Spop(Out); \
            ks_ast L = Spop(Out); \
            ks_ast new_bop = ks_ast_new_bop(top.bop_type, L, R); \
            new_bop->tok = top.tok; new_bop->tok_expr = ks_tok_combo(L->tok_expr, R->tok_expr); \
            KS_DECREF(L); KS_DECREF(R); \
            Spush(Out, new_bop); \
        } else if (top.type == SYT_LPAR) { \
            /* skip it */ \
        } else { \
            KPPE_ERR(ctok, "Internal Operator Error (%i)", top.type); \
        } \
        Spop(Ops); \
    }

    // current & last tokens
    ks_tok ctok = { .type = KS_TOK_NONE }, ltok = { .type = KS_TOK_NONE };

    ks_tok start_tok = CTOK();

    // number of (left paren) - (right paren) or left - right brackets
    int n_pars = 0, n_brks = 0;


    while (VALID()) {

        // skip things that are irrelevant to expressions
        SKIP_IRR_E();

        // try and end it
        if (!VALID()) goto kppe_end;

        ctok = CTOK();

        // check if we should stop parsing due to being at the end
        if (ctok.type == KS_TOK_EOF || ctok.type == KS_TOK_NEWLINE || 
            ctok.type == KS_TOK_SEMI || 
            ctok.type == KS_TOK_LBRC || ctok.type == KS_TOK_RBRC) goto kppe_end;

        if (ctok.type == KS_TOK_INT) {
            // push an integer onto the value stack

            // convert token to actual int value
            ks_int new_int = ks_int_new(tok_getint(ctok));

            // transform it into an AST
            ks_ast new_ast = ks_ast_new_const((ks_obj)new_int);
            KS_DECREF(new_int);

            new_ast->tok = new_ast->tok_expr = ctok;

            // push it on the output stack
            Spush(Out, new_ast);

        } else if (ctok.type == KS_TOK_STR) {
            // push a string onto the value stack


            // convert token to actual string value
            ks_str new_str = tok_getstr(ctok);

            if (!new_str) goto kppe_err;

            // transform it into an AST
            ks_ast new_ast = ks_ast_new_const((ks_obj)new_str);
            KS_DECREF(new_str);

            new_ast->tok = new_ast->tok_expr = ctok;

            // push it on the output stack
            Spush(Out, new_ast);

        } else if (ctok.type == KS_TOK_IDENT) {
            // push a variable reference

            // TODO: handle keywords

            // convert token to actual string value
            ks_str var_s = ks_str_new_l(self->src->chr + ctok.pos, ctok.len);

            // transform it into an AST
            ks_ast new_ast = ks_ast_new_var(var_s);
            KS_DECREF(var_s);

            new_ast->tok = new_ast->tok_expr = ctok;
            
            // push it on the output stack
            Spush(Out, new_ast);
        } else if (ctok.type == KS_TOK_DOT) {
            // the last token must be a value type, because you need one to take an attribute of
            if (!tok_isval(ltok.type)) KPPE_ERR(ctok, "Invalid Syntax")

            // save the dot token reference
            ks_tok dot_tok = ctok;
            // now advance past it
            ADV_1();
            // update the current token to what the attribute will be of
            ctok = CTOK();

            // the only valid attribute is an identifier, i.e. `x.1` does not make sense
            if (ctok.type != KS_TOK_IDENT) KPPE_ERR(ctok, "Attribute after a '.' must be a valid identifier")

            // take off the last
            ks_ast last = Spop(Out);
            // replace it with its attribute
            ks_str attr_name_s = ks_str_new_l(self->src->chr + ctok.pos, ctok.len);
            ks_ast new_attr = ks_ast_new_attr(last, attr_name_s);
            KS_DECREF(last);
            KS_DECREF(attr_name_s);

            // set up the new tokens
            new_attr->tok = dot_tok;
            new_attr->tok_expr = ks_tok_combo(last->tok_expr, ctok);

            Spush(Out, new_attr);

        } else if (ctok.type == KS_TOK_OP) {

            // operator was pushed, 
            // TODO: handle keywords

            // try and find the current operator, store it here
            syop used = { .type = SYT_NONE };

            // if case for an operator
            #define KPE_OPCASE(_tok, _opstr, _opval) if (TOK_EQ(_tok, _opstr)) { used = _opval; goto kpe_op_resolve; }

            // TODO: implement unary operators
            if (ltok.type == KS_TOK_NONE || tok_isop(ltok.type) || ltok.type == KS_TOK_COMMA || ltok.type == KS_TOK_LPAR || ltok.type == KS_TOK_LBRK || ltok.type == KS_TOK_LBRC) {
                // this is a unary prefix operator

                KPE_OPCASE(ctok, "-", syu_neg)
                KPE_OPCASE(ctok, "~", syu_sqig)

                KPPE_ERR(ctok, "Unexpected operator");

            } else {
                // either a binary operator, or a unary postfix operator

                if (tok_isval(ltok.type)) {
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
                    KPPE_ERR(ctok, "Unexpected operator");
                }
            }

            kpe_op_resolve: ;

            if (used.type == SYT_NONE) {
                // it was not found
                KPPE_ERR(ctok, "Unexpected operator");
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



        } else if (ctok.type == KS_TOK_COMMA) {
            // this is to prevent commas outside of operators outside of brackets
            //if (n_pars < 1 && n_bracks < 1) PEXPR_ERR(ctok, "Unexpected `,`, must have a comma within a group")

            // let someone above us handle the comma
            if (n_pars < 1 && n_brks < 1) goto kppe_end;
            
            // check and make sure no double commas
            if (ltok.type == KS_TOK_COMMA) KPPE_ERR(ks_tok_combo(ltok, ctok), "Invalid Syntax; expected a value between these commas");

            if (!(tok_isval(ltok.type) || ltok.type == KS_TOK_LPAR)) KPPE_ERR(ctok, "SyntaxError; did not expect this here");

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

        } else if (ctok.type == KS_TOK_LPAR) {
            n_pars++;
            
            // add a spacer to the output stack
            Spush(Out, NULL);

            if (tok_isval(ltok.type))  {
                // if the previous item parsed was a value, then this is a function call
                Spush(Ops, SYOP(SYT_FUNC, ctok));
            } else {
                // otherwise, this is a normal expression group, OR a tuple creation
                // add an operator denoting this
                Spush(Ops, SYOP(SYT_LPAR, ctok));
            }

        } else if (ctok.type == KS_TOK_RPAR) {
            n_pars--;

            // make sure its balanced
            if (n_pars < 0) KPPE_ERR(ctok, "Invalid Syntax; extra ')', remove it");

            // check and make sure its valid
            if (!(tok_isval(ltok.type) || ltok.type == KS_TOK_COMMA || ltok.type == KS_TOK_LPAR)) KPPE_ERR(ctok, "Invalid Syntax; did not expect ')' here")

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
                    if (n_items == 0 && !used.had_comma) KPPE_ERR(ks_tok_combo(used.tok, ctok), "Invalid Syntax; given an empty group. To create an empty tuple, use a comma: `(,)`");

                    // TODO: Also check for beginnings of empty tuples that become full tuples later in parsing.
                    // EXAMPLE: `(,3)`==`(3,)`, but this should be a syntax error
                    
                    // the new AST value we are parsing out
                    ks_ast new_val = NULL;

                    if (n_items != 1 || used.had_comma) {
                        // there's definitely a tuple here, so create it
                        KPPE_ERR(ctok, "Tuple not supported!");
                        //new_val = ks_ast_new_tuple(&Sget(Out, osp+1), n_items);
                        KS_DECREF_N((ks_obj*)&Sget(Out, osp+1), n_items);
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
                        KPPE_ERR(ctok, "Invalid Syntax")
                    }
                }
            }

        } else {
            // unknown token
            KPPE_ERR(ctok, "Unexpected token");
        }


        // advance to next token
        ltok = CTOK();
        ADV_1();

    }
    

    kppe_end:
    // successful end label
    if (n_pars > 0) {
        syntax_error(ltok, "Missing ending parenthesis after here");
        goto kppe_err;
    }

    // pop off operators
    while (Ops.len > 0) {
        POP_OP();
    }

    if (Out.len == 1) {
        ks_ast ret = Sget(Out, 0);
        ks_free(Out.base);
        ks_free(Ops.base);
        return ret;
    } else {
        // empty expression
        //PEXPR_ERR(ctok, "Invalid expression");

        syntax_error(start_tok, "Invalid Expression");
        goto kppe_err;
    }


    return syntax_error(CTOK(), "Expressions not working!");

    kppe_err:
    // ending error label

    ks_free(Out.base);
    ks_free(Ops.base);

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
    } else if (ctok.type == KS_TOK_IDENT || ctok.type == KS_TOK_INT || ctok.type == KS_TOK_STR || ctok.type == KS_TOK_LPAR || ctok.type == KS_TOK_LBRK) {

        // parse expression
        return ks_parser_parse_expr(self);
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



