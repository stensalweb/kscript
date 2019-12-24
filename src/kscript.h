/* kscript.h - header file for the kscript library & compiler

kscript is a highly dynamic, duck-typed, language. It emphasizes shortness, 
the fewest boilerplate parts possible, and direct readability (such as usage 
of operators rather than functions, when possible)

Included are also implementations of lists, dictionaries, string operations, 
    a type system, stacks, a virtual machine

Which are more or less re-usable for other purposes. I am attempting to make them efficient

https://chemicaldevelopment.us
Cade Brown, 2019

*/

#ifndef KSCRIPT_H_
#define KSCRIPT_H_

/* standard headers */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>

#include <string.h>

/* primitive, builtin C types */


// each bytecode is 1 byte
typedef uint8_t ks_bc;

// boolean type, 1 for true, 0 for false
typedef bool    ks_bool;

// integer type (native 64 bit integer type, specifically), represents positive and
//   negative values, and can easily be handled by C applications
// in the future, there may be another integer type which is an arbitrary
//   precision using GMP/MPZ/a new library I write
typedef int64_t ks_int;

// what should a hash function return?
typedef ks_int ks_hash_t;

// floating point type (native C double, specifically), which is good enough for most applications
// in the future, there may be another float type which has bigger precision, or arbitrary precision
typedef double  ks_float;

// the string type, both NUL and length encoded
typedef struct {
    // the characters of the string, NUL terminated
    char* _;

    // the length of the string, not including the NUL-terminator
    uint32_t len, max_len;

} ks_str;


// this represents the `NULL` string, which is also valid as a starting string, so initialize
//   strings to this value
#define KS_STR_EMPTY ((ks_str){ ._ = NULL, .len = 0, .max_len = 0 })
// represents a view of a C-string. i.e. nothing is copied, and any modifications made
//   stay in the original string
// NOTE: Reallocation to strings is undefined behavior, do not call anything which may resize 
//   the string
// NOTE: Do not free the `ks_str`, just free your internal buffer afterwards
#define KS_STR_VIEW(_charp, _len) ((ks_str){ ._ = (char*)(_charp), .len = (uint32_t)(_len), .max_len = (uint32_t)(_len) })
// useful for string constants, like `KS_STR_CONST("Hello World")`
// NOTE: Do not call anything that will modify this, or resize or free it
#define KS_STR_CONST(_charp) KS_STR_VIEW(_charp, sizeof(_charp) - 1)
// creates a new string from a constant (i.e. allocates memory for it)
// This must now be freed immediately
#define KS_STR_CREATE(_charp) (ks_str_dup(KS_STR_CONST(_charp)))

// str <- charp[len], copies `len` bytes from `charp`, and then NUL-terminates
void ks_str_copy_cp(ks_str* str, char* charp, int len);
// str <- from, making a copy of the data
void ks_str_copy(ks_str* str, ks_str from);
// returns a copy of `from`, mainly just a wrapper around `ks_str_copy`, but for ease of use
ks_str ks_str_dup(ks_str from);
// str <- str + A, concatenating the strings values
void ks_str_append(ks_str* str, ks_str A);
// str <- str + c, same as above, but for a single char
void ks_str_append_c(ks_str* str, char c);
// frees `str`'s resources, and blanks it to the empty string
void ks_str_free(ks_str* str);
// comparison, should be equivalent to `strcmp(A._, B._)`
int ks_str_cmp(ks_str A, ks_str B);
// yields whether the two strings are equal
#define ks_str_eq(_A, _B) (ks_str_cmp((_A), (_B)) == 0)
// set `str` to a formatted string (C-printf style, with C arguments)
int ks_str_cfmt(ks_str* str, const char* fmt, ...);
// set `str` to a formatted string (C-printf style, with C va_list arguments)
int ks_str_vcfmt(ks_str* str, const char* fmt, va_list ap);
// reads from a file pointer, the entire file, as the string
void ks_str_readfp(ks_str* str, FILE* fp);

// a generic object type, representing an object of any type
typedef struct kso* kso;


/* list - list of object references

Holds references to `len` objects

See more in `list.c`

*/
typedef struct {
    // the array of `len` items, which may be NULL if len==0
    kso* items;

    // current number of elements, and the maximum number the current allocation could handle
    uint32_t len, max_len;

} ks_list;

