/* ks.h - main header file for the kscript C API
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


// include configuration header
#include <ks-config.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>

#include <complex.h>


// include either the full version of GMP or the miniature version for
//   integer types
#if defined(KS_HAVE_GMP)
#include <gmp.h>
#else
#include <ks-mini-gmp.h>
#endif


/* --- Macros --- */


// KS_OBJ_BASE - use this macro whenever creating an object type, this must be the first thing
//   defined in the struct `{ ... }` block
#define KS_OBJ_BASE ks_type type; int64_t refcnt;


// Initialize an object of a given type, essentially setting its type as well as
// Setting its reference count to '1' (since it should be created with a reference)
// NOTE: This also increments the reference count of '_type', since objects of
//   a given type hold a reference to that type
#define KS_INIT_OBJ(_obj, _type) { \
    ks_obj _tobj = (ks_obj)(_obj); ks_type _ttype = (ks_type)(_type); \
    _tobj->type = _ttype; \
    _tobj->refcnt = 1; KS_INCREF(_ttype); \
}

// Uninitialize an object, i.e. unrecord the reference it has to it's type
#define KS_UNINIT_OBJ(_obj) {      \
    ks_obj _tobj = (ks_obj)(_obj); \
    KS_DECREF(_tobj->type);        \
}

// Record a reference to a given object (i.e. increment the reference count)
// EX: KS_INCREF(obj)
#define KS_INCREF(_obj) { ++((ks_obj)_obj)->refcnt; }

// Un-record a reference to a given object (i.e. decrement the reference count)
// NOTE: If the reference count reaches 0 (i.e. the object has became unreachable), this frees
//   the object
// EX: KS_DECREF(obj)
#define KS_DECREF(_obj) { ks_obj fobj = (ks_obj)(_obj); if (--fobj->refcnt <= 0) { ks_obj_free(fobj, __FILE__, __func__, __LINE__); } }

// This will create a new reference to '_obj', for ease of use returning functions
// NOTE: This also downcasts to 'ks_obj'
#define KS_NEWREF(_obj) ks_newref((ks_obj)(_obj))


// Allocate memory for a new object type (uses `ks_malloc`)
// For example: `KS_ALLOC_OBJ(ks_int)` will allocate a `ks_int`
// Use the type that is a pointer to the actual type, as it will construct the size of a dereferenced type
#define KS_ALLOC_OBJ(_type) ((_type)ks_malloc(sizeof(*(_type){NULL})))

// Free an object's memory (non-recursively; just the actual object pointer)
// Use this macro on things created with `KS_ALLOC_OBJ(_type)`
#define KS_FREE_OBJ(_obj) (ks_free((void*)(_obj)))


// This will declare a ks_type variable of name `_type`, and an internal structure of `_type`_s
// EXAMPLE: KS_TYPE_DECLFWD(ks_type_int) defines `ks_type_int_s` and `ks_type_int`, but `ks_type_int`
//   is a static address; not allocated. So, the type is not generated at runtime, but rather is constant
// This is mainly used internally
#define KS_TYPE_DECLFWD(_type) struct ks_type_s _type##_s; ks_type _type = &_type##_s;


// This will define a function with '_name'+_, as a kscript C-extension function
// i.e.: KS_FUNC(add) will define a function called `add_`
#define KS_FUNC(_name) ks_obj _name##_(int n_args, ks_obj* args)

// This will define a function with '_type'_'_name'+_, as a kscript C-extension function
// i.e.: KS_TFUNC(int, add) will define a function called `int_add_`
#define KS_TFUNC(_type, _name) ks_obj _type##_##_name##_(int n_args, ks_obj* args)


// A number of references for 'infinite' objects that should never be freed
// (this number being large prevents 'free' functions from being re-ran)
#define KS_REFS_INF 0x10FFFFFFFFULL


/* C typedefs */

// define size types
typedef uint64_t ks_size_t;
typedef int64_t ks_ssize_t;

// a hash type, representing a hash of an object
typedef uint64_t ks_hash_t;


/* --- Builtin Types --- */

// Simple/general types

// ks_none - the 'none' object
typedef struct ks_none_s* ks_none;

// ks_obj - the most general type, which all other objects can be down-casted to
typedef struct ks_obj_s* ks_obj;

// ks_type - represents a type in kscript, which can be used for reflection, or any other usage
typedef struct ks_type_s* ks_type;

// ks_str - a string type, representing a list of characters. This type is immutable, so DO NOT MODIFY the bytes
//   in a string
typedef struct ks_str_s* ks_str;

