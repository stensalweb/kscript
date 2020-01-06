/* ks_funcs.h - the standard/builtin functions, their functionality defined

*/


#pragma once
#ifndef KS_FUNCS_H__
#define KS_FUNCS_H__

#include "ks.h"




/* global singletons representing the builtin functions as objects */
extern ks_cfunc
    ks_F_print,
    ks_F_dict,
    ks_F_type,
    ks_F_hash,
    ks_F_call,
    ks_F_rand,
    ks_F_import,

    /* conversion to representation string */
    ks_F_repr,

    /* attribute getting/setting */
    ks_F_getattr,
    ks_F_setattr,

    /* item getting/setting */
    ks_F_getitem,
    ks_F_setitem,

    /* operators */
    ks_F_add,
    ks_F_sub,
    ks_F_mul,
    ks_F_div,
    ks_F_mod,
    ks_F_pow,
    ks_F_lt,
    ks_F_le,
    ks_F_gt,
    ks_F_ge,
    ks_F_eq,
    ks_F_ne
;






#endif
