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

// internal method for string comparison
static int s_strcmp(ks_str a, ks_str b) {
    if (a->len != b->len) return a->len - b->len;
    else return memcmp(a->chr, b->chr, a->len);
}

TFUNC(str, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");

    int res = s_strcmp(a, b);
    if (res > 0) res = 1;
    if (res < 0) res = -1;
    // just append their string representation
    return (kso)ks_int_new(res);
}


TFUNC(str, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");
    return KSO_BOOL(s_strcmp(a, b) < 0);
}
TFUNC(str, le) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");
    return KSO_BOOL(s_strcmp(a, b) <- 0);
}
TFUNC(str, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");
    return KSO_BOOL(s_strcmp(a, b) > 0);
}
TFUNC(str, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");
    return KSO_BOOL(s_strcmp(a, b) >= 0);
}
TFUNC(str, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");
    return KSO_BOOL(s_strcmp(a, b) == 0);
}
TFUNC(str, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str a = (ks_str)args[0], b = (ks_str)args[1];
    KS_REQ_TYPE(a, ks_T_str, "a");
    KS_REQ_TYPE(b, ks_T_str, "b");
    return KSO_BOOL(s_strcmp(a, b) != 0);
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


// -self, reverse self
TFUNC(str, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_T_str, "self");

    // create a temporary buffer
    char* new_str = ks_malloc(self->len);

    // reverse the string
    int i;
    for (i = 0; 2 * i < self->len; ++i) {
        new_str[i] = self->chr[self->len - i - 1];
        new_str[self->len - i - 1] = self->chr[i];
    }

    // construct a new string
    kso ret = (kso)ks_str_new_l(new_str, self->len);

    // free tmp buffer
    ks_free(new_str);

    // return generated string
    return ret;
}

// ~self, strip of whitespace
TFUNC(str, sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_T_str, "self");
    return KSO_NEWREF(self);
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

    ADDCF(ks_T_str, "__getitem__", "str.__getitem__(self, key)", str_getitem_);

    ADDCF(ks_T_str, "__add__", "str.__add__(self, B)", str_add_);

    ADDCF(ks_T_str, "__neg__", "str.__neg__(self)", str_neg_);
    ADDCF(ks_T_str, "__sqig__", "str.__sqig__(self)", str_sqig_);

    ADDCF(ks_T_str, "__cmp__", "str.__cmp__(self)", str_cmp_);
    ADDCF(ks_T_str, "__lt__", "str.__lt__(self)", str_lt_);
    ADDCF(ks_T_str, "__le__", "str.__le__(self)", str_le_);
    ADDCF(ks_T_str, "__gt__", "str.__gt__(self)", str_gt_);
    ADDCF(ks_T_str, "__ge__", "str.__ge__(self)", str_ge_);
    ADDCF(ks_T_str, "__eq__", "str.__eq__(self)", str_eq_);
    ADDCF(ks_T_str, "__ne__", "str.__ne__(self)", str_ne_);

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




