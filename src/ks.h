/* ks.h - main header for the kscript library
 *
 * 
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#ifndef KS_H__
#define KS_H__

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <string.h>
#include <assert.h>

// timing functions
#include <time.h>
#include <sys/time.h>

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



/* TYPES */

// ks_size_t - type representing the size of something, i.e. unsigned and long enough to hold
//   64 bit indices
typedef uint64_t ks_size_t;

// ks_hash_t - type representing a hash of an object. 
// NOTE: hashes should never be '0', that means the hash is uninitialized or invalid
typedef uint64_t ks_hash_t;


// ks_obj - the most generic kscript object, which any other objects are castable to
typedef struct ks_obj* ks_obj;

// ks_str - a string object in kscript
typedef struct ks_str* ks_str;

// ks_type - an object representing a type in kscript. Every object has a type, which you can check with `obj->type`
typedef struct ks_type* ks_type;

// ks_list - an object representing an ordered list (i.e. array) of objects in kscript. note that this only holds references
//   to the objects; objects are not duplicated for the list
typedef struct ks_list* ks_list;

// ks_tuple - an object representing a tuple or collection of objects, which is immutable
typedef struct ks_tuple* ks_tuple;

// ks_dict - an object representing a generic object mapping, where an object of (most) types can be a key, and any type can be
//   a value
// Internally, a hash table implementation is used, similar to Python's
typedef struct ks_dict* ks_dict;



// Put this macro at the beginning of the definition of any kscript object, i.e.:
// struct my_obj {
//   KS_OBJ_BASE
//   int num;
//   ...
// }
// This will make it a valid kscript object type
#define KS_OBJ_BASE int64_t refcnt; ks_type type;

// Record a reference to a given object (i.e. increment the reference count)
#define KS_INCREF(_obj) { ++((ks_obj)_obj)->refcnt; }

// Un-record a reference to a given object (i.e. decrement the reference count)
// NOTE: If the reference count reaches 0 (i.e. the object has became unreachable), this frees
//   the object
#define KS_DECREF(_obj) { if (--((ks_obj)_obj)->refcnt <= 0) { ks_obj_free(((ks_obj)_obj)); } }


// Allocate memory for a new object type (by default, use `ks_malloc`)
// For example: `KS_NEW_OBJ(ks_int)` will allocate a `ks_int`
#define KS_ALLOC_OBJ(_typeName) ((_typeName)ks_malloc(sizeof(*(_typeName){NULL})))

// Free an object's memory (non-recursively; just the actual object pointer)
// Use this macro on things created with `KS_ALLOC_OBJ(_typeName)`
#define KS_FREE_OBJ(_obj) (ks_free((void*)(_obj)))

// Initialize an object of a given type, essentially setting its type as well as
// Setting its reference count to '1' (since it should be created with a reference)
// NOTE: This also increments the reference count of '_typeType'
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

// Uninitialize an object, i.e. unrecord a reference to the type it has
#define KS_UNINIT_OBJ(_obj) { \
    ks_obj obj = (ks_obj)(_obj); \
    KS_DECREF(obj->type); \
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
     *   lookup, and instead just check
     * 
     */

    // type.__name__ -> the name of the type, typically human readable
    ks_str __name__;

    // type.__parents__ -> a list of parent classes from which this type derives from
    ks_list __parents__;

    // type.__str__(self) -> convert an item to a string
    ks_obj __str__;

    // type.__repr__(self) -> convert an item to a string representation
    ks_obj __repr__;

    // type.__free__(self) -> free the memory/references used by the object
    ks_obj __free__;

};

struct ks_tuple {
    KS_OBJ_BASE

    // the number of items in the tuple
    ks_size_t len;

