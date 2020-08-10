/* set.c - implementation of the set type
 *
 * Very closely related to the dictionary implementation; essentially copied from there
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


/* Constants */

// A bucket will be this value if it is empty
#define KS_SET_BUCKET_EMPTY     (-1)

// A bucket will be this value if it has been deleted
#define KS_SET_BUCKET_DELETED   (-2)


/* Tuning/Performance parameters */

// what is the maximum load factor we can tolerate in a set?
// When a dictionary exceeds this load factor, it is resized
#define KS_SET_MAX_LOAD 0.3

// what should the new load factor be when resizing a set?
// When a dictionary is being resized/rehashed, this is the target load factor
#define KS_SET_NEW_LOAD 0.15

// probe offset for index 'i'
#define KS_SET_PROBE(i) (i)
// quadratic probing
//#define KS_SET_PROBE(i) ((i)*(i))


/* Utility Funcs */

// return true if 'x' is prime, false otherwise
// TODO: perhaps use Miller-Rabin tests or the like for larger numbers?
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


// Create a new 'set' with 'len' elements (NOTE: the actual set may have fewer elements, if some compare equal)
// NOTE: Returns new reference, or NULL if an error was thrown
ks_set ks_set_new(ks_size_t len, ks_obj* elems) {
    ks_set self = KS_ALLOC_OBJ(ks_set);
    KS_INIT_OBJ(self, ks_T_set);

    // empty entries
    self->n_entries = 0;
    self->entries = NULL;

    // empty buckets
    self->n_buckets = 0;
    self->buckets = NULL;

    ks_size_t i;
    for (i = 0; i < len; ++i) {
        // get key/val pair
        ks_set_add(self, elems[i]);
    }

    // return constructed dictionary
    return self;

}


// calculate load factor
double ks_set_load(ks_set self) {
    return self->n_buckets > 0 ? (double)self->n_entries / self->n_buckets : 0.0;
}



// resize a set to have at least `new_n_buckets` buckets
static void set_resize(ks_set self, ks_size_t new_n_buckets) {
    // check if we are alreayd large enough
    if (self->n_buckets >= new_n_buckets && self->n_buckets > 0) return;

    // ensure its a prime
    new_n_buckets = next_prime(new_n_buckets - 1);

    // loop vars
    ks_size_t i, j;

    // now, allocate new buckets
    self->n_buckets = new_n_buckets;
    self->buckets = ks_realloc(self->buckets, sizeof(*self->buckets) * self->n_buckets);
    for (i = 0; i < self->n_buckets; ++i) self->buckets[i] = KS_SET_BUCKET_EMPTY;


    // now, go through and rehash the entries
    // NOTE: any entries that are not found will be removed
    for (i = 0; i < self->n_entries; ++i) {
        // i'th entry
        struct ks_set_entry* ent = &self->entries[i];

        if (ent->hash == 0 || ent->key == NULL) {
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

            if (ei == KS_SET_BUCKET_EMPTY) {
                // the bucket is empty, so set it to 'i' (current entry) and continue;
                self->buckets[bi] = i;
                found = true;
                break;
            }


            tries++;

            // probing function
            bi = (bi0 + KS_SET_PROBE(tries)) % self->n_buckets;

        } while (bi != bi0);

        assert (found && "could not resize set!");
    }
}

// Add 'key' to a set
// NOTE: Returns success, or `false` and throws an error
bool ks_set_add_h(ks_set self, ks_obj key, ks_hash_t hash) {

    if (ks_set_load(self) > KS_SET_MAX_LOAD || self->n_buckets == 0) set_resize(self, (int)(self->n_entries / KS_SET_NEW_LOAD));

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original and how many tries
    ks_size_t bi0 = bi, tries = 0;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        if (ei == KS_SET_BUCKET_EMPTY) {

            // we have found an empty bucket before a corresponding entry, so we can safely replace it
            ei = self->n_entries++;
            self->entries = ks_realloc(self->entries, sizeof(*self->entries) * self->n_entries);

            // set the bucket to the new location
            self->buckets[bi] = ei;

            // we are making a new entry, so we need to make new references to the key and the value
            KS_INCREF(key);
            
            // set that entry
            self->entries[ei] = (struct ks_set_entry){ .hash = hash, .key = key };
            
            // success
            return true;

        } else if (ei == KS_SET_BUCKET_DELETED) {
            // do nothing; skip it

        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_obj_eq(self->entries[ei].key, key)) {
                // the keys are equal, so the set already contains it, so we don't have to do anything
                return true;
            }
        }
        tries++;

        // probing function
        bi = (bi0 + KS_SET_PROBE(tries)) % self->n_buckets;

    } while (bi != bi0);

    assert (false && "could not set set (this should never happen)!");

    return false;
}


