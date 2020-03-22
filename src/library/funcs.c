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
    ks_F_exit = NULL,
    ks_F_typeof = NULL,
    ks_F_import = NULL,
    ks_F_iter = NULL,
    ks_F_next = NULL,

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
        ks_obj dur = args[0];
        if (dur->type == ks_type_int) {
            dur_d = ((ks_int)dur)->val;
        } else if (dur->type == ks_type_float) {
            dur_d = ((ks_float)dur)->val;
        } else {
            return ks_throw_fmt(ks_type_Error, "'dur' was not a numeric quantity; expected an 'int' or a 'float', but got '%T'", dur);
        }
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
        exit((int)obj->val);
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


    if (obj->type->__getitem__ != NULL) {

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
}


// global interpreter variables
static ks_dict inter_vars = NULL;

// put readline only code here:
#ifdef KS_HAVE_READLINE

// RESOURCES ON READLINE:
// http://web.mit.edu/gnu/doc/html/rlman_2.html


// attempt to tab complete match
static char**  match_gen(const char *text, int state) {
    // current index through 'inter_vars' and 'globals'
    static int idx, slen;
    char *name;

    if (!state) {
        idx = 0;
        slen = strlen(text);
    }

    while (idx < inter_vars->n_entries) {
        ks_str this_key = (ks_str)inter_vars->entries[idx++].key;
        if (!this_key) continue;
        if (this_key->type != ks_type_str) continue;

        // attempt potential match
        if (strncmp(text, this_key->chr, slen) == 0) {
            // return a new string
            char* new_match = malloc(this_key->len + 1);
            memcpy(new_match, this_key->chr, this_key->len + 1);
            return new_match;
        }
    }
    // adjusted index for globals
    #define ADJ_IDX (idx - inter_vars->n_entries)
    while (ADJ_IDX < ks_globals->n_entries) {
        ks_str this_key = (ks_str)ks_globals->entries[ADJ_IDX].key;
        idx++;
        if (!this_key) continue;
        if (this_key->type != ks_type_str) continue;

        // attempt potential match
        if (strncmp(text, this_key->chr, slen) == 0) {
            // return a new string
            char* new_match = malloc(this_key->len + 1);
            memcpy(new_match, this_key->chr, this_key->len + 1);
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
    printf("COUNT: %i\n", count);
    rl_menu_complete(count, key);
    return count + 1;
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

    //rl_bind_key('\t', rl_possible_completions);
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
        prog->kind == KS_AST_SUBSCRIPT || prog->kind == KS_AST_CALL) {
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
                ks_printf("%R\n", ret);
            }
        } 
        // discard error
        KS_DECREF(ret);
    }
    
    KS_DECREF(myc);
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


    // what should prompt every user input line?
    #define PROMPT " %> "

    // get whether its an actual terminal screen
    bool isTTY = isatty(STDIN_FILENO);

    // only use readline if it is a TTY and we are compiled with readline support
    #ifdef KS_HAVE_READLINE
    if (isTTY) {
        ensure_readline();

        // now, continue to read lines
        while ((cur_line = readline(PROMPT)) != NULL) {

            num_lines++;
            // only add non-empty
            if (cur_line && *cur_line) add_history(cur_line);
            else continue;

            // now, compile it
            ks_str src_name = ks_fmt_c("<interactive prompt #%i>", (int)num_lines);
            ks_str expr = ks_str_new(cur_line);

            run_interactive_expr(expr, src_name);
            KS_DECREF(expr);
            KS_DECREF(src_name);

            // free it. readline uses 'malloc', so we must use free
            free(cur_line);
        }

        return KSO_NONE;
    } else {
    // do fallback version
    
    #endif

    // do readline version
    if (isTTY) printf("%s", PROMPT);

    while ((len = ks_getline(&cur_line, &alloc_size, stdin)) >= 0) {

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
    }

    // end of the 'else' clause
    #ifdef KS_HAVE_READLINE
    }
    #endif

    return KSO_NONE;
}


// initialize all the functions
void ks_init_funcs() {

    ks_F_repr = ks_cfunc_new2(repr_, "repr(obj)");
    ks_F_hash = ks_cfunc_new2(hash_, "hash(obj)");
    ks_F_print = ks_cfunc_new2(print_, "print(*args)");
    ks_F_len = ks_cfunc_new2(len_, "len(obj)");
    ks_F_exit = ks_cfunc_new2(exit_, "exit(code=0)");
    ks_F_sleep = ks_cfunc_new2(sleep_, "sleep(dur=0)");
    ks_F_typeof = ks_cfunc_new2(typeof_, "typeof(obj)");
    ks_F_import = ks_cfunc_new2(import_, "import(name)");
    ks_F_iter = ks_cfunc_new2(iter_, "iter(obj)");
    ks_F_next = ks_cfunc_new2(next_, "next(obj)");

    ks_F_add = ks_cfunc_new2(add_, "__add__(L, R)");
    ks_F_sub = ks_cfunc_new2(sub_, "__sub__(L, R)");
    ks_F_mul = ks_cfunc_new2(mul_, "__mul__(L, R)");
    ks_F_div = ks_cfunc_new2(div_, "__div__(L, R)");
    ks_F_mod = ks_cfunc_new2(mod_, "__mod__(L, R)");
    ks_F_pow = ks_cfunc_new2(pow_, "__pow__(L, R)");

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


