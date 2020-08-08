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

// The READLINE library is used for the active interpreter
#ifdef KS_HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif


// include either the full version of GMP or the miniature version for
//   integer types
#if defined(KS_HAVE_GMP)
#include <gmp.h>
#else
#include <ks-mini-gmp.h>
#endif



// for signal handling
#include <signal.h>



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
// (this number being large prevents 'free' functions from being re-ran frequently)
#define KS_REFS_INF 0x10FFFFFFFFULL



// Special macro to get arguments for internal functions
#define KS_GETARGS(...) { if (!ks_getargs(n_args, args, __VA_ARGS__)) return NULL; }

/* C typedefs */

// define size types
typedef uint64_t ks_size_t;
typedef int64_t ks_ssize_t;


// maximum value in `ks_size_t`
#define KS_SIZE_MAX (UINT64_MAX)

#define KS_SSIZE_MAX (INT64_MAX)

// a hash type, representing a hash of an object
typedef uint64_t ks_hash_t;

// max hash value
#define KS_HASH_MAX ((ks_hash_t)(((ks_hash_t)(1ULL)) << (8 * sizeof(ks_hash_t) - 1)))

// adder (useful for combining sequences)
#define KS_HASH_ADD ((ks_hash_t)3628273133ULL)

// multiplier (useful for combining sequences)
#define KS_HASH_MUL ((ks_hash_t)3367900313ULL)



/* Math/Numeric Constants */

// PI, (circle constant)
#define KS_M_PI    3.141592653589793238462643383279502884197169399375105820974944592307816406286

// E, (euler's number)
#define KS_M_E     2.718281828459045235360287471352662497757247093699959574966967627724076630353



// a single unicode character (NOT grapheme/etc, but rather a single, decoded value)
typedef int32_t ks_unich;

// include unicode
#include <ks-unicode.h>



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
// NOTE: See the file `types/str.c` for a more in depth explanation
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

enum ks_type_flags {

    // no special flags
    KS_TYPE_FLAGS_NONE      = 0x00,

    // Equal-Short-Circuit (EQSS), this means that if two pointers are equal, then the objects are equal
    KS_TYPE_FLAGS_EQSS      = 0x01,

};


struct ks_type_s {
    KS_OBJ_BASE

    // attributes the type has
    ks_dict attr;

    /* Flags
     *
     * These are flags specific to the type
     * 
     */
    uint32_t flags;


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

    // type.__new__(obj, *args) - construct a new object of a given type (like a constructor), this should normally take 1 argument 
    //   or more, and if `__init__` is not NULL, it should ALWAYS be called with `1`, and then `__init__` is called with the resultant
    //   object and the rest of the arguments.
    //
    // With immutable types (int, str, float, tuple, etc), the __new__() takes the value(s) to construct the type from,
    //   and there is no __init__ (something that is immutable must be created and initialized with any value in the __new__
    //   function). Therefore, the __new__ constructor takes arguments
    //
    // However, mutable types (i.e. most types) should have __new__() be callable with 0 arguments to simply generate a new
    //   type; with 'empty' values, then __init__() is called with arguments
    //
    // For example, when you call 'x = type(a, b, c)', 'type' is first checked if it has an __init__ method:
    //   * if __init__ is found, the code is effectively: 'tmp = type.__new__(); tmp.__type__ = type; type.__init__(tmp, a, b, c)'
    //       * the type assignment is required for sub-types that may use their parent type's '__new__()' method, so that
    //           the created value has the correct type
    //   * if __init__ is not found, the code is effectively: 'x = type.__new__(a, b, c);'
    ks_obj __new__, __init__;


    // type.__free__(self) - free an object of a given type
    ks_obj __free__;



    // type conversion to standard types
    ks_obj __bool__, __int__, __float__, __str__, __bytes__;


    // getattr/setattr - custom attribute getters & setters (i.e. `x.a = b`)
    ks_obj __getattr__, __setattr__;

    // getitem/setitem - custom item getting (i.e. `x[a] = b`)
    ks_obj __getitem__, __setitem__;



    // type.__repr__(self) - convert a given object to a (string) representation
    ks_obj __repr__;


    // len, hash, misc. properties
    ks_obj __len__, __hash__;


    // iter, next - iterator protocol
    ks_obj __iter__, __next__;


    // type.__call__(self, *args) - override the calling feature
    ks_obj __call__;





    // ops


    // +, -, abs, ~ operators (unary)
    ks_obj __pos__, __neg__, __abs__, __sqig__;

    // +, -, *, /, %, ** operators
    ks_obj __add__, __sub__, __mul__, __div__, __mod__, __pow__;

    // <, >, <=, >=, ==, !=, <=> operators
    ks_obj __lt__, __gt__, __le__, __ge__, __eq__, __ne__, __cmp__;

    // <<. >> operators
    ks_obj __lshift__, __rshift__;

    // |, &, ^ operators
    ks_obj __binor__, __binand__, __binxor__;


};



struct ks_str_s {
    KS_OBJ_BASE

    // the value of the hash of the string contents, calculated via `ks_hash_bytes(chr, len_b)`
    ks_hash_t v_hash;
    
    // length (in bytes) of the string
    ks_size_t len_b;

    // length (in characters) of the string
    // NOTE: For ASCII-data, len_c==len_b
    ks_size_t len_c;

    // number of characters per entry in the 'offs' array; the main purpose here is
    //   to amortize some costly operators (such as getting the index of the 'i'th codepoint),
    //   see more in `types/str.c`
    // IF ENABLED:
    //   * Every string takes up `sizeof(ks_size_t*)` more bytes
    //   * Strings which are not ASCII (i.e. most unicode strings) will take up an additional `sizeof(ks_size_t) * (len_c / KS_STR_OFF_EVERY)` bytes
    //   * String indexing (worst case) is O(1), doing at maximum `4 * KS_STR_OFF_EVERY` byte reads (4 comes from max length of character encoded in UTF8)
    //       to seek to a given position
    // IF DISABLED:
    //   * ASCII strings (i.e. most common) still have the same optimizations, and will save `sizeof(ks_size_t*)` per string
    //   * String indexing (worst case) is O(N), which means, technically, doing a really naive loop like `for i in range(len(st)) { x =  }
    // NOTE: define to '0' to disable this feature and accept suboptimal searching on large strings which use any
    //   unicode
    #ifndef KS_STR_OFF_EVERY
    #define KS_STR_OFF_EVERY   64
    //#define KS_STR_OFF_EVERY    0
    #endif /* KS_STR_OFF_EVERY */

    #if KS_STR_OFF_EVERY

    // array of character offsets into the 'chr' array
    // NOTE: will be 'NULL' if len_b==len_c; in that case, the 'i'th offset is just 'i'
    // Otherwise, it holds `len_c/KS_STR_OFF_EVERY` values, where `offs[i]` is the byte-offset into `chr` of the `i * KS_STR_OFF_EVERY`'th character
    //   in unicode
    // This is mainly used to amortize the cost of string indexing. Without a set of offsets
    ks_size_t* offs;

    #endif /* KS_STR_OFF_EVERY */

    // the actual array of characters. In memory, ks_str's are allocated so that taking `->chr` just gives the address of
    // the start of the NUL-terminated part of the string. The [SZ] is to make sure that sizeof(ks_str) will allow
    // for enough room for 2 bytes (this is useful for the internal constants for single-length strings),
    // and so new strings can be created with: `malloc(sizeof(*ks_str) + len_b)`
    // Every 'character' is a code point from the Universal Character Set (UCS), but we do not use UTF32 or anything; everything
    //   is utf8 internally
    char chr[2];

};

// Get whether a string is ascii-only data. This is true if the length of the UTF8-data is the same as bytes (i.e. every character is a single byte)
// If this condition is true, characters can be indexed on a byte-by-byte basis. This means a lot of operations can be a lot faster (indexing, slicing,
//   searching AND returning the index at which it is found)
#define KS_STR_ISASCII(_str) ((_str)->len_b == (_str)->len_c)


/* UTF8 special characters */

enum ks_utf8_err {

    // No error
    KS_UTF8_ERR_NONE              = 0,

    // Error, there was an attempted out-of-bounds read
    KS_UTF8_ERR_OUTOFBOUNDS       = 1,

};


// unicode character representing there was an error
#define KS_UNICH_WASERR ((ks_unich)-1)

