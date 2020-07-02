/* types/namespace.c - namespace class implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"

// forward declare it
KS_TYPE_DECLFWD(ks_type_namespace);

// create a kscript dictionary from entries
ks_namespace ks_namespace_new(ks_dict attr) {
    ks_namespace self = KS_ALLOC_OBJ(ks_namespace);
    KS_INIT_OBJ(self, ks_type_namespace);

    self->attr = attr ? (ks_dict)KS_NEWREF(attr) : ks_dict_new(0, NULL);

    return self;
}

/* member functions */

// namespace.__new__(attr) -> create a new namespace
static KS_TFUNC(namespace, new) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    KS_REQ_TYPE(args[0], ks_type_dict, "attr");
    return (ks_obj)ks_namespace_new((ks_dict)args[0]);
};

// namespace.__getitem__(self, key) -> get an entry
static KS_TFUNC(namespace, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");
    ks_obj obj = args[1];
    // get the hash
    ks_hash_t hash_obj;// = obj->type == ks_type_str ? ((ks_str)obj)->v_hash : ks_hash(obj);

    if (!ks_hash(obj, &hash_obj)) {
        return NULL;
    }

    if (hash_obj == 0) {
        // special value meaning unhashable
        return ks_throw_fmt(ks_type_Error, "'%T' was not hashable!", obj);
    }


    ks_obj res = ks_dict_get(self->attr, hash_obj, obj);

    // throw error if it didnt exist
    if (!res) KS_ERR_KEY(self, obj);

    return res;
};

// namespace.__getattr__(self, attr) -> get an entry
static KS_TFUNC(namespace, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");
    ks_obj obj = args[1];
    // get the hash
    ks_hash_t hash_obj;// = obj->type == ks_type_str ? ((ks_str)obj)->v_hash : ks_hash(obj);

    if (!ks_hash(obj, &hash_obj)) {
        return NULL;
    }

    if (hash_obj == 0) {
        // special value meaning unhashable
        return ks_throw_fmt(ks_type_Error, "'%T' was not hashable!", obj);
    }

    ks_obj res = ks_dict_get(self->attr, hash_obj, obj);

    // throw error if it didnt exist
    if (!res && obj->type == ks_type_str) {
        // try and get it some other way

        // try type(obj).attr as a function with 'obj' filled in as the first argument
        ks_obj type_attr = ks_type_get(self->type, (ks_str)obj);
        if (!type_attr) KS_ERR_ATTR(self, obj);

        // make sure it is a member function
        if (!ks_is_callable(type_attr)) {
            KS_DECREF(type_attr);
            KS_ERR_ATTR(self, obj);
        }

        // now, create a partial function
        ks_pfunc ret = ks_pfunc_new(type_attr);
        KS_DECREF(type_attr);

        // fill #0 as 'self' (aka obj)
        ks_pfunc_fill(ret, 0, (ks_obj)self);

        // return the member function
        return (ks_obj)ret;
    }

    return res;
};



// namespace.__setitem__(self, key, val) -> set an entry
static KS_TFUNC(namespace, setitem) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");
    ks_obj obj = args[1];
    // get the hash
    ks_hash_t hash_obj;// = obj->type == ks_type_str ? ((ks_str)obj)->v_hash : ks_hash(obj);

    if (!ks_hash(obj, &hash_obj)) {
        return NULL;
    }

    if (hash_obj == 0) {
        // special value meaning unhashable
        return ks_throw_fmt(ks_type_Error, "'%T' was not hashable!", obj);
    }

    ks_obj val = args[2];


    // set value
    ks_dict_set(self->attr, hash_obj, obj, val);

    // just return the value
    return KS_NEWREF(val);
};

// namespace.__iter__(self) -> return an iterator for a dictionary
static KS_TFUNC(namespace, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");

    return ks_F_iter->func(1, (ks_obj[]){ (ks_obj)self->attr });
};


// namespace.keys(self) -> return a list of keys
static KS_TFUNC(namespace, keys) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");

    ks_list ret = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < self->attr->n_entries; ++i) {
        if (self->attr->entries[i].hash != 0 && self->attr->entries[i].val != NULL) {
            ks_list_push(ret, self->attr->entries[i].key);
        }
    }

    return (ks_obj)ret;
};



// namespace.__str__(self) -> convert to string
static KS_TFUNC(namespace, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");

    ks_str_b SB;
    ks_str_b_init(&SB);

    ks_str_b_add(&SB, 1, "{");

    int i;
    for (i = 0; i < self->attr->n_entries; ++i) {
        if (self->attr->entries[i].hash != 0) {
            if (i > 0 && i < self->attr->n_entries) ks_str_b_add(&SB, 2, ", ");

            // add the item
            ks_str_b_add_str(&SB, self->attr->entries[i].key);
            ks_str_b_add(&SB, 2, "=");
            ks_str_b_add_repr(&SB, self->attr->entries[i].val);
        }
    }

    ks_str_b_add(&SB, 1, "}");

    ks_str ret = ks_str_b_get(&SB);

    ks_str_b_free(&SB);

    return (ks_obj)ret;
};

// namespace.__free__(self) -> free resources
static KS_TFUNC(namespace, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_type_namespace, "self");

    KS_DECREF(self->attr)

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};




// initialize namespace type
void ks_type_namespace_init() {
    KS_INIT_TYPE_OBJ(ks_type_namespace, "namespace");

    ks_type_set_cn(ks_type_namespace, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(namespace_new_, "namespace.__new__(self, *keyvals)")},
        {"__str__", (ks_obj)ks_cfunc_new2(namespace_str_, "namespace.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(namespace_str_, "namespace.__repr__(self)")},
        {"__free__", (ks_obj)ks_cfunc_new2(namespace_free_, "namespace.__free__(self)")},

        {"__getattr__", (ks_obj)ks_cfunc_new2(namespace_getattr_, "namespace.__getattr__(self, attr)")},

        {"__getitem__", (ks_obj)ks_cfunc_new2(namespace_getitem_, "namespace.__getitem__(self, key)")},
        {"__setitem__", (ks_obj)ks_cfunc_new2(namespace_setitem_, "namespace.__setitem__(self, key, val)")},
        
        {"__iter__", (ks_obj)ks_cfunc_new2(namespace_iter_, "namespace.__iter__(self)")},

        {"keys", (ks_obj)ks_cfunc_new2(namespace_keys_, "namespace.keys(self)")},

        {NULL, NULL}   
    });

}

