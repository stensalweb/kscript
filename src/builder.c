/* builder.c - implementation of string building utilities
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// initialize the string builder
void ks_str_builder_init(ks_str_builder* self) {
    self->len = 0;
    self->data = NULL;
}

// Create a (new reference) of a string from the string builder at this point
ks_str ks_str_builder_get(ks_str_builder* self) {
    return ks_new_str_l(self->len, self->data);
}


// add bytes to the string builder
void ks_str_builder_add(ks_str_builder* self, int len, char* data) {
    // position to start writing the data
    int pos = self->len;

    self->len += len;

    // make sure we have enough data for the entire string
    self->data = ks_realloc(self->data, self->len + 1);

    // copy in the new data
    memcpy(&self->data[pos], data, len);
    self->data[self->len] = '\0';
}


// add repr(obj) to the string builder
bool ks_str_builder_add_repr(ks_str_builder* self, ks_obj obj) {

    // attempt to get the repr
    ks_str repr = ks_repr(obj);
    if (!repr) return false;

    // ensure the type was a string
    assert(repr->type == ks_type_str);

    // we have a valid string, add it
    ks_str_builder_add(self, repr->len, repr->chr);

    // dispose of our ref
    KS_DECREF(repr);

    // success
    return true;
}


// add str(obj) to the string buffer
bool ks_str_builder_add_str(ks_str_builder* self, ks_obj obj) {

    // attempt to get the repr
    ks_str to_str = ks_to_str(obj);
    if (!to_str) return false;

    // ensure the type was a string
    assert(to_str->type == ks_type_str);

    // we have a valid string, add it
    ks_str_builder_add(self, to_str->len, to_str->chr);

    // dispose of our ref
    KS_DECREF(to_str);

    // success
    return true;
}


// Free the string builder, freeing all internal resources (but not the built strings)
void ks_str_builder_free(ks_str_builder* self) {
    self->len = 0;
    ks_free(self->data);
}


