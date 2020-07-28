/* util.c - misc. utils for kscript
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// Returns a hash of given bytes, using djb2-based hashing algorithm
ks_hash_t ks_hash_bytes(const uint8_t* data, ks_size_t sz) {

    // hold our result
    ks_hash_t res = 5381;

    int i;
    // do iterations of DJB: 
    for (i = 0; i < sz; ++i) {
        res = (33 * res) + data[i];
    }

    // return out result, making sure it is never 0
    return res == 0 ? 1 : res;
}


// if it is currently freeing
static bool isFreeing = false;

// free an object
void ks_obj_free(ks_obj obj, const char* file, const char* func, int line) {
    
    assert (obj->refcnt <= 0 && "trying to free object with positive ref");
    bool wasFreeing = isFreeing;
    isFreeing = true;

    if (!wasFreeing) {
    //    ks_trace("ks", "[%s:%s:%i]: Freeing %O", file, func, line, obj);
    }

    if (obj->type->__free__ == NULL) {
        // just free memory & dereference the type,
        // assume nothing else as it wasn't provided
        KS_UNINIT_OBJ(obj);
        KS_FREE_OBJ(obj);

    } else {
        if (obj->type->__free__->type == ks_T_cfunc) {

            ks_obj res = ((ks_cfunc)obj->type->__free__)->func(1, &obj);
            if (!res) {
                ks_warn("ks", "Error freeing object %p", obj);
            }

        } else if (!ks_obj_call(obj->type->__free__, 1, &obj)) {
            // otherwise, call the function
            // there was an error in the freeing function
            ks_warn("ks", "Error freeing object %p", obj);
        }
    }

    isFreeing = wasFreeing;


}

// calculate hash
bool ks_obj_hash(ks_obj obj, ks_hash_t* out) {
    if (obj->type == ks_T_str) {
        *out = ((ks_str)obj)->v_hash;
        return true;
    } else {
        ks_throw(ks_T_TypeError, "Object of type '%T' could not be hashed!", obj);
        return false;
    }
}


// calculate equality (ignore any errors in this function!)
bool ks_obj_eq(ks_obj A, ks_obj B) {
    // TODO: implement operator overloads

    /**/ if (A->type == B->type) {
        // speed up some special cases here
        /**/ if (A->type == ks_T_str) {
            return ks_str_eq((ks_str)A, (ks_str)B);
        }
    }
    return false;
}




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
ks_obj ks_obj_call2(ks_obj func, int n_args, ks_obj* args, ks_dict locals) {
    ks_thread thread = ks_thread_get();
    assert (thread != NULL && "tried to call object, but no thread was available...");

    // create a new stack frame
    ks_stack_frame c_frame = ks_stack_frame_new(func);
    ks_list_push(thread->frames, (ks_obj)c_frame);
    KS_DECREF(c_frame);

    // the object to return
    ks_obj ret = NULL;


    if (func->type == ks_T_cfunc) {
        // let the C-function handle everything for us
        ret = ((ks_cfunc)func)->func(n_args, args);
    } else if (func->type == ks_T_kfunc) {
        // now, we need to unpack the kscript function
        // cast it to increase readability
        ks_kfunc kfc = (ks_kfunc)func;

        // either use the provided locals, or create new ones
        // This reference will be freed when the stack frame is freed
        c_frame->locals = locals ? (ks_dict)KS_NEWREF(locals) : ks_dict_new(0, NULL);
        
        // now, handle arguments

        // argument & parameter pointers
        int arg_i = 0, par_i = 0;
        bool keepGoing = true;


        while (keepGoing && arg_i < n_args && par_i < kfc->n_param) {
            
            // set current parameter
            ks_dict_set_h(c_frame->locals, (ks_obj)kfc->params[par_i].name, kfc->params[par_i].name->v_hash, args[arg_i]);

            arg_i++;
            par_i++;
        }


        // whether or not there was an error
        bool haderr = false;
        while (par_i < kfc->n_param) {

            ks_obj defa = kfc->params[par_i].defa;

            if (defa == NULL) {
                ks_throw(ks_T_ArgError, "Not enough arguments given!");
                haderr = true;
                break;
            }

            par_i++;
        }

        if (!haderr) {
            // actually perform call
            ret = ks__exec(thread, kfc->code);
        }
    } else if (func->type == ks_T_memberfunc) {
        // call `func(self, *args)`
        ks_memberfunc mfc = (ks_memberfunc)func;

        // allocate new array of argumnets
        int new_n_args = n_args + 1;
        ks_obj* new_args = ks_malloc(sizeof(*new_args) * new_n_args);

        // fill first argument
        new_args[0] = mfc->member_inst;

        // and the rest
        memcpy(&new_args[1], args, n_args * sizeof(*new_args));

        // actually perform call
        ks_obj ret = ks_obj_call(mfc->func, new_n_args, new_args);

        // free tmp args
        ks_free(new_args);

        // return result (which may be NULL)
        return ret;


    } else if (func->type == ks_T_code) {

        // execute bytecode object
        ks_code cf = (ks_code)func;

        if (n_args == 0) {
            // execute
            // either use the provided locals, or create new ones
            // This reference will be freed when the stack frame is freed
            c_frame->locals = locals ? (ks_dict)KS_NEWREF(locals) : ks_dict_new(0, NULL);

            ret = ks__exec(thread, cf);
        } else {
            // error; 
            ks_throw(ks_T_Error, "There were arguments provided to call a bytecode object, but 0 is the correct number!");

        }
    } else if (func->type == ks_T_type) {
        // see comments in `ks.h` regarding this

        // convert to the type
        ks_type ftyp = (ks_type)func;
        if (ftyp->__new__ != NULL) {

            if (ftyp->__init__ != NULL) {

                // if we have an __init__, call:
                // __new__(type)
                ret = ks_obj_call(ftyp->__new__, 1, (ks_obj[]){ (ks_obj)ftyp });

                if (ret != NULL) {

                    // set type
                    KS_INCREF(ftyp);
                    KS_DECREF(ret->type);
                    ret->type = ftyp;

                    int new_n_args = n_args + 1;
                    ks_obj* new_args = ks_malloc(sizeof(*new_args) * new_n_args);

                    // now, call:
                    // __init__(self, *args)
                    new_args[0] = ret;
                    memcpy(&new_args[1], args, n_args * sizeof(*new_args));

                    ks_obj _tmp = ks_obj_call(ftyp->__init__, new_n_args, new_args);
                    ks_free(new_args);
                    if (!_tmp) {
                        KS_DECREF(ret);
                        ret = NULL;
                    } else {
                        // the result from __init__ is not used
                        KS_DECREF(_tmp);
                    }
                } else {
                    // error in calling __new__; just keep going
                }


            } else {
                // no __init__, just call __new__ like:
                // __new__(type, *args)
                int new_n_args = n_args + 1;
                ks_obj* new_args = ks_malloc(sizeof(*new_args) * new_n_args);

                // now, call:
                new_args[0] = (ks_obj)ftyp;
                memcpy(&new_args[1], args, n_args * sizeof(*new_args));

                ret = ks_obj_call(ftyp->__new__, new_n_args, new_args);
                ks_free(new_args);

                if (ret != NULL) {
                    // set type
                    KS_INCREF(ftyp);
                    KS_DECREF(ret->type);
                    ret->type = ftyp;
                }

            }

        } else {
            ks_throw(ks_T_Error, "'%T' object was not callable! (expected there to be `%S.__new__()`, but there was none!)", func, func);
        }


    } else {
        ks_throw(ks_T_Error, "'%T' object was not callable!", func);
    }


    // take off our stack frame
    ks_list_popu(thread->frames);

    return ret;

}

