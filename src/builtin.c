/* builtin.c - builtin functions & types


functions included:

  * print
  * repr
  * exit
  * __add__
  * __sub__
  * __mul__
  * __div__
  * __mod__
  * __pow__
  * __lt__
  * __gt__
  * __eq__

types included:

  * type
  * none
  * bool
  * int
  * float
  * str
  * cfunc
  * list
  * dict


For defining a function, use a FUNC macro like:

FUNC(fname) {
    #define _FUNCSIG "fname(self, ...)"
    REQ_N_ARGS_B(1, 2)
}

_FUNCSIG is the macro that expands to the current function signature, useful for error macros
(this should be in kscript terminology, not C)

You can use `REQ_N_ARGS_...` to ensure enough arguments are given

*/


#include "kscript.h"

// so there are no naming conflicts
#undef bool

/* utility check macros */

// require n_args be equal to `amt`
#define REQ_N_ARGS(_amt) { if (n_args != (_amt)) { return ks_err_add_str_fmt(_FUNCSIG ": expected %d args, but got %d args", (_amt), n_args); } }

// require n_args to be between _min and _max, else throw an error
#define REQ_N_ARGS_B(_min, _max) { if (n_args < (_min) || n_args > (_max)) { return ks_err_add_str_fmt(_FUNCSIG ": expected between %d and %d args, but got %d args", (_min), (_max), n_args); } }

// require n_args to be at least _min, else throw an error
#define REQ_N_ARGS_MIN(_min) { if (n_args < (_min)) { return ks_err_add_str_fmt(_FUNCSIG ": expected between at least %d args, but got %d args", (_min), n_args); } }

// requires a given argument to be not-null, giving an error if it is
#define REQ_NOTNULL(_name, _obj) { if ((_obj) == NULL) { return ks_err_add_str_fmt(_FUNCSIG ": `%s` does not exist", _name); } }

// require `_obj` to be of type `_type`, else throw an error
// _name is what the object should be in kscript terms (i.e. a named parameter), its just a string
#define REQ_TYPE(_name, _obj, _type) { if ((_obj) == NULL) { return ks_err_add_str_fmt(_FUNCSIG ": expected %s to be of type `%s`, but it was NULL", (_name), (_type)->name._); } if ((_obj)->type != (_type)) { return ks_err_add_str_fmt(_FUNCSIG ": expected %s to be of type `%s`, but got `%s`", (_name), (_type)->name._, (_obj)->type->name._); } }

// errors out with a given type mismatch
#define TYPE_MISMATCH(_A, _B) { return ks_err_add_str_fmt(_FUNCSIG ": type mismatch of `%s` and `%s`", (_A)->type->name._, (_B)->type->name._); }

// calls an object with a list of arguments
// res = obj(n_args, args)
// uses `_fcall` and `_objname` in error message
#define TRYCALL(_fcall, _objname, _res, _obj, _n_args, _args) { \
    if ((_obj) == NULL) { \
        return ks_err_add_str_fmt(_FUNCSIG ": `%s` failed, `%s` doesn't exist", (_fcall), (_objname)); \
    } else if ((_obj)->type == kso_T_cfunc) { \
        _res = KSO_CAST(kso_cfunc, _obj)->v_cfunc(vm, (_n_args), (_args)); \
        if (_res == NULL) return NULL; \
    } else { \
        return ks_err_add_str_fmt(_FUNCSIG ": `%s` failed, `%s` was not callable", (_fcall), (_objname)); \
    } \
}

// same as above, but only calls if `_obj` is not NULL, otherwise nothing is done
// useful for things that are optionally defined
#define TRYCALL_IFE(_fcall, _objname, _obj, _n_args, _args) { \
    if ((_obj) == NULL) { \
    } else if ((_obj)->type == kso_T_cfunc) { \
        _res = KSO_CAST(kso_cfunc, _obj)->_cfunc(vm, (_n_args), (_args)); \
        if (_res == NULL) return NULL; \
    } else { \
        return ks_err_add_str_fmt(_FUNCSIG ": `%s` failed, `%s` was not callable", (_fcall), (_objname)); \
    } \
}

