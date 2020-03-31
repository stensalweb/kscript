/* ks.h - main header for the kscript library, including defintions for the C-API, and builtin types,
 *   as well as builtin globals
 *
 * |-   High-Level Overview   -|
 * 
 * kscript is a language/environment that is meant to be highly dynamic, cross platform,
 *   easy to use, and featureful. The standard library includes strings, dictionaries, list
 *   datastructures, image/audio/video utilities, math utilities, graphics utilities, etc
 * 
 * 
 * 
 * |-   Binary Inteface   -|
 * 
 * Every object in kscript is castable as a `ks_obj`, which stores the only
 *   essential parts of the object (type & reference count). However, some have extra data attached,
 *   which you can get by casting to that specific. To do this, with a list, first check:
 * 
 * ```
 * ks_obj my_obj = ...;
 * if (my_obj->type == ks_type_list) {
 *     // we know it is a list, so we can cast it
 *     ks_list my_list = (ks_list)my_obj;
 * } else {
 *     // not a list, perhaps throw an error?
 * }
 * ```
 * 
 * 
 * |-   Ref Counting   -|
 * 
 * Reference Counting (or Ref Counting) is the main memory management tool in kscript. When objects are created,
 *   for example by `ks_int_new(432)`, they come with an active reference (which means `obj->refcnt >= 1`). An active
 *   reference means that the object should not be freed. For example, if an object's reference count is 5, then there are 5
 *   other objects saying "don't free this object, I still need it!". When that count drops to 0, that means the object
 *   can be destroyed and no one will care. So, that is what is done!
 * 
 * This model requires that C-API functions and extensions manage the reference count correctly. So, that means that
 *   if you 'hold' a reference to an object (i.e. have incremented the reference count via `KS_DECREF(obj)`), you are expected 
 *   to decrease the reference count via `KS_DECREF(obj)` once you are done using the object. So, code written in kscript itself
 *   does not need to worry about this because all reference counting will be done in C automatically
 * 
 * Once an object is freed, `type(obj).__free__(self)` is called on the object, which should have a reference count of '0'
 * 
 * 
 * |-   Execution State   -|
 * 
 * Similar to python, there is a Global Interpreter Lock (GIL), which can be released periodically and during
 *   I/O operations. While this does have problems with threading in pure kscript, most intense workloads will
 *   use C-APIs to do the work, which can themselves release the GIL. See functions `ks_GIL_lock()` and `ks_GIL_unlock()`
 * 
 * Also, all execution (whether it be kscript code or a C-function called from kscript) must happen inside a `ks_thread`,
 *   which can be constructed with `ks_thread_new()` (see comments describing how to do this)
 * 
 * What this means is that `ks_thread_get()` should always return non-NULL for any code that is being ran, and it does not
 *   return a new reference, so you should NOT call `KS_DECREF(ks_thread_get())` ever!
 * 
 * 
 * 
 * 
 * 
 * You can use the standard utility functions `ks_call(func, n_args, args)`, etc
 *   to perform the same as the kscript builtins, or directly call their C-api functions
 * 
 * For example, the builtin print function is `ks_F_print`, so you can call:
 *   ks_obj res = ks_F_print->func(1, &obj);
 * 
 * 
 * You can construct primitive via their `_new` functions, i.e.: `ks_list_new(0, NULL)` constructs an empty
 *   list. See the comments above each function to see what they do
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef KS_H__
#define KS_H__

#ifdef __cplusplus
extern "C" {
#endif

// generated files
#include <ks-config.h>


// optional dependencies
#ifdef KS_HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif


/* system/stdlib headers */
#if defined(KS__WINDOWS) || defined(KS__CYGWIN)

// TODO: actually try and built on windows platform

// include specific headers
#include <windows.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>
#include <complex.h>

#include <time.h>
#include <sys/time.h>

#include <signal.h>

#include <pthread.h> 

/*#elif defined(KS__LINUX) || defined(KS__MACOS)*/
#else
// UNIX-style headers only
    
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>
#include <complex.h>

#include <time.h>
#include <sys/time.h>


#include <signal.h>

#include <pthread.h> 


#endif


/* CONSTANTS */

// enumeration for levels of logging, from least important to most important
enum {
    // tracing, i.e. minute details will cover things such as allocations/deallocations,
    // and fine grained output. Some release builds may be built with tracing completely disabled,
    // so no output will show up from trace statements (to improve speeds of release builds)
    KS_LOG_TRACE = 0,

    // debug, i.e. occasional output will come from operations, such as large allocations/deallocations
    // (typically > 500MB), any odd occurences (for example, a dictionary having a lot of collisions
    // could be debugged, so that people can see that their dictionary building schemes are inefficient)
    // most builds are built with DEBUG support
    KS_LOG_DEBUG,
    
    // info, i.e. large operations (such as allocations/deallocations >= 10% of system memory), things such
    // as successful module imports, initialization information, etc
    KS_LOG_INFO,

    // warn, i.e. warning of odd/peculiar happenings that don't neccessarily halt the program, but should be
    // attended to, such as extremely large allocations >= 25% of system memory, a FILE read did not produce the
    // correct result to return (like fread(..., n, 1, fp) != n)), or a parameter was NULL
    // This is the default value for release software
    KS_LOG_WARN,

    // error, i.e. critical issues that cause problems if not paid any attention to.
    // this includes thrown exceptions which are not caught, a FILE was not opened correctly, 
    // a doublefree/corruption error occured, an object was NULL when it shouldn't have been
    // This is always printed out, can not be ignored by setting the logging level
    KS_LOG_ERROR,


    KS_LOG__END
};


/* BYTECODE 
 *
 * The format/syntax is SIZE:[type desc], i.e. 4:[int thing it does]
 * 
 * All instructions are at least 1 byte, and they start with the op code (i.e. 'KSB_*')
 * 
 * Some bytecodes have indexes that refer to the constants array, which is held by the code object
 * 
 * All bytecode is executed on a 'ks_thread', so references to the 'stack', and 'exceptions' are variables
 *   on the thread (which can be obtained via `ks_thread_get()`)
 * 
 * Terminology:
 *   TOS: Top Of Stack, i.e. the item that is on top of the stack
 *   UTOS: Under Top Of Stack, i.e. the item directly under the top of the stack
 * 
 */

// enumeration of all VM commands
enum {

    // Do nothing, just go to the next instruction. This should not be generated by code generators
    // 1:[op]
    KSB_NOOP = 0,


    /** STACK MANIPULATION, these opcodes do simple operations on the stack **/

    // Push a constant value onto the stack
    // 1:[op] 4:[int index into the 'v_const' array of code constants]
    KSB_PUSH,

    // Push a new reference of the TOS on top of the stack
    // 1:[op]
    KSB_DUP,

    // Pop off the TOS, and do not use it in any computation
    // 1:[op]
    KSB_POPU,



    /** CONTROL FLOW, these opcodes change the control flow of the function **/

    // Pop off 'n_items', and perform a function call with them
    // The item on the bottom of the stack
    // Example: if the stack is | A B C , then KSB_CALL(n_items=3) will result in calculating 'A(B, C)' 
    //   and pushing that result back on the stack
    // If there are not 'n_items' on the stack, internally abort
    // 1:[op] 4:[int n_items]
    KSB_CALL,

    // Pop off the TOS, and return that as the return value of the currently executing function,
    //   causing the top item of the stack frame to be popped off
    // 1:[op]
    KSB_RET,

    // Jump, unconditionally, a 'relamt' bytes in the bytecode, from the position where the next
    //   instruction would have been.
    // EX: jmp +0 is always a no-op, the program counter is not changed
    // EX: jmp -5 creates an infinite loop, because the jump instruction itself is 5 bytes,
    //       so it would jump back exactly one instruction
    // 1:[op] 4:[int relamt]
    KSB_JMP,

    // Pop off the TOS, get a truthiness value (default: 'true'), and if it was truthy,
    //   jump a specified number of bytes
    // EX: jmpt +10 will skip 10 bytes if the TOS was truthy
    // 1:[op] 4:[int relamt]
    KSB_JMPT,

    // Pop off the TOS, get a truthiness value (default: 'true'), and if it was NOT truthy,
    //   jump a specified number of bytes
    // EX: jmpf +10 will skip 10 bytes if the TOS was falsey
    // 1:[op] 4:[int relamt]
    KSB_JMPF,

    // Pop off the TOS, and 'throw' it up the call stack, rewinding, etc. If there was no
    //   'catch' block set up, then it will cause the program to abort and print an error
    // 1:[op]
    KSB_THROW,

    // Pop off the TOS, and 'assert' it is true, or throw an exception
    // 1:[op]
    KSB_ASSERT,

    // Enter a 'try' block, which will cause the code to jump to +relamt if an exception is thrown
    // NOTE: The relative amount is from the point which the TRY_START instruction is encountered;
    //   not where the exception was thrown
    // 1:[op] 4:[int relamt]
    KSB_TRY_START,

    // Exit a 'try' block, jumping unconditionally 'relamt'
    // NOTE: This should be emitted at the end of the try block, but NOT in the exception block (as
    //   that will exit the try block when something is thrown)
    // 1:[op] 4:[int relamt]
    KSB_TRY_END,

    // Replace the function on top with a copy (i.e. new, distinct copy)
    // 1:[op]
    KSB_NEW_FUNC,

    // Add a closure reference to the current stack frame to the TOS, which must be a kfunc
    // 1:[op]
    KSB_ADD_CLOSURE,

    // Replace the TOS with iter(TOS), throwing an error if it couldn't do it
    // 1:[op]
    KSB_MAKE_ITER,

    // Push on next(TOS), or, if there was an error, jump 'relamt' bytes in the bytecode and discard the error
    // (i.e. finish the loop)
    // 1:[op]
    KSB_ITER_NEXT,



    /** PRIMITIVE CONSTRUCTION, these opcodes create basic primitives from the stack **/

