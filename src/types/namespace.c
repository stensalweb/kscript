/* types/namespace.c - namespace class implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// create a kscript dictionary from entries
ks_namespace ks_namespace_new(ks_dict attr) {
    ks_namespace self = KS_ALLOC_OBJ(ks_namespace);
    KS_INIT_OBJ(self, ks_T_namespace);

    self->attr = attr ? (ks_dict)KS_NEWREF(attr) : ks_dict_new(0, NULL);

    return self;
}

/* member functions */

// namespace.__new__(typ, attr={}) -> create a new namespace
static KS_TFUNC(namespace, new) {
    ks_type typ;
    ks_dict attr = NULL;
    KS_GETARGS("typ:* ?attr:*", &typ, ks_T_type, &attr, ks_T_dict)

    return (ks_obj)ks_namespace_new(attr);
}

// namespace.__getitem__(self, key) -> get an entry
static KS_TFUNC(namespace, getitem) {
    ks_namespace self;
    ks_str key;
    KS_GETARGS("self:* key:*", &self, ks_T_namespace, &key, ks_T_str)

    return ks_dict_get_h(self->attr, (ks_obj)key, key->v_hash);
}

// namespace.__getattr__(self, attr) -> get an entry
static KS_TFUNC(namespace, getattr) {
    ks_namespace self;
    ks_str key;
    KS_GETARGS("self:* key:*", &self, ks_T_namespace, &key, ks_T_str)

    return ks_dict_get_h(self->attr, (ks_obj)key, key->v_hash);
}



// namespace.__setitem__(self, key, val) -> set an entry
static KS_TFUNC(namespace, setitem) {
    ks_namespace self;
    ks_str key;
    ks_obj val;
    KS_GETARGS("self:* key:* val", &self, ks_T_namespace, &key, ks_T_str, &val)

    ks_dict_set_h(self->attr, (ks_obj)key, key->v_hash, val);

    return KS_NEWREF(val);
}


/*

// namespace.__iter__(self) -> return an iterator for a dictionary
static KS_TFUNC(namespace, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_T_namespace, "self");

    return ks_F_iter->func(1, (ks_obj[]){ (ks_obj)self->attr });
};


// namespace.keys(self) -> return a list of keys
static KS_TFUNC(namespace, keys) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_namespace self = (ks_namespace)args[0];
    KS_REQ_TYPE(self, ks_T_namespace, "self");

    ks_list ret = ks_list_new(0, NULL);

    int i;
    for (i = 0; i < self->attr->n_entries; ++i) {
        if (self->attr->entries[i].hash != 0 && self->attr->entries[i].val != NULL) {
            ks_list_push(ret, self->attr->entries[i].key);
        }
    }

    return (ks_obj)ret;
};

*/


// namespace.__str__(self) -> convert to string
static KS_TFUNC(namespace, str) {
    ks_namespace self;
    KS_GETARGS("self:*", &self, ks_T_namespace)

    return (ks_obj)ks_fmt_c("namespace(%R)", self->attr);
};



// namespace.__free__(self) -> free resources
static KS_TFUNC(namespace, free) {
    ks_namespace self;
    KS_GETARGS("self:*", &self, ks_T_namespace)

    KS_DECREF(self->attr)

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};



// forward declare it
KS_TYPE_DECLFWD(ks_T_namespace);

void ks_init_T_namespace() {
    ks_type_init_c(ks_T_namespace, "namespace", ks_T_obj, KS_KEYVALS(
        {"__new__", (ks_obj)ks_cfunc_new_c(namespace_new_, "namespace.__new__(typ, attr={})")},
        {"__free__", (ks_obj)ks_cfunc_new_c(namespace_free_, "namespace.__free__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new_c(namespace_str_, "namespace.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new_c(namespace_str_, "namespace.__repr__(self)")},

        {"__getattr__", (ks_obj)ks_cfunc_new_c(namespace_getattr_, "namespace.__getattr__(self, attr)")},

        {"__getitem__", (ks_obj)ks_cfunc_new_c(namespace_getitem_, "namespace.__getitem__(self, key)")},
        {"__setitem__", (ks_obj)ks_cfunc_new_c(namespace_setitem_, "namespace.__setitem__(self, key, val)")},
        
       // {"__iter__", (ks_obj)ks_cfunc_new_c(namespace_iter_, "namespace.__iter__(self)")},

        //{"keys", (ks_obj)ks_cfunc_new_c(namespace_keys_, "namespace.keys(self)")},
    ));

}

