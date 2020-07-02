/* types/Enum.c - implementation of the enum base type
 *
 * TODO: make it able to create new enumerations
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_Enum);

static KS_TFUNC(Enum, getitem);

// make an enum name
static ks_Enum make_Enum_val(ks_type enumtype, int idx, ks_str name) {
    ks_Enum self = KS_ALLOC_OBJ(ks_Enum);
    KS_INIT_OBJ(self, enumtype);

    self->enum_idx = idx;
    self->name = (ks_str)KS_NEWREF(name);

    return self;
}

// construct an enum
ks_type ks_Enum_create_c(char* name, struct ks_enum_entry_c* ents) {

    ks_type enumtype = KS_ALLOC_OBJ(ks_type);
    KS_INIT_TYPE_OBJ(enumtype, name);

    // inherit from Enum class
    ks_type_add_parent(enumtype, ks_type_Enum);

    // add in lists of enum keys & indexes
    ks_list e_keys = ks_list_new(0, NULL), e_idxs = ks_list_new(0, NULL);
    ks_dict e_idx2mem = ks_dict_new(0, NULL);

    ks_type_set_cn(enumtype, (ks_dict_ent_c[]){
        {"_enum_keys",  (ks_obj)e_keys},
        {"_enum_idxs",  (ks_obj)e_idxs},
        {"_enum_idx2mem",  (ks_obj)e_idx2mem},
        {NULL, NULL}
    });

    int i = 0, next_idx = 0;
    while (ents[i].name) {
        // get requested idx
        int idx = ents[i].idx;
        if (idx < 0) {
            // automatic; claim the next one
            idx = next_idx++;
        } else if (idx >= next_idx) {
            // otherwise, ensure the next automatic is available
            next_idx = idx + 1;
        }

        // add integer index
        ks_int this_idx = ks_int_new(idx);
        ks_list_push(e_idxs, (ks_obj)this_idx);
        KS_DECREF(this_idx);

        // add string key
        ks_str this_key = ks_str_new(ents[i].name);
        ks_list_push(e_keys, (ks_obj)this_key);
        // add the enumeration value
        ks_Enum this_enum_val = make_Enum_val(enumtype, idx, this_key);
        KS_DECREF(this_key);

        // map it
        ks_dict_set(e_idx2mem, 0, (ks_obj)this_idx, (ks_obj)this_enum_val);

        ks_type_set(enumtype, this_key, (ks_obj)this_enum_val);
        KS_DECREF(this_enum_val);

        // increment ptr
        i++;
    }

    ks_obj p_get__base_ = (ks_obj)ks_cfunc_new2(Enum_getitem_, "Enum.__getitem__(self, item)");
    ks_pfunc p_get_ = ks_pfunc_new(p_get__base_);
    KS_DECREF(p_get__base_);
    ks_pfunc_fill(p_get_, 0, (ks_obj)enumtype);


    ks_type_set_cn(enumtype, (ks_dict_ent_c[]){
        {"__new__",  (ks_obj)p_get_},
        {NULL, NULL}
    });

    return enumtype;
}


ks_Enum ks_Enum_get_c(ks_type enumtype, char* arg) {
    return (ks_Enum)ks_type_get_c(enumtype, arg);
}

ks_Enum ks_Enum_get_i(ks_type enumtype, int arg) {
    ks_dict e_idx2mem = (ks_dict)ks_type_get_c(enumtype, "_enum_idx2mem");

    if (!e_idx2mem) return ks_throw_fmt(ks_type_KeyError, "Element '%i' was not in enumeration %S", arg, enumtype);
    else if (e_idx2mem->type != ks_type_dict) {
        KS_DECREF(e_idx2mem);
        return ks_throw_fmt(ks_type_InternalError, "Enum type (%S) ._enum_idx2mem was not a dict!", enumtype);
    }

    ks_int argi = ks_int_new(arg);
    ks_obj ret = ks_dict_get(e_idx2mem, 0, (ks_obj)argi);
    KS_DECREF(argi);
    KS_DECREF(e_idx2mem);
    if (!ret) {
        return ks_throw_fmt(ks_type_KeyError, "Element '%i' was not in enumeration %S", arg, enumtype);
    }

    return (ks_Enum)ret;
}


/* member functions */

