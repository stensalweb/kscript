/* kso.h - implementation of builtin objects/types */


#pragma once

#ifndef KSO_H__
#define KSO_H__

#include "ks.h"

/* explanation:

Everything in kscript is an object, and can be casted to a `kso`

Primarily, these things are reference counted, using the `->refcnt` value

And so, they can be garbage collected when it reaches 0.

By default, when an object is created, its reference count is 0, you need to always make any reference apparanent

*/

// kscript object, as a pointer to the internal structure
typedef struct kso* kso;

// forward declaration of the type-type
typedef struct ks_type* ks_type;

/* AST -> an abstract syntax tree, representing a tree of computations */
typedef struct ks_ast* ks_ast;

// the base that should begin every object definition
// `refcnt` is the number of alive references
// `flags` are object flags (see KSOF_* macro/enums)
// `type` is a pointer to the type
#define KSO_BASE int32_t refcnt; uint32_t flags; ks_type type;

enum {
    KSOF_NONE = 0
};

// something to initialize, with a reference count
#define KSO_BASE_INIT_R(_type, _flags, _refcnt) .refcnt = (int32_t)(_refcnt), .flags = (uint32_t)(_flags), .type = (ks_type)(_type), 

// something to place in the base initializer, which includes 0 references
#define KSO_BASE_INIT(_type, _flags) KSO_BASE_INIT_R(_type, _flags, 0)

// increments (i.e. records) a reference to an object
#define KSO_INCREF(_obj) (++(_obj)->refcnt)

// decrements (i.e. unrecords) a reference to an object, freeing the object if its reference count goes
//   to 0
#define KSO_DECREF(_obj) { if (--(_obj)->refcnt <= 0) { kso_free((kso)(_obj)); } }

// checks the reference count, and frees the object if it is unreachable
#define KSO_CHKREF(_obj) { if ((_obj)->refcnt <= 0) { kso_free((kso)(_obj)); } }

/* builtin object types */


/* kso -> the generic object type, which all other objects can be casted down to.
Objects down-casted to `kso` can only see the type, reference count, and flags for that object
*/
struct kso {
    KSO_BASE;
};

/* none -> the NULL/empty value
NOTE: There is always a global singleton, `ks_V_none`. No other 'nones' should be allocated or deallocated
*/
typedef struct ks_none {
    KSO_BASE

}* ks_none;

extern ks_none ks_V_none;

// the global `none` value
#define KS_NONE (ks_V_none)

// `none` as an object, downcasted
#define KSO_NONE ((kso)ks_V_none)


/* bool -> the boolean type, representing true or false
NOTE: There are always just 2 global singltons, `ks_V_true`, and `ks_V_false`. No other booleans should be allocated
  or deallocated, and a boolean comparison is equivalent to their pointer comparison
*/
typedef struct ks_bool {
    KSO_BASE

    bool v_bool;

}* ks_bool;

// the global singletons representing `true` and `false` respectively
extern ks_bool ks_V_true, ks_V_false;

// the `true` value
#define KS_TRUE (ks_V_true)

// the `false` value
#define KS_FALSE (ks_V_false)

// return a boolean, given an expression
#define KS_BOOL(_val) ((_val) ? KS_TRUE : KS_FALSE)

// the `true` value, downcasted to an object
#define KSO_TRUE ((kso)KS_TRUE)

// the `false` value, downcasted to an object
#define KSO_FALSE ((kso)KS_FALSE)

// return a boolean (as a generic object), given an expression
#define KSO_BOOL(_val) ((_val) ? KSO_TRUE : KSO_FALSE)


/* int -> represents a whole number
For now, is just a 64bit integer, but in the future, it will be arbitrary size too
This type is immutable
*/
typedef struct ks_int {
    KSO_BASE

    // the actual integer value, as a 64 bit signed integer
    int64_t v_int;

}* ks_int;

