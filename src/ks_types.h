/* ks_types.h - definition of the builtin/primitive types

This defines the internal structure of the builtin types, as well as descibing their
functionality, including constructing, managing, etc

the builtin types:

  * none: the none-type, only a global singleton exists
  * bool: the boolean type, either true or false, and 2 singletons exist for the values
  * int: an integer type, typically a wrapper around the largest machine size integer
  * long (not included yet): an arbitrarily long integer
  * str: a string of characters, with a length. these are immutable
  * tuple: an immutable collection indexable like a list
  * list: a mutable, indexable collection of objects
  * dict: a key->value mapping that can take any type in or out
  * code: executable collection of bytecode
  * kfunc: wrapper around the code that descibes a function
  * cfunc: a wrapper around a callable C function

See the C files in `types/` for the actual implementations of the types

*/

#pragma once
#ifndef KS_TYPES_H__
#define KS_TYPES_H__

#include "ks.h"


/* kso - K-Script-Object, representing the most general of objects;
Every kscript object can be down-casted to this, and have the KSO_BASE parameters referenced generically.
Thus, most functions will take `kso`'s as arguments
*/
typedef struct kso* kso;

/* type - represents a type, its members, functions, etc */
typedef struct ks_type* ks_type;

/* AST -> an abstract syntax tree, representing a tree of computations */
typedef struct ks_ast* ks_ast;

// the signature for a C-function which is callable from kscript
typedef kso (*ks_cfunc_sig)(int n_args, kso* args);

/* OBJECT MANIPULATION */

// the base that should begin every object definition
// `refcnt` is the number of alive references (should start at 1, when the object is created)
// `flags` are object flags (see KSOF_* macro/enums), default is KSOF_NONE
// `type` is a pointer to the type
#define KSO_BASE int32_t refcnt; uint32_t flags; ks_type type;

// macro to initialize a kscript object, with a reference count and flags
#define KSO_BASE_INIT_RF(_refcnt, _flags, _type) .refcnt = (int32_t)(_refcnt), .flags = (uint32_t)(_flags), .type = (ks_type)(_type), 

// default initialization macro, intiializes with no special flags, and a single reference
#define KSO_BASE_INIT(_type) KSO_BASE_INIT_RF(1, KSOF_NONE, _type)

// increments (i.e. records) a reference to an object
#define KSO_INCREF(_obj) (++(_obj)->refcnt)

// decrements (i.e. unrecords) a reference to an object, freeing the object if its reference count goes
//   to 0
#define KSO_DECREF(_obj) { kso __obj = (kso)(_obj); if (--(__obj)->refcnt <= 0) { kso_free(__obj); } }

// checks the reference count, and frees the object if it is unreachable
#define KSO_CHKREF(_obj) { kso __obj = (kso)(_obj); if ((__obj)->refcnt <= 0) { kso_free(__obj); } }


// flags for objects
enum {
    // none, empty flags, the default
    KSOF_NONE = 0,

    // means the value is immortal and should never be freed. This is useful for singletons, global constants, etc
    KSOF_IMMORTAL = (1U << 31),

    KSOF__END

};


/* TYPE DEFINITIONS */

// forward declaration of the objects representing the types

/* the builtin types, see `types/` directory*/
extern ks_type
    ks_T_none,
    ks_T_bool,
    ks_T_int,
    ks_T_str,
    ks_T_tuple,
    ks_T_list,
    ks_T_dict,
    
    ks_T_code,
    ks_T_kfunc,

    ks_T_cfunc,

    ks_T_ast,
    ks_T_parser,

    ks_T_type,
    ks_T_module

;

/* the generic kso is empty asides from the base information every object must have */
struct kso {
    KSO_BASE
};


/* none -> a NULL/empty value
NOTE: There is always a global singleton, `ks_V_none`. No other 'nones' should be allocated or deallocated
*/
typedef struct ks_none {
    KSO_BASE
}* ks_none;

// declares the global `none` value, a singleton
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

// declaring the global singletons representing `true` and `false` respectively
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

