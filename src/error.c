/* error.c - error handling in kcsript

*/

#include "kscript.h"

// current error stack
static ks_list err_stk = KS_LIST_EMPTY;


// return NULL, always so it can be returned
void* ks_err_add_str(ks_str msg) {

    // add it to the error stack
    ks_list_push(&err_stk, kso_new_str(msg));

    return NULL;
}


// adds a format replacement
void* ks_err_add_str_fmt(const char* fmt, ...) {
    ks_str fmtted = KS_STR_EMPTY;

    va_list ap;
    va_start(ap, fmt);
    int res = ks_str_vfmt(&fmtted, fmt, ap);
    va_end(ap);

    ks_err_add_str(fmtted);

    // it makes a copy
    ks_str_free(&fmtted);

    return NULL;
}

int ks_err_N() {
    return err_stk.len;
}

kso ks_err_pop() {
    if (err_stk.len <= 0) {
        return NULL;
    } else {
        return ks_list_pop(&err_stk);
    }
}



