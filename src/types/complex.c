/* complex.c - implementation of the complex data type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// Construct a new 'complex' object
// NOTE: Returns new reference, or NULL if an error was thrown
ks_complex ks_complex_new(double complex val) {
    ks_complex self = KS_ALLOC_OBJ(ks_complex);
    KS_INIT_OBJ(self, ks_T_complex);

    self->val = val;

    return self;
}

// complex.__str__(self) - to string
static KS_TFUNC(complex, str) {
    ks_complex self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_complex)) return NULL;


    // print it out
    char cstr[256];
    snprintf(cstr, 255, "%.9lf", (double)self->val);

    int len = strlen(cstr);
    while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
        len--;
    }

    return (ks_obj)ks_str_new_c(cstr, len);
}

// complex.__free__(self) - free object
static KS_TFUNC(complex, free) {
    ks_complex self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_complex)) return NULL;

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_complex);

void ks_init_T_complex() {

    ks_type_init_c(ks_T_complex, "complex", ks_T_obj, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_cfunc_new_c(complex_str_, "complex.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(complex_free_, "complex.__free__(self)")},
    ));

}