    // Pop off 'num_elems' from the stack, create a tuple from them, and then push that tuple
    //   back onto the stack
    // 1:[op] 4:[int num_elems, number of elements to take off the stack]
    KSB_TUPLE,

    // Pop off 'num_elems' from the stack, create a list from them, and then push that list
    //   back onto the stack
    // 1:[op] 4:[int num_elems, number of elements to take off the stack]
    KSB_LIST,

    // Pop off 'num_elems'*2 from the stack, treat them as a bunch of keys & values (interleaved),
    //   and construct a dictionary from them. Then, push the dict back on the stack
    // 1:[op] 4:[int num_elems, aka num entries]
    KSB_DICT,


    
    /** VALUE LOOKUP **/

    // Load a value indicated by 'v_const[idx]' (being the string name), and push it on the stack
    // Raise an error if no such value was found
    // 1:[op] 4:[int idx into 'v_const']
    KSB_LOAD,

    // Pop off the TOS, then calculate getattr(TOS, v_const[idx]). Then, push that attribute on
    // Raise an error if no such attribute was found on the TOS
    // Internally abort if there was no item on TOS
    // 1:[op] 4:[int idx into 'v_const']
    KSB_LOAD_ATTR,

    // Store TOS into value 'v_const[idx]', creating it as a local if it was not found
    // Internally abort if there was no item on TOS
    // 1:[op] 4:[int idx into 'v_const']
    KSB_STORE,

    // Set an attribute to a value
    // Pop off the set UTOS.<attr> = TOS, then removes both, and pushes back on TOS
    // So stack goes from:
    // | UTOS TOS  -> set UTOS.<attr>=TOS -> | TOS
    // Internally abort if there was no item on TOS
    // 1:[op] 4:[int idx into 'v_const' of 'attr'1]
    KSB_STORE_ATTR,


    // Get an item via a subscript, i.e. obj[args]
    // 1:[op] 4:[int n_items : number of items (including object itself)]
    KSB_GETITEM,

    // Set an item via a subscript, i.e. obj[args] = val
    // 1:[op] 4:[int n_items : number of items (including object itself and value)]
    KSB_SETITEM,


    /** OPERATORS **/

    /***  UNARY ***/
    // All of these operators replace the TOS with the operator applied to it

    // the '-' operator (i.e. -a)
    KSB_UOP_NEG,
    // the '~' operator (i.e. ~a)
    KSB_UOP_SQIG,

    // the '!' operator (i.e. !a), which always converts to boolean
    KSB_UOP_NOT,

    /*** BINARY ***/
    // All of these operators pop 2 items off, attempt to do the operation on them using
    //   member functions, and then push that result back on
    // Will throw an error if not supported

    // the '+' operator
    KSB_BOP_ADD,
    // the '-' operator
    KSB_BOP_SUB,
    // the '*' operator
    KSB_BOP_MUL,
    // the '/' operator
    KSB_BOP_DIV,
    // the '%' operator
    KSB_BOP_MOD,
    // the '**' operator
    KSB_BOP_POW,

    // the '<=>' operator
    KSB_BOP_CMP,

    // the '<' operator
    KSB_BOP_LT,
    // the '<=' operator
    KSB_BOP_LE,
    // the '>' operator
    KSB_BOP_GT,
    // the '>=' operator
    KSB_BOP_GE,
    // the '==' operator
    KSB_BOP_EQ,
    // the '!=' operator
    KSB_BOP_NE,

    // Convert TOS to its truthy value
    KSB_TRUTHY

};


/* TYPES/STRUCTURE DEFS */

// make sure these are aligned to a single byte, because they are bytecodes
#pragma pack(push, 1)

// ksb - a single bytecode, i.e. sizeof(ksb) == 1
// can be part of the opcode, or any other parts
typedef uint8_t ksb;

// ksb_i32 - a sigle bytecode with a 32 bit signed integer component, sizeof(ksb_i32) == 5
typedef struct {

    ksb op;

    int32_t arg;

} ksb_i32;

// end single byte alignment
#pragma pack(pop)


// ks_size_t - type representing the size of something, i.e. unsigned and long enough to hold
//   64 bit indices
typedef uint64_t ks_size_t;


// ks_ssize_t - type representing a signed size, which can be negative
typedef int64_t ks_ssize_t;


// ks_hash_t - type representing a hash of an object. 
// NOTE: hashes should never be '0', that means the hash is uninitialized or invalid,
//   so manually 'nudge' the value to '1' or '-1' if that happens to come from a legimitate hash function
typedef uint64_t ks_hash_t;


// ks_obj - the most generic kscript object, which any other objects are castable to
// This contains a 'type' & a 'refcnt' variable (in the KS_OBJ_BASE macro), which tell the type object as
//   well as a count of active references to the object. One should not modify these, and instead use the macros
//   KS_INCREF(obj) & KS_DECREF(obj) respectively
// Once the reference count drops to 0, that means the object is effectively a 'dangling pointer', and thus can
//   be freed.
// Objects should always stay as pointers, so they aren't type-erased. Dereferencing
//   an object could be bad, so don't do it! Use -> operations
typedef struct ks_obj* ks_obj;

// ks_str - a string object in kscript
// These are immutable, and thus a new string should be constructed on any operation
// However, this poses a big problem: string concatenation becomes O(n^2). To solve this,
//   I've implemented a string builder struct `ks_str_b`, which can be used to build up a string in O(n)
//   time, and then generate a string once. This can, of course, be used in any C-extension, etc
//   Or, from within kscript as `"".join([strs...])`
// See more on string operations in `types/str.c`
typedef struct ks_str* ks_str;



// ks_type - an object representing a type in kscript. Every object has a type, which you can check with `obj->type`,
// SEE: types/type.c
typedef struct ks_type* ks_type;

// ks_tuple - an object representing a tuple or collection of objects, which is immutable
// SEE: types/tuple.c
typedef struct ks_tuple* ks_tuple;

// ks_list - an object representing an ordered list (i.e. array) of objects in kscript. note that this only holds references
//   to the objects; objects are not duplicated for the list
// SEE: types/list.c
typedef struct ks_list* ks_list;


// ks_dict - an object representing a generic object mapping, where an object of (most) types can be a key, and any type can be
//   a value
// Internally, a hash table implementation is used, similar to Python's
// SEE: types/dict.c
typedef struct ks_dict* ks_dict;

/* UTILITY MACROS */

// Require an expression to be true, otherwise throw an error and return 'NULL'
// This is similar to an assert(), but instead of crashing, it will throw an exception
// NOTE: Should be used inside of a KS_FUNC(), because this will return NULL!
#define KS_REQ(_expr, ...) {                      \
    if (!(_expr)) {                               \
        ks_throw_fmt(ks_type_Error, __VA_ARGS__); \
        return NULL;                              \
    }                                             \
}

// Require that the number of args is a specific amount
#define KS_REQ_N_ARGS(_nargs, _correct) KS_REQ((_nargs) == (_correct), "Incorrect number of arguments, expected %i, but got %i", (int)(_correct), (int)(_nargs))

// Require that the number of args is at least a specific amount
#define KS_REQ_N_ARGS_MIN(_nargs, _min) KS_REQ((_nargs) >= (_min), "Incorrect number of arguments, expected at least %i, but got %i", (int)(_min), (int)(_nargs))

// Require that the number of args is at most a specific amount
#define KS_REQ_N_ARGS_MAX(_nargs, _max) KS_REQ((_nargs) <= (_max), "Incorrect number of arguments, expected at most %i, but got %i", (int)(_max), (int)(_nargs))

// Require that the number of args is in a specific range
#define KS_REQ_N_ARGS_RANGE(_nargs, _min, _max) KS_REQ((_nargs) >= (_min) && (_nargs) <= (_max), "Incorrect number of arguments, expected between %i and %i, but got %i", (int)(_min), (int)(_max), (int)(_nargs))

// Require that the object is of a given type. 'name' is a C-string that is the human readable name for the variable
#define KS_REQ_TYPE(_obj, _type, _name) KS_REQ(ks_type_issub((_obj)->type, (_type)), "Incorrect type for '%s', expected '%S', but got '%S'", _name, _type, (_obj)->type)

// Require that an object is callable
#define KS_REQ_CALLABLE(_obj, _name) KS_REQ(ks_is_callable(_obj) == true, "Parameter '%s' (of type '%T') was not callable", _name, _obj)

// Require that an object is iterable
#define KS_REQ_ITERABLE(_obj, _name) KS_REQ(ks_is_iterable(_obj) == true, "'%T' object was not iterable", _obj)

// throw a type conversion error
#define KS_ERR_CONV(_obj, _totype) { \
    ks_throw_fmt(ks_type_TypeError, "'%T' object could not be converted to %S", _obj, _totype); \
    return NULL; \
}

// Throw an operator undefined error and return NULL
#define KS_ERR_BOP_UNDEF(_str, _L, _R) { \
    ks_throw_fmt(ks_type_OpError, "operator '%s' not defined for '%T' and '%T'", _str, _L, _R); \
    return NULL; \
}

// Throw an operator undefined error and return NULL
#define KS_ERR_UOP_UNDEF(_str, _V) { \
    ks_throw_fmt(ks_type_OpError, "operator '%s' not defined for '%T'", _str, _V); \
    return NULL; \
}

// throw an attribute error
#define KS_ERR_ATTR(_obj, _attr) { \
    ks_throw_fmt(ks_type_AttrError, "'%T' object has no attr %R", _obj, _attr); \
    return NULL; \
}

// throw an item key error
#define KS_ERR_KEY(_obj, _key) { \
    ks_throw_fmt(ks_type_KeyError, "'%T' object did not contain the key %*R", _obj, _key); \
    return NULL; \
}

