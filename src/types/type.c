/* type.c - implementation of the 'type' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// set all defaults to the appropriate values
static void my_setall(ks_type self, ks_type parent) {

    // default the flags
    self->flags = KS_TYPE_FLAGS_EQSS;

    // macro to handle the special case attributes
    #define SPEC_CASE(_name) self->_name = parent == NULL ? NULL : parent->_name;

    SPEC_CASE(__name__)
    SPEC_CASE(__parents__)
    SPEC_CASE(__new__)
    SPEC_CASE(__init__)
    SPEC_CASE(__free__)
    SPEC_CASE(__bool__)
    SPEC_CASE(__int__)
    SPEC_CASE(__float__)
    SPEC_CASE(__str__)
    SPEC_CASE(__bytes__)
    SPEC_CASE(__fmt__)
    SPEC_CASE(__getattr__)
    SPEC_CASE(__setattr__)
    SPEC_CASE(__getitem__)
    SPEC_CASE(__setitem__)
    SPEC_CASE(__repr__)
    SPEC_CASE(__len__)
    SPEC_CASE(__hash__)
    SPEC_CASE(__iter__)
    SPEC_CASE(__next__)
    SPEC_CASE(__call__)
    SPEC_CASE(__pos__)
    SPEC_CASE(__neg__)
    SPEC_CASE(__abs__)
    SPEC_CASE(__sqig__)
    SPEC_CASE(__add__)
    SPEC_CASE(__sub__)
    SPEC_CASE(__mul__)
    SPEC_CASE(__div__)
    SPEC_CASE(__mod__)
    SPEC_CASE(__pow__)
    SPEC_CASE(__lt__)
    SPEC_CASE(__gt__)
    SPEC_CASE(__le__)
    SPEC_CASE(__ge__)
    SPEC_CASE(__eq__)
    SPEC_CASE(__ne__)
    SPEC_CASE(__cmp__)
    SPEC_CASE(__lshift__)
    SPEC_CASE(__rshift__)
    SPEC_CASE(__binor__)
    SPEC_CASE(__binand__)
    SPEC_CASE(__binxor__)

    #undef SPEC_CASE

}

// Construct a new 'type' object, where `parent` can be any type that it will implement the same binary interface as
//   (giving NULL and ks_T_object are equivalent)
// NOTE: Returns new reference, or NULL if an error was thrown
ks_type ks_type_new(ks_str name, ks_type parent) {
    ks_type self = KS_ALLOC_OBJ(ks_type);
    KS_INIT_OBJ(self, ks_T_type);

    my_setall(self, parent);

    self->attr = ks_dict_new(0, NULL);
    ks_list pars = ks_list_new(1, (ks_obj[]){ (ks_obj)parent });
    ks_type_set_c(self, KS_KEYVALS({"__name__", KS_NEWREF(name)}, {"__parents__", (ks_obj)pars}, ));

    return self;
}

// Initialize a C-defined type
void ks_type_init_c(ks_type self, const char* name, ks_type parent, ks_keyval_c* keyvals) {

    // standard initialization
    KS_INIT_OBJ(self, ks_T_type);

    my_setall(self, parent);

    // set attributes
    self->attr = ks_dict_new(0, NULL);

    // set the name
    ks_str newname = ks_str_new_c(name, -1);
    ks_list pars = ks_list_new(self == parent ? 0 : 1, (ks_obj[]){ (ks_obj)parent });
    ks_type_set_c(self, KS_KEYVALS({"__name__", (ks_obj)newname}, {"__parents__", (ks_obj)pars}, ));

    ks_type_set_c(self, keyvals);

}

// get an attribute
ks_obj ks_type_get(ks_type self, ks_str key) {
    ks_hash_t hash = key->v_hash;

    // special case to avoid circular references
    //if (key->len == 8 && strncmp(key->chr, "__dict__", 8) == 0) return KS_NEWREF(self->attr);

    // get from the internal dictionary
    ks_obj res = ks_dict_get_h(self->attr, (ks_obj)key, hash);
    if (!res) {
        int i;
        for (i = 0; i < self->__parents__->len; ++i) {
            ks_type cpar = (ks_type)self->__parents__->elems[i];
            ks_obj tr = ks_type_get(cpar, key);
            if (tr != NULL) return tr;
        }

        // nothing found
        return NULL;
    } else {
        // just return the result

        return res;
    }

}

// Set a given key,val pair to a type, returning success
// NOTE: Returns success
bool ks_type_set(ks_type self, ks_str key, ks_obj val) {
    assert (key->type == ks_T_str && "setting type attribute that was not a string");

    // get hash
    ks_hash_t hash = key->v_hash;


    // double underscore attributes
    if (key->len_b > 4 && key->chr[0] == '_' && key->chr[1] == '_') {

        // helper macro for given special keys
        #define KEYCASE(_name, _totype, _type) else if ((sizeof(#_name) - 1) == key->len_b && strncmp(#_name, key->chr, sizeof(#_name) - 1) == 0) { \
            if (!ks_type_issub(val->type, _type)) { ks_throw(ks_T_InternalError, "Set '" #_name "' to a '%T', where it should have been of type '%S'", val->type, _type); return false; } \
            self->_name = (_totype)val; \
        }
        /**/ if (false) {}

        KEYCASE(__name__, ks_str, ks_T_str)
        KEYCASE(__parents__, ks_list, ks_T_list)

        KEYCASE(__new__, ks_obj, ks_T_object)
        KEYCASE(__init__, ks_obj, ks_T_object)
        KEYCASE(__free__, ks_obj, ks_T_object)
        KEYCASE(__bool__, ks_obj, ks_T_object)
        KEYCASE(__int__, ks_obj, ks_T_object)
        KEYCASE(__float__, ks_obj, ks_T_object)
        KEYCASE(__str__, ks_obj, ks_T_object)
        KEYCASE(__bytes__, ks_obj, ks_T_object)
        KEYCASE(__fmt__, ks_obj, ks_T_object)
        KEYCASE(__getattr__, ks_obj, ks_T_object)
        KEYCASE(__setattr__, ks_obj, ks_T_object)
        KEYCASE(__getitem__, ks_obj, ks_T_object)
        KEYCASE(__setitem__, ks_obj, ks_T_object)
        KEYCASE(__repr__, ks_obj, ks_T_object)
        KEYCASE(__len__, ks_obj, ks_T_object)
        KEYCASE(__hash__, ks_obj, ks_T_object)
        KEYCASE(__iter__, ks_obj, ks_T_object)
        KEYCASE(__next__, ks_obj, ks_T_object)
        KEYCASE(__call__, ks_obj, ks_T_object)
        KEYCASE(__pos__, ks_obj, ks_T_object)
        KEYCASE(__neg__, ks_obj, ks_T_object)
        KEYCASE(__abs__, ks_obj, ks_T_object)
        KEYCASE(__sqig__, ks_obj, ks_T_object)
        KEYCASE(__add__, ks_obj, ks_T_object)
        KEYCASE(__sub__, ks_obj, ks_T_object)
        KEYCASE(__mul__, ks_obj, ks_T_object)
        KEYCASE(__div__, ks_obj, ks_T_object)
        KEYCASE(__mod__, ks_obj, ks_T_object)
        KEYCASE(__pow__, ks_obj, ks_T_object)
        KEYCASE(__lt__, ks_obj, ks_T_object)
        KEYCASE(__gt__, ks_obj, ks_T_object)
        KEYCASE(__le__, ks_obj, ks_T_object)
        KEYCASE(__ge__, ks_obj, ks_T_object)
        KEYCASE(__eq__, ks_obj, ks_T_object)
        KEYCASE(__ne__, ks_obj, ks_T_object)
        KEYCASE(__cmp__, ks_obj, ks_T_object)
        KEYCASE(__lshift__, ks_obj, ks_T_object)
        KEYCASE(__rshift__, ks_obj, ks_T_object)
        KEYCASE(__binor__, ks_obj, ks_T_object)
        KEYCASE(__binand__, ks_obj, ks_T_object)
        KEYCASE(__binxor__, ks_obj, ks_T_object)

        else {
            // unknown double underscore; ignore it
        }

        #undef KEYCASE
    }

    // actually set the internal dictionary
    return ks_dict_set_h(self->attr, (ks_obj)key, hash, val);
}

