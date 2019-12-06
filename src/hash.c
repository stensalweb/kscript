/* hash.c - hashing values, objects, etc 

By default, 

*/

#include "kscript.h"

// use djb2 algorithm
ks_int ks_hash_bytes(uint8_t* data, int n) {
    ks_int hash = 5381;
    ks_int c;

    while (c = *data++)
        hash = ((hash << 5) + hash) + c;
    
    return hash;
}

ks_int ks_hash_bool(ks_bool val) {
    return ks_hash_bytes((uint8_t*)&val, 1);
}

ks_int ks_hash_int(ks_int val) {
    return ks_hash_bytes((uint8_t*)&val, sizeof(val));
}

ks_int ks_hash_float(ks_float val) {
    return ks_hash_bytes((uint8_t*)&val, sizeof(val));
}

ks_int ks_hash_str(ks_str str) {
    return ks_hash_bytes((uint8_t*)str._, str.len);
}

ks_int ks_hash_obj(kso obj) {
    // first, check for any builtins
    if (obj->type == kso_T_bool) {
        return ks_hash_bool(((kso_bool)obj)->_bool);
    } else if (obj->type == kso_T_int) {
        return ks_hash_int(((kso_int)obj)->_int);
    } else if (obj->type == kso_T_float) {
        return ks_hash_float(((kso_float)obj)->_float);
    } else if (obj->type == kso_T_str) {
        return ks_hash_str(((kso_str)obj)->_str);
    } else {
        // now, check the type for a hash function
        return -1;
    }
}