/* int -> represents a whole number, 64 bits large, with a sign.
*/
typedef struct ks_int {
    KSO_BASE

    // the actual integer value, as a 64 bit signed integer
    int64_t v_int;

}* ks_int;

// constructs a new integer from a given value, with a new reference
ks_int ks_int_new(int64_t v_int);


/* str -> the string type, a collection of ASCII characters
This type is immutable, and internally is both length encoded & NUL-terminated
(so the pointer can be passed to C functions)
mem of object: sizeof(struct ks_str) + len
There is no NULL string, only the empty one. So, any calls to construct a string 
  from NULL are valid; they yield the empty string
Repeated concatenation and generation is expensive, because they are immutable.
So, it is recommended you use the string builder class `ks_strB` to create strings that contain variable numbers of other strings
*/
typedef struct ks_str {
    KSO_BASE

    // the hash of the string, cached, because it seems to be useful to precompute them
    uint64_t v_hash;

    // the number of characters in the string, not including a NUL-terminator
    // len("Hello") -> 5
    uint32_t len;

    // the actual string value. In memory, ks_str's are allocated so that taking `->chr` just gives the address of
    // the start of the NUL-terminated part of the string. The [2] is to make sure that sizeof(ks_str) will allow
    // for enough room for two characters (this is useful for the internal constants for single-length strings),
    // and so new strings can be created with: `malloc(sizeof(*ks_str) + length)`
    char chr[2];

}* ks_str;

// test if a string object is equal to a constant
// EX: KS_STR_EQ_CONST(object, "test")
#define KS_STR_EQ_CONST(_strobj, _cval) ((_strobj)->len == (sizeof(_cval) - 1) && memcmp(_cval, (_strobj)->chr, sizeof(_cval) - 1) == 0)

// create a new string from a C-style character array, and a length of the string
ks_str ks_str_new_l(const char* cstr, int len);

// create a new string from a NUL-terminated C-style string, whose length is determined by `strlen(cstr)`
ks_str ks_str_new(const char* cstr);

// create a new string from a C-string, returning a new reference
ks_str ks_str_new(const char* cstr);



/* string building utilities (strB)
basic workflow:

ks_strB builder = ks_strB_create();
ks_strB_add_c(&builder, "asdf");
...
ks_str result = ks_strB_finish(&builder);

And that is all! Now, result will have created a reference for you

See `fmt.c` for the implementation

*/

typedef struct {
    // the current string that is being built.
    // NOTE: This is not meant to pass to other functions yet; call `ks_strB_finish()` to get a normal ks_str object
    ks_str cur;

} ks_strB;


// create a new empty string builder
ks_strB ks_strB_create();

// append a string to the string builder
void ks_strB_add(ks_strB* strb, char* val, int len);

// append `repr(obj)` to the string builder
void ks_strB_add_repr(ks_strB* strb, kso obj);

// append `str(obj)` to the string builder
void ks_strB_add_tostr(ks_strB* strb, kso obj);

// finish building the string, and return the new string
// after this, the builder should not be used again, without first assigning a new `ks_strB_create()` to it
ks_str ks_strB_finish(ks_strB* strb);


/* tuple -> an ordered collection of objects, which essentially groups them together as a single value
This type is immutable
mem of object: sizeof(struct ks_tuple) + len * sizeof(kso)
*/
typedef struct ks_tuple {
    KSO_BASE

    // the number of items in the tuple
    uint32_t len;

    // the address of the first item. The tuple is allocated with the items in the main buffer,
    // so this acts as the offset from the object pointer to the values
    kso items[0];

}* ks_tuple;

// construct an empty tuple, containing no items
ks_tuple ks_tuple_new_empty();

// create a new tuple containing items passed in
ks_tuple ks_tuple_new(kso* items, int n_items);


/* list -> the list type, a collection of other objects
This type is mutable, appendable, etc
*/
typedef struct ks_list {
    KSO_BASE

    // the number of items in the list
    uint32_t len;

    // the items in the list, of at least `len`
    kso* items;

}* ks_list;

// construct an empty list, containing nothing
ks_list ks_list_new_empty();

// create a new list containing some items
ks_list ks_list_new(kso* items, int n_items);

