/* kscript.h - main definitions for all kscript


*/

#ifndef KSCRIPT_H_
#define KSCRIPT_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct kso* kso;

// primitive/builtin C types
typedef bool    ks_bool;
typedef int64_t ks_int;
typedef double  ks_float;
typedef struct {
    // current length, and maximum length
    uint32_t len, max_len;
    // NUL-terminated char pointer
    char* _;
} ks_str;

// concatenates two string
// this represents the `NULL` string, which is also valid as a starting string
#define KS_STR_EMPTY ((ks_str){ ._ = NULL, .len = 0, .max_len = 0 })
// represents a view of a C-string. i.e. nothing is copied, and any modifications made
//   stay in the original string
#define KS_STR_VIEW(_charp, _len) ((ks_str){ ._ = (char*)(_charp), .len = (int)(_len), .max_len = (int)(_len) })
// useful for string constants, like `KS_STR_CONST("Hello World")`
#define KS_STR_CONST(_charp) KS_STR_VIEW(_charp, sizeof(_charp) - 1)
// creates a new string from a constant (i.e. allocates memory for it)
#define KS_STR_CREATE(_charp) (ks_str_dup(KS_STR_CONST(_charp)))

// str <- charp[len]
void ks_str_copy_cp(ks_str* str, char* charp, int len);
// str <- from
void ks_str_copy(ks_str* str, ks_str from);
// returns a copy of `from`
ks_str ks_str_dup(ks_str from);
// str <- str + A
void ks_str_append(ks_str* str, ks_str A);
// str <- str + c
void ks_str_append_c(ks_str* str, char c);
// frees `str`'s resources
void ks_str_free(ks_str* str);
// comparison, should be equivalent to `strcmp(A._, B._)`
int ks_str_cmp(ks_str A, ks_str B);
// yields whether the two strings are equal
#define ks_str_eq(_A, _B) (ks_str_cmp((_A), (_B)) == 0)
// set `str` to a formatted string (printf style)
int ks_str_fmt(ks_str* str, const char* fmt, ...);
// set `str` to a formatted string (printf style)
int ks_str_vfmt(ks_str* str, const char* fmt, va_list ap);
// reads from a file pointer, the entire file
void ks_str_readfp(ks_str* str, FILE* fp);

// list of object references
typedef struct {
    // current length, and maximum length
    uint32_t len, max_len;
    // array of items
    kso* items;
} ks_list;

#define KS_LIST_EMPTY ((ks_list){ .len = 0, .max_len = 0, .items = NULL })

// pops off an object, and returns that object
//#define ks_list_pop(_list) ((_list)->items[--(_list)->len])
kso ks_list_pop(ks_list* list);
// pushes an object reference to the list, returns the index
int ks_list_push(ks_list* list, kso obj);
// frees 'list's resources
void ks_list_free(ks_list* list);

// dictionary of object references
typedef struct {

    // current length, and maximum length
    uint32_t len, max_len;

    // a structure representing a single dictionary item
    struct ks_dict_item {

        // hash(key) -> saved once for performance reasons
        ks_int hash;

        // key, which must be internally of type int, float, str,
        // or in general, something constant
        kso key;

        // value, which can be of any type
        kso val;

    }* items;

} ks_dict;

// empty dictionary, can be used to initialize one
#define KS_DICT_EMPTY ((ks_dict){ .len = 0, .max_len = 0, .items = NULL })

// gets an object given a key, NULL if notfound
kso ks_dict_get(ks_dict* dict, kso key);
// gets an object given a string key, NULL if notfound
kso ks_dict_get_str(ks_dict* dict, ks_str key);

// sets the dictionary at a key, given a value (returns its index)
int ks_dict_set(ks_dict* dict, kso key, kso val);
// set the dictionary at a key (which is string), given a value (returns its index)
int ks_dict_set_str(ks_dict* dict, ks_str key, kso val);

// frees a dictionary and its resources
void ks_dict_free(ks_dict* dict);


/* object types */

// enum describing object flags, which are in the `KSO_BASE` of every object
enum {
    // the default for flags
    KSOF_NONE = 0,

    // if this flag is set, never free the object
    // useful for the global booleans, global integer constants, etc
    KSOF_IMMORTAL = 1 << 1

};


// the base parameters that should be at the start of any object
#define KSO_BASE kso_type type; int32_t refcnt; uint32_t flags;

typedef struct kso_type* kso_type;

