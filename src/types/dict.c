/* dict.c - implementation of the dictionary type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// tuning parameters

// what is the maximum load factor we can tolerate in a dictionary?
// When a dictionary exceeds this load factor, it is resized
#define KS_DICT_MAX_LOAD 0.3

// what should the new load factor be when resizing a dictionary?
// When a dictionary is being resized/rehashed, this is the target load factor
#define KS_DICT_NEW_LOAD 0.15

// probe offset for index 'i'
#define KS_DICT_PROBE(i) (i)
// quadratic probing
//#define KS_DICT_PROBE(i) ((i)*(i))

// utility functions


// return true if 'x' is prime, false otherwise
// TODO: perhaps use Miller-Rabin tests or the like?
static bool is_prime(int x) {
    /**/ if (x < 2) return false;
    else if (x == 2 || x == 3 || x == 5) return true;
    else if (x % 2 == 0 || x % 3 == 0 || x % 5 == 0) return false;

    int i = 3;
    while (i * i <= x) {
        if (x % i == 0) return false;
        i += 2;
    }

    return true;
}


// return the next prime > x
static int next_prime(int x) {

    int i = x % 2 == 0 ? x + 1 : x + 2;

    // search for primes
    while (!is_prime(i)) {
        i += 2;
    }

    return i;
}


// Construct a new dictionary from key, val pairs (elems[0], elems[1] is the first, elems[2*i+0], elems[2*i+1] makes up the i'th pair)
// NOTE: `len%2==0` is a requirement!
// NOTE: Returns new reference, or NULL if an error was thrown
ks_dict ks_dict_new(ks_size_t len, ks_obj* elems) {
    assert (len % 2 == 0 && "dict must be created with an even number of elements");

    ks_dict self = KS_ALLOC_OBJ(ks_dict);
    KS_INIT_OBJ(self, ks_T_dict);

    // empty entries
    self->n_entries = 0;
    self->entries = NULL;

    // empty buckets
    self->n_buckets = 0;
    self->buckets = NULL;

    ks_size_t i;
    for (i = 0; i < len / 2; ++i) {
        // get key/val pair
        ks_obj key = elems[2 * i + 0], val = elems[2 * i + 1];
        ks_dict_set(self, key, val);

    }

    // return constructed dictionary
    return self;

}

// Construct a new dictionary from C-style initializers
// NOTE: Returns new reference, or NULL if an error was thrown
ks_dict ks_dict_new_c(ks_keyval_c* keyvals) {

    // create empty dict
    ks_dict self = ks_dict_new(0, NULL);

    ks_size_t i = 0;
    // iterate through keys
    while (keyvals[i].key != NULL) {
        ks_obj val = keyvals[i].val;

        if (self != NULL) {
            // we need to continue creating the dictionary
            ks_str key = ks_str_new_c(keyvals[i].key, -1);
            if (!ks_dict_set_h(self, (ks_obj)key, key->v_hash, val)) {
                // if there was an error, delete temporaries
                KS_DECREF(self);
                self = NULL;
            }

            // destroy temporary resource
            KS_DECREF(key);
        } 

        // get rid of references
        KS_DECREF(val);

        i++;
    }

    return self;
}




// calculate load factor
double ks_dict_load(ks_dict self) {
    return self->n_buckets > 0 ? (double)self->n_entries / self->n_buckets : 0.0;
}



/* INTERNAL DICT IMPLEMENTATION */


// resize a dictionary to have a new number of buckets
static void dict_resize(ks_dict self, ks_size_t new_n_buckets) {
    // check if we are alreayd large enough
    if (self->n_buckets >= new_n_buckets && self->n_buckets > 0) return;

    // ensure its a prime
    new_n_buckets = next_prime(new_n_buckets - 1);

    // loop vars
    ks_size_t i, j;

    // now, allocate new buckets
    self->n_buckets = new_n_buckets;
    self->buckets = ks_realloc(self->buckets, sizeof(*self->buckets) * self->n_buckets);
    for (i = 0; i < self->n_buckets; ++i) self->buckets[i] = KS_DICT_BUCKET_EMPTY;


    // now, go through and rehash the entries
    // NOTE: any entries that are not found will be removed
    for (i = 0; i < self->n_entries; ++i) {
        // i'th entry
        struct ks_dict_entry* ent = &self->entries[i];

        if (ent->hash == 0 || ent->val == NULL) {
            // this item has been deleted; so shift all the entries down 1 and continue
            for (j = i; j < self->n_entries - 1; ++j) {
                self->entries[j] = self->entries[j + 1];
            }

            // retry the current i (since it has been shifted over)
            i--;

            // there is now 1 less entry
            self->n_entries--;

            continue;
        }

        // bucket index (bi)
        ks_size_t bi = ent->hash % self->n_buckets;

        // keep track of original and how many tries
        ks_size_t bi0 = bi, tries = 0;

        bool found = false;

        do {
            // get the entry index (ei), which is an index into self->entries
            int ei = self->buckets[bi];

            if (ei == KS_DICT_BUCKET_EMPTY) {
                // the bucket is empty, so set it to 'i' (current entry) and continue;
                self->buckets[bi] = i;
                found = true;
                break;
            }


            tries++;

            // probing function
            bi = (bi0 + KS_DICT_PROBE(tries)) % self->n_buckets;

        } while (bi != bi0);

        assert (found && "could not resize dictionary!");
    }
}


