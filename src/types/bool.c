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

// bool.__str__(self) -> convert to string
static KS_TFUNC(bool, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_bool self = (ks_bool)args[0];
    KS_REQ_TYPE(self, ks_type_bool, "self");

    // static globals so it only calculates once
    static ks_str s_true = NULL, s_false = NULL;

    // only update once
    if (!s_true) {
        s_true = ks_str_new("true");
        s_false = ks_str_new("false");
    }


    // select which one
    ks_obj ret = self == KS_TRUE ? s_true : s_false;
    return KS_NEWREF(ret);
};



// bool.__free__(self) -> do nothing, as bools should never be freed
static KS_TFUNC(bool, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_bool self = (ks_bool)args[0];
    KS_REQ_TYPE(self, ks_type_bool, "self");

    // up the refcnt so it won't be freed for a while
    self->refcnt = INT32_MAX;

    return KSO_NONE;
};




// initialize bool type
void ks_type_bool_init() {
    KS_INIT_TYPE_OBJ(ks_type_bool, "bool");

    ks_type_set_cn(ks_type_bool, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(bool_str_)},

        {"__free__", (ks_obj)ks_cfunc_new(bool_free_)},
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