// Check whether `key` is in the given set
// NOTE: Returns success, or `false`, but NEVER throw an error
bool ks_set_has_h(ks_set self, ks_obj key, ks_hash_t hash) {
    if (self->n_buckets < 1) goto get_h_end;

    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original
    ks_size_t bi0 = bi, tries = 0;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        /**/ if (ei == KS_SET_BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so we can say it does not contain the given key
            return false;
        } else if (ei == KS_SET_BUCKET_DELETED) {
            // do nothing; skip it
        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_obj_eq(self->entries[ei].key, key)) {
                // they are equal, so it contains the key already. Now, return the value
                return true;
            }
        }

        // try again
        tries++;

        bi = (bi0 + KS_SET_PROBE(tries)) % self->n_buckets;

    } while (bi != bi0);


    get_h_end: ;

    // error: not in set
    return false;
}


// delete item from set
bool ks_set_del_h(ks_set self, ks_obj key, ks_hash_t hash) {
    // bucket index (bi)
    ks_size_t bi = hash % self->n_buckets;

    // keep track of original and how many tries
    ks_size_t bi0 = bi, tries = 0;

    do {
        // get the entry index (ei), which is an index into self->entries
        int ei = self->buckets[bi];

        if (ei == KS_SET_BUCKET_EMPTY) {
            // we have found an empty bucket before a corresponding entry, so it does not exist
            // already removed
            return true;

        } else if (self->entries[ei].hash == hash) {
            // possible match; the hashes match
            if (self->entries[ei].key == key || ks_obj_eq(self->entries[ei].key, key)) {
                // the keys are equal, so the set already contains the key

                // since we are replacing the value, the previous values must be dereferenced
                KS_DECREF(self->entries[ei].key);

                // set to NULL
                self->entries[ei].key = NULL;

                // delete this bucket
                self->buckets[bi] = KS_SET_BUCKET_DELETED;

                // success
                return true;
            }
        }
        tries++;

        // probing function
        bi = (bi0 + KS_SET_PROBE(tries)) % self->n_buckets;

    } while (bi != bi0);

    // not found, so no error, nothing to delete
    return true;
}

/* Misc. Derivative Functions */

bool ks_set_add(ks_set self, ks_obj key) {
    ks_hash_t hash;
    if (!ks_obj_hash(key, &hash)) return false;

    return ks_set_add_h(self, key, hash);
}

bool ks_set_del(ks_set self, ks_obj key) {
    ks_hash_t hash;
    if (!ks_obj_hash(key, &hash)) return false;

    return ks_set_del_h(self, key, hash);
}

bool ks_set_has(ks_set self, ks_obj key) {
    ks_hash_t hash;
    if (!ks_obj_hash(key, &hash)) {
        ks_catch_ignore();
        return false;
    }
    return ks_set_has_h(self, key, hash);
}



/* Object Methods */

// set.__new__(objs=None) - create new set object
static KS_TFUNC(set, new) {
    ks_obj objs = KSO_NONE;
    KS_GETARGS("?objs", &objs)


    if (objs == KSO_NONE) {
        return (ks_obj)ks_set_new(0, NULL);
    } else {
        if (!ks_obj_is_iterable(objs)) return ks_throw(ks_T_ArgError, "'objs' must be iterable or 'none'");

        // create new set
        ks_set ret = ks_set_new(0, NULL);
        // iterate and add to set
        struct ks_citer cit = ks_citer_make(objs);
        ks_obj ob;
        while ((ob = ks_citer_next(&cit)) != NULL) {
            if (!ks_set_add(ret, ob)) cit.threwErr = true;            
            KS_DECREF(ob);
        }

        ks_citer_done(&cit);
        if (cit.threwErr) {
            KS_DECREF(ret);
            return NULL;
        }

        return (ks_obj)ret;
    }

}