// ks_list - a simple list type, holding references to other objects
typedef struct ks_list_s* ks_list;

// ks_dict - a dictionary which maps arbitrary keys to arbitrary values
typedef struct ks_dict_s* ks_dict;





struct ks_obj_s {
    KS_OBJ_BASE

};

struct ks_none_s {
    KS_OBJ_BASE

};

extern ks_none KS_NONE;

// 'none' downcasted to an object
#define KSO_NONE ((ks_obj)KS_NONE)

struct ks_type_s {
    KS_OBJ_BASE

    // attributes the type has
    ks_dict attr;


    /* special case attributes 
     *
     * We keep track of special attributes a type has, such as `__free__`, `__str__`, etc
     *
     * These do not hold an extra referece; since they are in the dictionary 'attr' anyway,
     *   but can make some internal routines faster since they can skip a dictionary lookup
     * 
     * For this reason, `ks_type_set_c()` should be used instead of accessing the dictionary directly
     * 
     */


    // type.__name__ - the string name of the type
    ks_str __name__;

    // type.__parents__ - list of parent types
    ks_list __parents__;

    // type.__free__(self) - free an object of a given type
    ks_obj __free__;

    // type.__str__(self) - convert a given object to a string
    ks_obj __str__;



};


struct ks_str_s {
    KS_OBJ_BASE

    // the value of the hash of the string contents, calculated via `ks_hash_bytes(chr, len_b)`
    ks_hash_t v_hash;

    // the length (in characters) of the string
    ks_size_t len;

    // the length (in bytes) of the buffer that the string takes up
    // NOTE: may be different than `len` for non-ascii sequences (i.e. unicode)
    ks_size_t len_b;

    // the actual string value. In memory, ks_str's are allocated so that taking `->chr` just gives the address of
    // the start of the NUL-terminated part of the string. The [2] is to make sure that sizeof(ks_str) will allow
    // for enough room for two characters (this is useful for the internal constants for single-length strings),
    // and so new strings can be created with: `malloc(sizeof(*ks_str) + len_b)`
    char chr[2];

};



// ks_str_builder - string builder, which is a mutable type that can generate strings
// This is used internally to reduce the overhead of string concatenation
typedef struct {
    KS_OBJ_BASE

    // the length (in bytes) that the string builder is currently
    ks_size_t len;

    // the data the string builder has
    void* data;

}* ks_str_builder;


// Numeric types

// ks_bool - boolean type representing a `true` or `false` value. Use the global singletons `KS_TRUE` and `KS_FALSE`,
//   or `KSO_BOOL(_cond)` to convert a C-style value to a kscript boolean
// NOTE: use `ks_num_get_*()` to retrieve a C-style integer from it. 
typedef struct ks_bool_s* ks_bool;

// ks_int - integer type in kscript, can hold any whole number value (i.e. not bit-limimted)
// NOTE: use `ks_num_get_*()` to retrieve a C-style integer from it. 
typedef struct ks_int_s* ks_int;

// ks_float - floating-point type in kscript, internally implemented as a double
// NOTE: use `ks_num_get_*()` to retrieve a C-style double from it. 
typedef struct ks_float_s* ks_float;

// ks_complex - floating-point complex type in kscript, i.e. a real and imaginary component
// NOTE: use `ks_num_get_*()` to retrieve a C-style double complex from it. 
typedef struct ks_complex_s* ks_complex;


struct ks_bool_s {
    KS_OBJ_BASE

    // the boolean value, which should not be required; you can do a pointer comparison if `val == KS_TRUE`
    bool val;
};

// global singletons
extern ks_bool KS_TRUE, KS_FALSE;

// create a boolean form a C-style conditional
#define KSO_BOOL(_cond) ((ks_obj)((_cond) ? (KS_TRUE) : (KS_FALSE)))


struct ks_int_s {
    KS_OBJ_BASE

    // union of possible types
    union {
        int64_t v64;
        mpz_t vz;
    };

    // if isLong, use `vz`, the long integer type, else
    //   use `v64`, the C integer type
    bool isLong;

};

struct ks_float_s {
    KS_OBJ_BASE

    // the value of the floating point number
    double val;

};

struct ks_complex_s {
    KS_OBJ_BASE

    // the value of the floating point complex number
    complex double val;

};


