/* log.c - kscript logging functions 
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

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


// keep true if is logging
static bool is_logging = false;

// logs with a levl. use the macros `ks_info`, etc
void ks_log(int level, const char *file, int line, const char* fmt, ...) {
    if (level < log_level) {
        // not important
        return;
    }

    // check for recursion
    if (is_logging) return;

    is_logging = true;

    // TODO: perhaps roll my own printf? similar to what I did for ks_str_new_cfmt()
    // by my tests, it performed about 9x-10x faster than using snprintf, even with small, simple arguments
    // although, this doesn't seem like the best place. Perhaps I will implement it as `ks_printf`, and then
    // call `ks_printf` here
    // for now, just generate the string, print it, then free it

    // print a header
    fprintf(stdout, BOLD "%s" RESET ": ", _level_strs[level]);

    // call the vfprintf
    va_list args;
    va_start(args, fmt);

    // use advanced formatting
    /*
    ks_str rstr = ks_str_new_vcfmt(fmt, args);
    fwrite(rstr->chr, 1, rstr->len, stdout);
    KSO_DECREF(rstr);
    */
    //vfprintf(stdout, fmt, args);
    ks_str gen_str = ks_fmt_vc(fmt, args);
    va_end(args);

    fprintf(stdout, "%s", gen_str->chr);

    KS_DECREF(gen_str);

    // always end with a newline for this function
    fprintf(stdout, "\n");

    // flush the output
    fflush(stdout);

    is_logging = false;

}
