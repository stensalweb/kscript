/* type.c - implementation of types */

#include "kscript.h"


kso_type kso_type_new() {
    kso_type ret = (kso_type)ks_malloc(sizeof(*ret));
    *ret = (struct kso_type) {
        KSO_TYPE_EMPTYFILL
    };
    ret->type = kso_T_type;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    return ret;
}