// push an object onto the list, returning the index
// NOTE: This adds a reference to the object
int ks_list_push(ks_list self, kso obj);

// push a list of object onto the list, returning the index of the first one added
// NOTE: This adds a reference to each object
int ks_list_pushN(ks_list self, kso* objs, int len);


// pops an object off of the list, transfering the reference to the caller
// NOTE: Call KSO_DECREF when the object reference is dead
kso ks_list_pop(ks_list self);

// pops off an object from the list, with it not being used
// NOTE: This decrements the reference originally added with the push function
void ks_list_popu(ks_list self);

// clears the list, resetting it to empty, and freeing memory
void ks_list_clear(ks_list self);


/* dict -> generic dictionary that pairs keys to values (of every type)
This type is mutable, flexible, and general
*/
typedef struct ks_dict {
    KSO_BASE

    // number of actual entries in the dictionary
    uint32_t n_items;

    // number of buckets in the hash-table (strictly >> n_items)
    uint32_t n_buckets;

    // buckets, holding various entries
    struct ks_dict_entry {
        // hash(key)
        int64_t hash;

        // the key for this entry
        kso key;

        // the value at this entry, NULL if the bucket is currently empty
        kso val;

    }* buckets;

}* ks_dict;


// set an item in the dictionary, given a key
int ks_dict_set(ks_dict self, kso key, uint64_t hash, kso val);

// get an item in the dictionary, given a key
// NOTE: returns NULL if the key did not exist
kso ks_dict_get(ks_dict self, kso key, uint64_t hash);

// gets an item in the dictionary, given a C-string key
kso ks_dict_get_cstr(ks_dict self, char* cstr);

// sets an item in the dictionary, given a C-string key
void ks_dict_set_cstr(ks_dict self, char* cstr, kso val);


// create a new, empty dictionary at the minimum size
// NOTE: Does not create a reference
ks_dict ks_dict_new_empty();


/* code -> stores bytecode, and can be appended to */
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
        // the point where it 'takes over' as the current ast, so it can be searched
        int bc_n;

        // the ast itself
        ks_ast ast;

    }* meta_ast;

}* ks_code;

// create a new (empty) collection of bytecode, given a refrence to a constant list
ks_code ks_code_new_empty(ks_list v_const);

// sets a new meta ast for debugging/error messages, assumes position is the current `bc_n` value
void ks_code_add_meta(ks_code self, ks_ast ast);

// link in another code object, appending it to the end, and merging its referenced constants
void ks_code_linkin(ks_code self, ks_code other);


/* bytecode generation helpers */

// noop; do nothing
void ksc_noop     (ks_code code);
// popu; pop unused value
void ksc_popu     (ks_code code);

// load "v_name"; loads a value
void ksc_load     (ks_code code, const char* v_name);
// load_a "v_attr"; replaces the top item with its attribute `v_attr`
void ksc_load_a   (ks_code code, const char* v_attr);

// store "v_name"; takes top value and stores it as a variable `v_name`
void ksc_store    (ks_code code, const char* v_name);
// store "v_attr"; takes top two values (obj, value), and sets `obj.v_attr = value`, 
//   then takes off obj, value, and just pops on value
void ksc_store_a  (ks_code code, const char* v_attr);

/* different versions for specific argument types */

// length encoded
void ksc_loadl    (ks_code code, const char* v_name, int len);
void ksc_load_al  (ks_code code, const char* v_attr, int len);
void ksc_storel   (ks_code code, const char* v_name, int len);
void ksc_store_al (ks_code code, const char* v_attr, int len);

// object format
void ksc_loado    (ks_code code, kso o_name);
void ksc_load_ao  (ks_code code, kso o_attr);
void ksc_storeo   (ks_code code, kso o_name);
void ksc_store_ao (ks_code code, kso o_attr);

// const val; pushes a constant
void ksc_const    (ks_code code, kso val);
// const true; pushes a true value
void ksc_const_true(ks_code code);
// const false; pushes a false value
void ksc_const_false(ks_code code);
// const none; pushes a none value
void ksc_const_none(ks_code code);
// const `v_int`; pushes a literal integer
void ksc_int      (ks_code code, int64_t v_int);

