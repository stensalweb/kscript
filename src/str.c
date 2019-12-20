/* str.c - string class implementation, for length-encoded and NUL-terminated 



*/

#include "kscript.h"


// resizes a string, ks_reallocating if neccessary
void ks_str_resize(ks_str* str, uint32_t new_len) {
    if (str->_ == NULL || new_len > str->max_len) {
        str->max_len = (uint32_t)(1.25 * new_len + 5);
        str->_ = ks_realloc(str->_, str->max_len + 1);
    }
    str->len = new_len;
}


void ks_str_copy_cp(ks_str* str, char* charp, int len) {
    ks_str_resize(str, len);
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
    ks_str_resize(str, new_len);
    memcpy(str->_ + start_len, A._, A.len);
    str->_[str->len] = '\0';
}

void ks_str_concat(ks_str* str, ks_str A, ks_str B) {
    ks_str_resize(str, A.len + B.len);
    memcpy(str->_, A._, A.len);
    memcpy(str->_ + A.len, B._, B.len);
    str->_[str->len] = '\0';
}

void ks_str_append_c(ks_str* str, char c) {
    ks_str_resize(str, str->len + 1);
    str->_[str->len-1] = c;
    str->_[str->len] = '\0';
}

int ks_str_cmp(ks_str A, ks_str B) {
    return (A.len == B.len) ? memcmp(A._, B._, A.len) : A.len - B.len;
}

void ks_str_free(ks_str* str) {
    ks_free(str->_);

    // reset it
    *str = KS_STR_EMPTY;
}


int ks_str_vcfmt(ks_str* str, const char *fmt, va_list ap) {
    va_list ap1;
    va_copy(ap1, ap);
    uint32_t req_size = vsnprintf(NULL, 0, fmt, ap1) + 1;
    va_end(ap1);

    int res = 0;
    //if (req_size > str->max_len) {
        // we will need to resize, but the argument could be used in `ap`, 
        // we just manually create a new string
        //ks_str_resize(str, req_size);
        char* old__ = str->_;
        str->_ = ks_malloc(req_size + 1);
        str->max_len = req_size - 1;
        str->len = req_size - 1;
        res = vsnprintf(str->_, req_size, fmt, ap);
        ks_free(old__);
    /*} else {
        // just print as normal
        str->len = req_size - 1;
        printf("'%s'\n", str->_);
        res = vsnprintf(str->_, req_size, fmt, ap);
        printf("'%s'\n", str->_);
    }*/

    return res;
}

int ks_str_cfmt(ks_str* str, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = ks_str_vcfmt(str, fmt, ap);
    va_end(ap);
    return ret;
}

void ks_str_readfp(ks_str* str, FILE* fp) {
    long cseek = ftell(fp);
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp) - cseek;
    fseek(fp, cseek, SEEK_SET);
    ks_str_resize(str, size);
    if (fread(str->_, 1, size, fp) != size) {
        ks_warn("Reading file encountered a problem... trying to continue");
    }
    str->_[str->len] = '\0';

}