// ks_keyval_c - special structure representing key, value pairs for C-style constructors (like dictionaries)
//   taking a string key, and generic object `val`
// Arrays of these are NULL-terminated; once `keyval->key == NULL`, the end of the array is signalled
// NOTE: Use `KS_KEYVAL_END` to signal the end of an input, or `KS_KEYVALS()` to construct an array
// NOTE: Anything you put in a keyval will have it's reference taken! So, if you need another reference, use `KS_NEWREF(obj)` instead of just `obj`,
//   this is done to allow C-routines not have to clean up everything and instead the internal implementations have that
typedef struct {

    // the key (as a C-style string)
    const char* key;

    // the value
    ks_obj val;

} ks_keyval_c;

// end/invalid keyval
#define KS_KEYVAL_END ((ks_keyval_c){ NULL, NULL })

// array to create an array of `ks_keyval_c`, which can be passed to a constructor expecting a NULL-terminated array
#define KS_KEYVALS(...) ((ks_keyval_c[]){ __VA_ARGS__ KS_KEYVAL_END })




// Container Types


// ks_list - a dense list of other objects, indexable from [0, len)
struct ks_list_s {
    KS_OBJ_BASE

    // length, in elements, of the list
    ks_size_t len;

    // array of the elements
    ks_obj* elems;

};


// ks_tuple - immutable collection of objects
typedef struct {
    KS_OBJ_BASE

    // length, in elements, of the tuple
    ks_size_t len;

    // array of the elements
    // similar to ks_str, we simply allocate the extra bytes in the tuple (since it is immutable)
    ks_obj elems[0];

}* ks_tuple;


// A bucket will be this value if it is empty
#define KS_DICT_BUCKET_EMPTY     -1

// A bucket will be this value if it has been deleted
#define KS_DICT_BUCKET_DELETED   -2

struct ks_dict_s {
    KS_OBJ_BASE

    // the number of active entries in the dictionary (NOTE: some may have been deleted)
    ks_size_t n_entries;

    // array of entries; ordered by insertion order
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


    // the number of buckets in the hash table (normally, a prime number)
    ks_size_t n_buckets;

    // array of buckets (each bucket is an index into the 'entries' array, or a negative number to signal some special case (see KS_DICT_BUCKET_*))
    ks_ssize_t* buckets;

};


/* Callables */

// Function type
typedef ks_obj (*ks_cfunc_f)(int n_args, ks_obj* args);

// C-style function wrapper
typedef struct {
    KS_OBJ_BASE

    // C-function pointer to call
    ks_cfunc_f func;

    // human-readable name of the function (i.e. 'print')
    ks_str name_hr;

    // human-readable signature of the function (i.e. 'print(*args)')
    ks_str sig_hr;

}* ks_cfunc;




/* Errors */

// ks_Error - base type of all errors that can be thrown
typedef struct {
    KS_OBJ_BASE 

    // attributes of the error
    ks_dict attr;

    // attr['what'], but no extra reference
    ks_str what;

}* ks_Error;

/* Logging */

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
    KS_LOG_DEBUG = 1,
    
    // info, i.e. large operations (such as allocations/deallocations >= 10% of system memory), things such
    // as successful module imports, initialization information, etc
    KS_LOG_INFO  = 2,

    // warn, i.e. warning of odd/peculiar happenings that don't neccessarily halt the program, but should be
    // attended to, such as extremely large allocations >= 25% of system memory, a FILE read did not produce the
    // correct result to return (like fread(..., n, 1, fp) != n)), or a parameter was NULL
    // This is the default value for release software
    KS_LOG_WARN  = 3,

    // error, i.e. critical issues that cause problems if not paid any attention to.
    // this includes thrown exceptions which are not caught, a FILE was not opened correctly, 
    // a doublefree/corruption error occured, an object was NULL when it shouldn't have been
    // This is always printed out, can not be ignored by setting the logging level
    KS_LOG_ERROR = 4,


    KS_LOG__END
};


// ks_logger - internal logging class
typedef struct {
    KS_OBJ_BASE

    // see KS_LOG_* enum values for explanation of the levels
    int level;

    // name of the logger, for example 'ks_mem' for the internal memory logger
    ks_str name;

}* ks_logger;


/* Threading */


// ks_thread - represents a single thread of execution
typedef struct {
    KS_OBJ_BASE

    // the human-readable name of the thread
    ks_str name;

    // the function to call
    ks_obj target;

    // arguments to the thread
    int n_args;

    // list of arguments to the thread
    ks_obj* args;


    /* exceptions */

    // what exception was thrown (NULL if no error/exception)
    ks_obj exc;

}* ks_thread;


// main thread
extern ks_thread ks_thread_main;


