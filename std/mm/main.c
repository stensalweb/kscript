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

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_mm_Audio, "__repr__", "mm.Audio.__repr__(self)", mm_Audio_repr_);
    //ADDCF(ks_T_mm_Audio, "__call__", "mm.Audio.__call__(self)", mm_Audio_call_);
    //ADDCF(ks_T_mm_Audio, "__free__", "mm.Audio.__free__(self)", mm_Audio_free_);
    // add our type
    MODULE_ADD_TYPE(mod, "Audio", ks_T_mm_Audio);

    // return our module
    return (kso)mod;
}

// finalize everything, making it a valid kscript module
MODULE_END();




