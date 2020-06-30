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
    ks_F_len = NULL,
    ks_F_sleep = NULL,
    ks_F_time = NULL,
    ks_F_exit = NULL,
    ks_F_typeof = NULL,
    ks_F_import = NULL,
    ks_F_iter = NULL,
    ks_F_next = NULL,
    ks_F_open = NULL,
    ks_F_sort = NULL,
    ks_F_filter = NULL,
    ks_F_any = NULL,
    ks_F_all = NULL,
    ks_F_map = NULL,
    ks_F_sum = NULL,
    ks_F_range = NULL,

    ks_F_getattr = NULL,
    ks_F_setattr = NULL,

    ks_F_getitem = NULL,
    ks_F_setitem = NULL,

    ks_F_add = NULL,
    ks_F_sub = NULL,
    ks_F_mul = NULL,
    ks_F_div = NULL,
    ks_F_mod = NULL,
    ks_F_pow = NULL,

    ks_F_binor = NULL,
    ks_F_binand = NULL,
    
    ks_F_cmp = NULL,
    ks_F_lt = NULL,
    ks_F_le = NULL,
    ks_F_gt = NULL,
    ks_F_ge = NULL,
    ks_F_eq = NULL,
    ks_F_ne = NULL,

    ks_F_neg = NULL,
    ks_F_sqig = NULL,

    ks_F_run_file = NULL,
    ks_F_run_expr = NULL,
    ks_F_run_interactive = NULL

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
    return ks_call2(func, n_args, args, NULL);
}
// Added version which can accept a local variables dictionary
ks_obj ks_call2(ks_obj func, int n_args, ks_obj* args, ks_dict locals) {

    // current thread
    ks_thread this_th = ks_thread_get();
    assert(this_th != NULL && "ks_call() used outside of a thread!");


    // the result to return
    ks_obj ret = NULL;

    if (func->type == ks_type_cfunc) {
        ks_cfunc cff = (ks_cfunc)func;

        // special case to prevent recursion
        bool ignoreStackFrame = func == ks_type_stack_frame->__free__;

        // create a new stack frame
        if (!ignoreStackFrame) {

            ks_stack_frame this_stack_frame = ks_stack_frame_new(func);
            ks_list_push(this_th->stack_frames, (ks_obj)this_stack_frame);
            KS_DECREF(this_stack_frame);

        }

        ret = cff->func(n_args, args);

        if (!ignoreStackFrame) ks_list_popu(this_th->stack_frames);


    } else if (func->type == ks_type_kfunc) {
        ks_kfunc kff = (ks_kfunc)func;

        // start off empty
        ks_stack_frame this_stack_frame = ks_stack_frame_new(func);
        ks_list_push(this_th->stack_frames, (ks_obj)this_stack_frame);
        KS_DECREF(this_stack_frame);

        KS_REQ_N_ARGS(n_args, kff->n_param);

        this_stack_frame->locals = locals ? (ks_dict)KS_NEWREF(locals) : ks_dict_new(0, NULL);

        // push on arguments
        int i;
        for (i = 0; i < kff->n_param; ++i) {
            ks_dict_set(this_stack_frame->locals, kff->params[i].name->v_hash, (ks_obj)kff->params[i].name, args[i]);
        }

        // actually call it
        ret = ks__exec(kff->code);

        ks_list_popu(this_th->stack_frames);

    } else if (func->type == ks_type_code) {

        // just execute bytecode
        ks_code cf = (ks_code)func;

        ks_stack_frame this_stack_frame = ks_stack_frame_new(func);
        ks_list_push(this_th->stack_frames, (ks_obj)this_stack_frame);
        KS_DECREF(this_stack_frame);

        // never any arguments to bytecode
        KS_REQ_N_ARGS(n_args, 0);

        // start off empty
        this_stack_frame->locals = locals ? (ks_dict)KS_NEWREF(locals) : ks_dict_new(0, NULL);

        // actually call it
        ret = ks__exec(cf);

        ks_list_popu(this_th->stack_frames);


    } else if (func->type == ks_type_type) {
        // try and construct a value by calling the constructor
        // cast it
        ks_type ft = (ks_type)func;

        if (ft->__new__ != NULL) {
            // we have a constructor
            
            // now check if the type uses an initializer
            if (ft->__init__ != NULL) {
                // uses an initializer, so just call __new__ with no arguments

                ret = ks_call(ft->__new__, 0, NULL);
                if (!ret) return NULL;

                // this is to handle derived classes; we always manually set the new type
                ks_type old_type = ret->type;
                ret->type = ft;
                KS_INCREF(ret->type);
                KS_DECREF(old_type);

                // now set up our newly created object as the first argument to the initializer
                ks_obj* new_args = ks_malloc(sizeof(ks_obj) * (1 + n_args));

                new_args[0] = ret;
                // copy other arguments passed in
                memcpy(&new_args[1], args, sizeof(ks_obj) * n_args);

                // initialize it, don't care about return value because we are returning the result from 'ret'
                ks_obj dontcare = ks_call(ft->__init__, 1 + n_args, new_args);
                if (!dontcare) {
                    KS_DECREF(ret);
                    return NULL;
                }

                KS_DECREF(dontcare);

                // free temporary results
                ks_free(new_args);

            } else {
                // no initializer, so call '__new__' with all the arguments
                ret = ks_call(ft->__new__, n_args, args);
                if (!ret) return NULL;

                // this is to handle derived classes; we always manually set the new type
                ks_type old_type = ret->type;
                ret->type = ft;
                KS_INCREF(ret->type);
                KS_DECREF(old_type);
            }
        } else {
            ks_throw_fmt(ks_type_Error, "'%T' object was not callable!", func);
            ret = NULL;
        }
    } else if (func->type->__call__ != NULL) {
        // call type(obj).__call__(self, *args)
        ks_obj* new_args = ks_malloc(sizeof(ks_obj) * (1 + n_args));

        // copy in arguments
        *new_args = func;
        memcpy(&new_args[1], args, sizeof(ks_obj) * n_args);

        ret = ks_call(func->type->__call__, 1 + n_args, new_args);

        // free temporary buffer
        ks_free(new_args);

    } else {
        ks_throw_fmt(ks_type_Error, "'%T' object was not callable!", func);
        ret = NULL;
    }

    // take off result
    return ret;
}


/* len(obj) -> int
 *
 * Get the length for an object
 *
 */
static KS_FUNC(len) {
    KS_REQ_N_ARGS(n_args, 1);

    ks_obj obj = args[0];

    if (obj->type->__len__ != NULL) {

        // call type(obj).__getattr__(obj, attr)
        return ks_call(obj->type->__len__, 1, &obj);
    }

    // error
    //KS_ERR_ATTR(obj, attr);
    ks_throw_fmt(ks_type_Error, "'%T' object had no '__len__' method!", obj);
    return NULL;
}

