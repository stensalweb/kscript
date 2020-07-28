/* type.c - implementation of the 'type' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// set all defaults to NULL
static void my_setallnull(ks_type self, ks_type parent) {
    // macro to set NULL
    #define CASENULL(_name) self->_name = NULL;

    self->flags = KS_TYPE_FLAGS_NONE;

    CASENULL(__name__)
    CASENULL(__parents__)

    CASENULL(__new__)
    CASENULL(__init__)
    CASENULL(__free__)
    CASENULL(__bool__)
    CASENULL(__int__)
    CASENULL(__float__)
    CASENULL(__str__)
    CASENULL(__blob__)
    CASENULL(__getattr__)
    CASENULL(__setattr__)
    CASENULL(__getitem__)
    CASENULL(__setitem__)
    CASENULL(__repr__)
    CASENULL(__len__)
    CASENULL(__hash__)
    CASENULL(__iter__)
    CASENULL(__next__)
    CASENULL(__pos__)
    CASENULL(__neg__)
    CASENULL(__sqig__)
    CASENULL(__add__)
    CASENULL(__sub__)
    CASENULL(__mul__)
    CASENULL(__div__)
    CASENULL(__mod__)
    CASENULL(__pow__)
    CASENULL(__lt__)
    CASENULL(__gt__)
    CASENULL(__le__)
    CASENULL(__ge__)
    CASENULL(__eq__)
    CASENULL(__ne__)
    CASENULL(__cmp__)
    CASENULL(__binor__)
    CASENULL(__binand__)
    CASENULL(__binxor__)

    #undef CASENULL

    if (parent != NULL) {
        #define FROMPAR(_name) self->_name = parent->_name;


        FROMPAR(flags)

        FROMPAR(__new__)
        FROMPAR(__init__)
        FROMPAR(__free__)
        FROMPAR(__bool__)
        FROMPAR(__int__)
        FROMPAR(__float__)
        FROMPAR(__str__)
        FROMPAR(__blob__)
        FROMPAR(__getattr__)
        FROMPAR(__setattr__)
        FROMPAR(__getitem__)
        FROMPAR(__setitem__)
        FROMPAR(__repr__)
        FROMPAR(__len__)
        FROMPAR(__hash__)
        FROMPAR(__iter__)
        FROMPAR(__next__)
        FROMPAR(__pos__)
        FROMPAR(__neg__)
        FROMPAR(__sqig__)
        FROMPAR(__add__)
        FROMPAR(__sub__)
        FROMPAR(__mul__)
        FROMPAR(__div__)
        FROMPAR(__mod__)
        FROMPAR(__pow__)
        FROMPAR(__lt__)
        FROMPAR(__gt__)
        FROMPAR(__le__)
        FROMPAR(__ge__)
        FROMPAR(__eq__)
        FROMPAR(__ne__)
        FROMPAR(__cmp__)
        FROMPAR(__binor__)
        FROMPAR(__binand__)
        FROMPAR(__binxor__)




        #undef FROMPAR
    }

}

// Construct a new 'type' object, where `parent` can be any type that it will implement the same binary interface as
//   (giving NULL and ks_T_obj are equivalent)
// NOTE: Returns new reference, or NULL if an error was thrown
ks_type ks_type_new(ks_str name, ks_type parent) {
    ks_type self = KS_ALLOC_OBJ(ks_type);
    KS_INIT_OBJ(self, ks_T_type);

    my_setallnull(self, parent);

    self->attr = ks_dict_new(0, NULL);
    ks_list pars = ks_list_new(1, (ks_obj[]){ (ks_obj)parent });
    ks_type_set_c(self, KS_KEYVALS({"__name__", KS_NEWREF(name)}, {"__parents__", (ks_obj)pars}, ));

    return self;
}

// Initialize a C-defined type
void ks_type_init_c(ks_type self, const char* name, ks_type parent, ks_keyval_c* keyvals) {

    // standard initialization
    KS_INIT_OBJ(self, ks_T_type);

    my_setallnull(self, parent);

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
            if (!ks_type_issub(val->type, _type)) { printf("BAD TYPE FOR TYPE.ATTR\n"); return false; } \
            self->_name = (_totype)val; \
        }
        /**/ if (false) {}

        KEYCASE(__name__, ks_str, ks_T_str)
        KEYCASE(__parents__, ks_list, ks_T_list)

        KEYCASE(__new__, ks_obj, ks_T_obj)
        KEYCASE(__init__, ks_obj, ks_T_obj)
        KEYCASE(__free__, ks_obj, ks_T_obj)
        KEYCASE(__bool__, ks_obj, ks_T_obj)
        KEYCASE(__int__, ks_obj, ks_T_obj)
        KEYCASE(__float__, ks_obj, ks_T_obj)
        KEYCASE(__str__, ks_obj, ks_T_obj)
        KEYCASE(__blob__, ks_obj, ks_T_obj)
        KEYCASE(__getattr__, ks_obj, ks_T_obj)
        KEYCASE(__setattr__, ks_obj, ks_T_obj)
        KEYCASE(__getitem__, ks_obj, ks_T_obj)
        KEYCASE(__setitem__, ks_obj, ks_T_obj)
        KEYCASE(__repr__, ks_obj, ks_T_obj)
        KEYCASE(__len__, ks_obj, ks_T_obj)
        KEYCASE(__hash__, ks_obj, ks_T_obj)
        KEYCASE(__iter__, ks_obj, ks_T_obj)
        KEYCASE(__next__, ks_obj, ks_T_obj)
        KEYCASE(__pos__, ks_obj, ks_T_obj)
        KEYCASE(__neg__, ks_obj, ks_T_obj)
        KEYCASE(__sqig__, ks_obj, ks_T_obj)
        KEYCASE(__add__, ks_obj, ks_T_obj)
        KEYCASE(__sub__, ks_obj, ks_T_obj)
        KEYCASE(__mul__, ks_obj, ks_T_obj)
        KEYCASE(__div__, ks_obj, ks_T_obj)
        KEYCASE(__mod__, ks_obj, ks_T_obj)
        KEYCASE(__pow__, ks_obj, ks_T_obj)
        KEYCASE(__lt__, ks_obj, ks_T_obj)
        KEYCASE(__gt__, ks_obj, ks_T_obj)
        KEYCASE(__le__, ks_obj, ks_T_obj)
        KEYCASE(__ge__, ks_obj, ks_T_obj)
        KEYCASE(__eq__, ks_obj, ks_T_obj)
        KEYCASE(__ne__, ks_obj, ks_T_obj)
        KEYCASE(__cmp__, ks_obj, ks_T_obj)
        KEYCASE(__binor__, ks_obj, ks_T_obj)
        KEYCASE(__binand__, ks_obj, ks_T_obj)
        KEYCASE(__binxor__, ks_obj, ks_T_obj)

        else {
            // unknown double underscore; ignore it
        }

        #undef KEYCASE
    }

    // actually set the internal dictionary
    ks_dict_set_h(self->attr, (ks_obj)key, hash, val);

    // success
    return true;

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

// calculate whether it is a sub type
bool ks_type_issub(ks_type self, ks_type of) {
    // TODO: implement type hierarchies
    if (self == of || of == NULL || of == ks_T_obj) return true;

    ks_size_t i;
    for (i = 0; i < self->__parents__->len; ++i) {
        ks_type this_par = (ks_type)self->__parents__->elems[i];
        if (this_par != self && ks_type_issub(this_par, of)) return true;
    }

    return false;
}



// type.__free__(self) - free obj
static KS_TFUNC(type, free) {
    ks_type self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_type)) return NULL;

    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);


    return KSO_NONE;
}


// type.__str__(self) - turn to string
static KS_TFUNC(type, str) {
    ks_type self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_type)) return NULL;

    return KS_NEWREF(self->__name__);
}


/* export */

KS_TYPE_DECLFWD(ks_T_type);

void ks_init_T_type() {
    ks_type_init_c(ks_T_type, "type", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(type_free_, "type.__free__(self)")},

        {"__str__",               (ks_obj)ks_cfunc_new_c(type_str_, "type.__str__(self)")},

    ));


    ks_T_type->flags |= KS_TYPE_FLAGS_EQSS;

}
