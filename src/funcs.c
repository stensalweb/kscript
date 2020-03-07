/* funcs.c - implementation of the built in functions
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"


// declare them here
ks_cfunc 
    ks_F_hash = NULL,
    ks_F_repr = NULL,
    ks_F_print = NULL,

    ks_F_getattr = NULL,
    ks_F_setattr = NULL,

    ks_F_add = NULL,
    ks_F_sub = NULL,
    ks_F_mul = NULL,
    ks_F_div = NULL,
    ks_F_mod = NULL,
    ks_F_pow = NULL
    
;

/* call(func, *args) -> obj
 *
 * Try and call 'func(*args)' and return the result
 * 
 * The rules for finding a way to call the function are:
 *   * If 'type(func)' is 'cfunc', call the C-style function with the given
 *       arguments and return the results
 *   * If 'type(func)' is 'type' and 'func.__new__' exists, try and construct a value from that type, like calling
 *       calling the constructor. If 'func.__init__' exists, call 'func.__new__' with 0 arguments, then call
 *       'func.__init__(new_obj, *args)' (where 'new_obj' is the object returned by '__new__')
 *       Otherwise, just call 'func.__new__(*args)' and return that
 *   * If 'type(func).__call__' is defined as a function, that function is called with
 *       'type(func).__call__(func, *args)' and that result is returned
 * 
 */
ks_obj ks_call(ks_obj func, int n_args, ks_obj* args) {

    if (func->type == ks_type_cfunc) {
        return ((ks_cfunc)func)->func(n_args, args);
    } else if (func->type == ks_type_type) {
        // try and construct a value by calling the constructor

        // cast it
        ks_type ft = (ks_type)func;

        if (ft->__new__ != NULL) {
            // we have a constructor
            
            // now check if the type uses an initializer
            if (ft->__init__ != NULL) {
                // uses an initializer, so just call __new__ with no arguments

                ks_obj ret = ks_call(ft->__new__, 0, NULL);

                // this is to handle derived classes; we always manually set the new type
                ks_type old_type = ret->type;
                ret->type = ft;
                KS_INCREF(ret->type);
                KS_DECREF(old_type);
                
                // now set up our newly created object as the first argument to the initializer
                ks_obj* new_args = ks_malloc(sizeof(*new_args) * (1 + n_args));

                new_args[0] = ret;
                // copy other arguments passed in
                memcpy(&new_args[1], args, n_args);

                // initialize it, don't care about return value because we are returning the result from 'ret'
                ks_obj dontcare = ks_call(ft->__init__, 1 + n_args, new_args);
                KS_DECREF(dontcare);

                // free temporary results
                ks_free(new_args);

                // now return our created object
                return ret;
            } else {
                // no initializer, so call '__new__' with all the arguments
                ks_obj ret = ks_call(ft->__new__, n_args, args);

                // this is to handle derived classes; we always manually set the new type
                ks_type old_type = ret->type;
                ret->type = ft;
                KS_INCREF(ret->type);
                KS_DECREF(old_type);
                return ret;
            }
        }
    } else if (func->type->__call__ != NULL) {
        // call type(obj).__call__(self, *args)
        ks_obj* new_args = ks_malloc(sizeof(*new_args) * (1 + n_args));
        *new_args = func;
        memcpy(&new_args[1], args, n_args);
        ks_obj ret = ks_call(func->type->__call__, 1 + n_args, new_args);
        ks_free(new_args);

        return ret;

    }

    // if nothing was returned
    return (ks_obj)ks_throw_fmt(ks_type_Error, "'%T' object was not callable!", func);
}


/* getattr(obj, attr) -> obj
 *
 * Try an get an attribute for an object
 *
 */
static KS_FUNC(getattr) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj obj = args[0], attr = args[1];

    if (obj->type->__getattr__ != NULL) {
        // call type(obj).__getattr__(obj, attr)
        return ks_call(obj->type->__getattr__, 2, (ks_obj[]){ obj, attr });
    } else if (attr->type == ks_type_str) {
        // it might be a member function
        // try type(obj).attr as a function with 'obj' filled in as the first argument
        ks_obj type_attr = ks_type_get(obj->type, (ks_str)attr);
        if (!type_attr) return NULL;

        // make sure it is a member function
        if (!ks_is_callable(type_attr)) {
            KS_DECREF(type_attr);
            KS_ERR_ATTR(obj, attr);
        }

        // now, create a partial function
        ks_pfunc ret = ks_pfunc_new(type_attr);
        KS_DECREF(type_attr);

        // fill #0 as 'self' (aka obj)
        ks_pfunc_fill(ret, 0, obj);

        // return the member function
        return (ks_obj)ret;
    }

    // error
    KS_ERR_ATTR(obj, attr);
}


/* setattr(obj, attr, val) -> none
 *
 * Try an set an attribute for an object
 *
 */
static KS_FUNC(setattr) {
    KS_REQ_N_ARGS(n_args, 3);

    ks_obj obj = args[0], attr = args[1], val = args[2];

    if (obj->type->__setattr__ != NULL) {
        // call type(obj).__setattr__(obj, attr, val)
        return ks_call(obj->type->__setattr__, 3, (ks_obj[]){ obj, attr, val });
    }

    // error
    KS_ERR_ATTR(obj, attr);
}



