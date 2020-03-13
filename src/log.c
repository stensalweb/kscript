/* log.c - kscript logging functions 
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// current logging level
static int log_level = KS_LOG_WARN;

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


// keep true if is logging
static bool is_logging = false;

static ks_mutex mut = NULL;

void ks_log_init() {
    mut = ks_mutex_new();
}


// print variadically
void ks_printf(const char* fmt, ...) {
    //if (is_logging) return;
    //is_logging = true;
    ks_mutex_lock(mut);

    // call the vfprintf
    va_list args;
    va_start(args, fmt);

    ks_str gen_str = ks_fmt_vc(fmt, args);
    va_end(args);

    fprintf(stderr, "%s", gen_str->chr);

    KS_DECREF(gen_str);

    // flush the output
    fflush(stderr);

    ks_mutex_unlock(mut);

    //is_logging = false;
}


// logs with a levl. use the macros `ks_info`, etc
void ks_log(int level, const char *file, int line, const char* fmt, ...) {
    if (level < log_level || is_logging) {
        // not important
        return;
    }
    is_logging = true;
    ks_mutex_lock(mut);

    // TODO: perhaps roll my own printf? similar to what I did for ks_str_new_cfmt()
    // by my tests, it performed about 9x-10x faster than using snprintf, even with small, simple arguments
    // although, this doesn't seem like the best place. Perhaps I will implement it as `ks_printf`, and then
    // call `ks_printf` here
    // for now, just generate the string, print it, then free it

    // print a header
    fprintf(stderr, BOLD "%s" RESET ": ", _level_strs[level]);

    // call the vfprintf
    va_list args;
    va_start(args, fmt);

    // use advanced formatting
    /*
    ks_str rstr = ks_str_new_vcfmt(fmt, args);
    fwrite(rstr->chr, 1, rstr->len, stderr);
    KSO_DECREF(rstr);
    */
    //vfprintf(stderr, fmt, args);
    ks_str gen_str = ks_fmt_vc(fmt, args);
    va_end(args);

    fprintf(stderr, "%s\n", gen_str->chr);
    KS_DECREF(gen_str);

    // flush the output
    fflush(stderr);
    is_logging = false;

    ks_mutex_unlock(mut);
}
