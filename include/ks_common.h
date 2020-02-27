/* ks_common.h - common helper functions/macros and standardized error handling 
 *
 * If you include this file, you don't need to include `ks.h`
 * 
 */

#pragma once
#ifndef KS_COMMON_H__
#define KS_COMMON_H__

#include "ks.h"

// define a C-function with a given name
// (the actual name has an _ appended)
// ex: KS_FUNC(myfunc) -> defines a C-function called `myfunc_`
#define KS_FUNC(_name) kso _name##_(int n_args, kso* args)

// define a C-function with a given type and name
// (the actual name has an _ appended)
// ex: KS_TFUNC(int, free) -> defines a C-function called `int_free_`
#define KS_TFUNC(_type, _name) kso _type##_##_name##_(int n_args, kso* args)

// define a C-function with a given module and name
// (the actual name has an _ appended)
// ex: KS_MFUNC(mymod, free) -> defines a C-function called `mymod_free_`
#define KS_MFUNC(_mod, _name) kso _mod##_##_name##_(int n_args, kso* args)


#endif
