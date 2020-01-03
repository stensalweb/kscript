/* types/int.c - implementation of the builtin `int` type 

The default integer type is a 64 bit signed value


*/

#include "ks_common.h"

/* int type config */

// determines how many integers are stored in the global constant tables
// essentially, these numbers are only ever created once, for efficiency reasons
// the numbers -_INT_CONST_MAX <= x < _INT_CONST_MAX are created
#define _INT_CONST_MAX 256

// the table of single-ly constructed constants, in order from min to largest
static struct ks_int int_const_tbl[2 * _INT_CONST_MAX];


/* C creation routines */

/* constructs a new integer from a 64-bit C integer */
ks_int ks_int_new(int64_t v_int) {
    if (v_int >= -_INT_CONST_MAX && v_int < _INT_CONST_MAX) {
        // it is held in the constant's table, so just return that
        return &int_const_tbl[v_int + _INT_CONST_MAX];
    } else {
        // else, we need to allocate a new integer size
        // NOTE: while we could attempt to intern integers, many tests (like Python's)
        //   have shown that this is actually not worth it, so for now we just always create a new one
        ks_int self = (ks_int)ks_malloc(sizeof(*self));
        *self = (struct ks_int) {
            KSO_BASE_INIT(ks_T_int, KSOF_NONE)
            .v_int = v_int
        };

        // return our constructed integer
        return self;
    }
}


/* type functions */

// int.__str__(self) -> return the integer as its string
TFUNC(int, str) {
    #define SIG "int.__str__(self)"
    REQ_N_ARGS(1);
    ks_int self = (ks_int)args[0];
    REQ_TYPE("self", self, ks_T_int);

    // capture it as an actual integer
    int64_t self_i = self->v_int;

    if (self_i >= 0 && self_i <= 9) {
        // single digit, so return a single length string
        char c = '0' + self_i;
        return (kso)ks_str_new_l(&c, 1);
    }

    // keep a string buffer for scratch space
    static char strbuf[64];

    // current position in the string buffer
    int strbuf_p = 0;

    // pull out the sign
    bool neg = false;
    if (self_i < 0) {
        neg = true;
        self_i = -self_i;
    }

    // now, export it in reverse, then reversify the string
    do {
        strbuf[strbuf_p++] = (self_i % 10) + '0';
        self_i /= 10;
    } while (self_i > 0);

    if (neg) strbuf[strbuf_p++] = '-';

    // now reverse the whole thing
    int i;
    for (i = 0; 2 * i < strbuf_p; ++i) {
        char tmp = strbuf[i];
        strbuf[i] = strbuf[strbuf_p - i - 1];
        strbuf[strbuf_p - i - 1] = tmp;
    }

    // return from C string
    return (kso)ks_str_new_l(strbuf, strbuf_p);
    #undef SIG
}

// int.__repr__(self) -> return the integer as its string representation
TFUNC(int, repr) {
    #define SIG "int.__repr__(self)"
    REQ_N_ARGS(1);
    ks_int self = (ks_int)args[0];
    REQ_TYPE("self", self, ks_T_int);

    // capture it as an actual integer
    int64_t self_i = self->v_int;

    if (self_i >= 0 && self_i <= 9) {
        // single digit, so return a single length string
        char c = '0' + self_i;
        return (kso)ks_str_new_l(&c, 1);
    }

    // keep a string buffer for scratch space
    static char strbuf[64];

    // current position in the string buffer
    int strbuf_p = 0;

    // pull out the sign
    bool neg = false;
    if (self_i < 0) {
        neg = true;
        self_i = -self_i;
    }

    // now, export it in reverse, then reversify the string
    do {
        strbuf[strbuf_p++] = (self_i % 10) + '0';
        self_i /= 10;
    } while (self_i > 0);

    if (neg) strbuf[strbuf_p++] = '-';

    // now reverse the whole thing
    int i;
    for (i = 0; 2 * i < strbuf_p; ++i) {
        char tmp = strbuf[i];
        strbuf[i] = strbuf[strbuf_p - i - 1];
        strbuf[strbuf_p - i - 1] = tmp;
    }

    // return from C string
    return (kso)ks_str_new_l(strbuf, strbuf_p);
    #undef SIG
}


