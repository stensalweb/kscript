/* obj.c - constructing/managing objects

Also, this holds some constants (like interned strings, small integers, etc)

*/

#include "kscript.h"


kso_obj kso_obj_new() {
    kso_obj ret = (kso_obj)ks_malloc(sizeof(*ret));
    ret->type = kso_T_obj;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->v_attrs = KS_DICT_EMPTY;
    return ret;
}

// TODO: have this method return early and just throw the pointer onto another thread which is just the GC thread
// Need to figure out multithreading first, though
bool kso_free(kso obj) {

    // don't free an immortal, or something still being referenced
    if (obj->flags & KSOF_IMMORTAL || obj->refcnt > 0) return false;

    ks_trace("ks_free(<'%s' obj @ %p>)", obj->type->name._, obj);
    kso f_free = obj->type->f_free;

    if (f_free != NULL) {
        if (f_free->type == kso_T_cfunc) {
            KSO_CAST(kso_cfunc, f_free)->v_cfunc(NULL, 1, &obj);
        } else {
            ks_warn("Something other than cfunc in kso_free");
        }
    }

    ks_free(obj);

    return true;
}