    // the address of the first item. The tuple is allocated with the items in the main buffer,
    // so this acts as the offset from the object pointer to the values
    // So, allocating a tuple is like: malloc(sizeof(struct ks_tuple) + len * sizeof(ks_obj))
    ks_obj items[0];

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

// ks_none - the 'none' type in kscript
typedef struct {
    KS_OBJ_BASE

}* ks_none;

// the global singleton none
extern ks_none KS_NONE;

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
extern ks_bool KS_TRUE, KS_FALSE;

// down-casted constants so returning from functions is simpler
#define KSO_TRUE ((ks_obj)KS_TRUE)
#define KSO_FALSE ((ks_obj)KS_FALSE)


// ks_int - type representing an integer value (signed, 64 bit) in kscript
typedef struct {
    KS_OBJ_BASE

    // the actual integer value
    int64_t val;

}* ks_int;

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


// ks_cfunc - a C-function wrapper which can be called
typedef struct {
    KS_OBJ_BASE

    // the actual C function which can be called
    ks_obj (*func)(int n_args, ks_obj* args);

}* ks_cfunc;

/* STRING BUILDING/UTILITY TYPES */


// ks_str_builder - a string building utility to make string concatenation simpler
//   and more efficient
typedef struct {

    // the current length of the string builder
    int len;

    // the current character data for the string builder
    char* data;

} ks_str_builder;



// Initialize the string builder
// NOTE: This must be called before `_get` or `_add*` methods
void ks_str_builder_init(ks_str_builder* self);

// Create a (new reference) of a string from the string builder at this point
ks_str ks_str_builder_get(ks_str_builder* self);

// Add character bytes to the string builder
void ks_str_builder_add(ks_str_builder* self, int len, char* data);

// Add repr(obj) to the string builder, returns true if it was fine, false if there was an error
bool ks_str_builder_add_repr(ks_str_builder* self, ks_obj obj);

// add str(obj) to the string buffer
bool ks_str_builder_add_str(ks_str_builder* self, ks_obj obj);

// Free the string builder, freeing all internal resources (but not the built strings)
void ks_str_builder_free(ks_str_builder* self);

/* meta-types */

// these are the built-in types
extern ks_type 
    ks_type_type,
    
    ks_type_none,
    ks_type_bool,
    ks_type_int,
    ks_type_str,
    ks_type_tuple,
    ks_type_list,
    ks_type_dict,