// int.__add__(A, B) -> returns the sum of A and B
TFUNC(int, add) {
    #define SIG "int.__add__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int + ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__sub__(A, B) -> returns the difference of A and B
TFUNC(int, sub) {
    #define SIG "int.__sub__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int - ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__mul__(A, B) -> returns the difference of A and B
TFUNC(int, mul) {
    #define SIG "int.mul(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int * ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__div__(A, B) -> returns the quotient of A and B
TFUNC(int, div) {
    #define SIG "int.__div__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return (kso)ks_int_new(((ks_int)A)->v_int / ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__mod__(A, B) -> returns the modulo of A and B (always positive)
TFUNC(int, mod) {
    #define SIG "int.__mod__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        int64_t Ai = ((ks_int)A)->v_int;
        int64_t Bi = ((ks_int)B)->v_int;

        int64_t Ri = Ai % Bi;

        // by convention, modulo should always be positive
        if (Ri < 0) Ri += Bi;

        return (kso)ks_int_new(Ri);
    }

    return NULL;
    #undef SIG
}

// int.__pow__(A, B) -> returns the power of A and B
TFUNC(int, pow) {
    #define SIG "int.__pow__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        int64_t Ai = ((ks_int)A)->v_int;
        int64_t Bi = ((ks_int)B)->v_int;
        // X^0 == 1
        if (Bi == 0) return (kso)ks_int_new(1);
        // 0^X == 0 (negative X's return 0s too)
        if (Ai == 0) return (kso)ks_int_new(0);
        // X^-n == 0, for integers
        if (Bi < 0) return (kso)ks_int_new(0);
        // X^1==X
        if (Bi == 1) return A;

        // the sign to be applied
        bool neg = (Ai < 0 && Bi & 1 == 1);
        if (Ai < 0) Ai = -Ai;

        // now, Bi>1 and Ai is positive, with sign extracted

        // now, calculate it
        int64_t Ri = 1;
        while (Bi != 0) {
            if (Bi & 1) Ri *= Ai;
            Bi >>= 1;
            Ai *= Ai;
        }

        return (kso)ks_int_new(neg ? -Ri : Ri);
    }

    return NULL;
    #undef SIG
}

// int.__lt__(A, B) -> returns A < B
TFUNC(int, lt) {
    #define SIG "int.__lt__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int < ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__le__(A, B) -> returns A <= B
TFUNC(int, le) {
    #define SIG "int.__le__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int <= ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__gt__(A, B) -> returns A > B
TFUNC(int, gt) {
    #define SIG "int.__gt__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int > ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__ge__(A, B) -> returns A >= B
TFUNC(int, ge) {
    #define SIG "int.__ge__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int >= ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__eq__(A, B) -> returns A == B
TFUNC(int, eq) {
    #define SIG "int.__eq__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int == ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}

// int.__ne__(A, B) -> returns A == B
TFUNC(int, ne) {
    #define SIG "int.__ne__(A, B)"
    REQ_N_ARGS(2);
    kso A = args[0], B = args[1];

    if (A->type == ks_T_int && B->type == ks_T_int) {
        return KSO_BOOL(((ks_int)A)->v_int != ((ks_int)B)->v_int);
    }

    return NULL;
    #undef SIG
}


/* exporting functionality */

struct ks_type T_int, *ks_T_int = &T_int;

void ks_init__int() {

    /* first create the type */
    T_int = (struct ks_type) {
        KS_TYPE_INIT("int")

        .f_str   = (kso)ks_cfunc_newref(int_str_),
        .f_repr  = (kso)ks_cfunc_newref(int_repr_),

        .f_add   = (kso)ks_cfunc_newref(int_add_),
        .f_sub   = (kso)ks_cfunc_newref(int_sub_),
        .f_mul   = (kso)ks_cfunc_newref(int_mul_),
        .f_div   = (kso)ks_cfunc_newref(int_div_),
        .f_mod   = (kso)ks_cfunc_newref(int_mod_),
        .f_pow   = (kso)ks_cfunc_newref(int_pow_),

        .f_lt    = (kso)ks_cfunc_newref(int_lt_),
        .f_le    = (kso)ks_cfunc_newref(int_le_),
        .f_gt    = (kso)ks_cfunc_newref(int_gt_),
        .f_ge    = (kso)ks_cfunc_newref(int_ge_),
        .f_eq    = (kso)ks_cfunc_newref(int_eq_),
        .f_ne    = (kso)ks_cfunc_newref(int_ne_),

    };

    /* now create the constant tables */
    int i;
    for (i = -_INT_CONST_MAX; i < _INT_CONST_MAX; ++i) {
        int_const_tbl[i + _INT_CONST_MAX] = (struct ks_int) {
            KSO_BASE_INIT_R(ks_T_int, KSOF_NONE, 1)
            .v_int = i
        };
    }
}


