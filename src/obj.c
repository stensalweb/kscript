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
        ks_free(obj);

    } else {
        // otherwise, call the function
        if (!ks_call(obj->type->__free__, 1, &obj)) {
            // there was an error in the freeing function
            ks_warn("Error freeing object %p", obj);
        }
    }

}


// calculate len(obj)
int64_t ks_len(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return ((ks_str)obj)->len;
    }

    // no len attribute; error
    //printf("NO LEN\n");
    return -1;
}


// calculate hash(obj)
ks_hash_t ks_hash(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return ((ks_str)obj)->v_hash;
    } else if (obj->type == ks_type_int) {
        int64_t v = ((ks_int)obj)->val;
        return (ks_hash_t)(v == 0 ? 1 : v);
    }

    // no hash, so return 0
    return 0;
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
    if (func->type == ks_type_type) return true;

    // callable here
    if (func->type->__call__ != NULL) {
        return true;
    }

    // there is no way to call it
    return false;
}



// the current object being thrown, or NULL if there is no such object
static ks_obj cur_thrown = NULL;

static ks_list cur_thrown_stk = NULL;

// throw an object up the call stack, and return 'NULL'
void* ks_throw(ks_obj obj) {
    // ensure 
    assert(cur_thrown == NULL && "There was already an object thrown and not caught, but someone threw something else!");

    if (obj == NULL) {
        // flush the throw status if NULL is passed
        // reset the stack
        if (cur_thrown) KS_DECREF(cur_thrown);
        cur_thrown = NULL;
    } else {
        // add to the throw stack
        cur_thrown = KS_NEWREF(obj);
    }

    if (!cur_thrown_stk) {
        // initialize it
        cur_thrown_stk = ks_list_new(0, NULL);
    }


    // copy it to the stack
    ks_list_clear(cur_thrown_stk);
    ks_list_pushn(cur_thrown_stk, ks_call_stk->len, ks_call_stk->elems);

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
        ks_error("Failed to format thrown error message in ks_throw_fmt()!");
        return NULL;
    }

    ks_obj errobj = ks_call((ks_obj)errtype, 1, (ks_obj*)&what);
    KS_DECREF(what);

    // actually throw the object
    ks_throw(errobj);
    KS_DECREF(errobj);

    return NULL;
}



// try and catch the object off
ks_obj ks_catch() {
    ks_obj ret = cur_thrown;
    cur_thrown = NULL;
    // return the active try/catch reference
    return ret;
}



// try and catch the object off
ks_obj ks_catch2(ks_list stk_info) {
    ks_obj ret = cur_thrown;
    cur_thrown = NULL;
    // copy this
    ks_list_clear(stk_info);

    if (ret) {
        ks_list_pushn(stk_info, cur_thrown_stk->len, cur_thrown_stk->elems);
    }
    // return the active try/catch reference
    return ret;
}


