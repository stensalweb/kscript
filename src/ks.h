/* ks.h - main header for the kscript library


*/

#pragma once
#ifndef KS_H__
#define KS_H__


/* standard system headers */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

/* timing headers (which may be different on windows/mac) */
#include <time.h>
#include <sys/time.h>


/* headers that require linker flags */
// -lm
#include <math.h>


/* object interface */
#include "kso.h"

/* the builtin types */
extern ks_type
    ks_T_none,
    ks_T_bool,
    ks_T_int,
    ks_T_str,
    ks_T_tuple,
    ks_T_list,
    ks_T_dict,
    ks_T_cfunc,
    ks_T_code,
    ks_T_ast,
    ks_T_parser,
    ks_T_vm,
    ks_T_type
;

/* builtin functions */
extern ks_cfunc
    ks_F_print,
    ks_F_add,
    ks_F_sub,
    ks_F_mul,
    ks_F_div
;



/* general library functions */

// initializes the kscript library. This should be the first function you call, and it should
//   only be called once
void ks_init();

// returns the time, in seconds, since the library started. It uses a fairly precise timer,
//   but is just provided as a convenience
double ks_time();

// allocates an area of at least `bytes` bytes, returning a pointer
// NOTE: with bytes==0, ks_malloc returns NULL
void* ks_malloc(size_t bytes);

// attempts to reallocate an area of memory, ensuring the new pointer can hold `bytes` of memory
// use this like the standard realloc() function, like: x = ks_realloc(x, new_size);
// NOTE: with bytes==0, ks_realloc returns NULL
//       with ptr==NULL, ks_realloc returns ks_malloc(bytes)
// Only call this function with pointers returned by `ks_malloc`
void* ks_realloc(void* ptr, size_t bytes);

// frees a pointer allocated by `ks_malloc`, or which has been reallocated using `ks_realloc`
// NOTE: ks_free(NULL) is safe; it does nothing
void  ks_free(void* ptr);

// returns the current amount of memory allocated. This is always a lower estimate than what is
// actually being used, because internally the system may request blocks which are larger than neccessary.
// These are just the bytes that ks_malloc/& know about
size_t ks_memuse();

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
#define ks_info(...) ks_log(KS_LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
// prints a warn message, assuming the current log level allows for it
#define ks_warn(...) ks_log(KS_LOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
// prints a error message, assuming the current log level allows for it
#define ks_error(...) ks_log(KS_LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)

/* global error system */

// add a C-string as an error message
void* kse_add(const char* errmsg);

// add a ks_str as an error message
void* kse_addo(ks_str errmsg);

// add a C-style format string (internally using ks_str_new_vcfmt) to generate an error message
// NOTE: Not all common formats are supported
void* kse_fmt(const char* fmt, ...);

// number of errors currently being held, 0 if none
int kse_N();

// pop off an error, NULL if there is no error
kso kse_pop();

// dump out all errors to the console, returns true if there were any, false if none
bool kse_dumpall();



/* internal methods */

// INTERNAL METHOD, DO NOT CALL
void kso_init();

// INTERNAL METHOD, DO NOT CALl
void ksf_init();

// INTERNAL METHOD; DO NOT CALL
void kse_init();


#endif

