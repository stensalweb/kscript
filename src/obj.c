/* obj.c - constructing/managing objects */

#include "kscript.h"

kso kso_new_none() {
    kso_none ret = (kso_none)malloc(sizeof(*ret));
    ret->type = kso_T_none;
    ret->flags = 0x0;
    return (kso)ret;
}

kso kso_new_bool(ks_bool val) {
    kso_bool ret = (kso_bool)malloc(sizeof(*ret));
    ret->type = kso_T_bool;
    ret->flags = 0x0;
    ret->_bool = val;
    return (kso)ret;
}

kso kso_new_int(ks_int val) {
    kso_int ret = (kso_int)malloc(sizeof(*ret));
    ret->type = kso_T_int;
    ret->flags = 0x0;
    ret->_int = val;
    return (kso)ret;
}

kso kso_new_float(ks_float val) {
    kso_float ret = (kso_float)malloc(sizeof(*ret));
    ret->type = kso_T_float;
    ret->flags = 0x0;
    ret->_float = val;
    return (kso)ret;
}

kso kso_new_str(ks_str val) {
    kso_str ret = (kso_str)malloc(sizeof(*ret));
    ret->type = kso_T_str;
    ret->flags = 0x0;
    ret->_str = ks_str_dup(val);
    return (kso)ret;
}


kso kso_new_str_fmt(const char* fmt, ...) {
    kso_str ret = (kso_str)malloc(sizeof(*ret));
    ret->type = kso_T_str;
    ret->flags = 0x0;
    ret->_str = KS_STR_EMPTY;

    va_list ap;
    va_start(ap, fmt);
    ks_str_vfmt(&ret->_str, fmt, ap);
    va_end(ap);
    
    return (kso)ret;
}

kso kso_new_list(int n, kso* refs) {
    kso_list ret = (kso_list)malloc(sizeof(*ret));
    ret->type = kso_T_list;
    ret->flags = 0x0;
    ret->_list = KS_LIST_EMPTY;
    ret->_list.len = n;
    ret->_list.max_len = n;
    ret->_list.items = malloc(n * sizeof(kso));
    if (refs != NULL)
        memcpy(ret->_list.items, refs, n * sizeof(kso));
    return (kso)ret;
}

kso kso_asval(kso obj) {
    if (obj->type == kso_T_int) {
        return kso_new_int(KSO_CAST(kso_int, obj)->_int);
    } else {
        // return self
        return obj;
    }
}

void kso_free(kso obj) {
    kso f_free = obj->type->f_free;

    if (f_free != NULL) {
        if (f_free->type == kso_T_cfunc) {
            KSO_CAST(kso_cfunc, f_free)->_cfunc(1, &obj);
        } else {
            ks_warn("Something other than cfunc in kso_free");
        }
    }

    free(obj);
}

kso kso_call(kso func, int args_n, kso* args) {
    if (func->type == kso_T_cfunc) {
        return KSO_CAST(kso_cfunc, func)->_cfunc(args_n, args);
    } else {
        ks_err_add_str_fmt("Object of type `%s` was not callable", func->type->name._);
        return NULL;
    }
}

