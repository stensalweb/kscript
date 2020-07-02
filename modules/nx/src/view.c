/* src/view - implementation of the `nx.view` type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// forward declaring
KS_TYPE_DECLFWD(nx_type_view);

// Create a new view
nx_view nx_view_new(nx_array ref, void* data, int N, nx_size_t* dim, nx_size_t* stride) {
    // create a new result
    nx_view self = KS_ALLOC_OBJ(nx_view);
    KS_INIT_OBJ(self, nx_type_view);

    self->data = data;
    self->N = N;
    self->dtype = ref->dtype;
    self->dim = ks_malloc(sizeof(*self->dim) * N);
    self->stride = ks_malloc(sizeof(*self->stride) * N);

    // TODO: perhaps allow '-1' arguments in dim for the enter size of 'ref'?
    int i;
    for (i = 0; i < N; ++i) {
        self->dim[i] = dim ? dim[i] : ref->dim[i];
        self->stride[i] = stride ? stride[i] : stride[i];
    }

    self->data_src = (nx_array)KS_NEWREF(ref);

    return self;
}


// nx.view.__new__(obj)
static KS_TFUNC(view, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];
    if (obj->type == nx_type_array) {
        return (ks_obj)nx_view_new(((nx_array)obj), ((nx_array)obj)->data, ((nx_array)obj)->N, NULL, NULL);
    } else {
        KS_ERR_CONV(obj, nx_type_view);
    }
}

// nx.view.__str__(obj)
static KS_TFUNC(view, str) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_view self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_view)) return NULL;

    return (ks_obj)nx_get_str(_NXAR_(self));
}

void nx_type_view_init() {
    KS_INIT_TYPE_OBJ(nx_type_view, "nx.view");

    ks_type_set_cn(nx_type_view, (ks_dict_ent_c[]) {

        {"__new__",           (ks_obj)ks_cfunc_new2(view_new_, "nx.view.__new__(obj)")},
        {"__str__",           (ks_obj)ks_cfunc_new2(view_str_, "nx.view.__str__(self)")},

        {NULL, NULL}
    });
}