/* primitive object definitions */
struct kso_type {
    KSO_BASE
    // the name of the type, i.e. 'int'
    ks_str name;

    /* type functions */
    kso 
      // init is used as a constructor, can take `self,...` as args
      f_init,
      // free should free all resources with 
      f_free,

      // returns the string representation
      f_repr,

      // builtin type conversion to bool
      f_bool,
      // builtin type conversion to int
      f_int,
      // builtin type conversion to str
      f_str,

      // builtin getting function
      f_get,
      // builtin setting function
      f_set
    ;


};

typedef struct kso_none {
    KSO_BASE
}* kso_none;

typedef struct kso_bool {
    KSO_BASE
    ks_bool _bool;
}* kso_bool;

typedef struct {
    KSO_BASE
    ks_int _int;
}* kso_int;

typedef struct {
    KSO_BASE
    ks_float _float;
}* kso_float;

typedef struct {
    KSO_BASE
    ks_str _str;
}* kso_str;

typedef struct {
    KSO_BASE
    ks_list _list;
}* kso_list;

typedef struct {
    KSO_BASE
    ks_dict _dict;
}* kso_dict;

typedef struct kso_cfunc {
    KSO_BASE
    kso (*_cfunc)(int n_args, kso* args);
}* kso_cfunc;


/* 
the base type of object, which all others must derive from
(i.e. be castable to)
so, every object definition must begin with `KSO_BASE`
This is the most generic object
*/
struct kso {
    KSO_BASE

};


// add a reference to the object
#define KSO_INCREF(_obj) { ++(_obj)->refcnt; }

// remove a reference to the object, freeing it if it reaches 0
#define KSO_DECREF(_obj) { if (--(_obj)->refcnt <= 0) { kso_free(_obj); } }

// cast `_obj` to `_type`, assumes its correct to do so
#define KSO_CAST(_type, _obj) ((_type)_obj)

// tries to call an object (which could be a cfunc, for instance)
// _success should be a boolean, which is set to true if it worked,
//   false if not
// _n_args should be an integer which tells how many args
// _args should be a pointer to the start of the arguments
// _res should be a `kso`, which will be assigned to the result of the function call
//   (or NULL if there was an error somewhere)
#define KSO_CALL(_success, _res, _obj, _n_args, _args) { \
    if ((_obj)->type == kso_T_cfunc) { \
        _res = KSO_CAST(kso_cfunc, _obj)->_cfunc((_n_args), (_args)); \
        _success = true; \
    } else { \
        _res = NULL; \
        _success = false; \
    } \
}



/* constructing new primitives */

kso kso_new_none();
kso kso_new_bool(ks_bool val);
kso kso_new_int(ks_int val);
kso kso_new_float(ks_float val);
// creates a copy of the data, so the new object doesn't rely on `val` at all
kso kso_new_str(ks_str val);
// creates a new string from a given format
kso kso_new_str_fmt(const char* fmt, ...);

// create list of references
kso kso_new_list(int n, kso* refs);

// call a function object, with arguments, and return its result
kso kso_call(kso func, int args_n, kso* args);


/* object manipulation */

// returns the object as a value. If obj is a primitive, (int, bool, etc...)
//   , then the object is duplicated, and a new value is returned
// else, (if it should be a reference type), `obj` is returned
// This function is useful for treating things as immutable value types, for example
//   when adding to a list
kso kso_asval(kso obj);

// free's an object's resources (through its type), and then frees `obj` itself
// This should only be done if you are sure no one else is using the object
void kso_free(kso obj);


/* bytecode format:
encoded with [SIZE: desc]
where SIZE is in bytes

So, this is variable-length bytecode, the arguments must
be consumed variabl-y

These are all executed on a stack, with a reference to a constant table
(for storing strings)

So, implicitly, they operate on the top of the stack

*/

enum {

    // do nothing
    // 1[opcode]
    KS_BC_NOOP = 0,


    // load a variable by string name
    // (idx is an index into the `str_tbl`)
    // 1[opcode] 4[int idx(name)]
    KS_BC_LOAD,

    // pop off the last value, and store into a variable name
    // 1[opcode] 4[int idx(name)]
    KS_BC_STORE,


    /* constructing values/primitives */

    // pop on a literal boolean (true or false)
    // 1[opcode]
    KS_BC_BOOLT,
    KS_BC_BOOLF,

    // pop on a literal integer
    // 1[opcode] 8[ks_int val]
    KS_BC_INT,