// the `empty` list, which is still a valid list, and is used to initialize a list
#define KS_LIST_EMPTY ((ks_list){ .items = NULL, .len = 0, .max_len = 0 })

// pushes an object reference to the list, returns the index at which it was added
// NOTE: This increments the refcnt of `obj`
int ks_list_push(ks_list* list, kso obj);
// pops off an object, and returns that object
// NOTE: This does NOT decrement the refcnt of `obj`, as the reference is "transferred"
//   to the callee. Use `ks_list_popu` to pop an unused value (returns void, and does 
//   decrement the refcnt)
kso ks_list_pop(ks_list* list);
// pops off an object, unused, so it does not return the object.
// The object's refcnt is decremented, so it may be freed
void ks_list_popu(ks_list* list);
// frees 'list's resources, decrementing the references of all the objects in the list
void ks_list_free(ks_list* list);

/* dict - dictionary of key->value mappings of all hashable types

Internally uses a hash table to implement storing/retrieving/managing the memory

See more in `dict.c`

*/
typedef struct {

    // represents a single entry in the hash table
    struct ks_dict_entry {

        // the object which is the key for this current entry
        kso key;

        // the hash of the key, stored for efficiency purposes
        ks_hash_t hash;

        // the value at this entry
        kso val;

    }* buckets;

    // the number of buckets currently in use (often the 'size' of the hash table)
    uint32_t n_buckets;

    // the total number of non-empty entries in the hash table
    uint32_t n_entries;

    // current length (number of items), and maximum length of items allocated
    //uint32_t len, max_len;

} ks_dict;

// the empty dictionary, which is a valid starting dictionary
#define KS_DICT_EMPTY ((ks_dict){ .buckets = NULL, .n_buckets = 0, .n_entries = 0 })
// the empty entry, which can be used to initialize an entry
#define KS_DICT_ENTRY_EMPTY ((struct ks_dict_entry){ .hash = 0, .key = NULL, .val = NULL })

// gets an object given a key, NULL if notfound
// NOTE: if `key==NULL`, it searches directly using `hash`, and will never do true equality checks
//   this fact is used by global string interning
kso ks_dict_get(ks_dict* dict, kso key, ks_hash_t hash);
// sets the dictionary at a key, given a value
// NOTE: if `key==NULL`, it searches directly using `hash`, and will never do true equality checks
//   this fact is used by global string interning
void ks_dict_set(ks_dict* dict, kso key, ks_hash_t hash, kso val);
// removes the dictionary entry for a given key, returns true if it existed
// NOTE: if it didn't exist, don't remove it, just return false
bool ks_dict_del(ks_dict* dict, kso key, ks_hash_t hash);
// frees a dictionary and its resources
void ks_dict_free(ks_dict* dict);


/* object types */

typedef struct kso_str* kso_str;

typedef struct ks_prog ks_prog;
typedef struct kso_vm* kso_vm;

// a type representing a C-callable function, taking any number of arguments
typedef kso (*ks_cfunc)(kso_vm vm, int n_args, kso* args);

// generates the signature for a C-function with a given name
#define KS_CFUNC_SIG(_name) kso _name(kso_vm vm, int n_args, kso* args)

// enum describing object flags, which are in the `KSO_BASE` of every object
enum {
    // the default for flags
    KSOF_NONE = 0,

    // if this flag is set, never free the object
    // useful for the global booleans, global integer constants, etc
    KSOF_IMMORTAL = 1 << 1

};

// the base parameters that should be at the start of any object
#define KSO_BASE struct kso_type* type; int32_t refcnt; uint32_t flags;

// increment the reference count by 1, i.e. add a reference to the object
// NOTE: Even if the reference count ends up being <= 0, the object is not freed.
//   if you want that, use `KSO_CHKREF(_obj)`
#define KSO_INCREF(_obj) { ++(_obj)->refcnt; }

// decrement the reference count by 1, i.e. remove a reference to the object
// If the reference count drops <= 0, then the object is freed. This will not affect immortal objects
#define KSO_DECREF(_obj) { if (--(_obj)->refcnt <= 0) { kso_free((kso)(_obj)); } }

