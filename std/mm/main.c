/* main.c -  */



#include "ksm_mm.h"

#define MODULE_NAME "mm"

// since this is a ks_module
#include "ks_module.h"

// declare the mm.Audio type
ks_type ks_T_mm_Audio = NULL; 

// initialization code
MODULE_INIT() {

    // create our new module
    ks_module mod = ks_module_new_c(MODULE_NAME);

    // create our type
    ks_T_mm_Audio = ks_type_new("mm.Audio");

    // add our type
    MODULE_ADD_TYPE(mod, "Audio", ks_T_mm_Audio);

    // return our module
    return (kso)mod;
}

// finalize everything, making it a valid kscript module
MODULE_END();




