// src.c - ks_str implementation of functionality
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

void ks_str_copy_cp(ks_str* str, char* charp, int len) {
    if (str->_ == NULL || str->max_len < len) {
        str->max_len = (int)(1.5 * len + 10);
        str->_ = realloc(str->_, str->max_len + 1);
    }
    str->len = len;
    memcpy(str->_, charp, len);
    str->_[len] = '\0';
}

void ks_str_copy(ks_str* str, ks_str from) {
    ks_str_copy_cp(str, from._, from.len);
}

ks_str ks_str_dup(ks_str from) {
    ks_str new_str = KS_STR_EMPTY;
    ks_str_copy(&new_str, from);
    return new_str;
}

void ks_str_append(ks_str* str, ks_str A) {
    int start_len = str->len;
    int new_len = str->len + A.len;
    if (str->_ == NULL || str->max_len < new_len) {
        str->max_len = (int)(1.5 * new_len + 10);
        str->_ = realloc(str->_, str->max_len + 1);
    }
    str->len = new_len;
    //memcpy(str->_, A._, A.len);
    memcpy(str->_ + start_len, A._, A.len);
    str->_[new_len] = '\0';
}


void ks_str_concat(ks_str* str, ks_str A, ks_str B) {
    int new_len = A.len + B.len;
    if (str->_ == NULL || str->max_len < new_len) {
        str->max_len = (int)(1.5 * new_len + 10);
        str->_ = realloc(str->_, str->max_len + 1);
    }
    str->len = new_len;
    memcpy(str->_, A._, A.len);
    memcpy(str->_ + A.len, B._, B.len);
    str->_[new_len] = '\0';
}

void ks_str_append_c(ks_str* str, char c) {
    str->len++;
    if (str->_ == NULL || str->max_len < str->len) {
        str->max_len = str->len;
        str->_ = realloc(str->_, str->max_len + 1);
    }
    str->_[str->len-1] = c;
    str->_[str->len] = '\0';
}

int ks_str_cmp(ks_str A, ks_str B) {
    return (A.len == B.len) ? memcmp(A._, B._, A.len) : A.len - B.len;
}

void ks_str_free(ks_str* str) {
    free(str->_);

    // reset it
    *str = KS_STR_EMPTY;
}


// implementation of some printfs
static int _ks_vasprintf(char **strp, const char *fmt, va_list ap) {
    va_list ap1;
    size_t size;
    char *buffer;

    va_copy(ap1, ap);
    size = vsnprintf(NULL, 0, fmt, ap1) + 1;
    va_end(ap1);
    buffer = calloc(1, size);

    if (!buffer)
        return -1;

    *strp = buffer;

    return vsnprintf(buffer, size, fmt, ap);
}


ks_str ks_str_vfmt(const char* fmt, va_list ap) {

    ks_str ret = KS_STR_EMPTY;

    // the formatted string
    char* rstr = NULL;
    _ks_vasprintf(&rstr, fmt, ap);

    ks_str_copy_cp(&ret, rstr, strlen(rstr));

    free(rstr);

    return ret;
}

ks_str ks_str_fmt(const char* fmt, ...) {

    ks_str ret = KS_STR_EMPTY;

    // the formatted string
    char* rstr = NULL;
    va_list ap;
    va_start(ap, fmt);
    _ks_vasprintf(&rstr, fmt, ap);
    va_end(ap);

    ks_str_copy_cp(&ret, rstr, strlen(rstr));

    free(rstr);

    return ret;
}