/* different constant loaders for C-strings */

void ksc_cstr      (ks_code code, const char* v_cstr);
void ksc_cstrl     (ks_code code, const char* v_cstr, int len);

// call n_items; pops on a function call on the last n_items (includes the function)
void ksc_call      (ks_code code, int n_items);
// getitem n_items; pops on a object.getitem on the last n_items (includes the object itself)
void ksc_getitem   (ks_code code, int n_items);
// setitem n_items; pops on a object.setitem on the last n_items (includes the object itself)
void ksc_setitem   (ks_code code, int n_items);
// tuple n_items; creates a tuple from the last n_items
void ksc_tuple     (ks_code code, int n_items);
// list n_items; creates a list from the last n_items
void ksc_list      (ks_code code, int n_items);

/* binary operators */
void ksc_add       (ks_code code);
void ksc_sub       (ks_code code);
void ksc_mul       (ks_code code);
void ksc_div       (ks_code code);
void ksc_mod       (ks_code code);
void ksc_pow       (ks_code code);
void ksc_lt        (ks_code code);
void ksc_le        (ks_code code);
void ksc_gt        (ks_code code);
void ksc_ge        (ks_code code);
void ksc_eq        (ks_code code);
void ksc_ne        (ks_code code);

/* branching/conditionals */
void ksc_jmp       (ks_code code, int relamt);
void ksc_jmpt      (ks_code code, int relamt);
void ksc_jmpf      (ks_code code, int relamt);

/* returning/control flow */
void ksc_ret       (ks_code code);
void ksc_ret_none  (ks_code code);


/* kfunc -> a wrapper that parameterizes a bytecode object */
typedef struct ks_kfunc {
    KSO_BASE

    // the internal bytecode used by the function
    ks_code code;

    // a list of `ks_str` which represents the parameter names
    ks_list params;

}* ks_kfunc;


// create a new kfunc from a code and a list of parameters
ks_kfunc ks_kfunc_new(ks_list params, ks_code code);


/* type -> a type of an object, which can be built-in or user defined */
struct ks_type {
    KSO_BASE

    // the type's common name (i.e. "int", "str", etc)
    ks_str name;

    // type.free(self) -> should free all the resources associated with the object, including references
    // except the object itself
    kso f_free;


    // type.str(self) -> should return a string of the object, like a toString method
    kso f_str;

    // type.repr(self) -> should return a string representation of the object, like a repr method
    kso f_repr;

    // type.hash(self) -> should return an integer representing a hash value
    kso f_hash;


    /** attribute functions **/

    // type.getattr(self, attr) -> return `self.attr`
    kso f_getattr;
    // type.setattr(self, attr, val) -> set  `self.attr = val`
    kso f_setattr;

    /** item functions **/

    // type.getitem(self, attr) -> return `self[item]`
    kso f_getitem;
    // type.setitem(self, attr, val) -> set  `self[item] = val`
    kso f_setitem;

    /** operator functions **/

    //  +      -
    kso f_add, f_sub;
    //  *      /
    kso f_mul, f_div;
    //  %      **
    kso f_mod, f_pow;

    /** comparison functions **/

    //  <      <=
    kso f_lt,  f_le;
    //  >      >=
    kso f_gt,  f_ge;
    //  ==     !=
    kso f_eq,  f_ne;


    // the rest of the members of the type
    ks_dict __dict__;

};

// initializes a given type, with a C-string style name
#define KS_TYPE_INIT(_name) KSO_BASE_INIT_R(ks_T_type, KSOF_NONE, 1) .name = ks_str_new(_name), \
    .f_str = NULL, .f_repr = NULL, .f_hash = NULL, \
    .f_getattr = NULL, .f_setattr = NULL, .f_getitem = NULL, .f_setitem = NULL, \
    .f_add = NULL, .f_sub = NULL, .f_mul = NULL, .f_div = NULL, .f_mod = NULL, .f_pow = NULL, \
    .f_lt = NULL, .f_le = NULL, .f_gt = NULL, .f_ge = NULL, .f_eq = NULL, .f_ne = NULL, 