// ks_str_citer - C-iterator for processing single codepoints in a string; Source code is in `types/str.c`
// you use it like:
// ```
// struct ks_str_citer cit = ks_str_citer_make(str_obj);
// ks_unich ch;
// while (!cit.done) {
//   ch = ks_str_citer_next(&cit);
//   if (cit.err) {
//     // handle error
//     break; 
//   }
//   if (ch == 'a') { // ... }
//   else {
//   // ERR, whatever you want 
//   }
// }
// ```
// No cleanup is neccessary, and no extra memory is taken
//
struct ks_str_citer {

    // the string object that the iterator is iterating through
    ks_str self;

    // whether or not the iteration is finished
    bool done;

    // error code that the iterator gives, 0 means no error, check `KS_UTF8_ERR_*` definitions for various errors
    enum ks_utf8_err err;

    // current character index
    int cchi;

    // current byte index
    int cbyi;

};

// Create a new C-style string iterator, starting at the beginning
KS_API struct ks_str_citer ks_str_citer_make(ks_str self);

// Get next unicode character, returns `KS_UNICH_WASERR` (a negative value) if there was an error that was thrown
KS_API ks_unich ks_str_citer_next(struct ks_str_citer* cit);

// Peek at the current unicode character, but do not change the state of 'cit'
KS_API ks_unich ks_str_citer_peek(struct ks_str_citer* cit);

// 'Seek' in the string to a given index, returning whether it was successful or not
KS_API bool ks_str_citer_seek(struct ks_str_citer* cit, ks_ssize_t idx);



// ks_bytes - immutable array of bytes
typedef struct ks_bytes_s {
    KS_OBJ_BASE

    // length (in bytes) of the array
    ks_size_t len_b;

    // hash(byt), precomputed via `ks_hash_bytes()`
    ks_hash_t v_hash;

    // array of bytes 
    // NOTE: similar to `ks_str`, since bytes are immutable, the bytes object is allocated as `sizeof(ks_bytes) + len_b - 1`
    // the [1] is so the global singletons for single bytes are allocated correctly
    uint8_t byt[1];

}* ks_bytes;


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


#define KSO_TRUE ((ks_obj)KS_TRUE)
#define KSO_FALSE ((ks_obj)KS_FALSE)

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

// macro to create an array of `ks_keyval_c`, which can be passed to a constructor expecting a NULL-terminated array
#define KS_KEYVALS(...) ((ks_keyval_c[]){ __VA_ARGS__ KS_KEYVAL_END })


// ks_enumval_c - special data structure for initializing enums in C
typedef struct {

    // the name of the enumeration
    const char* name;

    // the value
    int64_t val;

} ks_enumval_c;


// end of enum values
#define KS_ENUMVAL_END ((ks_enumval_c){ NULL, 0 })

// macro to create an array of `ks_enumval_c`, which can be passed to constructors
#define KS_ENUMVALS(...) ((ks_enumval_c[]){ __VA_ARGS__ KS_ENUMVAL_END })


// Create an enum entry for an actual C-declared enum
#define KS_ENUM_ENTRY_FILL(_enum) ((ks_enumval_c){ #_enum, (int64_t)(_enum) })
#define KS_EEF KS_ENUM_ENTRY_FILL


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



// ks_slice - a slice object, having a start, stop, step
typedef struct {
    KS_OBJ_BASE

    // start, stop, and step (default should be NONE for all of these)
    ks_obj start, stop, step;

}* ks_slice;


// ks_range - a range object, having a start, stop, step
typedef struct {
    KS_OBJ_BASE

    ks_int start, stop, step;

}* ks_range;


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


// struct ks_dict_citer - C iterator for dictionaries
// Use like:
//```
// struct ks_dict_citer cit = ks_dict_citer_make(dict_obj);
// ks_obj key, val;
// ks_hash_t hash;
// while (ks_dict_citer_next(&cit, &key, &val, &hash)) {
//   { code ... }
//   KS_DECREF(key);   
//   KS_DECREF(val);   
// }
//```
struct ks_dict_citer {

    // the dictionary it is iterating over
    ks_dict self;

    // current position in the elements list
    int64_t curpos;

};


// Create a new dictionary iterator for C
// NOTE: No cleanup is neccessary, because no references are made
KS_API struct ks_dict_citer ks_dict_citer_make(ks_dict dict_obj);


// Get the next element, and return whether it was successful (i.e. whether it had another entry)
// NOTE: No cleanup is neccessary, because no references are made
KS_API bool ks_dict_citer_next(struct ks_dict_citer* cit, ks_obj* key, ks_obj* val, ks_hash_t* hash);



// ks_namespace - a dictionary, but with attribute references rather than subscripting
typedef struct {
    KS_OBJ_BASE

    // attributes dictionary
    ks_dict attr;

}* ks_namespace;


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



// ks_pfunc - a wrapper around a function, filling in the first argument as a member instance (or more),
// when called, it behaves like `func(member_inst, *args)`
typedef struct {
    KS_OBJ_BASE

    // the (static) member function that takes the first argument as `member_inst`
    ks_obj func;

    // the 0th arg
    ks_obj member_inst;

}* ks_pfunc;

/* Enums */

// ks_Enum - base class for an enumeration
// other Enumerations may derive from this class, like for example:
// SideEnum derives from Enum,
// and SideEnum.LEFT and SideEnum.RIGHT are elements
// the Enum type itself has a list "_enum_keys", which is a list of string keys to the enum
typedef struct {
    KS_OBJ_BASE

    // the integer enumeration value
    ks_int enum_val;

    // the name of this particular enum value
    ks_str name;

}* ks_Enum;


/* Errors */

// ks_Error - base type of all errors that can be thrown
typedef struct {
    KS_OBJ_BASE 

    // attributes of the error
    ks_dict attr;

    // attr['what'], but no extra reference
    ks_str what;

}* ks_Error;

/* Modules/Importing */


// ks_module - type representing a module
typedef struct {
    KS_OBJ_BASE

    // atributes of the module
    ks_dict attr;

}* ks_module;


// internal structure used by the library to hold initialization
//   info
struct ks_module_cinit {

    // function that, when called, should return the module
    ks_module (*load_func)();

};



// Create a new module with a given name
// NOTE: Returns a new reference, or NULL if an error was thrown
KS_API ks_module ks_module_new(const char* mname);

// Attempt to import a module with a given name
// NOTE: Returns a new reference, or NULL if an error was thrown
KS_API ks_module ks_module_import(const char* mname);


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

    /* execution state */

    // list of `ks_stack_frame`'s that it is currently executing
    ks_list frames;

    // current stack of objects that the code is executing on
    ks_list stk;


    /* exceptions */

    // what exception was thrown (NULL if no error/exception)
    ks_obj exc;

    // list of stack frames (NULL if no error/exception)
    ks_list exc_info;

}* ks_thread;


/* Parsing / Code Generation */


// enumeration of different token types
enum {

    // None/error token type
    KS_TOK_NONE = 0,

    // Represents a combination of multiple tokens that may be of different types
    KS_TOK_COMBO,

    // a valid identifier (i.e. variable name, function name, etc)
    // NOTE: this may include unicode characters as well
    KS_TOK_IDENT,


    /* Value Literals */

    // a numerical literal, which may be an integer, float, complex, etc
    // Valid 'numbers' are:
    //  integer:
    //   [0-9]+
    //   0x[0-9a-fA-F]+
    //   0b[0-1]+
    //   0o[0-7]+
    //   0r[IVXLDCM]+ 
    //     NOTE: roman numeral literals also are checked for correctness
    //       (i.e. it is not just a regex check)
    //  floating point
    //   [0-9]+\.[0-9]*i? | [0-9]*+\.[0-9]+i?
    //   0x[0-9a-fA-F]+\.[0-9a-fA-F]*i? | 0x[0-9a-fA-F]*+\.[0-9a-fA-F]+i?
    // NOTE: Complex numbers (`a+bi`) are not a single constant, but include the `+` operation
    //   when parsed
    KS_TOK_NUMBER,

