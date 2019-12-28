/* util.c - general utility functionality */


#include "kscript.h"


// declare table here
struct kso_int kso_V_int_tbl[2 * KSO_INT_CACHE_MAX];


void ks_init() {
    // initialize constants
    //kso_init_consts();

    // initialize integer constants


    int i;
    for (i = 0; i < KSO_INT_CACHE_MAX; ++i) {

        kso_V_int_tbl[i] = (struct kso_int) {
            .type = kso_T_int,
            .flags = KSOF_IMMORTAL,
            .v_int = i
        };
    }

    for (i = -1; i >= -KSO_INT_CACHE_MAX; --i) {
        kso_V_int_tbl[2 * KSO_INT_CACHE_MAX + i] = (struct kso_int) {
            .type = kso_T_int,
            .flags = KSOF_IMMORTAL,
            .v_int = i
        };
    }
}