    // pop on a float value
    // 1[opcode] 8[ks_float val]
    KS_BC_FLOAT,

    KS_BC_STR,


    /* construct more primitives */

    // create a new list like: [A B]
    // uses the last `n_items` on the stack
    // 1[opcode] 2[int16_t n_items] 
    KS_BC_NEW_LIST,

    /* operators */

    // add the last two items
    // 1[opcode]
    KS_BC_ADD,

    // subtract the last two items
    // 1[opcode]
    KS_BC_SUB,
    
    // multiply the last two items
    // 1[opcode]
    KS_BC_MUL,

    // divide the last two items
    // 1[opcode]
    KS_BC_DIV,

    // compare the last two items <
    // 1[opcode]
    KS_BC_LT,


    /* func calls */

    // pop off the last value (which should be callable),
    // and then call with `n_args`
    // 1[opcode] 2[int16_t n_args]
    KS_BC_CALL,

    // basically does the subscript of the last object (a[])
    // so, a[b, c, d] would be:
    // b c d a [get 3]
    // 1[opcode] 2[int16_t n_args]
    KS_BC_GET,

    // basically does the subscript-equals of the last object (a[]=...)
    // so, a[b, c] = d would be a.__set__(b, c, d)
    // b c d a [set 3]
    // 1[opcode] 2[int16_t n_args]
    KS_BC_SET,

    // jump unconditionally
    // 1[opcode] 4[int relamt]
    KS_BC_JMP,
    
    // jump conditionally (if the last item is a bool which is true)
    // 1[opcode] 4[int relamt]
    KS_BC_JMPT,

    // jump conditionally (if the last item is a bool which is false)
    // 1[opcode] 4[int relamt]
    KS_BC_JMPF,

    /* operations */

    // replace the top of the stack with its type
    // [1:opcode]
    KS_BC_TYPEOF,

    // pop off and 'return' the last item on the stack
    // [1:opcode]
    KS_BC_RET,

    // return None
    // [1:opcode]
    KS_BC_RETNONE,

    // discards the top item
    // 1[opcode]
    KS_BC_DISCARD,

    // the end of the bytecodes
    KS_BC__END

};

// each bytecode is 1 byte
typedef uint8_t ks_bc;

struct ks_bc {
    ks_bc op;
};

/* instruction definitions */

struct ks_bc_noop {
    // always KS_BC_NOOP
    ks_bc op;
};

struct ks_bc_load {
    // always KS_BC_LOAD
    ks_bc op;
    
    // index of the name into the str_tbl of its program
    uint16_t name_idx;

};

struct ks_bc_store {
    // always KS_BC_STORE
    ks_bc op;
    
    // index of the name into the str_tbl of its program
    uint16_t name_idx;

};

/* constants */

struct ks_bc_int {
    // always KS_BC_INT
    ks_bc op;

    // value of the integer
    ks_int val;
};

struct ks_bc_float {
    // always KS_BC_FLOAT
    ks_bc op;

    // value of the integer
    ks_int val;
};

struct ks_bc_str {
    // always KS_BC_STR
    ks_bc op;

    // index of the value into the str_tbl of the program
    uint16_t val_idx;
};

/* constructing/calling */

struct ks_bc_new_list {
    // always KS_BC_NEW_LIST
    ks_bc op;

    // number of items on the stack to take
    uint16_t n_items;
};

struct ks_bc_call {
    // always KS_BC_CALL
    ks_bc op;
    
    // number of items on the stack to take as arguments
    // (not counting the function that is popped off first)
    uint16_t n_args;
};

struct ks_bc_get {
    // always KS_BC_GET
    ks_bc op;
    
    // number of items on the stack to take as arguments
    // (not counting the object that is popped off first)
    uint16_t n_args;
};

struct ks_bc_set {
    // always KS_BC_SET
    ks_bc op;
    
    // number of items on the stack to take as arguments
    // (not counting the object that is popped off first)
    uint16_t n_args;
};


/* conditionals */

struct ks_bc_jmp {
    // KS_BC_JMP or KS_BC_JMPT or KS_BC_JMPTF
    ks_bc op;

    // relative amount to jump if the last item was true, or false, or always
    int32_t relamt;
};


// a program, representing a bunch of instructions, and labels, as well as constants and required vals
typedef struct {

    // number of strings in the constant table
    int str_tbl_n;

    // the table of string constants used by bytecodes
    ks_str* str_tbl;


    // number of string labels defined
    int lbl_n;

    // array of labels
    struct ks_prog_lbl {
        ks_str name;
        int idx;
    }* lbls;

    // the current number of bytes (not neccesarily instructions) in `bc`
    // so, this is the index of the first free byte
    int bc_n;

    // the list of bytecode instructions
    ks_bc* bc;

} ks_prog;

