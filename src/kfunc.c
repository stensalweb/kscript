// kfunc.c - implementation of the kscript function interface
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

int ks_kfunc_add_param(ks_kfunc* kfunc, ks_str name) {
    int idx = kfunc->params_n++;
    kfunc->param_names = realloc(kfunc->param_names, sizeof(ks_str) * kfunc->params_n);
    kfunc->param_names[idx] = ks_str_dup(name);
    return idx;
}