/* Type Objects */

extern ks_type
    ks_T_obj,
    ks_T_none,
    ks_T_type,
    ks_T_str,
    ks_T_str_builder,

    ks_T_bool,
    ks_T_int,
    ks_T_float,
    ks_T_complex,

    ks_T_list,
    ks_T_tuple,
    ks_T_dict,

    ks_T_cfunc,

    ks_T_logger,

    ks_T_Error,
    ks_T_InternalError,
    ks_T_ArgError,
    ks_T_KeyError,

    ks_T_thread

;


// return a new reference to an object; use macro KS_NEWREF
static inline ks_obj ks_newref(ks_obj obj) {
    KS_INCREF(obj);
    return obj;
}


/* --- Functions --- */

// General library functions

// Initializes, kscript, returns whether it was successful or not
KS_API bool ks_init();

// Returns a hash of given bytes, using djb2-based hashing algorithm
KS_API ks_hash_t ks_hash_bytes(const uint8_t* data, ks_size_t sz);



// String formatting


// ks_fmt_* - formatting routines for strings
// NOTE: Returns a new reference, or NULL if an error was thrown
KS_API ks_str ks_fmt_vc(const char* fmt, va_list ap);
KS_API ks_str ks_fmt_c(const char* fmt, ...);


// ks_*printf(...) - prints out, similar to C-style ones, but using the `ks_fmt_*` methods
KS_API void ks_printf(const char* fmt, ...);
KS_API void ks_vfprintf(FILE* fp, const char* fmt, va_list ap);


// Loggin functions

// Get a C logger,
// If the requested name does not exist, behavior depends on 'createIfNeeded'
// If 'createIfNeeded' is set, then a new logger is created with a level of `KS_LOG_WARN`
// Else, an error is thrown and NULL is returned
KS_API ks_logger ks_logger_get(const char* logname, bool createIfNeeded);

// C logging function
// 'level' is the logging level (check KS_LOG_* enums)
// 'file' is the file it was sent at (this should be filled in by `ks_trace` & etc macros)
//   can be 'NULL' if you want to leave off the file
// 'line' is the current line of the file (filled in)
//   can be '-1' to not print (also, will not be printed if 'file==NULL')
// 'logname' is the identifier of the logger, for example 'ks_mem' is the internal one used by the memory logger
// NOTE: if 'logname' is not created yet, it will be created with 'KS_LOG_WARN' as the default level
//   use `ks_log_c_setlevel()`
KS_API void ks_log_c(int level, const char* file, int line, const char* logname, const char* fmt, ...);

// NOTE: if 'logname' is not created yet, it will be created with 'KS_LOG_WARN' as the default level
KS_API int ks_log_c_level(const char* logname);

// NOTE: if 'logname' is not created yet, it will be created with and its level will be initialized
KS_API void ks_log_c_set(const char* logname, int level);


// generically log given a level, the current file, line, and a C-style format string, with a list of arguments
// NOTE: don't use this, use the macros like `ks_info`, and `ks_warn`, which make it easier to use the logging
//   system
//KS_API void ks_log(int level, const char *file, int line, const char* fmt, ...);

// prints a trace message, assuming the current log level allows for it
#define ks_trace(_name, ...) ks_log_c(KS_LOG_TRACE, __FILE__, __LINE__, _name, __VA_ARGS__)
// prints a debug message, assuming the current log level allows for it
#define ks_debug(_name, ...) ks_log_c(KS_LOG_DEBUG, __FILE__, __LINE__, _name, __VA_ARGS__)
// prints a info message, assuming the current log level allows for it
#define ks_info(_name, ...)  ks_log_c(KS_LOG_INFO, __FILE__, __LINE__, _name, __VA_ARGS__)
// prints a warn message, assuming the current log level allows for it
#define ks_warn(_name, ...)  ks_log_c(KS_LOG_WARN, __FILE__, __LINE__, _name, __VA_ARGS__)
// prints a error message, assuming the current log level allows for it
#define ks_error(_name, ...) ks_log_c(KS_LOG_ERROR, __FILE__, __LINE__, _name, __VA_ARGS__)



// Memory related functions


// Allocate a block of memory guaranteed to hold at least `sz` bytes
// This pointer should only be used with `ks_realloc()` and `ks_free()` and `ks_*` functions!
// NOTE: Returns `NULL` and throws an error if there was a problem
KS_API void* ks_malloc(ks_size_t sz);

