/* tuple.c - implementation of the 'tuple' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// construct a tuple
ks_tuple ks_tuple_new(int len, ks_obj* elems) {
    // allocate enough memory for the elements
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + sizeof(ks_obj) * len);
    KS_INIT_OBJ(self, ks_T_tuple);

    // initialize type-specific things
    self->len = len;

    int i;
    // and populate the array
    for (i = 0; i < len; ++i) {
        self->elems[i] = KS_NEWREF(elems[i]);
    }


    return self;
}


// contruct a tuple with no new references
ks_tuple ks_tuple_new_n(int len, ks_obj* elems) {
    // allocate enough memory for the elements
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + sizeof(ks_obj) * len);
    KS_INIT_OBJ(self, ks_T_tuple);

    // initialize type-specific things
    self->len = len;

    int i;
    if (elems != NULL) {
        // and populate the array, without creating new references
        for (i = 0; i < len; ++i) {
            self->elems[i] = elems[i];
        }

    } else {
        // and populate the array, without creating new references
        for (i = 0; i < len; ++i) {
            self->elems[i] = NULL;
        }
    }

    return self;
}


// tuple.__free__(self) - free object
static KS_TFUNC(tuple, free) {
    ks_tuple self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_tuple)) return NULL;

    ks_size_t i;

    // free references held to entries
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}



/* export */

KS_TYPE_DECLFWD(ks_T_tuple);

void ks_init_T_tuple() {
    ks_type_init_c(ks_T_tuple, "tuple", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(tuple_free_, "tuple.__free__(self)")},
    ));

}


