/* types/none.c - the none-type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_none);

ks_none KS_NONE;


// initialize none type
void ks_type_none_init() {
    KS_INIT_TYPE_OBJ(ks_type_none);

    // initialize global singleton
    KS_NONE = KS_ALLOC_OBJ(ks_none);

    KS_INIT_OBJ(KS_NONE, ks_type_none);

}

