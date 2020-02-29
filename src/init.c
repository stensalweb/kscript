/* init.c - holds code for the initialization of the kscript library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"



// initialize the whole library
bool ks_init() {

    // initialize the builtin types
    ks_type_type_init();
    ks_type_none_init();
    ks_type_bool_init();
    ks_type_int_init();
    ks_type_str_init();
    ks_type_dict_init();

    ks_type_cfunc_init();

    // success
    return true;
}