    // a string literal, which can be single quote delimited, or triple quote delimeted
    // Valid strings are (NOTE: regex doesn't work for multi-line and some escape sequences):
    //   "..."
    //   '...'
    //   """..."""
    //   '''...'''
    // Strings may include the following escape sequences:
    //   '\\' - a literal backslash `\`
    //   '\'' - a literal single quote: `'`
    //   '\"' - a literal double quote: `"`
    //   '\a' - ASCII: `BEL` (bell)
    //   '\b' - ASCII: `BS` (backspace)
    //   '\f' - ASCII: `FF` (formfeed)
    //   '\n' - ASCII: `LF` (linefeed)
    //   '\r' - ASCII: `CR` (carriage return)
    //   '\t' - ASCII: `TAB` (tab)
    //   '\v' - ASCII: `VT` (vertical tab)
    //   '\x$$' - A literal byte with the value `$$` (given in hex). Two characters are required
    //   '\u$$$$' - Unicode codepoint with 16bit value `$$$$`. Four characters are required
    //   '\U$$$$$$$$' - Unicode codepoint with 32bit value `$$$$$$$$`. 8 characters are required
    //   '\N{$}' - Unicode codepoint with a given name `$`, i.e. '\N{LATIN CAPITAL LETTER A}' == 'a'
    KS_TOK_STR,

    /* Control Sequences */

    // Represents a new line break
    // NOTE: `\r\n` and `\n` and other combinations will all be treated as a single new line
    KS_TOK_NEWLINE,

    // Represents the end of file, has length zero
    // This is mainly included as an implementation detail so internal code can always check `tokens[i + 1]` without
    //   worrying about indexing past the end of the array of tokens
    KS_TOK_EOF,


    /* Character Sequences (or singles) */

    // literal '.'
    KS_TOK_DOT,

    // literal ','
    KS_TOK_COMMA,
    
    // literal ':'
    KS_TOK_COL,

    // literal ';'
    KS_TOK_SEMICOL,

    // literal '('
    KS_TOK_LPAR,
    // literal ')'
    KS_TOK_RPAR,

    // literal '['
    KS_TOK_LBRK,
    // literal ']'
    KS_TOK_RBRK,

    // literal '{'
    KS_TOK_LBRC,
    // literal '}'
    KS_TOK_RBRC,

    // an operator, can be any operator, e.g. '+', '-'
    // NOTE: length should be checked, and then check
    //   the source code to see which operator
    KS_TOK_OP,

};

// ks_tok - kscript token from parser
// These are not full 'objects', because that would require a lot of memory,
//   objects, and pointers for parsers. Many files have upwards of 10k tokens,
//   so allocating 10k objects & maintaining reference counts, etc might slow
//   down quite a bit
// Therefore, this structure does not hold a reference to 'parser',
//   since it is a part of a parser at all times, and the integer members
//   describe where in the source code the token is found
struct ks_tok {

    // the type of token, one of the KS_TOK_* enum values
    int type;

    // position & length (in bytes) within the source code
    int pos_b, len_b;

    // the line & column (in characters, not neccesarily bytes!)
    int line, col;

};

// ks_parser - an integrated parser which can parse kscript & bytecode to
//   ASTs & code objects
typedef struct {
    KS_OBJ_BASE
    
    // the source code the parser is parsing on
    ks_str src;

    // the name of the source (human readable)
    ks_str src_name;

    // the file name (printed when using files)
    ks_str file_name;

    // the current token index into the 'tok' array
    int toki;

    // number of tokens that were found
    int tok_n;

    // the array of tokens in the source code
    struct ks_tok* tok;


}* ks_parser;


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
    // NOTE: `.data` (if non-NULL) gives pointer to `children->len` instances of `struct ks_ast_iter_data`
    //   describing each constructor, if there were any unpacking arguments
    KS_AST_LIST,

    // Represents a tuple constructor, like (1, 2, 3)
    // elements are in children
    // NOTE: `.data` (if non-NULL) gives pointer to `children->len` instances of `struct ks_ast_iter_data`
    //   describing each constructor, if there were any unpacking arguments
    KS_AST_TUPLE,

    // Represents a dictionary constructor, like {"key":"value", ...}
    // elements are in children, there should be an even number (for keys & values)
    KS_AST_DICT,

    // Represents a slice constructor, like 1:2
    // elements are in children
    KS_AST_SLICE,

    // Represents an attribute reference, '(children[0]).(children[1])'
    // the value is 'children[0]' (AST), but the attribute is a string, in 'children[1]'
    KS_AST_ATTR,

    // Represents a function call, func(*args)
    // func is 'children[0]'
    // args are 'children[1:]'
    // NOTE: `.data` (if non-NULL) gives pointer to `children->len` instances of `struct ks_ast_iter_data`
    //   describing each constructor, if there were any unpacking arguments
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
    // 'children[4]' is the list of values (ASTs) of default parameters
    // The first such default parameter is given via `ast->dflag` (that is the index of the first, or -1 for no defaults)
    KS_AST_FUNC,

    // represents a 'for' block, i.e. iterating through some iterable
    // 'children[0]' is the item being iterated through (AST)
    // 'children[1]' is the body to execute on each run (AST)
    // 'children[2:-1]' is the variable(s) being assigned to
    // NOTE: `.data` (if non-NULL) gives pointer to `children->len` instances of `struct ks_ast_iter_data`
    //   describing each constructor, if there were any unpacking arguments
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

    // binary '|'
    KS_AST_BOP_BINOR,
    // binary '&'
    KS_AST_BOP_BINAND,
    // binary '^'
    KS_AST_BOP_BINXOR,

    // binary '<<'
    KS_AST_BOP_LSHIFT,
    // binary '>>'
    KS_AST_BOP_RSHIFT,

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
    // binary (short circuit) 'and'
    KS_AST_BOP_AND,

    // binary '=' (special case, only assignable things area allowed on the left side)
    // NOTE: `.data` (if non-NULL) gives pointer to `children->len` instances of `struct ks_ast_iter_data`
    //   describing each constructor, if there were any unpacking arguments
    KS_AST_BOP_ASSIGN,


    /** UNARY OPERATORS **/

    // unary '+'
    KS_AST_UOP_POS,
    // unary '-'
    KS_AST_UOP_NEG,
    // unary '~'
    KS_AST_UOP_SQIG,
    // unary '!'
    KS_AST_UOP_NOT,
    // unary '*' (used for starred expressions)
    KS_AST_UOP_STAR


};

// the first AST kind that is a binary operator
#define KS_AST_BOP__FIRST KS_AST_BOP_ADD

// the last AST kind that is a binary operator
#define KS_AST_BOP__LAST KS_AST_BOP_ASSIGN

// the first AST kind that is a unary operator
#define KS_AST_UOP__FIRST KS_AST_UOP_POS

// the last AST kind that is a unary operator
#define KS_AST_UOP__LAST KS_AST_UOP_STAR


// extra data for `KS_AST_LIST` and `KS_AST_TUPLE`
struct ks_ast_iter_data {

    // if true, the argument is requested to unpack (i.e. it is an iterable,
    //   then all of its elements should be added to the result)
    bool doUnpack;

};

// ks_ast - an Abstract Syntax Tree, a high-level representation of a program/expression
typedef struct {
    KS_OBJ_BASE

    // the kind of AST it is
    int kind;

    // the array of children nodes. They are packed differently per kind, so see the definition
    //   for a kind first
    ks_list children;
    
    // tokens for the AST, representing where it is in the source code
    struct ks_tok tok;

    // any flags (these are used per AST; see above)
    int dflag;

}* ks_ast;



