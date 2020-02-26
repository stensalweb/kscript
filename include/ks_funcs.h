/* ks_funcs.h - the standard/builtin functions, as kscript objects

Included:

UTIL/GENERIC:
  * repr(obj) : returns the string representation of an object
  * type(obj) : returns the type of `obj`
  * call(obj, args=(,)) : calls `obj` as a function, given arguments
  * hash(obj) : returns an object's hash, as an integer
  * print(*objs) : prints all objects out to the console

MODULE/NAMESPACING:
  * import(name) : imports a given file as a module, returns item

OPERATORS:
  * __add__(a, b) : returns a+b
  * __sub__(a, b) : returns a-b
  * __mul__(a, b) : returns a*b
  * __div__(a, b) : returns a/b
  * __mod__(a, b) : returns a%b
  * __pow__(a, b) : returns a**b
  * __lt__(a, b) : returns a<b
  * __le__(a, b) : returns a<=b
  * __gt__(a, b) : returns a>b
  * __ge__(a, b) : returns a>=b
  * __eq__(a, b) : returns a==b
  * __ne__(a, b) : returns a!=b
  * __neg__(a) : returns -a
  
MATH/MISC:
  * rand() : returns a random integer
  * 

*/


#pragma once
#ifndef KS_FUNCS_H__
#define KS_FUNCS_H__

// only include if not already included
#ifndef KS_H__
#include "ks.h"
#endif

/* global singletons representing the builtin functions as objects */
extern ks_cfunc
    /* utils */
    ks_F_repr,
    ks_F_type,
    ks_F_call,
    ks_F_hash,
    ks_F_print,
    ks_F_exit,
    
    ks_F_import,
    ks_F_rand,

    ks_F_new_type,

    ks_F_dict,

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

    /* unary operators */
    ks_F_neg,
    ks_F_sqig,

    /* comparison */
    ks_F_lt,
    ks_F_le,
    ks_F_gt,
    ks_F_ge,
    ks_F_eq,
    ks_F_ne,

    ks_F_shell

;






#endif
