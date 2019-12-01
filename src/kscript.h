// src/kscript.h - header file for the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#ifndef KSCRIPT_H_
#define KSCRIPT_H_

// standard header files
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <dlfcn.h>


// the boolean type
typedef bool ks_bool;

// the main integer type in `kscript` (signed, 64 bit)
typedef int64_t ks_int;

// main floating point type in `kscript`. I like `double` because it has more accuracy
//   by default
typedef double ks_float;

// string type for `kscript`. It's based off code I wrote for EZC, almost exactly
typedef struct {

    // this is the internal C-style null-terminated string (which may be `NULL`)
    char* _;

    // this is the length of the string (not including NULL-terminator)
    int len;

    // this is the maximum length the string has been, so we don't resize until necessary
    int max_len;

} ks_str;

// this represents the `NULL` string, which is also valid as a starting string
#define KS_STR_EMPTY ((ks_str){ ._ = NULL, .len = 0, .max_len = 0 })

// represents a view of a C-string. i.e. nothing is copied, and any modifications made
//   stay in the original string
#define KS_STR_VIEW(_charp, _len) ((ks_str){ ._ = (char*)(_charp), .len = (int)(_len), .max_len = (int)(_len) })

// useful for string constants, like `KS_STR_CONST("Hello World")`
#define KS_STR_CONST(_charp) KS_STR_VIEW(_charp, sizeof(_charp) - 1)


// copies `len` bytes from charp, and then NULL-terminate
void ks_str_copy_cp(ks_str* str, char* charp, int len);
// copies a string to another string
void ks_str_copy(ks_str* str, ks_str from);
// duplicates a string, returning new memory
ks_str ks_str_dup(ks_str from);
// concatenates two strings into another one
void ks_str_concat(ks_str* str, ks_str A, ks_str B);
// appends an entire string
void ks_str_append(ks_str* str, ks_str A);
// appends a character to the string
void ks_str_append_c(ks_str* str, char c);
// frees a string and its resources
void ks_str_free(ks_str* str);
// compares two strings, should be equivalent to `strcmp(A._, B._)`
int ks_str_cmp(ks_str A, ks_str B);
// whether or not the two strings are equal
#define ks_str_eq(_A, _B) (ks_str_cmp((_A), (_B)) == 0)
// get the results of printf/scanf as a ks_str, with automatic memory allocation
ks_str ks_str_fmt(const char* fmt, ...);
// get the results, but using a va_list
ks_str ks_str_vfmt(const char* fmt, va_list ap);

// we do the same as with `ks_obj`, having a pointer as the main type
typedef struct ks_ast* ks_ast;

// make the name `ks_obj` be a pointer to an internal structure
typedef struct ks_obj* ks_obj;

// the actual scope type for nested scopes, etc
typedef struct ks_scope* ks_scope;

// the global context type
typedef struct ks_ctx* ks_ctx;



enum {
    // none-type, shouldn't exist
    KS_AST_NONE = 0,

    // a constant int literal
    KS_AST_CONST_INT,

    // a constant float literal
    KS_AST_CONST_FLOAT,

    // a constant string literal
    KS_AST_CONST_STR,

    // a constant of a generic type; just holds an object
    KS_AST_CONST,

    // assigns a value to an AST
    KS_AST_ASSIGN,

    // a variable reference
    KS_AST_VAR,

    // a 'call', i.e. some sort of method like A(B, C)
    KS_AST_CALL

};

// internal structure, use `ks_ast` as a pointer to it
struct ks_ast {

    uint16_t type;

    // anonymous tagged union
    union {
        // if type==KS_AST_CONST_INT, this is that value
        ks_int _int;
        // if type==KS_AST_CONST_FLOAT, this is that value
        ks_float _float;
        // if type==KS_AST_CONST_STR, this is that value
        ks_str _str;
        // if type==KS_AST_CONST, this is that value
        ks_obj _const;
        // if type==KS_AST_VAR, this is the name the code references
        ks_str _var;

        // if type==KS_AST_CALL, this contains the relevant details
        struct {
            // example: A(B, C)
            // lhs = A
            // args = B, C (len=2)
            
            // the object being called
            ks_ast lhs;

            // number of arguments
            int args_n;

            // an array of arguments
            ks_ast* args;

        } _call;

        // if type==KS_AST_ASSIGN, this contains the relevant details
        struct {
            // example: A = B
            // lhs = A
            // rhs = B

            // the thing being assigned to.
            // NOTE: This has some restrictions, it must be a valid assignable expression
            //   such as a KS_AST_VAR
            ks_ast lhs;

            // any value-holding type of AST
            ks_ast rhs;

        } _assign;

    };
};