// Set the given elements from C-style strings
// NOTE: Returns success
bool ks_type_set_c(ks_type self, ks_keyval_c* keyvals) {
    // return status
    bool rst = true;

    ks_size_t i = 0;
    // iterate through keys
    while (keyvals[i].key != NULL) {
        ks_obj val = keyvals[i].val;

        if (rst) {
            // we need to continue creating the dictionary
            ks_str key = ks_str_new_c(keyvals[i].key, -1);
            if (!ks_type_set(self, key, val)) {
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

// calculate whether 'self' is a sub type of 'of'
bool ks_type_issub(ks_type self, ks_type of) {
    // some special cases that are always true
    if (self == of || of == NULL || of == ks_T_object) return true;

    ks_size_t i;
    for (i = 0; i < self->__parents__->len; ++i) {
        ks_type this_par = (ks_type)self->__parents__->elems[i];
        if (this_par != self && ks_type_issub(this_par, of)) return true;
    }

    return false;
}


// type.__new__(x) - special case - return the type of self (doesn't construct a new type)
static KS_TFUNC(type, new) {
    ks_obj x;
    KS_GETARGS("x", &x)

    return KS_NEWREF(x->type);
}


// type.__free__(self) - free obj
static KS_TFUNC(type, free) {
    ks_type self;
    KS_GETARGS("self:*", &self, ks_T_type)

    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);


    return KSO_NONE;
}


// type.__getattr__(self, attr) - get atrtibute
static KS_TFUNC(type, getattr) {
    ks_type self;
    ks_str attr;
    KS_GETARGS("self:* attr:*", &self, ks_T_type, &attr, ks_T_str)

    ks_obj ret = ks_type_get(self, attr);
    if (!ret) {
        KS_THROW_ATTR_ERR(self, attr);
    } else {
        return ret;
    }
}


// type.__str__(self) - turn to string
static KS_TFUNC(type, str) {
    ks_type self;
    KS_GETARGS("self:*", &self, ks_T_type)

    return KS_NEWREF(self->__name__);
}

// type.__hash__(self) - turn to hash
static KS_TFUNC(type, hash) {
    ks_type self;
    KS_GETARGS("self:*", &self, ks_T_type)

    return (ks_obj)ks_int_new((intptr_t)self);
};



/* export */

KS_TYPE_DECLFWD(ks_T_type);

void ks_init_T_type() {
    ks_type_init_c(ks_T_type, "type", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c_old(type_new_, "type.__new__(x)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(type_free_, "type.__free__(self)")},

        {"__str__",                (ks_obj)ks_cfunc_new_c_old(type_str_, "type.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(type_str_, "type.__repr__(self)")},
        {"__hash__",               (ks_obj)ks_cfunc_new_c_old(type_hash_, "type.__hash__(self)")},


        {"__getattr__",            (ks_obj)ks_cfunc_new_c_old(type_getattr_, "type.__getattr__(self, attr)")},

    ));


}