// throw an item key error for multiple key values
#define KS_ERR_KEY_N(_obj, _nkeys, _keys) { \
    int _nk = _nkeys; \
    ks_obj* _ky = (ks_obj*)(_keys); \
    ks_str_b SB; ks_str_b_init(&SB); \
    int i; \
    for (i = 0; i < _nk; ++i) ks_str_b_add_fmt(&SB, "%s%R", i > 0 ? " " : "", _ky[i]); \
    ks_str sbs = ks_str_b_get(&SB); \
    ks_str_b_free(&SB); \
    ks_throw_fmt(ks_type_KeyError, "'%T' object did not contain the key %S", _obj, sbs); \
    KS_DECREF(sbs); \
    return NULL; \
}


// Put this macro at the beginning of the definition of any kscript object, i.e.:
// struct my_obj {
//   KS_OBJ_BASE
//   int num;
//   ...
// }
// This will make it a valid kscript object type, and make pointers to your object type
//   be validly casted to `ks_obj`
#define KS_OBJ_BASE int64_t refcnt; ks_type type;

// Record a reference to a given object (i.e. increment the reference count)
// EX: KS_INCREF(obj)
#define KS_INCREF(_obj) { ++((ks_obj)_obj)->refcnt; }

// Un-record a reference to a given object (i.e. decrement the reference count)
// NOTE: If the reference count reaches 0 (i.e. the object has became unreachable), this frees
//   the object
// EX: KS_DECREF(obj)
#define KS_DECREF(_obj) { ks_obj fobj = (ks_obj)(_obj); if (--fobj->refcnt <= 0) { ks_obj_free(fobj); } }

// Allocate memory for a new object type (uses `ks_malloc`)
// For example: `KS_ALLOC_OBJ(ks_int)` will allocate a `ks_int`
// Use the type that is a pointer to the actual type, as it will construct the size of a dereferenced type
#define KS_ALLOC_OBJ(_typeName) ((_typeName)ks_malloc(sizeof(*(_typeName){NULL})))

// Free an object's memory (non-recursively; just the actual object pointer)
// Use this macro on things created with `KS_ALLOC_OBJ(_typeName)`
#define KS_FREE_OBJ(_obj) (ks_free((void*)(_obj)))

// Initialize an object of a given type, essentially setting its type as well as
// Setting its reference count to '1' (since it should be created with a reference)
// NOTE: This also increments the reference count of '_typeType', since objects of
//   a given type hold a reference to that type
#define KS_INIT_OBJ(_obj, _typeType) { \
    ks_obj obj = (ks_obj)(_obj); \
    ks_type typeType = (_typeType); \
    obj->type = typeType; \
    KS_INCREF(typeType); \
    obj->refcnt = 1; \
}

// Initialize an object that is itself a type
// Example: KS_INIT_TYPE_OBJ(ks_type_int, "int")
#define KS_INIT_TYPE_OBJ(_typeObj, _name) { \
    ks_init_type(_typeObj, _name); \
}

// Uninitialize an object, i.e. unrecord the reference it has to it's type
#define KS_UNINIT_OBJ(_obj) {    \
    ks_obj obj = (ks_obj)(_obj); \
    KS_DECREF(obj->type);        \
}

// This will declare a ks_type variable of name `_type`, and an internal structure of `_type`_s
// EXAMPLE: KS_TYPE_DECLFWD(ks_type_int) defines `ks_type_int_s` and `ks_type_int`, but `ks_type_int`
//   is a static address; not allocated. So, the type is not generated at runtime, but rather is constant
#define KS_TYPE_DECLFWD(_type) struct ks_type _type##_s; ks_type _type = &_type##_s;

// This will define a function with '_name'+_, as a kscript C-extension function
// i.e.: KS_FUNC(add) will define a function called `add_`
#define KS_FUNC(_name) ks_obj _name##_(int n_args, ks_obj* args)

// This will define a function with '_type'_'_name'+_, as a kscript C-extension function
// i.e.: KS_TFUNC(int, add) will define a function called `int_add_`
#define KS_TFUNC(_type, _name) ks_obj _type##_##_name##_(int n_args, ks_obj* args)

// ks_obj - the most generic kscript object, which any other objects are castable to
// This contains a 'type' & a 'refcnt' variable (in the KS_OBJ_BASE macro), which tell the type object as
//   well as a count of active references to the object. One should not modify these, and instead use the macros
//   KS_INCREF(obj) & KS_DECREF(obj) respectively
// Once the reference count drops to 0, that means the object is effectively a 'dangling pointer', and thus can
//   be freed.
// Objects should always stay as pointers, so they aren't type-erased. Dereferencing
//   an object could be bad, so don't do it! Use -> operations
struct ks_obj {
    KS_OBJ_BASE
};


// return a new reference to an object; use macro KS_NEWREF
static inline ks_obj ks_newref(ks_obj obj) {
    KS_INCREF(obj);
    return obj;
}

// This will create a new reference to '_obj', for ease of use returning functions
// NOTE: This also downcasts to 'ks_obj'
#define KS_NEWREF(_obj) ks_newref((ks_obj)(_obj))

// base structure describing what a 'type' is
struct ks_type {
    KS_OBJ_BASE

    // attributes of the type (i.e. member functions, static variables, etc)
    ks_dict attr;

    /* quick references: 
     *
     * These values are meant to be able to be quickly looked up, as they are builtins that will be called often.
     * These should always be equal to 'type.$NAME', so 'type->__str__' should always be equal to 
     *   'getattr(type, "__str__")'
     * 
     * The main reason for these attributes are speed of common operations, which this will allow us to skip a dict
     *   lookup, and instead just check whether the member function is non-null
     * 
     * However, they are still in the 'attr' dict, so any dictionary lookup will also succeed. Therefore,
     *   no reference is held by these member variables, as they use the one that the dictionary holds
     * 
     */

    // type.__name__ -> the name of the type, typically human readable
    ks_str __name__;

    // type.__parents__ -> a list of parent classes from which this type derives from
    ks_list __parents__;


    // type.__len__(self) -> get the length of an item
    ks_obj __len__;

    // type.__hash__(self) -> return the has of an object
    ks_obj __hash__;


    // type.__bool__(self) -> convert an item to a boolean
    ks_obj __bool__;


    // type.__str__(self) -> convert an item to a string
    ks_obj __str__;

    // type.__repr__(self) -> convert an item to a string representation
    ks_obj __repr__;

    // type.__new__(self) -> construct a new object of a given type. This should normally take 0 arguments
    //   and if '__init__' is not NULL, this should be called always with 0, then called __init__ with the resultant
    //   object and the rest of the arguments
    ks_obj __new__;

    // type.__init__(self) -> initialize an object (i.e. the second part of the constructor)
    ks_obj __init__;

    // type.__free__(self) -> free the memory/references used by the object
    ks_obj __free__;

    // type.__call__(self, *args) -> call 'self' like a function, given arguments
    ks_obj __call__;


    // type.__iter__(self) -> return an iterator for an object
    ks_obj __iter__;

    // type.__next__(self) -> return the next object for an iterator
    ks_obj __next__;


    // type.__getattr__(self, attr) -> get an attribute from an object
    ks_obj __getattr__;

    // type.__setattr__(self, attr, val) -> set an attribute on an object
    ks_obj __setattr__;

    // type.__getitem__(self, key) -> get a value from an object
    ks_obj __getitem__;

    // type.__setitem__(self, key, val) -> set a value on an object
    ks_obj __setitem__;


    /* operators */

    // type.__add__(A, B) -> return A + B
    ks_obj __add__;
    // type.__sub__(A, B) -> return A - B
    ks_obj __sub__;
    // type.__mul__(A, B) -> return A * B
    ks_obj __mul__;
    // type.__div__(A, B) -> return A / B
    ks_obj __div__;
    // type.__mod__(A, B) -> return A % B
    ks_obj __mod__;
    // type.__pow__(A, B) -> return A ** B
    ks_obj __pow__;

    // type.__cmp__(A, B) -> return A <=> B
    ks_obj __cmp__;
    // type.__lt__(A, B) -> return A < B
    ks_obj __lt__;
    // type.__le__(A, B) -> return A <= B
    ks_obj __le__;
    // type.__gt__(A, B) -> return A > B
    ks_obj __gt__;
    // type.__ge__(A, B) -> return A >= B
    ks_obj __ge__;
    // type.__eq__(A, B) -> return A == B
    ks_obj __eq__;
    // type.__ne__(A, B) -> return A != B
    ks_obj __ne__;

    // type.__neg__(A) -> return -A
    ks_obj __neg__;

    // type.__sqig__(A) -> return ~A
    ks_obj __sqig__;

};

struct ks_tuple {
    KS_OBJ_BASE

    // the number of items in the tuple
    ks_size_t len;

    // the address of the first item. The tuple is allocated with the items in the main buffer,
    // so this acts as the offset from the object pointer to the values
    // So, allocating a tuple is like: malloc(sizeof(struct ks_tuple) + len * sizeof(ks_obj))
    ks_obj elems[0];

};

struct ks_list {
    KS_OBJ_BASE

    // the number of items in the list
    ks_size_t len;

    // the array of elements, indexable starting at 0 to (len-1)
    ks_obj* elems;

};

struct ks_dict {
    KS_OBJ_BASE

    // the number of (active) entries in the hash table
    ks_size_t n_entries;

    struct ks_dict_entry {
        // hash(key), stored for efficiency reasons
        ks_hash_t hash;

        // the key of this entry
        // A reference is held to this
        ks_obj key;

        // the value of this entry
        // A reference is held to this
        ks_obj val;

    }* entries;

    // the number of buckets in the hash table (normally is a prime number)
    ks_size_t n_buckets;

    // the array of buckets (each bucket is an index into the 'entries' array, or -1 if it is empty, -2 if it has been deleted)
    int* buckets;

};

// special data structure for easier to read initialization from C, essentially
// each entry has a C-style string and a ks_obj that does not have an active reference in most cases
// i.e.:
// ks_dict_set_cn(dict, (ks_dict_ent_c[]){{"ExampleKey", (ks_obj)ks_int_new(43)}, {NULL, NULL}})
// creates a dictionary with NO REFERENCE LEAKS
// Normally, setting a 'new' object (like from ks_int_new()) requires the callee to call KS_DECREF after
//   the function has ran, but functions using this 'consume' the reference
typedef struct {

    // NUL-terminated key (NULL key means this is the last C-style entry for the dictionary)
    char* key;

    // the value for the entry
    ks_obj val;

} ks_dict_ent_c;


