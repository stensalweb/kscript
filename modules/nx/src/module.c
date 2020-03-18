/* modules/nx/src/module.c - source file for the main initialization of the 'nx' (numerics) library
 *   for kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "nx"

// include this since this is a module.
#include "ks-module.h"

// main header
#include "nx-impl.h"


bool hasInit = false;

// export everything
static ks_module get_module() {
    assert(!hasInit && "nx library initialized twice!");

    // attempt to initialize everything
    nx_init__array();
    nx_init__view();

    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){

        /* misc */
        {"__version__", (ks_obj)ks_tuple_new_version(0, 0, 1)},

        /* types */
        {"array",               KS_NEWREF(nx_type_array)},
        {"view",                KS_NEWREF(nx_type_view)},

        {NULL, NULL},
    });

    // finally
    hasInit = true;
    return mod;
}

// boiler plate code
MODULE_INIT(get_module)

