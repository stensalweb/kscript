/* obj.c - constructing/managing objects

Also, this holds some constants (like interned strings, small integers, etc)

*/

#include "kscript.h"


// TODO: have this method return early and just throw the pointer onto another thread which is just the GC thread
// Need to figure out multithreading first, though
bool kso_free(kso obj) {

    // don't free an immortal, or something still being referenced
    if (obj->flags & KSOF_IMMORTAL || obj->refcnt > 0) return false;

    ks_trace("ks_freeing obj %p [type %s]", obj, obj->type->name._);
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