// ks_none - the 'none' type in kscript
typedef struct {
    KS_OBJ_BASE

}* ks_none;

// the global singleton none
KS_API extern ks_none KS_NONE;

// down-casted constants so returning from functions is simpler
#define KSO_NONE ((ks_obj)KS_NONE)

// ks_bool - type representing either a 'true' or a 'false' value
// NOTE: These objects should not be created; just use the global singletons 'KS_TRUE' and 'KS_FALSE'
typedef struct {
    KS_OBJ_BASE

    // the actual boolean value
    bool val;

}* ks_bool;

// the global singletons for 'true' and 'false' respectively
KS_API extern ks_bool KS_TRUE, KS_FALSE;

// down-casted constants so returning from functions is simpler
#define KSO_TRUE ((ks_obj)KS_TRUE)
#define KSO_FALSE ((ks_obj)KS_FALSE)

// convert to a boolean
#define KSO_BOOL(_val) ((_val) ? KSO_TRUE : KSO_FALSE)


// ks_int - type representing an integer value (signed, 64 bit) in kscript
typedef struct ks_int {
    KS_OBJ_BASE

    // the actual integer value
    int64_t val;

}* ks_int;


// number of small integers to cache internally
// all integers with abs(x) <= KS_SMALL_INT_MAX
// are stored here
#define KS_SMALL_INT_MAX 255

// array of small integers:
// KS_SMALL_INTS[val + KS_SMALL_INT_MAX]
KS_API extern struct ks_int KS_SMALL_INTS[];


// ks_float - type representing a floating point real number
typedef struct {
    KS_OBJ_BASE

    // the actual float value
    double val;

}* ks_float;

// Global singleton representing the 'NAN' value (not-a-number)
KS_API extern ks_float KS_NAN;


// ks_complex - represents a complex number with a real and imaginary components
typedef struct {
    KS_OBJ_BASE

    // the actual float value, a C99 type
    double complex val;

}* ks_complex;


// ks_str - type representing a string of characters. Internally, the buffer is length encoded & NUL-terminated
//   and the hash is computed at creation time
struct ks_str {
    KS_OBJ_BASE

    // the hash of the string, cached, because it seems to be useful to precompute them
    ks_hash_t v_hash;

    // the number of characters in the string, not including a NUL-terminator
    // len("Hello") -> 5
    ks_size_t len;

    // the actual string value. In memory, ks_str's are allocated so that taking `->chr` just gives the address of
    // the start of the NUL-terminated part of the string. The [2] is to make sure that sizeof(ks_str) will allow
    // for enough room for two characters (this is useful for the internal constants for single-length strings),
    // and so new strings can be created with: `malloc(sizeof(*ks_str) + length)`
    char chr[2];

};

// character strings, global singletons for strings with size 1 (or the empty string with size 0)
KS_API extern struct ks_str KS_STR_CHARS[];



// ks_parser - an integrated parser which can parse kscript & bytecode to
//   ASTs & code objects
typedef struct ks_tok ks_tok;

// enumeration of different token types
enum {

    // whether the token is a valid type
    KS_TOK_NONE = 0,

    // Represents a combination of multiple tokens of different types
    KS_TOK_COMBO,

    // an identifier (i.e. any valid variable name)
    KS_TOK_IDENT,

    // an integer numerical literal (i.e. '123', '345', etc)
    KS_TOK_INT,

    // an floating-point numerical literal (i.e. '123.0', '345.', etc)
    KS_TOK_FLOAT,

    // a string constant, wrapped in quotes (i.e. '"Abc\nDef"')
    KS_TOK_STR,

    // a comment token, which typically starts with '#' and goes until the end
    //   of the line
    KS_TOK_COMMENT,

    // a newline token, i.e. 
    //
    KS_TOK_NEWLINE,

    // an operator, i.e. '+', '-', '==', etc
    KS_TOK_OP,

    // End-Of-File token, always the last token for a given file
    KS_TOK_EOF,
    

    /** GRAMMAR CHARACTERS **/

    // a single left parenthesis i.e. '('
    KS_TOK_LPAR,
    // a single right parenthesis i.e. ')'
    KS_TOK_RPAR,

    // a single left bracket i.e. '['
    KS_TOK_LBRK,
    // a single right bracket i.e. ']'
    KS_TOK_RBRK,

    // a single left brace i.e. '{'
    KS_TOK_LBRC,
    // a single right brace i.e. '}'
    KS_TOK_RBRC,

    // a single dot/period i.e. '.'
    KS_TOK_DOT,
    // a single colon i.e. ':'
    KS_TOK_COL,
    // a single comma i.e. ','
    KS_TOK_COMMA,
    // a single colon i.e. ';'
    KS_TOK_SEMI,

};


// ks_parser - an integrated parser which can parse kscript & bytecode to
//   ASTs & code objects
typedef struct {
    KS_OBJ_BASE
    
    // the source code the parser is parsing on
    ks_str src;

    // the name of the source (human readable)
    ks_str src_name;

    // the current token index into the 'tok' array
    int toki;

    // number of tokens that were found
    int tok_n;

    // the array of tokens in the source code
    ks_tok* tok;

}* ks_parser;


// ks_tok - kscript token from parser
// These are not full 'objects', because that would require a lot of memory,
//   objects, and pointers for parsers. Many files have upwards of 10k tokens,
//   so allocating 10k objects & maintaining reference counts, etc would not
//   be feasible or as efficient
// Therefore, this structure does not hold a reference to 'parser',
//   since it is a part of a parser at all times, and the integer members
//   describe where in the source code the token is found
struct ks_tok {

    // the parser the token came from
    ks_parser parser;

    // the type of token, one of the KS_TOK_* enum values
    int type;

    // absolute position & length in the string source code
    int pos, len;

    // the line & column at which it first appeared
    int line, col;

};

// ks_code - a bytecode object, which holds instructions to be executed
typedef struct {
    KS_OBJ_BASE

    // A reference to a list of constants, which are indexed by integers in the bytecode
    ks_list v_const;

    // human readable name for the code object
    ks_str name_hr;

    // number of bytes currently in the bytecode (bc)
    int bc_n;

    // a pointer to the actual bytecode, starting at index 0, through (bc_n-1)
    ksb* bc;


    // the number of meta-tokens, describing the input
    int meta_n;

    // array of meta-data tokens, which tell where the bytecode is located in the source code
    struct ks_code_meta {

        // the point at which this token is the relevant one
        int bc_n;

        // the token (a reference is held to tok.parser)
        ks_tok tok;

    }* meta;

}* ks_code;


// Different kinds of ASTs
enum {
    // Represents a constant, such as 'none', 'true', 'false', int, string
    // value is 'children[0]'
    KS_AST_CONST,

    // Represents a variable reference
    // name is 'children[0]'
    KS_AST_VAR,

    // Represents a list constructor, like [1, 2, 3]
    // elements are in children
    KS_AST_LIST,

    // Represents a tuple constructor, like (1, 2, 3)
    // elements are in children
    KS_AST_TUPLE,

    // Represents an attribute reference, 'children[0].(children[1])'
    // the value is 'children[0]' (AST), but the attribute is a string, in 'children[1]'
    KS_AST_ATTR,

    // Represents a function call, func(*args)
    // func is 'children[0]'
    // args are 'children[1:]'
    KS_AST_CALL,
    
    // represents a subscript call, i.e. obj[*args]
    // obj is 'children[0]'
    // args are 'children[1:]
    KS_AST_SUBSCRIPT,

    // Represents a return statement (a return without a result should be filled
    //   with a 'none' constant)
    // result is 'children[0]'
    KS_AST_RET,

    // Represents a throw statement
    // expression is 'children[0]'
    KS_AST_THROW,

    // Represents an assertion, which requires 'children[0]' to evaluate to true
    KS_AST_ASSERT,

    // Represents a block of other ASTs
    // all children are in 'children'
    KS_AST_BLOCK,

    /** CONTROL STRUCTURES **/

    // represents a conditional block, which can have a variable number of children
    // 'children[0]' is the primary conditional, children[1] os the body of the if
    // If there is a 3rd child, then it is the 'else' block
    // 'else if' is the same as having the entire child be an else section, so that
    //   is what is done
    // 
    // EX:
    // if (COND1) { BODY1 }
    // elif (COND2) { BODY2 }
    // else { BODY3 }
    // Will decompose into:
    // if [COND1, BODY1, [if COND2, BODY2, BODY3]
    KS_AST_IF,

    // represents a while loop, with continued iteration
    // 'children[0]' is the main condition, 'children[1]' is the body
    // And if there exists 'children[2]', that is the 'else' body
    // The 'else' body is only executed if the FIRST call of the condition is false
    // So:
    // while (x > 3) { ... } else { throw 'Incorrect x' }
    // Is the same (effectively) as:
    // if (x <= 3) { throw 'Incorrect x' }
    // while (x > 3) { ... }
    KS_AST_WHILE,

    // represents a 'try' block, which will try a given expression
    // children[0] is the code inside the try block, and 'children[1]' is the code in the 'catch' block
    KS_AST_TRY,

    // represents a 'func' definition, with a given name, parameter name list, and body
    // 'children[0]' is the function name (cast to ks_str)
    // 'children[1]' is the list of parameter names (cast to ks_list)
    // 'children[3]' is the body of the function, containing the code for the function
    KS_AST_FUNC,

    // represents a 'for' block, i.e. iterating through some iterable
    // 'children[0]' is the item being iterated through (AST)
    // 'children[1]' is the body to execute on each run (AST)
    // 'children[2]' is the variable to assign to
    KS_AST_FOR,


    /** BINARY OPERATORS **/

