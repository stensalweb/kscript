/* builtin.c - builtin functions & types

types included here:

  * type
  * none
  * bool
  * int
  * float
  * str
  * cfunc

functions included:

  * print


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
#undef int
#undef float


/* utility check macros */

// require n_args be equal to `amt`
#define REQ_N_ARGS(_amt) { if (n_args != (_amt)) { return ks_err_add_str_fmt(_FUNCSIG ": expected %d args, but got %d args", (_amt), n_args); } }

// require n_args to be between _min and _max, else throw an error
#define REQ_N_ARGS_B(_min, _max) { if (n_args < (_min) || n_args > (_max)) { return ks_err_add_str_fmt(_FUNCSIG ": expected between %d and %d args, but got %d args", (_min), (_max), n_args); } }

// require `_obj` to be of type `_type`, else throw an error
// _name is what the object should be in kscript terms (i.e. a named parameter), its just a string
#define REQ_TYPE(_name, _obj, _type) { if ((_obj) == NULL) { return ks_err_add_str_fmt(_FUNCSIG ": expected %s to be of type `%s`, but it was NULL", (_name), (_type)->name._); } if ((_obj)->type != (_type)) { return ks_err_add_str_fmt(_FUNCSIG ": expected %s to be of type `%s`, but got `%s`", (_name), (_type)->name._, (_obj)->type->name._); } }

// calls an object with a list of arguments
// res = obj(n_args, args)
// uses `_fcall` and `_objname` in error message
#define TRYCALL(_fcall, _objname, _res, _obj, _n_args, _args) { \
    if ((_obj) == NULL) { \
        return ks_err_add_str_fmt(_FUNCSIG ": `%s` failed, `%s` doesn't exist", (_fcall), (_objname)); \
    } else if ((_obj)->type == kso_T_cfunc) { \
        _res = KSO_CAST(kso_cfunc, _obj)->_cfunc((_n_args), (_args)); \
    } else { \
        return ks_err_add_str_fmt(_FUNCSIG ": `%s` failed, `%s` was not callable", (_fcall), (_objname)); \
    } \
}

// same as above, but only calls if `_obj` is not NULL, otherwise nothing is done
// useful for things that are optionally defined
#define TRYCALL_IFE(_fcall, _objname, _obj, _n_args, _args) { \
    if ((_obj) == NULL) { \
    } else if ((_obj)->type == kso_T_cfunc) { \
        _res = KSO_CAST(kso_cfunc, _obj)->_cfunc((_n_args), (_args)); \
    } else { \
        return ks_err_add_str_fmt(_FUNCSIG ": `%s` failed, `%s` was not callable", (_fcall), (_objname)); \
    } \
}

// utility macro for a function name
#define FUNC(_name) static kso _F_##_name(int n_args, kso* args)

// utility macro for defining a type function with a given name
#define TYPEFUNC(_type, _name) static kso _type##_##_name(int n_args, kso* args)



/* FUNCTIONS */

FUNC(print) {
    #undef _FUNCSIG
    #define _FUNCSIG "print(args...)"

    int i;
    for (i = 0; i < n_args; ++i) {
        kso f_str = args[i]->type->f_str;
        kso str_res = NULL;
        if (f_str == NULL) {
            return ks_err_add_str_fmt(_FUNCSIG ": `args[%d]`'s type (`%s`) did not have a `str()` function, so it can't be printed", i, args[i]->type->name._);
        } else if (f_str->type == kso_T_cfunc) {
            str_res = KSO_CAST(kso_cfunc, f_str)->_cfunc(1, &args[i]);
        } else {
            return ks_err_add_str_fmt(_FUNCSIG ": `str(args[%d])` failed, `%s.str(self)` was not callable", i, args[i]->type->name._);
        }

        if (str_res == NULL) return NULL;

        if (str_res->type != kso_T_str) {
            return ks_err_add_str_fmt(_FUNCSIG ": `spr(args[%d])` failed, `%s.spr(self)` was not of type `str`", i, args[i]->type->name._);
        }

        kso_str str_str = KSO_CAST(kso_str, str_res);

        printf("%s ", str_str->_str._);
    }

    printf("\n");

    return kso_new_none();
}


