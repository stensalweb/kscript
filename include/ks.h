/* ks.h - main header for the kscript library.
 *
 * Meant to be a flexible, dynamic programming language with fast startup time, easy
 *   C extension API, with a rich standard library. 
 *
 * Much is a work in progress, but most of the basic types are implemented. The next big goals are:
 *
 * * user-defined types (kscript-defined types)
 * * function closures (scope hoisting and carrying)
 * * constructors/class programming interface (how will __new__ / __call__ work?)
 *
 * Further down the line, the first big public changes neccessary before any release or tutorial is
 *   really written are:
 *  * Module/package system (this has not really been done at all. We want a very clear way to install
 *      and manage packages)
 *  * Versioning/compatability. Obviously, for the first bit, the language will be rapidly changing.
 *      eventually, it should be mostly backwards compatible in a way people can count on and not have
 *      to care about the specific version they have or are writing for. A lot needs to be standardized
 *      that has not been determined yet
 *  * Tests/example cases, including failures
 *  * Proper error types, so catch blocks can discriminate correctly
 *
 * See more at: https://github.com/ChemicalDevelopment/kscript
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef KS_H__
#define KS_H__

/* constants about kscript & versioning */

// the major version of the release
#define KS_VER_MAJOR 0

// the minor version of the release
#define KS_VER_MINOR 0

// the patch version of the release
#define KS_VER_PATCH 1


/* kscript configuration options */

// generated from `./ks_config.T.h`
#include <ks_config.h>


/* standard system headers */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>

/* timing headers (which may be different on windows/mac) */
#include <time.h>
#include <sys/time.h>

/* headers that require linker flags */
// -lm
#include <math.h>



/* macros/definitions */

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


/* kscript sub-headers */

/* include bytecode definitions */
#include <ks_bytecode.h>

/* include the builtin type definitions */
#include <ks_types.h>

/* include the builtin functions  */
#include <ks_funcs.h>

/* object interface, generic object manipulation */
#include <kso.h>


/* general library functions */

// initializes the kscript library. This should be the first function you call, and it should
//   only be called once
void ks_init();

/* memory allocation */

// allocates an area of at least `bytes` bytes, returning a pointer
// NOTE: with bytes==0, ks_malloc returns NULL
void* ks_malloc(size_t bytes);

// attempts to reallocate an area of memory, ensuring the new pointer can hold `bytes` of memory
// use this like the standard realloc() function, like: x = ks_realloc(x, new_size);
// NOTE: with bytes==0, ks_realloc attempts to free `ptr`, and returns NULL
//       with ptr==NULL, ks_realloc returns ks_malloc(bytes)
// Only call this function with pointers returned by `ks_malloc`
void* ks_realloc(void* ptr, size_t bytes);

// frees a pointer allocated by `ks_malloc`, or which has been reallocated using `ks_realloc`
// NOTE: ks_free(NULL) is safe; it does nothing
void  ks_free(void* ptr);


// returns the current amount of memory allocated. This is always a lower estimate than what is
// actually being used, because internally the system may request blocks which are larger than neccessary.
// These are just the bytes that ks_malloc and other functions know about
size_t ks_memuse();

// returns the maximum amount of memory that was allocated at a single time, i.e. the peak of `ks_memuse()`
size_t ks_memuse_max();


// returns the time, in seconds, since the library started. It uses a fairly precise timer,
//   but is just provided as a convenience. Do not expect any particular accuracy out of the results
//   of this function
double ks_time();


/* logging */

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

/* handle config options, disable them */
#ifdef KS_C_NO_TRACE
#undef ks_trace
#define ks_trace(...)
#endif
#ifdef KS_C_NO_DEBUG
#undef ks_debug
#define ks_debug(...)
#endif


/* error generation functions */

// add a C-string as an error message
void* kse_add(const char* errmsg);

// add an object as an error
void* kse_addo(kso errmsg);

// add a C-style format string (internally using ks_str_new_vcfmt) to generate an error message
// NOTE: Not all common formats are supported
void* kse_fmt(const char* fmt, ...);

// add a C-style format string to generate an error message, with a token
// the token will be used to generate additional helpful information in the error message
void* kse_tok(ks_tok tok, const char* fmt, ...);

// number of errors currently being held, 0 if none
int kse_N();

// pop off an error, NULL if there is no error
kso kse_pop();

// dump out all errors to the console, returns true if there were any, false if none
bool kse_dumpall();

// clear the errors, returning true if there were any
bool kse_clear();


/* assertions/requirements for checking & generating errors */

// assert that `_expr` is true, issuing an error if not
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_ASSERT(_expr, ...) { if (!(_expr)) { return kse_fmt("AssertError: " __VA_ARGS__); } }

// assert that `_expr` is true, issuing an error if not
// should be used only in Cfuncs/functions that will return NULL
// this just requires something to be true, but does not throw an AssertError if it is not,
// just a generic error
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ(_expr, ...) { if (!(_expr)) { return kse_fmt(__VA_ARGS__); } }


// error that you can't convert `_obj` to type `_type`
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_ERR_TYPECONV(_obj, _type) return kse_fmt("Don't know how to convert type '%S' to type '%S'", (_obj)->type->name, (_type)->name);


// require that an object has a given type, or print an error. _name is for printing purposes
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ_TYPE(_obj, _type, _name) KS_REQ((_obj)->type == (_type), "'type(%s)' (%T) was not '%s'", _name, _obj, (_type)->name->chr)

// require that an object be a sub type of a given type, or print an error. _name is for printing purposes
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ_SUBTYPE(_obj, _type, _name) KS_REQ(ks_type_issub((_obj)->type, (_type)), "'type(%s)' (%T) was not a subtype of '%s'", _name, _obj, (_type)->name->chr)


// assert that the number of arguments is a correct value
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ_N_ARGS(_narg, _correct) KS_REQ((_narg) == (_correct), "Wrong number of args, expected %i, but got %i", _correct, _narg)

// assert that the number of arguments is at least a minimum
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ_N_ARGS_MIN(_narg, _min) KS_REQ((_narg) >= (_min), "Wrong number of args, expected at least %i, but got %i", _min, _narg)

// assert that the number of arguments is at most a maximum
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ_N_ARGS_MAX(_narg, _max) KS_REQ((_narg) <= (_max), "Wrong number of args, expected at most %i, but got %i", _max, _narg)

// assert a given number of arguments
// NOTE: Should be used only in Cfuncs that return a kso (i.e. this will return NULL)
#define KS_REQ_N_ARGS_RANGE(_narg, _min, _max) KS_REQ((_narg) >= (_min) && (_narg) <= (_max), "Wrong number of args, expected between %i and %i, but got %i", _min, _max, _narg)


/* global state */

// return the global dictionary
ks_dict ks_get_globals();


/* VM execution */


// executes a chunk of code, discarding the results
void ks_vm_exec(ks_code code);

// internal coredump method
void ks_vm_coredump();

/* hash/utils */

// returns a hash from some bytes
// provided as an inline function. I may move this so its not inline
// and fix the magic constants 7 and 31
static inline uint64_t ks_hash_bytes(uint8_t* chr, int len) {
    uint64_t ret = 7;
    int i;
    for (i = 0; i < len; ++i) {
        ret = ret * 31 + chr[i];
    }
    return ret;
}


/* random generation */

// returns a random 64 bit signed integer
int64_t ks_random_i64();


/* module/extern loading */

// list of strings for searching for modules
extern ks_list ksm_search_path;


#endif

