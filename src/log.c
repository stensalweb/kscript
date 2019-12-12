/* log.c - logging utilities for kscript, including debug levels */

#include "kscript.h"

// formatting colors
#define BOLD   "\033[1m"
#define RESET  "\033[0m"
#define WHITE  "\033[37m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"

// current logging level
static int ks_loglvl = KS_LOGLVL_INFO;

// printed names for the levels (including colors)
static const char* _lvl_names[] = {
    WHITE  "TRACE",
    WHITE  "DEBUG",
    WHITE  "INFO ",
    YELLOW "WARN ",
    RED    "ERROR"
};

// returns current logging level
int ks_get_loglvl() {
    return ks_loglvl;
}

// sets the level
void ks_set_loglvl(int new_lvl) {
    // first clamp it
    if (new_lvl > KS_LOGLVL_ERROR) new_lvl = KS_LOGLVL_ERROR;
    else if (new_lvl < KS_LOGLVL_TRACE) new_lvl = KS_LOGLVL_TRACE;
    
    ks_loglvl = new_lvl;
}

// logs with a levl. use the macros `ks_info`, etc
void ks_log(int level, const char *file, int line, const char* fmt, ...) {
    if (level < ks_loglvl) {
        return;
    }

    fprintf(stdout, BOLD "%s" RESET ": ", _lvl_names[level]);

    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
    fprintf(stdout, "\n");
    fflush(stdout);

}