// checks the reference count, if the object can not be found (i.e. ref count <= 0), free the object
// If the object is still referenced elsewhere, nothing is done
#define KSO_CHKREF(_obj) { if ((_obj)->refcnt <= 0) { kso_free((kso)(_obj)); } }

// cast `_obj` to `_type`, assumes its correct to do so
#define KSO_CAST(_type, _obj) ((_type)(_obj))

/* primitive object definitions */

/* the 'type'-type, representing a type of data */
typedef struct kso_type {
    KSO_BASE

    // the name of the type, i.e. 'int'
    ks_str name;

    /* type functions */
    kso 
      // init is used as a constructor, can take `self,...` as args
      f_init,
      // free should free all resources used by the object
      f_free,

      // builtin type conversion to bool
      f_bool,
      // builtin type conversion to int
      f_int,
      // builtin type conversion to str
      f_str,
      // returns the string representation of the object
      f_repr,

      // builtin getting function
      f_get,
      // builtin setting function
      f_set

    ;

}* kso_type;


/* the 'none'-type, representing a NULL/not valid value */

extern kso_type kso_T_none;

typedef struct kso_none {
    KSO_BASE
}* kso_none;

// the global `none` instance, so they aren't freed or constructed
extern kso_none kso_V_none;

// the constant `none` value
#define KSO_NONE (kso_V_none)

// constructs a new `none` value
#define kso_none_new() (kso_V_none)


/* the 'bool'-type, representing a true or false */

extern kso_type kso_T_bool;

typedef struct kso_bool {
    KSO_BASE
    
    // the value of the boolean, true or false
    ks_bool v_bool;

}* kso_bool;

// global `bool` instances. These are never freed, so they can be re-used
extern kso_bool kso_V_false, kso_V_true;

// the value of `true`. This can be used to test if a value is true by comparing
// `val==KSO_TRUE`, assuming val is a boolean
#define KSO_TRUE (kso_V_true)

// the value of `false`. This can be used to test if a value is true by comparing
// `val==KSO_FALSE`, assuming val is a boolean
#define KSO_FALSE (kso_V_false)

// return a boolean from a given C-style value
#define kso_bool_new(_val) ((_val) ? KSO_TRUE : KSO_FALSE)


/* the 'int'-type, representing an integer value */

extern kso_type kso_T_int;

typedef struct kso_int {
    KSO_BASE
    
    // the value of the integer, as a 64-bit C integer
    ks_int v_int;

}* kso_int;

// the highest number to cache internally as a global value,
// so that small integer values don't need to be free'd and constructed
// By default, store -256->255
#define KSO_INT_CACHE_MAX (256)

// a global value table of integer literals
// To index, first make sure your integer `i` is < KSO_INT_CACHE_MAX,
//   and also i >= -KSO_INT_CACHE_MAX. If i>=0, then just take:
//   `kso_V_int_tbl[i]`, otherwise take `kso_V_int_tbl[2 * KSO_INT_CACHE_MAX+i]`
//   , keeping in mind that i will be negative, this exactly maps the entire range
extern struct kso_int kso_V_int_tbl[2 * KSO_INT_CACHE_MAX];

// the literal number `0`
#define KSO_0 (&kso_V_int_tbl[0])
// the literal number `1`
#define KSO_1 (&kso_V_int_tbl[1])

// constructs a new integer from a given value
kso_int kso_int_new(ks_int val);


/* the 'str'-type, representing a string value */

extern kso_type kso_T_str;

struct kso_str {
    KSO_BASE

    // the actual string value
    ks_str v_str;

    // the hash of the string
    ks_hash_t v_hash;

};

// construct a string object from an actual string
kso_str kso_str_new(ks_str val);

// construct a new string from C-style printf arguments (C-types only!)
kso_str kso_str_new_cfmt(const char* fmt, ...);

/* the 'list'-type, representing a list of references */

extern kso_type kso_T_list;

typedef struct kso_list {
    KSO_BASE

    // the actual list value
    ks_list v_list;

}* kso_list;

// construct the empty list
kso_list kso_list_new_empty();


