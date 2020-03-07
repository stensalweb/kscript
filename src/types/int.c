/* types/int.c - kscript's basic integer implementation (signed 64 bit) 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_int);

// create a kscript int from a C-style int
ks_int ks_int_new(int64_t val) {
    ks_int self = KS_ALLOC_OBJ(ks_int);
    KS_INIT_OBJ(self, ks_type_int);

    // initialize type-specific things
    self->val = val;

    return self;
}

/* member functions */


// int.__str__(self) -> free an int object
static KS_TFUNC(int, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    // print it out
    char cstr[256];
    snprintf(cstr, 255, "%lli", (long long int)self->val);

    return (ks_obj)ks_str_new(cstr);

};


// int.__free__(self) -> free an int object
static KS_TFUNC(int, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_int self = (ks_int)args[0];
    KS_REQ_TYPE(self, ks_type_int, "self");

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};



// int.__add__(self) -> add 2 integers
static KS_TFUNC(int, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val + ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("+", L, R);
};


// int.__sub__(self) -> subtract 2 integers
static KS_TFUNC(int, sub) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val - ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("-", L, R);
};

// int.__mul__(self) -> multiply 2 integers
static KS_TFUNC(int, mul) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val * ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("*", L, R);
};


// int.__div__(self) -> divide 2 integers
static KS_TFUNC(int, div) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_int && R->type == ks_type_int) {
        return (ks_obj)ks_int_new(((ks_int)L)->val / ((ks_int)R)->val);
    }

    KS_ERR_BOP_UNDEF("/", L, R);
};






// initialize int type
void ks_type_int_init() {
    KS_INIT_TYPE_OBJ(ks_type_int, "int");

    ks_type_set_cn(ks_type_int, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(int_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(int_str_)},
        
        {"__free__", (ks_obj)ks_cfunc_new(int_free_)},
 
        {"__add__", (ks_obj)ks_cfunc_new(int_add_)},
        {"__sub__", (ks_obj)ks_cfunc_new(int_sub_)},
        {"__mul__", (ks_obj)ks_cfunc_new(int_mul_)},
        {"__div__", (ks_obj)ks_cfunc_new(int_div_)},
 
        {NULL, NULL}   
    });
}

