/* err.c - error handling/global error stack

*/

#include "ks.h"

// current error stack
static ks_list err_stk = NULL;

void* kse_addo(ks_str errmsg) {
    ks_list_push(err_stk, (kso)errmsg);
    return NULL;
}

// return NULL, always so it can be returned as an error code
void* kse_add(const char* errmsg) {

    // add it to the error stack
    ks_list_push(err_stk, (kso)ks_str_new_r(errmsg));

    return NULL;
}

// adds a format string as an error
void* kse_fmt(const char* fmt, ...) {

    va_list ap;
    va_start(ap, fmt);
    ks_str filled = ks_str_new_vcfmt(fmt, ap);
    va_end(ap);

    // push it onto the error stack
    ks_list_push(err_stk, (kso)filled);

    return NULL;
}

// number of errors
int kse_N() {
    return err_stk->len;
}

// pop off an error
kso kse_pop() {
    return ks_list_pop(err_stk);
}

// dump out all errors
bool kse_dumpall() {
    int i;
    for (i = 0; i < err_stk->len; ++i) {
        kso erri = err_stk->items[i];

        if (erri->type == ks_T_str) {
            ks_error("%s", ((ks_str)erri)->chr);
        } else {
            ks_error("obj @ %p", erri);
        }
        
    }
    
    ks_list_clear(err_stk);

    return i != 0;
}


// INTERNAL METHOD; DO NOT CALL
void kse_init() {
    err_stk = ks_list_new_empty();
}

