/* kso.c - implementation of the builtin object types */

#include "ks.h"


/* generic object interface functionality */

// return a 64 bit hash of an object
uint64_t kso_hash(kso A) {

    // get the type
    ks_type T_A = A->type;

    // type-punning union to convert signed to unsigned
    union {
        int64_t v_s;
        uint64_t v_u;
    } val;

    // check for integers
    if (T_A == ks_T_int) {
        val.v_s = ((ks_int)A)->v_int;
        return val.v_u;
    } else if (T_A == ks_T_str) {
        // check for strings, which store their hash
        return ((ks_str)A)->v_hash;
    } else if (T_A->f_hash != NULL) {
        //then, just call the hash function
        kso hash_res = kso_call(T_A->f_hash, 1, &A);

        if (hash_res != NULL) {
            // if not NULL, it succeeded 
            if (hash_res->type == ks_T_int) {
                // people should always use integers
                val.v_s = ((ks_int)hash_res)->v_int;
                KSO_DECREF(hash_res);
                return val.v_u;
            } else {
                kse_fmt("hash returned was not an 'int'!, got hash(%R)==%R", A, hash_res);
                KSO_DECREF(hash_res);
                return 0;
            }

        } else {
            // there was an error calling it, just return
            return 0;
        }


        return val.v_u;
    }

    // there was an error, so return -1
    return (kse_fmt("Unhashable object: %R", A), -1);
}

// try to convert A to a boolean, return 0 if it would be false, 1 if it would be true,
//   and -1 if we couldn't decide
int kso_bool(kso A) {
    // short circuit some common values
    if (A == KSO_TRUE) return 1;
    if (A == KSO_FALSE) return 0;
    if (A == KSO_NONE) return 0;

    if (A->type == ks_T_int) return ((ks_int)A)->v_int == 0 ? 0 : 1;

    // containers are determined by their length
    if (A->type == ks_T_str) return ((ks_str)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_tuple) return ((ks_tuple)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_list) return ((ks_list)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_dict) return ((ks_dict)A)->n_items == 0 ? 0 : 1;

    // TODO: call __bool__ on the object

    // else, we couldn't decide

    return -1;
}

// convert A to a string
ks_str kso_tostr(kso A) {

    // short circuit some common values
    if (A->type == ks_T_str) return (ks_str)KSO_NEWREF(A);
    if (A->type == ks_T_none) return ks_str_new("none");
    if (A->type == ks_T_bool) return ks_str_new(A == KSO_TRUE ? "true" : "false");

    ks_type T_A = A->type;
    if (T_A->f_str != NULL) {
        ks_str ret = (ks_str)kso_call(T_A->f_str, 1, &A);
        if (ret != NULL) {
            // only return if no error occured
            return ret;
        }
    }

    // else, nothing has been returned so do the default
    return ks_str_new_cfmt("<'%*s' obj @ %p>", T_A->name->len, T_A->name->chr, A);
}

// convert A to its representation
ks_str kso_torepr(kso A) {

    if (A->type == ks_T_str) return ks_str_new_cfmt("\"%*s\"", ((ks_str)A)->len, ((ks_str)A)->chr);
    if (A->type == ks_T_none || A->type == ks_T_bool || A->type == ks_T_int) return kso_tostr(A);

    ks_type T_A = A->type;
    if (T_A->f_repr != NULL) {
        ks_str ret = (ks_str)kso_call(T_A->f_repr, 1, &A);
        if (ret != NULL) {
            // only return if no error occured
            return ret;
        }
    }

    // else, nothing has been returned so do the default
    return ks_str_new_cfmt("<'%*s' obj @ %p>", T_A->name->len, T_A->name->chr, A);
}

// return whether or not the 2 objects are equal
bool kso_eq(kso A, kso B) {
    // same pointer should always be equal
    if (A == B) return true;

    if (A->type == B->type) {
        // do basic type checks
        if (A->type == ks_T_int) {
            return ((ks_int)A)->v_int == ((ks_int)B)->v_int;
        } else if (A->type == ks_T_str) {
            ks_str As = (ks_str)A, Bs = (ks_str)B;
            // check their lengths and hashes
            if (As->len != Bs->len || As->v_hash != Bs->v_hash) return false;

            // now, just strcmp
            return memcmp(As->chr, Bs->chr, As->len) == 0;
        } else if (A->type == ks_T_tuple) {
            ks_tuple At = (ks_tuple)A, Bt = (ks_tuple)B;
            if (At->len != Bt->len) return false;
            int i;
            for (i = 0; i < At->len; ++i) {
                if (!kso_eq(At->items[i], Bt->items[i])) return false;
            }

            return true;
        }
    }

    // TODO: use their type functions
    return false;
}

bool kso_free(kso obj) {
    // if it can still be reached, don't free it
    if (obj->refcnt > 0 || obj->flags & KSOF_IMMORTAL) return false;
    else if (obj->refcnt < 0) ks_warn("refcnt of %o was %i", obj, obj->refcnt);


    // now, free it

    // uncomment to trace frees
    //ks_trace("kso_free(%o) (repr was: %R)", obj, obj);

    // capture the type name before freeing it so we have it for an error message, even if something happens
    // to the object
    ks_str type_name = obj->type->name;

    // check for a type function to free
    if (obj->type->f_free != NULL) {
        if (kso_call(obj->type->f_free, 1, &obj) == NULL) {
            ks_warn("Problem encountered while freeing <'%S' obj @ %p> ", type_name, obj);
        }
    } else {
        // do the default, which is to just free the object pointer,
        // assuming no other objects
        ks_free(obj);
    }

}

// gets an attribute from the object, given a key
kso kso_getattr(kso obj, kso key) {
    // call internal function
    kso args[2] = {obj, key};
    return ks_F_getattr->v_cfunc(2, args);
}

// set an attribute given a name and value
kso kso_setattr(kso obj, kso key, kso val) {
    // call internal function
    kso args[3] = {obj, key, val};
    return ks_F_setattr->v_cfunc(3, args);
}




void kso_init() {
    // any initialization code will go here
}


