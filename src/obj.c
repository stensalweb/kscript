/* obj.c - implementation of generic functionality on objects, 
 *   like string conversion, calling, hashing, etc
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// free the object
void ks_obj_free(ks_obj obj) {
    assert(obj->refcnt <= 0);


    if (obj->type->__free__ == NULL) {
        // just free memory; assume nothing else
        ks_free(obj);

    } else {
        // otherwise, call the function
        ks_info("Freeing object %s", obj->type->__name__->chr);
        if (!ks_call(obj->type->__free__, 1, &obj)) {
            // there was an error in the freeing function

            ks_warn("Error freeing object %p", obj);
        }
    }

}

// return the reprsentation of the object
ks_str ks_repr(ks_obj obj) {
    if (obj->type == ks_type_str) {

        ks_str sobj = (ks_str)obj;

        // generate a string representation
        ks_str_builder SB;
        ks_str_builder_init(&SB);

        ks_str_builder_add(&SB, 1, "'");

        int i;
        for (i = 0; i < sobj->len; ++i) {
            char c = sobj->chr[i];
            /**/ if (c == '\\') ks_str_builder_add(&SB, 2, "\\\\");
            else if (c == '\n') ks_str_builder_add(&SB, 2, "\\n");
            else if (c == '\t') ks_str_builder_add(&SB, 2, "\\t");
            else {
                // just add character
                ks_str_builder_add(&SB, 1, &c);
            }
        }

        ks_str_builder_add(&SB, 1, "'");

        ks_str ret = ks_str_builder_get(&SB);
        ks_str_builder_free(&SB);
        return ret;
    } else if (obj->type == ks_type_int) {
        // do standard formatting
        return ks_fmt_c("%l", ((ks_int)obj)->val);
    } else {
        // attempt to call type(obj).__repr__(obj)

    }

    // no known way to convert to a repr

    printf("ERR\n");
    return NULL;
}

// convert an object to a string
ks_str ks_to_str(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return (ks_str)KS_NEWREF(obj);
    } else {
        // try and get the type's function for tostring
        ks_obj T_str = obj->type->__str__;

        if (T_str != NULL) {
            // call type(obj).__str__(obj)
            return (ks_str)ks_call(T_str, 1, &obj);
        }
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



// Return obj.attr
ks_obj ks_getattr(ks_obj obj, ks_obj attr) {
    if (obj->type == ks_type_type) {
        return ks_type_get((ks_type)obj, (ks_str)attr);
    } else {
        // not found; try pfunc?
        return NULL;
    }

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

// return func.attr(*args)
ks_obj ks_call_attr(ks_obj func, ks_obj attr, int n_args, ks_obj* args) {

    ks_obj func_attr = ks_getattr(func, attr);
    if (!func_attr) return NULL;

    // call function
    ks_obj ret = ks_call(func_attr, n_args, args);
    KS_DECREF(func_attr);

    return ret;

}