/* str -> the string type, a collection of ASCII characters
This type is immutable, and internally is both length encoded & NUL-terminated
(so chr can be passed to C functions)
mem of object: sizeof(struct ks_str) + len + 1
*/
typedef struct ks_str {
    KSO_BASE

    // the hash of the string, cached, because it seems to be useful to precompute them
    uint64_t v_hash;

    // the number of characters in the string, not including a NUL-terminator
    // len("Hello") -> 5
    uint32_t len;

    // the actual string value. In memory, ks_str's are allocated so taking `->chr` just gives the address of
    // the start of the NUL-terminated part of the string. The [2] is to make sure that sizeof(ks_str) will allow
    // for enough room for two characters (this is useful for the internal constants for single-length strings)
    char chr[2];

}* ks_str;

/* tuple -> an ordered collection of objects, which essentially tuples them together as a single value
This type is immutable
mem of object: sizeof(struct ks_tuple) + len * sizeof(kso)
*/
typedef struct ks_tuple {
    KSO_BASE

    // the number of items in the tuple
    uint32_t len;

    // the address of the first item. The tuple is allocated with the items in the main buffer
    kso items[0];

}* ks_tuple;

/* list -> the list type, a collection of other objects
This type is mutable, extendable, etc
*/
typedef struct ks_list {
    KSO_BASE

    // the number of items in the list
    uint32_t len;

    // the items in the list, of at least `len`
    kso* items;

}* ks_list;

// the signature for a C-function taking arguments
typedef kso (*ks_cfunc_sig)(int n_args, kso* args);


/* dict -> generic dictionary that pairs hashable keys to values
This type is mutable, flexible, and general

Internally, it is using a hash-table implementation, similar to the new Python 3.6/7 dictionary (maybe, WIP)
*/
typedef struct ks_dict {
    KSO_BASE

    // number of actual items in the dictionary
    uint32_t n_items;

    // number of buckets in the hash-table
    uint32_t n_buckets;

    // dense array of `n_entries` entries in the hash table
    struct ks_dict_entry {
        // hash(key)
        int64_t hash;

        // the key for this entry
        kso key;

        // the value at this entry
        kso val;

    }* buckets;

}* ks_dict;


// return a new empty dictionary
ks_dict ks_dict_new_empty();

// set an item in the dictionary
int ks_dict_set(ks_dict self, kso key, uint64_t hash, kso val);

// get an item in the dictionary
kso ks_dict_get(ks_dict self, kso key, uint64_t hash);


// create a C-function with a given name
#define KS_CFUNC_DECL(_cfunc_name) kso _cfunc_name(int n_args, kso* args)

// createa a C function with a type, function name (with an underscore between them)
#define KS_CFUNC_TDECL(_type_name, _cfunc_name) kso _type_name##_##_cfunc_name(int n_args, kso* args)

/* cfunc -> a type wrapping a C-function which operates on kscript objects as args
*/
typedef struct ks_cfunc {
    KSO_BASE

    ks_cfunc_sig v_cfunc;

}* ks_cfunc;




/* bytecode enumerations, detailing different instructions
This is also the main reference for the functionality of the bytecode, outside of the
  interprereter source code
Scheme: SIZE[TYPE description], SIZE in bytes of the section, TYPE being what it can be casted to
  and a short description of what that parameter does
*/
enum {

    /* NOOP - does nothing, changes nothing, just skips over the instruction
    1[KSBC_NOOP]
    */
    KSBC_NOOP = 0,

    /* CONST - used to push a constant/literal (which is stored in the `v_const` of the code)
        which comes from the 4 byte `int` value stored in the instruction itself
    1[KSBC_CONST] 4[int v_const_idx, index into the `v_const` list for which constant it is]
    */
    KSBC_CONST,

    /* CONST_TRUE - push on the constant 'true' (boolean), without using the extra 4 bytes 
    1[KSBC_CONST_TRUE]
    */
    KSBC_CONST_TRUE,
    /* CONST_FALSE - push on the constant 'false' (boolean), without using the extra 4 bytes 
    1[KSBC_CONST_FALSE]
    */
    KSBC_CONST_FALSE,
    /* CONST_NONE - push on the constant 'none' (none-type), without using the extra 4 bytes 
    1[KSBC_CONST_NONE]
    */
    KSBC_CONST_NONE,