/* typeof(obj) -> type
 *
 * Get the type of an object
 *
 */
static KS_FUNC(typeof) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];

    return KS_NEWREF(obj->type);
}


/* time() -> float
 *
 * Return the time (in seconds) since the UNIX epoch
 *
 */
static KS_FUNC(time) {
    KS_REQ_N_ARGS(n_args, 0);

    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    return (ks_obj)ks_float_new((curtime.tv_sec) + 1.0e-6 * (curtime.tv_usec));
}


/* sleep(dur=0) -> float
 *
 * Sleep for an amount of time (in seconds). Returns the real amount of time slept
 *
 */
static KS_FUNC(sleep) {
    KS_REQ_N_ARGS_MAX(n_args, 1);

    // amount to sleep
    double dur_d = 0.0;

    if (n_args == 1) {

        // get the duration
        if (!ks_num_get_double(args[0], &dur_d)) return NULL;

    }

    double s_time = ks_time();

    // allow other threads to run
    ks_GIL_unlock();

    // call the base C library function
    ks_sleep(dur_d);

    // require the lock back
    ks_GIL_lock();

    // calculate and return real time
    s_time = ks_time() - s_time;
    return (ks_obj)ks_float_new(s_time);
}


/* exit(code=0) -> none
 *
 * Exit the process 
 *
 */
static KS_FUNC(exit) {
    KS_REQ_N_ARGS_MAX(n_args, 1);
    if (n_args == 1) {
        ks_int obj = (ks_int)args[0];
        KS_REQ_TYPE(obj, ks_type_int, "code");
        if (obj->isLong) {
            ks_warn("Tried to exit with long integer... You should be using one that can fit in 64 bits");
            exit(1);
        } else {
            exit((int)obj->v_i64);
        }
    } else {
        exit(0);
    }

    // should never happen
    return NULL;
}

/* __import__(name) -> module
 *
 * Attempt to import a module, and return the module as an object
 *
 */
static KS_FUNC(import) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str name = (ks_str)args[0];
    KS_REQ_TYPE(name, ks_type_str, "name");

    // attempt to import it
    return (ks_obj)ks_module_import(name->chr);
}

/* iter(obj) -> iterable
 *
 * Turn an object into an iterable
 *
 */
static KS_FUNC(iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];
    KS_REQ_ITERABLE(obj, "obj");

    if (obj->type->__next__ != NULL) {
        // already is iterable; just return it
        return KS_NEWREF(obj);
    } else if (obj->type->__iter__ != NULL) {
        // create an iterable and return it
        return ks_call(obj->type->__iter__, 1, &obj);

    } else {
        // should never happen
        return ks_throw_fmt(ks_type_Error, "ks_is_iterable() gave a bad result!");
    }
}


/* next(obj) -> obj
 *
 * Get the next item in an iterable, or throw a 'OutOfIterError'
 *
 */
static KS_FUNC(next) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];

    if (obj->type->__next__ != NULL) {
        return ks_call(obj->type->__next__, 1, &obj);
    } else {
        return ks_throw_fmt(ks_type_Error, "'%T' object had no '__next__' method!", obj);
    }
}



/* open(fname, mode='r') -> iostream
 *
 * Attempt to open a filename with a given mode
 *
 */
static KS_FUNC(open) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    // just call constructor
    return ks_call((ks_obj)ks_type_iostream, n_args, args);
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
        if (!type_attr) KS_ERR_ATTR(obj, attr);

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



/* getitem(obj, *args) -> obj
 *
 * Try an get a value in 'obj'
 *
 */
static KS_FUNC(getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 1);

    ks_obj obj = args[0];

    if (obj->type == ks_type_type && ((ks_type)obj)->__getitem__ != NULL) {
        // if 'obj' is a type,
        // call obj.__getitem__(obj, *args)
        return ks_call(((ks_type)obj)->__getitem__, n_args, args);
    } else if (obj->type->__getitem__ != NULL) {
        // default:
        // call type(obj).__getitem__(*args)
        return ks_call(obj->type->__getitem__, n_args, args);
    }


    // error
    //KS_ERR_KEY_N(obj, n_args - 1, args + 1);
    return ks_throw_fmt(ks_type_Error, "'%T' object had no '__getitem__' method!", obj);
}





/* setitem(obj, *args, val) -> obj
 *
 * Try an set a value in 'obj'
 *
 */
static KS_FUNC(setitem) {
    KS_REQ_N_ARGS_MIN(n_args, 2);

    ks_obj obj = args[0];

    if (obj->type->__setitem__ != NULL) {

        // call type(obj).__setitem__(*args)
        return ks_call(obj->type->__setitem__, n_args, args);
    }

    // error
    return ks_throw_fmt(ks_type_Error, "'%T' object had no '__setitem__' method!", obj);
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
    } else if (obj->type->__hash__ != NULL) {
        return ks_call(obj->type->__hash__, n_args, args);
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


// number of bytes written to the stdout
static int64_t _print_numbytes = 0;


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
        if (!ks_str_b_add_str(&SB, args[i])) {
            ks_str_b_free(&SB);
            return NULL;
        }
    }

    // end with a newline
    // TODO: add seperator argument
    ks_str_b_add_c(&SB, "\n");

    // get an actual string
    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    // now print out the buffer
    fwrite(ret->chr, 1, ret->len, stdout);

    _print_numbytes += ret->len + 1;

    // we are done with the buffer
    KS_DECREF(ret);

    // return 'none' always
    return KSO_NONE;
}


/** OPERATORS **/


// template for defining a binary operator function
#define T_KS_FUNC_BOP(_name, _str, _fname, _spec)          \
static KS_FUNC(_name) {                                    \
    KS_REQ_N_ARGS(n_args, 2);                              \
    if (args[0]->type->_fname != NULL) {                   \
        ks_obj ret = NULL;                                 \
        ret = ks_call(args[0]->type->_fname, 2, args);     \
        if (ret != NULL) return ret;                       \
        ks_thread cth = ks_thread_get();                   \
        if (cth->exc && cth->exc->type == ks_type_OpError) \
            { KS_DECREF(ks_catch()); }                     \
        else return NULL;                                  \
    }                                                      \
    if (args[1]->type->_fname != NULL) {                   \
        return ks_call(args[1]->type->_fname, 2, args);    \
    }                                                      \
    { _spec; }                                             \
    KS_ERR_BOP_UNDEF(_str, args[0], args[1]);              \
}


/* __add__(L, R) -> obj
 *
 * Attempt to calculate 'L+R'
 * 
 */
T_KS_FUNC_BOP(add, "+", __add__, {})