/* module -> a kscript module */
typedef struct ks_module {
    KSO_BASE

    // the module's common name (i.e. "std", etc)
    ks_str name;

    // the elements of the module
    ks_dict __dict__;

}* ks_module;


// construct a new module with a given name
ks_module ks_module_new(ks_str name);

// create a new C-extension module with a given name
ks_module ks_module_new_c(char* name);

// load a named module
ks_module ks_load_module(ks_str mod_name);



// type describing the initialization function
typedef struct {

    ks_cfunc_sig f_init;

} ks_module_init_t;


/* token enum, tells the kinds of tokens */
enum {

    /* none/error token */
    KS_TOK_NONE = 0,

    /* combo token, combination of multiple token types */
    KS_TOK_COMBO,

    /* a newline, \n */
    KS_TOK_NEWLINE,
    /* the end of file token */
    KS_TOK_EOF,

    /* a comment, which is typically ignored */
    KS_TOK_COMMENT,

    /* an integer literal */
    KS_TOK_INT,
    /* a string literal */
    KS_TOK_STR,
    /* a variable identifier (this can also be a keyword in a language) */
    KS_TOK_IDENT,

    /* a literal comma, ',' */
    KS_TOK_COMMA,
    /* a literal colon, ':' */
    KS_TOK_COLON,
    /* a literal cdot, '.' */
    KS_TOK_DOT,
    /* a literal semicolon, ';' */
    KS_TOK_SEMI,

    /* left parenthesis, '(' */
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
    KS_TOK_O_POW,

    /* less-than operator, '<' */
    KS_TOK_O_LT,
    /* less-than-equal-to operator, '<=' */
    KS_TOK_O_LE,
    /* greater-than operator, '>' */
    KS_TOK_O_GT,
    /* greater-than-equal-to operator, '>=' */
    KS_TOK_O_GE,
    /* equal-to operator, '==' */
    KS_TOK_O_EQ,
    /* not-equal-to operator, '!=' */
    KS_TOK_O_NE,

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

// create a new token
ks_tok ks_tok_new(int ttype, ks_parser kp, int offset, int len, int line, int col);

// combine A and B to form a larger meta token
ks_tok ks_tok_combo(ks_tok A, ks_tok B);

/* parser -> an object which can be used to parse the grammar/language into ASTs, and bytecode objects */
struct ks_parser {
    KSO_BASE

    // the human readable name of the source.
    // If a file input, then this is just the name of the file given
    ks_str src_name;

    // the source of the entire file
    ks_str src;

    // number of tokens
    int n_toks;

    // array of tokens
    ks_tok* toks;

    // the current token index
    int tok_i;

};

// create a new parser from a file
ks_parser ks_parser_new_file(const char* fname);

// create a new parser from an expression
ks_parser ks_parser_new_expr(const char* expr);

// parse out an AST representing assembly
ks_ast ks_parse_asm(ks_parser self);

// parse an expression out of a parser
ks_ast ks_parse_expr(ks_parser self);

// parse a statement out of a parser
ks_ast ks_parse_stmt(ks_parser self);

// parse a whole program out of a parser
ks_ast ks_parse_program(ks_parser self);



/* enumeration of the types of AST */
enum {

    /* none/error AST */
    KS_AST_ERR = 0,


    /* means this AST represents the `true` constant */
    KS_AST_TRUE,
    /* means this AST represents the `false` constant */
    KS_AST_FALSE,
    /* means this AST represents the `none` constant */
    KS_AST_NONE,


    /* means this AST represents a constant integer value */
    KS_AST_INT,
    /* means this AST represents a constant string value */
    KS_AST_STR,
    /* means this AST represents a variable reference, which will be looked up */
    KS_AST_VAR,
    /* means this AST represents getting an attribute of an object, `A.attr` */
    KS_AST_ATTR,


    /* means this AST represents a tuple to be created */
    KS_AST_TUPLE,
    /* means this AST represents a list to be created */
    KS_AST_LIST,

    /* means this AST represents a function call with a given number of arguments */
    KS_AST_CALL,
    /* means this AST represents a subscript call */
    KS_AST_SUBSCRIPT,

