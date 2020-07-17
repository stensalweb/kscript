/* init.c - handles initialization of the kscript library
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// whether or not kscript has been initialized
static bool hasInit = false;


bool ks_init() {
    if (hasInit) return true;


    // First, initialize types
    ks_init_T_logger();

    ks_logger _ks = ks_logger_get("ks", true);
    KS_DECREF(_ks);

    ks_init_T_obj();
    ks_init_T_none();
    ks_init_T_bool();
    ks_init_T_int();
    ks_init_T_str();
    ks_init_T_str_builder();

    ks_init_T_list();
    //ks_init_T_tuple();
    ks_init_T_dict();

    ks_init_T_cfunc();

    ks_init_T_Error();
    ks_init_T_thread();

    // initialize others
    ks_init_funcs();

    // success
    return hasInit = true;
}

