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
    if (len <= 0 || *cstr == '\0') return &str_const_chr_tbl[0];
    // single length check
    if (len == 1) return &str_const_chr_tbl[*cstr];

    // actually construct it
    ks_str self = (ks_str)ks_malloc(sizeof(*self) + len);
    *self = (struct ks_str) {
        KSO_BASE_INIT(ks_T_str, KSOF_NONE)
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

ks_str ks_str_newref(const char* cstr) {
    ks_str self = ks_str_new(cstr);
    KSO_INCREF(self);
    return self;    
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


struct ks_type T_str, *ks_T_str = &T_str;

void ks_init__str() {

    /* first create the type */
    T_str = (struct ks_type) {
        KS_TYPE_INIT("str")

    };

    /* now create the constant single-length strings */
    int i;
    for (i = 0; i < _STR_CHR_MAX; ++i) {
        str_const_chr_tbl[i] = (struct ks_str) {
            KSO_BASE_INIT_R(ks_T_str, KSOF_NONE, 1)
            .len = i == 0 ? 0 : 1,
            .v_hash = ks_hash_bytes((uint8_t*)&i, 1)
        };
        str_const_chr_tbl[i].chr[0] = (char)i;
        str_const_chr_tbl[i].chr[1] = (char)0;
    }
}



