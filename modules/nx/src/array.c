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
nx_array nx_array_new(enum nx_dtype dtype, int N, nx_size_t* dim, void* data) {
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


    if (data) {
        // copy given data
        memcpy(self->data, data, total_sz);
    } else {
        // initialize to 0
        ks_int ks0 = ks_int_new(0);
        nx_T_set_all(_NXAR_(self), (ks_obj)ks0);
        KS_DECREF(ks0);
    }


    return self;
}


// recursive internal procedure for filling array from recursive iterators
static bool my_array_fill(enum nx_dtype dtype, nx_array* resp, ks_obj cur, int* idx, int dep, ks_ssize_t** dims) {
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
            *resp = nx_array_new(dtype, dep, *dims, NULL);
        } else {
            // already created, ensure we are at maximum depth
            if (dep != (*resp)->N) {
                ks_throw_fmt(ks_type_SizeError, "Initializing entries had differing size!");
                return false;
            }
        }

        ks_ssize_t sizeof_dtype = nx_dtype_size(dtype);
        if (sizeof_dtype < 0) return false;

        // get address of result:
        uintptr_t addr = sizeof_dtype * my_idx + (uintptr_t)(*resp)->data;

        // now, cast to C-type
        if (!nx_T_set_all((void*)addr, dtype, 1, (nx_size_t[]){ 1 }, (nx_size_t[]){ 0 }, cur)) {
            return false;
        }
    }

    return true;
}



// create a nx array from an object
nx_array nx_array_from_obj(ks_obj obj, enum nx_dtype dtype) {

    if (ks_is_iterable(obj)) {

        // try to auto-set it
        if (dtype == NX_DTYPE_NONE) {
            dtype = NX_DTYPE_FP32;
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
        if (dtype == NX_DTYPE_NONE) {
            if (obj->type == ks_type_int) {
                dtype = NX_DTYPE_SINT64;
            } else if (obj->type == ks_type_float) {
                dtype = NX_DTYPE_FP64;
            } else if (obj->type == ks_type_complex) {
                dtype = NX_DTYPE_CPLX_FP64;
            } else {
                // default to FP32
                dtype = NX_DTYPE_FP32;
            }
        }

        // create (1,) array
        nx_array res = nx_array_new(dtype, 1, (nx_size_t[]){ 1 }, NULL);

        // attempt to set it
        if (!nx_T_set_all(_NXAR_(res), obj)) {
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
    ks_Enum dtype = NULL;
    if (!ks_parse_params(n_args, args, "obj%any ?dtype%*", &obj, &dtype, nx_enum_dtype)) return NULL;


    // use the creation routine
    return (ks_obj)nx_array_from_obj(obj, dtype ? (enum nx_dtype)dtype->enum_idx : NX_DTYPE_NONE);

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



// array.__getitem__(self, *idxs)
static KS_TFUNC(array, getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    nx_array self = args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");


    if ((n_args - 1) == 0) {
        // return view of the entire array
        return (ks_obj)nx_view_new(self, self->data, self->N, self->dim, self->stride);
    } else if ((n_args - 1) <= self->N) {
        // these number of arguments
        int nN = n_args - 1;

        // the index, or -1 if we need to calculate a slice
        int64_t* idxs = ks_malloc(nN * sizeof(*idxs));

        // whether or not slices are required
        bool needSlice = false;

        int i;
        for (i = 0; i < nN; ++i) {
            if (!ks_num_get_int64(args[i + 1], &idxs[i])) {
                // not found as int
                ks_catch_ignore();
                idxs[i] = -1;
                needSlice = true;
            } else {

                // wrap around
                idxs[i] = ((idxs[i] % self->dim[i]) + self->dim[i]) % self->dim[i];
            }
        }


        if (!needSlice && nN == self->N) {
            // return single element
            void* addr = (void*)((intptr_t)self->data + nx_szsdot(nN, self->dim, self->stride, nx_dtype_size(self->dtype), idxs));
            ks_free(idxs);

            return nx_cast_from(self->dtype, addr);
        } else {

            // result dimension is the number of indexes which were not integers + those not referenced
            int rN = self->N - n_args + 1;
            for (i = 0; i < nN; ++i) if (idxs[i] < 0) rN++;

            // otherwise, calculate slices & return view
            nx_size_t* dim = ks_malloc(sizeof(*dim) * rN);
            nx_size_t* stride = ks_malloc(sizeof(*stride) * rN);

            // offset from the base pointer
            nx_size_t total_offset = 0;

            // dtype size
            nx_size_t dtsz = nx_dtype_size(self->dtype);


            // return index
            int ri = 0;

            for (i = 0; i < nN; ++i) {


                if (idxs[i] >= 0) {
                    // single index here, just bump the array off
                    total_offset += dtsz * idxs[i] * self->stride[i];

                    //dim[ri] = self->dim[i];
                    //stride[ri] = self->stride[i];

                    //ri++;
                } else if (args[i + 1]->type == ks_type_slice) {
                    // add dimension from slice argument
                    ks_slice arg_slice = (ks_slice)args[i + 1];

                    int64_t first, last, delta;

                    if (!ks_slice_getci(arg_slice, self->dim[i], &first, &last, &delta)) {

                        // error
                        ks_free(idxs);
                        ks_free(dim);
                        ks_free(stride);

                        return NULL;
                    }

                    // otherwise, calculate number of elements
                    int num = (last - first) / delta;
                    if (num == 0) {
                        // TODO: determine what to do
                        ks_free(idxs);
                        ks_free(dim);
                        ks_free(stride);

                        return ks_throw_fmt(ks_type_ToDoError, "Need to determine what to return if size is 0 in a dimension");
                    }

                    // there will be this many
                    dim[ri] = num;

                    // stride offset
                    stride[ri] = self->stride[i] * delta;

                    // and add total offset to the first
                    total_offset += dtsz * first * self->stride[i];

                    // claim this dimension
                    ri++;
                } else {
                    ks_free(idxs);
                    ks_free(dim);
                    ks_free(stride);
                    return ks_throw_fmt(ks_type_TypeError, "Expected all indices to be either 'int' or 'slice', but got '%T'", args[i + 1]);

                }
            }
            
            ks_free(idxs);

            // fill out the rest, copying
            while (ri < rN) {

                dim[ri] = self->dim[i];
                stride[ri] = self->stride[i];

                i++;
                ri++;
            }

            nx_view ret = nx_view_new(self, (void*)((intptr_t)self->data + total_offset), rN, dim, stride);
            ks_free(dim);
            ks_free(stride);
            return (ks_obj)ret;
        }

    } else {
        return ks_throw_fmt(ks_type_KeyError, "nx.array[...] expected %i indices (for %iD array), but only got %i", self->N, self->N, n_args - 1);
    }

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

