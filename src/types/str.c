/* types/str.c - implementation of the builtin `str` type 


*/

#include "ks_common.h"

/* str type config */

// number of chars to create single-character strings for
#define _STR_CHR_MAX 256

// list of the single character constants (empty==NULL)
static struct ks_str str_const_chr_tbl[_STR_CHR_MAX];

/* C creation routines */

/* constructs a new string from a character array and length, need not be NUL-terminated */
ks_str ks_str_new_l(const char* cstr, int len) {
    // do empty-check
    if (len <= 0 || *cstr == '\0') {
        ks_str self = &str_const_chr_tbl[0];
        KSO_INCREF(self);
        return self;
    }
    // single length check
    if (len == 1) {
        ks_str self = &str_const_chr_tbl[*cstr];
        KSO_INCREF(self);
        return self;
    }

    // actually construct it
    ks_str self = (ks_str)ks_malloc(sizeof(*self) + len);
    *self = (struct ks_str) {
        KSO_BASE_INIT(ks_T_str)
        .v_hash = ks_hash_bytes((uint8_t*)cstr, len),
        .len = len,
    };

    memcpy(self->chr, cstr, len);

    // add NUL-terminator
    self->chr[len] = '\0';

    return self;
}

/* constructs a new string from a C-style NUL-terminated string */
ks_str ks_str_new(const char* cstr) {
    return ks_str_new_l(cstr, cstr == NULL ? 0 : (int)strlen(cstr));
}


// creation via C-style formatting
ks_str ks_str_new_cfmt(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    ks_str ret = ks_str_new_vcfmt(fmt, ap);
    va_end(ap);
    return ret;
}

/* exporting functionality */


TFUNC(str, new) {
    KS_REQ_N_ARGS(n_args, 1);
    kso obj = (kso)args[0];
    if (obj->type == ks_T_str) return KSO_NEWREF(obj);
    else {
        // do tostring
        return (kso)ks_str_new_cfmt("%S", obj);
    }

}

TFUNC(str, add) {
    KS_REQ_N_ARGS(n_args, 2);

    // just append their string representation
    return (kso)ks_str_new_cfmt("%S%S", args[0], args[1]);
}

TFUNC(str, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_T_str, "self");
    ks_int key = (ks_int)args[1];
    KS_REQ_TYPE(key, ks_T_int, "key");
    
    int idx = key->v_int;
    if (idx < 0) idx += self->len;
    if (idx < 0 || idx >= self->len) return kse_fmt("KeyError: %R", key);


    return (kso)ks_str_new_l(&self->chr[key->v_int], 1);
}



struct ks_type T_str, *ks_T_str = &T_str;

void ks_init__str() {

    /* first create the type */

    T_str = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_str, "str");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    ADDCF(ks_T_str, "__new__", "str.__new__(obj)", str_new_);
    ADDCF(ks_T_str, "__add__", "str.__add__(self, B)", str_add_);
    ADDCF(ks_T_str, "__getitem__", "str.__getitem__(self, key)", str_getitem_);

    /* now create the constant single-length strings */
    int i;
    for (i = 0; i < _STR_CHR_MAX; ++i) {
        str_const_chr_tbl[i] = (struct ks_str) {
            KSO_BASE_INIT_RF(1, KSOF_IMMORTAL, ks_T_str)
            .len = i == 0 ? 0 : 1,
            .v_hash = ks_hash_bytes((uint8_t*)&i, i == 0 ? 0 : 1)
        };
        str_const_chr_tbl[i].chr[0] = (char)i;
        str_const_chr_tbl[i].chr[1] = (char)0;
    }

}




