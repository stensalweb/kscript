/* kscript.h - header file for the kscript library & compiler

kscript is a highly dynamic, duck-typed, language. It emphasizes shortness, 
the fewest boilerplate parts possible, and direct readability (such as usage 
of operators rather than functions, when possible)

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


typedef struct kso* kso;

// primitive/builtin C types

// boolean type, 1 for true, 0 for false
typedef bool    ks_bool;

// integer type (native 64 bit integer type, specifically), represents positive and
//   negative values, and can easily be handled by C applications
// in the future, there may be another integer type which is an arbitrary
//   precision using GMP/MPZ/a new library I write
typedef int64_t ks_int;

// floating point type (native C double, specifically), which is good enough for most applications
// in the future, there may be another float type which has bigger precision, or arbitrary precision
typedef double  ks_float;

/* string type */

/* string type, which is NUL-terminated, as well as length-encoded, which can be passed to native
  C functions
NOTE: Although this can be realloc'd, strings are immutable in kscript itself, so it should never
  be realloc'd in practice
*/
typedef struct {
    // NUL-terminated char pointer of length `len`
    char* _;

    // current length, and maximum length that has been allocated
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

        // the hash of the key, stored for efficiency purposes
        ks_int hash;

        // the object which is the key for this current entry
        kso key;

        // the value at this entry
        kso val;

        // the next item in the linked list of this bucket (NULL to signify end)
        struct ks_dict_entry* next;

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
#define KS_DICT_ENTRY_EMPTY ((struct ks_dict_entry){ .hash = 0, .key = NULL, .val = NULL, .next = NULL })




// gets an object given a key, NULL if notfound
kso ks_dict_get(ks_dict* dict, kso key, ks_int hash);
// sets the dictionary at a key, given a value (returns its index)
void ks_dict_set(ks_dict* dict, kso key, ks_int hash, kso val);

// string specific methods
kso ks_dict_get_str(ks_dict* dict, ks_str key, ks_int hash);
void ks_dict_set_str(ks_dict* dict, ks_str key, ks_int hash, kso val);



// gets an object given a string key, NULL if notfound
//kso ks_dict_get_str(ks_dict* dict, ks_str key);

// set the dictionary at a key (which is string), given a value (returns its index)
//int ks_dict_set_str(ks_dict* dict, ks_str key, kso val);

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


// definition of a type
struct kso_type {
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

};


/* none-type

Represents NULL/none/empty

NOTE: You should never manually allocate/create one of these, because there is a global singleton
  that represents `none`, just call either `kso_new_none()`, or the value `kso_V_none`.

*/
typedef struct kso_none {
    KSO_BASE
}* kso_none;


/* bool-type

Represents either true or false (1 or 0)

NOTE: You should never manually allocate/create one of these, because there are global constants so only 1 true and false are ever created,
  use `kso_new_bool(true or false)`, or just `kso_V_true`, or `kso_V_false`

*/
typedef struct kso_bool {
    KSO_BASE
    ks_bool _bool;
}* kso_bool;


/* int-type

Represents a 64 bit integer normally, to construct use `kso_new_int(val)`

TODO: Perhaps use GMP/mpz/bigint as the default integer type? Or should this be a seperate type alltogether?

*/
typedef struct {
    KSO_BASE
    ks_int _int;
}* kso_int;


/* float-type

Represents a 64 bit float, (a C double), to construct use `kso_new_float(val)`

*/
typedef struct {
    KSO_BASE
    ks_float _float;
}* kso_float;

/* str-type (strings)

Represents a string of characters (1 byte each) of a given length, construct using `kso_new_str(val)`

TODO: Maybe use interning of strings, similar to python. i.e. keep a single instance of a given string alive at a time,
  since they are immutable, and just keep a hash-table of hashes and references, so everybody shares a reference count

*/
typedef struct {
    KSO_BASE
    // store the string hash when created
    ks_int _strhash;
    ks_str _str;
}* kso_str;


/* list-type (ref list)

Represents a list of references to other objects (i.e. does not 'own' the data of the objects), so freeing
  or creating a list doesn't really free or create any additional data, unless the reference count drops below 0 for some of the objects in the array.

Construct using `kso_new_list(n, obj-array)`

*/
typedef struct {
    KSO_BASE
    ks_list _list;
}* kso_list;


/* dict-type (ref dictionary)

Represents a dictionary mapping of key->value

Right now, it uses a linear search through the hashes, so is not as efficient as it could be. It also could use a method for handling hash collisions

*/
typedef struct {
    KSO_BASE
    ks_dict _dict;
}* kso_dict;


/* cfunc-type (C-function)

Represents a function as a C-style function pointer taking a number of args and list of arguments, returning a result

Typically, I think these should be marked as `immortal`, because I can't think of any need to garbage collect these EXCEPT in the case where I (or someone else) writes a JIT compiler, and constructs these programmatically. That would be sick

*/
typedef struct kso_cfunc {
    KSO_BASE
    kso (*_cfunc)(int n_args, kso* args);
}* kso_cfunc;


/* generic type

This encapsulates just the base of an object, which is only required to have the `KSO_BASE` vars

*/
struct kso {
    KSO_BASE
};


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


/* constructing new primitives 

All these either return global constants (which are marked as immortal), or interned references of ojbects that may be duplicated (like strings), or new references with a reference count of 0. So, use `KSO_INCREF` once you get these objects, and KSO_DECREF once you are done using them

*/

// returns a new `none` object (namely, the only one).
// This returns the immortal-global-constant `kso_V_none`
// It is safe to use normally
kso kso_new_none();

// returns a new `bool` object with either true or false value
// It will return either one of the immortal-global-constants, either `kso_V_true`, or `kso_V_false`
// It is safe to use normally
kso kso_new_bool(ks_bool val);
// returns a new `int` object with a given value
// For certain values, a immortal-global-constant may be returned (for efficiency). I'm thinking (-255,255),
//   not sure though
// Dealing with integers as immutable values will take some hardcoding if this is implemented, because we can never be allowed to modify these
kso kso_new_int(ks_int val);

// returns a new `float` object with a given value
// this always allocates a new object, and returns the pointer, it's never constant, so always needs to be reference counted
kso kso_new_float(ks_float val);

// returns a string object with a given value
// NOTE: The result does not depend on `val` at all, so it can be called with a constant argument, else the caller is responsible for `val` afterwards
kso kso_new_str(ks_str val);

// returns a string object from a given C-printf-style format and args
kso kso_new_str_cfmt(const char* fmt, ...);


// creates a list of `n` references, and fills it in from `refs`. If `refs==NULL`, then the array is uninitialized
kso kso_new_list(int n, kso* refs);

// attempt to "call" a function or functor-like object with a number of arguments
// so, like `func(args[0], args[1], ...)`
// if `func` is a C-func, the native C function is called
// otherwise, NULL is returned and a global error is set via ks_err_add_*
kso kso_call(kso func, int args_n, kso* args);

/* object manipulation/management */

// free's an object's resources (through its type), and then frees `obj` itself
// This is okay to call if `obj` is immortal; it is a no-op in that case
// This should only be done if you are sure no one else is using the object
void kso_free(kso obj);


/* bytecode format:
encoded with SIZE[desc]
where SIZE is in bytes

So, this is variable-length bytecode, the arguments must be consumed variabl-y

These are all executed on a stack, sometimes with a reference to a constant table
(for storing strings)

So, implicitly, they operate on the top of the stack

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


// a program, representing a bunch of instructions, and labels, as well as constants and required vals
typedef struct {

    // number of strings in the constant table
    int str_tbl_n;

    // the table of string constants used by bytecodes
    kso_str* str_tbl;


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

/* generate instructions (these all return the index at which the instruction was added) */

int ksb_noop(ks_prog* prog);

/* constant/literal instructions */

int ksb_bool(ks_prog* prog, ks_bool val);
int ksb_int64(ks_prog* prog, int64_t val);
int ksb_int32(ks_prog* prog, int32_t val);
int ksb_int16(ks_prog* prog, int16_t val);
int ksb_float(ks_prog* prog, ks_float val);
int ksb_str(ks_prog* prog, ks_str val);
int ksb_create_list(ks_prog* prog, uint32_t n_items);

/* load/store functionality */

int ksb_load(ks_prog* prog, ks_str name);
int ksb_store(ks_prog* prog, ks_str name);

/* operators */
int ksb_add(ks_prog* prog);
int ksb_sub(ks_prog* prog);
int ksb_mul(ks_prog* prog);
int ksb_div(ks_prog* prog);
int ksb_mod(ks_prog* prog);
int ksb_pow(ks_prog* prog);
int ksb_lt(ks_prog* prog);
int ksb_gt(ks_prog* prog);
int ksb_eq(ks_prog* prog);

int ksb_call(ks_prog* prog, uint32_t n_args);
int ksb_get(ks_prog* prog, uint32_t n_args);
int ksb_set(ks_prog* prog, uint32_t n_args);
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

/* AST implementation */

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
    KS_AST_BOP_ASSIGN

};