/* __sub__(L, R) -> obj
 *
 * Attempt to calculate 'L-R'
 * 
 */
T_KS_FUNC_BOP(sub, "-", __sub__, {})


/* __mul__(L, R) -> obj
 *
 * Attempt to calculate 'L*R'
 * 
 */
T_KS_FUNC_BOP(mul, "*", __mul__, {})

/* __div__(L, R) -> obj
 *
 * Attempt to calculate 'L/R'
 * 
 */
T_KS_FUNC_BOP(div, "/", __div__, {})


/* __mod__(L, R) -> obj
 *
 * Attempt to calculate 'L%R'
 * 
 */
T_KS_FUNC_BOP(mod, "%", __mod__, {})

/* __pow__(L, R) -> obj
 *
 * Attempt to calculate 'L**R'
 * 
 */
T_KS_FUNC_BOP(pow, "**", __pow__, {})


/* __binor__(L, R) -> obj
 *
 * Attempt to calculate 'L|R'
 * 
 */
T_KS_FUNC_BOP(binor, "|", __binor__, { })



/* __binand__(L, R) -> obj
 *
 * Attempt to calculate 'L|R'
 * 
 */
T_KS_FUNC_BOP(binand, "&", __binand__, {})



/* __cmp__(L, R) -> obj
 *
 * Attempt to calculate 'L<=>R'
 * 
 */
T_KS_FUNC_BOP(cmp, "<=>", __cmp__, {})



/* __lt__(L, R) -> obj
 *
 * Attempt to calculate 'L<R'
 * 
 */
T_KS_FUNC_BOP(lt, "<", __lt__, {})

/* __le__(L, R) -> obj
 *
 * Attempt to calculate 'L<=R'
 * 
 */
T_KS_FUNC_BOP(le, "<=", __le__, {})

/* __gt__(L, R) -> obj
 *
 * Attempt to calculate 'L>R'
 * 
 */
T_KS_FUNC_BOP(gt, ">", __gt__, {})

/* __ge__(L, R) -> obj
 *
 * Attempt to calculate 'L>=R'
 * 
 */
T_KS_FUNC_BOP(ge, ">=", __ge__, {})

/* __eq__(L, R) -> obj
 *
 * Attempt to calculate 'L==R'
 * 
 */
T_KS_FUNC_BOP(eq, "==", __eq__, { return KSO_BOOL(args[0] == args[1]); })

/* __ne__(L, R) -> obj
 *
 * Attempt to calculate 'L!=R'
 * 
 */
T_KS_FUNC_BOP(ne, "!=", __ne__, { return KSO_BOOL(args[0] != args[1]); })



// template for defining a unary operator function
#define T_KS_FUNC_UOP(_name, _str, _fname)                 \
static KS_FUNC(_name) {                                    \
    KS_REQ_N_ARGS(n_args, 1);                              \
    if (args[0]->type->_fname != NULL)                    \
        return ks_call(args[0]->type->_fname, 1, args);   \
    KS_ERR_UOP_UNDEF(_str, args[0]);              \
}


/* __neg__(V) -> obj
 *
 * Attempt to calculate '-V'
 * 
 */
T_KS_FUNC_UOP(neg, "-", __neg__)

/* __sqig__(V) -> obj
 *
 * Attempt to calculate '~V'
 * 
 */
T_KS_FUNC_UOP(sqig, "~", __sqig__)




// global interpreter variables
static ks_dict inter_vars = NULL;


// what should prompt every user input line?
#define PROMPT " %> "

// put readline only code here:
#ifdef KS_HAVE_READLINE

// RESOURCES ON READLINE:
// http://web.mit.edu/gnu/doc/html/rlman_2.html


// Autocompleted globals
// NOTE: We don't want all, because that's excessive


// extra buffer size for printing
#define RLM_BUF 16


// globals that should be completed
static const char* global_matches[] = {
    "true", "false", "none", "NaN",

    "PI", "PHI", "E",

    "bool", "int", "float", "str",


    "typeof(", "hash(", "print(",
    "len(", "exit(", "repr(",

    "__import__(", "sleep(", "time("
    "iter(", "next(", "open(",

    "any(", "all(", 
    "map(", "sum(", 
    "filter(", "sort(",

    "range("

};

// number of global matches
#define N_GLOBAL_MATCHES (sizeof(global_matches) / sizeof(global_matches[0]))

// attempt to tab complete match
static char* match_gen(const char *text, int state) {
    // current index through 'inter_vars' and 'globals'
    static int idx, slen;
    // module attribute idx
    static int mod_idx;
    char *name;

    if (!state) {
        idx = 0;
        mod_idx = 0;
        slen = strlen(text);
    }

    // don't append a space
    rl_completion_append_character = '\0';

    // search through local variables
    while (idx < inter_vars->n_entries) {
        // get the object
        ks_str this_key = (ks_str)inter_vars->entries[idx].key;
        ks_obj this_val = inter_vars->entries[idx].val;

        if (!this_key || this_key->type != ks_type_str) {
            idx++;
            continue;
        }

        // handle modules
        if (this_val->type == ks_type_module && strchr(text, '.') != NULL) {
            ks_module this_mod = (ks_module)this_val;
            char* attr_pos = strchr(text, '.') + 1;
            int slen = strlen(attr_pos);

            while (mod_idx < this_mod->attr->n_entries) {
                ks_str mod_key = (ks_str)this_mod->attr->entries[mod_idx].key;
                ks_obj mod_val = this_mod->attr->entries[mod_idx].val;
                mod_idx++;
                if (!mod_key || mod_key->type != ks_type_str) continue;

                // now, compare it
                if (strncmp(attr_pos, mod_key->chr, slen) == 0) {
                    // return a new string
                    char* new_match = malloc(this_key->len + RLM_BUF + slen + 1);
                    snprintf(new_match, this_key->len + RLM_BUF, "%s.%s%s", this_key->chr, mod_key->chr, ks_is_callable(mod_val) ? "(" : "");
                    return new_match;
                }
            }
            
            mod_idx = 0;
            idx++;
            continue;
        }

        idx++;

        // attempt potential match for generic variables
        if (strncmp(text, this_key->chr, slen) == 0) {
            // return a new string
            char* new_match = malloc(this_key->len + RLM_BUF + 1);
            snprintf(new_match, this_key->len + RLM_BUF, "%s%s", this_key->chr, ks_is_callable(this_val) ? "(" : "");
            return new_match;
        }
    }

    // adjusted index for globals
    #define ADJ_IDX (idx - inter_vars->n_entries)
    while (ADJ_IDX < N_GLOBAL_MATCHES) {
        char* this_key = (char*)global_matches[ADJ_IDX];
        idx++;

        if (!this_key) continue;

        // attempt potential match
        if (strncmp(text, this_key, slen) == 0) {
            // return a new string
            char* new_match = malloc(strlen(this_key) + RLM_BUF + 1);
            snprintf(new_match, strlen(this_key) + RLM_BUF, "%s", this_key);
            return new_match;
        }
    }

    // no matches found
    return NULL;
}


