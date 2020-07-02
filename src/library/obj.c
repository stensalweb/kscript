/* obj.c - implementation of generic functionality on objects, 
 *   like string conversion, calling, hashing, etc
 *
 * Defines standard operations
 * 
 * 
 * TODO: Move all these to functions, which should be natively C-functions,
 *   that way C API will just call those C-functions ->func attribute,
 *   and behaviour will always be consistent
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// free a given object
void ks_obj_free(ks_obj obj) {
    assert(obj->refcnt <= 0);

    if (obj->type->__free__ == NULL) {
        // just free memory & dereference the type,
        // assume nothing else as it wasn't provided
        KS_UNINIT_OBJ(obj);
        KS_FREE_OBJ(obj);

    } else {
        if (obj->type->__free__->type == ks_type_cfunc) {

            ks_obj res = ((ks_cfunc)obj->type->__free__)->func(1, &obj);
            if (!res) {
                ks_warn("Error freeing object %p", obj);
            }

        } else if (!ks_call(obj->type->__free__, 1, &obj)) {
            // otherwise, call the function
            // there was an error in the freeing function
            ks_warn("Error freeing object %p", obj);
        }
    }
}

// calculate len(obj)
int64_t ks_len(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return ((ks_str)obj)->len;
    } else if (obj->type == ks_type_list) {
        return ((ks_list)obj)->len;
    }

    // no len attribute; error
    //printf("NO LEN\n");
    return -1;
}

// calculate hash(obj)
static bool my_hash(ks_obj obj, ks_hash_t* out, int dep) {
    if (dep > 4) return false;
    if (obj->type == ks_type_str) {
        *out = ((ks_str)obj)->v_hash;
        return true;
    } else if (obj->type == ks_type_bool || obj->type == ks_type_int || obj->type == ks_type_float || obj->type == ks_type_complex) {
        return ks_num_hash(obj, out);
    } else if (obj->type->__hash__ != NULL) {

        ks_obj hv = ks_call(obj->type->__hash__, 1, &obj);
        if (!hv) {
            ks_catch_ignore();
            return false;
        }

        bool rstat = my_hash(hv, out, dep+1);
        KS_DECREF(hv);
        return rstat;

    } else {
        // TODO; try ks_F_hash
        return false;
    }
}

bool ks_hash(ks_obj obj, ks_hash_t* out) {
    return my_hash(obj, out, 0);
}

// return A==B
bool ks_eq(ks_obj A, ks_obj B) {
    if (A == B) return true;

    if (A->type == ks_type_str && B->type == ks_type_str) {
        return ((ks_str)A)->v_hash == ((ks_str)B)->v_hash && (ks_str_cmp((ks_str)A, (ks_str)B) == 0);
    }

    // undefined, so return false
    return false;
}

// Return if it is callable
bool ks_is_callable(ks_obj func) {
    if (func->type == ks_type_cfunc) return true;
    if (func->type == ks_type_kfunc) return true;
    if (func->type == ks_type_type) return true;

    // callable here
    if (func->type->__call__ != NULL) {
        return true;
    }

    // there is no way to call it
    return false;
}

// return if it is iterable
bool ks_is_iterable(ks_obj obj) {
    return obj->type->__iter__ != NULL || obj->type->__next__ != NULL;
}

// Return truthyness value
int ks_truthy(ks_obj obj) {
    if (obj->type == ks_type_bool) {
        return obj == KSO_TRUE ? 1 : 0;
    } else if (obj->type == ks_type_none) {
        return 0;
    } else if (obj->type == ks_type_int) {
        return ks_int_sgn((ks_int)obj) != 0;
    } else if (obj->type == ks_type_float) {
        return ((ks_float)obj)->val != 0 ? 1 : 0;
    } else if (obj->type == ks_type_complex) {
        return ((ks_complex)obj)->val != 0 ? 1 : 0;
    } else if (obj->type == ks_type_str) {
        return ((ks_str)obj)->len > 0 ? 1 : 0;
    } else if (obj->type == ks_type_list) {
        return ((ks_list)obj)->len > 0 ? 1 : 0;
    } else if (obj->type == ks_type_tuple) {
        return ((ks_tuple)obj)->len > 0 ? 1 : 0;
    } else if (obj->type == ks_type_dict) {
        // TODO: perhaps check if any of the entries were deleted?
        return ((ks_dict)obj)->n_entries > 0 ? 1 : 0;
    } else if(obj->type->__bool__ != NULL) {
        ks_obj ret = ks_call(obj->type->__bool__, 1, &obj);
        if (!ret) return -1;
        int res = ks_truthy(ret);
        KS_DECREF(ret);
        return res;
    }

    ks_throw_fmt(ks_type_TypeError, "'%T' object could not be converted to bool!", obj);
    return -1;
}

// throw an object up the call stack, and return 'NULL'
void* ks_throw(ks_obj obj) {

    // get current thread
    ks_thread cth = ks_thread_get();

    // ensure 
    if (cth->exc != NULL) {
        ks_warn("Already object on cth->exc: %T, then obj: %T", cth->exc, obj);
    }
    assert(cth->exc == NULL && "There was already an object thrown and not caught, but someone threw something else!");

    if (obj == NULL) {
        // flush the throw status if NULL is passed
        // reset the stack
        if (cth->exc) KS_DECREF(cth->exc);
        cth->exc = NULL;
    } else {
        // add to the throw stack
        cth->exc = KS_NEWREF(obj);
    }

    // set exc_info to the tuple of stack frames
    if (cth->exc_info) KS_DECREF(cth->exc_info);
    cth->exc_info = ks_tuple_new(cth->stack_frames->len, cth->stack_frames->elems);

    // return NULL so people can return this and return NULL easily
    return NULL;
}


// throw an error type
void* ks_throw_fmt(ks_type errtype, char* fmt, ...) {
    // default error type
    if (!errtype) errtype = ks_type_Error;

    // construct the 'what' string
    va_list ap;
    va_start(ap, fmt);
    ks_str what = ks_fmt_vc(fmt, ap);
    va_end(ap);

    if (!what) {
        fprintf(stderr, RED "ERROR" RESET ": Internal Throw Error!\n");
        return NULL;
    }

    ks_obj errobj = ks_call((ks_obj)errtype, 1, (ks_obj*)&what);
    if (!errobj) {
        fprintf(stderr, RED "ERROR" RESET ": Internal Throw Error!\n");
        return NULL;
    }

    KS_DECREF(what);

    // actually throw the object
    ks_throw(errobj);
    KS_DECREF(errobj);

    return NULL;
}


// try and catch the object off
ks_obj ks_catch() {
    // get current thread
    ks_thread cth = ks_thread_get();

    ks_obj ret = cth->exc;
    cth->exc = NULL;
    
    // return the active try/catch reference
    return ret;
}

// try and catch the object off
ks_obj ks_catch2(ks_list stk_info) {

    ks_thread cth = ks_thread_get();

    ks_obj ret = cth->exc;
    cth->exc = NULL;

    // copy this
    ks_list_clear(stk_info);

    // push the result
    if (ret) {
        ks_list_pushn(stk_info, cth->exc_info->len, cth->exc_info->elems);
        KS_DECREF(cth->exc_info);
        cth->exc_info = NULL;
    }

    // return the active try/catch reference
    return ret;
}

// catch and ignore any error
void ks_catch_ignore() {
    ks_thread cth = ks_thread_get();
    assert(cth != NULL && "'ks_catch_ignore()' called outside of a thread!");

    if (cth->exc != NULL) {
        KS_DECREF(cth->exc);
        cth->exc = NULL;
    }
    if (cth->exc_info != NULL) {
        KS_DECREF(cth->exc_info);
        cth->exc_info = NULL;
    }
}