// constructs new ast's
ks_ast ks_ast_new_int(ks_int val);
ks_ast ks_ast_new_float(ks_float val);
ks_ast ks_ast_new_str(ks_str val);
ks_ast ks_ast_new_const(ks_obj val);
ks_ast ks_ast_new_var(ks_str name);
ks_ast ks_ast_new_call(ks_ast lhs, int args_n, ks_ast* args);
ks_ast ks_ast_new_assign(ks_ast lhs, ks_ast rhs);

// frees resources
void ks_ast_free(ks_ast ast);


// kscript stack, holds a stack of ks_obj's
typedef struct {
    
    // number of entries
    int len;

    // the maximum numbers of entries
    int max_len;

    // the array of current values
    ks_obj* vals;


} ks_stk;

// the empty, starting stack
#define KS_STK_EMPTY ((ks_stk){ .len = 0, .max_len = 0, .vals = NULL })

// pushes on an object to the stack, then returns the index
int ks_stk_push(ks_stk* stk, ks_obj obj);
// pops off an object from the stack, and returns it
ks_obj ks_stk_pop(ks_stk* stk);
// frees a stack and all its resources
void ks_stk_free(ks_stk* stk);

// kscript dictionary, translates ks_str->ks_obj's 
typedef struct {

    // number of entries
    int len;

    // the maximum number of entries
    int max_len;

    // a list of keys
    ks_str* keys;

    // list of their values
    ks_obj* vals;

} ks_dict;

// the empty, starting dictionary
#define KS_DICT_EMPTY ((ks_dict){ .len = 0, .max_len = 0, .keys = NULL, .vals = NULL })

// a key-value pair
struct ks_dict_kvp {
    ks_str key;
    ks_obj val;
};

#define KS_DICT_KVP(_key, _val) ((struct ks_dict_kvp){ .key = (ks_str)(_key), (ks_obj)(_val) })

// a key value pair, but using a char pointer as the key
struct ks_dict_kvp_cp {
    const char* key;
    ks_obj val;
};

// returns the index of the key into the dictionary, or -1 if the key doesn't exist within it
int ks_dict_geti(ks_dict* dict, ks_str key);
// returns the object given a key, or NULL if it doesnt exist
ks_obj ks_dict_get(ks_dict* dict, ks_str key);
// sets dictionary at a given index, or if `idx` is -1, adds the value to the dictionary
// returns the index of the object added (same as `idx`, unless `idx` was -1)
int ks_dict_seti(ks_dict* dict, ks_str key, int idx, ks_obj val);
// sets the dictionary's value for a given key, and returns the index at which it is located now
int ks_dict_set(ks_dict* dict, ks_str key, ks_obj val);
// free's the dictionary and its resources
void ks_dict_free(ks_dict* dict);
// constructs, and allocates a dictionary from key-value pairs
ks_dict ks_dict_fromkvp(int len, struct ks_dict_kvp* kvp);
ks_dict ks_dict_fromkvp_cp(int len, struct ks_dict_kvp_cp* kvp_cp);


// a C-function signature
typedef ks_obj (*ksf_cfunc)(ks_ctx ctx, int args_n, ks_obj* args);

struct ks_bc_inst;


// a structure describing how a function written in kscript is defined
typedef struct {

    // a pointer to the start of the instructions
    struct ks_bc_inst* inst;
    //ks_str label;


    // number of parameters it takes
    int params_n;

    // parameter names it takes (these are just used because we need to know what names
    //   to set in the local scope on function entry)
    ks_str* param_names;

} ks_kfunc;

#define KS_KFUNC_EMPTY ((ks_kfunc){ .inst = NULL, .params_n = 0, .param_names = NULL })

// adds a parameter, and returns its index
int ks_kfunc_add_param(ks_kfunc* kfunc, ks_str name);


// types of objects
enum {
    // the none-type, null-type, etc
    KS_TYPE_NONE = 0,

    // builtin integer type
    KS_TYPE_INT,

    // builtin boolean type
    KS_TYPE_BOOL,

    // builtin floating point type
    KS_TYPE_FLOAT,

    // builtin string type
    KS_TYPE_STR,

    // builtin exception type, has a message
    KS_TYPE_EXCEPTION,

    // builtin C-function type (of signature ksf_cfunc)
    KS_TYPE_CFUNC,

    // a kscript function object
    KS_TYPE_KFUNC,

    // a type representing a type (trippy!)
    KS_TYPE_TYPE,

    // this isn't a type, but is just the starting point for custom types. So you can test
    //   if `obj->type >= KS_TYPE_CUSTOM` to determine whether or not it is a built-in type
    KS_TYPE_CUSTOM
    
};

