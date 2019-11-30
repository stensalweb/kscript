// src/kscript.h - header file for the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#ifndef KSCRIPT_H_
#define KSCRIPT_H_

// standard header files
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>





// string type for `kscript`. It's based off code I wrote for EZC, almost exactly
typedef struct {

    // this is the internal C-style null-terminated string (which may be `NULL`)
    char* _;

    // this is the length of the string (not including NULL-terminator)
    int len;

    // this is the maximum length the string has been, so we don't resize until necessary
    int max_len;

} ks_str;

// this represents the `NULL` string, which is also valid as a starting string
#define KS_STR_EMPTY ((ks_str){ ._ = NULL, .len = 0, .max_len = 0 })

// represents a view of a C-string. i.e. nothing is copied, and any modifications made
//   stay in the original string
#define KS_STR_VIEW(_charp, _len) ((ks_str){ ._ = (char*)(_charp), .len = (int)(_len), .max_len = (int)(_len) })

// useful for string constants, like `KS_STR_CONST("Hello World")`
#define KS_STR_CONST(_charp) KS_STR_VIEW(_charp, strlen(_charp))

// copies `len` bytes from charp, and then NULL-terminate
void ks_str_copy_cp(ks_str* str, char* charp, int len);
// copies a string to another string
void ks_str_copy(ks_str* str, ks_str from);
// concatenates two strings into another one
void ks_str_concat(ks_str* str, ks_str A, ks_str B);
// appends an entire string
void ks_str_append(ks_str* str, ks_str A);
// appends a character to the string
void ks_str_append_c(ks_str* str, char c);
// frees a string and its resources
void ks_str_free(ks_str* str);
// compares two strings, should be equivalent to `strcmp(A._, B._)`
int ks_str_cmp(ks_str A, ks_str B);
// whether or not the two strings are equal
#define ks_str_eq(_A, _B) (ks_str_cmp((_A), (_B)) == 0)












/* logging */

// levels of logging
enum {
    KS_LOGLVL_ERROR = 0,
    KS_LOGLVL_WARN,
    KS_LOGLVL_INFO,
    KS_LOGLVL_DEBUG,
    KS_LOGLVL_TRACE
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
#define ks_warn(...) kl_log(KS_LOGLVL_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define ks_error(...) ks_log(KS_LOGLVL_ERROR, __FILE__, __LINE__, __VA_ARGS__)




#endif

