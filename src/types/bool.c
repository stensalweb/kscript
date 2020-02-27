/* types/bool.c - implementation of the builtin `bool` type */

#include "ks_common.h"


/* exporting functionality */

struct ks_type T_bool, *ks_T_bool = &T_bool;

struct ks_bool 
    V_true , *ks_V_true  = &V_true,
    V_false, *ks_V_false = &V_false
;

void ks_init__bool() {

    /* create the type */
    T_bool = KS_TYPE_INIT();

    ks_type_setname_c(ks_T_bool, "bool");

    // create the global values
    V_true = (struct ks_bool) {
        KSO_BASE_INIT_RF(1, KSOF_IMMORTAL, ks_T_bool)
        .v_bool = true
    };
    V_false = (struct ks_bool) {
        KSO_BASE_INIT_RF(1, KSOF_IMMORTAL, ks_T_bool)
        .v_bool = false
    };

}




