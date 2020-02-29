/* types/dict.c - dictionary implementation, for key->value mappings of all types
 *
 * Internally, we use a hash table with open addressing to store the objects,
 *   and their hashes are determined by the builtin hash() function (although we make
 *   optimizations for hashes of strings, which are the most common, and are precomputed)
 * 
 * A bucket is said to be empty if `bucket.val==NULL`, so this can be used to test if you
 *   can put an item in the bucket
 * 
 * 
 * 
 */

#include "ks_common.h"

// starting length for the dictionary, the minimum
#define _DICT_MIN_LEN 8

// the maximum load value (as a percentage used)
// once the load factor exceeds this, the dictionary is resized
#define _DICT_LOAD_MAX 30

// generates the new size for the dictionary when it needs to be resized
#define _DICT_NEW_SIZE(_dict) (2 * (_dict)->n_buckets + (_dict)->n_items)

// return a new empty dictionary, with no elements
ks_dict ks_dict_new_empty() {
    ks_dict self = (ks_dict)ks_malloc(sizeof(*self));
    *self = (struct ks_dict) {
        KSO_BASE_INIT(ks_T_dict)
        .n_items = 0,
        .n_buckets = _DICT_MIN_LEN,
        .buckets = ks_malloc(sizeof(*self->buckets) * _DICT_MIN_LEN)
    };

    // initialize them to empty buckets (i.e. everything is 0)
    int i;
    for (i = 0; i < _DICT_MIN_LEN; ++i) {
        self->buckets[i] = (struct ks_dict_entry){ .key = NULL, .hash = 0, .val = NULL };
    }
    return self;
}


/* dictionary helpers */

// maps a hash to a bucket index
static int32_t dict_buck(ks_dict self, uint64_t hash) {
    return hash % self->n_buckets;
}

// gets the next bucket, given a try index
// this is the probing function
static int32_t dict_buck_next(int32_t cur_buck, int try) {
    // linear probing
    return cur_buck + 1;
}

// check whether it fully matches
static bool dict_entry_matches(struct ks_dict_entry entry, kso key, uint64_t hash) {
    // TODO: Also add a literal `x==y` using their object types and everything
    return entry.hash == hash && kso_eq(entry.key, key);
}

/* prime number finding, for optimal hash-table sizes */

// return true iff 'x' is prime
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

// resize a dictionary to a given size
void ks_dict_resize(ks_dict self, int new_size) {
    // if we already have enough, do nothing
    if (self->n_buckets >= new_size) return;

    // always round up to a prime number
    new_size = nextprime(new_size);
    //ks_trace("dict resize %d -> %d", self->n_buckets, new_size);

    // get the old entries
    int old_n_buckets = self->n_buckets;
    struct ks_dict_entry* old_buckets = self->buckets;

    // allocate the new buckets
    self->n_buckets = new_size;
    self->buckets = ks_malloc(sizeof(*self->buckets) * self->n_buckets);

    // initialize them to empty buckets
    int i;
    for (i = 0; i < self->n_buckets; ++i) {
        self->buckets[i] = (struct ks_dict_entry){ .key = NULL, .hash = 0, .val = NULL };
    }

    // go through all the buckets, and merge them over, this is called rehashing
    for (i = 0; i < old_n_buckets; ++i) {
        struct ks_dict_entry* old_entry = &old_buckets[i];

        if (old_entry->val != NULL) {
            // we have a valid bucket that needs to be copied
            ks_dict_set(self, old_entry->key, old_entry->hash, old_entry->val);
            KSO_DECREF(old_entry->key);
            KSO_DECREF(old_entry->val);
        }
    }

    // free the old buckets
    ks_free(old_buckets);
}

int ks_dict_set(ks_dict self, kso key, uint64_t hash, kso val) {

    // we will always hold a new reference to 'val'
    KSO_INCREF(val);

    // make sure it is large enough, and/or resize if it has reached the critical load factor
    if (self->n_buckets * _DICT_LOAD_MAX <= self->n_items * 100) {
        ks_dict_resize(self, _DICT_NEW_SIZE(self));
    }

    struct ks_dict_entry* entry = NULL;
    int b_idx = dict_buck(self, hash), tries = 0;

    // first, search through filled buckets (those)
    while ((entry = &self->buckets[b_idx])->val != NULL && tries++ < self->n_buckets) {

        if (dict_entry_matches(*entry, key, hash)) {
            // we've found it, just replace the value
            KSO_DECREF(entry->val);
            entry->val = val;
            return b_idx;
        }

        // update the bucket index, try again
        b_idx = dict_buck_next(b_idx, tries);
        // wrap back around
        while (b_idx > self->n_buckets) b_idx -= self->n_buckets;
    }

    // if we've gotten to here, it means we found an empty bucket, so just replace it, and add the new item
    KSO_INCREF(key);
    
    self->n_items++;

    *entry = (struct ks_dict_entry) {
        .key = key,
        .hash = hash,
        .val = val
    };

    return b_idx;
}

