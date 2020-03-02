/* types/dict.c - kscript's basic dictionary implementation
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
ks_dict ks_new_dict(int len, ks_obj* entries) {
    ks_dict self = KS_ALLOC_OBJ(ks_dict);
    KS_INIT_OBJ(self, ks_type_dict);

    // always start with no entries
    self->n_entries = 0;
    self->entries = NULL;

    // calculate a good size of buckets for the length
    self->n_buckets = next_prime(2 * len + 5);
    self->buckets = ks_malloc(sizeof(*self->buckets) * self->n_buckets);
    
    // now, set all buckets to empty
    int i;
    for (i = 0; i < self->n_buckets; ++i) self->buckets[i] = BUCKET_EMPTY;

    // now, set the (key, val) pairs of 'entries' into the dictionary
    for (i = 0; i < len; ++i) {
        ks_dict_set(self, 0, entries[2 * i], entries[2 * i + 1]);
    }


    return self;
}


// construct a new dictionary from C-style entries
ks_dict ks_new_dict_c(ks_dict_ent_c* ent_cs) {

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

    while (ent_cs->key != NULL) {
        // keep iterating t hrough entries
        ks_str my_key = ks_new_str(ent_cs->key);
        ks_dict_set(self, my_key->v_hash, (ks_obj)my_key, ent_cs->val);

        KS_DECREF(my_key);

        // next entry
        ent_cs++;
    }


    return self;

}

// construct a new dictionary from C-style entries, but do not create new references for them
ks_dict ks_new_dict_cn(ks_dict_ent_c* ent_cns) {

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
        ks_str my_key = ks_new_str(ent_cns->key);
        ks_dict_set(self, my_key->v_hash, (ks_obj)my_key, ent_cns->val);

        KS_DECREF(my_key);

        // since there will be a reference added, we remove it here
        KS_DECREF(ent_cns->val);

        // next entry
        ent_cns++;
    }


    return self;

}

// Sets a list of C-entries (without creating new references)
int ks_dict_set_cn(ks_dict self, ks_dict_ent_c* ent_cns) {
    while (ent_cns->key != NULL) {
        // keep iterating t hrough entries
        ks_str my_key = ks_new_str(ent_cns->key);

        ks_dict_set(self, my_key->v_hash, (ks_obj)my_key, ent_cns->val);

        KS_DECREF(my_key);

        // since there will be a reference added, we remove it here
        KS_DECREF(ent_cns->val);

        // next entry
        ent_cns++;
    }
}


// free a kscript dict
void ks_free_dict(ks_dict self) {

    int i;
    for (i = 0; i < self->n_entries; ++i) {
        // decrease any reference we held here
        if (self->entries[i].key) KS_DECREF(self->entries[i].key);
        if (self->entries[i].val) KS_DECREF(self->entries[i].val);
    }

    ks_free(self->buckets);
    ks_free(self->entries);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
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

            // probing function
            bi = bi_orig + tries;

            // always wrap it back around the range
            bi %= self->n_buckets;
        } while (bi != bi_orig);

        if (!found) {
            ks_error("Internal Dictionary Error! (Could not resize)");
        }
    }
}


// test whether or not the dictionary has a given key
bool ks_dict_has(ks_dict self, ks_hash_t hash, ks_obj key) {
    // try and make sure that hash is correct
    if (hash == 0) hash = ks_hash(key);
    if (hash == 0) return false;

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
ks_obj ks_dict_get(ks_dict self, ks_hash_t hash, ks_obj key) {
    // try and make sure that hash is correct
    if (hash == 0) hash = ks_hash(key);
    if (hash == 0) return NULL;

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


        // probing function
        bi = bi_orig + tries;

        // always wrap it back around the range
        bi %= self->n_buckets;
    } while (bi != bi_orig);


    return NULL;
}



// set the given element
int ks_dict_set(ks_dict self, ks_hash_t hash, ks_obj key, ks_obj val) {

    if (self->n_entries >= self->n_buckets / 4) {
        // we need to scale up the dictionary
        dict_resize(self, self->n_buckets * 4);
    }

    // try and make sure that hash is correct
    if (hash == 0) hash = ks_hash(key);
    if (hash == 0) return -1;

    // we will always increase the reference count for the new value
    KS_INCREF(val);

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

            // since key is just now being added, we need to add a new reference to it
            KS_INCREF(key);
            
            // set that entry
            self->entries[ei] = (struct ks_dict_entry){ .hash = hash, .key = key, .val = val };

            return 0;

        } else if (ei == BUCKET_DELETED) {
            // do nothing; skip it

        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key already. Now, just update the value
                self->entries[ei].val = val;
                return 1;
            }
        }


        // probing function
        bi = bi_orig + tries;

        // always wrap it back around the range
        bi %= self->n_buckets;
    } while (bi != bi_orig);

    // some problem adding it, should not happen
    return -1;
}



// delete the given element
bool ks_dict_del(ks_dict self, ks_hash_t hash, ks_obj key) {
    // try and make sure that hash is correct
    if (hash == 0) hash = ks_hash(key);
    if (hash == 0) return false;

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi_orig = bi;
    ks_size_t tries = 0;

    do {

        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        if (ei == BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so it does not exist
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
    return false;
}

/* member functions */

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
};

// dict.__free__(self) -> free resources
static KS_TFUNC(dict, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_type_dict, "self");

    // call internal free function
    ks_free_dict(self);

    return KSO_NONE;
};


// initialize dict type
void ks_type_dict_init() {
    KS_INIT_TYPE_OBJ(ks_type_dict, "dict");

    ks_type_set_cn(ks_type_dict, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_new_cfunc(dict_str_)},
        {"__free__", (ks_obj)ks_new_cfunc(dict_free_)},
        {NULL, NULL}   
    });

}

