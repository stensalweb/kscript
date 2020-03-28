/* types/type.c - implementation of the 'type' type, which represents abstract types
 *
 * I.e. `type(type(x))` should return 'type'
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_type);


// initialize a type
void ks_init_type(ks_type self, char* name) {
    // zero it out
    memset(self, 0, sizeof(*self));
    KS_INIT_OBJ(self, ks_type_type);

    // initialize the attribute dictionary
    self->attr = ks_dict_new(0, NULL);
    // initialize all other to empty
    // set some defaults

    ks_str name_str = ks_str_new(name);
    ks_type_set_c(self, "__name__",(ks_obj)name_str);
    KS_DECREF(name_str);

    ks_list parents_list = ks_list_new(0, NULL);
    ks_type_set_c(self, "__parents__", (ks_obj)parents_list);
    KS_DECREF(parents_list);

}


// check if 'self' is a sub type of 'of'
bool ks_type_issub(ks_type self, ks_type of) {
    if (self == of) return true;

    int i;
    for (i = 0; i < self->__parents__->len; ++i) {
        // check if it is a parent
        if (ks_type_issub((ks_type)self->__parents__->elems[i], of)) return true;
    }

    // not a sub type
    return false;

}

// get an attribute
ks_obj ks_type_get(ks_type self, ks_str key) {
    assert(key->type == ks_type_str);
    ks_hash_t hash = key->v_hash;
    assert(hash != 0);

    // special case to avoid circular references
    if (key->len == 8 && strncmp(key->chr, "__dict__", 8) == 0) return KS_NEWREF(self->attr);

    // get from the internal dictionary
    return ks_dict_get(self->attr, hash, (ks_obj)key);
}


// return a memberfunction of self.attr(obj, *)
ks_obj ks_type_get_mf(ks_type self, ks_str attr, ks_obj obj) {
    ks_obj sa = ks_type_get(self, attr);
    if (!sa) return NULL;

    // ensure it is callable
    if (!ks_is_callable(sa)) {
        KS_DECREF(sa);
        return NULL;
    }

    // create a partial function
    ks_pfunc ret = ks_pfunc_new(sa);
    KS_DECREF(sa);

    // fill in #0 as an argument
    ks_pfunc_fill(ret, 0, obj);

    return (ks_obj)ret;
}

// set a given attribute
void ks_type_set(ks_type self, ks_str key, ks_obj val) {
    assert(key->type == ks_type_str);

    ks_hash_t hash = key->v_hash;
    assert(hash != 0);


    if (key->len > 2 && key->chr[0] == '_' && key->chr[1] == '_') {
        // we detect if it is one of the builtins, and also keep track of that as the type value
        // Don't hold a reference here, since we set the dictionary anyway

        #define ATTR_CASE_TYPE(_name, _memb, _cast, _type) else if (((sizeof(_name) - 1) == key->len) && strncmp(_name, key->chr, (sizeof(_name) - 1)) == 0) { \
            if (val->type != _type) { /* error: unsupported type, also need to account for sub-types */ } \
            self->_memb = (_cast)val; \
        }

        #define ATTR_CASE(_name, _memb) else if (((sizeof(_name) - 1) == key->len) && strncmp(_name, key->chr, (sizeof(_name) - 1)) == 0) { \
            self->_memb = val; \
        }

        // dummy so the 'ATTR_CASE' can stack
        if (false) ;
        ATTR_CASE_TYPE("__name__", __name__, ks_str, ks_type_str)
        ATTR_CASE_TYPE("__parents__", __parents__, ks_list, ks_type_list)

        ATTR_CASE("__new__", __new__)
        ATTR_CASE("__init__", __init__)

        ATTR_CASE("__str__", __str__)
        ATTR_CASE("__repr__", __repr__)

        ATTR_CASE("__len__", __len__)

        ATTR_CASE("__getattr__", __getattr__)
        ATTR_CASE("__setattr__", __setattr__)

        ATTR_CASE("__getitem__", __getitem__)
        ATTR_CASE("__setitem__", __setitem__)

        ATTR_CASE("__iter__", __iter__)
        ATTR_CASE("__next__", __next__)

        ATTR_CASE("__call__", __call__)
        
        ATTR_CASE("__free__", __free__)

        ATTR_CASE("__add__", __add__)
        ATTR_CASE("__sub__", __sub__)
        ATTR_CASE("__mul__", __mul__)
        ATTR_CASE("__div__", __div__)
        ATTR_CASE("__mod__", __mod__)
        ATTR_CASE("__pow__", __pow__)

        ATTR_CASE("__lt__", __lt__)
        ATTR_CASE("__le__", __le__)
        ATTR_CASE("__gt__", __gt__)
        ATTR_CASE("__ge__", __ge__)
        ATTR_CASE("__eq__", __eq__)
        ATTR_CASE("__ne__", __ne__)

        ATTR_CASE("__neg__", __neg__)
        ATTR_CASE("__sqig__", __sqig__)

        ATTR_CASE("__hash__", __hash__)


        else {
            // some unknown builtin, or just a varible with 2 underscores
        }

    }

    // actually set the internal dictionary
    ks_dict_set(self->attr, hash, (ks_obj)key, val);

}

// set an attribute by a C-style string
void ks_type_set_c(ks_type self, char* key, ks_obj val) {

    // create a temporary kscript object to attempt to set it
    ks_str str_key = ks_str_new(key);
    ks_type_set(self, str_key, val);
    KS_DECREF(str_key);

}
// set C entries
int ks_type_set_cn(ks_type self, ks_dict_ent_c* ent_cns) {
    int res = 0;
    while (ent_cns && ent_cns->key != NULL) {
        
        ks_type_set_c(self, ent_cns->key, ent_cns->val);

        // delete reference
        KS_DECREF(ent_cns->val);

        // next one
        ent_cns++;
    }

    return res;
}