// utility macro for a function name
#define FUNC(_name) static KS_CFUNC_SIG(_F_##_name)

// utility macro for defining a type function with a given name
#define TYPEFUNC(_type, _name) static KS_CFUNC_SIG(_type##_##_name)


/* FUNCTIONS */

/* print(args...) -> prints the str() of all arguments,
     seperated by a space

This internally uses a.type.str(a) to compute the string

*/
FUNC(print) {
    #undef _FUNCSIG
    #define _FUNCSIG "print(args...)"

    int i;
    for (i = 0; i < n_args; ++i) {
        // get the tostring method from the ith arg
        kso f_str = args[i]->type->f_str;

        // the result of str(a)
        kso a_str = NULL;

        if (f_str == NULL) {
            return ks_err_add_str_fmt(_FUNCSIG ": `args[%d]`'s type (`%s`) did not have a `str()` function, so it can't be printed", i, args[i]->type->name._);
        } else if (f_str->type == kso_T_cfunc) {
            a_str = KSO_CAST(kso_cfunc, f_str)->v_cfunc(vm, 1, &args[i]);
            KSO_INCREF(a_str);
        } else if (f_str->type == kso_T_kfunc) {
            a_str = kso_vm_call(vm, f_str, 1, &args[i]);
        } else {
            return ks_err_add_str_fmt(_FUNCSIG ": `str(args[%d])` failed, `%s.str(self)` was not callable", i, args[i]->type->name._);
        }

        if (a_str == NULL) return NULL;
        // check and make sure it is a string
        if (a_str->type != kso_T_str) {
            return ks_err_add_str_fmt(_FUNCSIG ": `str(args[%d])` failed, `%s.str(self)` was not of type `str`", i, args[i]->type->name._);
        }

        // print the whole thing
        printf("%s ", KSO_CAST(kso_str, a_str)->v_str._);

        // free the result that was given back
        //kso_free(a_str);
        KSO_DECREF(a_str);
    }

    // print a newline, by default
    printf("\n");

    // return `none`
    return (kso)KSO_NONE;
}


/* repr - get string representation */
FUNC(repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "repr(A)"
    REQ_N_ARGS(1);

    kso A = args[0];

    // get the tostring method from the ith arg
    kso f_repr = A->type->f_str;

    if (f_repr == NULL) {
        return (kso)kso_str_new_cfmt("<'%s' obj @ %p>", A->type->name._, A);
    } else {
        return kso_vm_call(vm, f_repr, 1, &A);
    }
}


/* exit(code=0) -> exits the entire program, optionally with an error code


*/
FUNC(exit) {
    #undef _FUNCSIG
    #define _FUNCSIG "exit(code=0)"
    // special case so that it will still exit, even if called weirdly
    if (n_args > 1) {
        ks_warn("called exit() with %d arguments, should only call with 0 or 1", n_args);
    }
    //REQ_N_ARGS_B(0, 1);

    if (n_args == 0) {
        exit(0);
    } else {
        kso code = args[0];
        if (code->type == kso_T_int) {
            exit((int)KSO_CAST(kso_int, code)->v_int);
        } else {
            // TODO: make int
            ks_warn("Called exit() with unsupported argument type `%s`", code->type->name._);
            exit(1);
        }
    }

    // return none, even though it will never get here
    return (kso)KSO_NONE;
}


/* returns total memory usage */
FUNC(memuse) {
    #undef _FUNCSIG
    #define _FUNCSIG "memuse()"
    REQ_N_ARGS(0);
    return kso_int_new(ks_memuse());
}


/* OPERATORS/BUILTINS */


