/* types/bool.c - implementation of the boolean (true/false) type for kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_bool);

ks_bool KS_TRUE, KS_FALSE;


// initialize bool type
void ks_type_bool_init() {
    KS_INIT_TYPE_OBJ(ks_type_bool, "bool");

    // initialize global singletons
    KS_TRUE = KS_ALLOC_OBJ(ks_bool);
    KS_FALSE = KS_ALLOC_OBJ(ks_bool);

    KS_INIT_OBJ(KS_TRUE, ks_type_bool);
    KS_INIT_OBJ(KS_FALSE, ks_type_bool);

    KS_TRUE->val = true;
    KS_TRUE->val = false;

}

