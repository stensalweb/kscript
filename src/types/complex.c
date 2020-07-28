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


    char cstr[260];
    int len = 0;
    if (fabs(creal(self->val)) < 1e-14) {
        // just imaginary part

        snprintf(cstr, 255, "%.9lf", cimag(self->val));
        len = strlen(cstr);

        while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
            len--;
        }

        cstr[len++] = 'i';
        cstr[len] = '\0';

    } else {
        // real + imaginary, even if imaginary is 0, so it is obvious it is a complex number
        snprintf(cstr, 255, "(%.9lf", creal(self->val));

        len = strlen(cstr);
        while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
            len--;
        }

        // now, print out imaginary
        snprintf(cstr+len, 255-len, "%+.9lf", cimag(self->val));
        len = strlen(cstr);
        while (len > 1 && cstr[len - 1] == '0' && cstr[len - 2] != '.') {
            len--;
        }
        cstr[len++] = 'i';
        cstr[len++] = ')';
        cstr[len] = '\0';

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


// operators
KST_NUM_OPFS(fcomplex)

/* export */

KS_TYPE_DECLFWD(ks_T_complex);

void ks_init_T_complex() {

    ks_type_init_c(ks_T_complex, "complex", ks_T_obj, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_cfunc_new_c(complex_str_, "complex.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(complex_free_, "complex.__free__(self)")},

        KST_NUM_OPKVS(fcomplex)

    ));

}