FUNC(getattr) {
    #undef _FUNCSIG
    #define _FUNCSIG "getattr(A, aname)"
    REQ_N_ARGS(2);

    kso A = args[0];

    kso T_getattr = A->type->f_getattr;
    if (T_getattr == NULL) {
        return ks_err_add_str_fmt(_FUNCSIG ": Object `A` of type `%s` has no `.getattr()` method", A->type->name._);
    } else {

        return kso_vm_call(vm, T_getattr, n_args, args);
    }

    return (kso)KSO_NONE;
}


FUNC(setattr) {
    #undef _FUNCSIG
    #define _FUNCSIG "setattr(A, aname, val)"
    REQ_N_ARGS(3);

    kso A = args[0];

    kso T_setattr = A->type->f_setattr;

    if (T_setattr == NULL) {
        return ks_err_add_str_fmt(_FUNCSIG ": Object `A` of type `%s` has no `.setattr()` method", A->type->name._);
    } else {
        return kso_vm_call(vm, T_setattr, n_args, args);
    }

    return (kso)KSO_NONE;
}



FUNC(get) {
    #undef _FUNCSIG
    #define _FUNCSIG "get(A, idx...)"
    REQ_N_ARGS_MIN(2);

    kso A = args[0];

    kso T_get = A->type->f_get;
    if (T_get == NULL) {
        return ks_err_add_str_fmt(_FUNCSIG ": Object `A` of type `%s` has no `.get()` method", A->type->name._);
    } else {
        return kso_vm_call(vm, T_get, n_args, args);
    }


    return (kso)KSO_NONE;

    // do standard operator resolving
    //OP_RESOLVE(f_add, args);
}


FUNC(set) {
    #undef _FUNCSIG
    #define _FUNCSIG "set(A, idx..., val)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS_MIN(2);

    kso A = args[0];

    kso T_set = A->type->f_set;
    if (T_set == NULL) {
        return ks_err_add_str_fmt(_FUNCSIG ": Object `A` of type `%s` has no `.set()` method", A->type->name._);
    } else {
        return kso_vm_call(vm, T_set, n_args, args);
    }

    return (kso)KSO_NONE;

    // do standard operator resolving
    //OP_RESOLVE(f_add, args);
}


// resolves an operator call, given an operator function name defined in the struct kso_type
// for example: OP_RESOLVE(f_add, &args[0])
#define OP_RESOLVE(_opfname, _args) { \
    kso _A = (_args)[0], _B = (_args)[1]; \
    if (_A->type == _B->type) { \
        kso T_opf = _A->type->_opfname; \
        if (T_opf == NULL) { TYPE_MISMATCH(_A, _B); } \
        else { \
            return kso_vm_call(vm, T_opf, 2, _args); \
        } \
    } else { \
        kso AT_opf = _A->type->_opfname; \
        if (AT_opf == NULL) { TYPE_MISMATCH(_A, _B); } \
        else { \
            return kso_vm_call(vm, AT_opf, 2, _args); \
        } \
    } \
    return (kso)KSO_NONE; \
}

FUNC(add) {
    #undef _FUNCSIG
    #define _FUNCSIG "add(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_add, args);
}

FUNC(sub) {
    #undef _FUNCSIG
    #define _FUNCSIG "sub(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_sub, args);
}

FUNC(mul) {
    #undef _FUNCSIG
    #define _FUNCSIG "mul(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_mul, args);
}

FUNC(div) {
    #undef _FUNCSIG
    #define _FUNCSIG "div(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_div, args);
}

FUNC(mod) {
    #undef _FUNCSIG
    #define _FUNCSIG "mod(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_mod, args);
}

FUNC(pow) {
    #undef _FUNCSIG
    #define _FUNCSIG "pow(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_pow, args);
}

FUNC(lt) {
    #undef _FUNCSIG
    #define _FUNCSIG "lt(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_lt, args);
}

FUNC(gt) {
    #undef _FUNCSIG
    #define _FUNCSIG "gt(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_gt, args);
}

