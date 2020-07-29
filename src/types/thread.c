/* thread.c - implementation of the 'thread' type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

ks_thread ks_thread_main = NULL;


// construct a new kscript thread
// if 'name==NULL', then a name is generated
KS_API ks_thread ks_thread_new(const char* name, ks_obj target, int n_args, ks_obj* args) {
    ks_thread self = KS_ALLOC_OBJ(ks_thread);
    KS_INIT_OBJ(self, ks_T_thread);

    ks_str new_name = NULL;
    if (name == NULL) {
        new_name = ks_fmt_c("thread%p", self);
    } else {
        new_name = ks_str_new_c(name, -1);
    }

    self->name = new_name;
    self->target = target;
    if (target) KS_INCREF(target);

    // copy arguments
    self->n_args = n_args;
    self->args = ks_malloc(sizeof(*self->args) * self->n_args);
    memcpy(self->args, args, sizeof(*self->args) * self->n_args);

    self->stk = ks_list_new(0, NULL);

    self->frames = ks_list_new(0, NULL);

    self->exc = NULL;
    self->exc_info = NULL;

    return self;
}

// get thread
// TODO: use pthreads
ks_thread ks_thread_get() {
    return ks_thread_main;
}

// thread.__free__(self) -> free object
static KS_TFUNC(thread, free) {
    ks_thread self = NULL;
    KS_GETARGS("self:*", &self, ks_T_thread)


    KS_DECREF(self->name);
    KS_DECREF(self->target);

    ks_free(self->args);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_thread);

void ks_init_T_thread() {
    ks_type_init_c(ks_T_thread, "thread", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(thread_free_, "thread.__free__(self)")},
    ));

    // create the main thread as well
    ks_thread_main = ks_thread_new("main", NULL, 0, NULL);

}
