// dict.c - implementation of the `ks_dict` type
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

int ks_dict_geti(ks_dict* dict, ks_str key) {
    int i;
    for (i = 0; i < dict->len; ++i) {
        if (ks_str_eq(key, dict->keys[i])) {
            return i;
        }
    }
    // not found
    return -1;
}


int ks_dict_seti(ks_dict* dict, int idx, ks_obj val) {
    if (idx < 0) {
        // add it to the list
        idx = dict->len++;
        if (dict->len >= dict->max_len) {
            dict->max_len = (int)(dict->len * 1.5 + 10);
            dict->keys = realloc(dict->keys, sizeof(ks_str) * dict->max_len);
            dict->vals = realloc(dict->vals, sizeof(ks_obj) * dict->max_len);
        }
    }
    // just modify it
    dict->vals[idx] = val;
    return idx;
}

int ks_dict_set(ks_dict* dict, ks_str key, ks_obj val) {
    return ks_dict_seti(dict, ks_dict_geti(dict, key), val);
}

void ks_dict_free(ks_dict* dict) {
    free(dict->keys);
    free(dict->vals);
    *dict = KS_DICT_EMPTY;
}