    /* POPU - pops off a value from the stack, which is unused (i.e. will remove the stack's ref)
        and possibly free the object, if the refcnt reaches 0
    1[KSBC_POP]
    */
    KSBC_POPU,

    /* LOAD - reads an index into the `v_const` list, and then treats that as a key to the current VM state,
        looking up a value by that key. This is the generic lookup function that will search locals/globals
    1[KSBC_LOAD] 4[int v_const_idx, the name of the object to load]
    */
    KSBC_LOAD,

    /* STORE - reads an index into the `v_const` list, and then treats that as a key to the current VM state,
        looking up a value by that key, and setting its entry to the top of the stack. This is the generic
        setting function to locals. NOTE: The top object is not popped off, so it should be followed by a
        `KSBC_POPU` if its not used subsequently
    1[KSBC_LOAD] 4[int v_const_idx, the name of the object to load]
    */
    KSBC_STORE,

    /* CALL - pops off `n_items` items, and performs a functor-like call, with the first such object
        as the functor. So, `A B C call(3)` yields `A(B, C)`. To call with no arguments, `n_items` should
        still be at least 1. 
    1[KSBC_CALL] 4[int n_items, number of items (including the functor) that the call should have]
    */
    KSBC_CALL,
    
    /* TUPLE - pops off `n_items` items, and turns them into a tuple, pushing on the result
    1[KSBC_TUPLE] 4[int n_items, number of items in the tuple]
    */
    KSBC_TUPLE,

    /* LIST - pops off `n_items` items, and turns them into a list, pushing on the result
    1[KSBC_LIST] 4[int n_items, number of items in the list]
    */
    KSBC_LIST,

    
    /** binary operators **/

    /* ADD - pops off 2 objects, binary-adds them
    1[KSBC_ADD] */
    KSBC_ADD,
    /* SUB - pops off 2 objects, binary-subtracts them
    1[KSBC_SUB] */
    KSBC_SUB,
    /* MUL - pops off 2 objects, binary-muliplies them
    1[KSBC_MUL] */
    KSBC_MUL,
    /* DIV - pops off 2 objects, binary-divides them
    1[KSBC_DIV] */
    KSBC_DIV,

    /* LT - pops off 2 objects, binary-less-than compares them
    1[KSBC_DIV] */
    KSBC_LT,
    /* GT - pops off 2 objects, binary-greater-than compares them
    1[KSBC_DIV] */
    KSBC_GT,
    /* EQ - pops off 2 objects, binary-equality checks them
    1[KSBC_EQ] */
    KSBC_EQ,

    /** jumps/branches **/
    
    /* JMP - unconditionally jumps ahead `relamt` bytes in the bytecode
    1[KSBC_JMP] 4[int relamt] */
    KSBC_JMP,

    /* JMPT - pops off a value from the stack, jumps ahead `relamt` bytes in the bytecode
        if the value was exactly `ks_V_true`, otherwise jump ahead 0
    1[KSBC_JMPT] 4[int relamt] */
    KSBC_JMPT,

    /* JMPF - pops off a value from the stack, jumps ahead `relamt` bytes in the bytecode
        if the value was exactly `ks_V_false`, otherwise jump ahead 0
    1[KSBC_JMPF] 4[int relamt] */
    KSBC_JMPF,


    /** return/higher order control flow **/

    /* RET - return the last value on the stack as the function result
    1[KSBC_RET] */
    KSBC_RET,

    /* RET_NONE - return a new none (i.e. does not touch the stack) as the function result
    1[KSBC_RET_NONE] */
    KSBC_RET_NONE,


    // phony enum value to denote the end
    KSBC__END
};


/* structure definitions */

// if the compiler supports it, pack to a single byte, to minimize space
#pragma pack(push, 1)

// instruction only, for 1 byte instructions
typedef struct {
    // the first byte, always the opcode, one of the KSBC_* enum values
    uint8_t op;

} ksbc_;

