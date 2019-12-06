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



