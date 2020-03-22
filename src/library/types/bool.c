/* types/bool.c - implementation of the boolean (true/false) type for kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_bool);

ks_bool KS_TRUE, KS_FALSE;

/* member functions */


// bool.__free__(self) -> do nothing, as bools should never be freed
static KS_TFUNC(bool, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_bool self = (ks_bool)args[0];
    KS_REQ_TYPE(self, ks_type_bool, "self");

    // up the refcnt so it won't be freed for a while
    self->refcnt = INT32_MAX;

    return KSO_NONE;
};

// bool.__str__(self) -> convert to string
static KS_TFUNC(bool, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_bool self = (ks_bool)args[0];
    KS_REQ_TYPE(self, ks_type_bool, "self");

    // singletons only initialized once
    static ks_str c_true = NULL, c_false = NULL;
    if (!c_true) {
        c_true = ks_str_new("true");
        c_false = ks_str_new("false");
    }
    return (ks_obj)(self == KS_TRUE ? c_true : c_false);
};




// initialize bool type
void ks_type_bool_init() {
    KS_INIT_TYPE_OBJ(ks_type_bool, "bool");

    ks_type_set_cn(ks_type_bool, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new2(bool_str_, "bool.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(bool_str_, "bool.__repr__(self)")},

        {"__free__", (ks_obj)ks_cfunc_new2(bool_free_, "bool.__free__(self)")},
        {NULL, NULL}
    });


    // initialize global singletons
    KS_TRUE = KS_ALLOC_OBJ(ks_bool);
    KS_FALSE = KS_ALLOC_OBJ(ks_bool);

    KS_INIT_OBJ(KS_TRUE, ks_type_bool);
    KS_INIT_OBJ(KS_FALSE, ks_type_bool);

    KS_TRUE->val = true;
    KS_TRUE->val = false;

}

