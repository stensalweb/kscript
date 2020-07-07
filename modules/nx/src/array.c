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


// Create a new array with a given data type, and dimensions
nx_array nx_array_new(nxar_t nxar) {
    // create a new result
    nx_array self = KS_ALLOC_OBJ(nx_array);
    KS_INIT_OBJ(self, nx_type_array);
    
    // initialize to empty
    self->dim = self->stride = self->data = NULL;

    self->dtype = nxar.dtype;
    KS_INCREF(nxar.dtype);

    // create size information
    self->rank = nxar.rank;
    self->dim = ks_malloc(sizeof(*self->dim) * self->rank);
    self->stride = ks_malloc(sizeof(*self->stride) * self->rank);

    // copy the given data
    memcpy(self->dim, nxar.dim, sizeof(*self->dim) * self->rank);

    // last dimension is immediate stride
    self->stride[self->rank - 1] = self->dtype->size;

    int i;
    // calculate strides
    for (i = self->rank - 2; i >= 0; --i) {
        self->stride[i] = self->stride[i + 1] * self->dim[i + 1];
    }

    // size of 1 element
    nx_size_t total_sz = self->dtype->size;

    // times all dimensions
    for (i = 0; i < self->rank; ++i) {
        total_sz *= self->dim[i];
    }

    // allocate the data
    self->data = ks_malloc(total_sz);

    if (!self->data) {
        KS_DECREF(self);
        return ks_throw_fmt(ks_type_Error, "Failed to allocate tensor of size [%+z] (%lGB)", self->rank, self->dim, (int64_t)(total_sz / 1e9));
    }

    if (nxar.data) {
        // copy it

        if (!nx_T_cast(
            nxar,
            NXAR_ARRAY(self)
        )) {
            KS_DECREF(self);
            return NULL;
        }

        return self;

    } else {

        // set all to 0
        int tmp0 = 0;

        if (!nx_T_cast(
            (nxar_t){ 
                .data = (void*)&tmp0,
                .dtype = nx_dtype_sint32,
                .rank = 1,
                .dim = (nx_size_t[]){ 1 },
                .stride = (nx_size_t[]){ 0 },
            },
            NXAR_ARRAY(self)
        )) {

            KS_DECREF(self);
            return NULL;
        }

        return self;
    }

}


// recursive internal procedure for filling array from recursive iterators
static bool my_array_fill(nx_dtype dtype, nx_array* resp, ks_obj cur, int* idx, int dep, ks_ssize_t** dims) {
    if (ks_is_iterable(cur)) {
        // we have a list, so need to continue recursively calling it
        ks_list elems = ks_list_from_iterable(cur);
        if (!elems) return false;

        if (*resp) {
            // already been created, so ensure it is the correct length
            if ((*dims)[dep] != elems->len) {
                ks_throw_fmt(ks_type_SizeError, "Initializing entries had differing size!");
                return false;
            }
        } else {
            // set the dimensions
            *dims = ks_realloc(*dims, sizeof(**dims) * (dep + 1));
            (*dims)[dep] = elems->len;
        }


        int i;
        for (i = 0; i < elems->len; ++i) {
            // recursively call more
            if (!my_array_fill(dtype, resp, elems->elems[i], idx, dep + 1, dims)) {
                return false;
            }
        }



        KS_DECREF(elems);

    } else {

        // actually set the next object
        int my_idx = *idx;
        *idx = *idx + 1;

        if (my_idx == 0) {
            // we are the first! create 'resp'
            //*resp = nx_array_new(dtype, dep, *dims, NULL);
            *resp = nx_array_new((nxar_t){ .rank = dep, .dtype = dtype, .dim = *dims, .stride = NULL, .data = NULL });
        } else {
            // already created, ensure we are at maximum depth
            if (dep != (*resp)->rank) {
                ks_throw_fmt(ks_type_SizeError, "Initializing entries had differing size!");
                return false;
            }
        }


        // get address of result:
        intptr_t addr = (intptr_t)(*resp)->data + dtype->size * my_idx;

        // now, cast to C-type
        if (!nx_T_set_all((nxar_t){
            .data = (void*)addr, 
            .dtype = dtype,
            .rank = 1, 
            .dim = (nx_size_t[]){ 1 }, 
            .stride = (nx_size_t[]){ 0 }
        }, cur)) {
            return false;
        }

    }

    return true;
}