/* BYTECODE 
 *
 * The format/syntax is SIZE:[type desc], i.e. 4:[int thing it does]
 * 
 * All instructions are at least 1 byte, and they start with the op code (i.e. 'KSB_*')
 * 
 * Some bytecodes have indexes that refer to a constants array, which is held by the code object
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

    // Do nothing, just go to the next instruction. This should not be generated by code generators, but is included
    //   as a matter of having it
    // 1:[op]
    KSB_NOOP = 0,


    /** -- STACK MANIPULATION, these opcodes do simple operations on the stack -- **/

    // Push a constant value onto the stack
    // 1:[op] 4:[int index into the 'v_const' array of code constants]
    KSB_PUSH,

    // Push a new reference to the TOS on top of that, i.e.
    // | A B
    // ->
    // | A B B
    // 1:[op]
    KSB_DUP,

    // Pop off the TOS, and do not use it in any computation
    // 1:[op]
    KSB_POPU,

    // Pop off TOS, convert to a boolean through truthiness value, and then push back on the boolean
    // 1:[op]
    KSB_TRUTHY,


    /** -- OBJECT CONSTRUCTION, these opcodes create various types of objects -- **/

    // Pop off 3 elements from the stack and construct a 'slice' from them ('none's should be added
    //   if you want to leave some as 'default')
    // 1:[op]
    KSB_SLICE,

    // Pop off 'num_elems' from the stack, create a tuple from them, and then push that tuple
    //   back onto the stack
    // 1:[op] 4:[int num_elems, number of elements to take off the stack]
    KSB_TUPLE,

    // Pop off 'num_elems' from the stack, create a list from them, and then push that list
    //   back onto the stack
    // 1:[op] 4:[int num_elems, number of elements to take off the stack]
    KSB_LIST,

    // Add a number of elements to a list under the items on the stack
    // | list_obj a b c
    // | list_obj
    // 1:[op] 4:[int num_elems, number of elements to take off the stack]
    KSB_LIST_ADD_OBJS,

    // Pop off an iterable object, and add all elements to the object under it (which must be of type list)
    // | list_obj iter_obj
    // | list_obj
    // 1:[op] 
    KSB_LIST_ADD_ITER,

    // Pop off 'num_elems' from the stack, treat them as a bunch of keys & values (interleaved),
    //   and construct a dictionary from them. Then, push the dict back on the stack
    // i.e. {"Cade": "Brown", "Kscript": 2342} should have:
    // | "Cade" "Brown" "Kscript" 2342
    // Then, 'KSB_DICT[4]' will create the dictionary
    // 1:[op] 4:[int num_elems, aka num entries]
    KSB_DICT,

    // Create string from string interpolation
    // Essentially, pops on `"".join(str(stk[-num_elems:]))`, then pop off `num_elems` objects
    // 1:[op] 4:[int num_elems]
    KSB_BUILDSTR,


    /** -- CONTROL FLOW, these opcodes change the control flow of the program -- **/

    // Jump, unconditionally, a 'relamt' bytes in the bytecode, from the position where the next
    //   instruction would have been.
    // EX: jmp +0 is always a no-op, the program counter is not changed
    // EX: jmp -5 creates an infinite loop, because the jump instruction itself is 5 bytes,
    //       so it would jump back exactly one instruction
    // 1:[op] 4:[int relamt]
    KSB_JMP,

    // Pop off the TOS, get a truthiness value, and if it was truthy,
    //   jump a specified number of bytes
    // EX: jmpt +10 will skip 10 bytes if the TOS was truthy, otheriwse jumps 0
    // 1:[op] 4:[int relamt]
    KSB_JMPT,

    // Pop off the TOS, get a truthiness value, and if it was NOT truthy,
    //   jump a specified number of bytes
    // EX: jmpf +10 will skip 10 bytes if the TOS was falsey
    // 1:[op] 4:[int relamt]
    KSB_JMPF,

    // Pop off 'n_items', and perform a function call with them (the first object must be the function object)
    // The item on the bottom of the stack
    // Example: if the stack is | A B C , then KSB_CALL(n_items=3) will result in calculating 'A(B, C)' 
    //   and pushing that result back on the stack
    // If there are not 'n_items' on the stack, internally abort
    // 1:[op] 4:[int n_items]
    KSB_CALL,

    // Variable call, not from items on the stack, but from the last item
    // Stack is expected to be:
    // | func objs
    // Where `objs` is a LIST (not any iterable) of the arguments given
    // This operation is useful for unpacking arguments of variable size
    // 1:[op]
    KSB_VCALL,

    // Pop off the TOS, and return that as the return value of the currently executing function,
    //   causing the top item of the stack frame to be popped off
    // 1:[op]
    KSB_RET,

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

    // Pop off the TOS, and 'throw' it up the call stack, rewinding, etc. If there was no
    //   'catch' block set up, then it will cause the program to abort and print an error
    // 1:[op]
    KSB_THROW,

    // Pop off the TOS, and 'assert' it is true, or throw an exception
    // 1:[op]
    KSB_ASSERT,

    // Replace the function on top with a copy (i.e. new, distinct copy), which has defaults under it:
    // | defas... func
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

    
    /** -- VALUE LOOKUP -- **/

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

    // the '+' operator (i.e. +a)
    KSB_UOP_POS,
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
    // NOTE: There are no 'OR' or 'AND' operators, because those are ALWAYS defined via truthiness conversion,
    //   and cannot be overriden

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

    // the '|' operator
    KSB_BOP_BINOR,
    // the '&' operator
    KSB_BOP_BINAND,
    // the '^' operator
    KSB_BOP_BINXOR,
    
    // the '<<' operator
    KSB_BOP_LSHIFT,
    // the '>>' operator
    KSB_BOP_RSHIFT,

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


};


/* TYPES/STRUCTURE DEFS */

// try to pack these to single bytes
#pragma pack(push, 1)

// ksb - a single bytecode, i.e. sizeof(ksb) == 1
// can be part of the opcode, or any other parts
typedef uint8_t ksb;

// ksb_i32 - a sigle bytecode with a 32 bit signed integer component, sizeof(ksb_i32) == 5
typedef struct {

    // the operation itself (KSB_* enum)
    ksb op;

    // the argument encoded
    int32_t arg;

} ksb_i32;


// end single byte alignment
#pragma pack(pop)


// ks_code - a bytecode object, which holds instructions to be executed
typedef struct {
    KS_OBJ_BASE

    // human readable name for the code object
    ks_str name_hr;

    // A reference to a list of constants, which are indexed by integers in the bytecode
    ks_list v_const;


    // number of bytes currently in the bytecode (bc)
    int bc_n;

    // a pointer to the actual bytecode, starting at index 0, through (bc_n-1)
    // NOTE: it has variable length members, so you must traverse through the bytecode
    ksb* bc;


    // the parser (if non-NULL) that the code was created from
    ks_parser parser;

    // the number of meta-tokens, describing the input
    int meta_n;

    // array of meta-data tokens, which tell where the bytecode is located in the source code
    // these are used for creating error messages, and create traceback information
    // NOTE: I've designed this so that it is not a performance hit UNLESS there is an error/exception
    //   thrown, as this is very important
    struct ks_code_meta {

        // the point at which this token is the relevant one
        int bc_n;

        // the token, which holds a reference to the parser
        struct ks_tok tok;

    }* meta;

}* ks_code;



// ks_kfunc - a kscrip
typedef struct {
    KS_OBJ_BASE

    // human-readable name (default: <kfunc @ ADDR>)
    ks_str name_hr;


    // the bytecode to execute for the function
    ks_code code;

    // list of locals (i.e. dictionaries) for each closure that the function is wrapped in
    ks_list closures;

    // if true, then the function allows extra positional arguments (i.e. last argument is declared '*name')
    // and `params[n_param - 1]` describes the var-arg (and should have `defa==NULL`)
    bool isVarArg;

    // the number of parameters (positional) that it takes (not including defaults/vararg)
    int n_param;

    // list of parameters
    struct ks_kfunc_param {

        // the name of the parameter
        ks_str name;

        // the 'default' object for the parameter, or 'NULL' if it is required
        // (NOTE: if `isVarArg`, and this is `params[n_param - 1]`, then defa==NULL, but it is not required)
        ks_obj defa;

    }* params;


    // the index at which defaults start
    int defa_start_idx;

    // number of defaults
    int n_defa;

    // defaults to be added
    ks_ast* defas;



}* ks_kfunc;



// ks_stk_frame - represents a 'stack frame' of execution,
//   which is typically a function call or evaluation
typedef struct {
    KS_OBJ_BASE

    // the actual function being called, which is normally either:
    //  'cfunc'
    //  'kfunc'
    // Or, (in the case of expressions, for example)
    //  'code'
    // Enough to say, anything using this value should check the type
    ks_obj func;

    // the 'code' (bytecode) object that the stack frame is operating on
    // NOTE: May be NULL if the stack frame does not have a bytecode object
    //   (i.e. is a C-style function that doesn't use local variables)
    ks_code code;

    // dictionary of local variables, iff type(func)==kfunc,
    // otherwise, NULL
    ks_dict locals;

    // Current program counter (only valid if `code != NULL`),
    //   in which case it is the current position (starting from code->bc+0)
    ksb* pc;

}* ks_stack_frame;


// create a new stack frame
// NOTE: This returns a new reference
ks_stack_frame ks_stack_frame_new(ks_obj func);




/* I/O 
 *
 * All the IO functionality in kscript is tailored such that everything can be done
 *   agnostically to any OS
 * 
 * 
 */