// call an object
ks_obj ks_obj_call(ks_obj func, int n_args, ks_obj* args) {
    return ks_obj_call2(func, n_args, args, NULL);
}




// Return if it is callable
bool ks_obj_is_callable(ks_obj func) {
    if (func->type == ks_T_cfunc) return true;
    if (func->type == ks_T_kfunc) return true;
    if (func->type == ks_T_type) return true;

    // callable here
    if (func->type->__call__ != NULL) {
        return true;
    }

    // there is no way to call it
    return false;
}

// return if it is iterable
bool ks_obj_is_iterable(ks_obj obj) {
    return obj->type->__iter__ != NULL || obj->type->__next__ != NULL;
}

// Throw an object, return NULL 
ks_obj ks_obj_throw(ks_obj obj) {
    ks_thread th = ks_thread_get();

    if (th->exc != NULL) {
        ks_error("ks", "While handling '%S', another exception was thrown: '%S'", th->exc, obj);
        exit(-1);
        return NULL;
    } else {

        // keep the exception
        th->exc = KS_NEWREF(obj);
        th->exc_info = ks_list_new(th->frames->len, th->frames->elems);

        return NULL;
    }
}
ks_obj ks_catch(ks_list* frames) {

    ks_thread th = ks_thread_get();

    if (th->exc) {
        *frames = th->exc_info;
        ks_obj ret = th->exc;

        // reset
        th->exc = NULL;
        th->exc_info = NULL;
        return ret;
    } else {
        return ks_throw(ks_T_InternalError, "Tried to use ks_catch() when no exception was thrown!");
    }
}
// ignore any errors thrown
void ks_catch_ignore() {

    ks_thread th = ks_thread_get();

    if (th->exc) {
        KS_DECREF(th->exc);
        th->exc = NULL;
    }

    if (th->exc_info) {
        KS_DECREF(th->exc_info);
        th->exc_info = NULL;
    }

}

