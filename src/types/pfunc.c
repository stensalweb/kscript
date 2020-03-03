/* types/pfunc.c - Partial function implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_pfunc);

// create a kscript int from a C-style int
ks_pfunc ks_new_pfunc(ks_obj func) {
    // assume it is callable
    assert(ks_is_callable(func));

    ks_pfunc self = KS_ALLOC_OBJ(ks_pfunc);
    KS_INIT_OBJ(self, ks_type_pfunc);

    // initialize type-specific things
    self->func = KS_NEWREF(func);

    // start with nothing
    self->n_fill = 0;
    self->fill = NULL;

    return self;
}

void ks_pfunc_fill(ks_pfunc self, int idx, ks_obj arg) {

    KS_INCREF(arg);

    int i;
    for (i = 0; i < self->n_fill; ++i) {
        if (self->fill[i].idx == idx) {
            // replace it here
            KS_DECREF(self->fill[i].arg);
            self->fill[i].arg = arg;
            return;
        } else {
            // insert it here, so the list is sorted

            int j = self->n_fill++;
            self->fill = ks_realloc(self->fill, sizeof(*self->fill) * self->n_fill);

            // shift everything forward
            while (j > i) {
                self->fill[j] = self->fill[j - 1];
                j--;
            }

            // set the interior one
            self->fill[i].idx = idx;
            self->fill[i].arg = arg;

            return;

        }

    }

    // not found, so append it
    i = self->n_fill++;
    self->fill = ks_realloc(self->fill, sizeof(*self->fill) * self->n_fill);

    //set it
    self->fill[i].idx = idx;
    self->fill[i].arg = arg;

}

// free a kscript cfunc
void ks_free_pfunc(ks_pfunc self) {

    int i;
    for (i = 0; i < self->n_fill; ++i) {
        KS_DECREF(self->fill[i].arg);
    }

    ks_free(self->fill);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
}



/* member functions */


// pfunc.__call__(self, *args) -> call a pfunc with arguments
static KS_TFUNC(pfunc, call) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    ks_pfunc self = (ks_pfunc)args[0];
    KS_REQ_TYPE(self, ks_type_pfunc, "self");

    // skip first argument, which is the pfunc
    args++;
    n_args--;
    
    // combine the filled in args with what is passed
    int new_n_args = self->n_fill + n_args;

    // fill in this temporary buffer
    ks_obj* new_args = ks_malloc(sizeof(*new_args) * new_n_args);

    // index into the self->fill array
    int fi = 0;

    // index into the passed-in arguments
    int ai = 0;

    int i;
    for (i = 0; i < new_n_args; ++i) {
        if (fi < self->n_fill && self->fill[fi].idx == i) {
            // fill current index with a fill argument
            new_args[i] = self->fill[fi++].arg;
        } else if (ai < n_args) {
            // otherwise, fill in from the arguments
            new_args[i] = args[ai++];
        } else {
            // error: not called with enough arguments
            ks_free(new_args);
            return NULL;
        }
    }

    ks_obj ret = ks_call(self->func, new_n_args, new_args);

    ks_free(new_args);

    return ret;
};


// pfunc.__free__(self) -> free a pfunc object
static KS_TFUNC(pfunc, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_pfunc self = (ks_pfunc)args[0];
    KS_REQ_TYPE(self, ks_type_pfunc, "self");
    
    // actually free the object
    ks_free_pfunc(self);

    return KSO_NONE;
};


// initialize pfunc type
void ks_type_pfunc_init() {
    KS_INIT_TYPE_OBJ(ks_type_pfunc, "pfunc");

    ks_type_set_cn(ks_type_pfunc, (ks_dict_ent_c[]){
        {"__call__", (ks_obj)ks_new_cfunc(pfunc_call_)},
        {"__free__", (ks_obj)ks_new_cfunc(pfunc_free_)},
        {NULL, NULL}   
    });

}