FUNC(eq) {
    #undef _FUNCSIG
    #define _FUNCSIG "eq(A, B)"
    // special case so that it will still exit, even if called weirdly
    REQ_N_ARGS(2);

    // do standard operator resolving
    OP_RESOLVE(f_eq, args);
}


/* TYPE: none 

There is only a single global `none`, it is `kso_V_none`, it will never be freed

*/

TYPEFUNC(none, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return (kso)KSO_FALSE;
}

TYPEFUNC(none, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return (kso)KSO_0;
}

TYPEFUNC(none, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return (kso)kso_str_new(KS_STR_CONST("none"));
}

TYPEFUNC(none, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return (kso)kso_str_new(KS_STR_CONST("none"));
}

/* TYPE: bool

There are 2 global immortal values, `kso_V_true`, and `kso_V_false`

So, all booleans are one of these two pointers

*/


TYPEFUNC(bool, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_bool);

    return args[0];
}

TYPEFUNC(bool, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.int(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_bool);

    kso_bool self = KSO_CAST(kso_bool, args[0]);

    return (kso)(self->v_bool ? KSO_1 : KSO_0);
}

TYPEFUNC(bool, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_bool);

    kso_bool self = KSO_CAST(kso_bool, args[0]);

    return (kso)(kso_str_new(self->v_bool ? KS_STR_CONST("true") : KS_STR_CONST("false")));
}

TYPEFUNC(bool, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_bool);

    kso_bool self = KSO_CAST(kso_bool, args[0]);

    return (kso)(kso_str_new(self->v_bool ? KS_STR_CONST("true") : KS_STR_CONST("false")));
}


/* TYPE: int */

TYPEFUNC(int, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);

    kso_int self = KSO_CAST(kso_int, args[0]);

    return (kso)(self->v_int == 0 ? KSO_FALSE : KSO_TRUE);
}

TYPEFUNC(int, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.int(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);

    return args[0];
}

TYPEFUNC(int, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);

    kso_int self = KSO_CAST(kso_int, args[0]);

    return (kso)kso_str_new_cfmt("%lld", self->v_int);
}

TYPEFUNC(int, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);

    kso_int self = KSO_CAST(kso_int, args[0]);

    return (kso)kso_str_new_cfmt("%lld", self->v_int);
}


/* integer operator */

