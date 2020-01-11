/* ks_common.h - common helper functions/macros and standardized error handling 

NOTE: This file is not included by default in `ks.h`, because it defines non-standard names that may conflict.

If you include this file, you don't need to include `ks.h`

Mainly, this defines `REQ_*` macros, which help with readability for function errors.

In the future, this will mostly be replaced, or even removed by more proper error handling

*/

#pragma once
#ifndef KS_COMMON_H__
#define KS_COMMON_H__

#include "ks.h"

// define SIG as the C-string descibing the signature of the function. This will help with errors
// ex: #define SIG "myfunc(A, B)"
#define SIG
#undef SIG

// require a specific number of arguments
// ex: REQ_N_ARGS(2)
#define REQ_N_ARGS(_num) if (n_args != _num) { return kse_fmt(SIG ": Expected %i args, but got %i", _num, n_args); }

// require a minimum number of arguments
// ex: REQ_N_ARGS_MIN(1)
#define REQ_N_ARGS_MIN(_min) if (n_args < _min) { return kse_fmt(SIG ": Expected >=%i args, but got %i", _min, n_args); }

// require a maximum number of arguments
// ex: REQ_N_ARGS_MAX(3)
#define REQ_N_ARGS_MAX(_max) if (n_args > _max) { return kse_fmt(SIG ": Expected <=%i args, but got %i", _max, n_args); }

// require a range of number of arguments
// ex: REQ_N_ARGS_RANGE(0, 10)
#define REQ_N_ARGS_RANGE(_min, _max) if (n_args > _max || n_args < _min) { return kse_fmt(SIG ": Expected %i<=args<=%i, but got %i", _min, _max, n_args); }

// require an object _obj to be of type _type, or print an error. The object is referred to as its C-string style name `_objname`
// ex: REQ_TYPE("MyVar", myvar_obj, ks_T_int)
#define REQ_TYPE(_objname, _obj, _type) if (!(_obj) || (_obj)->type != (_type)) { return kse_fmt(SIG ": Expected %s to be of type '%*s', but it was of type '%*s'", _objname, (_type)->name->len, (_type)->name->chr, (_obj)->type->name->len, (_obj)->type->name->chr); }

// define a C-function with a given name
// (the actual name has an _ appended)
// ex: FUNC(myfunc) -> defines a C-function called `myfunc_`
#define FUNC(_name) kso _name##_(int n_args, kso* args)

// define a C-function with a given type and name
// (the actual name has an _ appended)
// ex: TFUNC(int, free) -> defines a C-function called `int_free_`
#define TFUNC(_type, _name) kso _type##_##_name##_(int n_args, kso* args)

// define a C-function with a given module and name
// (the actual name has an _ appended)
// ex: MFUNC(mymod, free) -> defines a C-function called `mymod_free_`
#define MFUNC(_mod, _name) kso _mod##_##_name##_(int n_args, kso* args)


#endif