// the empty program, used to initialize
#define KS_PROG_EMPTY ((ks_prog){ .str_tbl_n = 0, .str_tbl = NULL, .lbl_n = 0, .lbls = NULL, .bc_n = 0, .bc = NULL })

/* add instructions (these all return the index at which the instruction was added) */

int ksb_noop(ks_prog* prog);
int ksb_bool(ks_prog* prog, ks_bool val);
int ksb_int(ks_prog* prog, ks_int val);
int ksb_float(ks_prog* prog, ks_float val);
int ksb_str(ks_prog* prog, ks_str val);
int ksb_load(ks_prog* prog, ks_str name);
int ksb_store(ks_prog* prog, ks_str name);
int ksb_add(ks_prog* prog);
int ksb_sub(ks_prog* prog);
int ksb_mul(ks_prog* prog);
int ksb_div(ks_prog* prog);
int ksb_lt(ks_prog* prog);

int ksb_call(ks_prog* prog, uint32_t n_args);
int ksb_get(ks_prog* prog, uint32_t n_args);
int ksb_set(ks_prog* prog, uint32_t n_args);
int ksb_new_list(ks_prog* prog, uint32_t n_items);
int ksb_jmp(ks_prog* prog, int32_t relamt);
int ksb_jmpt(ks_prog* prog, int32_t relamt);
int ksb_jmpf(ks_prog* prog, int32_t relamt);
int ksb_discard(ks_prog* prog);

int ksb_typeof(ks_prog* prog);
int ksb_ret(ks_prog* prog);
int ksb_retnone(ks_prog* prog);


// adds a label to a program (returns the index in the `lbls` array it was added at)
int ks_prog_lbl_add(ks_prog* prog, ks_str name, int idx);

// returns the offset from `prog->bc` where the label is defined
int ks_prog_lbl_i(ks_prog* prog, ks_str name);

// returns the string representation of the bytecode
int ks_prog_tostr(ks_prog* prog, ks_str* to);

// frees all resources associated with the program
void ks_prog_free(ks_prog* prog);

/* parsing implementation */


typedef struct ks_token ks_token;

// the struct responsible for a parsing state
typedef struct {

    // the human readable name for the source.
    // For files, it is that file name (i.e. "file.kscript")
    // For stdin, it is typically "-",
    // Or, for lines of an interpreter it may be "-#LINENUM"
    ks_str src_name;

    // the full string of the source. This doesn't change over the whole iteration
    ks_str src;

    // the current error string, which will be set if there was some problem parsing
    ks_str err;

    // structure describing the current state of the parser, assumes `src` hasn't changed
    struct ks_parse_state {
        // current position in the input string
        int line, col;

        // current index into `src._`, so this is just the linear index without caring about newlines, etc
        int i;

    } state;

    // number of tokens, including the EOF token
    int tokens_n;

    // current index of the tokens array
    int token_i;

    // the list of tokens
    ks_token* tokens;

} ks_parse;


// a single token of input
struct ks_token {

    // the type of token,
    enum {
        // invalid token
        KS_TOK_NONE = 0,

        // valid identifier, [a-zA-Z_][a-zA-Z_0-9]*
        KS_TOK_IDENT,

        // valid integer: [0-9]+
        KS_TOK_INT,

        // valid string literal, ".*"
        KS_TOK_STR,

        // a comment token, starts with `#`
        KS_TOK_COMMENT,

        // a literal colon ':'
        KS_TOK_COLON,
        // a literal semicolon ';'
        KS_TOK_SEMI,
        // literal dot '.'
        KS_TOK_DOT,
        // a newline
        KS_TOK_NEWLINE,

        /* operators */

        // literal tilde `~`
        KS_TOK_U_TIL,

        // literal add `+`
        KS_TOK_O_ADD,
        // literal sub `-`
        KS_TOK_O_SUB,
        // literal mul `*`
        KS_TOK_O_MUL,
        // literal div `/`
        KS_TOK_O_DIV,
        // literal less than `<`
        KS_TOK_O_LT,
        // literal greater than `>`
        KS_TOK_O_GT,
        // literal equals `==`
        KS_TOK_O_EQ,


        KS_TOK__NUM

    } type;


