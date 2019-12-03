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
// reads from a file pointer, the entire file
void ks_str_readfp(ks_str* str, FILE* fp);

// we do the same as with `ks_obj`, having a pointer as the main type
typedef struct ks_ast* ks_ast;

// make the name `ks_obj` be a pointer to an internal structure
typedef struct ks_obj* ks_obj;

// the actual scope type for nested scopes, etc
typedef struct ks_scope* ks_scope;

// the global context type
typedef struct ks_ctx* ks_ctx;

// the bytecode program type
typedef struct ks_bc* ks_bc;



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
struct ks_bc;

// a structure describing how a function written in kscript is defined
typedef struct {

    // the bytecode collection in which it is defined
    struct ks_bc* bc;

    // the index into `bc`'s instructions
    int idx;

    // number of parameters for the function
    int params_n;

    // an array of formal names for the parameters
    ks_str* param_names;

} ks_kfunc;


#define KS_KFUNC_EMPTY ((ks_kfunc){ .bc = NULL, .idx = -1, .params_n = 0, .param_names = NULL })

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

    // builtin C-function type (of signature ksf_cfunc)
    KS_TYPE_CFUNC,

    // a kscript function object
    KS_TYPE_KFUNC,

    // a type representing a generic object (with a dictionary of properties)
    KS_TYPE_OBJECT,

    // a type representing a type (trippy!)
    //KS_TYPE_TYPE,

    // a type representing a module (should just be a dictionary)
    //KS_TYPE_MODULE,

    // this isn't a type, but is just the starting point for custom types. So you can test
    //   if `obj->type >= KS_TYPE_CUSTOM` to determine whether or not it is a built-in type
    //KS_TYPE_CUSTOM
    
};

// the internal storage of an object. However, most code should just use
//   `ks_obj` (no struct), as it will be a pointer.
struct ks_obj {

    // one of the `KS_TYPE_*` enum values, or a type-id (which will be > KS_TYPE_OBJECT if not a builtin)
    uint16_t type;

    // These will be used in the future; they will hold various info
    //   about the object, for GC, reference counting etc, but for now, will be 0
    uint16_t flags;

    // an anonymous tagged union, representing the data of the object
    union {
        // if type==KS_TYPE_NONE, there is no field

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

        /*

        // if type==KS_TYPE_TYPE, the dictionary representing the type
        ks_dict _type;

        // if type==KS_TYPE_MODULE, the dictionary representing the module
        ks_dict _module;

        */

        // if type==KS_TYPE_OBJECT, it has a generic dictionary of other objects
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
// return a new object type
ks_obj ks_obj_new_object(ks_dict dict);

// frees an object and its resources
void ks_obj_free(ks_obj obj);

/* module definition */

// structure representing how to load a module
struct ks_module_loader {

    // the one function that will load the module into context, returning an error code
    int (*f_load)(ks_ctx ctx, ks_obj module);

};

// construct a module loader
#define KS_MODULE_LOADER(_f_load) ((struct ks_module_loader){ .f_load = (_f_load) })

// loads a given module by name
ks_obj ks_module_load(ks_ctx ctx, ks_str name);

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

// types of bytecode instructions, with desciriptions of their VLE
// (variable length encoding)
enum {

    // how to read encodings:
    // [] means a single operation
    // [N] means how many bytes
    // ex:
    // [1:opcode] [2:...]
    // total of 3 bytes

    // do nothing
    // [1:opcode]
    KS_BC_NOOP = 0,
    // lookup and load a value, push onto stack
    // [1:opcode] [4:int const_str idx]
    KS_BC_LOAD,
    // pop off a value, and store it in the string value in the string
    //   constant table
    // [1:opcode] [4:int const_str idx]
    KS_BC_STORE,
    // pop an object, loop up its attribute, then push on the attribute
    // [1:opcode] [4:int const_str idx]
    KS_BC_ATTR,

    // push the 'none' constant to the stack
    // [1:opcode]
    KS_BC_CONST_NONE,
    // push an integer constant to the stack.
    // NOTE: limited to signed 64 bit
    // [1:opcode] [8:int64_t value]
    KS_BC_CONST_INT,
    // push a boolean 'true' to the stack
    // [1:opcode]
    KS_BC_CONST_BOOL_TRUE,
    // push a boolean 'false' to the stack
    // [1:opcode]
    KS_BC_CONST_BOOL_FALSE,
    // push a float constant to the stack
    // [1:opcode] [8:double value]
    KS_BC_CONST_FLOAT,
    // push a string constant to the stack
    // [1:opcode] [4:int const_str idx]
    KS_BC_CONST_STR,

