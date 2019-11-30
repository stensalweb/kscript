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