typedef struct {
    KS_OBJ_BASE

    // the mode in which the iostream was opened in, typically 'r', 'w', etc
    ks_str mode;

    // whether or not the file is currently open;
    // if not, then members such as `_FILEp` may be NULL
    bool isOpen;

    // whether or not the ios is extern (if extern, we don't free any resources or anything on close)
    bool isExtern;

    // the name of the file, which is either the path that was used to open it,
    //   or something enclosed in brackets like `<internal buffer>`
    // Never NULL
    ks_str name;




    // TODO: support different OS-specific stuff here

    // the underlying file pointer
    FILE* _fp;

}* ks_ios;


// Create a blank 'iostream', with no target
// NOTE: Use `ks_ios_open()` to actually get a target
// NOTE: Returns a new reference
KS_API ks_ios ks_ios_new();

// Open 'self' to the given file and mode
// NOTE: Returns success, or false and throws an error
KS_API bool ks_ios_open(ks_ios self, ks_str fname, ks_str mode);

// Open 'self' as an extern 
// NOTE: `fname` is not used for opening any files! It is only used as a decoration
//   if it is NULL, then one is generated
// NOTE: `mode` should simply describe the mode _fp was opened in
// NOTE: Returns success, or false and throws an error
KS_API bool ks_ios_extern_FILE(ks_ios self, ks_str fname, ks_str mode, FILE* _fp);


// Close 'self' (or, if already closed, do nothing)
// NOTE: will never throw an error; all errors are ignored
KS_API void ks_ios_close(ks_ios self);

// Return the current position (in bytes) in 'self', or -1 and throw an error
KS_API ks_ssize_t ks_ios_tell(ks_ios self);

// Return the size (in bytes) of the entire stream, or -1 and throw an error
KS_API ks_ssize_t ks_ios_size(ks_ios self);



enum {

    // Set position in absolute bytes
    KS_IOS_SEEK_SET    = 1,

    // Set position relative to current position
    KS_IOS_SEEK_CUR    = 2,

    // Set position relative to the end of the stream
    KS_IOS_SEEK_END    = 3,

};


// Attempt to seek `self` to given position, in mode `seekmode` (see KS_IOS_SEEK_* enum definitions)
// NOTE: everything is in bytes here
KS_API bool ks_ios_seek(ks_ios self, ks_ssize_t pos_b, int seekmode);

// Read a given number of bytes from the iostream and put them into `dest`
// NOTE: Returns number of bytes actually read (accounting for truncation), or -1 and throw an error
KS_API ks_ssize_t ks_ios_readb(ks_ios self, ks_ssize_t len_b, void* dest);


// Read a given number of characters from the iostream and put them into `dest` (encoded as utf8)
// NOTE: Returns the number of bytes read into 'dest'; note: `dest` may be reallocated via `ks_realloc()`
// NOTE: if `len_c_actual` is not NULL, it is set to the number of characters read
// Example:
// void* dest = NULL;
// ks_ios_reads(self, 4, &dest);
// ks_free(dest);
KS_API ks_ssize_t ks_ios_reads(ks_ios self, ks_ssize_t len_c, void** dest, ks_ssize_t* len_c_actual);





// main thread
extern ks_thread ks_thread_main;


// list of paths (i.e. directories) to search when importing modules, files, etc
// See `init.c` for how this is constructed, but essentially:
//   * Common places (`/usr/local/kscript` on UNIX-like OSes, sometimes `~/.local/kscript`)
//   * Any paths specified in the environment variable `KS_PATH`; these are `:` delimited, so
//       `KS_PATH=/usr/local/kscript:~/mykscriptdir ./my_file.ks` will add the given paths to search
extern ks_list ks_paths;


/* Type Objects */

extern ks_type
    ks_T_obj,
    ks_T_none,
    ks_T_type,
    ks_T_str,
    ks_T_bytes,
    ks_T_str_builder,

    ks_T_bool,
    ks_T_int,
    ks_T_float,
    ks_T_complex,
    ks_T_Enum,

    ks_T_list,
    ks_T_tuple,
    ks_T_slice,
    ks_T_range,
    ks_T_dict,
    ks_T_namespace,

    ks_T_parser,
    ks_T_ast,
    ks_T_code,
    ks_T_kfunc,

    ks_T_cfunc,
    ks_T_pfunc,

    ks_T_logger,
    ks_T_ios,

    ks_T_Enum,

    ks_T_Error,
    ks_T_InternalError,
    ks_T_ImportError,
    ks_T_SyntaxError,
    ks_T_IOError,
    ks_T_TodoError,
        
    ks_T_KeyError,
    ks_T_AttrError,
    
    ks_T_TypeError,
    ks_T_ArgError,
    ks_T_SizeError,
    
    ks_T_OpError,
    ks_T_OutOfIterError,
    
    ks_T_MathError,
    ks_T_AssertError,

    ks_T_module,

    ks_T_thread,
    ks_T_stack_frame

;


// return a new reference to an object; use macro KS_NEWREF
static inline ks_obj ks_newref(ks_obj obj) {
    KS_INCREF(obj);
    return obj;
}


/* --- Functions --- */

// General library functions

// Initializes, kscript, returns whether it was successful or not
KS_API bool ks_init(int verbose);

// Finalize the library
KS_API void ks_finalize();

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


// General utility functions


// Read an entire file, and return an allocated string buffer
// NOTE: Returns the string, or NULL and throws an error
KS_API ks_str ks_readfile(const char* fname, const char* mode);

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


// Extented version of `ks_obj_call` that takes a non-required (i.e. NULL-able) locals dictionary
KS_API ks_obj ks_obj_call2(ks_obj func, int n_args, ks_obj* args, ks_dict locals);

// Call func() with the given arguments
// NOTE: Returns the result of the function call, or NULL if an error was thrown
KS_API ks_obj ks_obj_call(ks_obj func, int n_args, ks_obj* args);


// Return whether or not 'obj' is a 'truthy' value, which is primarily defined by:
//  * the value of 'obj', if 'obj' is a boolean
//  * if 'obj' is non-zero if 'obj' is a numeric type
//  * if 'len(obj)' is non-zero if it is an iterable
//  * if 'type(obj).__bool__(self)' is defined, it is called, and its result is used
// The result is -1 if there was a failure/exception in attempting to convert 'obj' to a boolean,
// 0 if it was falsy, and 1 if it was truthy
KS_API int ks_obj_truthy(ks_obj obj);

// Return whether or not 'func' is callable as a function
KS_API bool ks_obj_is_callable(ks_obj func);

// Return whether or not 'obj' is iterable, through the `iter()` and `next()` protocol
KS_API bool ks_obj_is_iterable(ks_obj obj);


// Throw an object, return NULL 
KS_API ks_obj ks_obj_throw(ks_obj obj);

// Catch an exception (or return NULL if there was none),
// and set 'frames' to the list of stack frames
// NOTE: `frames` should point to NULL before the catch!
KS_API ks_obj ks_catch(ks_list* frames);

// Catch and ignore any object thrown
KS_API void ks_catch_ignore();

// Throw an object, return NULL (use ks_throw macro)
KS_API void* ks_ithrow(const char* file, const char* func, int line, ks_type errtype, const char* fmt, ...);

// throw an error type, i.e.
// ks_throw(ks_T_Error, "My format: %i", 34);
#define ks_throw(_errtype, ...) ks_ithrow(__FILE__, __func__, __LINE__, _errtype, __VA_ARGS__)

// throw a specialized error message for binary operators
#define KS_THROW_BOP_ERR(_bopstr, _L, _R) { ks_throw(ks_T_OpError, "Binary operator '%s' is undefined for '%T' and '%T'", _bopstr, _L, _R); return NULL; }

// throw a specialized error message for unary operators
#define KS_THROW_UOP_ERR(_uopstr, _V) { ks_throw(ks_T_OpError, "Unary operator '%s' is undefined for '%T'", _uopstr, _V); return NULL; }

// throw a specialized error message for non-iterables
#define KS_THROW_ITER_ERR(_V) { ks_throw(ks_T_Error, "'%T' object was not iterable!", _V); return NULL; }

// throw a specialized error message for missing method
#define KS_THROW_METH_ERR(_V, _meth) { ks_throw(ks_T_Error, "'%T' object had no '%s' method!", _V, _meth); return NULL; }

// throw a type conversion error
#define KS_THROW_TYPE_ERR(_obj, _totype) { ks_throw(ks_T_TypeError, "'%T' object could not be converted to '%S'", _obj, _totype); return NULL; }

