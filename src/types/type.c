/* type.c - implementation of the 'type' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// set all defaults to NULL
static void my_setallnull(ks_type self, ks_type parent) {
    // macro to set NULL
    #define CASENULL(_name) self->_name = NULL;

    CASENULL(__free__)
    CASENULL(__parents__)
    CASENULL(__str__)

    #undef CASENULL

    if (parent != NULL) {
        #define FROMPAR(_name) self->_name = parent->_name;

        FROMPAR(__free__)
        FROMPAR(__parents__)
        FROMPAR(__str__)

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


// Set a given key,val pair to a type, returning success
// NOTE: Returns success
bool ks_type_set(ks_type self, ks_str key, ks_obj val) {
    assert (key->type == ks_T_str && "setting type attribute that was not a string");

    // get hash
    ks_hash_t hash = key->v_hash;


    // double underscore attributes
    if (key->len > 4 && key->chr[0] == '_' && key->chr[1] == '_') {

        // helper macro for given special keys
        #define KEYCASE(_name, _totype, _type) else if ((sizeof(#_name) - 1) == key->len && strncmp(#_name, key->chr, sizeof(#_name) - 1) == 0) { \
            if (!ks_type_issub(val->type, _type)) { printf("BAD TYPE FOR TYPE.ATTR\n"); return false; } \
            self->_name = (_totype)val; \
        }
        /**/ if (false) {}

        KEYCASE(__name__, ks_str, ks_T_str)
        KEYCASE(__parents__, ks_list, ks_T_list)
        KEYCASE(__free__, ks_obj, ks_T_obj)
        KEYCASE(__str__, ks_obj, ks_T_obj)

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




/* export */

KS_TYPE_DECLFWD(ks_T_type);

void ks_init_T_type() {
    ks_type_init_c(ks_T_type, "type", ks_T_obj, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_str_new_c("asdfasdf", -1)},
    ));

}
