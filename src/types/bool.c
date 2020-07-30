/* bool.c - implementation of the 'bool' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


static struct ks_bool_s KS_TRUE_s, KS_FALSE_s;
ks_bool KS_TRUE = &KS_TRUE_s, KS_FALSE = &KS_FALSE_s;



// bool.__free__(self) -> free object
static KS_TFUNC(bool, free) {
    ks_bool self = NULL;
    KS_GETARGS("self:*", &self, ks_T_bool)

    // reset references
    self->refcnt = KS_REFS_INF;

    return KSO_NONE;
}

KST_NUM_OPFS(tbool)


/* export */

KS_TYPE_DECLFWD(ks_T_bool);

void ks_init_T_bool() {
    // initialize singletons
    KS_INIT_OBJ(KS_TRUE, ks_T_bool);
    KS_INIT_OBJ(KS_FALSE, ks_T_bool);
    KS_TRUE->val = true;
    KS_FALSE->val = false;

    ks_type_init_c(ks_T_bool, "bool", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(bool_free_, "bool.__free__(self)")},

        KST_NUM_OPKVS(tbool)
    ));

    ks_T_bool->flags |= KS_TYPE_FLAGS_EQSS;

}