// Allocate a block of memory of size `n * sz`, i.e. `n` elements of size `sz`
// NOTE: Returns `NULL` and throws an error if there was a problem
KS_API void* ks_calloc(ks_size_t n, ks_size_t sz);

// Attempt to reallocate a given pointer to fit at least `sz` bytes, and return the new pointer
// NOTE: Returns `NULL` and throws an error if there was a problem, but the original pointer is not freed!
KS_API void* ks_realloc(void* ptr, ks_size_t sz);

// Free a pointer allocated by `ks_malloc`, `ks_calloc` or `ks_realloc`
KS_API void ks_free(void* ptr);



// General object manipulation

// Frees an object (i.e. calls a deconstructor)
// NOTE: Do not call this function! You should use `KS_DECREF()` on an object, which will call this if neccessary
KS_API void ks_obj_free(ks_obj obj, const char* file, const char* func, int line);

// Calculates a hash for an object
// NOTE: Returns success, or false and throws an error
KS_API bool ks_obj_hash(ks_obj obj, ks_hash_t* out);

// Calculate whether two objects are 'equal'
// NOTE: Ignores any errors generated while comparing them; if there was an error, this return false but does not throw anything
KS_API bool ks_obj_eq(ks_obj A, ks_obj B);

// Call func() with the given arguments
// NOTE: Returns the result of the function call, or NULL if an error was thrown
KS_API ks_obj ks_obj_call(ks_obj func, int n_args, ks_obj* args);

// Throw an object, return NULL 
KS_API ks_obj ks_obj_throw(ks_obj obj);


// Catch and ignore any object thrown
KS_API void ks_catch_ignore();

// Throw an object, return NULL (use ks_throw macro)
KS_API ks_obj ks_ithrow(const char* file, const char* func, int line, ks_type errtype, const char* fmt, ...);

// throw an error type, i.e.
// ks_throw(ks_T_Error, "My format: %i", 34);
#define ks_throw(_errtype, ...) ks_ithrow(__FILE__, __func__, __LINE__, _errtype, __VA_ARGS__)


// If there was an uncaught object on `th->exc`, print out the stack trace and quit
KS_API void ks_exit_if_err();

// Helper C functions


// Attempt to parse arguments to a Cfunc, and return whether it was successful.
// Given a format string (which describes which args should be parsed and of what types), set variables given
// NOTE: Returns success, or returns false and throws an error
KS_API bool ks_getargs(int n_args, ks_obj* args, const char* fmt, ...);


/* --- Builtins --- */

// Construct a new 'type' object, where `parent` can be any type that it will implement the same binary interface as
//   (giving NULL and ks_T_obj are equivalent)
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_type ks_type_new(ks_str name, ks_type parent);

// Initialize a 'type' object that is built as a C-type
// NOTE: May abort internally; this should only be used in well-tested C modules & internal features; use more general creation routines
//   like `ks_type_new()` for dynamically creating types and such
KS_API void ks_type_init_c(ks_type self, const char* name, ks_type parent, ks_keyval_c* keyvals);


// Set a given key,val pair to a type, returning success
// NOTE: Returns success
KS_API bool ks_type_set(ks_type self, ks_str key, ks_obj val);

// Set the given elements from C-style strings
// NOTE: Returns success
KS_API bool ks_type_set_c(ks_type self, ks_keyval_c* keyvals);

// Return whether 'self' is a sub-type of 'of'
KS_API bool ks_type_issub(ks_type self, ks_type of);



// Construct a new 'int' object
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_int ks_int_new(int64_t val);

// Create a kscript int from a string in a given base
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_int ks_int_new_s(char* str, int base);


// Construct a new 'float' object
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_float ks_float_new(double val);


// Construct a new 'complex' object
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_complex ks_complex_new(double complex val);



// Construct a new 'str' object, from a C-style string.
// NOTE: If `len<0`, then `len` is calculated by the C `strlen()` function
// NOTE: If `cstr==NULL`, then the empty string "" is returned, and `len` is not checked
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_str ks_str_new_c(const char* cstr, ks_ssize_t len);

// Create new string from Cstring
#define ks_str_new(_cstr) ks_str_new_c(_cstr, -1)

// Compare two strings, and return their string comparison (i.e. strcmp() in C)
KS_API int ks_str_cmp(ks_str A, ks_str B);

// Return whether two strings are equal (this may be faster than `ks_str_cmp`)
KS_API bool ks_str_eq(ks_str A, ks_str B);



// Create a new string builder
// NOTE: Returns new reference
KS_API ks_str_builder ks_str_builder_new();

