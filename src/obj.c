/* obj.c - implementation of generic functionality on objects, 
 *   like string conversion, calling, hashing, etc
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// return the reprsentation of the object
ks_str ks_repr(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return (ks_str)KS_NEWREF(obj);
    }


    printf("ERR\n");
    return NULL;
}

// convert an object to a string
ks_str ks_to_str(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return (ks_str)KS_NEWREF(obj);
    }


    printf("ERR\n");
    return NULL;
}

// calculate len(obj)
int64_t ks_len(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return ((ks_str)obj)->len;
    }

    // no len attribute; error
    printf("NO LEN\n");
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

    // no len attribute; error
    printf("NO HASH\n");
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

    if (func->type == ks_type_cfunc) {
        return true;
    }


    // there is no way to call it
    return false;
}

// Return func(*args)
ks_obj ks_call(ks_obj func, int n_args, ks_obj* args) {

    if (func->type == ks_type_cfunc) {
        // just call it
        return ((ks_cfunc)func)->func(n_args, args);
    } else {
        // no way to call it, return an error
        return NULL;
    }

}