    // binary '+'
    KS_AST_BOP_ADD,
    // binary '-'
    KS_AST_BOP_SUB,
    // binary '*'
    KS_AST_BOP_MUL,
    // binary '/'
    KS_AST_BOP_DIV,
    // binary '%'
    KS_AST_BOP_MOD,
    // binary '**'
    KS_AST_BOP_POW,

    // binary '<=>'
    KS_AST_BOP_CMP,

    // binary '<'
    KS_AST_BOP_LT,
    // binary '<='
    KS_AST_BOP_LE,
    // binary '>'
    KS_AST_BOP_GT,
    // binary '>='
    KS_AST_BOP_GE,
    // binary '=='
    KS_AST_BOP_EQ,
    // binary '!='
    KS_AST_BOP_NE,

    // binary (short circuit) 'or'
    KS_AST_BOP_OR,
    // binary (short circuit) 'or'
    KS_AST_BOP_AND,


    // binary '=' (special case, only assignable things area allowed on the left side)
    KS_AST_BOP_ASSIGN,


    /** UNARY OPERATORS **/

    // unary '-'
    KS_AST_UOP_NEG,
    // unary '~'
    KS_AST_UOP_SQIG,
    // unary '!'
    KS_AST_UOP_NOT

};

// the first AST kind that is a binary operator
#define KS_AST_BOP__FIRST KS_AST_BOP_ADD

// the last AST kind that is a binary operator
#define KS_AST_BOP__LAST KS_AST_BOP_ASSIGN

// the first AST kind that is a unary operator
#define KS_AST_UOP__FIRST KS_AST_UOP_NEG

// the last AST kind that is a unary operator
#define KS_AST_UOP__LAST KS_AST_UOP_NOT



// ks_ast - an Abstract Syntax Tree, a high-level representation of a program/expression
typedef struct {
    KS_OBJ_BASE

    // the kind of AST it is
    int kind;

    // the array of children nodes. They are packed differently per kind, so see the definitino
    //   for a kind first
    ks_list children;

    // tokens for the AST, representing where it is in the source code
    ks_tok tok, tok_expr;


}* ks_ast;


// ks_cfunc - a C-function wrapper which can be called the same as a kscript function
typedef struct {
    KS_OBJ_BASE

    // human readable name (default: <cfunc @ ADDR>)
    ks_str name_hr;

    // the actual C function which can be called
    ks_obj (*func)(int n_args, ks_obj* args);

}* ks_cfunc;


// ks_kfunc - a k-script function
typedef struct {
    KS_OBJ_BASE

    // human readable name (default: <cfunc @ ADDR>)
    ks_str name_hr;

    // list of dictionaries to look up values in
    ks_list closures;


    // number of parameters (positional) that is takes
    int n_param;

    // list of formal parameters for the function
    struct ks_kfunc_param {
        // the name of the variable
        ks_str name;
    }* params;


    // the bytecode to execute
    ks_code code;

}* ks_kfunc;


// Construct a new kfunc from bytecode and a human readable name
KS_API ks_kfunc ks_kfunc_new(ks_code code, ks_str name_hr);

// create a new copy of the kfunc
KS_API ks_kfunc ks_kfunc_new_copy(ks_kfunc func);

// add a parameter name
KS_API void ks_kfunc_addpar(ks_kfunc self, ks_str name);


// ks_pfunc - a partial function wrapper, which wraps a callable with some of the arguments prefilled
// This is useful for member functions, for example, which have their first argument prefilled
typedef struct {
    KS_OBJ_BASE

    // the base function that will be called, and have its arguments filled in
    ks_obj func;

    // the number of arguments to fill in
    int n_fill;

    // list of indices & values to prefill in the call to func
    struct ks_pfunc_fill_arg {
        // 0-based index of where to insert it
        int idx;

        // the actual value provided as a prefill for the argument
        ks_obj arg;

    }* fill;

}* ks_pfunc;


// ks_Error - base class for an error. 
// NOTE: do not confuse this with 'ks_error' - that is a printing macro
typedef struct {
    KS_OBJ_BASE

    // attribute dictionary
    ks_dict attr;

}* ks_Error;


/* EXECUTION */

// ks_stk_frame - represents a single point of execution
typedef struct {
    KS_OBJ_BASE

    // the function being called in the stack frame,
    // normally a 'ks_cfunc', or a 'ks_code' object
    ks_obj func;

    // dictionary of local variables, iff type(func)==kfunc,
    // otherwise, NULL
    ks_dict locals;

    // the kfunct it is executing on, or NULL if not
    ks_kfunc kfunc;

    // the program counter
    // Only valid if typeof(func) == bytecode
    ksb* pc;

}* ks_stack_frame;


// create a new stack frame
// NOTE: This returns a new reference
ks_stack_frame ks_stack_frame_new(ks_obj func);

// ks_mutex - a mutual exclusion type, which can be locked over threads to sync
//   access or code execution
// NOTE: Most code does not need to use a mutex, as the GIL will restrict access
//   to interpreting code. However, in some cases it may be useful to atomicize entire
//   operations by using a mutex
typedef struct {
    KS_OBJ_BASE

    /* internal pthread object */

    pthread_mutex_t* _mut;

}* ks_mutex;


// The Global Interpreter Lock (GIL) for kscript, which is shared between threads
// Only 1 thread may access this at once, and thus it must be locked before use, and unlocked after.
// NOTE: Don't use `ks_mutex_*` functions on this, use `ks_GIL_*` functions (like `ks_GIL_lock()`)
//   to manage GIL access
KS_API extern ks_mutex ks_GIL;

// Acquire the Global Interpreter Lock (GIL) for the current thread. If the current thread does not
//   already have the GIL, this function blocks until it is available
KS_API void ks_GIL_lock();

// Let go of the Global Interpreter Lock (GIL) for the current thread.
// It should have been acquired via `ks_GIL_lock()` prior to calling this function
KS_API void ks_GIL_unlock();



// Construct a new, unlocked, mutex, which can be used for the mutual exclusion principle across threads
// NOTE: This returns a new reference
KS_API ks_mutex ks_mutex_new();

// Lock a mutex
KS_API void ks_mutex_lock(ks_mutex self);

// Unlock a mutex
KS_API void ks_mutex_unlock(ks_mutex self);


// ks_thread - a thread that is currently executing code
typedef struct {
    KS_OBJ_BASE

    /* internal pthread implementation (do not touch!) */

    // the underlying pthread object
    pthread_t _pth;

    // whether or not the underlying pthread is still active
    bool _pth_active;

    // list of 'ks_thread' objects which are sub threads
    ks_list sub_threads;

    // true iff the thread currently owns the GIL
    bool hasGIL;


    /* general variables about the thread */

    // readable name for the thread, or just the address
    ks_str name;


    /* execution variables */

    // the functor object to execute
    ks_obj target;

    // list of arguments
    ks_tuple args;

    // the result of the function call
    ks_obj result;


    // the stack frames being called,
    // all of them must be of type 'ks_stack_frame'
    ks_list stack_frames;

    // the stack for the thread
    ks_list stk;


    /* exception/error handling */

    // the currently raised exception (or NULL if it did not exist)
    ks_obj exc;

    // the exception's info when it was thrown (or NULL, if one has not been thrown yet)
    ks_tuple exc_info;


}* ks_thread;


// construct a new kscript thread
// if 'name==NULL', then a random name is generated
KS_API ks_thread ks_thread_new(char* name, ks_obj func, int n_args, ks_obj* args);

// start executing the thread
KS_API void ks_thread_start(ks_thread self);

// join the thread back
KS_API void ks_thread_join(ks_thread self);

// Get the current thread object
// NOTE: Does *NOT* return a new reference to the thread, so do not DECREF it!
// NOTE: Only returns NULL if it is outside a kscript thread, which should never be the case
//   outside of about 20 lines of 'main()' code in the interpreter itself. You shouldn't check
//   NULL-status of the result of this function
KS_API ks_thread ks_thread_get();


// internal execution method to execute 'code' on the current thread and then return
// the result, or 'NULL' if there was an exception
// NOTE: Don't use this function, unless you know what you're doing!
// Use `ks_call()` for most execution needs
KS_API ks_obj ks__exec(ks_code code);



/* MODULES/EXTENSIONS */

// ks_module - a type representing an entire module, which can be generated
// from C, or generated by a kscript file
typedef struct {
    KS_OBJ_BASE

    // attributes in the module
    ks_dict attr;

}* ks_module;


// construct a new module, given a name
// NOTE: Returns a new reference
KS_API ks_module ks_module_new(char* mname);

// search for and return a module, if successful
// 'mname' can be just the module name (i.e. 'mypackage')
// NOTE: throws an error if not found
// NOTE: Returns a new reference
KS_API ks_module ks_module_import(char* mname);


/* I/O */

// Bitset for the I/O stream capabilities
enum {
    KS_IOS_NONE    = 0x0,

    // The I/O stream can read bytes
    KS_IOS_READ    = 0x1,

    // The I/O stream can write bytes
    KS_IOS_WRITE   = 0x2,

    // The I/O stream is currently open
    KS_IOS_OPEN    = 0x4,

    // The I/O stream is an external stream, of which the kscript object is just a wrapper over
    // For example, stdout/stderr/stdin are wrapped as extern I/O streams
    KS_IOS_EXTERN  = 0x8,

};

typedef struct {
    KS_OBJ_BASE

    // the 'KS_IOS_*' enum, see above for comments
    uint32_t ios_flags;

    // the underlying file pointer
    FILE* fp;

}* ks_iostream;

// Create a blank 'iostream', with no target
// NOTE: Use `ks_iostream_open()` to actually get a target
// NOTE: Returns a new reference
KS_API ks_iostream ks_iostream_new();

// Create an IOstream wrapper around an already existing file pointer, with the mode it was created with
// NOTE: Returns a new reference
KS_API ks_iostream ks_iostream_new_extern(FILE* fp, char* mode);

