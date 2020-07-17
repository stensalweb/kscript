/* int.c - implementation of the 'int' type in kscript
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// All integers with abs(x) < KS_SMALL_INT_MAX are 'small' integers and kept as an interned list
#define KS_SMALL_INT_MAX 255

// global singletons of small integers
static struct ks_int_s KS_SMALL_INTS[2 * KS_SMALL_INT_MAX + 1];


// Construct a new 'int' object
// NOTE: Returns new reference, or NULL if an error was thrown
ks_int ks_int_new(int64_t val) {
    if (val <= KS_SMALL_INT_MAX && val >= -KS_SMALL_INT_MAX) return &KS_SMALL_INTS[val + KS_SMALL_INT_MAX];
    
    // now, actually create a value
    ks_int self = KS_ALLOC_OBJ(ks_int);

    KS_INIT_OBJ(self, ks_T_int);

    // int64_t's can always fit
    self->isLong = false;

    self->v64 = val;

    return self;
}

// Return the digit value (irrespective of base), or -1 if there was a problem
static int my_getdig(char c) {
    if (isdigit(c)) {
        return c - '0';
    } else if (c >= 'a' && c <= 'z') {
        return (c - 'a') + 10;
    } else if (c >= 'A' && c <= 'Z') {
        return (c - 'A') + 10;
    } else {
        // errro: invalid digit
        return -1;
    }
}

// Create a kscript int from a string in a given base
ks_int ks_int_new_s(char* str, int base) {
    // calculate string length    
    int len = strlen(str);

    // try to calculate in a 64 bit integer
    int64_t v64 = 0;

    // check for any signs
    bool isNeg = *str == '-';
    if (isNeg || *str == '+') {
        str++;
        len--;
    }

    int i = 0;

    // parse out main value
    while (i < len) {
        int dig = my_getdig(str[i]);
        // check for invalid/out of range digit
        if (dig < 0 || dig >= base) return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: %s", base, str);

        int64_t old_v64 = v64;
        // calculate new value in 64 bits
        v64 = base * v64 + dig;

        if (v64 < old_v64) {
            // overflow
            goto do_mpz_str;
        }
        i++;
    }

    // we construct via v64 methods
    return ks_int_new(isNeg ? -v64 : v64);

    do_mpz_str:;

    // now, we need to handle via MPZ initialization method

    // allocate a new integer
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_T_int);

    // must be a long integer
    self->isLong = true;

    // initialize the mpz integer
    mpz_init(self->vz);

    if (mpz_set_str(self->vz, str, base) != 0) {
        // there was a problem
        KS_DECREF(self);
        return (ks_int)ks_throw(ks_T_ArgError, "Invalid format for base %i integer: %s", base, str);
    }

    return self;
}


// int.__str__(self) - to string
static KS_TFUNC(int, str) {
    ks_int self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_int)) return NULL;


    int base = 10;

    if (self->isLong) {

        // do mpz to string
        size_t total_size = 16 + mpz_sizeinbase(self->vz, base);

        // temporary buffer
        char* tmp = ks_malloc(total_size);
        int i = 0;


        // add prefix specifier
        if (base == 10) {
            // do nothing
        } else if (base == 16) {
            tmp[i++] = '0';
            tmp[i++] = 'x';
        } else if (base == 8) {
            tmp[i++] = '0';
            tmp[i++] = 'o';
        } else if (base == 2) {
            tmp[i++] = '0';
            tmp[i++] = 'b';
        } else {
            return ks_throw(ks_T_ArgError, "Invalid base '%i' for int->str conversion", base);
        }

        // use GMP to get string
        mpz_get_str(&tmp[i], base, self->vz);

        ks_str res = ks_str_new_c(tmp, -1);
        ks_free(tmp);

        return (ks_obj)res;

    } else {

        char tmp[256];
        int ct = snprintf(tmp, sizeof(tmp) - 1, "%lli", (long long int)self->v64);

        return (ks_obj)ks_str_new_c(tmp, ct);

    }

}



/* export */

KS_TYPE_DECLFWD(ks_T_int);

void ks_init_T_int() {

    // initialize singletons
    int64_t i;
    for (i = -KS_SMALL_INT_MAX; i <= KS_SMALL_INT_MAX; ++i) {
        ks_int self = &KS_SMALL_INTS[KS_SMALL_INT_MAX + i];
        KS_INIT_OBJ(self, ks_T_int);

        self->isLong = false;
        self->v64 = i;

    }
    
    ks_type_init_c(ks_T_int, "int", ks_T_obj, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_cfunc_new_c(int_str_, "int.__str__(self)")},
    ));

}
