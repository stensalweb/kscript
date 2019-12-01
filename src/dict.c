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

ks_obj ks_dict_get(ks_dict* dict, ks_str key) {
    int i;
    for (i = 0; i < dict->len; ++i) {
        if (ks_str_eq(key, dict->keys[i])) {
            return dict->vals[i];
        }
    }
    // not found
    return NULL;
}


int ks_dict_seti(ks_dict* dict, ks_str key, int idx, ks_obj val) {
    if (idx < 0) {
        // add it to the list
        idx = dict->len++;
        if (dict->len >= dict->max_len) {
            dict->max_len = (int)(dict->len * 1.5 + 10);
            dict->keys = realloc(dict->keys, sizeof(ks_str) * dict->max_len);
            dict->vals = realloc(dict->vals, sizeof(ks_obj) * dict->max_len);
        }
        // copy the key
        dict->keys[idx] = KS_STR_EMPTY;
        ks_str_copy(&dict->keys[idx], key);
    }
    // just modify it
    dict->vals[idx] = val;
    return idx;
}

int ks_dict_set(ks_dict* dict, ks_str key, ks_obj val) {
    return ks_dict_seti(dict, key, ks_dict_geti(dict, key), val);
}

void ks_dict_free(ks_dict* dict) {
    free(dict->keys);
    free(dict->vals);
    *dict = KS_DICT_EMPTY;
}


ks_dict ks_dict_fromkvp(int len, struct ks_dict_kvp* kvp) {
    ks_dict ret = KS_DICT_EMPTY;
    int i;
    for (i = 0; i < len; ++i) {
        ks_dict_set(&ret, kvp[i].key, kvp[i].val);
    }

    return ret;
}


ks_dict ks_dict_fromkvp_cp(int len, struct ks_dict_kvp_cp* kvp_cp) {
    ks_dict ret = KS_DICT_EMPTY;
    int i;
    for (i = 0; i < len; ++i) {
        ks_dict_set(&ret, KS_STR_VIEW(kvp_cp[i].key, strlen(kvp_cp[i].key)), kvp_cp[i].val);
    }

    return ret;
}