// Attempt to open a created iostream (via ks_iostream_new()), for a given file and mode:
// TODO: document 'mode'
// Returns whether the operation was successful, and if not, throws an error
KS_API bool ks_iostream_open(ks_iostream self, char* fname, char* mode);

// Attempt to close an iostream
// Returns whether it was successfull, and if not, throws an error
KS_API bool ks_iostream_close(ks_iostream self);

// Read a up to a number of bytes from the iostream, consuming them and returning a string with their
//   bytes
// If there was a problem, return NULL and throw an error
// NOTE: Returns a new reference
KS_API ks_str ks_iostream_readstr_n(ks_iostream self, ks_ssize_t sz);

// attempt to write a string, returning whether it was successful
// If it was not, an error will be thrown
KS_API bool ks_iostream_writestr(ks_iostream self, ks_str data);

// Return the current position in the IOstream, or -1 if there was an error (and throw an error in that case)
KS_API ks_ssize_t ks_iostream_tell(ks_iostream self);

// Seek to a given position in the file, as an absolute offset
// NOTE: Returns -1 if there was an error
KS_API ks_ssize_t ks_iostream_seek(ks_iostream self, ks_ssize_t pos);

// Get the size of the I/O stream, in bytes
// NOTE: Returns -1 if there was an error
KS_API ks_ssize_t ks_iostream_size(ks_iostream self);




/* ITERATOR TYPES */

// ks_list_iter - list iterable object
// SEE: types/list.c
typedef struct {
    KS_OBJ_BASE

    // the object it is iterating on
    ks_list obj;

    // the current position in the list
    int pos;

}* ks_list_iter;


// ks_tuple_iter - tuple iterable object
// SEE: types/tuple.c
typedef struct {
    KS_OBJ_BASE

    // the object it is iterating on
    ks_tuple obj;

    // the current position in the list
    int pos;

}* ks_tuple_iter;


// ks_dict_iter - dict iterable object
// SEE: types/dict.c
typedef struct {
    KS_OBJ_BASE

    // the object it is iterating on
    ks_dict obj;

    // current entry position
    int pos;

}* ks_dict_iter;


// Create a new list iterator for a given list
// NOTE: Returns a new reference
KS_API ks_list_iter ks_list_iter_new(ks_list obj);

// Create a new tuple iterator for a given tuple
// NOTE: Returns a new reference
KS_API ks_tuple_iter ks_tuple_iter_new(ks_tuple obj);

// Create a new dict iterator for a given dict
// NOTE: Returns a new reference
KS_API ks_dict_iter ks_dict_iter_new(ks_dict obj);


/* STRING BUILDING/UTILITY TYPES */

// ks_str_b - a string building utility to make string concatenation simpler
//   and more efficient
// NOTE: This is not a 'ks_obj', just an internal utility class, therefore
// 'KS_DECREF()' and family are not used in this
// SEE: fmt.c
typedef struct {

    // the current length of the string builder
    int len;

    // the current character data for the string builder
    char* data;

} ks_str_b;


// Initialize the string builder
// NOTE: This must be called before `_get` or `_add*` methods
KS_API void ks_str_b_init(ks_str_b* self);

// Create a (new reference) of a string from the string builder at this point
KS_API ks_str ks_str_b_get(ks_str_b* self);

// Add character bytes to the string builder
KS_API void ks_str_b_add(ks_str_b* self, int len, char* data);

// Add a NUL-terminated C-style string to the buffer
KS_API void ks_str_b_add_c(ks_str_b* self, char* cstr);

// Add a formatted string (formmated by ks_vfmt), and then appended to the string buffer
KS_API void ks_str_b_add_fmt(ks_str_b* self, char* fmt, ...);

// Add repr(obj) to the string builder, returns true if it was fine, false if there was an error
KS_API bool ks_str_b_add_repr(ks_str_b* self, ks_obj obj);

// add str(obj) to the string buffer
KS_API bool ks_str_b_add_str(ks_str_b* self, ks_obj obj);

// Free the string builder, freeing all internal resources (but not the built strings)
KS_API void ks_str_b_free(ks_str_b* self);

/* BUILTIN TYPES (see 'types/' directory) */

// these are the built-in types
KS_API extern ks_type 
    ks_type_type,
    
    ks_type_none,
    ks_type_bool,
    ks_type_int,
    ks_type_float,
    ks_type_complex,
    ks_type_str,
    ks_type_tuple,
    ks_type_list,
    ks_type_dict,

    // error types
    ks_type_Error,
    ks_type_SyntaxError,
    ks_type_MathError,
    ks_type_ArgError,
    ks_type_IOError,
    ks_type_KeyError,
    ks_type_SizeError,
    ks_type_AttrError,
    ks_type_TypeError,
    ks_type_AssertError,
    ks_type_OpError,
    ks_type_ToDoError,

    // special error; used to signal the end of an iterator
    ks_type_OutOfIterError,

    ks_type_stack_frame,
    ks_type_mutex,
    ks_type_thread,

    ks_type_code,
    ks_type_ast,
    ks_type_parser,
    
    ks_type_cfunc,
    ks_type_kfunc,

    ks_type_module,

    // I/O
    ks_type_iostream,

    // iterators
    ks_type_list_iter,
    ks_type_tuple_iter,
    ks_type_dict_iter

;


/* BUILTIN FUNCTIONS (see ./funcs.c) */

KS_API extern ks_cfunc
    ks_F_repr,
    ks_F_hash,
    ks_F_print,
    ks_F_exit,
    ks_F_sleep,
    ks_F_len,
    ks_F_typeof,
    ks_F_import,
    ks_F_iter,
    ks_F_next,
    ks_F_open,
    ks_F_sort,
    ks_F_map,
    ks_F_range,

    // operators

    ks_F_add,
    ks_F_sub,
    ks_F_mul,
    ks_F_div,
    ks_F_mod,
    ks_F_pow,

    ks_F_cmp,
    ks_F_lt,
    ks_F_le,
    ks_F_gt,
    ks_F_ge,
    ks_F_eq,
    ks_F_ne,

    ks_F_neg,
    ks_F_sqig,

    ks_F_getattr,
    ks_F_setattr,

    ks_F_getitem,
    ks_F_setitem,

    // specialty

    ks_F_run_file,
    ks_F_run_expr,
    ks_F_run_interactive

;

// global variables, i.e. the builtin types and a few of the functions
KS_API extern ks_dict ks_globals;

// internal globals, used by the compiler & interpreter. Users should not be
//   modifying/using these, as there is no guarantee what it may or may not contain
KS_API extern ks_dict ks_internal_globals;

// the paths to search for things (similar to 'PYTHONPATH')
KS_API extern ks_list ks_paths;


/* GENERIC/GENERAL LIBRARY FUNCTIONS */

// Attempt to initialize the library. Return 'true' on success, 'false' otherwise
KS_API bool ks_init();

// Type to hold a kscript version
typedef struct {
    
    // the semver <major>.<minor>.<patch> build
    int major, minor, patch;

    // the build type of kscript, typically either 'release' or 'debug'
    const char* build_type;

    // the date of the compilation
    //   which is the '__DATE__' macro when it was compiled
    const char* date;

    // the time of the compilation,
    //   which is the '__TIME__' macro when it was compiled
    const char* time;

} ks_version_t;

// Get the version of kscript
// NOTE: Do not free or modify this variable
KS_API const ks_version_t* ks_version();

// Return the time, in seconds, since the library started. It uses a fairly good wall clock,
//   but is only meant for rough approximation. Using the std time module is best for most results (TODO)
KS_API double ks_time();

// Sleep (i.e. yield the thrad) for a given duration (in seconds).
// Will emit a warning if a syscall (i.e. nanosleep on linux) gives an error code/warning
//   but will NOT raise an exception
KS_API void ks_sleep(double dur);

/* LOGGING */

// return the current logging level, one of KS_LOG_* enum values
KS_API int ks_log_level();

// set the logging level to `new_level`
KS_API void ks_log_level_set(int new_level);

// generically log given a level, the current file, line, and a C-style format string, with a list of arguments
// NOTE: don't use this, use the macros like `ks_info`, and `ks_warn`, which make it easier to use the logging
//   system
KS_API void ks_log(int level, const char *file, int line, const char* fmt, ...);

// prints a trace message, assuming the current log level allows for it
#define ks_trace(...) ks_log(KS_LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
// prints a debug message, assuming the current log level allows for it
#define ks_debug(...) ks_log(KS_LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
// prints a info message, assuming the current log level allows for it
#define ks_info(...)  ks_log(KS_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
// prints a warn message, assuming the current log level allows for it
#define ks_warn(...)  ks_log(KS_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
// prints a error message, assuming the current log level allows for it
#define ks_error(...) ks_log(KS_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

// disable tracing
#ifdef KS_C_NO_TRACE
#undef ks_trace
#define ks_trace(...)
#endif

// disable debug
#ifdef KS_C_NO_DEBUG
#undef ks_debug
#define ks_debug(...)
#endif

// Implementation similar to 'printf', but using custom kscript formatting options:
// %i - 32 bit integer, prints an integer value
// %l - 64 bit integer, prints an integer value
// %f - 64 bit double, prints a floating point value
// %p - void* (or any pointer type), prints address in hex (i.e. '0xab983')
// %s - char*, prints a NULL-terminated C-style string
// kscript specifiers:
// %S - ks_obj, prints `str(obj)`
// %R - ks_obj, prints `repr(obj)`
// %T - ks_obj, prints `type(obj)`
// %O - ks_obj, prints stupid simple format `<'%T' obj @ %p>`
KS_API void ks_printf(const char* fmt, ...);


/* MEMORY MANAGEMENT */

// Allocate at least 'sz' bytes, and return a pointer to that memory. `ks_malloc(0)` is guaranteed to return non-NULL
// NOTE: This memory must be free'd by `ks_free(ptr)`, and reallocated using `ks_realloc(ptr, new_sz)`
KS_API void* ks_malloc(ks_size_t sz);