// instruction + a 4 byte signed integer
typedef struct {
    // the first byte, opcode, one of the KSBC_* enum values
    uint8_t op;

    // the signed, 32 bit integer encoded with the instruction
    int32_t i32;

} ksbc_i32;


// the generic bytecode object which can hold all of them
typedef union {

    // opcode only
    ksbc_ _;

    // 1[opcode] 4[int i32]
    ksbc_i32 i32;

} ksbc;

// stop our single byte alignment
#pragma pack(pop)


/* code -> a bytecode object which can be executed */
typedef struct ks_code {
    KSO_BASE

    // a reference to a list of constants the bytecode references
    // (since internally, instructions just store an index into this array)
    ks_list v_const;

    // number of bytes the bytecode currently holds
    int bc_n;

    // the actual bytecode
    uint8_t* bc;

    // number of meta-asts for things like debugging
    int meta_ast_n;

    // list of those meta-asts
    struct {
        // the point where it 'takes over' as the current ast
        int bc_n;

        // the ast itself
        ks_ast ast;

    }* meta_ast;

}* ks_code;

/* kfunc -> a bytecode function that is callable from C and kscript */
typedef struct ks_kfunc {
    KSO_BASE

    // the internal bytecode used by the function
    ks_code code;

    // a list of `ks_str` which represents the parameter names
    ks_list params;

}* ks_kfunc;

/* type -> a type of an object, which can be built-in or user defined */
struct ks_type {
    KSO_BASE

    // the type's common name (i.e. "int", "str", etc)
    ks_str name;

    // type.str(self) -> should return a string of the object, like a toString method
    kso f_str;

    // type.repr(self) -> should return a string representation of the object, like a repr method
    kso f_repr;

    // type.free(self) -> should free all the resources associated with the object, including references
    // except the object itself
    kso f_free;


    /** operator functions **/

    // type.add(A, B) -> returns the + op of A, B
    kso f_add;

    // type.sub(A, B) -> returns the - op of A, B
    kso f_sub;

    // type.mul(A, B) -> returns the * op of A, B
    kso f_mul;

    // type.div(A, B) -> returns the / op of A, B
    kso f_div;


    /** comparison functions **/

    // <, >, == functions
    kso f_lt, f_gt, f_eq;

};

#define KS_TYPE_INIT KSO_BASE_INIT_R(ks_T_type, KSOF_NONE, 1) .name = NULL, .f_str = NULL, .f_repr = NULL, .f_add = NULL, .f_sub = NULL, .f_mul = NULL, .f_div = NULL, .f_lt = NULL, .f_gt = NULL, .f_eq = NULL, 

/* token enum, tells the kinds of tokens */
enum {

    /* none/error token */
    KS_TOK_NONE = 0,

    /* combo token, combination of multiples */
    KS_TOK_COMBO,

    /* an integer literal */
    KS_TOK_INT,

    /* a string literal */
    KS_TOK_STR,

    /* a variable identifier (this can also be a keyword in a language) */
    KS_TOK_IDENT,

    /* a comment, which is typically ignored */
    KS_TOK_COMMENT,

    /* a newline, \n */
    KS_TOK_NEWLINE,

    /* the end of file token */
    KS_TOK_EOF,


    /* a literal comma, ',' */
    KS_TOK_COMMA,

    /* a literal colon, ':' */
    KS_TOK_COLON,

    /* a literal semicolon, ';' */
    KS_TOK_SEMI,


    /* left parenthes
    is, '(' */
    KS_TOK_LPAR,
    /* right parenthesis, ')' */
    KS_TOK_RPAR,

    /* left bracket, '[' */
    KS_TOK_LBRACK,
    /* right bracket, ']' */
    KS_TOK_RBRACK,

    /* left brace, '{' */
    KS_TOK_LBRACE,
    /* right brace, '}' */
    KS_TOK_RBRACE,

    /** operators **/