/* the 'cfunc'-type, representing a callable function in C */

extern kso_type kso_T_cfunc;

typedef struct kso_cfunc {
    KSO_BASE

    // the callable C-function pointer
    ks_cfunc v_cfunc;
}* kso_cfunc;

//kso_cfunc kso_cfunc_new(ks_cfunc cfunc);

// the builtin C functions

extern kso_cfunc
    kso_F_print
;



/* the 'code'-type, representing a bit of kscript code */

extern kso_type kso_T_code;

typedef struct kso_code {
    KSO_BASE

    // values of constants used in the code, like strings, functions, etc
    kso_list v_const;

    // number of bytecode bytes
    int bc_n;

    // the actual bytecode array
    ks_bc* bc;

}* kso_code;


// constructs a blank code
kso_code kso_code_new_empty(kso_list v_const);

/* the 'kfunc'-type, representing a kscript function */

extern kso_type kso_T_kfunc;

typedef struct kso_kfunc {
    KSO_BASE

    // list of strings for parameter names
    kso_list params;

    // the code object representing the the code associated with the function
    kso_code code;


}* kso_kfunc;

// constructs a new kfunc from a list of string parameter names, and a code object
kso_kfunc kso_kfunc_new(kso_list params, kso_code code);



/* BYTECODE */


/* bytecode format:
encoded with SIZE[desc]
where SIZE is in bytes, and desciption tells what that segment does

So, this is variable-length bytecode, the arguments must be consumed variably (see exec.c for an example)

These are all executed on a stack, sometimes with a reference to a constant table
(for storing strings)

So, implicitly, they operate on the top of the stack

A list of improvements that could be made:

  * Registers that could be used for efficient caching within a function

*/

enum {

    // do nothing
    // 1[opcode]
    KS_BC_NOOP = 0,

    /* stack operations */

    // discards the top item
    // 1[opcode]
    KS_BC_DISCARD,

    // discards all items on the stack
    // 1[opcode]
    //KS_BC_CLEAR,

    /* constructing values/primitives */

    // push on a literal boolean value of `true`
    // 1[opcode]
    KS_BC_BOOLT,

    // push on a literal boolean value of `false`
    // 1[opcode]
    KS_BC_BOOLF,

    // push on a literal integer (64 bit max)
    // 1[opcode] 8[int64_t val]
    KS_BC_INT64,

    // push on a literal integer (32 bit max)
    // 1[opcode] 4[int32_t val]
    KS_BC_INT32,

    // push on a literal integer (16 bit max)
    // 1[opcode] 2[int16_t val]
    KS_BC_INT16,

    // push on a float value
    // 1[opcode] 8[ks_float val]
    KS_BC_FLOAT,

    // push on a string value, looked up from the `str_tbl`
    // 1[opcode] 4[int32_t idx(value)]
    KS_BC_STR,

    // pop on a function literal
    KS_BC_FUNC_LIT,

    // pop off `n_items` on the stack, then push on a list containing all of them
    // 1[opcode] 4[int n_items] 
    KS_BC_CREATE_LIST,

    /* lookup functions */

    // load a variable by string name
    // (idx is an index into the `str_tbl`)
    // 1[opcode] 4[int32_t idx(name)]
    KS_BC_LOAD,

    // pop off the last value, and store into a variable name
    // 1[opcode] 4[int32_t idx(name)]
    KS_BC_STORE,

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

    // modulo the last two items
    // 1[opcode]
    KS_BC_MOD,

    // exponentiate (power) the last two items
    // 1[opcode]
    KS_BC_POW,

    // compare the last two items <
    // 1[opcode]
    KS_BC_LT,

    // compare the last two items >
    // 1[opcode]
    KS_BC_GT,

    // compare the last two items ==
    // 1[opcode]
    KS_BC_EQ,

    /* func calls */

    // pop off the last item (which is the function)
    // then, pop off `n_args` more item, and call `func(args)`, pushing back on the result of the function call
    // example: `A B C f [call 3]` results in `f(A, B, C)` on the stack
    // 1[opcode] 2[uint16_t n_args]
    KS_BC_CALL,

