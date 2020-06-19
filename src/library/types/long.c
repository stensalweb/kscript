/* types/long.c - arbitrary precision value
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_long);

// create a kscript int from a C-style int
ks_long ks_long_new(int64_t val) {
    ks_long self = KS_ALLOC_OBJ(ks_long);
    KS_INIT_OBJ(self, ks_type_long);

    // ensure a cast is exact
    if (val == (signed long)val) {
        mpz_init_set_si(self->val, (signed long)val);
    } else {

        // on windows, we may have problems with long being 32 bit size,
        // so default to string conversion for all time
        char tmp[256];
        snprintf(tmp, 255, "%llx", (long long int)val);

        mpz_init_set_str(self->val, tmp, 16);
    }

    return self;
}

// create a kscript int from a C-style int
ks_long ks_long_new_str(char* str, int base) {
    ks_long self = KS_ALLOC_OBJ(ks_long);
    KS_INIT_OBJ(self, ks_type_long);

    mpz_init_set_str(self->val, str, base);

    return self;
}

// construct a copy of a long
static ks_long my_longcopy(ks_long other) {
    ks_long self = KS_ALLOC_OBJ(ks_long);
    KS_INIT_OBJ(self, ks_type_long);

    // copy over the value
    mpz_init_set(self->val, other->val);

    return self;
}

// convert a long into an integer,
// or return false otherwise
static bool my_getint(ks_long self, int64_t* ret) {
    if (mpz_fits_slong_p(self->val)) {
        *ret = mpz_get_si(self->val);
        return true;
    } else {
        *ret = -1;
        return false;
    }
}



#define MAX_BASE 36

// long.__new__(obj, base=10) -> convert 'obj' to a long
static KS_TFUNC(long, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    int base = 10;

    if (n_args >= 2) {
        ks_int base_o = (ks_int)args[1];
        KS_REQ_TYPE(base_o, ks_type_int, "base");
        base = base_o->val;
        if (base < 2 || base > MAX_BASE) return ks_throw_fmt(ks_type_ArgError, "Invalid base '%i' (only handles base 2 through %i)", base, MAX_BASE);
    }

    ks_obj obj = args[0];
    if (obj->type == ks_type_long) {
        return KS_NEWREF(obj);
    } else if (obj->type == ks_type_int) {
        return (ks_obj)ks_long_new(((ks_int)obj)->val);
    } else if (obj->type == ks_type_float) {
        return (ks_obj)ks_long_new(round(((ks_float)obj)->val));
    } else if (obj->type == ks_type_complex) {
        return (ks_obj)ks_long_new(round(((ks_complex)obj)->val));
    } else if (obj->type == ks_type_str) {

        return (ks_obj)ks_long_new_str(((ks_str)obj)->chr, base);
    } else {
        KS_ERR_CONV(obj, ks_type_long);
    }
};


// long.__str__(self, base=10) -> convert to a string
static KS_TFUNC(long, str) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_long self = (ks_long)args[0];
    KS_REQ_TYPE(self, ks_type_long, "self");

    int base = 10;
    if (n_args > 1) {
        ks_int base_o = (ks_int)args[1];
        KS_REQ_TYPE(base_o, ks_type_int, "base");
        base = base_o->val;
        if (base < 2 || base > MAX_BASE) return ks_throw_fmt(ks_type_ArgError, "Invalid base '%i' (only handles base 2 through %i)", base, MAX_BASE);
    }

    size_t req = mpz_sizeinbase(self->val, base) + 2;

    char* new_str = ks_malloc(req);

    mpz_get_str(new_str, base, self->val);
    ks_str res = ks_str_new(new_str);
    ks_free(new_str);

    return (ks_obj)res;
};


// long.__hash__(self) -> calculate the hash of a long object
static KS_TFUNC(long, hash) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_long self = (ks_long)args[0];
    KS_REQ_TYPE(self, ks_type_long, "self");

    int64_t self_int;

    // attempt to hash it like a normal integer
    if (my_getint(self, &self_int)) {
        // fits integer, so return that hash
        return (ks_obj)ks_int_new(self_int == 0 ? 1 : self_int);
    }
    // otherwise, just hash bytes
    return (ks_obj)ks_int_new(ks_hash_bytes(sizeof(*self->val->_mp_d) * self->val->_mp_size, (uint8_t*)self->val->_mp_d));
};

// long.__free__(self) -> free an long object
static KS_TFUNC(long, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_long self = (ks_long)args[0];
    KS_REQ_TYPE(self, ks_type_long, "self");

    mpz_clear(self->val);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// long.__add__(L, R) -> add 2 integers
static KS_TFUNC(long, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_long && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        mpz_add(res->val, ((ks_long)L)->val, ((ks_long)R)->val);
        return (ks_obj)res;
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        ks_long res = ks_long_new(0);
        int64_t iR = ((ks_int)R)->val;
        if ((unsigned long)iR == iR) {
            mpz_add_ui(res->val, ((ks_long)L)->val, (unsigned long)iR);
        } else {
            ks_long lR = ks_long_new(iR);
            mpz_add(res->val, ((ks_long)L)->val, lR->val);
            KS_DECREF(lR);
        }
        return (ks_obj)res;
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        int64_t iL = ((ks_int)L)->val;
        if ((unsigned long)iL == iL) {
            mpz_add_ui(res->val, ((ks_long)R)->val, (unsigned long)iL);
        } else {
            ks_long lL = ks_long_new(iL);
            mpz_add(res->val, ((ks_long)R)->val, lL->val);
            KS_DECREF(lL);
        }
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("+", L, R);
};



// long.__sub__(L, R) -> sub 2 integers
static KS_TFUNC(long, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_long && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        mpz_sub(res->val, ((ks_long)L)->val, ((ks_long)R)->val);
        return (ks_obj)res;
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        ks_long res = ks_long_new(0);
        int64_t iR = ((ks_int)R)->val;
        if ((unsigned long)iR == iR) {
            mpz_sub_ui(res->val, ((ks_long)L)->val, (unsigned long)iR);
        } else {
            ks_long lR = ks_long_new(iR);
            mpz_sub(res->val, ((ks_long)L)->val, lR->val);
            KS_DECREF(lR);
        }
        return (ks_obj)res;
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        int64_t iL = ((ks_int)L)->val;
        if ((unsigned long)iL == iL) {
            mpz_ui_sub(res->val, (unsigned long)iL, ((ks_long)R)->val);
        } else {
            ks_long lL = ks_long_new(iL);
            mpz_sub(res->val, lL->val, ((ks_long)R)->val);
            KS_DECREF(lL);
        }
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("-", L, R);
};


// long.__mul__(L, R) -> mul 2 integers
static KS_TFUNC(long, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_long && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        mpz_mul(res->val, ((ks_long)L)->val, ((ks_long)R)->val);
        return (ks_obj)res;
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        ks_long res = ks_long_new(0);
        int64_t iR = ((ks_int)R)->val;
        if ((unsigned long)iR == iR) {
            mpz_mul_ui(res->val, ((ks_long)L)->val, (unsigned long)iR);
        } else {
            ks_long lR = ks_long_new(iR);
            mpz_mul(res->val, ((ks_long)L)->val, lR->val);
            KS_DECREF(lR);
        }
        return (ks_obj)res;
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        int64_t iL = ((ks_int)L)->val;
        if ((unsigned long)iL == iL) {
            mpz_mul_ui(res->val, ((ks_long)R)->val, (unsigned long)iL);
        } else {
            ks_long lL = ks_long_new(iL);
            mpz_mul(res->val, lL->val, ((ks_long)R)->val);
            KS_DECREF(lL);
        }
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("*", L, R);
};


// long.__div__(L, R) -> div 2 integers
static KS_TFUNC(long, div) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_long && R->type == ks_type_long) {
        if (mpz_sgn(((ks_long)R)->val) == 0) return ks_throw_fmt(ks_type_MathError, "Division by 0!");
        ks_long res = ks_long_new(0);
        mpz_tdiv_q(res->val, ((ks_long)L)->val, ((ks_long)R)->val);
        return (ks_obj)res;
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        int64_t iR = ((ks_int)R)->val;
        if (iR == 0) return ks_throw_fmt(ks_type_MathError, "Division by 0!");
        ks_long res = ks_long_new(0);

        if (iR >= 0 && (unsigned long)iR == iR) {
            mpz_tdiv_q_ui(res->val, ((ks_long)L)->val, (unsigned long)iR);
        } else {
            ks_long lR = ks_long_new(iR);
            mpz_tdiv_q(res->val, ((ks_long)L)->val, lR->val);
            KS_DECREF(lR);
        }
        return (ks_obj)res;
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        if (mpz_sgn(((ks_long)R)->val) == 0) return ks_throw_fmt(ks_type_MathError, "Division by 0!");

        ks_long res = ks_long_new(0);
        int64_t iL = ((ks_int)L)->val;
        ks_long lL = ks_long_new(iL);
        mpz_tdiv_q(res->val, lL->val, ((ks_long)R)->val);
        KS_DECREF(lL);
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("/", L, R);
};


// long.__pow__(L, R) -> pow 2 integers
static KS_TFUNC(long, pow) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_long && R->type == ks_type_long) {
        // NOTE: must be an integer on the right, otherwise it might overflow
        int64_t iR;
        if (!my_getint((ks_long)R, &iR)) return ks_throw_fmt(ks_type_MathError, "Exponent %R was too large!", R);

        // now, apply power
        ks_long res = ks_long_new(0);
        if (iR < 0) return (ks_obj)res;

        mpz_pow_ui(res->val, ((ks_long)L)->val, iR);
        return (ks_obj)res;
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        int64_t iR = ((ks_int)R)->val;

        ks_long res = ks_long_new(0);
        if (iR < 0) return (ks_obj)res;

        mpz_pow_ui(res->val, ((ks_long)L)->val, iR);
        return (ks_obj)res;
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        // NOTE: must be an integer on the right, otherwise it might overflow
        int64_t iR;
        if (!my_getint((ks_long)R, &iR)) return ks_throw_fmt(ks_type_MathError, "Exponent %R was too large!", R);

        ks_long res = ks_long_new(0);
        if (iR < 0) return (ks_obj)res;

        mpz_ui_pow_ui(res->val, ((ks_int)L)->val, iR);
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("**", L, R);
};


// long.__mod__(L, R) -> mod 2 integers
static KS_TFUNC(long, mod) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_long && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        mpz_mod(res->val, ((ks_long)L)->val, ((ks_long)R)->val);
        if (mpz_sgn(res->val) < 0) mpz_add(res->val, res->val, ((ks_long)R)->val);

        return (ks_obj)res;
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        ks_long res = ks_long_new(0);
        int64_t iR = ((ks_int)R)->val;
        if (iR >= 0 && (unsigned long)iR == iR) {
            mpz_mod_ui(res->val, ((ks_long)L)->val, (unsigned long)iR);
            //if (mpz_sgn(res->val) < 0) mpz_add_ui(res->val, res->val, (unsigned long)iR);
        } else {
            ks_long lR = ks_long_new(iR);
            mpz_mod(res->val, ((ks_long)L)->val, lR->val);
            if (mpz_sgn(res->val) < 0) mpz_add(res->val, res->val, lR->val);
            KS_DECREF(lR);
        }
        return (ks_obj)res;
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        ks_long res = ks_long_new(0);
        ks_long lL = ks_long_new(((ks_int)L)->val);
        mpz_mod(res->val, lL->val, ((ks_long)R)->val);
        if (mpz_sgn(res->val) < 0) mpz_add(res->val, res->val, ((ks_long)R)->val);

        KS_DECREF(lL);
        return (ks_obj)res;
    }

    KS_ERR_BOP_UNDEF("%", L, R);
};


// compare, returning 1,0,-1, or -2 if there was an error
static int my_longcmp(ks_obj L, ks_obj R) {

    if (L->type == ks_type_long && R->type == ks_type_long) {
        int res = mpz_cmp(((ks_long)L)->val, ((ks_long)R)->val);
        return res > 0 ? 1 : (res < 0 ? -1 : 0);
    } else if (L->type == ks_type_long && R->type == ks_type_int) {
        int64_t iR = ((ks_int)R)->val;
        int res = 0;
        if (iR == (signed long)iR) {
            // compare normally
            res = mpz_cmp_si(((ks_long)L)->val, (signed long)iR);
        } else {
            ks_long lR = ks_long_new(iR);
            res = mpz_cmp(((ks_long)L)->val, lR->val);
            KS_DECREF(lR);

        }
        return res > 0 ? 1 : (res < 0 ? -1 : 0);
    } else if (L->type == ks_type_int && R->type == ks_type_long) {
        int64_t iL = ((ks_int)L)->val;
        int res = 0;
        if (iL == (signed long)iL) {
            // compare normally
            res = -mpz_cmp_si(((ks_long)R)->val, (signed long)iL);
        } else {
            ks_long lL = ks_long_new(iL);
            res = mpz_cmp(lL->val, ((ks_long)R)->val);
            KS_DECREF(lL);

        }
        return res > 0 ? 1 : (res < 0 ? -1 : 0);
    } else if (L->type == ks_type_long && R->type == ks_type_float) {
        int res = mpz_cmp_d(((ks_long)L)->val, ((ks_float)R)->val);
        return res > 0 ? 1 : (res < 0 ? -1 : 0);
    } else if (L->type == ks_type_float && R->type == ks_type_long) {
        int res = -mpz_cmp_d(((ks_long)R)->val, ((ks_float)L)->val);
        return res > 0 ? 1 : (res < 0 ? -1 : 0);
    } else {
        return -2;
    }
}


// long.__cmp__(L, R) -> cmp 2 integers
static KS_TFUNC(long, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF("<=>", L, R);
    } else {
        return (ks_obj)ks_int_new(res);
    }
};


// long.__lt__(L, R) -> cmp 2 integers
static KS_TFUNC(long, lt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF("<", L, R);
    } else {
        return KSO_BOOL(res < 0);
    }
};

// long.__le__(L, R) -> cmp 2 integers
static KS_TFUNC(long, le) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];


    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF("<=", L, R);
    } else {
        return KSO_BOOL(res <= 0);
    }
};

// long.__gt__(L, R) -> cmp 2 integers
static KS_TFUNC(long, gt) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];


    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF(">", L, R);
    } else {
        return KSO_BOOL(res > 0);
    }
};

// long.__ge__(L, R) -> cmp 2 integers
static KS_TFUNC(long, ge) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF(">=", L, R);
    } else {
        return KSO_BOOL(res >= 0);
    }
};

// long.__eq__(L, R) -> cmp 2 integers
static KS_TFUNC(long, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF("==", L, R);
    } else {
        return KSO_BOOL(res == 0);
    }
};

// long.__ne__(L, R) -> cmp 2 integers
static KS_TFUNC(long, ne) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];


    int res = my_longcmp(L, R);

    if (res == -2) {
        KS_ERR_BOP_UNDEF("!=", L, R);
    } else {
        return KSO_BOOL(res != 0);
    }
};


/*

// long.__neg__(V) -> negative int
static KS_TFUNC(long, neg) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_int) {
        return (ks_obj)ks_int_new(-((ks_int)V)->val);
    }

    KS_ERR_UOP_UNDEF("-", V);
};




// long.__sqig__(V) -> sqig int
static KS_TFUNC(long, sqig) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj V = args[0];

    if (V->type == ks_type_int) {
        return (ks_obj)ks_int_new(~((ks_int)V)->val);
    }

    KS_ERR_UOP_UNDEF("~", V);
};

*/