struct ks_ast {
    uint16_t type;

    union {
        ks_int _int;
        ks_float _float;
        ks_str _str;

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

    };


};


ks_ast ks_ast_new_const_int(ks_int val);
ks_ast ks_ast_new_const_float(ks_float val);
ks_ast ks_ast_new_const_str(ks_str val);
ks_ast ks_ast_new_var(ks_str name);
ks_ast ks_ast_new_call(ks_ast func, int args_n, ks_ast* args);

ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R);

ks_ast ks_ast_new_block(int sub_n, ks_ast* subs);
int ks_ast_block_add(ks_ast block, ks_ast sub);

ks_ast ks_ast_new_if(ks_ast cond, ks_ast body);
ks_ast ks_ast_new_while(ks_ast cond, ks_ast body);


// generates bytecode from an AST
int ks_ast_codegen(ks_ast ast, ks_prog* to);


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

        // left parenthesis '('
        KS_TOK_LPAREN,
        // right parenthesis ')'
        KS_TOK_RPAREN,

        // left brace '{'
        KS_TOK_LBRACE,
        // right brace '}'
        KS_TOK_RBRACE,

        // comma ','
        KS_TOK_COMMA,


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
        // literal assignment
        KS_TOK_O_ASSIGN,


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
void ks_parse_setsrc(ks_parse* kp, ks_str src_name, ks_str src);

// sets the parser error, given a token and format args (doesn't display anything,
//   but now you can check kp->err)
// this will fill it out, and if token is not `KS_TOK_EMPTY`, add useful information
//   about where the error occured in relation to the source code
void ks_parse_err(ks_parse* kp, ks_token tok, const char* fmt, ...);

// parses out an entire bytecode program
void ks_parse_bc(ks_parse* kp, ks_prog* to);

// parses out a normal program
ks_ast ks_parse_code(ks_parse* kp);

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
    kso_F_mod,
    kso_F_pow,
    
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

// dumps all errors out, printing them as errors, returns true if something was printed (i.e. there was an error)
bool ks_err_dumpall();



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

