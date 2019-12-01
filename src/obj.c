// obj.c - implementations of the ks_obj type
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//


#include "kscript.h"

ks_obj ks_obj_new_none() {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_NONE;
    return ret;
}

ks_obj ks_obj_new_int(ks_int val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_INT;
    ret->_int = val;
    return ret;
}

ks_obj ks_obj_new_bool(ks_bool val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_BOOL;
    ret->_bool = val;
    return ret;
}

ks_obj ks_obj_new_float(ks_float val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_FLOAT;
    ret->_float = val;
    return ret;
}

// NOTE: the object return has its string copied from `val`, i.e. 
//   they are not the same anymore
ks_obj ks_obj_new_str(ks_str val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_STR;
    ret->_str = KS_STR_EMPTY;
    ks_str_copy(&ret->_str, val);
    return ret;
}


ks_obj ks_obj_new_exception(ks_str message) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_EXCEPTION;
    ret->_str = KS_STR_EMPTY;
    ks_str_copy(&ret->_str, message);
    return ret;
}

ks_obj ks_obj_new_exception_fmt(const char* fmt, ...) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_EXCEPTION;
    va_list ap;
    va_start(ap, fmt);
    ret->_str = ks_str_vfmt(fmt, ap);
    //_ks_vasprintf(&rstr, fmt, ap);
    va_end(ap);
    return ret;
}

ks_obj ks_obj_new_cfunc(ksf_cfunc val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_CFUNC;
    ret->_cfunc = val;
    return ret;
}

ks_obj ks_obj_new_kfunc(ks_kfunc val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_KFUNC;
    ret->_kfunc = val;
    return ret;
}

ks_obj ks_obj_new_type() {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_TYPE;
    ret->_type = KS_DICT_EMPTY;
    return ret;
}
ks_obj ks_obj_new_type_dict(ks_dict dict) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_TYPE;
    ret->_type = dict;
    return ret;
}

ks_obj ks_obj_new_custom() {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_CUSTOM;
    ret->_dict = KS_DICT_EMPTY;
    return ret;
}

void ks_obj_free(ks_obj obj) {

    // do nothing if given NULL
    if (obj != NULL) {
        // some types (int, float) don't need to be free'd, so do nothing
        if (obj->type == KS_TYPE_STR) {
            ks_str_free(&obj->_str);
        }

        free(obj);
    }
}