TYPEFUNC(int, add) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.add(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_int_new(KSO_CAST(kso_int, A)->v_int + KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, sub) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.sub(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_int_new(KSO_CAST(kso_int, A)->v_int - KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, mul) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.mul(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_int_new(KSO_CAST(kso_int, A)->v_int * KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, div) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.div(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_int_new(KSO_CAST(kso_int, A)->v_int / KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, mod) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.mod(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_int_new(KSO_CAST(kso_int, A)->v_int % KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, pow) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.pow(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        ks_int Ai = KSO_CAST(kso_int, A)->v_int;
        ks_int Bi = KSO_CAST(kso_int, B)->v_int;
        // X^0 == 1
        if (Bi == 0) return (kso)KSO_1;
        // 0^X == 0 (negative X's return 0s too)
        if (Ai == 0) return (kso)KSO_0;
        // X^-n == 0, for integers
        if (Bi < 0) return (kso)KSO_0;
        // X^1==X
        if (Bi == 1) return A;


        // the sign to be applied
        int sign = (Ai < 0 && Bi & 1 == 1) ? -1 : 1;
        if (Ai < 0) Ai = -Ai;

        // now, Bi>1 and Ai is positive, with sign extracted

        // now, calculate it
        ks_int Ri = 1;
        while (Bi != 0) {
            if (Bi & 1) Ri *= Ai;
            Bi >>= 1;
            Ai *= Ai;
        }

        return (kso)kso_int_new(Ri);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, lt) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.lt(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_bool_new(KSO_CAST(kso_int, A)->v_int < KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, gt) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.gt(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_bool_new(KSO_CAST(kso_int, A)->v_int > KSO_CAST(kso_int, B)->v_int);
    } else {
        TYPE_MISMATCH(A, B);
    }
}

TYPEFUNC(int, eq) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.eq(A, B)"
    REQ_N_ARGS(2);

    kso A = args[0], B = args[1];

    if (A->type == kso_T_int && B->type == kso_T_int) {
        return (kso)kso_bool_new(KSO_CAST(kso_int, A)->v_int == KSO_CAST(kso_int, B)->v_int);

    } else {
        TYPE_MISMATCH(A, B);
    }
}




// int.free is superflous, because no additional memory is used


/* TYPE: str */

TYPEFUNC(str, from) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.from(val=\"\")"
    REQ_N_ARGS_B(0, 1);

    if (n_args == 0) {
        // default to empty
        return (kso)kso_str_new(KS_STR_CONST(""));
    } else {
        kso val = args[0];
        kso T_str = val->type->f_str;

        if (T_str == NULL) {
            return ks_err_add_str_fmt("No str method");
        }

        kso res = kso_vm_call(vm, T_str, 1, &val);

        if (res == NULL) return NULL;

        REQ_TYPE("val.str()", res, kso_T_str);

        // now, just make it the string
        // we can do this, because `str()` should return a string, 
        // so this internal method is the only reference
        //self->v_str._ = KSO_CAST(kso_str, res)->v_str._;
        return res;
    }

    return (kso)KSO_NONE;
}

TYPEFUNC(str, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    return (kso)kso_bool_new(ks_str_eq(self->v_str, KS_STR_CONST("true")));
}

TYPEFUNC(str, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.int(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    return (kso)kso_int_new((ks_int)atoll(self->v_str._));
}

TYPEFUNC(str, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    // just return itself
    return (kso)self;
}

TYPEFUNC(str, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    return (kso)kso_str_new_cfmt("\"%s\"", self->v_str._);
}

TYPEFUNC(str, free) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.free(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    ks_str_free(&self->v_str._);

    return (kso)KSO_NONE;
}

TYPEFUNC(str, add) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.add(A, B)"
    REQ_N_ARGS(2);
    REQ_TYPE("A", args[0], kso_T_str);
    REQ_TYPE("B", args[1], kso_T_str);

    kso_str A = KSO_CAST(kso_str, args[0]);
    kso_str B = KSO_CAST(kso_str, args[1]);

    return (kso)kso_str_new_cfmt("%s%s", A->v_str._, B->v_str._);
}





/* TYPE: list */

TYPEFUNC(list, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_list);

    kso_list self = KSO_CAST(kso_list, args[0]);

    return (kso)(self->v_list.len == 0 ? KSO_FALSE : KSO_TRUE);
}

TYPEFUNC(list, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_list);

    kso_list self = KSO_CAST(kso_list, args[0]);

    kso_str strval = kso_str_new(KS_STR_CONST("["));

    int i;
    for (i = 0; i < self->v_list.len; ++i) {
        if (i != 0) ks_str_append(&strval->v_str, KS_STR_CONST(", "));

        // get repr
        kso argi = self->v_list.items[i];
        kso_str repr = (kso_str)_F_repr(vm, 1, &argi);

        if (repr->type != kso_T_str) {
            return ks_err_add_str_fmt(_FUNCSIG ": `repr(self[%d])` (of type '%s') gave a non-string result!", i, args[i]->type->name._);
        }

        // append
        ks_str_append(&strval->v_str, repr->v_str);

        // clean up if possible
        KSO_CHKREF(repr);

    }

    ks_str_append(&strval->v_str, KS_STR_CONST("]"));
    return (kso)strval;
}

TYPEFUNC(list, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_list);

    kso_list self = KSO_CAST(kso_list, args[0]);

    kso_str strval = kso_str_new(KS_STR_CONST("["));

    int i;
    for (i = 0; i < self->v_list.len; ++i) {
        if (i != 0) ks_str_append(&strval->v_str, KS_STR_CONST(", "));

        // get repr
        kso argi = self->v_list.items[i];
        kso_str repr = (kso_str)_F_repr(vm, 1, &argi);

        if (repr->type != kso_T_str) {
            return ks_err_add_str_fmt(_FUNCSIG ": `repr(self[%d])` (of type '%s') gave a non-string result!", i, args[i]->type->name._);
        }

        // append
        ks_str_append(&strval->v_str, repr->v_str);

        // clean up if possible
        KSO_CHKREF(repr);

    }

    ks_str_append(&strval->v_str, KS_STR_CONST("]"));
    return (kso)strval;
}

TYPEFUNC(list, add) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.add(self, other)"
    REQ_N_ARGS(2);
    REQ_TYPE("self", args[0], kso_T_list);
    REQ_TYPE("other", args[1], kso_T_list);

    kso_list self = KSO_CAST(kso_list, args[0]);
    kso_list other = KSO_CAST(kso_list, args[1]);

    kso_list new_list = kso_list_new(self->v_list.len, self->v_list.items);

    int i;
    for (i = 0; i < other->v_list.len; ++i) {
        ks_list_push(&new_list->v_list, other->v_list.items[i]);
    }

    return (kso)new_list;
}

