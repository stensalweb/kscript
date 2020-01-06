/* ks_module.h - header to be included when defining a kscript module extension in C */

#pragma once
#ifndef KS_MODULE_H__
#define KS_MODULE_H__

#ifndef MODULE_NAME
#error Need to define MODULE_NAME for ks_module.h
#endif

#include "ks.h"

// create a function for initializing the module:
// example:
// MODULE_INIT() {
// /* init code goes here */
// }
#define MODULE_INIT() MFUNC(_this, init)

// boilerplate to be included at the end of the module file, so it is a valid kscript module
#define MODULE_END() ks_module_init_t _module_init = {.f_init = _this_init_};

// add a C function to a module:
// i.e. MODULE_ADD_CFUNC(module, "asdfa", my_func_)
// my_func_ needs to be of `ks_cfunc_sig` to work
#define MODULE_ADD_CFUNC(_mod, _cstr, _cfunc) { ks_cfunc made_cfunc = ks_cfunc_new(_cfunc); ks_dict_set_cstr((_mod)->__dict__, _cstr, (kso)made_cfunc); KSO_DECREF(made_cfunc); }

#endif

