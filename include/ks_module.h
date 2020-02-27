/* ks_module.h - header to be included when defining a kscript module extension in C 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef KS_MODULE_H__
#define KS_MODULE_H__

#ifndef MODULE_NAME
#error Need to define MODULE_NAME for ks_module.h
#endif

// create a function for initializing the module:
// example:
// MODULE_INIT() {
// /* init code goes here */
// }
#define MODULE_INIT() static KS_MFUNC((_module, init)

// add a C function to a module:
// i.e. MODULE_ADD_CKS_FUNC(module, "funcname", "decl(a, b)", my_func_)
// my_func_ needs to be of `ks_cfunc_sig` to work
#define MODULE_ADD_CKS_FUNC(_mod, _cstr, _sigs, _cfunc) { ks_cfunc made_cfunc = ks_cfunc_new(_cfunc, _sigs); ks_dict_set_cstr((_mod)->__dict__, _cstr, (kso)made_cfunc); KSO_DECREF(made_cfunc); }

// add a type to a module:
// i.e. MODULE_ADD_TYPE(module, "MyType", T_mytype)
#define MODULE_ADD_TYPE(_mod, _cstr, _type) { ks_dict_set_cstr((_mod)->__dict__, _cstr, (kso)(_type)); KSO_DECREF(_type); }

// add a value as an attribute to the module
// i.e. MODULE_ADD_VAL(module, "MyType", T_mytype)
#define MODULE_ADD_VAL(_mod, _cstr, _val) { ks_dict_set_cstr((_mod)->__dict__, _cstr, (kso)(_val)); KSO_DECREF(_val); }


// declare the initialization function

// include helper macros for the module
#include "ks_common.h"

// declare the initialization function, which should be defined in 1 file
MODULE_INIT();

// create a loadable symbol named `_this_init_` that makes the kscript module loadable from the main interpreter
ks_module_init_t _this_init_ = { .f_init = _module_init_ };

#endif