    /* add operator, '+' */
    KS_TOK_O_ADD,
    /* sub operator, '-' */
    KS_TOK_O_SUB,
    /* mul operator, '*' */
    KS_TOK_O_MUL,
    /* div operator, '/' */
    KS_TOK_O_DIV,
    /* mod operator, '%' */
    KS_TOK_O_MOD,
    ///* pow operator, '**' */
    //KS_TOK_O_POW,

    /* less-than operator, '<' */
    KS_TOK_O_LT,
    /* greater-than operator, '>' */
    KS_TOK_O_GT,
    /* equal-to operator, '==' */
    KS_TOK_O_EQ,

    /* assignment operator, '=' */
    KS_TOK_O_ASSIGN,


    // phony ending member
    KS_TOK__END

};

/* parser -> an object which can be used to parse the grammar/language into ASTs, and bytecode objects */
typedef struct ks_parser* ks_parser;

/* tok -> a token type which represents a piece of input for a given length, and being of some classification (see above) */
typedef struct ks_tok {
    // the token type, see an above KS_TOK_* enum value
    int ttype;

    // reference to the parser
    ks_parser v_parser;

    // offset and length in bytes
    int offset, len;

    // the line and column which it starts
    int line, col;

} ks_tok;

/* parser -> an object which can be used to parse the grammar/language into ASTs, and bytecode objects */
struct ks_parser {
    KSO_BASE

    // the human readable name of the source.
    // If a file input, then this is just the name of the file given
    ks_str src_name;

    // the source of the entire file
    ks_str src;

    // the current token index
    int tok_i;

    // number of tokens
    int n_toks;

    // array of tokens
    ks_tok* toks;

};


/* vm -> the virtual machine object, which can run code */
typedef struct ks_vm {
    KSO_BASE

    // the global stack
    ks_list stk;

    // the global dictionary of variables (i.e. builtins)
    ks_dict globals;

    // the number of scopes currently on the VM
    int n_scopes;

    // list 
    struct ks_vm_scope {

        // reference to the code the scope is in
        ks_code code;

        // the program counter for this scope
        uint8_t* pc;

        // the start of the bytecode, for computing relative addresses
        uint8_t* start_bc;

        // the starting stack length at the beginning of the scope
        int start_stk_len;

        // the const for this list
        ks_list v_const;

        // the locals for the scope
        ks_dict locals;

    }* scopes;

}* ks_vm;



/* enumeration of the types of AST */
enum {

    /* none/error AST */
    KS_AST_ERR = 0,

    /* means this AST represents a constant integer value */
    KS_AST_INT,

    /* means this AST represents a constant string value */
    KS_AST_STR,

    /* means this AST represents a variable reference, which will be looked up */
    KS_AST_VAR,

    /* means this AST represents the `true` constant */
    KS_AST_TRUE,
    /* means this AST represents the `false` constant */
    KS_AST_FALSE,
    /* means this AST represents the `none` constant */
    KS_AST_NONE,

    /* means this AST represents a tuple to be created */
    KS_AST_TUPLE,

    /* means this AST represents a list to be created */
    KS_AST_LIST,


    /* means this AST represents a function call with a given number of arguments */
    KS_AST_CALL,

    /* means this AST represents a list of other ASTs in a block */
    KS_AST_BLOCK,

    /* means this AST represents a conditional block */
    KS_AST_IF,

    /* means this AST represents a while-loop block */
    KS_AST_WHILE,

    /* means this AST represents a function-literal, aka a lambda */
    KS_AST_FUNC,

    /* means this AST represents a return statement */
    KS_AST_RET,

    /* means the AST represents a block of code (assembly code) to be ran */
    KS_AST_CODE,

    /** binary operators **/

    // the first binary operator
    #define KS_AST_BOP__START KS_AST_BOP_ADD
    // the last binary operator
    #define KS_AST_BOP__END KS_AST_BOP_ASSIGN