// the internal storage of an object. However, most code should just use
//   `ks_obj` (no struct), as it will be a pointer.
struct ks_obj {

    // one of the `KS_TYPE_*` enum values
    uint16_t type;

    // These will be used in the future; they will hold various info
    //   about the object, for GC, reference counting etc, but for now, will be 0
    uint16_t flags;

    // an anonymous tagged union
    union {

        // if type==KS_TYPE_INT, the value
        ks_int _int;
        // if type==KS_TYPE_BOOL, the value
        ks_bool _bool;

        // if type==KS_TYPE_FLOAT, the value
        ks_float _float;
        // if type==KS_TYPE_STR, the value
        ks_str _str;

        // if type==KS_TYPE_EXCEPTION, the message
        ks_str _exception;

        // if type==KS_TYPE_CFUNC, the function
        ksf_cfunc _cfunc;

        // if type==KS_TYPE_KFUNC, the function
        ks_kfunc _kfunc;

        // if type==KS_TYPE_TYPE, the dictionary representing the type
        ks_dict _type;

        // if type>=KS_TYPE_CUSTOM, it just has a dictionary of values that it keeps updated
        ks_dict _dict;

        // misc. usage
        void* _ptr;

    };
};

// returns a new none object
ks_obj ks_obj_new_none();
// returns a new integer with specified value
ks_obj ks_obj_new_int(ks_int val);
// returns a new boolean with specified value
ks_obj ks_obj_new_bool(ks_bool val);
// returns a new float with specified value
ks_obj ks_obj_new_float(ks_float val);
// returns a new string with specified value
ks_obj ks_obj_new_str(ks_str val);
// returns a new exception with a specified message
ks_obj ks_obj_new_exception(ks_str message);
// returns a new exception, but with a format (like printf)
ks_obj ks_obj_new_exception_fmt(const char* fmt, ...);
// returns a new cfunc with specified value
ks_obj ks_obj_new_cfunc(ksf_cfunc val);
// returns a new kfunc with a specified value
ks_obj ks_obj_new_kfunc(ks_kfunc val);
// returns a new type-type
ks_obj ks_obj_new_type();
// returns a new type-type, initialized with a dictionary
ks_obj ks_obj_new_type_dict(ks_dict dict);
// returns a new custom-type object with a fresh dictionary
ks_obj ks_obj_new_custom();
// frees an object and its resources
void ks_obj_free(ks_obj obj);

/* module definition */

typedef struct {

} ks_module;


// structure representing how to load a module
struct ks_module_loader {

    // the one function that will load the module into context, returning an error code
    int (*f_load)(ks_ctx ctx);

};

// construct a module loader
#define KS_MODULE_LOADER(_f_load) ((struct ks_module_loader){ .f_load = (_f_load) })


// scope internal structure
struct ks_scope {
    // the parent scope (NULL if it is the global scope)
    ks_scope parent;

    //TODO: perhaps add meta-data about the scope?

    // a dictionary of local objects, which the scope owns
    ks_dict locals;

    // TODO: should imports be different than locals?

};

// constructs a new scope (parent can be NULL, or the parent scope which)
ks_scope ks_scope_new(ks_scope parent);

// frees a scope and its resources
void ks_scope_free(ks_scope scope);

// internal global context structure
struct ks_ctx {
    // number of items on the call stack
    int call_stk_n;

    // the stack of current scopes (of length call_stk_n)
    ks_scope* call_stk_scopes;

    ks_str* call_stk_names;

    // list of global types

    // number of types
    int types_n;

    // list of type names
    ks_str* type_names;

    ks_obj* types;


    // TODO: full types



    // the global stack
    ks_stk stk;

    // the C exception. Since in C, the control is handed over to C, the function
    //   simply creates a new object and assigns it to the context. When the context
    //   returns back into the interop loop, it checks whether this is null, if it isn't
    //   , it knows an exception has been thrown
    ks_obj cexc;

};

// create a new global context
ks_ctx ks_ctx_new();

// push on a scope to the current context, and return its index
int ks_ctx_push(ks_ctx ctx, ks_str name, ks_scope scope);
// pops off a context
ks_scope ks_ctx_pop(ks_ctx ctx);
// resolve a symbol in a context
ks_obj ks_ctx_resolve(ks_ctx ctx, ks_str name);
// creates a new type, and returns the typeid
int ks_ctx_new_type(ks_ctx ctx, ks_str name, ks_obj type);


/* bytecode: the semi-compiled portion of kscript */

// types of bytecode instructions
enum {
    // do nothing
    KS_BC_NOOP = 0,

    // load a variable by a given name, push it on the stack
    KS_BC_LOAD,