kso ks_dict_get(ks_dict self, kso key, uint64_t hash) {
    if (self->n_buckets == 0) return NULL;

    int b_idx = dict_buck(self, hash), tries = 0;
    struct ks_dict_entry* entry = NULL;


    // search through non-empty buckets
    while ((entry = &self->buckets[b_idx])->val != NULL && tries++ < self->n_buckets) {

        if (dict_entry_matches(*entry, key, hash)) {
            // we've found a match, just return it
            return entry->val;
        }

        // update the bucket index, try again
        b_idx = dict_buck_next(b_idx, tries);
        // wrap back around
        while (b_idx > self->n_buckets) b_idx -= self->n_buckets;
    }


    // not found, return NULL
    return NULL;

}

// gets an item in the dictionary, given a C-string key
kso ks_dict_get_cstrl(ks_dict self, char* cstr, int len) {
    ks_str stro = ks_str_new_l(cstr, len);
    kso ret = ks_dict_get(self, (kso)stro, stro->v_hash);
    KSO_DECREF(stro);
    return ret;
}

// gets an item in the dictionary, given a C-string key
kso ks_dict_get_cstr(ks_dict self, char* cstr) {
    ks_str stro = ks_str_new(cstr);
    kso ret = ks_dict_get(self, (kso)stro, stro->v_hash);
    KSO_DECREF(stro);
    return ret;
}

// sets an item in the dictionary, given a C-string key
void ks_dict_set_cstrl(ks_dict self, char* cstr, int len, kso val) {
    ks_str stro = ks_str_new_l(cstr, len);
    ks_dict_set(self, (kso)stro, stro->v_hash, val);
    KSO_DECREF(stro);
}


// sets an item in the dictionary, given a C-string key
void ks_dict_set_cstr(ks_dict self, char* cstr, kso val) {
    ks_str stro = ks_str_new(cstr);
    ks_dict_set(self, (kso)stro, stro->v_hash, val);
    KSO_DECREF(stro);
}


/* KSCRIPT FUNCTIONS */

// dict.__new__(...)
// TODO: document
KS_TFUNC(dict, new) {
    if (n_args == 0) {
        // just construct an empty one
        return (kso)ks_dict_new_empty();
    } else if (n_args == 1) {
        // should be a collection of (key, val) pairs in a list/tuple
        kso entries = args[0];
        kso* ent_src = NULL;
        int ne = 0;
        if (entries->type == ks_T_list) {
            ent_src = ((ks_list)entries)->items;
            ne = ((ks_list)entries)->len;
        } else if (entries->type == ks_T_tuple) {
            ent_src = ((ks_tuple)entries)->items;
            ne = ((ks_tuple)entries)->len;
        } else {
            return kse_fmt("Invalid argument; expected the first argument to be either a list or tuple containg (key, val) pairs");
        }

        // otherwise, create an object and fill it with entries
        ks_dict ret = ks_dict_new_empty();

        // set them all
        int i;
        for (i = 0; i < ne; ++i) {
            kso cur = ent_src[i];
            if (cur->type == ks_T_tuple) {
                ks_tuple curtup = (ks_tuple)cur;
                if (curtup->len == 2) {
                    // add key value pair
                    ks_dict_set(ret, curtup->items[0], kso_hash(curtup->items[0]), curtup->items[1]);
                } else {
                    KSO_DECREF(ret);
                    return kse_fmt("Invalid key,val pair (idx %i) in the list of (key, val) pairs. Expected length 2, but got length %i", i, (int)curtup->len);
                }

            } else {
                KSO_DECREF(ret);
                return kse_fmt("Invalid argument (idx %i) in the list of (key, val) pairs. Was of type '%T', but expected a tuple", i, cur);
            }
        }
        return (kso)ret;

    } else if (n_args % 2 == 0) {
        // every other argument is key,val
        ks_dict ret = ks_dict_new_empty();

        int i;
        for (i = 0; i < n_args; i += 2) {
            ks_dict_set(ret, args[i+0], kso_hash(args[i+0]), args[i+1]);
        }

        return (kso)ret;

    } else {
        return kse_fmt("Invalid arguments; expected either: '[(key, val), ...]' or 'key, val, ...' to create dict");
    }
}


