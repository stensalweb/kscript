/* hash.c - hashing values, objects, etc 

By default, 

*/

#include "kscript.h"

// use djb2 algorithm
ks_hash_t ks_hash_bytes(uint8_t* data, int n) {
    ks_hash_t hash = 5381;

    int i;
    for (i = 0; i < n; ++i) {
        hash = ((hash << 5) + hash) + data[i];
    }

    return hash;
}

ks_hash_t ks_hash_str(ks_str str) {
    return ks_hash_bytes((uint8_t*)str._, str.len);
}


ks_int ks_hash_obj(kso obj) {
    if (obj->type == kso_T_str) {
        return ((kso_str)obj)->v_hash;
    } else {
        return ((ks_int)obj);
    }
}