// full completion function
static char** match_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    // TODO: actually be smart about what position in the text we are

    return rl_completion_matches(text, match_gen);
}


// kscript's 'tab' functionality
int my_tab_func (int count, int key) {
    //printf("COUNT: %i\n", count);
    rl_menu_complete(count, key);
    return count + 1;
}


// Handle a SIGINT
static void 
handle_sigint(int signum) {
    // do nothing
    #define MSG_SIGINT_RESET "\nUse 'CTRL-D' or 'exit()' to quit the process\n"
    fwrite(MSG_SIGINT_RESET, 1, sizeof(MSG_SIGINT_RESET) - 1, stdout);
    rl_on_new_line(); // Regenerate the prompt on a newline
    rl_replace_line("", 0); // Clear the previous text
    rl_redisplay();

    signal(SIGINT, handle_sigint);
}

// ensure readline is enabled
static void ensure_readline() {
    static bool isInit = false;
    if (isInit) return;
    isInit = true;

    // ensured

    // TODO: initialize it
    // set the tab completion to our own
    rl_attempted_completion_function = match_completion;

    // disable their signal handling
    rl_catch_signals = 0;
    rl_clear_signals();
   
    signal(SIGINT, handle_sigint);

    //rl_bind_key('\t', my_tab_func);
}

#endif



// handle exception
static void interactive_handle_exc() {
    // handle error
    ks_list exc_info = ks_list_new(0, NULL);
    ks_obj exc = ks_catch2(exc_info);
    assert(exc != NULL && "NULL returned with no exception!");

    // error, so rethrow it and return
    ks_printf(RED BOLD "%T" RESET ": %S\n", exc, exc);

    // print in reverse order
    ks_printf("Call Stack:\n");
    
    int i;
    for (i = 0; i < exc_info->len; i++) {
        ks_printf("%*c#%i: In %S\n", 2, ' ', i, exc_info->elems[i]);
    }
    
    ks_printf("In thread %R @ %p\n", ks_thread_get()->name, ks_thread_get());

    KS_DECREF(exc);
    KS_DECREF(exc_info);
}


// runs a single expression from the interactive 
static void run_interactive_expr(ks_str expr, ks_str src_name) {

    // 1. construct a parser
    ks_parser p = ks_parser_new(expr, src_name);
    if (!p) {
        interactive_handle_exc();
        return;
    }

    // 2. parse out the entire module (which will also syntax validate)
    ks_ast prog = ks_parser_parse_file(p);
    if (!prog) {
        KS_DECREF(p);
        interactive_handle_exc();
        return;
    }

    // whether ot not to print the result
    bool doPrint = false;
    // if this is true, we should only print if nothing else has printed in that time,
    //   to avoid double printing
    bool doPrintIffy = false;
    int64_t start_nbytes = _print_numbytes;

    // get whether its an actual terminal screen
    bool isTTY = isatty(STDIN_FILENO);

    // now, check if it is a binary operator or a special case,
    //   in which case 
    if (isTTY) if ((prog->kind >= KS_AST_BOP__FIRST && prog->kind <= KS_AST_BOP__LAST) ||
        (prog->kind >= KS_AST_UOP__FIRST && prog->kind <= KS_AST_UOP__LAST) ||
        prog->kind == KS_AST_ATTR || prog->kind == KS_AST_VAR || prog->kind == KS_AST_CONST ||
        prog->kind == KS_AST_SUBSCRIPT || prog->kind == KS_AST_CALL || prog->kind == KS_AST_LIST || prog->kind == KS_AST_TUPLE) {
            doPrint = true;
            doPrintIffy = prog->kind == KS_AST_CALL;
            ks_ast new_prog = ks_ast_new_ret(prog);
            KS_DECREF(prog);
            prog = new_prog;
        }

    // 3. generate a bytecode object reprsenting the module
    ks_code myc = ks_codegen(prog);
    KS_DECREF(prog);
    if (!myc) {
        KS_DECREF(p);    
        interactive_handle_exc();
        return;
    }

    // 4. (optional) attempt to set some metadata for the code, if asked
    if (myc->meta_n > 0 && myc->meta[0].tok.parser != NULL) {
        if (myc->name_hr) KS_DECREF(myc->name_hr);
        myc->name_hr = ks_fmt_c("%S", myc->meta[0].tok.parser->src_name);
    }

    // debug the code it is going to run
    ks_debug("CODE: %S", myc);

    // now, call the code object with no arguments, and return the result
    // If there is an error, it will return NULL, and the thread will call ks_errend(),
    //   which will print out a stack trace and terminate the program for us
    ks_obj ret = ks_call2((ks_obj)myc, 0, NULL, inter_vars);
    if (!ret) {
        interactive_handle_exc();

    } else {
        if (doPrint) {
            if (doPrintIffy && _print_numbytes != start_nbytes) {
                // do nothing
            } else {
                //ks_printf("%R\n", ret);
                ks_obj repr_obj = repr_(1, &ret);
                if (repr_obj) {
                    print_(1, &repr_obj);
                    KS_DECREF(repr_obj);
                } else {
                    ks_catch_ignore();
                }
            }
        } 
        // discard error
        KS_DECREF(ret);
    }
    
    KS_DECREF(myc);

}

// __std.run_file(fname) -> run and execute a file
static KS_TFUNC(std, run_file) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str fname = (ks_str)args[0];
    KS_REQ_TYPE(fname, ks_type_str, "fname");

    // 1. Attempt to read the entire file
    ks_str src_code = ks_readfile(fname->chr);
    if (!src_code) return NULL;

    // 2. construct a parser
    ks_parser p = ks_parser_new(src_code, fname);
    KS_DECREF(src_code);
    if (!p) {
        return NULL;
    }

    // 3. parse out the entire module (which will also syntax validate)
    ks_ast prog = ks_parser_parse_file(p);
    if (!prog) {
        KS_DECREF(p);
        return NULL;
    }

    // 4. generate a bytecode object reprsenting the module
    ks_code myc = ks_codegen(prog);
    KS_DECREF(prog);
    if (!myc) {
        KS_DECREF(p);    
        return NULL;
    }

    // 5. (optional) attempt to set some metadata for the code, if asked
    if (myc->meta_n > 0 && myc->meta[0].tok.parser != NULL) {
        if (myc->name_hr) KS_DECREF(myc->name_hr);
        myc->name_hr = ks_fmt_c("%S", myc->meta[0].tok.parser->src_name);
    }

    // debug the code it is going to run
    ks_debug("CODE: %S", myc);

    // now, call the code object with no arguments, and return the result
    // If there is an error, it will return NULL, and the thread will call ks_errend(),
    //   which will print out a stack trace and terminate the program for us
    return ks_call((ks_obj)myc, 0, NULL);
}

