/* dict.c - dictionary implementation, based on hashtables

We use open addressing, with (by default) linear probing


*/

#include "kscript.h"

/* prime number finding, for optimal hash-table sizes */

static bool isprime(int x) {
    // true if prime
    if (x < 2) return false;
    if (x == 2 || x == 3 || x == 5) return true;
    if (x % 2 == 0 || x % 3 == 0 || x % 5 == 0) return false;

    // sqrt(x)

    // now check all odd numbers  from 7 to sqrt(x)
    int i;
    for (i = 7; i * i <= x; i += 2) {
        if (x % i == 0) return false;
    }

    return true;
}
// returns the next prime after x (not including x)
static int nextprime(int x) {
    // round up to next odd number
    int p;
    if (x % 2 == 0) p = x + 1;
    else p = x + 2;

    do {
        if (isprime(p)) return p;

        p += 2;
    } while (true);
    
    // just return it anyway
    return p;
}


// gets the object indexed by `key`. `hash` can be generated by calling `ks_hash(key)`
kso ks_dict_get(ks_dict* dict, kso key, ks_hash_t hash) {
    if (dict->n_buckets == 0) return NULL;

    // calculate the index into the items array
    int idx = hash % dict->n_buckets;

    // check that bucket, if no key even exists in that bucket, don't even search further
    if (dict->buckets[idx].val == NULL) return NULL;

    struct ks_dict_entry* entry = &dict->buckets[idx];

    int tries = 0;

    while (entry->val != NULL) {
        if (entry->hash == hash) {
            // we have found a potential match, for now assume no hash collisions
            // in the future, also use an equals method to check `entry->key==key`
            return entry->val;
        }

        // iterate through
        tries++;
        idx = (idx + 1) % dict->n_buckets;
        entry = &dict->buckets[idx];
    }

    // we haven't found anything, return NULL
    return NULL;
}

// resizes to a new number of buckets
void ks_dict_resize(ks_dict* dict, int size) {
    if (dict->n_buckets >= size) return;

    // always round up to the next prime
    size = nextprime(size);

    ks_dict new_dict = KS_DICT_EMPTY;
    new_dict.n_entries = 0;
    new_dict.n_buckets = size;
    new_dict.buckets = ks_malloc(size * sizeof(*new_dict.buckets));

    int i;

    // empty all the buckets in the new one
    for (i = 0; i < new_dict.n_buckets; ++i) {
        new_dict.buckets[i] = KS_DICT_ENTRY_EMPTY;
    }

    // resize buckets, throw them all over
    for (i = 0; i < dict->n_buckets; ++i) {
        if (dict->buckets[i].val != NULL) {
            // a used bucket
            // add to the new dictionary
            //int idx = dict->buckets[i].hash % new_dict.n_buckets;
            ks_dict_set(&new_dict, dict->buckets[i].key, dict->buckets[i].hash, dict->buckets[i].val);
        }
    }

    ks_dict_free(dict);

    // set to the new dict
    *dict = new_dict;
}


// sets the object indexed by `key`. `hash` can be generated by calling `ks_hash(key)`
void ks_dict_set(ks_dict* dict, kso key, ks_hash_t hash, kso val) {
    //printf("hash: %s,%lld\n", ((kso_str)key)->v_str._, hash);

    int req_size = 4 * dict->n_entries + 1;

    // if it has a load factor of .25, resize so it does
    if (req_size > dict->n_buckets) {
        ks_dict_resize(dict, 3 * req_size / 2 + 1);

    }

    // increment their references, since they will both be in the dictionary now
    if (key != NULL) KSO_INCREF(key);
    KSO_INCREF(val);


    // calculate the index into the items array
    int idx = hash % dict->n_buckets;

    // check that bucket, if no key even exists in that bucket, we create it here
    if (dict->buckets[idx].val == NULL) {
        dict->buckets[idx] = (struct ks_dict_entry) {
            .key = key,
            .hash = hash,
            .val = val
        };
        // we added an entry
        dict->n_entries++;
        
        return;
    }


    // else, we will need to traverse the linked list, trying to find a match
    int tries = 0;

    struct ks_dict_entry* entry = &dict->buckets[idx];
    while (entry->val != NULL) {

        if (entry->hash == hash) {
            // possible match (assuming no hash collisions), now change the value
            // remove old value and replace
            kso old_val = entry->val;
            KSO_DECREF(old_val);
            entry->val = val;

            // done
            return;
        }

        // try a new index
        tries++;

        idx = (idx + 1) % dict->n_buckets;
        entry = &dict->buckets[idx];
    }

    // if we've gone this far, we've now reached a bucket which is unused
    // initialize it
    dict->buckets[idx] = (struct ks_dict_entry) {
        .hash = hash,
        .key = key,
        .val = val
    };

    // we added an entry, so increment the count
    dict->n_entries++;
    return;
}


bool ks_dict_del(ks_dict* dict, kso key, ks_hash_t hash) {

    // calculate the index into the items array
    int idx = hash % dict->n_buckets;

    // check that bucket, if it wasn't used, then it was an invalid key
    if (dict->buckets[idx].val == NULL) {
        return false;
    }

    // else, we will need to traverse the linked list, trying to find a match
    int tries = 0;

    struct ks_dict_entry* entry = &dict->buckets[idx];
    while (entry->val != NULL) {

        if (entry->hash == hash) {
            // possible match (assuming no hash collisions), now remove the entry
            if (entry->key != NULL) KSO_DECREF(entry->key);
            KSO_DECREF(entry->val);

            *entry = KS_DICT_ENTRY_EMPTY;
            dict->n_entries--;

            return true;
        }

        // try a new index
        tries++;

        idx = (idx + 1) % dict->n_buckets;
        entry = &dict->buckets[idx];
    }

    // this means it was not a match, so it didn't exist

    return false;
}

void ks_dict_free(ks_dict* dict) {
    int i;
    for (i = 0; i < dict->n_buckets; ++i) {
        if (dict->buckets[i].val != NULL) {
            // in a used bucket, so decrement references
            if (dict->buckets[i].key != NULL) KSO_DECREF(dict->buckets[i].key);
            KSO_DECREF(dict->buckets[i].val);
        }
    }

    // free all the buckets
    ks_free(dict->buckets);

    *dict = KS_DICT_EMPTY;
}