    // assign a variable to the last item on the stack (popping that item off)
    KS_BC_STORE,

    // return the last item on the stack
    KS_BC_RET,

    // return nothing
    KS_BC_RET_NONE,

    // pops on a constant integer
    KS_BC_CONST_INT,

    // pops on a constant float
    KS_BC_CONST_FLOAT,

    // pops on a constant string
    KS_BC_CONST_STR,


    // jumps an immediate amount (see ._jmpi)
    KS_BC_JMPI,

    // pops off a conditional (which must be an `bool` type),
    // and jumps if it is true. If the last item was not a bool, it is treated as false
    KS_BC_JMPC,

    // pops off 1 argument (the function), and then pops off `args_n` arguments
    //   (see ._call in the union), and calls the function
    // so:
    // A B C f call! would find `f`, and call it like `f(A, B, C)`
    KS_BC_CALL,

    // pops off an argument and 'throws' an exception with it
    KS_BC_THROW,

    // return from the exception handler (this should only be within exception handlers).
    // Normally, this does nothing, as exception handlers just keep executing after the try/catch block
    KS_BC_ERET,

    // exit, abort the entire program, without exception handling.
    // takes a single integer off the stack
    KS_BC_EXIT

};

// a bytecode instruction
struct ks_bc_inst {

    uint16_t type;

    // tagged union
    union {
        // if type==KS_BC_LOAD, this is the string of the name to load
        ks_str _load;
        // if type==KS_BC_STORE, this is the string of the name to assign to
        ks_str _store;
        // if type==KS_BC_CONST_INT, this is the value
        ks_int _int;
        // if type==KS_BC_CONST_FLOAT, this is the value
        ks_float _float;
        // if type==KS_BC_CONST_STR, this is the value
        ks_str _str;
        // if type==KS_BC_JMPI, this is the (relative) number it jumps
        //   if `_jmpi==0`, then nothing should happen
        int _jmpi;
        // if type==KS_BC_JMPC, this is the (relative) number it jumps if the last item on the stack
        //   was a bool & its value was true
        //   if `_jmpc==0` or the condition is false, then nothing should happen
        int _jmpc;

        struct {
            // how many args was it called with?
            int args_n;
        } _call;

    };

};

typedef struct ks_bc_inst ks_bc_inst;

// constructing bytecodes
ks_bc_inst ks_bc_new_load(ks_str name);
ks_bc_inst ks_bc_new_store(ks_str name);
ks_bc_inst ks_bc_new_ret();
ks_bc_inst ks_bc_new_ret_none();
ks_bc_inst ks_bc_new_int(ks_int val);
ks_bc_inst ks_bc_new_float(ks_float val);
ks_bc_inst ks_bc_new_str(ks_str val);
ks_bc_inst ks_bc_new_jmpi(int relamt);
ks_bc_inst ks_bc_new_jmpc(int relamt);
ks_bc_inst ks_bc_new_call(int args_n);
ks_bc_inst ks_bc_new_throw();
ks_bc_inst ks_bc_new_exit();

void ks_bc_inst_free(ks_bc_inst* inst);


// a collection of bytecode
typedef struct {

    // number of instructions
    int len;

    // a list of instructions
    ks_bc_inst* inst;

} ks_bc;

#define KS_BC_EMPTY ((ks_bc){ .len = 0, .inst = NULL })

// add an instruction, and return the index
int ks_bc_add(ks_bc* bc, ks_bc_inst inst);

// executes bitcode on a context
void ks_exec(ks_ctx ctx, ks_bc_inst* bc);
// executes a function, and returns the result
ks_obj ks_exec_kfunc(ks_ctx ctx, ks_kfunc kfunc, int args_n, ks_obj* args);



/* logging */

// levels of logging
enum {
    KS_LOGLVL_TRACE = 0,
    KS_LOGLVL_DEBUG,
    KS_LOGLVL_INFO,
    KS_LOGLVL_WARN,
    KS_LOGLVL_ERROR
};

// returns current log level
int ks_get_loglvl();
// sets the level
void ks_set_loglvl(int new_lvl);
// logs with a given level (i.e. KS_LOGLVL_*). however, use the macros
//   `ks_info(...)`, etc for easier use
void ks_log(int level, const char *file, int line, const char* fmt, ...);

// per-level macros
#define ks_trace(...) ks_log(KS_LOGLVL_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define ks_debug(...) ks_log(KS_LOGLVL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define ks_info(...) ks_log(KS_LOGLVL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define ks_warn(...) kl_log(KS_LOGLVL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ks_error(...) ks_log(KS_LOGLVL_ERROR, __FILE__, __LINE__, __VA_ARGS__)


#endif

