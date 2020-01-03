/* types/type.c - represents a type-type */

#include "ks_common.h"









/* exporting functionality */


struct ks_type T_type, *ks_T_type = &T_type;

void ks_init__type() {

    T_type = (struct ks_type) {
        KS_TYPE_INIT("type")

    };
}



