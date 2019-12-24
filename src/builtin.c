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
            if (a_str == NULL) return NULL;
        } else {
            return ks_err_add_str_fmt(_FUNCSIG ": `str(args[%d])` failed, `%s.str(self)` was not callable", i, args[i]->type->name._);
        }

        // check and make sure it is a string
        if (a_str->type != kso_T_str) {
            return ks_err_add_str_fmt(_FUNCSIG ": `str(args[%d])` failed, `%s.str(self)` was not of type `str`", i, args[i]->type->name._);
        }

        // print the whole thing
        printf("%s ", KSO_CAST(kso_str, a_str)->v_str._);

        // free the result that was given back
        kso_free(a_str);
    }

    // print a newline, by default
    printf("\n");

    // return `none`
    return (kso)KSO_NONE;
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

// int.free is superflous, because no additional memory is used


/* TYPE: str */

TYPEFUNC(str, init) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.init(self, val=\"\")"
    REQ_N_ARGS_B(1, 2);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    if (n_args == 1) {
        // default to 0
        self->v_str = KS_STR_EMPTY;
    } else {
        kso val = args[1];
        //TRYCALL(),
        kso res = NULL;
        TRYCALL("val.str()", "val.str", res, val->type->f_str, 1, &val);

        REQ_TYPE("val.str()", res, kso_T_str);

        // now, just make it the string
        // we can do this, because `str()` should return a string, 
        // so this internal method is the only reference
        self->v_str._ = KSO_CAST(kso_str, res)->v_str._;
    }

    return (kso)self;
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

    //ks_str_free(&self->v_str._);

    return (kso)KSO_NONE;
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

    _CFUNC(str_init),
    _CFUNC(str_free),
    _CFUNC(str_bool),
    _CFUNC(str_int),
    _CFUNC(str_str),
    _CFUNC(str_repr)

;


/*

global builtin types

*/

static struct kso_type
    T_type = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("type"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = NULL,
        .f_int  = NULL,
        .f_str  = NULL,
        .f_repr = NULL,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_none = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("none"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = (kso)&_none_bool,
        .f_int  = (kso)&_none_int,
        .f_str  = (kso)&_none_str,
        .f_repr = (kso)&_none_repr,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_bool = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("bool"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = (kso)&_bool_bool,
        .f_int  = (kso)&_bool_int,
        .f_str  = (kso)&_bool_str,
        .f_repr = (kso)&_bool_repr,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_int = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("int"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = (kso)&_int_bool,
        .f_int  = (kso)&_int_int,
        .f_str  = (kso)&_int_str,
        .f_repr = (kso)&_int_repr,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_str = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("str"),
        .f_init = (kso)&_str_init,
        .f_free = (kso)&_str_free,

        .f_bool = (kso)&_str_bool,
        .f_int  = (kso)&_str_int,
        .f_str  = (kso)&_str_str,
        .f_repr = (kso)&_str_repr,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_list = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("list"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = NULL,
        .f_int  = NULL,
        .f_str  = NULL,
        .f_repr = NULL,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_cfunc = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("cfunc"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = NULL,
        .f_int  = NULL,
        .f_str  = NULL,
        .f_repr = NULL,

        .f_get  = NULL,
        .f_set  = NULL,
    },
    T_vm = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("vm"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = NULL,
        .f_int  = NULL,
        .f_str  = NULL,
        .f_repr = NULL,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_code = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("code"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = NULL,
        .f_int  = NULL,
        .f_str  = NULL,
        .f_repr = NULL,

        .f_get  = NULL,
        .f_set  = NULL,

    },
    T_kfunc = {
        .type = &T_type,
        .flags = KSOF_IMMORTAL,
        .name = KS_STR_CONST("kfunc"),
        .f_init = NULL,
        .f_free = NULL,

        .f_bool = NULL,
        .f_int  = NULL,
        .f_str  = NULL,
        .f_repr = NULL,

        .f_get  = NULL,
        .f_set  = NULL,
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
        .v_cfunc = _F_print
    },
    F_exit = {
        .type = &T_cfunc,
        .flags = KSOF_IMMORTAL,
        .v_cfunc = _F_exit
    }

;

/* externally visibile global constants which behave as normal objects */

kso_cfunc
    kso_F_print = &F_print,
    kso_F_exit = &F_exit
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
    kso_T_kfunc  = &T_kfunc

;