    ks_type_cfunc

;


/* GENERIC/GENERAL LIBRARY FUNCTIONS */

// Attempt to initialize the library. Return 'true' on success, 'false' otherwise
bool ks_init();

// Return the time, in seconds, since the library started. It uses a fairly good wall clock,
//   but is only meant for rough approximation. Using the std time module is best for most results
double ks_time();

/* LOGGING */

// return the current logging level, one of KS_LOG_* enum values
int ks_log_level();

// set the logging level to `new_level`
void ks_log_level_set(int new_level);

// generically log given a level, the current file, line, and a C-style format string, with a list of arguments
// NOTE: don't use this, use the macros like `ks_info`, and `ks_warn`, which make it easier to use the logging
//   system
void ks_log(int level, const char *file, int line, const char* fmt, ...);

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


/* MEMORY MANAGEMENT */

// Allocate at least 'sz' bytes, and return a pointer to that memory. `ks_malloc(0)` is guaranteed to return non-NULL
// NOTE: This memory must be free'd by `ks_free(ptr)`, and reallocated using `ks_realloc(ptr, new_sz)`
void* ks_malloc(ks_size_t sz);

// Attempt to reallocate 'ptr' (which was created using `ks_malloc`) to be at least 'new_sz' bytes. 
// NOTE: `ks_realloc(NULL, sz)` is the same as doing `ks_malloc(sz)`
void* ks_realloc(void* ptr, ks_size_t new_sz);

// Attempt to free 'ptr' (which must have been allocated using `ks_malloc` or `ks_realloc`)
// NOTE: `ks_free(NULL)` is a guaranteed NO-OP
void ks_free(void* ptr);

// Return the current amount of memory being used, or -1 if memory usage is not being tracked
int64_t ks_mem_cur();

// Return the maximum amount of memory that has been used at one time, or -1 if memory usage is not being tracked
int64_t ks_mem_max();


/* CREATING/DESTROYING PRIMITIVES */

// Initialize a type variable. Make sure 'self' has not been ref cnted, etc. Just an allocated blob of memory!
// NOTE: Returns a new reference
void ks_init_type(ks_type self, char* name);

// Create a new kscript int from a C-style integer value
// NOTE: Returns a new reference
ks_int ks_new_int(int64_t val);

// Create a new kscript string from a C-style NUL-terminated string
// NOTE: Returns a new reference
ks_str ks_new_str(char* val);

// Create a new kscript string from a C-style length encoded string
// NOTE: Returns a new reference
ks_str ks_new_str_l(int len, char* chr);

// Create a new kscript tuple from an array of elements, or an empty tuple if `len==0`
// NOTE: Returns a new reference
ks_tuple ks_new_tuple(int len, ks_obj* elems);

// Create a new kscript list from an array of elements, or an empty list if `len==0`
// NOTE: Returns a new reference
ks_list ks_new_list(int len, ks_obj* elems);

// Create a new kscript dictionary from an array of entries (which should be 'len' number of key, val pairs)
// Example:
// ks_new_dict(3, (ks_obj[]){ key1, val1, key2, val2, key3, val3 })
// NOTE: Returns a new reference
ks_dict ks_new_dict(int len, ks_obj* entries);



/* OPERATIONS ON PRIMITIVES */

// perform a string comparison on 2 strings
int ks_str_cmp(ks_str A, ks_str B);

// Push an object on to the end of the list, expanding the list
void ks_list_push(ks_list self, ks_obj obj);

// Pop off an object from the end of the list
// NOTE: Returns a reference
ks_obj ks_list_pop(ks_list self);

// Pop off an object from the end of the list, destroying the reference
void ks_list_popu(ks_list self);


// special data structure for easier to read initialization from C
typedef struct {

    // NUL-terminated key (NULL string means this is the last one)
    char* key;

    // the value for the entry
    ks_obj val;

} ks_dict_ent_c;

// Create a new kscript dictionary from an array of C-style strings to values
// For example:
// ks_new_dict_cn((ks_dict_ent_cn[]){ {"Cade", myval}, {"Brown", otherval, {NULL, NULL}} });
// Will create a dictionary, and not introduce any memory leaks
// If you want to create values and transfer their references, see `ks_new_dict_cn` (n=no new references)
ks_dict ks_new_dict_c(ks_dict_ent_c* ent_cns);

// Create a new kscript dictionary from an array of C-style strings to values, which will not create new references to values
// The last key is 'NULL'
// For example:
// ks_new_dict_cn((ks_dict_ent_cn[]){ {"Cade", ks_new_int(42)}, {"Brown", ks_new_str("asdfasdf"), {NULL, NULL}} });
// Will create a dictionary, and not introduce any memory leaks
// If you're using already created variables, use `ks_new_dict_c()`, or replace the keys with `KS_NEWREF(key)`
ks_dict ks_new_dict_cn(ks_dict_ent_c* ent_cns);

// Test whether the dictionary has a given key. `hash` is always `hash(key)`. If it is 0, then 
//   attempt to calculate `hash(key)`. If it is 0, there is no error, but the dictionary is said to
//   not have the key
// For efficiency reasons, this allows the caller to precompute the hash from some other source,
//   so the dictionary doesn't have to
bool ks_dict_has(ks_dict self, ks_hash_t hash, ks_obj key);

// Get a value of the dictionary
// NULL if it does not exist
// NOTE: Returns a new reference
ks_obj ks_dict_get(ks_dict self, ks_hash_t hash, ks_obj key);

// Set a dictionary entry for a key, to a value
// If the entry already exists, dereference the old value, and replace it with the new value
// result > 0 means that an item was replaced
// result == 0 means no item was replaced, and there were no problems
// result < 0 means there was some internal problem (most likely the key was not hashable)
int ks_dict_set(ks_dict self, ks_hash_t hash, ks_obj key, ks_obj val);

// Delete an entry to the dictionary, returning 'true' if it was successful, false if it wasn't
bool ks_dict_del(ks_dict self, ks_hash_t hash, ks_obj key);

// Sets a list of C-entries (without creating new references)
// result == 0 means no problems
// result < 0 means there was some internal problem (most likely the key was not hashable)
int ks_dict_set_cn(ks_dict self, ks_dict_ent_c* ent_cns);

// Create a new C-function wrapper
// NOTE: Returns a new reference
ks_cfunc ks_new_cfunc(ks_obj (*func)(int n_args, ks_obj* args));


/* MISC. TYPE/OBJECT FUNCTIONS */

// add a parent to the type, which the type will derive from
void ks_type_add_parent(ks_type self, ks_type parent);

// Get an attribute for the given type
// 0 can be passed to 'hash', and it will be calculated
// NOTE: Returns a new referece
ks_obj ks_type_get(ks_type self, ks_str key);


// Set an attribute for the given type
// 0 can be passed to 'hash', and it will be calculated
void ks_type_set(ks_type self, ks_str key, ks_obj val);

// Set a C-style string key as the attribute for a type
void ks_type_set_c(ks_type self, char* key, ks_obj val);

// Sets a list of C-entries (without creating new references)
// result == 0 means no problems
// result < 0 means there was some internal problem (most likely the key was not hashable)
// So, do NOT remove additional references from 'ent_cns'
int ks_type_set_cn(ks_type self, ks_dict_ent_c* ent_cns);



/* OBJECT INTERFACE (see ./obj.c) */

// Get the string representation of an object, or NULL if there was an error
// NOTE: Returns a new reference
ks_str ks_repr(ks_obj obj);

// Convert the given object to a string, or NULL if there was an error
// NOTE: Returns a new reference
ks_str ks_to_str(ks_obj obj);

// Free an object, by either calling its deconstructor or freeing the memory
void ks_obj_free(ks_obj obj);

// Return the length of the object (len(obj)) as an integer.
// Negative values indicate there was an exception
int64_t ks_len(ks_obj obj);

// Return the hash of the object (hash(obj)) as an integer.
// A return value of '0' indicates that there was some error with the hash function
//   (a hash function should never return 0)
ks_hash_t ks_hash(ks_obj obj);

// Return whether or not `A==B`. If the comparison is undefined, return 'false'
bool ks_eq(ks_obj A, ks_obj B);

// Return whether or not 'func' is callable as a function
bool ks_is_callable(ks_obj func);


// Get an attribute by name, i.e. 'obj.attr'
// NOTE: Returns a new reference
ks_obj ks_getattr(ks_obj obj, ks_obj attr);


// Attempt to call 'func' on 'args', returning NULL if there was an error
// NOTE: Returns a new reference
ks_obj ks_call(ks_obj func, int n_args, ks_obj* args);

// Attempt to call 'func.attr' on 'args', returning NULL if there was an error
// NOTE: Returns a new reference
ks_obj ks_call_attr(ks_obj func, ks_obj attr, int n_args, ks_obj* args);



/* STRING FORMATTING (see ./fmt.c) */

// Perform C-style formatting, with various arguments
// TODO: document format specifiers
// NOTE: Returns a reference
ks_str ks_fmt_c(const char* fmt, ...);

// Perform variadic C-style formating, with a list of arguments
// TODO: document format specifiers
// NOTE: Returns a reference
ks_str ks_fmt_vc(const char* fmt, va_list ap);



/* KSCRIPT FUNCTION ERROR HANDLING */

// Require an expression to be true, otherwise throw an error and return 'NULL'
// NOTE: Should be used inside of a KS_FUNC(), because this will return NULL!
#define KS_REQ(_expr, ...) {       \
    if (!(_expr)) {                \
        ks_error(__VA_ARGS__);     \
        return NULL;               \
    }                              \
}

