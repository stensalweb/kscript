/* dict.c - dictionary implementation


*/

#include "kscript.h"


void ks_dict_resize(ks_dict* dict, int new_len) {
    if (new_len > dict->max_len) {
        dict->max_len = (uint32_t)(1.25 * new_len + 10);
        dict->items = realloc(dict->items, dict->max_len * sizeof(*dict->items));
    }
}

kso ks_dict_get_byhash(ks_dict* dict, ks_int hash) {
    // linear search, in the future I will replace this with a hash-table approach
    int i;
    for (i = 0; i < dict->len; ++i) {
        if (hash == dict->items[i].hash) {
            return kso_asval(dict->items[i].val);
        }
    }

    return NULL;
}
kso ks_dict_get(ks_dict* dict, kso key) {
    return ks_dict_get_byhash(dict, ks_hash_obj(key));
}

kso ks_dict_get_str(ks_dict* dict, ks_str key) {
    return ks_dict_get_byhash(dict, ks_hash_str(key));
}

int ks_dict_set_byhash(ks_dict* dict, ks_int hash, kso key, kso val) {
    // linear search, in the future I will replace this with a hash-table approach
    int i;
    for (i = 0; i < dict->len; ++i) {
        if (hash == dict->items[i].hash) {
            dict->items[i].val = val;
            return i;
        }
    }

    // else, add it
    ks_dict_resize(dict, ++dict->len);

    i = dict->len - 1;
    dict->items[i] = (struct ks_dict_item){
        .hash = hash,
        .key = key,
        .val = val
    };

    return i;

}

int ks_dict_set(ks_dict* dict, kso key, kso val) {
    return ks_dict_set_byhash(dict, ks_hash_obj(key), key, val);
}

int ks_dict_set_str(ks_dict* dict, ks_str key, kso val) {
    return ks_dict_set_byhash(dict, ks_hash_str(key), kso_new_str(key), val);
}

void ks_dict_free(ks_dict* dict) {
    free(dict->items);

    *dict = KS_DICT_EMPTY;
}