// Throw an object, return NULL (use ks_throw macro)
ks_obj ks_ithrow(const char* file, const char* func, int line, ks_type errtype, const char* fmt, ...) {

    va_list ap;
    va_start(ap, fmt);
    ks_str what = ks_fmt_vc(fmt, ap);
    va_end(ap);
    ks_Error newerr = ks_Error_new(errtype, what);
    KS_DECREF(what);

    ks_obj_throw((ks_obj)newerr);
    KS_DECREF(newerr);
    return NULL;
}

// quit if there is any error
void ks_exit_if_err() {
    ks_thread th = ks_thread_get();
    if (th->exc) {

        ks_list frames = NULL;
        ks_obj exc = ks_catch(&frames);

        ks_printf(COL_RED COL_BOLD "%T" COL_RESET ": %S\n", exc, exc);

        // print in reverse order
        ks_printf("Call Stack:\n");

        int i;
        for (i = 0; i < frames->len; i++) {
            ks_printf("%*c#%i: In %S\n", 2, ' ', i, frames->elems[i]);
        }
        
        ks_printf("In thread %R @ %p\n", th->name, th);

        //ks_error("ks", "Uncaught object: %S", th->exc);
        exit(-1);
    }
}

// Read a file
ks_str ks_readfile(const char* fname, const char* mode) {
    FILE* fp = fopen(fname, mode);
    if (!fp) {
        return (ks_str)ks_throw(ks_T_IOError, "Could not open '%s': %s", fname, strerror(errno));
    }

    // get size
    fseek(fp, 0, SEEK_END);
    ks_size_t sz = ftell(fp), read_sz;

    // rewind
    fseek(fp, 0, SEEK_SET);

    char* buf = ks_malloc(sz);

    if ((read_sz = fread(buf, 1, sz, fp)) != sz) {
        ks_warn("ks", "While reading file '%s', measured size did not meet actual size (read %z, but expected %z)", read_sz, sz);
    }

    fclose(fp);


    // create new string
    return ks_str_new_c(buf, sz);

}


// Return truthyness value
int ks_obj_truthy(ks_obj obj) {
    if (obj->type == ks_T_bool) {
        return obj == KSO_TRUE ? 1 : 0;
    } else if (obj->type == ks_T_none) {
        return 0;
    } else if (obj->type == ks_T_int) {
        return ks_int_sgn((ks_int)obj) != 0;
    } else if (obj->type == ks_T_float) {
        return ((ks_float)obj)->val != 0 ? 1 : 0;
    } else if (obj->type == ks_T_complex) {
        return ((ks_complex)obj)->val != 0 ? 1 : 0;
    } else if (obj->type == ks_T_str) {
        return ((ks_str)obj)->len_c > 0 ? 1 : 0;
    } else if (obj->type == ks_T_list) {
        return ((ks_list)obj)->len > 0 ? 1 : 0;
    } else if (obj->type == ks_T_tuple) {
        return ((ks_tuple)obj)->len > 0 ? 1 : 0;
    } else if (obj->type == ks_T_dict) {
        // TODO: perhaps check if any of the entries were deleted?
        return ((ks_dict)obj)->n_entries > 0 ? 1 : 0;
    } else if(obj->type->__bool__ != NULL) {
        ks_obj ret = ks_obj_call(obj->type->__bool__, 1, &obj);
        if (!ret) return -1;
        int res = ks_obj_truthy(ret);
        KS_DECREF(ret);
        return res;
    }

    ks_throw(ks_T_TypeError, "'%T' object could not be converted to bool!", obj);
    return -1;
}