TYPEFUNC(list, mul) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.mul(self, other)"
    REQ_N_ARGS(2);
    REQ_TYPE("self", args[0], kso_T_list);
    REQ_TYPE("other", args[1], kso_T_int);

    kso_list self = KSO_CAST(kso_list, args[0]);
    kso_int other = KSO_CAST(kso_int, args[1]);

    kso_list new_list = kso_list_new(self->v_list.len * other->v_int, NULL);

    int i;
    for (i = 0; i < other->v_int; ++i) {
        int j;
        for (j = 0; j < self->v_list.len; ++j) {
            new_list->v_list.items[i * self->v_list.len + j] = self->v_list.items[j];
        }
    }

    return (kso)new_list;
}




TYPEFUNC(list, get) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.get(self, idx)"
    REQ_N_ARGS(2);
    REQ_TYPE("self", args[0], kso_T_list);
    REQ_TYPE("idx", args[1], kso_T_int);

    kso_list self = KSO_CAST(kso_list, args[0]);
    kso_int idx = KSO_CAST(kso_int, args[1]);

    return self->v_list.items[idx->v_int];
}

TYPEFUNC(list, set) {
    #undef _FUNCSIG
    #define _FUNCSIG "list.set(self, idx, val)"
    REQ_N_ARGS(3);
    REQ_TYPE("self", args[0], kso_T_list);
    REQ_TYPE("idx", args[1], kso_T_int);

    kso_list self = KSO_CAST(kso_list, args[0]);
    kso_int idx = KSO_CAST(kso_int, args[1]);
    kso val = args[2];

    self->v_list.items[idx->v_int] = val;
    
    return val;
}


/* TYPE: obj */

TYPEFUNC(obj, free) {
    #undef _FUNCSIG
    #define _FUNCSIG "obj.free(self)"
    REQ_N_ARGS(1);

    kso_obj self = (kso_obj)args[0];
    ks_dict_free(&self->v_attrs);

    return (kso)KSO_NONE;
}

TYPEFUNC(obj, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "obj.str(self)"
    REQ_N_ARGS(1);

    return (kso)kso_str_new_cfmt("<'%s' obj @ %p>", args[0]->type->name._, args[0]);
}


TYPEFUNC(obj, getattr) {
    #undef _FUNCSIG
    #define _FUNCSIG "obj.getattr(self, key)"
    REQ_N_ARGS(2);
    //REQ_TYPE("self", args[0], kso_T_obj);
    REQ_TYPE("key", args[1], kso_T_str);

    kso_obj self = (kso_obj)args[0];
    kso_str key = (kso_str)args[1];

    kso val = ks_dict_get(&self->v_attrs, (kso)key, key->v_hash);

    if (val == NULL) {
        return ks_err_add_str_fmt("KeyError: '%s'", key->v_str._);
    } else {
        return val;
    }
}

