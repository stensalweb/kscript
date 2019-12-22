/* obj.c - constructing/managing objects

Also, this holds some constants (like interned strings, small integers, etc)

*/

#include "kscript.h"

// if this is defined, we will intern strings, so there is only one unique string for any given string
// comment this out to disable
// TODO: Also delete strings from the global cache once they are used up. Perhaps add this to the str_free method,
//   to call a method here to `unintern` them
#define INTERN_STR 


// store 0...INT_CONST_N-1 as literals
// and to -INT_CONST_N as well
#define INT_CONST_N 256

kso kso_new_none() {
    return (kso)kso_V_none;
}

kso kso_new_bool(ks_bool val) {
    return (kso)(val ? kso_V_true : kso_V_false);
}


/* integer constants */

// store the constants from 0...255 in this array
static struct kso_int int_vals[INT_CONST_N];
// store the constants 1,-256 in this array
static struct kso_int neg_int_vals[INT_CONST_N];

kso kso_new_int(ks_int val) {
    if (val >= 0 && val < INT_CONST_N) {
        return (kso)(int_vals + val);
    } else if (val >= -INT_CONST_N && val < 0) {
        return (kso)(neg_int_vals - val - 1);
    } else {
        // construct
        kso_int ret = (kso_int)ks_malloc(sizeof(*ret));
        ret->type = kso_T_int;
        ret->flags = KSOF_NONE;
        ret->refcnt = 0;
        ret->_int = val;
        return (kso)ret;
    }
}

kso kso_new_float(ks_float val) {
    kso_float ret = (kso_float)ks_malloc(sizeof(*ret));
    ret->type = kso_T_float;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->_float = val;
    return (kso)ret;
}


/* str-interning

Essentially, we use a hash-table (dictionary) with NULL keys to store references to unique strings

If a string is duplicated, we simply increment the reference count in the dictionary

*/

#ifdef INTERN_STR
// a 'box' of interned strings
static ks_dict str_box = KS_DICT_EMPTY;
#endif

kso kso_new_str(ks_str val) {
    // always compute hash
    ks_int hash = ks_hash_str(val);
    // the return value
    kso_str ret = NULL;

    #ifdef INTERN_STR
        // use the `str_box` to intern strings
        if ((ret = (kso_str)ks_dict_get(&str_box, NULL, hash)) != NULL) {
            // we have found it, so just return it
            return (kso)ret;
        } else {
            // else, add it to the dictionary, with NULL key
            ret = (kso_str)ks_malloc(sizeof(*ret));
            ret->type = kso_T_str;
            ret->flags = KSOF_IMMORTAL;
            ret->refcnt = 1;

            ret->_str = ks_str_dup(val);
            ret->_strhash = hash;

            ks_dict_set(&str_box, NULL, hash, (kso)ret);
            
            return (kso)ret;
        }
    #else
        // not interned, so just always create a new string
        ret = (kso_str)ks_malloc(sizeof(*ret));
        ret->type = kso_T_str;
        ret->flags = KSOF_NONE;
        ret->refcnt = 0;

        ret->_str = ks_str_dup(val);
        ret->_strhash = hash;

        return (kso)ret;

    #endif
}

kso kso_new_str_cfmt(const char* fmt, ...) {
    kso_str ret = (kso_str)ks_malloc(sizeof(*ret));
    ret->type = kso_T_str;
    ret->flags = KSOF_NONE;

    ret->refcnt = 0;
    ret->_str = KS_STR_EMPTY;

    va_list ap;
    va_start(ap, fmt);
    ks_str_vcfmt(&ret->_str, fmt, ap);
    va_end(ap);

    // create string hash
    ret->_strhash = ks_hash_str(ret->_str);
    
    return (kso)ret;
}

kso kso_new_list(int n, kso* refs) {
    kso_list ret = (kso_list)ks_malloc(sizeof(*ret));
    ret->type = kso_T_list;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->_list = KS_LIST_EMPTY;

    ret->_list.len = n;
    ret->_list.max_len = n;
    ret->_list.items = ks_malloc(n * sizeof(kso));
    if (refs != NULL) {
        memcpy(ret->_list.items, refs, n * sizeof(kso));
        int i;
        for (i = 0; i < n; ++i) {
            KSO_INCREF(refs[i]);
        }
    }
    return (kso)ret;
}

kso kso_new_kfunc(ks_prog* prog, int idx) {
    kso_kfunc ret = (kso_kfunc)ks_malloc(sizeof(*ret));
    ret->type = kso_T_kfunc;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->prog = prog;
    ret->idx = idx;

    return (kso)ret;
}

kso kso_call(ks_vm* vm, kso func, int args_n, kso* args) {
    if (func->type == kso_T_cfunc) {
        return KSO_CAST(kso_cfunc, func)->_cfunc(vm, args_n, args);
    } else if (func->type == kso_T_kfunc) {
        return ks_exec(vm, KSO_CAST(kso_kfunc, func)->prog, KSO_CAST(kso_kfunc, func)->idx);
    } else {
        ks_err_add_str_fmt("Object of type `%s` was not callable", func->type->name._);
        return NULL;
    }
}

// TODO: have this method return early and just throw the pointer onto another thread which is just the GC thread
// Need to figure out multithreading first, though
bool kso_free(kso obj) {

    // don't free an immortal, or something still being referenced
    if (obj->flags & KSOF_IMMORTAL || obj->refcnt > 0) return false;

    ks_trace("ks_freeing obj %p [type %s]", obj, obj->type->name._);
    kso f_free = obj->type->f_free;

    if (f_free != NULL) {
        if (f_free->type == kso_T_cfunc) {
            KSO_CAST(kso_cfunc, f_free)->_cfunc(NULL, 1, &obj);
        } else {
            ks_warn("Something other than cfunc in kso_free");
        }
    }

    ks_free(obj);

    return true;
}

// initialize the constants
void kso_init_consts() {
    int i;
    // 0-255
    for (i = 0; i < INT_CONST_N; ++i) {
        int_vals[i] = (struct kso_int) {
            .type = kso_T_int,
            .refcnt = 1,
            .flags = KSOF_IMMORTAL,
            ._int = i
        };
    }
    for (i = -1; i >= -INT_CONST_N; --i) {
        neg_int_vals[- i - 1] = (struct kso_int) {
            .type = kso_T_int,
            .refcnt = 1,
            .flags = KSOF_IMMORTAL,
            ._int = i
        };
    }

}


