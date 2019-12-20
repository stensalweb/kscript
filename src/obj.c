/* obj.c - constructing/managing objects */

#include "kscript.h"

kso kso_new_none() {
    return (kso)kso_V_none;
}

kso kso_new_bool(ks_bool val) {
    return (kso)(val ? kso_V_true : kso_V_false);
}

kso kso_new_int(ks_int val) {
    kso_int ret = (kso_int)ks_malloc(sizeof(*ret));
    ret->type = kso_T_int;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->_int = val;
    return (kso)ret;
}

kso kso_new_float(ks_float val) {
    kso_float ret = (kso_float)ks_malloc(sizeof(*ret));
    ret->type = kso_T_float;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->_float = val;
    return (kso)ret;
}

kso kso_new_str(ks_str val) {
    kso_str ret = (kso_str)ks_malloc(sizeof(*ret));
    ret->type = kso_T_str;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;

    ret->_str = ks_str_dup(val);

    // create string hash
    ret->_strhash = ks_hash_str(ret->_str);
    

    return (kso)ret;
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

void kso_free(kso obj) {

    // don't free an immortal
    if (obj->flags & KSOF_IMMORTAL || obj->refcnt > 0) return;

    ks_trace("ks_freeing obj %p [type %s]", obj, obj->type->name._);
    kso f_free = obj->type->f_free;

    if (f_free != NULL) {
        if (f_free->type == kso_T_cfunc) {
            KSO_CAST(kso_cfunc, f_free)->_cfunc(1, &obj);
        } else {
            ks_warn("Something other than cfunc in kso_free");
        }
    }

    ks_free(obj);
}

kso kso_call(kso func, int args_n, kso* args) {
    if (func->type == kso_T_cfunc) {
        return KSO_CAST(kso_cfunc, func)->_cfunc(args_n, args);
    } else {
        ks_err_add_str_fmt("Object of type `%s` was not callable", func->type->name._);
        return NULL;
    }
}