// add a parent to the type
void ks_type_add_parent(ks_type self, ks_type parent) {

    int i;
    // check if it already exists
    for (i = 0; i < self->__parents__->len; ++i) {
        if ((ks_type)self->__parents__->elems[i] == parent) return;
    }

    // otherwise, append it to the list
    ks_list_push(self->__parents__, (ks_obj)parent);

    // and set other variables
    #define REPL(_x) if (self->_x == NULL) self->_x = parent->_x;

    REPL(__new__)
    REPL(__init__)

    REPL(__add__)
    REPL(__sub__)
    REPL(__mul__)
    REPL(__div__)
    REPL(__mod__)
    REPL(__pow__)

    REPL(__lt__)
    REPL(__le__)
    REPL(__gt__)
    REPL(__ge__)
    REPL(__eq__)
    REPL(__ne__)

    REPL(__neg__)
    REPL(__sqig__)

    REPL(__getattr__)
    REPL(__setattr__)

    REPL(__getitem__)
    REPL(__setitem__)

    REPL(__iter__)
    REPL(__next__)

    REPL(__call__)
    REPL(__free__)
    REPL(__repr__)
    REPL(__str__)
    REPL(__hash__)
    REPL(__len__)

}


/* member functions */

// type.__str__(self) -> convert to string
static KS_TFUNC(type, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_type self = (ks_type)args[0];
    KS_REQ_TYPE(self, ks_type_type, "self");
    
    return KS_NEWREF(self->__name__);
};

// type.__getattr__(self, attr) -> get an attribute
static KS_TFUNC(type, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type self = (ks_type)args[0];
    KS_REQ_TYPE(self, ks_type_type, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");

    ks_obj ret = ks_type_get(self, attr);
    if (!ret) KS_ERR_ATTR(self, attr);

    return ret;
};

// type.__setattr__(self, attr, val) -> set an attribute
static KS_TFUNC(type, setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_type self = (ks_type)args[0];
    KS_REQ_TYPE(self, ks_type_type, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");
    ks_obj val = args[2];

    ks_type_set(self, attr, val);

    return KSO_NONE;
};

// compare 2 types, 0 if they are the same,
// >0 if L is a sub type of R, or <0 if R is a sub type if R
// returns '2' if neither was directly related
static int type__cmp(ks_type L, ks_type R) {
    if (L == R) return 0;
    if (ks_type_issub(L, R)) return 1;
    if (ks_type_issub(R, L)) return -1;

    // special case
    return 2;
}

// type.__lt__(L, R) -> get parenting
static KS_TFUNC(type, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_type && R->type == ks_type_type) {
        int tcmp = type__cmp((ks_type)L, (ks_type)R);
        return KSO_BOOL(tcmp != 2 && tcmp < 0);
    }

    KS_ERR_BOP_UNDEF("<", L, R);
};

// type.__le__(L, R) -> get parenting
static KS_TFUNC(type, le) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_type && R->type == ks_type_type) {
        int tcmp = type__cmp((ks_type)L, (ks_type)R);
        return KSO_BOOL(tcmp != 2 && tcmp <= 0);
    }

    KS_ERR_BOP_UNDEF("<=", L, R);
};

// type.__gt__(L, R) -> get parenting
static KS_TFUNC(type, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_type && R->type == ks_type_type) {
        int tcmp = type__cmp((ks_type)L, (ks_type)R);
        return KSO_BOOL(tcmp != 2 && tcmp > 0);
    }

    KS_ERR_BOP_UNDEF(">", L, R);
};

// type.__ge__(L, R) -> get parenting
static KS_TFUNC(type, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_type && R->type == ks_type_type) {
        int tcmp = type__cmp((ks_type)L, (ks_type)R);
        return KSO_BOOL(tcmp != 2 && tcmp >= 0);
    }

    KS_ERR_BOP_UNDEF(">=", L, R);
};


// type.__eq__(L, R) -> get parenting
static KS_TFUNC(type, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_type && R->type == ks_type_type) {
        return KSO_BOOL(L == R);
    }

    KS_ERR_BOP_UNDEF("==", L, R);
};

// type.__ne__(L, R) -> get parenting
static KS_TFUNC(type, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_type && R->type == ks_type_type) {
        return KSO_BOOL(L != R);
    }

    KS_ERR_BOP_UNDEF("!=", L, R);
};





// initialize type type
void ks_type_type_init() {
    KS_INIT_TYPE_OBJ(ks_type_type, "type");

    ks_type_set_cn(ks_type_type, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(type_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(type_str_)},

        {"__getattr__", (ks_obj)ks_cfunc_new(type_getattr_)},
        {"__setattr__", (ks_obj)ks_cfunc_new(type_setattr_)},

        {"__lt__", (ks_obj)ks_cfunc_new(type_lt_)},
        {"__le__", (ks_obj)ks_cfunc_new(type_le_)},
        {"__gt__", (ks_obj)ks_cfunc_new(type_gt_)},
        {"__ge__", (ks_obj)ks_cfunc_new(type_ge_)},
        {"__eq__", (ks_obj)ks_cfunc_new(type_eq_)},
        {"__ne__", (ks_obj)ks_cfunc_new(type_ne_)},

        {NULL, NULL}   
    });
}