// __std.run_expr(expr) -> run and execute an expression
static KS_TFUNC(std, run_expr) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str expr = (ks_str)args[0];
    KS_REQ_TYPE(expr, ks_type_str, "expr");

    ks_str src_name = ks_fmt_c("-e %R", expr);

    run_interactive_expr(expr, src_name);
    KS_DECREF(src_name);
    return KSO_NONE;

/*
    // 2. construct a parser
    ks_parser p = ks_parser_new(expr, src_name);
    KS_DECREF(expr);

    // 3. parse out the entire module (which will also syntax validate)
    ks_ast prog = ks_parser_parse_file(p);
    if (!prog) {
        KS_DECREF(p);
        return NULL;
    }

    // 4. generate a bytecode object reprsenting the module
    ks_code myc = ks_codegen(prog);
    KS_DECREF(prog);
    if (!myc) {
        KS_DECREF(p);    
        return NULL;
    }

    // 5. (optional) attempt to set some metadata for the code, if asked
    if (myc->meta_n > 0 && myc->meta[0].tok.parser != NULL) {
        if (myc->name_hr) KS_DECREF(myc->name_hr);
        myc->name_hr = ks_fmt_c("%S", myc->meta[0].tok.parser->src_name);
    }

    // debug the code it is going to run
    ks_debug("CODE: %S", myc);

    // now, call the code object with no arguments, and return the result
    // If there is an error, it will return NULL, and the thread will call ks_errend(),
    //   which will print out a stack trace and terminate the program for us
    ks_obj ret = ks_call((ks_obj)myc, 0, NULL);

    KS_DECREF(myc);
    return ret;
    */
}


// __std.run_interactive() -> run an interactive shell
static KS_TFUNC(std, run_interactive) {
    KS_REQ_N_ARGS(n_args, 0);

    size_t alloc_size = 0;
    int len = 0;
    char* cur_line = NULL;

    // how many lines have been processed
    int num_lines = 0;

    #ifndef KS_HAVE_READLINE
    ks_warn("Using interactive interpreter, but not compiled with readline!");
    #endif

    // get whether its an actual terminal screen
    bool isTTY = isatty(STDIN_FILENO);

    // only use readline if it is a TTY and we are compiled with readline support
    #ifdef KS_HAVE_READLINE
    if (isTTY) {
        ensure_readline();

        // yield GIL
       // ks_GIL_unlock();

        // now, continue to read lines
        while ((cur_line = readline(PROMPT)) != NULL) {

            //ks_GIL_lock();

            num_lines++;
            // only add non-empty
            if (cur_line && *cur_line) add_history(cur_line);
            else continue;

            // now, compile it
            ks_str src_name = ks_fmt_c("<prompt #%i>", (int)num_lines);
            ks_str expr = ks_str_new(cur_line);

            run_interactive_expr(expr, src_name);
            KS_DECREF(expr);
            KS_DECREF(src_name);

            // free it. readline uses 'malloc', so we must use free
            free(cur_line);

           // ks_GIL_unlock();
        }
        
       // ks_GIL_lock();

    } else {
    // do fallback version
    
    #endif

    // yield GIL
    ks_GIL_unlock();

    // do readline version
    if (isTTY) printf("%s", PROMPT);

    while ((len = ks_getline(&cur_line, &alloc_size, stdin)) >= 0) {

        // hold GIL
        ks_GIL_lock();

        num_lines++;

        // remove last newline
        if (cur_line[len] == '\n') cur_line[len] = '\0';

        // short cut out if there is nothing
        if (len == 0) {
            printf("%s", PROMPT);
            continue;
        }

        // now, compile it
        ks_str src_name = ks_fmt_c("<interactive prompt #%i>", (int)num_lines);
        ks_str expr = ks_str_new(cur_line);

        run_interactive_expr(expr, src_name);
        KS_DECREF(expr);
        KS_DECREF(src_name);

        if (isTTY) printf("%s", PROMPT);

        ks_GIL_unlock();

    }

    ks_GIL_lock();

    // end of the 'else' clause
    #ifdef KS_HAVE_READLINE
    }
    #endif

    return KSO_NONE;
}


// swap 2 indices in the result/keys lists
// NOTE: Assumes result is named 'res' and keys are named 'keys'
#define SORT_SWAP(_i, _j) {               \
    int ii = (_i);                         \
    int jj = (_j);                         \
    tmp = res->elems[ii];                  \
    res->elems[ii] = res->elems[jj];       \
    res->elems[jj] = tmp;                  \
    if (keys != res) {                     \
        tmp = keys->elems[ii];             \
        keys->elems[ii] = keys->elems[jj]; \
        keys->elems[jj] = tmp;             \
    }                                      \
}


// sort 'res' based on keys, using bubble sort
// BUBBLE SORT:
// The most basic sorting algorithm, which continues to sweep the array until
//   the array is in order, swapping elements which are in the wrong order
// O(N^2) - for out of order input
// O(N) - for already sorted input
// PERF:
/*
:: sort(range(N))
1 0.000011921
8 0.00000906
64 0.00003314
512 0.000265121
4096 0.001962185
32768 0.014616966
262144 0.056243896
2097152 0.441668034

:: sort(range(N, 0, -1))
1 0.000010967
8 0.00001502
64 0.000622988
512 0.025257826
4096 1.322135925
... DID NOT FINISH


*/
static bool my_sort_bubble(ks_list res, ks_list keys) {

    // temporary object used for swapping
    ks_obj tmp;

    // cmp(L, R)
    ks_obj cLR;

    // calculate where it should be correct from
    int base_i = 0;

    // and cap the number of tries
    int tries = 0;

    // iterator
    int i;

    // whether or not anything has changed
    bool hasChanged = true;

    while (hasChanged && tries <= res->len) {
        // keep track of whether anything has been changed
        hasChanged = false;

        for (i = base_i; i < res->len - 1; ++i) {
            // do L <=> R
            cLR = cmp_(2, &keys->elems[i]);
            if (!cLR) return false;

            // Ensure an integer was given
            if (cLR->type != ks_type_int) {
                ks_throw_fmt(ks_type_ArgError, "Invalid result from '__cmp__', got type '%T'", cLR);
                KS_DECREF(cLR);
                return false;
            }

            // calculate the difference and free that object
            int diff = ks_int_sgn((ks_int)cLR);
            KS_DECREF(cLR);

            // if they are out of order, swap them so that they are
            if (diff > 0) {
                hasChanged = true;
                SORT_SWAP(i, i + 1);
            } else if (!hasChanged) {
                // up the minimum sorted place
                base_i = i;
            }
        }
    }
    
    // no error
    return true;
}