TYPEFUNC(obj, setattr) {
    #undef _FUNCSIG
    #define _FUNCSIG "obj.setattr(self, key, val)"
    REQ_N_ARGS(3);
    //REQ_TYPE("self", args[0], kso_T_obj);
    REQ_TYPE("key", args[1], kso_T_str);

    kso_obj self = (kso_obj)args[0];
    kso_str key = (kso_str)args[1];
    kso val = args[2];

    ks_dict_set(&self->v_attrs, (kso)key, key->v_hash, val);

    return val;
}



static struct kso_type T_cfunc;

// creates a CFUNC object from a C function
#define _CFUNC(_name) _##_name = { .type = &T_cfunc, .flags = KSOF_IMMORTAL, .v_cfunc = _name }

/*

list of C-functions implementing type functions

*/

static struct kso_cfunc 
    _CFUNC(none_bool),
    _CFUNC(none_int),
    _CFUNC(none_str),
    _CFUNC(none_repr),

    _CFUNC(bool_bool),
    _CFUNC(bool_int),
    _CFUNC(bool_str),
    _CFUNC(bool_repr),

    _CFUNC(int_bool),
    _CFUNC(int_int),
    _CFUNC(int_str),
    _CFUNC(int_repr),
    _CFUNC(int_add),
    _CFUNC(int_sub),
    _CFUNC(int_mul),
    _CFUNC(int_div),
    _CFUNC(int_mod),
    _CFUNC(int_pow),
    _CFUNC(int_lt),
    _CFUNC(int_gt),
    _CFUNC(int_eq),

    _CFUNC(str_from),
    _CFUNC(str_free),
    _CFUNC(str_bool),
    _CFUNC(str_int),
    _CFUNC(str_str),
    _CFUNC(str_repr),
    _CFUNC(str_add),

    _CFUNC(list_str),
    _CFUNC(list_repr),
    _CFUNC(list_add),
    _CFUNC(list_mul),
    _CFUNC(list_get),
    _CFUNC(list_set),

    _CFUNC(obj_free),
    _CFUNC(obj_str),
    _CFUNC(obj_getattr),
    _CFUNC(obj_setattr)

;


/*

global builtin types

*/

static struct kso_type
    T_type = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("type"),
        KSO_TYPE_EMPTYFILL

    },
    T_none = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("none"),
        KSO_TYPE_EMPTYFILL

        .f_bool = (kso)&_none_bool,
        .f_int  = (kso)&_none_int,
        .f_str  = (kso)&_none_str,
        .f_repr = (kso)&_none_repr,

    },
    T_bool = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("bool"),
        KSO_TYPE_EMPTYFILL

        .f_bool = (kso)&_bool_bool,
        .f_int  = (kso)&_bool_int,
        .f_str  = (kso)&_bool_str,
        .f_repr = (kso)&_bool_repr,

    },
    T_int = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("int"),
        KSO_TYPE_EMPTYFILL

        .f_bool = (kso)&_int_bool,
        .f_int  = (kso)&_int_int,
        .f_str  = (kso)&_int_str,
        .f_repr = (kso)&_int_repr,

        .f_add  = (kso)&_int_add,
        .f_sub  = (kso)&_int_sub,
        .f_mul  = (kso)&_int_mul,
        .f_div  = (kso)&_int_div,
        .f_mod  = (kso)&_int_mod,
        .f_pow  = (kso)&_int_pow,

        .f_lt  = (kso)&_int_lt,
        .f_gt  = (kso)&_int_gt,
        .f_eq  = (kso)&_int_eq,

    },
    T_str = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("str"),
        KSO_TYPE_EMPTYFILL

        .f_call = (kso)&_str_from,
        .f_free = (kso)&_str_free,

        .f_bool = (kso)&_str_bool,
        .f_int  = (kso)&_str_int,
        .f_str  = (kso)&_str_str,
        .f_repr = (kso)&_str_repr,

        .f_add  = (kso)&_str_add,

    },
    T_list = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("list"),
        KSO_TYPE_EMPTYFILL

        .f_str  = (kso)&_list_str,
        .f_repr = (kso)&_list_repr,

        .f_add  = (kso)&_list_add,
        .f_mul  = (kso)&_list_mul,
        
        .f_get  = (kso)&_list_get,
        .f_set  = (kso)&_list_set,

    },
    T_cfunc = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("cfunc"),
        KSO_TYPE_EMPTYFILL

    },
    T_vm = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("vm"),
        KSO_TYPE_EMPTYFILL

    },
    T_code = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("code"),
        KSO_TYPE_EMPTYFILL


    },
    T_kfunc = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("kfunc"),
        KSO_TYPE_EMPTYFILL

    },
    T_obj = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("obj"),
        KSO_TYPE_EMPTYFILL
        .f_free  = (kso)&_obj_free,

        .f_str  = (kso)&_obj_str,

        .f_getattr  = (kso)&_obj_getattr,
        .f_setattr  = (kso)&_obj_setattr,
    }
