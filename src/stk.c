// stk.c - implementation of the kscript stack
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

int ks_stk_push(ks_stk* stk, ks_obj obj) {
    int idx = stk->len++;
    if (stk->len > stk->max_len) {
        stk->max_len = (int)(1.5 * stk->len + 10);
        stk->vals = realloc(stk->vals, stk->max_len * sizeof(ks_obj));
    }

    stk->vals[idx] = obj;
    return idx;
}

ks_obj ks_stk_pop(ks_stk* stk) {
    return stk->vals[--stk->len];
}

void ks_stk_free(ks_stk* stk) {
    free(stk->vals);
    *stk = KS_STK_EMPTY;
}

