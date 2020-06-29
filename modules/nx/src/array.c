/* src/array.c - implementation of the `nx.array` type
 *
 * Essentially, the array type is a dense array (i.e. all data is contiguous) that has a shape (a tuple of sizes of each dimensions), and a defined type (see `enum nx_dtype` for examples)
 * 
 * It can be thought of as an N-dimensional tensor. Right-most are the least significant dimensions
 *
 * Although, most functions in the `nx` library don't take an `nx_array`, they take a function signature that contains:
 * 
 *   ...(..., enum nx_dtype dtype, void* data, int N, nx_size_t* dim, nx_size_t* stride, ...)
 *
 * Which can be called by an array, or a 'view', or some other data structure. This allows the most generic interface possible, so that is preferred
 * 
 * 
 * 
 * 'dim' sizes are in elements, and 'stride' sizes are also in elements
 * 
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


    // initialize to 0
    ks_int ks0 = ks_int_new(0);
    nx_T_set_all(_NXAR_(self), (ks_obj)ks0);
    KS_DECREF(ks0);

    return self;
}

// create a nx array from an object
nx_array nx_array_from_obj(ks_obj obj, enum nx_dtype dtype) {

    if (ks_is_iterable(obj)) {

        return ks_throw_fmt(ks_type_ToDoError, "arrays from iterables not created yet");
    } else {

        // create (1,) array
        nx_array res = nx_array_new(dtype, 1, (nx_size_t[]){ 1 });

        // attempt to set it
        if (!nx_T_set_all(_NXAR_(res), obj)) {
            KS_DECREF(res);
            return NULL;
        }

        return res;
    }
}


// array.__new__(obj, dtype=nx.FP32)
static KS_TFUNC(array, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj obj;
    ks_Enum dtype = nx_FP32;
    if (!ks_parse_params(n_args, args, "obj%any ?dtype%*", &obj, &dtype, nx_enum_dtype)) return NULL;


    // use the creation routine
    return (ks_obj)nx_array_from_obj(obj, (enum nx_dtype)dtype->enum_idx);

}

// array.__free__(self)
static KS_TFUNC(array, free) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_array)) return NULL;

    ks_free(self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// array.__str__(self)
static KS_TFUNC(array, str) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_array)) return NULL;

    return (ks_obj)nx_get_str(_NXAR_(self));
}

// array.__getattr__(self, attr)
static KS_TFUNC(array, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    nx_array self;
    ks_str attr;
    if (!ks_parse_params(n_args, args, "self%* attr%s", &self, nx_type_array, &attr)) return NULL;


    // attempt to get one of the attributes
    if (attr->len == 5 && strncmp(attr->chr, "shape", 5) == 0) {
        return (ks_obj)ks_build_tuple("%+z", self->N, self->dim);
    } else if (attr->len == 5 && strncmp(attr->chr, "dtype", 5) == 0) {
        return (ks_obj)nx_dtype_get_enum(self->dtype);
    } else {

        // now, try getting a member function
        ks_obj ret = ks_type_get_mf(self->type, attr, (ks_obj)self);
        if (!ret) {
            KS_ERR_ATTR(self, attr);
        }

        return ret;
    }
}


void nx_type_array_init() {
    KS_INIT_TYPE_OBJ(nx_type_array, "nx.array");

    ks_type_set_cn(nx_type_array, (ks_dict_ent_c[]) {

        {"__new__", (ks_obj)ks_cfunc_new2(array_new_, "nx.array.__new__(obj, dtype=nx.FP32)")},
        {"__str__", (ks_obj)ks_cfunc_new2(array_str_, "nx.array.__str__(self)")},

        {"__free__", (ks_obj)ks_cfunc_new2(array_free_, "nx.array.__free__(self)")},

        {"__getattr__", (ks_obj)ks_cfunc_new2(array_getattr_, "nx.array.__getattr__(self, attr)")},

        {NULL, NULL}
    });
}

