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

ks_obj ks_obj_new_cfunc(ksf_cfunc val) {
    ks_obj ret = (ks_obj)malloc(sizeof(struct ks_obj));
    ret->type = KS_TYPE_CFUNC;
    ret->_cfunc = val;
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