    // pops off the last item (which is the main object)
    // then, pop off `n_args` items off the stack, and `get` them using whatever method is appropriate for the type
    // in general, this is the subscript method: `a[]`
    // so, a[b, c, d] would be:
    // b c d a [get 3]
    // 1[opcode] 2[uint16_t n_args]
    KS_BC_GET,

    // pops off the last item (which is the main object)
    // then, pop off `n_args` items off the stack, and `set` them using whatever method is appropriate for the type
    // in general, this is the subscript method: `a[]=`
    // so, a[b, c, d] = e would be:
    // b c d e a [set 4]
    // the item before last on the stack should be the value its being set to
    // 1[opcode] 2[uint16_t n_args]
    KS_BC_SET,

    // jump unconditionally a relative amount (always in bytes)
    // 1[opcode] 4[int relamt]
    KS_BC_JMP,
    
    // jump conditionally (if the last item is a bool which is true or an item which is truthy)
    // 1[opcode] 4[int relamt]
    KS_BC_JMPT,

    // jump conditionally (if the last item is a bool which is false or an item which is falsy)
    // 1[opcode] 4[int relamt]
    KS_BC_JMPF,

    /* operations */

    // pop off and 'return' the last item on the stack
    // [1:opcode]
    KS_BC_RET,

    // return None, not popping anything off the stack
    // [1:opcode]
    KS_BC_RETNONE,

    // the end of the bytecodes
    KS_BC__END

};
// a single-byte operation, no additional arguments
struct ks_bc {
    ks_bc op;
};

/* instruction definitions */

struct ks_bc_noop {
    // always KS_BC_NOOP
    ks_bc op;
};


/* constants/literals */

struct ks_bc_int64 {
    // always KS_BC_INT
    ks_bc op;

    // value of the integer
    int64_t val;
};

struct ks_bc_int32 {
    // always KS_BC_INT
    ks_bc op;

    // value of the integer
    int32_t val;
};

struct ks_bc_int16 {
    // always KS_BC_INT
    ks_bc op;

    // value of the integer
    int16_t val;
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
    int32_t val_idx;
};

struct ks_bc_func_lit {
    // always KS_BC_FUNC_LIT
    ks_bc op;

    // index into the table of the constants
    int32_t val_idx;
};

struct ks_bc_create_list {
    // always KS_BC_CREATE_LIST
    ks_bc op;

    // number of items on the stack to take
    int32_t n_items;
};

/* loading/storing */

struct ks_bc_load {
    // always KS_BC_LOAD
    ks_bc op;
    
    // index of the name into the str_tbl of its program
    int32_t name_idx;

};

struct ks_bc_store {
    // always KS_BC_STORE
    ks_bc op;
    
    // index of the name into the str_tbl of its program
    int32_t name_idx;

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

    // relative amount to jump if the last item was true, or false, or always (depending on the instruction)
    // NOTE: always in bytes
    int32_t relamt;
};

/* appending bytecode to a `code` object */

void ksc_noop(kso_code code);
void ksc_discard(kso_code code);
void ksc_boolt(kso_code code);
void ksc_boolf(kso_code code);
void ksc_int(kso_code code, ks_int v_int);
void ksc_str(kso_code code, ks_str v_str);
void ksc_func_lit(kso_code code, kso_kfunc v_kfunc);

void ksc_load(kso_code code, ks_str name);
void ksc_store(kso_code code, ks_str name);

void ksc_jmpt(kso_code code, int relamt);
void ksc_jmpf(kso_code code, int relamt);

void ksc_call(kso_code code, int n_args);

void ksc_ret(kso_code code);
void ksc_retnone(kso_code code);


/* the 'vm'-type, a virtual machine for executing code on */

extern kso_type kso_T_vm;

struct kso_vm {
    KSO_BASE

    // the stack of objects
    ks_list stk;

    // dictionary of global functions/types/etc
    ks_dict globals;
    
    // number of items in the call stack
    int call_stk_n;

    struct kso_vm_call_stk_item {
        // the program counter for this call stack item
        ks_bc* pc;

        // local variables
        ks_dict locals;

    } call_stk[256];


};

// an empty call stack item
#define KSO_VM_CALL_STK_ITEM_EMPTY ((struct kso_vm_call_stk_item){ .pc = NULL, .locals = KS_DICT_EMPTY })