// Enum.__getitem__(self, key) -> get a key
// Enum.__getitem__(enumtype, key) -> get a key
static KS_TFUNC(Enum, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_type enumtype = NULL;

    if (args[0]->type == ks_type_type && ks_type_issub((ks_type)args[0], ks_type_Enum)) {
        enumtype = (ks_type)args[0];
    } else if (ks_type_issub(args[0]->type, ks_type_Enum)) {
        enumtype = args[0]->type;
    } else {
        return ks_throw_fmt(ks_type_TypeError, "Expected 'self' to be either an enum object, or an enum type");
    }

    ks_str key = (ks_str)args[1];
    KS_REQ_TYPE(key, ks_type_str, "key");

    ks_obj res = ks_dict_get(enumtype->attr, key->v_hash, (ks_obj)key);
    if (!res) {
        KS_ERR_KEY(enumtype, key);
    } else return res;
};


// Enum.__new__(name, members) -> create a new enum type
static KS_TFUNC(Enum, new) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str name = (ks_str)args[0];
    KS_REQ_TYPE(name, ks_type_str, "name");
    ks_obj members = args[1];
    KS_REQ_ITERABLE(members, "members");

    // collect into a list
    ks_list members_list = ks_list_from_iterable(members);
    if (!members_list) return NULL;

    ks_type enumtype = KS_ALLOC_OBJ(ks_type);
    KS_INIT_TYPE_OBJ(enumtype, name->chr);

    // inherit from Enum class
    ks_type_add_parent(enumtype, ks_type_Enum);

    // add in lists of enum keys & indexes
    ks_list e_keys = ks_list_new(0, NULL), e_idxs = ks_list_new(0, NULL);

    ks_type_set_cn(enumtype, (ks_dict_ent_c[]){
        {"_enum_keys",  (ks_obj)e_keys},
        {"_enum_idxs",  (ks_obj)e_idxs},
        {NULL, NULL}
    });

    int i, next_idx = 0;
    for (i = 0; i < members_list->len; ++i) {
        ks_str this_key = (ks_str)members_list->elems[i];
        if (this_key->type != ks_type_str) {
            KS_DECREF(enumtype);
            KS_DECREF(members_list);
            return ks_throw_fmt(ks_type_TypeError, "All enumeration values must be of type 'str'!");
        }

        // ensure it isn't already taken
        ks_obj tmpget = ks_type_get(enumtype, this_key);
        if (tmpget != NULL) {
            ks_throw_fmt(ks_type_ArgError, "Enum cannot have member '%S'; it is already taken or an invalid name", this_key);
            KS_DECREF(tmpget);
            KS_DECREF(enumtype);
            KS_DECREF(members_list);
            return NULL;
        }

        int idx = i;

        // add integer index
        ks_int this_idx = ks_int_new(idx);
        ks_list_push(e_idxs, (ks_obj)this_idx);
        KS_DECREF(this_idx);

        // add string key
        ks_list_push(e_keys, (ks_obj)this_key);
        // add the enumeration value
        ks_Enum this_enum_val = make_Enum_val(enumtype, idx, this_key);
        KS_DECREF(this_key);

        ks_type_set(enumtype, this_key, (ks_obj)this_enum_val);
        KS_DECREF(this_enum_val);

    }
    KS_DECREF(members_list);

    // create constructor as the getattr
    ks_obj p_get__base_ = (ks_obj)ks_cfunc_new2(Enum_getitem_, "Enum.get(key)");
    ks_pfunc p_get_ = ks_pfunc_new(p_get__base_);
    KS_DECREF(p_get__base_);
    ks_pfunc_fill(p_get_, 0, (ks_obj)enumtype);

    ks_type_set_c(enumtype, "get", (ks_obj)p_get_);
    KS_DECREF(p_get_);

    return (ks_obj)enumtype;
};

// Enum.__str__(self) -> create a string representing it
static KS_TFUNC(Enum, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Enum self = (ks_Enum)args[0];
    KS_REQ_TYPE(self, ks_type_Enum, "self");

    return (ks_obj)ks_fmt_c("%S.%S", self->type->__name__, self->name);
};


// Enum.__free__(self) -> frees resources with the enum
static KS_TFUNC(Enum, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Enum self = (ks_Enum)args[0];
    KS_REQ_TYPE(self, ks_type_Enum, "self");

    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


/* math ops */

// Enum.__hash__(self) -> hash an enum value
static KS_TFUNC(Enum, hash) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_Enum self = (ks_Enum)args[0];
    KS_REQ_TYPE(self, ks_type_Enum, "self");

    if (self->enum_idx == 0) {
        return (ks_obj)ks_int_new(1);
    } else {
        return (ks_obj)ks_int_new(self->enum_idx);
    }
};