// create a nx array from an object
nx_array nx_array_from_obj(ks_obj obj, nx_dtype dtype) {

    if (ks_is_iterable(obj)) {


        // try to auto-set it
        if (dtype == NULL) {
            dtype = nx_dtype_fp32;
        }

        // current dimensional index
        int idx = 0;
        
        // list of dimensions for the tensor (which will be reallocated in 'my_array_fill')
        ks_ssize_t* dims = NULL;

        // result
        nx_array res = NULL;

        // attempt to convert & fill it up
        if (!my_array_fill(dtype, &res, obj, &idx, 0, &dims)) {
            ks_free(dims);
            if (res) KS_DECREF(res);
            return NULL;
        }


        ks_free(dims);

        return res;
    } else {

        // try to auto-set it
        if (dtype == NX_DTYPE_KIND_NONE) {
            if (obj->type == ks_type_int) {
                dtype = nx_dtype_sint64;
            } else if (obj->type == ks_type_float) {
                dtype = nx_dtype_fp64;
            } else if (obj->type == ks_type_complex) {
                dtype = nx_dtype_cplx_fp64;
            } else {
                // default to FP32
                dtype = nx_dtype_fp32;
            }
        }

        // create (1,) array
        nx_array res = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = dtype, 
            .rank = 1, 
            .dim = (nx_size_t[]){ 1 },
            .stride = NULL
        });

        // attempt to set it
        if (!nx_T_set_all(NXAR_ARRAY(res), obj)) {
            KS_DECREF(res);
            return NULL;
        }

        return res;
    }
}


// array.__new__(obj, dtype=none)
static KS_TFUNC(array, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj obj;
    nx_dtype dtype = NULL;
    if (!ks_parse_params(n_args, args, "obj%any ?dtype%*", &obj, &dtype, nx_type_dtype)) return NULL;

    // use the creation routine
    return (ks_obj)nx_array_from_obj(obj, dtype);

}

// array.__free__(self)
static KS_TFUNC(array, free) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_array)) return NULL;

    ks_free(self->data);
    ks_free(self->dim);
    ks_free(self->stride);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// array.__str__(self)
static KS_TFUNC(array, str) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_array)) return NULL;

    return (ks_obj)nx_get_str(NXAR_ARRAY(self));
}

// array.__getattr__(self, attr)
static KS_TFUNC(array, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    nx_array self;
    ks_str attr;
    if (!ks_parse_params(n_args, args, "self%* attr%s", &self, nx_type_array, &attr)) return NULL;


    // attempt to get one of the attributes
    if (attr->len == 5 && strncmp(attr->chr, "shape", 5) == 0) {
        return (ks_obj)ks_build_tuple("%+z", self->rank, self->dim);
    } else if (attr->len == 6 && strncmp(attr->chr, "stride", 6) == 0) {
        return (ks_obj)ks_build_tuple("%+z", self->rank, self->stride);
    } else if (attr->len == 5 && strncmp(attr->chr, "dtype", 5) == 0) {
        return KS_NEWREF(self->dtype);
    } else {

        // now, try getting a member function
        ks_obj ret = ks_type_get_mf(self->type, attr, (ks_obj)self);
        if (!ret) {
            KS_ERR_ATTR(self, attr);
        }

        return ret;
    }
}

// array.__getitem__(self, *idxs)
// TODO; move the actual functionality to a C-API function
static KS_TFUNC(array, getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    return nx_nxar_getitem(NXAR_ARRAY(self), n_args-1, args+1);
}


/* OPERATORS */


// array.__add__(L, R)
static KS_TFUNC(array, add) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_add->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("+", args[0], args[1]);
    } else {
        return ret;
    }
}

// array.__sub__(L, R)
static KS_TFUNC(array, sub) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_sub->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("-", args[0], args[1]);
    } else {
        return ret;
    }
}

// array.__mul__(L, R)
static KS_TFUNC(array, mul) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_mul->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("*", args[0], args[1]);
    } else {
        return ret;
    }
}

// array.__div__(L, R)
static KS_TFUNC(array, div) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_div->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("/", args[0], args[1]);
    } else {
        return ret;
    }
}




void nx_type_array_init() {
    KS_INIT_TYPE_OBJ(nx_type_array, "nx.array");

    ks_type_set_cn(nx_type_array, (ks_dict_ent_c[]) {

        {"__new__", (ks_obj)ks_cfunc_new2(array_new_, "nx.array.__new__(obj, dtype=none)")},
        {"__str__", (ks_obj)ks_cfunc_new2(array_str_, "nx.array.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(array_str_, "nx.array.__repr__(self)")},

        {"__free__", (ks_obj)ks_cfunc_new2(array_free_, "nx.array.__free__(self)")},

        {"__getattr__", (ks_obj)ks_cfunc_new2(array_getattr_, "nx.array.__getattr__(self, attr)")},

        {"__getitem__", (ks_obj)ks_cfunc_new2(array_getitem_, "nx.array.__getitem__(self, *idxs)")},


        /* ops */
        {"__add__",        (ks_obj)ks_cfunc_new2(array_add_, "nx.array.__add__(L, R)")},
        {"__sub__",        (ks_obj)ks_cfunc_new2(array_sub_, "nx.array.__sub__(L, R)")},
        {"__mul__",        (ks_obj)ks_cfunc_new2(array_mul_, "nx.array.__mul__(L, R)")},
        {"__div__",        (ks_obj)ks_cfunc_new2(array_div_, "nx.array.__div__(L, R)")},

        {NULL, NULL}
    });
}

