/* int.c - implementation of some basic integer functionality 



*/

#include "kscript.h"


kso_int kso_int_new(ks_int val) {
    if (val >= 0 && val < KSO_INT_CACHE_MAX) {
        return kso_V_int_tbl + val;
    } else if (val >= -KSO_INT_CACHE_MAX && val < 0) {
        return kso_V_int_tbl + 2 * KSO_INT_CACHE_MAX + val;
    } else {
        // construct
        kso_int ret = (kso_int)ks_malloc(sizeof(*ret));
        ret->type = kso_T_int;
        ret->flags = KSOF_NONE;
        ret->refcnt = 0;
        ret->v_int = val;
        return ret;
    }
}


