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


    return syntax_error(CTOK(), "Expressions not working!");
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