// long.modpow(base, ex, md) -> ret (self ** ex) % md
static KS_TFUNC(long, modpow) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_long base = NULL, ex = NULL, md = NULL;

    if (args[0]->type == ks_type_int) {
        base = ks_long_new(((ks_int)args[0])->val);
    } else if (args[0]->type == ks_type_long) {
        base = (ks_long)KS_NEWREF(args[0]);
    } else {
        return ks_throw_fmt(ks_type_TypeError, "Must have 'int' or 'long' as 'base', not '%T'", args[0]);
    }

    if (args[1]->type == ks_type_int) {
        ex = ks_long_new(((ks_int)args[1])->val);
    } else if (args[1]->type == ks_type_long) {
        ex = (ks_long)KS_NEWREF(args[1]);
    } else {
        KS_DECREF(base);
        return ks_throw_fmt(ks_type_TypeError, "Must have 'int' or 'long' as 'base', not '%T'", args[0]);
    }

    if (args[2]->type == ks_type_int) {
        md = ks_long_new(((ks_int)args[2])->val);
    } else if (args[0]->type == ks_type_long) {
        md = (ks_long)KS_NEWREF(args[2]);
    } else {
        KS_DECREF(base);
        KS_DECREF(ex);
        return ks_throw_fmt(ks_type_TypeError, "Must have 'int' or 'long' as 'md', not '%T'", args[2]);
    }

    ks_long res = ks_long_new(0);

    // calculate it
    mpz_powm(res->val, base->val, ex->val, md->val);

    KS_DECREF(base);
    KS_DECREF(ex);
    KS_DECREF(md);


    return (ks_obj)res;

};



