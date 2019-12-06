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
#define ks_list_pop(_list) ((_list)->items[--(_list)->len])
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
#define KSO_BASE kso_type type; uint64_t flags;

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
      f_str
    ;


};

typedef struct {
    KSO_BASE
}* kso_none;

typedef struct {
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

    // pop on a literal boolean
    // 1[opcode] 1[ks_bool val]
    KS_BC_BOOL,

    // pop on a literal integer
    // 1[opcode] 8[ks_int val]
    KS_BC_INT,

    // pop on a float value
    // 1[opcode] 8[ks_float val]
    KS_BC_FLOAT,

    // load a variable by string name
    // (idx is an index into the `str_tbl`)
    // 1[opcode] 4[int idx(name)]
    KS_BC_LOAD,

    // pop off the last value, and store into a variable name
    // 1[opcode] 4[int idx(name)]
    KS_BC_STORE,

    // pop off the last value (which should be callable),
    // and then call with `n_args`
    // 1[opcode] 2[int16_t n_args]
    KS_BC_CALL,

    /* operations */

    // replace the top of the stack with its type
    // [1:opcode]
    KS_BC_TYPEOF,

    // pop off and 'return' the last item on the stack
    // [1:opcode]
    KS_BC_RET,

    // return None
    KS_BC_RETNONE,


    // the end of the bytecodes
    KS_BC__END

};

// each bytecode is 1 byte
typedef uint8_t ks_bc;

// a program, representing a bunch of instructions, and labels, as well as constants and required vals
typedef struct {

    // number of strings in the constant table
    int str_tbl_n;

    // the table of string constants used by bytecodes
    ks_str* str_tbl;

    // the current number of bytes (not neccesarily instructions) in `bc`
    // so, this is the index of the first free byte
    int bc_n;

    // the list of bytecode instructions
    ks_bc* bc;

} ks_prog;

// the empty program, used to initialize
#define KS_PROG_EMPTY ((ks_prog){ .str_tbl_n = 0, .str_tbl = NULL, .bc_n = 0, .bc = NULL })

/* add instructions (these all return the index at which the instruction was added) */

int ksb_noop(ks_prog* prog);
int ksb_bool(ks_prog* prog, ks_bool val);
int ksb_int(ks_prog* prog, ks_int val);
int ksb_load(ks_prog* prog, ks_str name);
int ksb_store(ks_prog* prog, ks_str name);
int ksb_call(ks_prog* prog, int16_t n_args);
int ksb_typeof(ks_prog* prog);
int ksb_ret(ks_prog* prog);
int ksb_retnone(ks_prog* prog);

// frees all resources associated with the program
void ks_prog_free(ks_prog* prog);


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

// defined in `types.c`, these are the global builtin types
extern kso_type
    kso_T_type,
    kso_T_none,
    kso_T_bool,
    kso_T_int, 
    kso_T_float,
    kso_T_str,
    kso_T_cfunc
;

extern kso_cfunc
    kso_F_print
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