// merge subsets of the array
static bool my_sort_merge__child(ks_list res, ks_list keys, int start_l, int stop_l, int start_r, int stop_r) {
    // left and right positions
    int lp = start_l;
    int rp = start_r;

    int total_num = stop_r - start_l;
    // where the results go
    int n_o = 0;
    ks_obj* reso = ks_malloc(sizeof(*reso) * total_num);
    ks_obj* keyso = ks_malloc(sizeof(*keyso) * total_num);

    // keep going
    while (lp < stop_l && rp < stop_r) {
        // compare
        ks_int cLR = (ks_int)cmp_(2, (ks_obj[]){ keys->elems[lp], keys->elems[rp] });
        if (!cLR) {
            ks_free(reso);
            ks_free(keyso);
            return false;
        }

        // Ensure an integer was given
        if (cLR->type != ks_type_int) {
            ks_throw_fmt(ks_type_ArgError, "Invalid result from '__cmp__', got type '%T'", cLR);
            ks_free(reso);
            ks_free(keyso);
            KS_DECREF(cLR);
            return false;
        }

        // get comparison value
        int64_t cmp = ks_int_sgn(cLR);
        KS_DECREF(cLR);

        // push the lowest object
        n_o++;

        // now, advance the merge
        if (cmp <= 0) {
            // already sorted correctly
            reso[n_o - 1] = res->elems[lp];
            keyso[n_o - 1] = keys->elems[lp];
            lp++;
        } else {
            reso[n_o - 1] = res->elems[rp];
            keyso[n_o - 1] = keys->elems[rp];
            rp++;
        }
    }

    // now, copy over the rest which are in sorted order
    while (lp < stop_l) {
        n_o++;
        reso[n_o - 1] = res->elems[lp];
        keyso[n_o - 1] = keys->elems[lp];
        lp++;
    }

    // and from the right hand
    while (rp < stop_r) {
        n_o++;
        reso[n_o - 1] = res->elems[rp];
        keyso[n_o - 1] = keys->elems[rp];
        rp++;
    }

    // copy over sorted subset
    int res_i = start_l, reso_i = 0;
    while (reso_i < n_o) {
        res->elems[res_i] = reso[reso_i];
        keys->elems[res_i] = keyso[reso_i];
        reso_i++;
        res_i++;
    }

    ks_free(reso);
    ks_free(keyso);

    return true;
}

// sort 'res' based on 'keys' according to merge sort, returning success
// NOTE: O(NlogN) performance
// PERF NUMS:
/*

:: sort(range(N))
1 0.000007868
8 0.000010967
64 0.000052214
512 0.000505924
4096 0.004536152
32768 0.028655052
262144 0.247076988
2097152 2.21036315

:: sort(range(N, 0, -1))
1 0.000010967
8 0.000011921
64 0.000066996
512 0.000627041
4096 0.006075144
32768 0.028308868
262144 0.242085934
2097152 2.167088032

*/
static bool my_sort_merge(ks_list res, ks_list keys, int start, int stop) {
    if (start < stop - 1) {
        int mid = start + (stop - start) / 2;

        // recursively sort
        if (!my_sort_merge(res, keys, start, mid)) return false;
        if (!my_sort_merge(res, keys, mid, stop)) return false;

        // merge them together
        if (!my_sort_merge__child(res, keys, start, mid, mid, stop)) return false;
    }

    return true;
}

/* sort(objs, func=none) -> []
 *
 * Return a list of objects, sorted by their result of a function (default: identity function)
 */
static KS_FUNC(sort) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj objs = args[0];
    KS_REQ_ITERABLE(objs, "objs");


    // collect into a list
    ks_list res = ks_list_from_iterable(objs);
    if (!res) return NULL;

    // the keys to sort by (default==objs)
    ks_list keys = NULL;

    // check for a function
    if (n_args > 1 && args[1] != KSO_NONE) {
        // there was a transformation function that should be applied
        ks_obj func = args[1];

        // construct a new array
        keys = ks_list_new(0, NULL);
        
        // apply function to each element
        int i;
        for (i = 0; i < res->len; ++i) {
            ks_obj this_obj = ks_call(func, 1, &res->elems[i]);
            if (!this_obj) {
                KS_DECREF(keys);
                KS_DECREF(res);
                return NULL;
            }

            ks_list_push(keys, this_obj);
            KS_DECREF(this_obj);
        }

    } else {
        // nothing supplied, so just use the identity function (make a new reference to the input keys)
        keys = (ks_list)KS_NEWREF(res);
    }

    // TODO: switch between bubble/qsort/etc
    //bool status = my_sort_bubble(res, keys);
    // merge sort is the obvious choice; it is quite fast
    bool status = my_sort_merge(res, keys, 0, res->len);

    // done with the keys array
    KS_DECREF(keys);

    // check for errors
    if (!status) {
        KS_DECREF(res);
        return NULL;
    }

    // returned sorted list
    return (ks_obj)res;
}



/* filter(objs, func=none) -> []
 *
 * Return a list of objects for which a function returns true (default: identity)
 */
static KS_FUNC(filter) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj objs = args[0];
    KS_REQ_ITERABLE(objs, "objs");

    // the function, or NULL if there was none
    ks_obj func = NULL;

    // get the function from the argument
    if (n_args > 1 && args[1] != KSO_NONE) {
        func = args[1];
    }

    // convert to an iterator
    ks_obj objs_iter = iter_(1, &objs);
    if (!objs_iter) return NULL;

    // the results which pass the filter
    ks_list res = ks_list_new(0, NULL);

    // traverse the iterator
    while (true) {
        ks_obj next_obj = next_(1, &objs_iter);
        if (!next_obj) {
            if (ks_thread_get()->exc->type == ks_type_OutOfIterError) {
                // we have hit the end, so stop executing
                ks_catch_ignore();
                break;
            }

            // error has occured
            KS_DECREF(objs_iter);
            KS_DECREF(res);
            return NULL;
        }
    
        if (func) {
            // apply a function
            ks_obj filt_val = ks_call(func, 1, &next_obj);
            if (!filt_val) {
                // error
                KS_DECREF(next_obj);
                KS_DECREF(objs_iter);
                KS_DECREF(res);
                return NULL;
            }

            // now, see if it is truthy
            int truthy = ks_truthy(filt_val);
            KS_DECREF(filt_val);

            if (truthy < 0) {
                KS_DECREF(next_obj);
                KS_DECREF(objs_iter);
                KS_DECREF(res);
                return NULL;
            } else if (truthy) {
                ks_list_push(res, next_obj);
            }

        } else {
            // just use the object itself
            int truthy = ks_truthy(next_obj);
            if (truthy < 0) {
                KS_DECREF(next_obj);
                KS_DECREF(objs_iter);
                KS_DECREF(res);
                return NULL;
            } else if (truthy) {
                ks_list_push(res, next_obj);
            }
        }

        // we are done using this object
        KS_DECREF(next_obj);

    }

    // done with the iterator
    KS_DECREF(objs_iter);

    return (ks_obj)res;
}