    /* add: +, the sum of two objects */
    KS_AST_BOP_ADD,
    /* sub: -, the diff of two objects */
    KS_AST_BOP_SUB,
    /* mul: *, the prod of two objects */
    KS_AST_BOP_MUL,
    /* div: /, the quot of two objects */
    KS_AST_BOP_DIV,
    /* mod: %, modulo */
    KS_AST_BOP_MOD,
    /* pow: ^, power */
    KS_AST_BOP_POW,
    /* lt: <, less than */
    KS_AST_BOP_LT,
    /* gt: >, greater than */
    KS_AST_BOP_GT,
    /* eq: ==, equal to */
    KS_AST_BOP_EQ,

    /* assign: =, special operator denoting a name setting */
    KS_AST_BOP_ASSIGN,


    // phony member to denote the end
    KS_AST__END

};

/* AST -> an abstract syntax tree, representing a tree of computations */
struct ks_ast {
    KSO_BASE

    // the type of AST, one of the above `KS_AST_*` enum values
    int atype;

    // the token that this AST directly references (i.e. a single token)
    ks_tok tok;

    // the token that represents all sub expressions, i.e. all children combined into a token
    ks_tok tok_expr;

    // a union representing all the possible values of the AST
    union {

        /* int value iff atype==KS_AST_INT */
        ks_int v_int;

        /* str value iff atype==KS_AST_STR */
        ks_str v_str;

        /* var name iff atype==KS_AST_VAR */
        ks_str v_var;

        /* list of ASTs iff atype==KS_AST_LIST or atype==KS_AST_TUPLE */
        ks_list v_list;


        /* a list of the function & its arguments iff atype==KS_AST_CALL 
        So, `f(A, B, C)` would have a list of size 4 containing [f, A, B, C]
        */
        ks_list v_call;

        /* a list of the sub items of the block iff atype==KS_AST_BLOCK */
        ks_list v_block;

        /* the condition and code to be ran if true iff atype==KS_AST_IF */
        struct {
            // the conditional in the parentheticals
            ks_ast cond;

            // the body inside the braces
            ks_ast body;

        } v_if;

        /* the condition and code to be ran if true iff atype==KS_AST_WHILE */
        struct {
            // the conditional in the parentheticals
            ks_ast cond;

            // the body inside the braces
            ks_ast body;

        } v_while;

        /* the structure describing a function literal */
        struct {
            
            // list of the strings of the function names (all of these should be ks_str objects)
            ks_list params;

            // the body of the function
            ks_ast body;

        } v_func;

        /* the value to return iff atype==KS_AST_RET */
        ks_ast v_ret;

        /* the code object representing the assembly iff atype==KS_AST_CODE */
        ks_code v_code;

        /* the left and right hand sides of the binary operator iff KS_BOP_START <= atype <= KS_BOP_END */
        struct {

            ks_ast L, R;

        } v_bop;

    };

};




/* constructing primitives */

// constructs a new integer, returning the result
ks_int ks_int_new(int64_t v_int);


// create a new string from a character array
ks_str ks_str_new(int len, const char* chr);

// create a new string, taking length as strlen(chr)
ks_str ks_str_new_r(const char* chr);

// creates a new string from C-style format arguments, in va_list passing style
// NOTE: This is a custom implementation, there may be bugs.
// NOTE: This is implemented in `fmt.c`, rather than `kso.c`
ks_str ks_str_new_vcfmt(const char* fmt, va_list ap);

// creates a new string from C-style format arguments (i.e. only C-types are supported, not arbitrary kscript objects)
ks_str ks_str_new_cfmt(const char* fmt, ...);


// create a new empty tuple
ks_tuple ks_tuple_new_empty();


// create a new tuple from var-args, being NUL-terminated
// so, ks_tuple_new_va(NULL) -> empty tuple
// ks_tuple_new_va(a, NULL) -> (a, )
// so have ks_tuple_new_va(a, b, ..., NULL)
#define ks_tuple_new_va(_first, ...) _ks_tuple_new_va((kso)(_first), __VA_ARGS__)
ks_tuple _ks_tuple_new_va(kso first, ...);