// set.__free__(self) -> free obj
static KS_TFUNC(set, free) {
    ks_set self;
    KS_GETARGS("self:*", &self, ks_T_set)

    ks_size_t i;

    // free references held to entries
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].key != NULL) KS_DECREF(self->entries[i].key);
    }

    // free allocated space
    ks_free(self->entries);
    ks_free(self->buckets);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// set.__str__(self) - convert to string
static KS_TFUNC(set, str) {
    ks_set self;
    KS_GETARGS("self:*", &self, ks_T_set)

    // build up a string
    ks_str_builder sb = ks_str_builder_new();

    ks_str_builder_add(sb, "{", 1);

    int i, ct = 0;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].hash != 0) {
            if (ct > 0) ks_str_builder_add(sb, ", ", 2);

            // add the item
            ks_str_builder_add_repr(sb, self->entries[i].key);
            ct++;
        }
    }

    ks_str_builder_add(sb, "}", 1);
    if (ct > 0) {
        ks_str ret = ks_str_builder_get(sb);
        KS_DECREF(sb);

        return (ks_obj)ret;
    } else {
        // special case to not confuse it with dictionaries
        KS_DECREF(sb);
        return (ks_obj)ks_str_new("set()");
    }
}

// set.__len__(self) - get length
static KS_TFUNC(set, len) {
    ks_set self;
    KS_GETARGS("self:*", &self, ks_T_set)
 
    // count non-null entries
    ks_ssize_t i, ct = 0;
    for (i = 0; i < self->n_entries; ++i) {
        if (self->entries[i].key != NULL) ct++;
    }

    return (ks_obj)ks_int_new(ct);
}

/* Iterator Type */

// ks_set_iter - set iterable type
typedef struct {
    KS_OBJ_BASE

    // set being iterated
    ks_set self;

    // current position
    ks_size_t pos;


}* ks_set_iter;

KS_TYPE_DECLFWD(ks_T_set_iter);

// set_iter.__free__(self) - free obj
static KS_TFUNC(set_iter, free) {
    ks_set_iter self;
    KS_GETARGS("self:*", &self, ks_T_set_iter)

    // remove reference to string
    KS_DECREF(self->self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// set_iter.__next__(self) - return next character
static KS_TFUNC(set_iter, next) {
    ks_set_iter self;
    KS_GETARGS("self:*", &self, ks_T_set_iter)


    while (self->pos < self->self->n_entries && self->self->entries[self->pos].key == NULL) {
        self->pos++;
    }

    // no more valid entries
    if (self->pos >= self->self->n_entries) return ks_throw(ks_T_OutOfIterError, "");

    ks_obj ret = self->self->entries[self->pos++].key;
    KS_INCREF(ret);
    return ret;
}

// set.__iter__(self) - return iterator
static KS_TFUNC(set, iter) {
    ks_set self;
    KS_GETARGS("self:*", &self, ks_T_set)

    ks_set_iter ret = KS_ALLOC_OBJ(ks_set_iter);
    KS_INIT_OBJ(ret, ks_T_set_iter);

    ret->self = self;
    KS_INCREF(self);
    ret->pos = 0;

    return (ks_obj)ret;
}


/* export */

KS_TYPE_DECLFWD(ks_T_set);

void ks_init_T_set() {
    ks_type_init_c(ks_T_set, "set", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c(set_new_, "set.__new__(objs=none)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(set_free_, "set.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c(set_str_, "set.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(set_str_, "set.__repr__(self)")},

        {"__len__",                (ks_obj)ks_cfunc_new_c(set_len_, "set.__len__(self)")},

        {"__iter__",               (ks_obj)ks_cfunc_new_c(set_iter_, "set.__iter__(self)")},

    ));
    ks_type_init_c(ks_T_set_iter, "set_iter", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(set_iter_free_, "set_iter.__free__(self)")},

        {"__next__",               (ks_obj)ks_cfunc_new_c(set_iter_next_, "set_iter.__next__(self)")},
    ));
}