FUNC(exit) {
    #undef _FUNCSIG
    #define _FUNCSIG "exit(code=0)"
    REQ_N_ARGS_B(0, 1);

    if (n_args == 0) {
        exit(0);
    } else {
        kso code = args[0];
        if (code->type == kso_T_int) {
            exit((int)KSO_CAST(kso_int, code)->_int);
        } else {
            // TODO: make int
            ks_warn("Called exit() with unsupported arguent `%s`", code->type->name._);
            exit(1);
        }
    }

    printf("\n");

    return kso_new_none();
}


/* TYPES */

/* TYPE: none */

TYPEFUNC(none, init) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.init(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return args[0];
}


TYPEFUNC(none, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return kso_new_str(KS_STR_CONST("none"));
}

TYPEFUNC(none, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return kso_new_bool(false);
}

TYPEFUNC(none, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return kso_new_int(0);
}

TYPEFUNC(none, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "none.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_none);

    return kso_new_str(KS_STR_CONST("none"));
}

// none.free is superflous, because no additional memory is used

/* TYPE: bool */

TYPEFUNC(bool, init) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.init(self, val=false)"
    REQ_N_ARGS_B(1, 2);
    REQ_TYPE("self", args[0], kso_T_bool);

    kso_bool self = KSO_CAST(kso_bool, args[0]);

    if (n_args == 1) {
        // default to false
        self->_bool = false;
    } else {
        kso val = args[1];
        if (val->type == kso_T_bool) {
            self->_bool = KSO_CAST(kso_bool, val)->_bool;
        } else if (val->type == kso_T_int) {
            self->_bool = KSO_CAST(kso_int, val)->_int == 0 ? false : true;
        } else if (val->type == kso_T_float) {
            self->_bool = KSO_CAST(kso_float, val)->_float == 0.0 ? false : true;
        } else if (val->type == kso_T_str) {
            self->_bool = KSO_CAST(kso_str, val)->_str.len == 0 ? false : true;
        } else {
            kso res = NULL;
            TRYCALL("val.bool()", "val.bool", res, val->type->f_bool, 1, &val);
            REQ_TYPE("val.bool()", res, kso_T_bool);
            self->_bool = KSO_CAST(kso_bool, res)->_bool;
        }
    }

    return (kso)self;
}


TYPEFUNC(bool, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_bool);

    kso_bool self = KSO_CAST(kso_bool, args[0]);

    if (self->_bool) {
        return kso_new_str(KS_STR_CONST("true"));
    } else {
        return kso_new_str(KS_STR_CONST("false"));
    }
}

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

    if (self->_bool) {
        return kso_new_int(1);
    } else {
        return kso_new_int(0);
    }
}

TYPEFUNC(bool, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "bool.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_bool);

    kso_bool self = KSO_CAST(kso_bool, args[0]);

    if (self->_bool) {
        return kso_new_str(KS_STR_CONST("true"));
    } else {
        return kso_new_str(KS_STR_CONST("false"));
    }
}


// bool.free is superflous, because no additional memory is used

/* TYPE: int */

TYPEFUNC(int, init) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.init(self, val=0)"
    REQ_N_ARGS_B(1, 2);
    REQ_TYPE("self", args[0], kso_T_int);

    kso_int self = KSO_CAST(kso_int, args[0]);

    if (n_args == 1) {
        // default to 0
        self->_int = 0;
    } else {
        kso val = args[1];
        if (val->type == kso_T_bool) {
            self->_int = KSO_CAST(kso_bool, val)->_bool ? 1 : 0;
        } else if (val->type == kso_T_int) {
            self->_int = KSO_CAST(kso_int, val)->_int;
        } else if (val->type == kso_T_float) {
            self->_int = (ks_int)(KSO_CAST(kso_float, val)->_float);
        } else if (val->type == kso_T_str) {
            self->_int = atoll(KSO_CAST(kso_str, val)->_str._);
        } else {
            kso res = NULL;
            TRYCALL("val.int()", "val.int", res, val->type->f_int, 1, &val);
            REQ_TYPE("val.int()", res, kso_T_bool);
            self->_int = KSO_CAST(kso_int, res)->_int;
        }
    }

    return (kso)self;
}

