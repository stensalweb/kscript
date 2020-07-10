/* types/dict.c - kscript's basic dictionary implementation
 *
 * TODO:
 *   * Perhaps support multiple probing functions (or at least configurable at build time)
 *   * Change load factor / readjustment criteria
 * 
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


/* CONSTANTS */

// The value for a bucket if that bucket is 'empty'
#define BUCKET_EMPTY (int)(-1)

// The value for a bucket if that bucket was at one point filled, but has since been deleted
#define BUCKET_DELETED (int)(-2)


// the maximum load a dictionary should have; anything above this is resized
#define DICT_MAX_LOAD 0.4f


// forward declare it
KS_TYPE_DECLFWD(ks_type_dict);
KS_TYPE_DECLFWD(ks_type_dict_iter);


// return true if 'x' is prime, false otherwise
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

// create a kscript dictionary from entries
ks_dict ks_dict_new(int len, ks_obj* entries) {
    ks_dict self = KS_ALLOC_OBJ(ks_dict);
    KS_INIT_OBJ(self, ks_type_dict);
    assert(len % 2 == 0 && "Given odd multiple!");

    // always start with no entries
    self->n_entries = 0;
    self->entries = NULL;

    // calculate a good size of buckets for the length
    self->n_buckets = next_prime(4 * len + 5);
    self->buckets = ks_malloc(sizeof(*self->buckets) * self->n_buckets);
    
    // now, set all buckets to empty
    int i;
    for (i = 0; i < self->n_buckets; ++i) self->buckets[i] = BUCKET_EMPTY;

    // now, set the (key, val) pairs of 'entries' into the dictionary
    for (i = 0; i < len / 2; ++i) {
        ks_dict_set(self, entries[2 * i], entries[2 * i + 1]);
    }

    return self;
}

void ks_dict_merge(ks_dict self, ks_dict src) {
    int i;
    for (i = 0; i < src->n_entries; ++i) {
        // ensure it wasn't deleted
        struct ks_dict_entry* ent = &src->entries[i];
        if (ent->hash != 0 && ent->val != NULL) {
            // set it in 'self'
            ks_dict_set_h(self, ent->key, ent->hash, ent->val);
        }
    }
}


// construct a new dictionary from C-style entries, but do not create new references for them
ks_dict ks_dict_new_cn(ks_dict_ent_c* ent_cns) {

    ks_dict self = KS_ALLOC_OBJ(ks_dict);
    KS_INIT_OBJ(self, ks_type_dict);

    // always start with no entries
    self->n_entries = 0;
    self->entries = NULL;

    // calculate a good size of buckets for the length
    self->n_buckets = 5;
    self->buckets = ks_malloc(sizeof(*self->buckets) * self->n_buckets);
    
    // now, set all buckets to empty
    int i;
    for (i = 0; i < self->n_buckets; ++i) self->buckets[i] = BUCKET_EMPTY;

    // now, set the (key, val) pairs of 'entries' into the dictionary

    while (ent_cns->key != NULL) {
        // keep iterating t hrough entries
        if (!ks_dict_set_c(self, ent_cns->key, ent_cns->val)) {
            KS_DECREF(self);
            return NULL;
        }

        // since there will be a reference added, we remove it here
        KS_DECREF(ent_cns->val);

        // next entry
        ent_cns++;
    }

    return self;

}

// Sets a list of C-entries (without creating new references)
bool ks_dict_set_cn(ks_dict self, ks_dict_ent_c* ent_cns) {
    bool rstat = true;
    while (rstat && ent_cns->key != NULL) {

        // keep iterating t hrough entries
        if (!ks_dict_set_c(self, ent_cns->key, ent_cns->val)) {
            rstat = false;
        }

        // since there will be a reference added, we remove it here
        KS_DECREF(ent_cns->val);

        // next entry
        ent_cns++;
    }


    while (ent_cns->key != NULL) {

        KS_DECREF(ent_cns->val);
        ent_cns++;

    }

    return rstat;
}

/* ACCESS UTILS */

