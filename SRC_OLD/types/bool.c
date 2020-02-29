/* types/bool.c - implementation of the builtin `bool` type */

#include "ks_common.h"


/* operators */

KS_TFUNC(bool, sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_bool self = (ks_bool)args[0];
    KS_REQ_TYPE(self, ks_T_bool, "self");

    return self == KSO_TRUE ? KSO_FALSE : KSO_TRUE;
}


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


    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_bool, "__sqig__", "bool.__sqig__(self)", bool_sqig_);


}