// get a given element
ks_obj ks_dict_get_h(ks_dict self, ks_obj key, ks_hash_t hash) {
    if (self->n_buckets < 1) goto get_h_end;

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi0 = bi, tries = 0;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        /**/ if (ei == KS_DICT_BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so we can say it does not contain the given key
            return NULL;
        } else if (ei == KS_DICT_BUCKET_DELETED) {
            // do nothing; skip it
        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_obj_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key already. Now, return the value
                return KS_NEWREF(self->entries[ei].val);
            }
        }

        // try again
        tries++;

        bi = (bi0 + KS_DICT_PROBE(tries)) % self->n_buckets;

    } while (bi != bi0);


    get_h_end: ;

    // error: not in dictionary
    return NULL;
}

// Set an item in a dictionary
bool ks_dict_set_h(ks_dict self, ks_obj key, ks_hash_t hash, ks_obj val) {

    if (ks_dict_load(self) > KS_DICT_MAX_LOAD || self->n_buckets == 0) dict_resize(self, (int)(self->n_entries / KS_DICT_NEW_LOAD));

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original and how many tries
    ks_size_t bi0 = bi, tries = 0;

    bool found = false;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        if (ei == KS_DICT_BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so we can safely replace it
            ei = self->n_entries++;
            self->entries = ks_realloc(self->entries, sizeof(*self->entries) * self->n_entries);

            // set the bucket to the new location
            self->buckets[bi] = ei;

            // we are making a new entry, so we need to make new references to the key and the value
            KS_INCREF(key);
            KS_INCREF(val);
            
            // set that entry
            self->entries[ei] = (struct ks_dict_entry){ .hash = hash, .key = key, .val = val };

            return true;

        } else if (ei == KS_DICT_BUCKET_DELETED) {
            // do nothing; skip it

        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_obj_eq(self->entries[ei].key, key)) {
                // the keys are equal, so the dictionary already contains the key
                // (therefore, the dictionary already holds a reference to an equivalent key)

                // since we are replacing the value, the previous value must be dereferenced
                KS_DECREF(self->entries[ei].val);

                // add new reference
                KS_INCREF(val);
                self->entries[ei].val = val;
                
                return true;
            }
        }
        tries++;

        // probing function
        bi = (bi0 + KS_DICT_PROBE(tries)) % self->n_buckets;

    } while (bi != bi0);

    assert (found && "could not resize dictionary!");

    // success
    return true;
}

// test whether or not the dictionary has a given key
bool ks_dict_has_h(ks_dict self, ks_obj key, ks_hash_t hash) {
    if (self->n_buckets < 1) goto has_h_end;

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi0 = bi, tries = 0;

    do {

        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        /**/ if (ei == KS_DICT_BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so it is not in here
            return false;
        } else if (ei == KS_DICT_BUCKET_DELETED) {
            // do nothing; skip it
        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match

            if (self->entries[ei].key == key || ks_obj_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key
                return true;
            }
        }

        tries++;

        bi = (bi0 + KS_DICT_PROBE(tries)) % self->n_buckets;

    } while (bi != bi0);

    has_h_end: ;

    // default case, never found
    return false;
}


/* derivative methods */



// Set an item in a dictionary
ks_obj ks_dict_get(ks_dict self, ks_obj key) {
    ks_hash_t hash;
    if (!ks_obj_hash(key, &hash)) return false;

    return ks_dict_get_h(self, key, hash);
}


// Set an item in a dictionary
ks_obj ks_dict_get_c(ks_dict self, char* key) {
    ks_str key_str = ks_str_new(key);
    ks_obj ret = ks_dict_get_h(self, (ks_obj)key_str, key_str->v_hash);
    KS_DECREF(key_str);

    return ret;
}


// Set an item in a dictionary
bool ks_dict_set(ks_dict self, ks_obj key, ks_obj val) {
    ks_hash_t hash;
    if (!ks_obj_hash(key, &hash)) return false;

    return ks_dict_set_h(self, key, hash, val);
}

