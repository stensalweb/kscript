/* kfunc.c - kscript functions */

#include "kscript.h"


kso_kfunc kso_kfunc_new(kso_list params, kso_code code) {
    kso_kfunc ret = (kso_kfunc)ks_malloc(sizeof(*ret));
    ret->type = kso_T_kfunc;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->code = code;
    ret->params = params;

    // record references
    KSO_INCREF(ret->code);
    KSO_INCREF(ret->params);

    return ret;
}