// Add raw bytes to the string builder
// NOTE: Returns success
KS_API bool ks_str_builder_add(ks_str_builder self, void* data, ks_size_t len);

// Add str(obj) to the string builder
// NOTE: Returns success
KS_API bool ks_str_builder_add_str(ks_str_builder self, ks_obj obj);

// Add a given string formatting (see `ks_fmt_*` documentation for more)
// NOTE: Returns success
KS_API bool ks_str_builder_add_fmt(ks_str_builder self, const char* fmt, ...);
KS_API bool ks_str_builder_add_vfmt(ks_str_builder self, const char* fmt, va_list ap);



// Get the current string the builder has been building
// NOTE: Returns new reference
KS_API ks_str ks_str_builder_get(ks_str_builder self);



// Construct a new list from an array of elements
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_list ks_list_new(ks_size_t len, ks_obj* elems);

// Construct a new tuple from an array of elements
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_tuple ks_tuple_new(ks_size_t len, ks_obj* elems);



// Construct a new dictionary from key, val pairs (elems[0], elems[1] is the first, elems[2*i+0], elems[2*i+1] makes up the i'th pair)
// NOTE: `len%2==0` is a requirement!
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_dict ks_dict_new(ks_size_t len, ks_obj* elems);

// Construct a new dictionary from C-style initializers
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_dict ks_dict_new_c(ks_keyval_c* keyvals);

// Return the load factor for a given dictionary, between 0.0 and 1.0
KS_API double ks_dict_load(ks_dict self);

// Set `self[key] = val`
// Variants that include `_h`, which require that hash(key) is precomputed
// Variants that include `_c`, take a NUL-terminated C-string as the key
// Returns whether it was successful, if `false`, an error is thrown
KS_API bool ks_dict_set(ks_dict self, ks_obj key, ks_obj val);
KS_API bool ks_dict_set_h(ks_dict self, ks_obj key, ks_hash_t hash, ks_obj val);
KS_API bool ks_dict_set_c(ks_dict self, ks_keyval_c* keyvals);

// Return the object stored in the dictionary, keyed 'key'
// Variants that include `_h`, which require that hash(key) is precomputed
// Variants that include `_c`, take a NUL-terminated C-string as the key
// NOTE: Returns a new reference, or NULL if there was a problem (if NULL, an error was thrown)
KS_API ks_obj ks_dict_get(ks_dict self, ks_obj key);
KS_API ks_obj ks_dict_get_h(ks_dict self, ks_obj key, ks_hash_t hash);
KS_API ks_obj ks_dict_get_c(ks_dict self, char* key);

// Return whether or not the dictionary has a given value
// Variants that include `_h`, require that hash(key) is precomputed
// This can be efficient for data-storages which compute hash once and then keep it
// NOTE: Never throws an error
KS_API bool ks_dict_has(ks_dict self, ks_obj key);
KS_API bool ks_dict_has_h(ks_dict self, ks_obj key, ks_hash_t hash);
KS_API bool ks_dict_has_c(ks_dict self, char* key);

// Delete 'key' from the dictionary's entries
// Variants that include `_h`, which require that hash(key) is precomputed
// Variants that include `_c`, take a NUL-terminated C-string as the key
// Returns whether it was successful, if `false`, an error is thrown
KS_API bool ks_dict_del(ks_dict self, ks_obj key);
KS_API bool ks_dict_del_h(ks_dict self, ks_obj key, ks_hash_t hash);
KS_API bool ks_dict_del_c(ks_dict self, char* key);



// Construct a new Cfunc with given name and signature
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_cfunc ks_cfunc_new(ks_cfunc_f func, ks_str name_hr, ks_str sig_hr);

// Construct a new Cfunc with given signature (the function name is taken as the substring up to '(')
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_cfunc ks_cfunc_new_c(ks_cfunc_f func, const char* sig);


// Construct a new error of a given type, sets `.what` to the given string
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_Error ks_Error_new(ks_type errtype, ks_str what);


// construct a new kscript thread
// if 'name==NULL', then a name is generated
KS_API ks_thread ks_thread_new(const char* name, ks_obj target, int n_args, ks_obj* args);


// Return the current thread
// NOTE: NO reference is returned; do not call KS_DECREF() on this!
KS_API ks_thread ks_thread_get();

/* --- Global/Builtin Functions --- */

// functions, found in `funcs.c`
extern ks_cfunc
    ks_F_print


;


#ifdef __cplusplus
}
#endif

#endif /* KS_H__ */