// throw an item key error
#define KS_THROW_KEY_ERR(_obj, _key) { ks_throw(ks_T_KeyError, "'%T' object did not contain the key '%S'", _obj, _key); return NULL; }

// throw an item index error
#define KS_THROW_INDEX_ERR(_obj, _key) { ks_throw(ks_T_KeyError, "'%T' object given invalid index: '%S'", _obj, _key); return NULL; }


// throw an item attribute error
#define KS_THROW_ATTR_ERR(_obj, _key) { ks_throw(ks_T_AttrError, "'%T' object did not contain the attribute '%S'", _obj, _key); return NULL; }


// struct ks_citer - C iterable structure, used for iterating over iterables in C
// See source in `citer.c`
// 
// Usage:
// struct ks_citer cit = ks_citer_make(my_obj);
// ks_obj ob = NULL;
// while (ob = ks_citer_next(&cit)) {
//   { // do stuff with ob ... }
//   // destroy reference from iterator
//   KS_DECREF(ob);
// }
// // handle error thrown 
// if (cit.threw) return NULL;
// // clean it up
// ks_citer_done(&cit);
// 
struct ks_citer {
    // the original object created with
    ks_obj obj;

    // the iterable object over which we will be iterating
    // NOTE: may be == obj, if `obj.__next__` exists
    ks_obj iter_obj;

    // variable telling whether the iterator is done processing values
    bool done;

    // whether or not the C-iterator has thrown an error that you need to return from
    bool threwErr;

};


// Create a new C iterator for object
// NOTE: Make sure to use `ks_citer_done(&result)` after you are done with the object
KS_API struct ks_citer ks_citer_make(ks_obj obj);

// Do the next C-iterator
// NOTE: NULL just indicates end-of-iterator, not neccessarily an error
KS_API ks_obj ks_citer_next(struct ks_citer* cit);

// Declare we are done with the C iterator; free all resources and turn `cit` INVALID for further use
KS_API void ks_citer_done(struct ks_citer* cit);





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


// Get an attribute from a type, going up to its parents if neccessary
KS_API ks_obj ks_type_get(ks_type self, ks_str key);

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


enum {

    // automatic base, which will look for beginnings such as `0x`, `0b`, `0r`, etc
    //   but default to base 10 if none are found
    KS_BASE_AUTO   = 0,

    // binary, i.e. base 2
    KS_BASE_2      = 2,

    // octal, i.e. base 8
    KS_BASE_8      = 8,

    // hex, i.e. base 16
    KS_BASE_16     = 16,


    // decimal, i.e. base 10
    KS_BASE_10     = 10,

    // roman base, i.e. parsing roman numerals
    KS_BASE_ROMAN  = -1,

};

// Create a kscript int from a string in a given base (check KS_BASE_* enums)
// If `base==KS_BASE_AUTO==0`, then if a prefix is given, that base is used (i.e. 0x, 0r, 0o, 0b). Otherwise, base 10 is assumed
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_int ks_int_new_s(char* str, int base);

// Create a new integer from an MPZ
// NOTE: Returns a new reference
KS_API ks_int ks_int_new_mpz(mpz_t val);

// Create a new integer from an MPZ, that can use the `val` and clear if it
//   is not required
// NOTE: Don't call `mpz_clear(val)`; kscript now owns that
// NOTE: Returns a new reference
KS_API ks_int ks_int_new_mpz_n(mpz_t val);

// Return the sign of self, i.e. { -1, 0, 1 }
KS_API int ks_int_sgn(ks_int self);

// Compare 2 integers, returning a comparator
KS_API int ks_int_cmp(ks_int L, ks_int R);

// Compare to a C-style integer
KS_API int ks_int_cmp_c(ks_int L, int64_t R);

// Compute and return the hash of an integer
KS_API ks_hash_t ks_int_hash(ks_int self);


// Construct a new 'float' object
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_float ks_float_new(double val);


// Construct a new 'complex' object
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_complex ks_complex_new(double complex val);



/* Raw C API for text: (source code is still in `types/str.c`) */


// Calculate the length (in unicode characters, defined via the number of sequences decoded) of a string
//   of (valid) UTF8. A negative return value indicates a unicode error was thrown
// NOTE: If `len_b<0`, then `src` is assumed to be NUL-terminated
KS_API ks_ssize_t ks_text_utf8_len_c(const char* src, ks_size_t len_b);

// Transcode 'len_c' characters of utf8 text (located in 'src'), into utf32 (located in 'dest')
// NOTE: it is assumed that there is enough space in both arrays; `dest` should have been allocated for
//   at least `sizeof(ks_unich) * len_c`, and those are exactly the bytes that will be filled (no NULL-terminator) is given
// NOTE: Returns the number of bytes decoded in utf8, or a negative number to indicate an error
KS_API ks_ssize_t ks_text_utf8_to_utf32(const char* src, ks_unich* dest, ks_ssize_t len_c);

// Transcode 'len_c' characters of utf32 text (located in 'src'), into utf8 (located in 'dest')
// NOTE: it is assumed that there is enough space in both arrays; `dest` should have been allocated for
//   at least `sizeof(ks_unich) * len_c`. 
// NOTE: Returns the number of bytes decoded in utf8 (so, always >= len_c), or a negative number to indicate an error
// A NUL-terminator is NOT added
KS_API ks_ssize_t ks_text_utf32_to_utf8(const ks_unich* src, char* dest, ks_ssize_t len_c);



// Construct a new 'str' object, from a utf-8 string
// NOTE: Native C ascii strings are allowed, in which case `len_b` is the number of characters, but
//   for any non-ascii strings, the number of characters is less than the length in bytes
// NOTE: if len_b < 0, then it is NUL-terminated, and the length is calculated via `strlen()`
KS_API ks_str ks_str_utf8(const char* cstr, ks_ssize_t len_b);

// Construct a new 'str' object, from a C-style string.
// NOTE: If `len<0`, then `len` is calculated by the C `strlen()` function
// NOTE: If `cstr==NULL`, then the empty string "" is returned, and `len` is not checked
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_str ks_str_new_c(const char* cstr, ks_ssize_t len);

// Create new string from Cstring
#define ks_str_new(_cstr) ks_str_new_c(_cstr, -1)


// Convert a length one string to an ordinal, and return it
// NOTE: Returns the value, or a negative number indiciating an error was thrown
KS_API ks_unich ks_str_ord(ks_str str);

// Returns a string of length 1 from a given unicode character (NOT encoded in UTF8; decoded as a single value)
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_str ks_str_chr(ks_unich chr);


// Create a substring of another string, on given character indices
// NOTE: indices must be adjusted! a negative `len_c` returns the rest of the string starting at 'start'
KS_API ks_str ks_str_substr(ks_str self, ks_ssize_t start, ks_ssize_t len_c);


// Compare two strings, and return their string comparison (i.e. strcmp() in C)
KS_API int ks_str_cmp(ks_str A, ks_str B);

// Return whether two strings are equal (this may be faster than `ks_str_cmp`)
KS_API bool ks_str_eq(ks_str A, ks_str B);

// Return whether a kscript string equals a C-style string (of length (in bytes) len, or strlen(cstr) if len<0)
// NOTE: Only byte-wise equality is checked
KS_API bool ks_str_eq_c(ks_str A, const char* cstr, ks_ssize_t len);

// Escape the string 'A', i.e. replace '\' -> '\\', and newlines to '\n', replace unicode characters with their representations
// NOTE: Returns a new reference
KS_API ks_str ks_str_escape(ks_str A);

// Undo the string escaping, i.e. replaces '\n' with a newline
// NOTE: Returns a new reference
KS_API ks_str ks_str_unescape(ks_str A);



// Create a new `bytes` object from the given pointer & size
KS_API ks_bytes ks_bytes_new(const uint8_t* byt, ks_size_t len_b);




// Create a new string builder
// NOTE: Returns new reference
KS_API ks_str_builder ks_str_builder_new();

// Add raw bytes to the string builder
// NOTE: Returns success
KS_API bool ks_str_builder_add(ks_str_builder self, void* data, ks_size_t len);

// Add str(obj) to the string builder
// NOTE: Returns success
KS_API bool ks_str_builder_add_str(ks_str_builder self, ks_obj obj);
KS_API bool ks_str_builder_add_repr(ks_str_builder self, ks_obj obj);

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


