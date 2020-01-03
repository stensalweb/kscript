/* types/none.c - implementation of the builtin `none` type */

#include "ks_common.h"


/* exporting functionality */

struct ks_type T_none, *ks_T_none = &T_none;

struct ks_none V_none, *ks_V_none = &V_none;

void ks_init__none() {

    /* first create the type */
    T_none = (struct ks_type) {
        KS_TYPE_INIT("none")

    };

    //create constant with 1 reference
    V_none = (struct ks_none) {
        KSO_BASE_INIT_R(ks_T_none, KSOF_NONE, 1)
    };
}