// resize a dictionary to have a new number of buckets
static void dict_resize(ks_dict self, ks_size_t new_n_buckets) {

    // check if we are alreayd large enough
    if (self->n_buckets >= new_n_buckets) return;

    // ensure its a prime
    new_n_buckets = next_prime(new_n_buckets - 1);

    int i;

    // now, allocate new buckets
    self->n_buckets = new_n_buckets;
    self->buckets = ks_realloc(self->buckets, sizeof(*self->buckets) * self->n_buckets);
    for (i = 0; i < self->n_buckets; ++i) self->buckets[i] = BUCKET_EMPTY;


    // now, go through and rehash the entries
    for (i = 0; i < self->n_entries; ++i) {
        // i'th entry
        struct ks_dict_entry* ent = self->entries + i;

        if (ent->hash == 0 || ent->val == NULL) {
            // this item has been deleted; so shift all the entries down 1 and continue
            int j;
            for (j = i; j < self->n_entries - 1; ++j) {
                self->entries[j] = self->entries[j + 1];
            }

            // retry the current i
            i--;
            // there is now 1 less entry
            self->n_entries--;

            continue;

        }

        // bucket index (bi)
        ks_size_t bi = ent->hash % self->n_buckets;

        // keep track of original
        ks_size_t bi_orig = bi;
        ks_size_t tries = 0;


        bool found = false;

        do {

            // get the entry index (ei), which is an index into self->entries
            int ei = self->buckets[bi];

            if (ei == BUCKET_EMPTY) {
                // the bucket is empty, so set it to 'i' (current entry) and continue;
                self->buckets[bi] = i;
                found = true;
                break;
            }


            tries++;

            // probing function
            bi = bi_orig + tries;

            // always wrap it back around the range
            bi %= self->n_buckets;
        } while (bi != bi_orig);

        if (!found) {
            fprintf(stderr, RED "ERROR" RESET ": Internal dict error! (could not resize dict @ %p)\n", self);
        }
    }
}


// test whether or not the dictionary has a given key
bool ks_dict_has_h(ks_dict self, ks_obj key, ks_hash_t hash) {
    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi_orig = bi;
    ks_size_t tries = 0;

    do {

        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        /**/ if (ei == BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so it is not in here
            return false;
        } else if (ei == BUCKET_DELETED) {
            // do nothing; skip it
        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match

            if (self->entries[ei].key == key || ks_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key
                return true;
            }
        }


        // probing function
        bi = bi_orig + tries;

        // always wrap it back around the range
        bi %= self->n_buckets;
    } while (bi != bi_orig);


    // default case, never found
    return false;
}

// get a given element
ks_obj ks_dict_get_h(ks_dict self, ks_obj key, ks_hash_t hash) {

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi_orig = bi;
    ks_size_t tries = 0;

    do {

        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        /**/ if (ei == BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so we can say it does not contain the given key
            return NULL;
        } else if (ei == BUCKET_DELETED) {
            // do nothing; skip it
        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key already. Now, return the value
                return KS_NEWREF(self->entries[ei].val);
            }
        }

        // try again
        tries++;

        // probing function
        bi = bi_orig + tries;

        // always wrap it back around the range
        bi %= self->n_buckets;
    } while (bi != bi_orig);

    // error: not in dictionary
    return ks_throw_fmt(ks_type_KeyError, "Key '%S' did not exist this dict", key);
}

// set the given element
bool ks_dict_set_h(ks_dict self, ks_obj key, ks_hash_t hash, ks_obj val) {

    // don't allow above a certsain load factor
    // TODO: tune this
    if (self->n_entries >= self->n_buckets / 4) {
        // we need to scale up the dictionary
        dict_resize(self, self->n_buckets * 4);
    }


    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi_orig = bi;
    ks_size_t tries = 0;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        if (ei == BUCKET_EMPTY) {

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

        } else if (ei == BUCKET_DELETED) {
            // do nothing; skip it

        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_eq(self->entries[ei].key, key)) {
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
        bi = bi_orig + tries;

        // always wrap it back around the range
        bi %= self->n_buckets;
    } while (bi != bi_orig);

    // some problem adding it, should not happen
    ks_throw_fmt(ks_type_InternalError, "Could not add '%S' to dict", key);
    return false;
}