TYPEFUNC(int, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);



    kso_int self = KSO_CAST(kso_int, args[0]);

    return kso_new_str_fmt("%lld", self->_int);
}

TYPEFUNC(int, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "int.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);

    kso_int self = KSO_CAST(kso_int, args[0]);

    return kso_new_bool(self->_int == 0 ? false : true);
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

    return kso_new_str_fmt("%lld", self->_int);
}


// int.free is superflous, because no additional memory is used


/* TYPE: float */

TYPEFUNC(float, init) {
    #undef _FUNCSIG
    #define _FUNCSIG "float.init(self, val=0.0)"
    REQ_N_ARGS_B(1, 2);
    REQ_TYPE("self", args[0], kso_T_float);

    kso_float self = KSO_CAST(kso_float, args[0]);

    if (n_args == 1) {
        // default to 0
        self->_float = 0.0;
    } else {
        kso val = args[1];
        if (val->type == kso_T_bool) {
            self->_float = KSO_CAST(kso_bool, val)->_bool ? 1.0 : 0.0;
        } else if (val->type == kso_T_int) {
            self->_float = (ks_float)KSO_CAST(kso_int, val)->_int;
        } else if (val->type == kso_T_float) {
            self->_float = (KSO_CAST(kso_float, val)->_float);
        } else if (val->type == kso_T_str) {
            self->_float = atof(KSO_CAST(kso_str, val)->_str._);
        } else {
            return ks_err_add_str_fmt(_FUNCSIG ": `val` was of invalid type `%s`", val->type->name._);
        }
    }

    return (kso)self;
}

TYPEFUNC(float, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "float.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_int);

    kso_float self = KSO_CAST(kso_float, args[0]);

    return kso_new_str_fmt("%lf", self->_float);
}


TYPEFUNC(float, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "float.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_float);

    kso_float self = KSO_CAST(kso_float, args[0]);

    return kso_new_bool(self->_float == 0.0 ? false : true);
}

TYPEFUNC(float, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "float.int(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_float);

    kso_float self = KSO_CAST(kso_float, args[0]);

    return kso_new_int((ks_int)self->_float);
}

TYPEFUNC(float, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "float.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_float);

    kso_float self = KSO_CAST(kso_float, args[0]);

    return kso_new_str_fmt("%lf", self->_float);
}

// float.free is useless, because no memory is taken up


/* TYPE: str */

TYPEFUNC(str, init) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.init(self, val=\"\")"
    REQ_N_ARGS_B(1, 2);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    if (n_args == 1) {
        // default to 0
        self->_str = KS_STR_EMPTY;
    } else {
        kso val = args[1];
        //TRYCALL(),
        kso res = NULL;
        TRYCALL("val.str()", "val.str", res, val->type->f_str, 1, &val);

        REQ_TYPE("val.str()", res, kso_T_str);

        // now, just make it the string
        // we can do this, because `str()` should return a string, 
        // so this internal method is the only reference
        self->_str = KSO_CAST(kso_str, res)->_str;
    }

    return (kso)self;
}

TYPEFUNC(str, repr) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.repr(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    return kso_new_str_fmt("\"%s\"", self->_str._);
}


TYPEFUNC(str, bool) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.bool(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    if (ks_str_eq(self->_str, KS_STR_CONST("true"))) {
        return kso_new_bool(true);
    } else {
        return kso_new_bool(false);
    }
}

