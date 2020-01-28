/* main.c - main module file */

// module header
#include "ksm_mm.h"

// since this is a ks_module, include the boilerplate
#define MODULE_NAME "mm"
#include "ks_module.h"

// declare the mm.Audio type
ks_type ks_T_mm_Audio = NULL; 

// initialization code for the module
MODULE_INIT() {

    // initalize libav
    //av_register_all();

    // create the module
    ks_module mod = ks_module_new_c(MODULE_NAME);

    // helper macro to add a C function to a type
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    // create our type
    ks_T_mm_Audio = ks_type_new("mm.Audio");

    // populate with member functions    
    ADDCF(ks_T_mm_Audio, "__new__",     "mm.Audio.__new__(self)",     mm_Audio_new_);
    ADDCF(ks_T_mm_Audio, "__str__",     "mm.Audio.__str__(self)",     mm_Audio_str_);
    ADDCF(ks_T_mm_Audio, "__repr__",    "mm.Audio.__repr__(self)",    mm_Audio_repr_);
    ADDCF(ks_T_mm_Audio, "__getattr__", "mm.Audio.__getattr__(self)", mm_Audio_getattr_);
    ADDCF(ks_T_mm_Audio, "__getitem__", "mm.Audio.__getitem__(self, idx)", mm_Audio_getitem_);
    ADDCF(ks_T_mm_Audio, "__setitem__", "mm.Audio.__setitem__(self, idx, val)", mm_Audio_setitem_);
    ADDCF(ks_T_mm_Audio, "__free__",    "mm.Audio.__free__(self)",    mm_Audio_free_);
    ADDCF(ks_T_mm_Audio, "write",       "mm.Audio.write(self, fname)",    mm_Audio_write_);

    /* add types to module */

    MODULE_ADD_TYPE(mod, "Audio", ks_T_mm_Audio);

    // return our module
    return (kso)mod;
}

