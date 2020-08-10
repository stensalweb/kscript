/* func.c - implementation of the abstract function class
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"



/* export */

KS_TYPE_DECLFWD(ks_T_func);

void ks_init_T_func() {
    ks_type_init_c(ks_T_func, "func", ks_T_object, KS_KEYVALS(
    ));

}
