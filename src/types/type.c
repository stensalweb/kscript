/* types/type.c - implementation of the 'type' type, which represents abstract types
 *
 * I.e. `type(type(x))` should return 'type'
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_type);



// initialize type type
void ks_type_type_init() {
    KS_INIT_TYPE_OBJ(ks_type_type);

}