/* map(func, objs) -> []
 *
 * Return a list of objects, which is the function applied to them
 */
static KS_FUNC(map) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj func = args[0];
    KS_REQ_CALLABLE(func, "func");
    ks_obj objs = args[1];
    KS_REQ_ITERABLE(objs, "objs");

    // convert to an iterable
    ks_obj iter_objs = iter_(1, &objs);
    if (!iter_objs) return NULL;

    // construct the results
    ks_list res = ks_list_new(0, NULL);

    // now, iterate through and map each object
    while (true) {
        ks_obj next_obj = next_(1, &iter_objs);
        if (!next_obj) {
            // try and handle iterator
            if (ks_thread_get()->exc->type == ks_type_OutOfIterError) {
                // stop iterating, and ignore the error
                ks_catch_ignore();
                break;
            }

            // there was a legitimate error
            KS_DECREF(res);
            KS_DECREF(iter_objs);
            return NULL;
        }

        ks_obj out = ks_call(func, 1, &next_obj);
        KS_DECREF(next_obj);

        if (!out) {
            KS_DECREF(res);
            KS_DECREF(iter_objs);
            return NULL;
        }

        // add to results
        ks_list_push(res, out);
        KS_DECREF(out);

    }

    // done with iterator
    KS_DECREF(iter_objs);

    return (ks_obj)res;
}



/* any(objs) -> bool
 *
 * Return whether any object in 'objs' evaluates to truthy
 * 
 */
static KS_FUNC(any) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj objs = args[0];
    KS_REQ_ITERABLE(objs, "objs");

    // convert to an iterable
    ks_obj iter_objs = iter_(1, &objs);
    if (!iter_objs) return NULL;

    // now, iterate through and map each object
    while (true) {
        ks_obj next_obj = next_(1, &iter_objs);
        if (!next_obj) {
            // try and handle iterator
            if (ks_thread_get()->exc->type == ks_type_OutOfIterError) {
                // stop iterating, and ignore the error,
                ks_catch_ignore();

                break;
            }

            // there was a legitimate error
            KS_DECREF(iter_objs);
            return NULL;
        }

        // get whether the next object was truthy
        int tres = ks_truthy(next_obj);
        KS_DECREF(next_obj);

        // handle error
        if (tres < 0) {
            KS_DECREF(iter_objs);
            return NULL;
        }

        // if it was truthy, return true since one was found
        if (tres) {
            KS_DECREF(iter_objs);
            return KSO_TRUE;
        }

    }

    // done with iterator
    KS_DECREF(iter_objs);

    // none were truthy
    return KSO_FALSE;
}



/* all(objs) -> bool
 *
 * Return whether all objects in 'objs' evaluates to truthy
 * 
 */
static KS_FUNC(all) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj objs = args[0];
    KS_REQ_ITERABLE(objs, "objs");

    // convert to an iterable
    ks_obj iter_objs = iter_(1, &objs);
    if (!iter_objs) return NULL;

    // now, iterate through and map each object
    while (true) {
        ks_obj next_obj = next_(1, &iter_objs);
        if (!next_obj) {
            // try and handle iterator
            if (ks_thread_get()->exc->type == ks_type_OutOfIterError) {
                // stop iterating, and ignore the error,
                ks_catch_ignore();

                break;
            }

            // there was a legitimate error
            KS_DECREF(iter_objs);
            return NULL;
        }

        // get whether the next object was truthy
        int tres = ks_truthy(next_obj);
        KS_DECREF(next_obj);

        // handle error
        if (tres < 0) {
            KS_DECREF(iter_objs);
            return NULL;
        }

        // if it wasn't truthy, then all weren't so return false
        if (!tres) {
            KS_DECREF(iter_objs);
            return KSO_FALSE;
        }

    }

    // done with iterator
    KS_DECREF(iter_objs);

    // none were truthy
    return KSO_TRUE;
}



/* sum(objs, initial=none) -> []
 *
 * Return the sum of a list of objects. If `initial` is given, then begin the summation with that number
 * Otherwise, begin with `objs[0]`
 * 
 */
static KS_FUNC(sum) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj objs = args[0];
    KS_REQ_ITERABLE(objs, "objs");

    // calculate the result
    ks_obj res = NULL;
    if (n_args >= 2) res = KS_NEWREF(args[1]);

    // convert to an iterable
    ks_obj iter_objs = iter_(1, &objs);
    if (!iter_objs) return NULL;

    // whether or not the first item for the result has been gotten
    // (only false if there was not an initial object given)
    bool hasGrabbedFirst = res != NULL;

    // now, iterate through and map each object
    while (true) {
        ks_obj next_obj = next_(1, &iter_objs);
        if (!next_obj) {
            // try and handle iterator
            if (ks_thread_get()->exc->type == ks_type_OutOfIterError) {
                // stop iterating, and ignore the error,
                ks_catch_ignore();
                KS_DECREF(iter_objs);

                // if an object has been grabbed, return that, otherwise return nothing
                return hasGrabbedFirst ? res : KSO_NONE;
            }

            // there was a legitimate error
            if (res) KS_DECREF(res);
            KS_DECREF(iter_objs);
            return NULL;
        }

        if (!hasGrabbedFirst) {
            // no object has been generated yet, so just take the first as initial
            res = next_obj;
            hasGrabbedFirst = true;
        } else {
            // otherwise, add it
            ks_obj out = ks_call((ks_obj)ks_F_add, 2, (ks_obj[]){res, next_obj});
            KS_DECREF(next_obj);
            KS_DECREF(res);

            // check for errors
            if (!out) {
                KS_DECREF(iter_objs);
                return NULL;
            }

            // assign the result to the new output
            res = out;

        }
    }

    // done with iterator
    KS_DECREF(iter_objs);

    return (ks_obj)res;
}



// ks_range_iter - the result given by 'range'
typedef struct {
    KS_OBJ_BASE

    // start, stop, and step of the range
    ks_int start, stop, step;

    // current value
    ks_int cur;

}* ks_range_iter;


// range iter type
KS_TYPE_DECLFWD(ks_type_range_iter);