// creates an empty vm
kso_vm kso_vm_new_empty();

// executes some code on the VM
kso kso_vm_exec(kso_vm vm, kso_code code);


/* execution */

// attempt to "call" a function or functor-like object with a number of arguments
// so, like `func(args[0], args[1], ...)`
// if `func` is a C-func, the native C function is called
// otherwise, NULL is returned and a global error is set via ks_err_add_*
//kso kso_call(ks_vm* vm, kso func, int args_n, kso* args);

// execute a program (starting at `idx` in the bytecode) onto a VM
//kso ks_exec(ks_vm* vm, ks_prog* prog, int idx);





/* generic type

This encapsulates just the base of an object, which is only required to have the `KSO_BASE` vars

So, anything should be validly casted to kso

*/
struct kso {
    KSO_BASE
};


/* object manipulation/management */

// free's an object's resources (through its type), and then frees `obj` itself
// This is okay to call if `obj` is immortal; it is a no-op in that case
// returns whether the object was freed
bool kso_free(kso obj);


/* AST - Abstract Syntax Trees */

// the main object that represents an AST
typedef struct ks_ast* ks_ast;

enum {
    KS_AST_NONE = 0,

    KS_AST_CONST_INT,
    KS_AST_CONST_FLOAT,
    KS_AST_CONST_STR,

    // a variable reference
    KS_AST_VAR,

    // a function-style call
    KS_AST_CALL,

    // if (cond) { BODY }
    KS_AST_IF,

    // while (cond) { BODY }
    KS_AST_WHILE,
    
    // a list of ast's
    KS_AST_BLOCK,

    // ret (value)
    KS_AST_RETURN,

    // operators
    KS_AST_BOP_ADD,
    KS_AST_BOP_SUB,
    KS_AST_BOP_MUL,
    KS_AST_BOP_DIV,
    KS_AST_BOP_MOD,
    KS_AST_BOP_POW,
    KS_AST_BOP_LT,
    KS_AST_BOP_GT,
    KS_AST_BOP_EQ,

    // assignment
    KS_AST_BOP_ASSIGN,

    // definition
    KS_AST_BOP_DEFINE,

    // a function definition, i.e.:
    // func NAME(ARGS) := { BODY }
    KS_AST_FUNCDEF

};

struct ks_ast {
    uint16_t type;

    union {
        ks_int _int;
        ks_float _float;
        ks_str _str;

        ks_ast _val;

        struct {
            // function being called
            ks_ast func;

            // list of the arguments
            int args_n;
            ks_ast* args;
        } _call;

        struct {
            ks_ast L, R;
        } _bop;

        struct {
            int sub_n;
            ks_ast* subs;
        } _block;

        struct {
            ks_ast cond;
            ks_ast body;
        } _if;

        struct {
            ks_ast cond;
            ks_ast body;
        } _while;

        struct {

            // name of the function
            ks_str name;

            // number of the parameters
            int n_params;

            // names for each of the parameters
            ks_str* param_names;

            // body of the function
            ks_ast body;

        } _funcdef;

    };

};

/* construct ASTs */

ks_ast ks_ast_new_const_int(ks_int val);
ks_ast ks_ast_new_const_float(ks_float val);
ks_ast ks_ast_new_const_str(ks_str val);
ks_ast ks_ast_new_var(ks_str name);
ks_ast ks_ast_new_call(ks_ast func, int args_n, ks_ast* args);
ks_ast ks_ast_new_ret(ks_ast val);

ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R);

ks_ast ks_ast_new_block(int sub_n, ks_ast* subs);
int ks_ast_block_add(ks_ast block, ks_ast sub);

ks_ast ks_ast_new_if(ks_ast cond, ks_ast body);
ks_ast ks_ast_new_while(ks_ast cond, ks_ast body);

// creates a funcdef with 0 params
ks_ast ks_ast_new_funcdef(ks_str name);
// adds another parameter
void ks_ast_funcdef_add_param(ks_ast funcdef, ks_str param_name);



/* AST implementation */


// generates bytecode from an AST
int ks_ast_codegen(ks_ast ast, ks_prog* to);