// Construct a list from an iterator
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_list ks_list_new_iter(ks_obj iter_obj);

// Clear a list out, removing any references
KS_API void ks_list_clear(ks_list self);


// Empty out `objs` (which must be iterable!), and add everything to `self`
// NOTE: Returns success, or false and throws an error
KS_API bool ks_list_pushall(ks_list self, ks_obj objs);

// Pushes 'obj' on to the end of the list
KS_API void ks_list_push(ks_list self, ks_obj obj);

// Push 'objs' on to the end of the list
KS_API void ks_list_pushn(ks_list self, int n, ks_obj* objs);

// Pops the last element off of list and returns its reference
// NOTE: Throws 'SizeError' if the list was empty
KS_API ks_obj ks_list_pop(ks_list self);

// Pop off 'n' items into 'dest'
// NOTE: Returns a reference to each object, so now each object in 'dest' now has a reference
//   you must handle!
// NOTE: Returns success, or false and throws an error
KS_API bool ks_list_popn(ks_list self, int n, ks_obj* dest);

// Pops the last element off and throw away the reference
// NOTE: Returns success, or false and throws an error
KS_API bool ks_list_popu(ks_list self);

// Pops the last 'n' elements off and throw away the references
// NOTE: Returns success, or false and throws an error
KS_API bool ks_list_popun(ks_list self, int n);


// Create a new kscript tuple from an array of elements, or an empty tuple if `len==0`
// NOTE: Returns a new reference
KS_API ks_tuple ks_tuple_new(int len, ks_obj* elems);

// Create a new kscript tuple from an array of elements, or an empty tuple if `len==0`
// NOTE: This variant does not create references to the objects!, so don't call DECREF on 'elems'
// NOTE: Returns a new reference
KS_API ks_tuple ks_tuple_new_n(int len, ks_obj* elems);


// Build a tuple from C-style variables
// For example, `ks_build_tuple("%i %f", 2, 3.0)` returns (2, 3.0)
// NOTE: Returns a new reference, or NULL if there was an error thrown
KS_API ks_tuple ks_build_tuple(const char* fmt, ...);


// Create a new kscript namespace from a dictionary
// NOTE: Returns a new reference
KS_API ks_namespace ks_namespace_new(ks_dict attr);

// Construct a new dictionary from key, val pairs (elems[0], elems[1] is the first, elems[2*i+0], elems[2*i+1] makes up the i'th pair)
// NOTE: `len%2==0` is a requirement!
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_dict ks_dict_new(ks_size_t len, ks_obj* elems);

// Construct a new dictionary from C-style initializers
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_dict ks_dict_new_c(ks_keyval_c* keyvals);

// Return the load factor for a given dictionary, between 0.0 and 1.0
KS_API double ks_dict_load(ks_dict self);

// Merge in entries of 'src' into self, replacing any entries in 'self' that existed
KS_API void ks_dict_merge(ks_dict self, ks_dict src);

// Merge in all enumeration members to 'self'
KS_API void ks_dict_merge_enum(ks_dict self, ks_type enumtype);

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


// Construct a new member function with the given function & member instance
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_pfunc ks_pfunc_new(ks_obj func, ks_obj member_inst);




// Create a new enumeration type, with given values
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_type ks_Enum_create_c(const char* name, ks_enumval_c* enumvals);


// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_Enum ks_Enum_new(ks_type enumtype, ks_str val);

// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_Enum ks_Enum_new_c(ks_type enumtype, char* cstr);


// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_Enum ks_Enum_new_i(ks_type enumtype, ks_int val);

// Construct a new enumeration value
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_Enum ks_Enum_new_ic(ks_type enumtype, int64_t val);


// Construct a new error of a given type, sets `.what` to the given string
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_Error ks_Error_new(ks_type errtype, ks_str what);

// construct a new kscript thread
// if 'name==NULL', then a name is generated
KS_API ks_thread ks_thread_new(const char* name, ks_obj target, int n_args, ks_obj* args);

// Return the current thread
// NOTE: NO reference is returned; do not call KS_DECREF() on this!
KS_API ks_thread ks_thread_get();


// Find out the current filename and line that a thread is executing (in kscript), or return `false`
//   if nothing was found
// NOTE: This will never throw an error; it will only return whether it actually found something
// NOTE: you are NOT given a reference to `fname`! Don't `KS_DECREF` it! Make a copy or add a reference
//   if you need to keep it
KS_API bool ks_thread_getloc(ks_thread self, ks_str* fname, int* cur_line);


// controlling how the parser parses
enum ks_parse_flags {
    // default, no special flags
    KS_PARSE_NONE                 = 0x00,

    // if set, accept '{}' blocks, and not dictionary literals
    KS_PARSE_BLOCK                = 0x01,

    // if true, ignore extra ']' at the end of line and return early
    KS_PARSE_INBRK                = 0x02,
    // if true, ignore extra ')' at the end of line and return early
    KS_PARSE_INPAR                = 0x04,

};

// Create a new parser from some source code
// Or, return NULL if there was an error (and 'throw' the exception)
// NOTE: Returns a new reference
KS_API ks_parser ks_parser_new(ks_str src_code, ks_str src_name, ks_str file_name);

// Parse out an expression from the parser
// NOTE: Returns a new reference, or NULL and throw an error
KS_API ks_ast ks_parser_expr(ks_parser self, enum ks_parse_flags flags);

// Parse a single statement out of 'p'
// NOTE: Returns a new reference
KS_API ks_ast ks_parser_stmt(ks_parser self, enum ks_parse_flags flags);

// Parse the entire file out of 'p', returning the AST of the program
// Or, return NULL if there was an error (and 'throw' the exception)
// NOTE: Returns a new reference
KS_API ks_ast ks_parser_file(ks_parser self);


// Create string for exceptions, detailing where 'tok' is in the parser source code
KS_API ks_str ks_tok_expstr(ks_parser parser, struct ks_tok tok);


/*
// convert token to a string with mark
KS_API ks_str ks_tok_expstr(ks_parser parser, struct ks_tok tok);

// convert token to string, just the 2 relevant lines
KS_API ks_str ks_tok_expstr(ks_parser parser, struct ks_tok tok);
*/

/* AST (Abstract Syntax Trees) */

// Create an AST representing a constant value
// Type should be none, bool, int, or str
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_const(ks_obj val);

// Create an AST representing a variable reference
// Type should always be string
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_var(ks_str name);
// Create an AST representing a tuple constructor
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_tuple(int n_items, ks_ast* items);

// Create an AST representing a list constructor
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_list(int n_items, ks_ast* items);

// Create an AST representing a list constructor
// NOTE: Returns a new referenceks_num
KS_API ks_ast ks_ast_new_dict(int n_items, ks_ast* items);

// Create an AST representing a slice constructor
// NOTE: Returns a new reference
KS_API ks_ast ks_ast_new_slice(ks_ast start, ks_ast stop, ks_ast step);


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



/* CODE */

// Create a new kscript code object, with a given constant list. The constant list can be non-empty,
//   in which case new constants will be allocated starting at the end. Cannot be NULL
// It can also be created with the 'parser' object (or it can be NULL)
// NOTE: Returns a new reference
KS_API ks_code ks_code_new(ks_list v_const, ks_parser parser);

// Compile an AST into a bytecode object
KS_API ks_code ks_compile(ks_parser parser, ks_ast self);

// Append an array of bytecode to 'self'
KS_API void ks_code_add(ks_code self, int len, ksb* data);

// Add a constant to the internal constant list, return the index at which it was added (or already located)
KS_API int ks_code_add_const(ks_code self, ks_obj val);

// add a meta token (and hold a reference to the parser)
KS_API void ks_code_add_meta(ks_code self, struct ks_tok tok);

/*** ADDING BYTECODES (see ks.h for bytecode definitions) ***/
KS_API void ksca_noop      (ks_code self);

KS_API void ksca_push      (ks_code self, ks_obj val);
KS_API void ksca_dup       (ks_code self);
KS_API void ksca_popu      (ks_code self);

KS_API void ksca_list      (ks_code self, int n_items);
KS_API void ksca_list_add_objs (ks_code self, int n_items);
KS_API void ksca_list_add_iter (ks_code self);

KS_API void ksca_tuple     (ks_code self, int n_items);
KS_API void ksca_dict      (ks_code self, int n_items);
KS_API void ksca_slice     (ks_code self);