/* repr(obj, *args) -> str
 *
 * Try to convert 'obj' to a string representation, with optional arguments
 *   to be passed to the internal '__repr__' function (by default, none are passed)
 * 
 * The rules for converting to 'repr' are as follows:
 *   * If 'type(obj).__repr__' exists (and is callable), that function is
 *       called with (obj, *args), so 'type(obj).__repr__(obj, *args)' is returned
 * 
 *   * Else, the string returned is a string indicated by '<'$TYPE' obj @ $ADDR>',
 *       where '$TYPE' is the name of the type (i.e. 'int', 'ast', etc), and '$ADDR'
 *       is the 64 bit hex address 
 * 
 * @EXAMPLES
 * 
 * repr(123) -> '123'
 * repr("Cade\nBrown") -> 'Cade\nBrown'
 * repr("   Bird") -> '\tBird'
 * repr(MyClass()) -> '<'MyClass' obj @ 0x55d17d260228>'
 * 
 */
static KS_FUNC(repr) {
    // require at least a single argument
    KS_REQ_N_ARGS_MIN(n_args, 1);

    // get main argument
    ks_obj obj = args[0];

    // check for a type-definition 
    if (obj->type->__repr__ != NULL) {
        // call type(obj).__repr__(obj, *args)
        return ks_call(obj->type->__repr__, n_args, args);
    } else {
        // return the default format string
        return (ks_obj)ks_fmt_c("<'%T' obj @ %p>", obj, (void*)obj);
    }
}


/* hash(obj) -> int
 *
 * Try to hash 'obj' and return the 64 bit integer
 * 
 * The rules for hashing are as follows:
 *   * If 'type(obj).__hash__' exists, call it with `type(obj).__hash__(self)` and return that
 * 
 *   * Else, the object is unhashable, and an error is thrown
 * 
 */
static KS_FUNC(hash) {
    // require at least a single argument
    KS_REQ_N_ARGS(n_args, 1);

    // get main argument
    ks_obj obj = args[0];

    // handle some cases faster
    if (obj->type == ks_type_str) {
        return (ks_obj)ks_int_new(((ks_str)obj)->v_hash);
    } else {
        return (ks_obj)ks_throw_fmt(ks_type_Error, "'%T' object was not hashable!", obj);
    }

    // check for a type-definition 
    /*if (obj->type->__hash__ != NULL) {
        // call type(obj).__repr__(obj, *args)
        return ks_call(obj->type->__repr__, n_args, args);
    } else {
        // return the default format string
        return ks_fmt_c("<'%T' obj @ %p>", obj, (void*)obj);
    }*/
}



/* print(*args) -> none
 *
 * Try to print all the items in 'args' (seperated by a space), and then a newline
 * 
 * Each item is converted using 'str(obj)', and that is printed. An error may be
 *   raised if any of the object's string conversion raises an error
 * 
 */
static KS_FUNC(print) {
    // with no arguments it will just print a newline
    KS_REQ_N_ARGS_MIN(n_args, 0);

    ks_str_b SB;
    ks_str_b_init(&SB);

    int i;
    for (i = 0; i < n_args; ++i) {
        // add spaces between them, i.e. ' '.join()
        if (i != 0) ks_str_b_add_c(&SB, " ");
        // append str(args[i]) to the string builder
        ks_str_b_add_str(&SB, args[i]);
    }

    // end with a newline
    // TODO: add seperator argument
    ks_str_b_add_c(&SB, "\n");

    // get an actual string
    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    // now print out the buffer
    fwrite(ret->chr, 1, ret->len, stdout);

    // we are done with the buffer
    KS_DECREF(ret);

    // return 'none' always
    return KSO_NONE;
}


/** OPERATORS **/


// template for defining a binary operator function
#define T_KS_FUNC_BOP(_name, _str, _fname)                 \
static KS_FUNC(_name) {                                    \
    KS_REQ_N_ARGS(n_args, 2);                              \
    if (args[0]->type->_fname != NULL)                    \
        return ks_call(args[0]->type->_fname, 2, args);   \
    KS_ERR_BOP_UNDEF(_str, args[0], args[1]);              \
}


/* __add__(L, R) -> obj
 *
 * Attempt to calculate 'L+R'
 * 
 */
T_KS_FUNC_BOP(add, "+", __add__)


/* __sub__(L, R) -> obj
 *
 * Attempt to calculate 'L-R'
 * 
 */
T_KS_FUNC_BOP(sub, "-", __sub__)


/* __mul__(L, R) -> obj
 *
 * Attempt to calculate 'L*R'
 * 
 */
T_KS_FUNC_BOP(mul, "*", __mul__)

/* __div__(L, R) -> obj
 *
 * Attempt to calculate 'L/R'
 * 
 */
T_KS_FUNC_BOP(div, "/", __div__)





// initialize all the functions
void ks_init_funcs() {
    ks_F_repr = ks_cfunc_new(repr_);
    ks_F_hash = ks_cfunc_new(hash_);
    ks_F_print = ks_cfunc_new(print_);

    ks_F_add = ks_cfunc_new(add_);
    ks_F_sub = ks_cfunc_new(sub_);
    ks_F_mul = ks_cfunc_new(mul_);
    ks_F_div = ks_cfunc_new(div_);

    ks_F_getattr = ks_cfunc_new(getattr_);
    ks_F_setattr = ks_cfunc_new(setattr_);

}