/* PARSING */

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

    // current token input
    int tok_i;

    // number of tokens, including the EOF token
    int tokens_n;

    // the list of tokens
    ks_token* tokens;

} ks_parse;


// a single token of input
struct ks_token {

    // the type of token,
    enum {
        // invalid token
        KS_TOK_NONE = 0,


        /* literals */

        // a literal colon ':'
        KS_TOK_COLON,
        // a literal semicolon ';'
        KS_TOK_SEMI,
        // literal dot '.'
        KS_TOK_DOT,
        // comma ','
        KS_TOK_COMMA,

        // left parenthesis '('
        KS_TOK_LPAREN,
        // right parenthesis ')'
        KS_TOK_RPAREN,

        // left brace '{'
        KS_TOK_LBRACE,
        // right brace '}'
        KS_TOK_RBRACE,

        // left bracket '['
        KS_TOK_LBRACK,
        // right bracket ']'
        KS_TOK_RBRACK,

        // a newline
        KS_TOK_NEWLINE,


        /* variables/values */

        // valid identifier, [a-zA-Z_][a-zA-Z_0-9]*
        KS_TOK_IDENT,

        // valid integer: [0-9]+
        KS_TOK_INT,

        // valid string literal, ".*"
        KS_TOK_STR,

        // a comment token, starts with `#`
        KS_TOK_COMMENT,


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
        // literal mod `%`
        KS_TOK_O_MOD,
        // literal pow `^`
        KS_TOK_O_POW,
        // literal less than `<`
        KS_TOK_O_LT,
        // literal greater than `>`
        KS_TOK_O_GT,
        // literal equals `==`
        KS_TOK_O_EQ,
        // literal assignment, `=`
        KS_TOK_O_ASSIGN,
        // literal definition, `:=`
        KS_TOK_O_DEFINE,


        KS_TOK__NUM

    } type;


    // the state at which the token appears
    struct ks_parse_state state;

    // the length of the token
    int len;

};

// the empty, default token
#define KS_TOK_EMPTY ((ks_token){ .type = KS_TOK_NONE, .state = KS_PARSE_STATE_ERROR, .len = -1})

// the 'beginning' parse state
#define KS_PARSE_STATE_BEGINNING ((struct ks_parse_state){ .line = 0, .col = 0, .i = 0 })

// there was an error
#define KS_PARSE_STATE_ERROR ((struct ks_parse_state){ .line = -1, .col = -1, .i = -1 })

// the empty parser, which is a valid initializer for it
#define KS_PARSE_EMPTY ((ks_parse){ .src_name = KS_STR_EMPTY, .src = KS_STR_EMPTY, .err = KS_STR_EMPTY, .state = KS_PARSE_STATE_BEGINNING, .tokens_n = 0, .tokens = NULL, .tok_i = 0  })


// set the current source, tokenizes, and resets the state to the beginning
void ks_parse_setsrc(ks_parse* kp, ks_str src_name, ks_str src);

// frees the parser's internal resources
void ks_parse_free(ks_parse* kp);

// sets the parser error, given a token and format args (doesn't display anything,
// this will fill it out, and if token is not `KS_TOK_EMPTY`, add useful information
//   about where the error occured in relation to the source code
// set an error globally, like using `ks_err_add_...`
void ks_parse_err(ks_parse* kp, ks_token tok, const char* fmt, ...);

/* language parsers */

// parses out a ksassembly source code to `to`
void ks_parse_ksasm(ks_parse* kp, kso_code to);

// parses out a normal program
ks_ast ks_parse_code(ks_parse* kp);


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

// dumps all errors out, printing them as errors, returns true if something was printed (i.e. there was an error)
bool ks_err_dumpall();

/* memory management routines */

void* ks_malloc(size_t bytes);

void* ks_realloc(void* ptr, size_t bytes);

void ks_free(void* ptr);

size_t ks_memuse();

/* utils, like hashing, etc */

// hashes `n` bytes
ks_hash_t ks_hash_bytes(uint8_t* data, int n);

// returns the hash of a string
ks_hash_t ks_hash_str(ks_str str);



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

/* high-level library functionality */

// initializes the library
void ks_init();


#endif