KS_API void ksca_buildstr  (ks_code self, int n_items);

KS_API void ksca_getitem   (ks_code self, int n_items);
KS_API void ksca_setitem   (ks_code self, int n_items);

KS_API void ksca_call      (ks_code self, int n_items);
KS_API void ksca_vcall     (ks_code self);
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



// Construct a new kfunc from bytecode and a human readable name
KS_API ks_kfunc ks_kfunc_new(ks_code code, ks_str name_hr);

// create a new copy of the kfunc
KS_API ks_kfunc ks_kfunc_new_copy(ks_kfunc func);

// add a parameter name (defa==NULL allows no default)
KS_API void ks_kfunc_addpar(ks_kfunc self, ks_str name, ks_obj defa);



// Construct a slice with paramaters.
// NOTE: KSO_NONE should be used for defaults
// NOTE: Returns a new reference
KS_API ks_slice ks_slice_new(ks_obj start, ks_obj stop, ks_obj step);

// Calculate C-iteration values
// To be used like so:
// if (!ks_slice_getci(slice, array_len, &first, &last, &delta)
// for (i = first; i != last; i += delta) { ... do operation ... }
// To calculate the number of indexes, you can use `(last - first) / delta`. `delta` is guaranteed to be non-zero
// NOTE: Returns success, and if false, an error is thrown
KS_API bool ks_slice_getci(ks_slice self, int64_t len, int64_t* first, int64_t* last, int64_t* delta);



// Construct a range with paramaters.
// NOTE: KSO_NONE should be used for defaults
// NOTE: Returns a new reference
KS_API ks_range ks_range_new(ks_int start, ks_int stop, ks_int step);


/* GENERIC NUMERICS (i.e. work with all numeric types) */


// Parse `len` bytes of str as a numeric type in base `base`
// If `base==KS_BASE_AUTO`, then autodetect the base, or use 10 as a default
// NOTE: Returns new reference, or NULL if there was an error (and throw it)
KS_API ks_obj ks_num_parse(const char* str, int len, int base);

// Attempt to hash 'self'
// Supported types:
//   + bool
//   + int
//   + int (long)
//   + float
//   + complex
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_hash(ks_obj self, ks_hash_t* out);

// Return whether a given object would fit an int64_t
// Supported types:
//   + bool
//   + int
//   - int (long)
//   - float
//   - complex
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_fits_int64(ks_obj self);

// Return whether a given object is a numeric type
// Supported types:
//   + bool
//   + int
//   + int (long)
//   + float
//   + complex
KS_API bool ks_num_is_numeric(ks_obj self);

// Return whether a given object is an integral type
// Supported types:
//   + bool
//   + int
//   + int (long)
//   - float
//   - complex
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_is_integral(ks_obj self);


// Attempt to convert 'self' to a boolean, and set 'out' to the value
// Supported types:
//   + bool
//   + int
//   + int (long)
//   + float
//   + complex
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_get_bool(ks_obj self, bool* out);

// Attempt to convert 'self' to a int64_t, and set 'out' to the value
// Supported types:
//   + bool
//   + int
//   - int (long)
//   - float
//   - complex
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_get_int64(ks_obj self, int64_t* out);


// Attempt to convert 'self' to an 'int'
// NOTE: If there was a problem, return false and throw an error
KS_API ks_int ks_num_get_int(ks_obj self);



// Attempt to convert 'self' to a mpz_t (which is already initialized)
// Supported types:
//   + bool
//   + int
//   + int (long)
//   - float
//   - complex
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_get_mpz(ks_obj self, mpz_t out);


// Attempt to convert 'self' to a double, and set 'out' to the value
// Supported types:
//   + bool
//   + int
//   + int (long)
//   + float
//   - complex
//   + complex (real only)
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_get_double(ks_obj self, double* out);

// Attempt to convert 'self' to a double complex, and set 'out' to the value
// Supported types:
//   + bool
//   + int
//   + int (long)
//   + float
//   + complex
//   + complex (real only)
// NOTE: If there was a problem, return false and throw an error
KS_API bool ks_num_get_double_complex(ks_obj self, double complex* out);


// Get the sign of a numeric object
KS_API bool ks_num_sgn(ks_obj self, int* out);

// compare 2 numeric objects
KS_API bool ks_num_cmp(ks_obj L, ks_obj R, int* out);

// compare 2 numeric objects
KS_API bool ks_num_eq(ks_obj L, ks_obj R, bool* out);


/* OPS */

// Compute L+R
KS_API ks_obj ks_num_add(ks_obj L, ks_obj R);
// Compute L-R
KS_API ks_obj ks_num_sub(ks_obj L, ks_obj R);
// Compute L*R
KS_API ks_obj ks_num_mul(ks_obj L, ks_obj R);
// Compute L/R
KS_API ks_obj ks_num_div(ks_obj L, ks_obj R);
// Compute L%R
KS_API ks_obj ks_num_mod(ks_obj L, ks_obj R);
// Compute L**R
KS_API ks_obj ks_num_pow(ks_obj L, ks_obj R);

// Compute -L
KS_API ks_obj ks_num_neg(ks_obj L);
// Compute ~V
KS_API ks_obj ks_num_sqig(ks_obj V);

// Compute abs(L)
KS_API ks_obj ks_num_abs(ks_obj L);

// Compute L<R
KS_API ks_obj ks_num_lt(ks_obj L, ks_obj R);
// Compute L<=R
KS_API ks_obj ks_num_le(ks_obj L, ks_obj R);
// Compute L>R
KS_API ks_obj ks_num_gt(ks_obj L, ks_obj R);
// Compute L>=R
KS_API ks_obj ks_num_ge(ks_obj L, ks_obj R);

// Compute L<<R
KS_API ks_obj ks_num_lshift(ks_obj L, ks_obj R);
// Compute L>>R
KS_API ks_obj ks_num_rshift(ks_obj L, ks_obj R);


// Compute L|R
KS_API ks_obj ks_num_binor(ks_obj L, ks_obj R);
// Compute L&R
KS_API ks_obj ks_num_binand(ks_obj L, ks_obj R);
// Compute L^R
KS_API ks_obj ks_num_binxor(ks_obj L, ks_obj R);




/* Execution */

/* ks__exec -> perform execution on a thread
 *
 * This function makes a lot of assumptions, such as:
 *   * You are on a thread currently
 *   * The current thread has already had teh stack frame loaded (i.e.
 *       this method does not create a stack frame)
 *
 * If any of these are not met, it will just abort (no exceptions generated),
 *   because this IS this code that generates exceptions, it's okay to safeguard it like this
 * 
 */
KS_API ks_obj ks__exec(ks_thread self, ks_code code);




/* --- Global/Builtin Functions --- */

// functions, found in `funcs.c`
extern ks_cfunc
    ks_F_print,

    ks_F_getattr,
    ks_F_setattr,
    ks_F_getitem,
    ks_F_setitem,

    ks_F_iter,
    ks_F_next,

    ks_F_truthy,
    ks_F_recurse,

    ks_F_repr,
    ks_F_hash,
    ks_F_id,
    ks_F_len,
    ks_F_typeof,

    ks_F_import,

    ks_F_issub,
    
    ks_F_any,
    ks_F_all,

    ks_F_sort,
    ks_F_map,
    ks_F_sum,
    ks_F_filter,

    ks_F_chr,
    ks_F_ord,

    ks_F_add,
    ks_F_sub,
    ks_F_mul,
    ks_F_div,
    ks_F_mod,
    ks_F_pow,

    ks_F_binand,
    ks_F_binor,
    ks_F_binxor,

    ks_F_lshift,
    ks_F_rshift,

    ks_F_cmp,

    ks_F_lt,
    ks_F_gt,
    ks_F_le,
    ks_F_ge,
    ks_F_eq,
    ks_F_ne,

    ks_F_pos,
    ks_F_neg,
    ks_F_abs,
    ks_F_sqig,

    ks_F_eval,
    ks_F_exec_file,
    ks_F_exec_expr,
    ks_F_exec_interactive

;

// dictionary of globals
extern ks_dict ks_globals;

// interpreter variables
extern ks_dict ks_inter_vars;

// TODO: add GIL


// Lock the GIL for operation
KS_API void ks_GIL_lock();

// Unock the GIL for operation
KS_API void ks_GIL_unlock();



#ifdef __cplusplus
}
#endif

#endif /* KS_H__ */

