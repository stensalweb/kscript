/* kso.c - implementation of the builtin object types */

#include "ks.h"





/* generic object interface functionality */

uint64_t kso_hash(kso obj) {
    if (obj->type == ks_T_str) {
        return ((ks_str)obj)->v_hash;
    } else {
        return (uint64_t)obj;
    }
}

// try to convert A to a boolean, return 0 if it would be false, 1 if it would be true,
//   and -1 if we couldn't decide
int kso_bool(kso A) {
    if (A == KSO_TRUE) return 1;
    if (A == KSO_FALSE) return 0;
    if (A == KSO_NONE) return 0;

    if (A->type == ks_T_int) return ((ks_int)A)->v_int == 0 ? 0 : 1;

    // containers are determined by their length
    if (A->type == ks_T_str) return ((ks_str)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_tuple) return ((ks_tuple)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_list) return ((ks_list)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_dict) return ((ks_dict)A)->n_items == 0 ? 0 : 1;

    // else, we couldn't decide

    return -1;

}

// convert A to a string
ks_str kso_tostr(kso A) {

    if (A->type == ks_T_str) return (ks_str)A;
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
        }
    }

    // TODO: use their types
    return false;
}

bool kso_free(kso obj) {
    // if it can still be reached, don't free it
    if (obj->refcnt > 0) return false;
    else if (obj->refcnt < 0) ks_warn("refcnt of %R was %i", obj, obj->refcnt);

    // otherwise, free it
    ks_trace("kso_free(%o) (repr was: %R)", obj, obj);

    ks_str type_name = obj->type->name;

    // check for a type function to free
    if (obj->type->f_free != NULL) {
        if (kso_call(obj->type->f_free, 1, &obj) == NULL) {
            ks_warn("Problem encountered while freeing < '%s' obj @ %p > ", type_name->chr, obj);
        }
    } else {
        // do the default, which is to just free the object
        ks_free(obj);
    }

}

void kso_init() {

}


