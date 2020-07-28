/* float.c - implementatino of floating point type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// Construct a new 'float' object
// NOTE: Returns new reference, or NULL if an error was thrown
ks_float ks_float_new(double val) {
    ks_float self = KS_ALLOC_OBJ(ks_float);
    KS_INIT_OBJ(self, ks_T_float);

    self->val = val;

    return self;
}

// float.__str__(self) - to string
static KS_TFUNC(float, str) {
    ks_float self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_float)) return NULL;


    // print it out
    char cstr[256];
    snprintf(cstr, 255, "%.9lf", (double)self->val);

    int len = strlen(cstr);
    while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
        len--;
    }

    return (ks_obj)ks_str_new_c(cstr, len);
}

// float.__free__(self) - free object
static KS_TFUNC(float, free) {
    ks_float self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_float)) return NULL;

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// operators
KST_NUM_OPFS(float)


/* export */

KS_TYPE_DECLFWD(ks_T_float);

void ks_init_T_float() {

    ks_type_init_c(ks_T_float, "float", ks_T_obj, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_cfunc_new_c(float_str_, "float.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(float_free_, "float.__free__(self)")},

        KST_NUM_OPKVS(float)


    ));

}