// Attempt to reallocate 'ptr' (which was created using `ks_malloc`) to be at least 'new_sz' bytes. 
// NOTE: `ks_realloc(NULL, sz)` is the same as doing `ks_malloc(sz)`
KS_API void* ks_realloc(void* ptr, ks_size_t new_sz);

// Attempt to free 'ptr' (which must have been allocated using `ks_malloc` or `ks_realloc`)
// NOTE: `ks_free(NULL)` is a guaranteed NO-OP
KS_API void ks_free(void* ptr);

// Return the current amount of memory being used, or -1 if memory usage is not being tracked
KS_API int64_t ks_mem_cur();

// Return the maximum amount of memory that has been used at one time, or -1 if memory usage is not being tracked
KS_API int64_t ks_mem_max();


/* CREATING/DESTROYING PRIMITIVES */

/* TYPE */

// Initialize a type variable. Make sure 'self' has not been ref cnted, etc. Just an allocated blob of memory!
// NOTE: Returns a new reference
KS_API void ks_init_type(ks_type self, char* name);

// check if 'self' is a sub type of 'of'
KS_API bool ks_type_issub(ks_type self, ks_type of);

// add a parent to the type, which the type will derive from
KS_API void ks_type_add_parent(ks_type self, ks_type parent);

// Get an attribute for the given type
// 0 can be passed to 'hash', and it will be calculated
// NOTE: Returns a new referece
KS_API ks_obj ks_type_get(ks_type self, ks_str key);

// Get a member function (self.attr), with the first argument filled as 'obj'
//   as the instance
// NOTE: Returns a new referece
KS_API ks_obj ks_type_get_mf(ks_type self, ks_str attr, ks_obj obj);

// Set an attribute for the given type
// 0 can be passed to 'hash', and it will be calculated
KS_API void ks_type_set(ks_type self, ks_str key, ks_obj val);

// Set a C-style string key as the attribute for a type
KS_API void ks_type_set_c(ks_type self, char* key, ks_obj val);

// Sets a list of C-entries (without creating new references)
// result == 0 means no problems
// result < 0 means there was some internal problem (most likely the key was not hashable)
// NOTE: References in `ent_cns` are consumed by this function! So if you continue using the values,
//   use `KS_NEWREF()` to create another reference to pass in `ent_cns`
// EXAMPLE:
// ks_type_set_cn(ks_type_int, (ks_dict_ent_c[]){
//   {"__str__", (ks_obj)ks_cfunc_new(mystr)},
//   {"mine", KS_NEWREF(myotherval)},
//   {NULL, NULL} // end should look like this   
// })
KS_API int ks_type_set_cn(ks_type self, ks_dict_ent_c* ent_cns);



/* INT */

// Create a new kscript int from a C-style integer value
// NOTE: Returns a new reference
KS_API ks_int ks_int_new(int64_t val);



/* FLOAT */

// Create a new kscript int from a C-style integer value
// NOTE: Returns a new reference
KS_API ks_float ks_float_new(double val);


/* COMPLEX */

// Create a new kscript complex from a C-style complex value
// NOTE: Returns a new reference
KS_API ks_complex ks_complex_new(double complex val);


/* STR */

// Create a new kscript string from a C-style NUL-terminated string
// NOTE: Returns a new reference
KS_API ks_str ks_str_new(char* val);

// Create a new kscript string from a C-style length encoded string
// NOTE: Returns a new reference
KS_API ks_str ks_str_new_l(char* chr, int len);

// perform a string comparison on 2 strings
KS_API int ks_str_cmp(ks_str A, ks_str B);

// Escape the string 'A', i.e. replace '\' -> '\\', and newlines to '\n'
// NOTE: Returns a new reference
KS_API ks_str ks_str_escape(ks_str A);

// Undo the string escaping, i.e. replaces '\n' with a newline
// NOTE: Returns a new reference
KS_API ks_str ks_str_unescape(ks_str A);

/* STRING FORMATTING (see ./fmt.c) */

// Perform C-style formatting, with various arguments
// TODO: document format specifiers
// NOTE: Returns a reference
KS_API ks_str ks_fmt_c(const char* fmt, ...);

// Perform variadic C-style formating, with a list of arguments
// TODO: document format specifiers
// NOTE: Returns a reference
KS_API ks_str ks_fmt_vc(const char* fmt, va_list ap);


/* TUPLE */

// Create a new kscript tuple from an array of elements, or an empty tuple if `len==0`
// NOTE: Returns a new reference
KS_API ks_tuple ks_tuple_new(int len, ks_obj* elems);

// Create a new kscript tuple from an array of elements, or an empty tuple if `len==0`
// NOTE: This variant does not create references to the objects!, so don't call DECREF on 'elems'
// NOTE: Returns a new reference
KS_API ks_tuple ks_tuple_new_n(int len, ks_obj* elems);

// Create a tuple representing a version
// NOTE: Returns a new reference
KS_API ks_tuple ks_tuple_new_version(int major, int minor, int patch);


/* LIST */

// Create a new kscript list from an array of elements, or an empty list if `len==0`
// NOTE: Returns a new reference
KS_API ks_list ks_list_new(int len, ks_obj* elems);

// Create a list from iterating through an iterator, draining it completely
// NOTE: Returns a new reference
KS_API ks_list ks_list_from_iterable(ks_obj obj);

// Clear a list, emptying the contents
KS_API void ks_list_clear(ks_list self);

// Push an object on to the end of the list, expanding the list
KS_API void ks_list_push(ks_list self, ks_obj obj);

// Push 'n' objects on to the end of the list, expanding the list
KS_API void ks_list_pushn(ks_list self, int n, ks_obj* objs);

// Pop off an object from the end of the list
// NOTE: Returns a reference
KS_API ks_obj ks_list_pop(ks_list self);

// Pop off 'n' items into 'dest'
// NOTE: Returns a reference to each object
KS_API void ks_list_popn(ks_list self, int n, ks_obj* dest);

// Pop off an object from the end of the list, destroying the reference
KS_API void ks_list_popu(ks_list self);


/* DICT */

// Create a new kscript dictionary from an array of entries (which should be 'len' number of key, val pairs)
// Example:
// ks_dict_new(3, (ks_obj[]){ key1, val1, key2, val2, key3, val3 })
// NOTE: Returns a new reference
KS_API ks_dict ks_dict_new(int len, ks_obj* entries);

// Merge in entries of 'src' into self, replacing any entries in 'self' that existed
KS_API void ks_dict_merge(ks_dict self, ks_dict src);

// Create a new kscript dictionary from an array of C-style strings to values, which will not create new references to values
// The last key is 'NULL'
// For example:
// ks_dict_new_cn((ks_dict_ent_cn[]){ {"Cade", ks_int_new(42)}, {"Brown", ks_str_new("asdfasdf"), {NULL, NULL}} });
// Will create a dictionary, and not introduce any memory leaks
// NOTE: References in `ent_cns` are consumed by this function! So if you continue using the values,
//   use `KS_NEWREF()` to create another reference to pass in `ent_cns`
KS_API ks_dict ks_dict_new_cn(ks_dict_ent_c* ent_cns);

// Test whether the dictionary has a given key. `hash` is always `hash(key)`. If it is 0, then 
//   attempt to calculate `hash(key)`. If it is 0, there is no error, but the dictionary is said to
//   not have the key
// For efficiency reasons, this allows the caller to precompute the hash from some other source,
//   so the dictionary doesn't have to
KS_API bool ks_dict_has(ks_dict self, ks_hash_t hash, ks_obj key);

// Get a value of the dictionary
// NULL if it does not exist
// NOTE: Returns a new reference
KS_API ks_obj ks_dict_get(ks_dict self, ks_hash_t hash, ks_obj key);

// Get a value of the dictionary
// NULL if it does not exist
// NOTE: Returns a new reference
KS_API ks_obj ks_dict_get_c(ks_dict self, char* key);

// Set a dictionary entry for a key, to a value
// If the entry already exists, dereference the old value, and replace it with the new value
// result > 0 means that an item was replaced
// result == 0 means no item was replaced, and there were no problems
// result < 0 means there was some internal problem (most likely the key was not hashable)
KS_API int ks_dict_set(ks_dict self, ks_hash_t hash, ks_obj key, ks_obj val);

// Delete an entry to the dictionary, returning 'true' if it was successful, false if it wasn't
KS_API bool ks_dict_del(ks_dict self, ks_hash_t hash, ks_obj key);

// Sets a list of C-entries (without creating new references)
// result == 0 means no problems
// result < 0 means there was some internal problem (most likely the key was not hashable)
// NOTE: References in `ent_cns` are consumed by this function! So if you continue using the values,
//   use `KS_NEWREF()` to create another reference to pass in `ent_cns`
KS_API int ks_dict_set_cn(ks_dict self, ks_dict_ent_c* ent_cns);


/* ERROR */

// Construct a new error from a string reason
// NOTE: Returns a new reference
KS_API ks_Error ks_Error_new(ks_str what);

// create a kscript error from a C style string
// NOTE: Returns a new reference
KS_API ks_Error ks_Error_new_c(char* what);

/* CODE */

// Create a new kscript code object, with a given constant list. The constant list can be non-empty,
//   in which case new constants will be allocated starting at the end. Cannot be NULL
// NOTE: Returns a new reference
KS_API ks_code ks_code_new(ks_list v_const);

// Output it to a binary encoded file, returning whether it was successful
KS_API bool ks_code_tofile(ks_code self, char* fname);

// Attempt to read from a binary file, returning 'NULL' if there was an error
KS_API ks_code ks_code_fromfile(char* fname);

// Append an array of bytecode to 'self'
KS_API void ks_code_add(ks_code self, int len, ksb* data);

// Add a constant to the internal constant list, return the index at which it was added (or already located)
KS_API int ks_code_add_const(ks_code self, ks_obj val);

// add a meta token (and hold a reference to the parser)
KS_API void ks_code_add_meta(ks_code self, ks_tok tok);

