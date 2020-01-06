/* types/bool.c - implementation of the builtin `bool` type */

#include "ks_common.h"


/* exporting functionality */

struct ks_type T_bool, *ks_T_bool = &T_bool;

struct ks_bool 
    V_true , *ks_V_true  = &V_true,
    V_false, *ks_V_false = &V_false
;

void ks_init__bool() {

    /* first create the type */
    T_bool = (struct ks_type) {
        KSO_BASE_INIT(ks_T_bool)

        .name = ks_str_new("bool")

    };

    //create constant with 1 reference
    V_true = (struct ks_bool) {
        KSO_BASE_INIT_RF(1, KSOF_IMMORTAL, ks_T_bool)
        .v_bool = true
    };
    V_false = (struct ks_bool) {
        KSO_BASE_INIT_RF(1, KSOF_IMMORTAL, ks_T_bool)
        .v_bool = false
    };

}




