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

