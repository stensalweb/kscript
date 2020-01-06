/* types/dict.c - dictionary implementation */

#include "ks_common.h"

/*
Dictionary implementation:

*/

// starting length for the dictionary
#define _DICT_MIN_LEN 8

// the maximum load value (as a percentage used)
// once the load factor exceeds this, the dictionary is resized
#define _DICT_LOAD_MAX 25

// generates the new size for the dictionary
#define _DICT_NEW_SIZE(_dict) (2 * (_dict)->n_buckets + (_dict)->n_items)

// return a new empty dictionary
ks_dict ks_dict_new_empty() {
    ks_dict self = (ks_dict)ks_malloc(sizeof(*self));
    *self = (struct ks_dict) {
        KSO_BASE_INIT(ks_T_dict)
        .n_items = 0,
        .n_buckets = _DICT_MIN_LEN,
        .buckets = ks_malloc(sizeof(*self->buckets) * _DICT_MIN_LEN)
    };

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


void ks_dict_resize(ks_dict self, int new_size) {
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

    // initialize them to empty
    int i;
    for (i = 0; i < self->n_buckets; ++i) {
        self->buckets[i] = (struct ks_dict_entry){ .key = NULL, .hash = 0, .val = NULL };
    }

    // go through all the buckets, and merge them over
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

    KSO_INCREF(val);

    // make sure it is large enough
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
kso ks_dict_get_cstr(ks_dict self, char* cstr) {
    ks_str stro = ks_str_new(cstr);
    kso ret = ks_dict_get(self, (kso)stro, stro->v_hash);
    KSO_DECREF(stro);
    return ret;
}

// sets an item in the dictionary, given a C-string key
void ks_dict_set_cstr(ks_dict self, char* cstr, kso val) {
    ks_str stro = ks_str_new(cstr);
    ks_dict_set(self, (kso)stro, stro->v_hash, val);
    KSO_DECREF(stro);
}


/* TYPE FUNCS */


TFUNC(dict, str) {
    #define SIG "dict.__str__(self)"
    REQ_N_ARGS(1);
    ks_dict self = (ks_dict)args[0];
    REQ_TYPE("self", self, ks_T_dict);

    if (self->n_items == 0) {
        return (kso)ks_str_new("{}");
    }

    ks_strB ksb = ks_strB_create();

    ks_strB_add(&ksb, "{", 1);

    int i, num = 0;
    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* ent = &self->buckets[i];
        if (ent->val != NULL) {
            if (num != 0) ks_strB_add(&ksb, ", ", 2);
            ks_strB_add_repr(&ksb, ent->key);
            ks_strB_add(&ksb, ": ", 2);
            ks_strB_add_tostr(&ksb, ent->val);
            num++;
        }
    }
    ks_strB_add(&ksb, "}", 1);

    return (kso)ks_strB_finish(&ksb);
    #undef SIG
}


TFUNC(dict, repr) {
    #define SIG "dict.__repr__(self)"
    REQ_N_ARGS(1);
    ks_dict self = (ks_dict)args[0];
    REQ_TYPE("self", self, ks_T_dict);

    if (self->n_items == 0) {
        return (kso)ks_str_new("{}");
    }

    ks_strB ksb = ks_strB_create();

    ks_strB_add(&ksb, "{", 1);

    int i, num = 0;
    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* ent = &self->buckets[i];
        if (ent->val != NULL) {
            if (num != 0) ks_strB_add(&ksb, ", ", 2);
            ks_strB_add_repr(&ksb, ent->key);
            ks_strB_add(&ksb, ": ", 2);
            ks_strB_add_tostr(&ksb, ent->val);
            num++;
        }
    }
    ks_strB_add(&ksb, "}", 1);

    return (kso)ks_strB_finish(&ksb);
    #undef SIG
}


TFUNC(dict, free) {
    #define SIG "dict.__free__(self)"
    REQ_N_ARGS(1);
    ks_dict self = (ks_dict)args[0];

    REQ_TYPE("self", self, ks_T_dict);


    int i;

    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* entry = &self->buckets[i];
        if (entry->val != NULL) {
            KSO_DECREF(entry->key);
            KSO_DECREF(entry->val);
        }
    }

    ks_free(self->buckets);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}


TFUNC(dict, getitem) {
    #define SIG "dict.__getitem__(self, key[, default])"
    REQ_N_ARGS_RANGE(2, 3);
    ks_dict self = (ks_dict)args[0];
    REQ_TYPE("self", self, ks_T_dict);
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
    #undef SIG
}

TFUNC(dict, setitem) {
    #define SIG "dict.__setitem__(self, key, val)"
    REQ_N_ARGS(3);
    ks_dict self = (ks_dict)args[0];
    REQ_TYPE("self", self, ks_T_dict);
    kso key = args[1], val = args[2];

    ks_dict_set(self, key, kso_hash(key), val);

    return KSO_NEWREF(val);
    #undef SIG
}





/* exporting functionality */

struct ks_type T_dict, *ks_T_dict = &T_dict;

void ks_init__dict() {

    /* first create the type */
    T_dict= (struct ks_type) {
        KSO_BASE_INIT(ks_T_type)

        .name = ks_str_new("dict"),

        .f_free = (kso)ks_cfunc_new(dict_free_),

        .f_str  = (kso)ks_cfunc_new(dict_str_),
        .f_repr = (kso)ks_cfunc_new(dict_repr_),

        .f_getitem = (kso)ks_cfunc_new(dict_getitem_),
        .f_setitem = (kso)ks_cfunc_new(dict_setitem_),

    };

}