// initialize long type
void ks_type_long_init() {
    KS_INIT_TYPE_OBJ(ks_type_long, "long");

    ks_type_set_cn(ks_type_long, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(long_new_, "long.__new__(obj)")},
        {"__str__", (ks_obj)ks_cfunc_new2(long_str_, "long.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(long_str_, "long.__repr__(self)")},
        {"__hash__", (ks_obj)ks_cfunc_new2(long_hash_, "long.__hash__(self)")},
        
        {"__free__", (ks_obj)ks_cfunc_new2(long_free_, "long.__free__(self)")},

        {"__add__", (ks_obj)ks_cfunc_new2(long_add_, "long.__add__(L, R)")},
        {"__sub__", (ks_obj)ks_cfunc_new2(long_sub_, "long.__sub__(L, R)")},
        {"__mul__", (ks_obj)ks_cfunc_new2(long_mul_, "long.__mul__(L, R)")},
        {"__div__", (ks_obj)ks_cfunc_new2(long_div_, "long.__div__(L, R)")},

        {"__pow__", (ks_obj)ks_cfunc_new2(long_pow_, "long.__pow__(L, R)")},
        {"__mod__", (ks_obj)ks_cfunc_new2(long_mod_, "long.__mod__(L, R)")},

        {"__cmp__", (ks_obj)ks_cfunc_new2(long_cmp_, "long.__cmp__(L, R)")},

        {"__lt__", (ks_obj)ks_cfunc_new2(long_lt_, "long.__lt__(L, R)")},
        {"__le__", (ks_obj)ks_cfunc_new2(long_le_, "long.__le__(L, R)")},
        {"__gt__", (ks_obj)ks_cfunc_new2(long_gt_, "long.__gt__(L, R)")},
        {"__ge__", (ks_obj)ks_cfunc_new2(long_ge_, "long.__ge__(L, R)")},
        {"__eq__", (ks_obj)ks_cfunc_new2(long_eq_, "long.__eq__(L, R)")},
        {"__ne__", (ks_obj)ks_cfunc_new2(long_ne_, "long.__ne__(L, R)")},
        
        
        {"modpow", (ks_obj)ks_cfunc_new2(long_modpow_, "long.modpow(base, ex, md)")},
 
        {NULL, NULL}   
    });
}