/*** ADDING BYTECODES (see ks.h for bytecode definitions) ***/
KS_API void ksca_noop      (ks_code self);

KS_API void ksca_push      (ks_code self, ks_obj val);
KS_API void ksca_dup       (ks_code self);
KS_API void ksca_popu      (ks_code self);

KS_API void ksca_list      (ks_code self, int n_items);
KS_API void ksca_tuple     (ks_code self, int n_items);

KS_API void ksca_getitem   (ks_code self, int n_items);
KS_API void ksca_setitem   (ks_code self, int n_items);

KS_API void ksca_call      (ks_code self, int n_items);
KS_API void ksca_ret       (ks_code self);
KS_API void ksca_throw     (ks_code self);
KS_API void ksca_assert    (ks_code self);
KS_API void ksca_jmp       (ks_code self, int relamt);
KS_API void ksca_jmpt      (ks_code self, int relamt);
KS_API void ksca_jmpf      (ks_code self, int relamt);

KS_API void ksca_try_start (ks_code self, int relamt);
KS_API void ksca_try_end   (ks_code self, int relamt);

KS_API void ksca_closure   (ks_code self);
KS_API void ksca_new_func  (ks_code self);

KS_API void ksca_make_iter (ks_code self);
KS_API void ksca_iter_next (ks_code self, int relamt);

KS_API void ksca_load      (ks_code self, ks_str name);
KS_API void ksca_load_attr (ks_code self, ks_str name);
KS_API void ksca_store     (ks_code self, ks_str name);
KS_API void ksca_store_attr(ks_code self, ks_str name);

KS_API void ksca_bop       (ks_code self, int ksb_bop_type);
KS_API void ksca_uop       (ks_code self, int ksb_uop_type);

KS_API void ksca_truthy    (ks_code self);

// C-style versions
KS_API void ksca_load_c      (ks_code self, char* name);
KS_API void ksca_load_attr_c (ks_code self, char* name);
KS_API void ksca_store_c     (ks_code self, char* name);
KS_API void ksca_store_attr_c(ks_code self, char* name);


/* AST (Abstract Syntax Trees) */

// Create an AST representing a constant value
// Type should be none, bool, int, or str
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_const(ks_obj val);

// Create an AST representing a variable reference
// Type should always be string
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_var(ks_str name);

// Create an AST representing a list constructor
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_list(int n_items, ks_ast* items);

// Create an AST representing a list constructor
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_tuple(int n_items, ks_ast* items);



// Create an AST representing an attribute reference
// Type should always be string
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_attr(ks_ast obj, ks_str attr);

// Create an AST representing a function call
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_call(ks_ast func, int n_args, ks_ast* args);

// Create an AST representing a subscript
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_subscript(ks_ast obj, int n_args, ks_ast* args);

// Create an AST representing a return statement
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_ret(ks_ast val);

// Create an AST representing a throw statement
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_throw(ks_ast expr);

// Create an AST representing an assertion
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_assert(ks_ast expr);


// Create an AST representing a block of code
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_block(int num, ks_ast* elems);

// Create an AST representing an 'if' construct
// 'else_body' may be NULL, in which case it is constructed without an 'else' body
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_if(ks_ast cond, ks_ast if_body, ks_ast else_body);

// Create an AST representing an 'while' construct
// 'else_body' may be NULL, in which case it is constructed without an 'else' body
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_while(ks_ast cond, ks_ast while_body, ks_ast else_body);


// Create an AST representing a 'try' block
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_try(ks_ast try_body, ks_ast catch_body, ks_str catch_name);

// Create an AST representing a function definition
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_func(ks_str name, ks_list params, ks_ast body);

// Create an AST representing a for loop
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_for(ks_ast iter_obj, ks_ast body, ks_str assign_to);



// Create a new AST represernting a binary operation on 2 objects
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R);

// Create a new AST represernting a unary operation
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_uop(int uop_type, ks_ast V);


/* CODE GENERATION */

// Generate corresponding bytecode for a given AST
// NOTE: Returns a new reference
// See `codegen.c` for more info
KS_API ks_code ks_codegen(ks_ast self);


/* PARSER */

// Create a new parser from some source code
// Or, return NULL if there was an error (and 'throw' the exception)
// NOTE: Returns a new reference
KS_API ks_parser ks_parser_new(ks_str src_code, ks_str src_name);

// Parse a single expression out of 'p'
// NOTE: Returns a new reference
KS_API ks_ast ks_parser_parse_expr(ks_parser self);

// Parse a single statement out of 'p'
// NOTE: Returns a new reference
KS_API ks_ast ks_parser_parse_stmt(ks_parser self);

// Parse the entire file out of 'p', returning the AST of the program
// Or, return NULL if there was an error (and 'throw' the exception)
// NOTE: Returns a new reference
KS_API ks_ast ks_parser_parse_file(ks_parser self);


// combine A and B to form a larger meta token
KS_API ks_tok ks_tok_combo(ks_tok A, ks_tok B);

// convert token to a string with mark
KS_API ks_str ks_tok_expstr(ks_tok tok);

// convert token to string, just the 2 relevant lines
KS_API ks_str ks_tok_expstr_2(ks_tok tok);

/* CFUNC */

// Create a new C-function wrapper
// NOTE: Returns a new reference
KS_API ks_cfunc ks_cfunc_new(ks_obj (*func)(int n_args, ks_obj* args));

// Create a new C-function wrapper with a given signature
// NOTE: Returns a new reference
KS_API ks_cfunc ks_cfunc_new2(ks_obj (*func)(int n_args, ks_obj* args), char* hrname);


/* PFUNC */

// Create a new partial function wrapper
// NOTE: 'func' must be callable
// NOTE: Returns a new reference
KS_API ks_pfunc ks_pfunc_new(ks_obj func);

// Fill a given index with an argument
// NOTE: if 'idx' is already filled, it will be replaced
KS_API void ks_pfunc_fill(ks_pfunc self, int idx, ks_obj arg);


/* OBJECT INTERFACE (see ./obj.c) */
// NOTE: This should be replaced by a bunch of standard 'cfunc' objects that
//   can be called

// Free an object, by either calling its deconstructor or freeing the memory
KS_API void ks_obj_free(ks_obj obj);

// Return the length of the object (len(obj)) as an integer.
// Negative values indicate there was an exception
KS_API int64_t ks_len(ks_obj obj);

// Return the hash of the object (hash(obj)) as an integer.
// A return value of '0' indicates that there was some error with the hash function
//   (a hash function should never return 0)
KS_API ks_hash_t ks_hash(ks_obj obj);

// Return whether or not `A==B`. If the comparison is undefined, return 'false'
KS_API bool ks_eq(ks_obj A, ks_obj B);

// Return whether or not 'func' is callable as a function
KS_API bool ks_is_callable(ks_obj func);

// Return whether or not 'obj' is iterable, through the `iter()` and `next()` protocol
KS_API bool ks_is_iterable(ks_obj obj);

// Return whether or not 'obj' is a 'truthy' value, which is primarily defined by:
//  * the value of 'obj', if 'obj' is a boolean
//  * if 'obj' is non-zero if 'obj' is a numeric type
//  * if 'len(obj)' is non-zero if it is an iterable
//  * if 'type(obj).__bool__(self)' is defined, it is called, and its result is used
// The result is -1 if there was a failure/exception in attempting to convert 'obj' to a boolean,
// 0 if it was falsy, and 1 if it was truthy
KS_API int ks_truthy(ks_obj obj);

// Attempt to call 'func' on 'args', returning NULL if there was an error
// NOTE: Returns a new reference
KS_API ks_obj ks_call(ks_obj func, int n_args, ks_obj* args);

// Attempt to call 'func' on 'args', returning NULL if there was an error
// NOTE: Sets the local variables to 'locals', which can be NULL to generate a new dictionary
// NOTE: Returns a new reference
KS_API ks_obj ks_call2(ks_obj func, int n_args, ks_obj* args, ks_dict locals);

/* EXCEPTION HANDLING */

// Throw an object up the call stack
// NOTE: Throws an error if there is already an object on the call stack
// NOTE: Always returns NULL
KS_API void* ks_throw(ks_obj obj);

// Throw an error with a given format string, with an optional 'errtype' (which)
//   should always be allowed to set the '.what' attribute on
// NOTE: Throws an error if there is already an object on the call stack
// NOTE: Always returns NULL
KS_API void* ks_throw_fmt(ks_type errtype, char* fmt, ...);

// Attempt to catch an object from the call stack
// Returns 'NULL' if nothing has been thrown,
// otherwise, return the object that was thrown, and take it off the thrown location
// (so now other things can be thrown)
// NOTE: Returns a new reference, if it was non-NULL
KS_API ks_obj ks_catch();

// catches excetion, also setting stack info
KS_API ks_obj ks_catch2(ks_list stk_info);

// catch & ignore any error, resetting the error state
KS_API void ks_catch_ignore();


// if there was an error, print stack trace and exit
// only call if there was an error! (this should really only be called by ks_thread's class)
// TODO: make this more repeatable and general
KS_API void ks_errend();


/* MISC. UTILS/FUNCTIONS */

// Attempt to open and read an entire file indicated by 'fname'.
// Throw an exception if it failed
KS_API ks_str ks_readfile(char* fname);

// Implementation of GNU getline function, reading an entire line from a FILE pointer
// Always ks_free(line) after done with this function, as this function reallocates buffers
//   to fit a line
// 'n' is not the line length, but the internal buffer size
// Example:
// char* line = NULL;
// size_t bufsize = 0;
// int len = ks_getline(&line, &len, fp);
// ks_free(line);
// NOTE: Returns -1 at the end of file
KS_API int ks_getline(char** lineptr, size_t* n, FILE* fp);


// structure describing a C-extension initializer
struct ks_module_cext_init {

    // initialize and import the function
    ks_module (*init_func)();

};

#ifdef __cplusplus
}
#endif

#endif /* KS_H__ */