;


/* global builtin constants */

static struct kso_none
    V_none = {
        .type = &T_none,
        .flags = KSOF_IMMORTAL,
    }
;

static struct kso_bool 
    V_true = {
        .type = &T_bool,
        .flags = KSOF_IMMORTAL,
        .v_bool = true
    },
    V_false = {
        .type = &T_bool,
        .flags = KSOF_IMMORTAL,
        .v_bool = false
    }
;


/* global builtin functions */

static struct kso_cfunc

    /* normal funcs */
    F_print = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_print
    },
    F_exit = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_exit
    },
    F_memuse = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_memuse
    },
    /* builtins */

    F_getattr = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_getattr
    },

    F_setattr = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_setattr
    },


    F_get = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_get
    },

    F_set = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_set
    },


    /* operator functions */

    F_add = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_add
    },
    F_sub = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_sub
    },
    F_mul = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_mul
    },
    F_div = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_div
    },
    F_mod = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_mod
    },
    F_pow = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_pow
    },
    F_lt = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_lt
    },
    F_gt = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_gt
    },
    F_eq = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .refcnt = 1,
        .v_cfunc = _F_eq
    }

;

/* externally visibile global constants which behave as normal objects */

kso_cfunc
    kso_F_print = &F_print,
    kso_F_exit = &F_exit,
    kso_F_memuse = &F_memuse,

    kso_F_getattr = &F_getattr,
    kso_F_setattr = &F_setattr,

    kso_F_get = &F_get,
    kso_F_set = &F_set,

    kso_F_add = &F_add,
    kso_F_sub = &F_sub,
    kso_F_mul = &F_mul,
    kso_F_div = &F_div,
    kso_F_mod = &F_mod,
    kso_F_pow = &F_pow,

    kso_F_lt  = &F_lt,
    kso_F_gt  = &F_gt,
    kso_F_eq  = &F_eq

;

kso_none 
    kso_V_none = &V_none
;

kso_bool 
    kso_V_true = &V_true,
    kso_V_false = &V_false
;

kso_type 
    kso_T_type   = &T_type,
    kso_T_none   = &T_none,
    kso_T_bool   = &T_bool,
    kso_T_int    = &T_int,
    kso_T_str    = &T_str,
    kso_T_list   = &T_list,
    kso_T_cfunc  = &T_cfunc,
    kso_T_vm     = &T_vm,
    kso_T_code   = &T_code,
    kso_T_kfunc  = &T_kfunc,
    kso_T_obj  = &T_obj

;