// dict.__str__(self) -> return a string for the dictionary
KS_TFUNC(dict, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");

    if (self->n_items == 0) {
        return (kso)ks_str_new("{}");
    }

    // format for the string builder is {KEY: VAL, KEY: VAL, ...}
    ks_strB ksb = ks_strB_create();
    ks_strB_add(&ksb, "{", 1);

    int i, num = 0;
    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* ent = &self->buckets[i];
        // only add non-empty buckets
        if (ent->val != NULL) {
            if (num != 0) ks_strB_add(&ksb, ", ", 2);
            ks_strB_add_repr(&ksb, ent->key);
            ks_strB_add(&ksb, ": ", 2);
            ks_strB_add_repr(&ksb, ent->val);
            num++;
        }
    }
    ks_strB_add(&ksb, "}", 1);

    return (kso)ks_strB_finish(&ksb);
}

// dict.__repr__(self) -> return a string representation of the dictionary
KS_TFUNC(dict, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");

    if (self->n_items == 0) {
        return (kso)ks_str_new("{}");
    }

    // format for the string builder is {KEY: VAL, KEY: VAL, ...}
    ks_strB ksb = ks_strB_create();
    ks_strB_add(&ksb, "{", 1);

    int i, num = 0;
    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* ent = &self->buckets[i];
        // only add non-empty buckets
        if (ent->val != NULL) {
            if (num != 0) ks_strB_add(&ksb, ", ", 2);
            ks_strB_add_repr(&ksb, ent->key);
            ks_strB_add(&ksb, ": ", 2);
            ks_strB_add_repr(&ksb, ent->val);
            num++;
        }
    }
    ks_strB_add(&ksb, "}", 1);

    return (kso)ks_strB_finish(&ksb);
}

// dict.__free__(self) -> free dictionary
KS_TFUNC(dict, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");

    int i;
    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* entry = &self->buckets[i];
        // dec-references from non-empty buckets
        if (entry->val != NULL) {
            KSO_DECREF(entry->key);
            KSO_DECREF(entry->val);
        }
    }

    // free allocated buffers
    ks_free(self->buckets);
    ks_free(self);

    return KSO_NONE;
}


// dict.__getitem__(self, key, def=None) -> get item by 'key' in dictionary, with an optional default argument
KS_TFUNC(dict, getitem) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");
    kso key = args[1];

    // ask for the result in the dictionary
    kso res = ks_dict_get(self, key, kso_hash(key));

    if (res == NULL) {
        if (n_args == 3) {
            // default here
            res = args[2];
        } else {
            // otherwise, it was requested with an error
            return kse_fmt("KeyError: %R", key);
        }
    }

    return KSO_NEWREF(res);
}

// dict.__setitem____(self, key, val) -> set a given key in the dictionary to that value
KS_TFUNC(dict, setitem) {

    KS_REQ_N_ARGS(n_args, 3);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");
    kso key = args[1], val = args[2];

    ks_dict_set(self, key, kso_hash(key), val);

    return KSO_NEWREF(val);
}


KS_TFUNC(dict, get) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");
    kso key = args[1];

    kso res = ks_dict_get(self, key, kso_hash(key));

    if (res == NULL) {
        if (n_args == 3) {
            // default here
            res = args[2];
        } else {
            return kse_fmt("KeyError: %R", key);
        }
    }

    return KSO_NEWREF(res);
}

// dict.__iter__(self) -> return a dictionary key,val iterator for 'self'
KS_TFUNC(dict, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_dict self = (ks_dict)args[0];
    KS_REQ_TYPE(self, ks_T_dict, "self");

    return (kso)ks_dict_iter_new(self);
}


/* exporting functionality */

struct ks_type T_dict, *ks_T_dict = &T_dict;

void ks_init__dict() {

    /* create the type */
    T_dict = KS_TYPE_INIT();

    ks_type_setname_c(ks_T_dict, "dict");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_dict, "__new__", "dict.__new__(...)", dict_new_);
    ADDCF(ks_T_dict, "__str__", "dict.__str__(self)", dict_str_);
    ADDCF(ks_T_dict, "__repr__", "dict.__repr__(self)", dict_repr_);
    
    ADDCF(ks_T_dict, "__getitem__", "dict.__getitem__(self, key)", dict_getitem_);
    ADDCF(ks_T_dict, "__setitem__", "dict.__setitem__(self, key, val)", dict_setitem_);

    ADDCF(ks_T_dict, "__iter__", "dict.__iter__(self)", dict_iter_);

    ADDCF(ks_T_dict, "__free__", "dict.__free__(self)", dict_free_);

}

