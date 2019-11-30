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
#include <math.h>

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
#define KS_STR_CONST(_charp) KS_STR_VIEW(_charp, strlen(_charp))

// copies `len` bytes from charp, and then NULL-terminate
void ks_str_copy_cp(ks_str* str, char* charp, int len);
// copies a string to another string
void ks_str_copy(ks_str* str, ks_str from);
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


// we do the same as with `ks_obj`, having a pointer as the main type
typedef struct ks_ast* ks_ast;

// make the name `ks_obj` be a pointer to an internal structure
typedef struct ks_obj* ks_obj;

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

// returns the index of the key into the dictionary, or -1 if the key doesn't exist within it
int ks_dict_geti(ks_dict* dict, ks_str key);
// sets dictionary at a given index, or if `idx` is -1, adds the value to the dictionary
// returns the index of the object added (same as `idx`, unless `idx` was -1)
int ks_dict_seti(ks_dict* dict, ks_str key, int idx, ks_obj val);
// sets the dictionary's value for a given key, and returns the index at which it is located now
int ks_dict_set(ks_dict* dict, ks_str key, ks_obj val);
// free's the dictionary and its resources
void ks_dict_free(ks_dict* dict);




// a C-function signature
typedef ks_obj (*ksf_cfunc)(int args_n, ks_obj* args);


// types of objects
enum {
    // the none-type, null-type, etc
    KS_TYPE_NONE = 0,

    // builtin integer type
    KS_TYPE_INT,

    // builtin floating point type
    KS_TYPE_FLOAT,

    // builtin string type
    KS_TYPE_STR,

    // builtin C-function type (of signature ksf_cfunc)
    KS_TYPE_CFUNC,

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
        // if type==KS_TYPE_FLOAT, the value
        ks_float _float;
        // if type==KS_TYPE_STR, the value
        ks_str _str;

        // if type==KS_TYPE_CFUNC, the function
        ksf_cfunc _cfunc;

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
// returns a new float with specified value
ks_obj ks_obj_new_float(ks_float val);
// returns a new string with specified value
ks_obj ks_obj_new_str(ks_str val);
// returns a new cfunc with specified value
ks_obj ks_obj_new_cfunc(ksf_cfunc val);
// returns a new custom-type object with a fresh dictionary
ks_obj ks_obj_new_custom();
// frees an object and its resources
void ks_obj_free(ks_obj obj);


/* module definition */

typedef struct {

} ks_module;



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