    // pops off a number of arguments (should be an int)
    // then pops off a function
    // pops off that many arguments, and calls the function
    // so, `A B C f 3`, and vcall will result in `f(A, B, C)` (since there are 3 arguments)
    // `A B C f 2`, then vcall will result in `f(B, C)`
    // [1:opcode]
    KS_BC_VCALL,
    // pops off a function, then pops off `n_args` values as arguments, and executes
    // so stack will look like `A B C f`, and `call` will result in `f(A, B, C)`
    // [1:opcode] [4:int n_args]
    KS_BC_CALL,

    // jumps a relative amount, unconditionally
    // [1:opcode] [4:int relamt]
    KS_BC_JMP,
    // pops off a conditional (which should be a boolean), if true, add `relamt` to the program counter (in instructions)
    // [1:opcode] [4:int relamt]
    KS_BC_BEQT,
    // pops off a conditional (which should be a boolean), if false, add `relamt` to the program counter (in instructions)
    // [1:opcode] [4:int relamt]
    KS_BC_BEQF,

    // pops off a value, and returns it
    // [1:opcode]
    KS_BC_RET,

    // return from the exception handler (this should only be within exception handlers).
    // Normally, this does nothing, as exception handlers just keep executing after the try/catch block
    // [1:opcode]
    KS_BC_ERET,
    // pops off an argument and 'throws' an exception with it
    // [1:opcode]
    KS_BC_THROW

};

// bytecode operation
typedef uint8_t ks_bc_op;


// an entire bytecode program
struct ks_bc {


    // the list of instructions within the bytecode objects
    ks_bc_op* inst;

    // number of instructions allocated (including VLE instruction memory)
    int inst_n;


    // a list of constant strings
    ks_str* const_str;

    // number of constant strings
    int const_str_n;


};

// create a new (blank) bytecode program
ks_bc ks_bc_new();
// adds a new constant string to the bytecode program, returns the index
// NOTE: duplicates the string internally, so you can free(str)
int ks_bc_const_str_add(ks_bc bc, ks_str str);
// returns the index into the `const_str` array at which a string is stored,
// -1 if it is not in the constant strings
int ks_bc_const_str_idx(ks_bc bc, ks_str str);

// just append bytes to the end of the instruction data
// returns index at which the first byte was added
int ks_bc_append_bytes(ks_bc bc, void* data, int n);
// append a single byte to the end of instruction data (return index)
int ks_bc_append_uint8(ks_bc bc, uint8_t val);
// append a whole int (i.e. 4 bytes) onto inst data (return idx)
int ks_bc_append_int(ks_bc bc, int val);

// all these return the index at which they were added

/* adding instructions */

// add a 'noop' instruction
int ks_bc_noop(ks_bc bc);
// add a 'load' instruction from name
int ks_bc_load(ks_bc bc, ks_str name);
// add a 'store' instruction to a name
int ks_bc_load(ks_bc bc, ks_str name);
// add a 'attr' instruction of a name
int ks_bc_attr(ks_bc bc, ks_str name);

// add constant instruction
int ks_bc_const_none(ks_bc bc);
int ks_bc_const_bool_true(ks_bc bc);
int ks_bc_const_bool_false(ks_bc bc);
int ks_bc_const_int(ks_bc bc, ks_int val);
int ks_bc_const_float(ks_bc bc, ks_float val);
int ks_bc_const_str(ks_bc bc, ks_str val);

// add calls
int ks_bc_vcall(ks_bc bc);
int ks_bc_call(ks_bc bc, int args_n);

// jmp/branch instructions
int ks_bc_jmp(ks_bc bc, int relamt);
int ks_bc_beqt(ks_bc bc, int relamt);
int ks_bc_beqf(ks_bc bc, int relamt);

// return instructions
int ks_bc_ret(ks_bc bc);
int ks_bc_throw(ks_bc bc);
int ks_bc_eret(ks_bc bc);


// free a bytecode program and its resources
void ks_bc_free(ks_bc bc);

// executes bytecode (with offset) in a context
void ks_exec(ks_ctx ctx, ks_bc bc, int idx);

//#define KS_BC_EMPTY ((ks_bc){ .inst_len = 0, .inst = NULL, .labels_len = 0, .labels = NULL, .labels_idx = NULL })

// add an instruction, and return the index
/*int ks_bc_add(ks_bc* bc, ks_bc_inst inst);

// executes bitcode on a context
void ks_exec(ks_ctx ctx, ks_bc_inst* bc);
// executes a function, and returns the result
ks_obj ks_exec_kfunc(ks_ctx ctx, ks_kfunc kfunc, int args_n, ks_obj* args);
// adds a label to an index in the instructions
void ks_bc_label(ks_bc* bc, ks_str name, int lidx);

*/

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
#define ks_warn(...) ks_log(KS_LOGLVL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ks_error(...) ks_log(KS_LOGLVL_ERROR, __FILE__, __LINE__, __VA_ARGS__)


#endif

