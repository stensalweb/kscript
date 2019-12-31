/* log.c - logging utilities for kscript, including debug levels */

#include "ks.h"

// formatting colors
#define BOLD   "\033[1m"
#define RESET  "\033[0m"
#define WHITE  "\033[37m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"

// current logging level
static int log_level = KS_LOG_INFO;

// printed names for the levels (including colors)
static const char* _level_strs[] = {
    WHITE  "TRACE",
    WHITE  "DEBUG",
    WHITE  "INFO ",
    YELLOW "WARN ",
    RED    "ERROR"
};

// returns current logging level
int ks_log_level() {
    return log_level;
}

// sets the level
void ks_log_level_set(int new_level) {
    // first clamp it
    if (new_level > KS_LOG_ERROR) new_level = KS_LOG_ERROR;
    else if (new_level < KS_LOG_TRACE) new_level = KS_LOG_TRACE;
    
    log_level = new_level;
}

// logs with a levl. use the macros `ks_info`, etc
void ks_log(int level, const char *file, int line, const char* fmt, ...) {
    if (level < log_level) {
        // not important
        return;
    }

    // TODO: perhaps roll my own printf? similar to what I did for ks_str_new_cfmt()
    // by my tests, it performed about 9x-10x faster than using snprintf, even with small, simple arguments
    // although, this doesn't seem like the best place. Perhaps I will implement it as `ks_printf`, and then
    // call `ks_printf` here

    // print a header
    fprintf(stdout, BOLD "%s" RESET ": ", _level_strs[level]);

    // call the vfprintf
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    // always end with a newline for this function
    fprintf(stdout, "\n");

    // flush the output
    fflush(stdout);
}