// range_iter.__next__(self) -> return the next object, or throw a 'OutOfIterError'
static KS_TFUNC(range_iter, next) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_range_iter self = (ks_range_iter)args[0];
    KS_REQ_TYPE(self, ks_type_range_iter, "self");



    // check out ranges & their relations to make sure we are in bounds
    int sgn = ks_int_sgn(self->step);
    int cmp = ks_int_cmp(self->cur, self->stop);


    if (sgn > 0 && cmp >= 0) return ks_throw_fmt(ks_type_OutOfIterError, "");
    if (sgn < 0 && cmp <= 0) return ks_throw_fmt(ks_type_OutOfIterError, "");

    // create a new cur
    ks_int new_cur = (ks_int)ks_F_add->func(2, (ks_obj[]){ (ks_obj)self->cur, (ks_obj)self->step });
    if (!new_cur) {
        KS_DECREF(new_cur);
        return NULL;
    }

    // what we should return as the result
    // (and, transfer the reference to the return reference, since self->cur is about to be overriden)
    ks_int res = self->cur;
    self->cur = new_cur;

    // return our result
    return (ks_obj)res;
}


/* range(stop)
 * range(start, stop, step=1)
 */
static KS_FUNC(range) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 3);
    KS_REQ_TYPE(args[0], ks_type_int, (n_args == 1 ? "stop" : "start"));
    if (n_args > 1) {
        KS_REQ_TYPE(args[1], ks_type_int, "stop");
    }
    if (n_args > 2) {
        KS_REQ_TYPE(args[2], ks_type_int, "step");
    }

    // construct it
    ks_range_iter res = KS_ALLOC_OBJ(ks_range_iter);
    KS_INIT_OBJ(res, ks_type_range_iter);

    if (n_args == 1) {
        res->start = ks_int_new(0);
        res->step = ks_int_new(1);
        res->stop = (ks_int)KS_NEWREF(args[0]);
    } else {
        res->start = (ks_int)KS_NEWREF(args[0]);
        res->stop = (ks_int)KS_NEWREF(args[1]);
        if (n_args > 2) {
            res->step = (ks_int)KS_NEWREF(args[2]);
            if (ks_int_sgn(res->step) == 0) {
                ks_throw_fmt(ks_type_ArgError, "'step' is supposed to be non-zero!");
                KS_DECREF(res);
                return NULL;
            }
        }
    }

    // start at beginning
    res->cur = (ks_int)KS_NEWREF(res->start);

    return (ks_obj)res;
}


// initialize all the functions
void ks_init_funcs() {

    // range_iter type
    KS_INIT_TYPE_OBJ(ks_type_range_iter, "range_iter");
    ks_type_set_cn(ks_type_range_iter, (ks_dict_ent_c[]){
        {"__next__", (ks_obj)ks_cfunc_new2(range_iter_next_, "range_iter.__next__(self)")},

        {NULL, NULL}   
    });


    ks_F_repr = ks_cfunc_new2(repr_, "repr(obj)");
    ks_F_hash = ks_cfunc_new2(hash_, "hash(obj)");
    ks_F_print = ks_cfunc_new2(print_, "print(*args)");
    ks_F_len = ks_cfunc_new2(len_, "len(obj)");
    ks_F_exit = ks_cfunc_new2(exit_, "exit(code=0)");
    ks_F_sleep = ks_cfunc_new2(sleep_, "sleep(dur=0)");
    ks_F_time = ks_cfunc_new2(time_, "time()");
    ks_F_typeof = ks_cfunc_new2(typeof_, "typeof(obj)");
    ks_F_import = ks_cfunc_new2(import_, "import(name)");
    ks_F_iter = ks_cfunc_new2(iter_, "iter(obj)");
    ks_F_next = ks_cfunc_new2(next_, "next(obj)");
    ks_F_open = ks_cfunc_new2(open_, "open(fname, mode='r')");
    ks_F_sort = ks_cfunc_new2(sort_, "sort(objs, func=none)");
    ks_F_filter = ks_cfunc_new2(filter_, "filter(objs, func=none)");

    ks_F_any = ks_cfunc_new2(any_, "any(objs)");
    ks_F_all = ks_cfunc_new2(all_, "all(objs)");

    ks_F_map = ks_cfunc_new2(map_, "map(func, objs)");
    ks_F_sum = ks_cfunc_new2(sum_, "sum(objs, initial=none)");

    ks_F_range = ks_cfunc_new2(range_, "range(start_or_stop, stop, step=1)");

    ks_F_add = ks_cfunc_new2(add_, "__add__(L, R)");
    ks_F_sub = ks_cfunc_new2(sub_, "__sub__(L, R)");
    ks_F_mul = ks_cfunc_new2(mul_, "__mul__(L, R)");
    ks_F_div = ks_cfunc_new2(div_, "__div__(L, R)");
    ks_F_mod = ks_cfunc_new2(mod_, "__mod__(L, R)");
    ks_F_pow = ks_cfunc_new2(pow_, "__pow__(L, R)");

    ks_F_binor = ks_cfunc_new2(binor_, "__binor__(L, R)");
    ks_F_binand = ks_cfunc_new2(binand_, "__binand__(L, R)");

    ks_F_cmp = ks_cfunc_new2(cmp_, "__cmp__(L, R)");
    ks_F_lt = ks_cfunc_new2(lt_, "__lt__(L, R)");
    ks_F_le = ks_cfunc_new2(le_, "__le__(L, R)");
    ks_F_gt = ks_cfunc_new2(gt_, "__gt__(L, R)");
    ks_F_ge = ks_cfunc_new2(ge_, "__ge__(L, R)");
    ks_F_eq = ks_cfunc_new2(eq_, "__eq__(L, R)");
    ks_F_ne = ks_cfunc_new2(ne_, "__ne__(L, R)");

    ks_F_neg = ks_cfunc_new2(neg_, "__neg__(V)");
    ks_F_sqig = ks_cfunc_new2(sqig_, "__sqig__(V)");

    ks_F_getattr = ks_cfunc_new2(getattr_, "getattr(obj, attr)");
    ks_F_setattr = ks_cfunc_new2(setattr_, "setattr(obj, attr, val)");

    ks_F_getitem = ks_cfunc_new2(getitem_, "getitem(obj, *vals)");
    ks_F_setitem = ks_cfunc_new2(setitem_, "setitem(obj, *args)");

    ks_F_run_file = ks_cfunc_new2(std_run_file_, "run_file(fname)");
    ks_F_run_expr = ks_cfunc_new2(std_run_expr_, "run_expr(expr)");
    ks_F_run_interactive = ks_cfunc_new2(std_run_interactive_, "run_interactive()");

    inter_vars = ks_dict_new(0, NULL);

}


