/* src/array.c - implementation of the array class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// forward declaring
KS_TYPE_DECLFWD(nx_type_array);
KS_TYPE_DECLFWD(nx_type_view);


// Create a new array with a given data type, and dimensions
nx_array nx_array_new(enum nx_dtype dtype, int N, nx_size_t* dim) {
    // create a new result
    nx_array self = KS_ALLOC_OBJ(nx_array);
    KS_INIT_OBJ(self, nx_type_array);
    
    // initialize to empty
    self->dim = self->stride = self->data = NULL;

    self->dtype = dtype;

    // create size information
    self->N = N;
    self->dim = ks_malloc(sizeof(*self->dim) * self->N);
    self->stride = ks_malloc(sizeof(*self->stride) * self->N);

    // copy the given data
    memcpy(self->dim, dim, sizeof(*self->dim) * self->N);

    // last dimension is immediate stride
    self->stride[self->N - 1] = 1;

    int i;
    // calculate strides
    for (i = self->N - 2; i >= 0; --i) {
        self->stride[i] = self->stride[i + 1] * self->dim[i + 1];
    }

    // size of 1 element
    nx_size_t total_sz = nx_dtype_size(self->dtype);

    // times all dimensions
    for (i = 0; i < self->N; ++i) {
        total_sz *= self->dim[i];
    }

    // allocate the data
    self->data = ks_malloc(total_sz);

    if (!self->data) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_Error, "Failed to allocate tensor of size [%+z] (%lGB)", self->N, self->dim, (int64_t)(total_sz / 1e9));
    }

    return self;
}




// array.__new__(obj, dtype=nx.FP32)
static KS_TFUNC(array, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj obj;
    ks_Enum dtype = nx_FP32;
    if (!ks_parse_params(n_args, args, "obj%any ?dtype%any", &obj, &dtype)) return NULL;

    nx_array self = nx_array_new(NX_DTYPE_FP32, 2, (nx_size_t[]){ 10, 5 });

    nx_T_set_all(self->dtype, self->data, self->N, self->dim, self->stride, obj);

    return (ks_obj)self;
}



// array.__str__(self)
static KS_TFUNC(array, str) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_array)) return NULL;

    return (ks_obj)nx_get_str(self->dtype, self->data, self->N, self->dim, self->stride);
}




void nx_type_array_init() {
    KS_INIT_TYPE_OBJ(nx_type_array, "array");

    ks_type_set_cn(nx_type_array, (ks_dict_ent_c[]) {

        {"__new__", (ks_obj)ks_cfunc_new2(array_new_, "array.__new__(obj, dtype=nx.FP32)")},
        {"__str__", (ks_obj)ks_cfunc_new2(array_str_, "array.__str__(self)")},

        {NULL, NULL}
    });
}