// create a tuple containing a single object, o0
ks_tuple ks_tuple_new_1(kso o0);
// create a tuple containing two objects, (o0, o1)
ks_tuple ks_tuple_new_2(kso o0, kso o1);
// create a tuple containing 3 objects, (o0, o1, o2)
ks_tuple ks_tuple_new_3(kso o0, kso o1, kso o2);

// create a new tuple containing some items
ks_tuple ks_tuple_new(int len, kso* items);

// create a new tuple containing some items, return a reference to it
ks_tuple ks_tuple_newref(int len, kso* items);


// create a new empty list
ks_list ks_list_new_empty();

// create a new list containing some items
ks_list ks_list_new(int len, kso* items);

// push an object onto the list, returning the index
// NOTE: This adds a reference to the object
int ks_list_push(ks_list self, kso obj);

// pops an object off of the list, transfering the reference to the caller
// NOTE: Call KSO_DECREF when the object reference is dead
kso ks_list_pop(ks_list self);

// pops off an object from the list, with it not being used
// NOTE: This decrements the reference originally added with the push function
void ks_list_popu(ks_list self);

// clears the list, resetting it to empty
void ks_list_clear(ks_list self);


// create a new C-function wrapper
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc);

// create a new C-function wrapper with a new reference
ks_cfunc ks_cfunc_newref(ks_cfunc_sig v_cfunc);


// create a new (empty) collection of bytecode, given a refrence to a constant list
ks_code ks_code_new_empty(ks_list v_const);

// create a new kfunc from a code and a list of parameters
ks_kfunc ks_kfunc_new(ks_list params, ks_code code);


// sets a new meta ast for debugging/error messages
void ks_code_add_meta(ks_code self, ks_ast ast);

// link in another code object, appending it to the end
void ks_code_linkin(ks_code self, ks_code other);


/* bytecode generation helpers */
void ksc_noop(ks_code code);
void ksc_popu(ks_code code);
void ksc_load(ks_code code, const char* v_name);
void ksc_loadl(ks_code code, int len, const char* v_name);
void ksc_loado(ks_code code, kso obj);
void ksc_store(ks_code code, const char* v_name);
void ksc_storeo(ks_code code, kso obj);
void ksc_const(ks_code code, kso val);
void ksc_const_true(ks_code code);
void ksc_const_false(ks_code code);
void ksc_const_none(ks_code code);
void ksc_int(ks_code code, int64_t v_int);
void ksc_cstr(ks_code code, const char* v_cstr);
void ksc_cstrl(ks_code code, int len, const char* v_cstr);
void ksc_call(ks_code code, int n_items);
void ksc_tuple(ks_code code, int n_items);
void ksc_list(ks_code code, int n_items);
void ksc_add(ks_code code);
void ksc_sub(ks_code code);
void ksc_mul(ks_code code);
void ksc_div(ks_code code);
void ksc_lt(ks_code code);
void ksc_gt(ks_code code);
void ksc_eq(ks_code code);
void ksc_jmp(ks_code code, int relamt);
void ksc_jmpt(ks_code code, int relamt);
void ksc_jmpf(ks_code code, int relamt);
void ksc_ret(ks_code code);
void ksc_ret_none(ks_code code);



// create a new AST representing a constant int
ks_ast ks_ast_new_int(int64_t v_int);

// create a new AST representing a constant string
ks_ast ks_ast_new_str(const char* v_str);

// create a new AST representing a string
ks_ast ks_ast_new_stro(ks_str v_str);

// create a new AST representing the 'true' value
ks_ast ks_ast_new_true();
// create a new AST representing the 'false' value
ks_ast ks_ast_new_false();
// create a new AST representing the 'none' value
ks_ast ks_ast_new_none();


// create a new AST representing a tuple of objects
ks_ast ks_ast_new_tuple(int n_items, ks_ast* items);

// create a new AST representing a list of objects
ks_ast ks_ast_new_list(int n_items, ks_ast* items);

// create a new AST representing a variable reference
ks_ast ks_ast_new_var(const char* var_name);

// create a new AST representing a variable reference
ks_ast ks_ast_new_varl(int len, const char* var_name);