// delete the given element
bool ks_dict_del_h(ks_dict self, ks_obj key, ks_hash_t hash) {
    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi_orig = bi;
    ks_size_t tries = 0;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        if (ei == BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so it does not exist in the dictionary
            ks_throw_fmt(ks_type_Error, "Attempted to delete '%S' from dict, but the dict did not contain it!", key);
            return false;

        } else if (ei == BUCKET_DELETED) {
            // do nothing; skip it

        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key already. Now, remove that entry
                if (self->entries[ei].key) KS_DECREF(self->entries[ei].key);
                if (self->entries[ei].val) KS_DECREF(self->entries[ei].val);
                self->entries[ei].hash = 0;
                self->entries[ei].key = self->entries[ei].val = NULL;
                self->buckets[bi] = BUCKET_DELETED;
                return true;
            }
        }

        // probing function
        bi = bi_orig + tries;

        // always wrap it back around the range
        bi %= self->n_buckets;
    } while (bi != bi_orig);

    // didn't exist
    ks_throw_fmt(ks_type_Error, "Attempted to delete '%S' from dict, but the dict did not contain it!", key);
    return false;
}



/* variants of dictionary functions */



bool ks_dict_has(ks_dict self, ks_obj key) {
    // calc hash
    ks_hash_t hash;
    if (!ks_hash(key, &hash)) {
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



ks_obj ks_dict_get(ks_dict self, ks_obj key) {
    // calc hash
    ks_hash_t hash;
    if (!ks_hash(key, &hash)) {
        return false;
    }
    return ks_dict_get_h(self, key, hash);
}

ks_obj ks_dict_get_c(ks_dict self, char* key) {
    ks_str key_str = ks_str_new(key);
    ks_obj res = ks_dict_get_h(self, (ks_obj)key_str, key_str->v_hash);
    KS_DECREF(key_str);
    return res;
}



bool ks_dict_set(ks_dict self, ks_obj key, ks_obj val) {
    // calc hash
    ks_hash_t hash;
    if (!ks_hash(key, &hash)) {
        return false;
    }
    return ks_dict_set_h(self, key, hash, val);
}

bool ks_dict_set_c(ks_dict self, char* key, ks_obj val) {
    ks_str key_str = ks_str_new(key);
    bool res = ks_dict_set_h(self, (ks_obj)key_str, key_str->v_hash, val);
    KS_DECREF(key_str);
    return res;
}



bool ks_dict_del(ks_dict self, ks_obj key) {
    // calc hash
    ks_hash_t hash;
    if (!ks_hash(key, &hash)) {
        return false;
    }
    return ks_dict_del_h(self, key, hash);
}

bool ks_dict_del_c(ks_dict self, char* key) {
    ks_str key_str = ks_str_new(key);
    bool res = ks_dict_del_h(self, (ks_obj)key_str, key_str->v_hash);
    KS_DECREF(key_str);
    return res;
}





/* member functions */

// dict.__new__(self, *kvps) -> create a new dictionary
static KS_TFUNC(dict, new) {
    KS_REQ_N_ARGS_MIN(n_args, 0);
    if (n_args % 2 != 0) {
        return ks_throw_fmt(ks_type_Error, "Expected an even number of arguments (k, v, ...), but got %i", n_args);
    }
    return (ks_obj)ks_dict_new(n_args, args);
}

// dict.__getitem__(self, key) -> get an entry
static KS_TFUNC(dict, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");
    ks_obj key = args[1];

    return ks_dict_get(self, key);
}

// dict.__setitem__(self, key, val) -> set an entry
static KS_TFUNC(dict, setitem) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");
    ks_obj key = args[1], val = args[2];

    if (!ks_dict_set(self, key, val)) {
        return NULL;
    } else {
        return KS_NEWREF(val);
    }
}

// dict.__iter__(self) -> return an iterator for a dictionary
static KS_TFUNC(dict, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    return (ks_obj)ks_dict_iter_new(self);
}


// dict.keys(self) -> return a list of keys
static KS_TFUNC(dict, keys) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    ks_list ret = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].hash != 0 && self->entries[i].val != NULL) {
            ks_list_push(ret, self->entries[i].key);
        }
    }

    return (ks_obj)ret;
}

// dict.vals(self) -> return a list of vals
static KS_TFUNC(dict, vals) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    ks_list ret = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].hash != 0 && self->entries[i].val != NULL) {
            ks_list_push(ret, self->entries[i].val);
        }
    }

    return (ks_obj)ret;
}