// Require that the number of args is a specific amount
#define KS_REQ_N_ARGS(_nargs, _correct) KS_REQ((_nargs) == (_correct), "Incorrect number of arguments, expected %i, but got %i", (int)(_correct), (int)(_nargs))

// Require that the number of args is a specific amount
#define KS_REQ_N_ARGS_MIN(_nargs, _min) KS_REQ((_nargs) >= (_min), "Incorrect number of arguments, expected at least %i, but got %i", (int)(_min), (int)(_nargs))

// Require that the number of args is a specific amount
#define KS_REQ_N_ARGS_MAX(_nargs, _max) KS_REQ((_nargs) <= (_max), "Incorrect number of arguments, expected at most %i, but got %i", (int)(_max), (int)(_nargs))

// Require that the number of args is a specific amount
#define KS_REQ_N_ARGS_RANGE(_nargs, _min, _max) KS_REQ((_nargs) >= (_min) && (_nargs) <= (_max), "Incorrect number of arguments, expected between %i and %i, but got %i", (int)(_min), (int)(_max), (int)(_nargs))


// Require that the object is of a given type. 'name' is a C-string that is the human readable name for the variable
#define KS_REQ_TYPE(_obj, _type, _name) KS_REQ((_obj)->type == (_type), "Incorrect type for '%s', expected '%S', but got '%S'", _name, _type, (_obj)->type)




#endif /* KS_H__ */
