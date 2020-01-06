/* types/none.c - implementation of the builtin `none` type */

#include "ks_common.h"


/* exporting functionality */

struct ks_type T_none, *ks_T_none = &T_none;

struct ks_none V_none, *ks_V_none = &V_none;

void ks_init__none() {

    /* first create the type */

    // create the type
    T_none = (struct ks_type) {
        KSO_BASE_INIT(ks_T_type)

        .name = ks_str_new("none")

    };

    // create the global none
    V_none = (struct ks_none) {
        KSO_BASE_INIT_RF(1, KSOF_IMMORTAL, ks_T_none)
    };
}