TYPEFUNC(str, int) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.int(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    return kso_new_int((ks_int)atoll(self->_str._));
}

TYPEFUNC(str, str) {
    #undef _FUNCSIG
    #define _FUNCSIG "str.str(self)"
    REQ_N_ARGS(1);
    REQ_TYPE("self", args[0], kso_T_str);

    kso_str self = KSO_CAST(kso_str, args[0]);

    return kso_new_str(self->_str);
}


static struct kso_type T_cfunc;

// creates a CFUNC object from a C function
#define _CFUNC(_name) _##_name = { .type = &T_cfunc, .flags = 0x0, ._cfunc = _name }

static struct kso_cfunc 
    _CFUNC(none_init),
    _CFUNC(none_repr),
    _CFUNC(none_bool),
    _CFUNC(none_int),
    _CFUNC(none_str),

    _CFUNC(bool_init),
    _CFUNC(bool_repr),
    _CFUNC(bool_bool),
    _CFUNC(bool_int),
    _CFUNC(bool_str),

    _CFUNC(int_init),
    _CFUNC(int_repr),
    _CFUNC(int_bool),
    _CFUNC(int_int),
    _CFUNC(int_str),

    _CFUNC(float_init),
    _CFUNC(float_repr),
    _CFUNC(float_bool),
    _CFUNC(float_int),
    _CFUNC(float_str),

    _CFUNC(str_init),
    _CFUNC(str_repr),
    _CFUNC(str_bool),
    _CFUNC(str_int),
    _CFUNC(str_str)

;

/* construct the internal types */
static struct kso_type
    T_type = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("type")
    },
    T_none = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("none"),
        .f_init = (kso)&_none_init,
        .f_repr = (kso)&_none_repr,

        .f_bool = (kso)&_none_bool,
        .f_int  = (kso)&_none_int,
        .f_str  = (kso)&_none_str,

        .f_free = NULL

    },
    T_bool = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("bool"),
        .f_init = (kso)&_bool_init,
        .f_repr = (kso)&_bool_repr,

        .f_bool = (kso)&_bool_bool,
        .f_int  = (kso)&_bool_int,
        .f_str  = (kso)&_bool_str,

        .f_free = NULL
    },
    T_int = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("int"),

        .f_init = (kso)&_int_init,
        .f_repr = (kso)&_int_repr,

        .f_bool = (kso)&_int_bool,
        .f_int  = (kso)&_int_int,
        .f_str  = (kso)&_int_str,

        .f_free = NULL
    },
    T_float = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("float"),

        .f_init = (kso)&_float_init,
        .f_repr = (kso)&_float_repr,

        .f_bool = (kso)&_float_bool,
        .f_int  = (kso)&_float_int,
        .f_str  = (kso)&_float_str,

        .f_free = NULL
    },
    T_str = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("str"),

        .f_init = (kso)&_str_init,
        .f_repr = (kso)&_str_repr,

        .f_bool = (kso)&_str_bool,
        .f_int  = (kso)&_str_int,
        .f_str  = (kso)&_str_str,

        .f_free = NULL
    },
    T_cfunc = {
        .type = &T_type,
        .flags = 0x0,
        .name = KS_STR_CONST("cfunc")
    }
;

/* export C functions too */

static struct kso_cfunc 
    F_print = {
        .type = &T_cfunc,
        .flags = 0x0,
        ._cfunc = _F_print
    },
    F_exit = {
        .type = &T_cfunc,
        .flags = 0x0,
        ._cfunc = _F_exit
    }
;


// now, export as the global functions & types (which were defined as `extern` in kscript.h)

kso_cfunc
    kso_F_print = &F_print,
    kso_F_exit = &F_exit
;

kso_type 
    kso_T_type  = &T_type,
    kso_T_none  = &T_none,
    kso_T_bool  = &T_bool,
    kso_T_int   = &T_int,
    kso_T_float = &T_float,
    kso_T_str   = &T_str,
    kso_T_cfunc = &T_cfunc
;





