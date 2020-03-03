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
        //ks_info("Freeing object %s", obj->type->__name__->chr);
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
        ks_str_b SB;
        ks_str_b_init(&SB);

        ks_str_b_add(&SB, 1, "'");

        int i;
        for (i = 0; i < sobj->len; ++i) {
            char c = sobj->chr[i];
            /**/ if (c == '\\') ks_str_b_add(&SB, 2, "\\\\");
            else if (c == '\n') ks_str_b_add(&SB, 2, "\\n");
            else if (c == '\t') ks_str_b_add(&SB, 2, "\\t");
            else {
                // just add character
                ks_str_b_add(&SB, 1, &c);
            }
        }

        ks_str_b_add(&SB, 1, "'");

        ks_str ret = ks_str_b_get(&SB);
        ks_str_b_free(&SB);
        return ret;
    } else if (obj->type == ks_type_int) {
        // do standard formatting
        return ks_fmt_c("%l", ((ks_int)obj)->val);
    } else if (obj->type->__repr__ != NULL) {
        // attempt to call type(obj).__repr__(obj)
        return (ks_str)ks_call(obj->type->__repr__, 1, &obj);
    } else {
        // do a default formatting
        return ks_fmt_c("<'%s' obj @ %p>", obj->type->__name__->chr, (void*)obj);
    }
}

// convert an object to a string
ks_str ks_to_str(ks_obj obj) {
    if (obj->type == ks_type_str) {
        return (ks_str)KS_NEWREF(obj);
    } else if (obj->type->__str__ != NULL) {
        // attempt to call type(obj).__str__(obj)
        return (ks_str)ks_call(obj->type->__str__, 1, &obj);
    } else {
        // do a default formatting
        return ks_fmt_c("<'%s' obj @ %p>", obj->type->__name__->chr, (void*)obj);
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
    if (func->type == ks_type_cfunc) {
        return true;
    }

    // callable here
    if (func->type->__call__ != NULL) {
        return true;
    }

    // there is no way to call it
    return false;
}



// Return obj.attr
ks_obj ks_getattr(ks_obj obj, ks_obj attr) {
    /*if (obj == ks_type_type) {
        return ks_type_get((ks_type)obj, (ks_str)attr);
    } else */
    
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
            return NULL;
        }


        // now, create a partial function
        ks_pfunc ret = ks_new_pfunc(type_attr);
        KS_DECREF(type_attr);

        // fill #0 as 'self' (aka obj)
        ks_pfunc_fill(ret, 0, obj);

        // return the member function
        return (ks_obj)ret;
    }

    printf("Noattr\n");

    // nothing found
    return NULL;

}

// Return obj.attr
ks_obj ks_getattr_c(ks_obj obj, char* attr) {
    ks_str attrstr = ks_new_str(attr);
    ks_obj ret = ks_getattr(obj, (ks_obj)attrstr);
    KS_DECREF(attrstr);
    return ret;
}


// Return func(*args)
ks_obj ks_call(ks_obj func, int n_args, ks_obj* args) {
    assert(ks_is_callable(func) && "ks_call() given a non-callable object!");

    if (func->type == ks_type_cfunc) {
        // just call it
        return ((ks_cfunc)func)->func(n_args, args);
    } else if (func->type->__call__ != NULL) {
        // call via (type(obj).__call__(obj, *args))
        // no way to call it, return an error
        ks_obj* new_args = ks_malloc(sizeof(ks_obj) * (1 + n_args));

        // set args to obj, *args
        new_args[0] = func;
        memcpy(&new_args[1], args, n_args);

        // call the internal function
        ks_obj ret = ks_call(func->type->__call__, 1 + n_args, new_args);

        ks_free(new_args);

        return ret;

    } else {

        // not callab,e return NULL
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


// the current object being thrown, or NULL if there is no such object
static ks_obj cur_thrown = NULL;

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

    // return NULL so people can return this and return NULL easily
    return NULL;
}


// try and catch the object off
ks_obj ks_catch() {
    ks_obj ret = cur_thrown;
    cur_thrown = NULL;
    // return the active try/catch reference
    return ret;
}