static KS_TFUNC(Enum, add) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_add(args[0], args[1]);
};
static KS_TFUNC(Enum, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_sub(args[0], args[1]);
};
static KS_TFUNC(Enum, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_mul(args[0], args[1]);
};
static KS_TFUNC(Enum, div) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_div(args[0], args[1]);
};
static KS_TFUNC(Enum, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_mod(args[0], args[1]);
};
static KS_TFUNC(Enum, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_pow(args[0], args[1]);
};

static KS_TFUNC(Enum, binor) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_binor(args[0], args[1]);
};
static KS_TFUNC(Enum, binand) {
    KS_REQ_N_ARGS(n_args, 2);
    return ks_num_binand(args[0], args[1]);
};

// initialize Enum type
void ks_type_Enum_init() {
    KS_INIT_TYPE_OBJ(ks_type_Enum, "Enum");

    ks_obj p_get__base_ = (ks_obj)ks_cfunc_new2(Enum_getitem_, "Enum.get(key)");
    ks_pfunc p_get_ = ks_pfunc_new(p_get__base_);
    KS_DECREF(p_get__base_);
    ks_pfunc_fill(p_get_, 0, (ks_obj)ks_type_Enum);

    ks_type_set_cn(ks_type_Enum, (ks_dict_ent_c[]){
        {"create",       (ks_obj)ks_cfunc_new2(Enum_new_, "Enum.__new__()")},
        {"__free__",     (ks_obj)ks_cfunc_new2(Enum_free_, "Enum.__free__(self)")},

        {"__str__",      (ks_obj)ks_cfunc_new2(Enum_str_, "Enum.__str__(self)")},
        {"__repr__",     (ks_obj)ks_cfunc_new2(Enum_str_, "Enum.__repr__(self)")},

        {"__getitem__",  (ks_obj)ks_cfunc_new2(Enum_getitem_, "Enum.__getitem__(self, key)")},
        {"get",          (ks_obj)p_get_},

        {"__hash__",     (ks_obj)ks_cfunc_new2(Enum_hash_, "Enum.__hash__(self)")},


        {"__add__",      (ks_obj)ks_cfunc_new2(Enum_add_, "Enum.__add__(L, R)")},
        {"__sub__",      (ks_obj)ks_cfunc_new2(Enum_sub_, "Enum.__sub__(L, R)")},
        {"__mul__",      (ks_obj)ks_cfunc_new2(Enum_mul_, "Enum.__mul__(L, R)")},
        {"__div__",      (ks_obj)ks_cfunc_new2(Enum_div_, "Enum.__div__(L, R)")},
        {"__mod__",      (ks_obj)ks_cfunc_new2(Enum_mod_, "Enum.__mod__(L, R)")},
        {"__pow__",      (ks_obj)ks_cfunc_new2(Enum_pow_, "Enum.__pow__(L, R)")},
        {"__binor__",    (ks_obj)ks_cfunc_new2(Enum_binor_,"Enum.__binor__(L, R)")},
        {"__binand__",   (ks_obj)ks_cfunc_new2(Enum_binand_, "Enum.__binand__(L, R)")},


        /*

        {"__cmp__",      (ks_obj)ks_cfunc_new2(Enum_cmp_, "Enum.__cmp__(L, R)")},
        {"__lt__",       (ks_obj)ks_cfunc_new2(Enum_lt_, "Enum.__lt__(L, R)")},
        {"__le__",       (ks_obj)ks_cfunc_new2(Enum_le_, "Enum.__le__(L, R)")},
        {"__gt__",       (ks_obj)ks_cfunc_new2(Enum_gt_, "Enum.__gt__(L, R)")},
        {"__ge__",       (ks_obj)ks_cfunc_new2(Enum_ge_, "Enum.__ge__(L, R)")},
        {"__eq__",       (ks_obj)ks_cfunc_new2(Enum_eq_, "Enum.__eq__(L, R)")},
        {"__ne__",       (ks_obj)ks_cfunc_new2(Enum_ne_, "Enum.__ne__(L, R)")},
        */

        {NULL, NULL}   
    });
}