// Set a dictionary from C-style initializers
// NOTE: Returns new reference, or NULL if an error was thrown
bool ks_dict_set_c(ks_dict self, ks_keyval_c* keyvals) {

    // return status
    bool rst = true;

    ks_size_t i = 0;
    // iterate through keys
    while (keyvals[i].key != NULL) {
        ks_obj val = keyvals[i].val;

        if (rst) {
            // we need to continue creating the dictionary
            ks_str key = ks_str_new_c(keyvals[i].key, -1);
            if (!ks_dict_set_h(self, (ks_obj)key, key->v_hash, val)) {
                // if there was an error, stop returning true
                rst = false;
            }

            // destroy temporary resource
            KS_DECREF(key);
        } 

        // get rid of references
        KS_DECREF(val);

        i++;
    }

    return rst;
}

bool ks_dict_has(ks_dict self, ks_obj key) {
    // calc hash
    ks_hash_t hash;
    if (!ks_obj_hash(key, &hash)) {
        return false;
    }
    return ks_dict_has_h(self, key, hash);
}

bool ks_dict_has_c(ks_dict self, char* key) {
    ks_str key_str = ks_str_new(key);
    bool res = ks_dict_has_h(self, (ks_obj)key_str, key_str->v_hash);
    KS_DECREF(key_str);
    return res;
}




// dict.__free__(self) -> free obj
static KS_TFUNC(dict, free) {
    ks_dict self;
    KS_GETARGS("self:*", &self, ks_T_dict)


    ks_size_t i;

    // free references held to entries
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].key != NULL) KS_DECREF(self->entries[i].key);
        if (self->entries[i].val != NULL) KS_DECREF(self->entries[i].val);
    }

    // free allocated space
    ks_free(self->entries);
    ks_free(self->buckets);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}



// dict.__str__(self) - convert to string
static KS_TFUNC(dict, str) {
    ks_dict self;
    KS_GETARGS("self:*", &self, ks_T_dict)

    // build up a string
    ks_str_builder sb = ks_str_builder_new();

    ks_str_builder_add(sb, "{", 1);

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].hash != 0) {
            if (i > 0 && i < self->n_entries) ks_str_builder_add(sb, ", ", 2);

            // add the item
            ks_str_builder_add_repr(sb, self->entries[i].key);
            ks_str_builder_add(sb, ": ", 2);
            ks_str_builder_add_repr(sb, self->entries[i].val);
        }
    }


    ks_str_builder_add(sb, "}", 1);
    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);

    return (ks_obj)ret;
}


// dict.__len__(self) - get length
static KS_TFUNC(dict, len) {
    ks_dict self;
    KS_GETARGS("self:*", &self, ks_T_dict)
 
    // count non-null entries
    ks_ssize_t i, ct = 0;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].key != NULL) ct++;
    }

    return (ks_obj)ks_int_new(ct);
}


// dict.__getitem__(self, key) -> get an entry
static KS_TFUNC(dict, getitem) {
    ks_dict self;
    ks_obj key;
    KS_GETARGS("self:* key", &self, ks_T_dict, &key)

    ks_obj ret = ks_dict_get(self, key);
    if (!ret) {
        KS_THROW_KEY_ERR(self, key);
    } else {
        return ret;
    }
}

// dict.__setitem__(self, key, val) -> get an entry
static KS_TFUNC(dict, setitem) {
    ks_dict self;
    ks_obj key, val;
    KS_GETARGS("self:* key val", &self, ks_T_dict, &key, &val)

    if (!ks_dict_set(self, key, val)) {
        // shouldn't happen
        KS_THROW_KEY_ERR(self, key);
    } else {
        return KS_NEWREF(val);
    }
}

// dict.keys(self) - return list of keys
static KS_TFUNC(dict, keys) {
    ks_dict self;
    KS_GETARGS("self:*", &self, ks_T_dict)

    ks_list ret = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].key != NULL) {
            ks_list_push(ret, self->entries[i].key);
        }
    }

    return (ks_obj)ret;
}

// dict.vals(self) -> return a list of vals
static KS_TFUNC(dict, vals) {
    ks_dict self;
    KS_GETARGS("self:*", &self, ks_T_dict)

    ks_list ret = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].key != NULL) {
            ks_list_push(ret, self->entries[i].val);
        }
    }

    return (ks_obj)ret;
}


/* export */

KS_TYPE_DECLFWD(ks_T_dict);

void ks_init_T_dict() {
    ks_type_init_c(ks_T_dict, "dict", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(dict_free_, "dict.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c(dict_str_, "dict.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(dict_str_, "dict.__repr__(self)")},

        {"__len__",                (ks_obj)ks_cfunc_new_c(dict_len_, "dict.__len__(self)")},

        {"__getitem__",            (ks_obj)ks_cfunc_new_c(dict_getitem_, "dict.__getitem__(self, key)")},
        {"__setitem__",            (ks_obj)ks_cfunc_new_c(dict_setitem_, "dict.__setitem__(self, key, val)")},

        {"keys",                   (ks_obj)ks_cfunc_new_c(dict_keys_, "dict.keys(self)")},
        {"vals",                   (ks_obj)ks_cfunc_new_c(dict_vals_, "dict.vals(self)")},


    ));

}