// create a new AST representing a functor call, with `items[0]` being the function
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_call(int n_items, ks_ast* items);

// create a new AST representing a binary operator, assumes KS_BOP__START <= bop_type <= KS_BOP__END
ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R);

// returns a new, empty block AST
ks_ast ks_ast_new_block_empty();

// returns a new block populated by the given arguments
ks_ast ks_ast_new_block(int n_items, ks_ast* items);

// return a new AST representing an `if`
ks_ast ks_ast_new_if(ks_ast cond, ks_ast body);

// return a new AST representing a `while`
ks_ast ks_ast_new_while(ks_ast cond, ks_ast body);

// return a new function representing a function literal
ks_ast ks_ast_new_func(ks_list params, ks_ast body);

// return a new ast representing a return
ks_ast ks_ast_new_ret(ks_ast val);

// returns a new AST representing a bytecode assembly segment
ks_ast ks_ast_new_code(ks_code code);


// generates the bytecode for a given AST, returns the code object
// NOTE: this is implemented in codegen.c, rather than kso.c
ks_code ks_ast_codegen(ks_ast self, ks_list v_const);

// return a new, empty virtual machine
ks_vm ks_vm_new_empty();

// internal execution routine, this should only really be used by internal functions
void ks_vm_exec(ks_vm vm, ks_code code);



// create a new token
ks_tok ks_tok_new(int ttype, ks_parser kp, int offset, int len, int line, int col);

// combine A and B to form a larger meta token
ks_tok ks_tok_combo(ks_tok A, ks_tok B);

// output an error from a given token
void* ks_tok_err(ks_tok tok, const char* fmt, ...);

// create a new parser from a file
ks_parser ks_parser_new_file(const char* fname);

// create a new parser from an expression
ks_parser ks_parser_new_expr(const char* expr);

// parse out an AST representing assembly
ks_ast ks_parse_asm(ks_parser self);

// parse out an AST from the most general parser, which handles a lot of sub-cases, sub-blocks, etc
ks_ast ks_parse_all(ks_parser self);



/* GENERIC OBJECT FUNCTIONALITY */

// calls an object as if it was a function with a list of arguments
kso kso_call(kso func, int n_args, kso* args);

// returns the hash of the object
uint64_t kso_hash(kso obj);

// return whether or not the 2 objects are equal
bool kso_eq(kso A, kso B);


// returns the result of turning `A` into a boolean. It returns 0 if A->false, 1 if A->true, and
//   -1 if it could not be determined
// RULES:
//   if A==KSO_TRUE, ret 1
//   if A==KSO_FALSE, ret 0
//   if A==KSO_NONE, ret 0
//   if A is an int, ret 0 if A==0, 1 otherwise
//   if A is a string, ret 0 if len(A)==0, 1 otherwise
//   if A is a list, ret 0 if len(A)==0, 1 otherwise
//   if A is a tuple, ret 0 if len(A)==0, 1 otherwise
//   if A is a dict, ret 0 if it is empty, 1 otherwise
// TODO: also add a lookup function for custom types
// otherwise, return -1, because it could not be determined
int kso_bool(kso A);


// returns the tostring of the object
// if A is a string, just return A,
// if A is an int, none, bool, just return those strings
// if A's type defines a `f_str` method, call that and return the result
// else, return the string:
// <'%TYPE%' obj @ %ADDR%>
// where %TYPE% is the object's type's name
// and %ADDR% is the pointer formatted name (i.e. 0x238748237483)
ks_str kso_tostr(kso A);

// returns the string representation of an object
// if A is a string, return A with quotes around it
// if A is an int, none, bool, just return the same as tostr
// if A's type defines an `f_repr` method, call that and return the result
// else, return the string:
// else, return the string:
// <'%TYPE%' obj @ %ADDR%>
// where %TYPE% is the object's type's name
// and %ADDR% is the pointer formatted name (i.e. 0x238748237483)
ks_str kso_torepr(kso A);




// frees an object, returns true if successful, false otherwise
bool kso_free(kso obj);

#endif