// dict.__str__(self) -> convert to string
static KS_TFUNC(dict, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    ks_str_b SB;
    ks_str_b_init(&SB);

    ks_str_b_add(&SB, 1, "{");

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].hash != 0) {
            if (i > 0 && i < self->n_entries) ks_str_b_add(&SB, 2, ", ");

            // add the item
            ks_str_b_add_repr(&SB, self->entries[i].key);
            ks_str_b_add(&SB, 2, ": ");
            ks_str_b_add_repr(&SB, self->entries[i].val);
        }
    }

    ks_str_b_add(&SB, 1, "}");

    ks_str ret = ks_str_b_get(&SB);

    ks_str_b_free(&SB);

    return (ks_obj)ret;
}

// dict.__free__(self) -> free resources
static KS_TFUNC(dict, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    // free all entries
    int i;
    for (i = 0; i < self->n_entries; ++i) {
        // decrease any reference we held here
        if (self->entries[i].key) KS_DECREF(self->entries[i].key);
        if (self->entries[i].val) KS_DECREF(self->entries[i].val);
    }

    // free buffers used
    ks_free(self->buckets);
    ks_free(self->entries);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// dict.__len__(self) -> return length in elements
static KS_TFUNC(dict, len) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    int64_t ret = 0;

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].hash != 0 && self->entries[i].val != NULL) {
            ret++;
        }
    }

    return (ks_obj)ks_int_new(ret);
}




/* iterator */

ks_dict_iter ks_dict_iter_new(ks_dict obj) {
    ks_dict_iter self = KS_ALLOC_OBJ(ks_dict_iter);
    KS_INIT_OBJ(self, ks_type_dict_iter);

    // initialize type-specific things
    self->pos = 0;
    self->obj = (ks_dict)KS_NEWREF(obj);

    return self;
}

// dict_iter.__free__(self) -> free resources
static KS_TFUNC(dict_iter, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict_iter self = (ks_dict_iter)args[0];
    KS_REQ_TYPE(self, ks_type_dict_iter, "self");

    // release reference to the list
    KS_DECREF(self->obj);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// dict_iter.__next__(self) -> return the next (key, val) pair, or return an error
static KS_TFUNC(dict_iter, next) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict_iter self = (ks_dict_iter)args[0];
    KS_REQ_TYPE(self, ks_type_dict_iter, "self");


    // skip empty/deleted entries
    while (self->pos < self->obj->n_entries && self->obj->entries[self->pos].val == NULL) {
        self->pos++;
    }


    if (self->pos >= self->obj->n_entries) {
        return ks_throw_fmt(ks_type_OutOfIterError, "");
    } else {
        // get next object
        ks_tuple ret = ks_tuple_new_n(2, (ks_obj[]){ 
            self->obj->entries[self->pos].key, 
            self->obj->entries[self->pos].val 
        });

        // declare it as used
        self->pos++;
        return (ks_obj)ret;
    }
}



// initialize dict type
void ks_type_dict_init() {
    KS_INIT_TYPE_OBJ(ks_type_dict, "dict");

    ks_type_set_cn(ks_type_dict, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(dict_new_, "dict.__new__(self, *keyvals)")},
        {"__str__", (ks_obj)ks_cfunc_new2(dict_str_, "dict.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(dict_str_, "dict.__repr__(self)")},
        {"__free__", (ks_obj)ks_cfunc_new2(dict_free_, "dict.__free__(self)")},

        {"__getitem__", (ks_obj)ks_cfunc_new2(dict_getitem_, "dict.__getitem__(self, key)")},
        {"__setitem__", (ks_obj)ks_cfunc_new2(dict_setitem_, "dict.__setitem__(self, key, val)")},
        
        {"__iter__", (ks_obj)ks_cfunc_new2(dict_iter_, "dict.__iter__(self)")},

        {"__len__", (ks_obj)ks_cfunc_new2(dict_len_, "dict.__len__(self)")},

        {"keys", (ks_obj)ks_cfunc_new2(dict_keys_, "dict.keys(self)")},
        {"vals", (ks_obj)ks_cfunc_new2(dict_vals_, "dict.vals(self)")},

        {NULL, NULL}   
    });


    // add iterable type
    KS_INIT_TYPE_OBJ(ks_type_dict_iter, "dict_iter");

    ks_type_set_cn(ks_type_dict_iter, (ks_dict_ent_c[]){
        {"__free__", (ks_obj)ks_cfunc_new2(dict_iter_free_, "dict_iter.__free__(self)")},

        {"__next__", (ks_obj)ks_cfunc_new2(dict_iter_next_, "dict_iter.__next__(self)")},


        {NULL, NULL}   
    });

}

