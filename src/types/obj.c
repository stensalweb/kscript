/* obj.c - implementation of the general object type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"




/* export */

KS_TYPE_DECLFWD(ks_T_obj);

void ks_init_T_obj() {
    ks_type_init_c(ks_T_obj, "obj", ks_T_obj, KS_KEYVALS(
        KS_KEYVAL_END,
    ));

}