    /* means this AST represents a conditional block */
    KS_AST_IF,
    /* means this AST represents a while-loop block */
    KS_AST_WHILE,
    /* means this AST represents a return statement */
    KS_AST_RET,

    /** binary operators **/

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
    /* pow: **, power */
    KS_AST_BOP_POW,
    /* lt: <, less than */
    KS_AST_BOP_LT,
    /* le: <=, less than or equal to */
    KS_AST_BOP_LE,
    /* gt: >, greater than */
    KS_AST_BOP_GT,
    /* ge: >=, greater than or equal to */
    KS_AST_BOP_GE,
    /* eq: ==, equal to */
    KS_AST_BOP_EQ,
    /* ne: !=, NOT equal to */
    KS_AST_BOP_NE,

    /* assign: =, special operator denoting a name setting */
    KS_AST_BOP_ASSIGN,

    /* means this AST represents a function-literal, aka a lambda */
    KS_AST_FUNC,
    /* means the AST represents a block of code (assembly code) to be ran */
    KS_AST_CODE,

    /* means this AST represents a list of other ASTs in a block */
    KS_AST_BLOCK,


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

        /* the value iff atype==KS_AST_INT,KS_AST_STR */
        kso v_val;

        /* the name of the variable iff atype==KS_AST_VAR */
        ks_str v_name;

        /* obj and attribute such that it represents `obj.attr`, iff atype==KS_AST_ATTR */
        struct {

            // the object to look up its attribute
            ks_ast obj;

            // the name of the attribute
            ks_str attr;

        } v_attr;

        /* list of ASTs iff atype==KS_AST_LIST,KS_AST_TUPLE,KS_AST_CALL,KS_AST_SUBSCRIPT,KS_AST_BLOCK 
        call: `f(A, B, C)` would have size 4, containing [f, A, B, C]
        */
        ks_list v_list;

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

// create a new AST representing the 'true' value
ks_ast ks_ast_new_true();
// create a new AST representing the 'false' value
ks_ast ks_ast_new_false();
// create a new AST representing the 'none' value
ks_ast ks_ast_new_none();

// create a new AST representing a constant int
ks_ast ks_ast_new_int(int64_t v_int);
// create a new AST representing a constant string
ks_ast ks_ast_new_str(ks_str v_str);

// create a new variable reference
ks_ast ks_ast_new_var(ks_str v_name);
// create a new attribute reference
ks_ast ks_ast_new_attr(ks_ast obj, ks_str v_attr);

// create a new AST representing a tuple of objects
ks_ast ks_ast_new_tuple(ks_ast* items, int n_items);
// create a new AST representing a list of objects
ks_ast ks_ast_new_list(ks_ast* items, int n_items);
// create a new AST representing a functor call, with `items[0]` being the function
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_call(ks_ast* items, int n_items);
// create a new AST representing an object subscript, with `items[0]` being the object iself
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_subscript(ks_ast* items, int n_items);

// create a new AST representing a binary operator, assumes `bop_type` is a valid binary operator
ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R);

// return a new AST representing an `if`
ks_ast ks_ast_new_if(ks_ast cond, ks_ast body);
// return a new AST representing a `while`
ks_ast ks_ast_new_while(ks_ast cond, ks_ast body);

// return a new ast representing a return
ks_ast ks_ast_new_ret(ks_ast val);

// returns a new AST representing a bytecode assembly segment
ks_ast ks_ast_new_code(ks_code code);
// return a new function representing a function literal
ks_ast ks_ast_new_func(ks_list params, ks_ast body);

// returns a new, empty block AST
ks_ast ks_ast_new_block_empty();
// returns a new block populated by the given arguments
ks_ast ks_ast_new_block(ks_ast* items, int n_items);



/* cfunc -> a type wrapping a C-function which can be called within kscript
*/
typedef struct ks_cfunc {
    KSO_BASE

    // the actual C function pointer, which is callable given a number of args and a list of arguments
    ks_cfunc_sig v_cfunc;

}* ks_cfunc;

// create a new C-function wrapper
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc);

// create a new C-function wrapper with a new reference
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc);




#endif