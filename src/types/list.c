/* list.c - implementation of the 'list' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"




// Construct a new list from an array of elements
// NOTE: Returns new reference, or NULL if an error was thrown
KS_API ks_list ks_list_new(ks_size_t len, ks_obj* elems) {

    ks_list self = KS_ALLOC_OBJ(ks_list);
    KS_INIT_OBJ(self, ks_T_list);


    self->len = len;
    self->elems = ks_malloc(sizeof(*self->elems) * self->len);

    ks_size_t i;
    for (i = 0; i < len; ++i) {
        self->elems[i] = elems[i];
        KS_INCREF(elems[i]);
    }


    return self;

}




// list.__free__(self) - free object
static KS_TFUNC(list, free) {
    ks_list self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_list)) return NULL;


    ks_size_t i;

    // free references held to entries
    for (i = 0; i < self->len; ++i) {
        KS_DECREF(self->elems[i]);
    }

    // free allocated space
    ks_free(self->elems);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_list);

void ks_init_T_list() {
    ks_type_init_c(ks_T_list, "list", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(list_free_, "list.__free__(self)")},
    ));

}


