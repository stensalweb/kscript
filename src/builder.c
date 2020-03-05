/* builder.c - implementation of string building utilities
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// initialize the string builder
void ks_str_b_init(ks_str_b* self) {
    self->len = 0;
    self->data = NULL;
}

// Create a (new reference) of a string from the string builder at this point
ks_str ks_str_b_get(ks_str_b* self) {
    return ks_str_new_l(self->data, self->len);
}


// add bytes to the string builder
void ks_str_b_add(ks_str_b* self, int len, char* data) {
    // position to start writing the data
    int pos = self->len;

    self->len += len;

    // make sure we have enough data for the entire string
    self->data = ks_realloc(self->data, self->len + 1);

    // copy in the new data
    memcpy(&self->data[pos], data, len);
    self->data[self->len] = '\0';
}


void ks_str_b_add_c(ks_str_b* self, char* cstr) {
    ks_str_b_add(self, strlen(cstr), cstr);
}

// add repr(obj) to the string builder
bool ks_str_b_add_repr(ks_str_b* self, ks_obj obj) {

    // try some builtins
    if (obj == KSO_NONE) {
        ks_str_b_add_c(self, "none");
        return true;
    } else if (obj == KSO_TRUE) {
        ks_str_b_add_c(self, "true");
        return true;
    } else if (obj == KSO_TRUE) {
        ks_str_b_add_c(self, "false");
        return true;
    }

    // attempt to get the repr
    ks_str repr = ks_repr(obj);
    if (!repr) return false;

    // ensure the type was a string
    assert(repr->type == ks_type_str);

    // we have a valid string, add it
    ks_str_b_add(self, repr->len, repr->chr);

    // dispose of our ref
    KS_DECREF(repr);

    // success
    return true;
}


// add str(obj) to the string buffer
bool ks_str_b_add_str(ks_str_b* self, ks_obj obj) {

    // try some builtins
    if (obj == KSO_NONE) {
        ks_str_b_add_c(self, "none");
        return true;
    } else if (obj == KSO_TRUE) {
        ks_str_b_add_c(self, "true");
        return true;
    } else if (obj == KSO_TRUE) {
        ks_str_b_add_c(self, "false");
        return true;
    }

    // attempt to get the repr
    ks_str to_str = ks_to_str(obj);
    if (!to_str) return false;

    // ensure the type was a string
    assert(to_str->type == ks_type_str);

    // we have a valid string, add it
    ks_str_b_add(self, to_str->len, to_str->chr);

    // dispose of our ref
    KS_DECREF(to_str);

    // success
    return true;
}


// Free the string builder, freeing all internal resources (but not the built strings)
void ks_str_b_free(ks_str_b* self) {
    self->len = 0;
    ks_free(self->data);
}