    // the state at which the token appears
    struct ks_parse_state state;

    // the length of the token
    int len;

};


// the empty, default token
#define KS_TOK_EMPTY ((ks_token){ .type = KS_TOK_NONE, .state = KS_PARSE_STATE_ERROR, .len = -1 })

// the 'beginning' parse state
#define KS_PARSE_STATE_BEGINNING ((struct ks_parse_state){ .line = 0, .col = 0, .i = 0 })

// there was an error
#define KS_PARSE_STATE_ERROR ((struct ks_parse_state){ .line = -1, .col = -1, .i = -1 })

// the empty parser, which is a valid initializer for it
#define KS_PARSE_EMPTY ((ks_parse){ .src_name = KS_STR_EMPTY, .src = KS_STR_EMPTY, .err = KS_STR_EMPTY, .state = KS_PARSE_STATE_BEGINNING, .tokens_n = 0, .tokens = NULL, .token_i = 0 })



// set the current source, and resets the state to the beginning
int ks_parse_setsrc(ks_parse* kp, ks_str src_name, ks_str src);

// sets the parser error, given a token and format args (doesn't display anything,
//   but now you can check kp->err)
// this will fill it out, and if token is not `KS_TOK_EMPTY`, add useful information
//   about where the error occured in relation to the source code
int ks_parse_err(ks_parse* kp, ks_token tok, const char* fmt, ...);


// parses out an entire bytecode program
int ks_parse_bc(ks_parse* kp, ks_prog* to);


// frees the parser's internal resources
void ks_parse_free(ks_parse* kp);



/* virtual machine state */

// virtual machine
typedef struct {

    // the global dictionary
    ks_dict dict;

    // the global stack
    ks_list stk;

} ks_vm;

// the empty vm, can be used to initialize a VM
#define KS_VM_EMPTY ((ks_vm){ .dict = KS_DICT_EMPTY, .stk = KS_LIST_EMPTY })

// frees all the virtual machine's resources
void ks_vm_free(ks_vm* vm);


/* globals/builtins */

// defined in `builtin.c`, these are the global builtin types
extern kso_type
    kso_T_type,
    kso_T_none,
    kso_T_bool,
    kso_T_int, 
    kso_T_float,
    kso_T_str,
    kso_T_cfunc,
    kso_T_list
;


extern kso_none
    kso_V_none
;


// defined in `builtin.c`, these are the global builtin values/constant
extern kso_bool
    kso_V_true,
    kso_V_false
;

// defined in `builtin.c`, these are the global builtin functions
extern kso_cfunc
    // misc
    kso_F_print,
    kso_F_exit,

    // getting/setting
    kso_F_get,
    kso_F_set,

    // math/operator builtins
    kso_F_add,
    kso_F_sub,
    kso_F_mul,
    kso_F_div,
    
    // comparison
    kso_F_lt,
    kso_F_gt,
    kso_F_eq

;


/* execution */

// execute a program (starting at `idx` in the bytecode) onto a VM
kso ks_exec(ks_vm* vm, ks_prog* prog, int idx);


/* utils, like hashing, etc */

// hashes `n` bytes
ks_int ks_hash_bytes(uint8_t* data, int n);

// returns the hash of a bool
ks_int ks_hash_bool(ks_bool val);

// returns the hash of an integer
ks_int ks_hash_int(ks_int val);

// returns the hash of a float
ks_int ks_hash_float(ks_float val);

// returns the hash of a string
ks_int ks_hash_str(ks_str str);

// hashes a generic object
ks_int ks_hash_obj(kso obj);


/* error handling */

// adds a string error message to the error stack
// returns NULL
void* ks_err_add_str(ks_str msg);

// constructs an error from a string format and arguments
// returns NULL
void* ks_err_add_str_fmt(const char* fmt, ...);

// returns the number of errors (0 if none)
int ks_err_N();

// pops an error off the stack
// returns NULL if there were no errors, and doesn't affect anything else
kso ks_err_pop();


/* memory management routines */

void* ks_malloc(size_t bytes);

void* ks_realloc(void* ptr, size_t bytes);

void ks_free(void* ptr);

size_t ks_memuse();

/* logging */

// levels of logging
enum {
    KS_LOGLVL_TRACE = 0,
    KS_LOGLVL_DEBUG,
    KS_LOGLVL_INFO,
    KS_LOGLVL_WARN,
    KS_LOGLVL_ERROR,
    KS_LOGLVL__END
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

