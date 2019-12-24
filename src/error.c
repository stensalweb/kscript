/* error.c - error handling in kcsript, basically a global error stack

*/

#include "kscript.h"

// current error stack
static ks_list err_stk = KS_LIST_EMPTY;

// return NULL, always so it can be returned as an error code
void* ks_err_add_str(ks_str msg) {

    // add it to the error stack
    ks_list_push(&err_stk, (kso)kso_str_new(msg));

    return NULL;
}

// adds a format string as an error
void* ks_err_add_str_fmt(const char* fmt, ...) {
    ks_str fmtted = KS_STR_EMPTY;

    va_list ap;
    va_start(ap, fmt);
    int res = ks_str_vcfmt(&fmtted, fmt, ap);
    va_end(ap);

    ks_err_add_str(fmtted);

    // it makes a copy
    ks_str_free(&fmtted);

    return NULL;
}

// number of errors
int ks_err_N() {
    return err_stk.len;
}

// pop off an error
kso ks_err_pop() {
    if (err_stk.len <= 0) {
        return NULL;
    } else {
        return ks_list_pop(&err_stk);
    }
}


// dump out all errors
bool ks_err_dumpall() {
    int i;
    for (i = 0; i < err_stk.len; ++i) {
        kso erri = err_stk.items[i];
        if (erri->type == kso_T_str) {
            ks_error("%s", KSO_CAST(kso_str, erri)->v_str._);
        } else {
            ks_error("obj @ %p", erri);
        }

        kso_free(erri);
    }
    bool res = err_stk.len != 0;

    err_stk.len = 0;
    return res;
}

