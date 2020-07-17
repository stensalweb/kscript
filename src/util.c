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

// free an object
void ks_obj_free(ks_obj obj, const char* file, const char* func, int line) {
    ks_trace("ks", "[%s:%s:%i]: Freeing %p\n", file, func, line, obj);

}

// calculate hash
bool ks_obj_hash(ks_obj obj, ks_hash_t* out) {
    if (obj->type == ks_T_str) {
        *out = ((ks_str)obj)->v_hash;
        return true;
    } else {
        printf("ERR COULD NOT HASH\n");
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

// call an object
ks_obj ks_obj_call(ks_obj func, int n_args, ks_obj* args) {

    if (func->type == ks_T_cfunc) {
        // call CFUNC
        return ((ks_cfunc)func)->func(n_args, args);
    }

    printf("CANT CALL\n");
    return NULL;
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
        return NULL;
    }
}

// ignore any errors thrown
void ks_catch_ignore() {

    ks_thread th = ks_thread_get();

    if (th->exc) {
        KS_DECREF(th->exc);
        th->exc = NULL;
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
        ks_error("ks", "Uncaught object: %S", th->exc);
        exit(-1);
    }
}

