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

// Create a new array with a given data type, and dimensions
nx_array nx_array_new(nxar_t nxar) {
    // create a new result
    nx_array self = KS_ALLOC_OBJ(nx_array);
    KS_INIT_OBJ(self, nx_T_array);
    
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
        return ks_throw(ks_T_Error, "Failed to allocate tensor of size [%+z] (%lGB)", self->rank, self->dim, (int64_t)(total_sz / 1e9));
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
        int32_t tmp0 = 0;

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
static bool my_array_fill(nx_dtype dtype, nx_array* resp, ks_obj cur, int* idx, int dep, int* max_dep, ks_ssize_t** dims) {

    if (dep > *max_dep) *max_dep = dep;

    if (cur->type == nx_T_array || cur->type == nx_T_view) {
        ks_throw(ks_T_TodoError, "Need to implement sub-objects in array filling");

        return false;
        nxar_t cur_ar;
        ks_list refadd = ks_list_new(0, NULL);
        if (!nx_get_nxar(cur, &cur_ar, refadd)) {
            KS_DECREF(refadd);
            return false;
        }

        if (*resp) {

        } else {
            
        }

        // now, spread it out

        KS_DECREF(refadd);
    } else if (ks_obj_is_iterable(cur)) {
        // we have a list, so need to continue recursively calling it
        ks_list elems = ks_list_new_iter(cur);
        if (!elems) return false;

        if (*resp) {
            // already been created, so ensure it is the correct length
            if ((*dims)[dep] != elems->len) {
                ks_throw(ks_T_SizeError, "Initializing entries had differing size!");
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
            if (!my_array_fill(dtype, resp, elems->elems[i], idx, dep + 1, max_dep, dims)) {
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
                ks_throw(ks_T_SizeError, "Initializing entries had differing size!");
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

    if (dep == 0 && !*resp) {
        // fill in
        *resp = nx_array_new((nxar_t){ .rank = *max_dep + 1, .dtype = dtype, .dim = *dims, .stride = NULL, .data = NULL });
    }

    return true;
}


// create a nx array from an object
nx_array nx_array_from_obj(ks_obj obj, nx_dtype dtype) {

    if (obj->type == nx_T_array || obj->type == nx_T_view) {
        nxar_t obj_ar;
        ks_list refadd = ks_list_new(0, NULL);
        if (!nx_get_nxar(obj, &obj_ar, refadd)) {
            KS_DECREF(refadd);
            return NULL;
        }
        nx_array res = nx_array_new(obj_ar);
        KS_DECREF(refadd);

        return res;

    } else if (ks_obj_is_iterable(obj)) {

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

        int max_dep = 0;
        // attempt to convert & fill it up
        if (!my_array_fill(dtype, &res, obj, &idx, 0, &max_dep, &dims)) {
            ks_free(dims);
            if (res) KS_DECREF(res);
            return NULL;
        }

        ks_free(dims);

        return res;
    } else {


        // try to auto-set it
        if (dtype == NX_DTYPE_KIND_NONE) {
            if (obj->type == ks_T_int) {
                dtype = nx_dtype_sint64;
            } else if (obj->type == ks_T_float) {
                dtype = nx_dtype_fp64;
            } else if (obj->type == ks_T_complex) {
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
    ks_obj obj;
    nx_dtype dtype = NULL;
    KS_GETARGS("obj ?dtype:*", &obj, &dtype, nx_T_dtype)

    // use the creation routine
    return (ks_obj)nx_array_from_obj(obj, dtype);

}

// array.__free__(self)
static KS_TFUNC(array, free) {
    nx_array self;
    KS_GETARGS("self:*", &self, nx_T_array)

    ks_free(self->data);
    ks_free(self->dim);
    ks_free(self->stride);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// array.__str__(self)
static KS_TFUNC(array, str) {
    nx_array self;
    KS_GETARGS("self:*", &self, nx_T_array)

    return (ks_obj)nx_get_str(NXAR_ARRAY(self));
}

// array.__getattr__(self, attr)
static KS_TFUNC(array, getattr) {
    nx_array self;
    ks_str attr;
    KS_GETARGS("self:* attr:*", &self, nx_T_array, &attr, ks_T_str)

    // attempt to get one of the attributes
    if (ks_str_eq_c(attr, "shape", 5)) {
        return (ks_obj)ks_build_tuple("%+z", self->rank, self->dim);
    } else if (ks_str_eq_c(attr, "stride", 6)) {
        return (ks_obj)ks_build_tuple("%+z", self->rank, self->stride);
    } else if (ks_str_eq_c(attr, "rank", 4)) {
        return (ks_obj)ks_int_new(self->rank);
    } else if (ks_str_eq_c(attr, "dtype", 5)) {
        return KS_NEWREF(self->dtype);
    }

    KS_THROW_ATTR_ERR(self, attr);
}

// array.__getitem__(self, *idxs)
// TODO; move the actual functionality to a C-API function
static KS_TFUNC(array, getitem) {
    nx_array self;
    int n_idxs;
    ks_obj* idxs;
    KS_GETARGS("self:* *idxs", &self, nx_T_array, &n_idxs, &idxs)


    return nx_nxar_getitem(NXAR_ARRAY(self), n_idxs, idxs);
}


// array.__setitem__(self, *idxs)
// TODO; move the actual functionality to a C-API function
static KS_TFUNC(array, setitem) {
    nx_array self;
    int n_idxs;
    ks_obj* idxs;
    KS_GETARGS("self:* *idxs", &self, nx_T_array, &n_idxs, &idxs)

    if (n_idxs < 1) {
        return ks_throw(ks_T_ArgError, "Expected at least one 'idx'/'value' in setitem");
    }

    return KSO_BOOL(nx_nxar_setitem(NXAR_ARRAY(self), n_idxs - 1, idxs, args[n_args - 1]));
}



/* OPERATORS */


// array.__add__(L, R)
static KS_TFUNC(array, add) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);
    return nx_F_add->func(n_args, args);
}

// array.__sub__(L, R)
static KS_TFUNC(array, sub) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_sub->func(n_args, args);
}

// array.__mul__(L, R)
static KS_TFUNC(array, mul) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_mul->func(n_args, args);
}

// array.__div__(L, R)
static KS_TFUNC(array, div) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_div->func(n_args, args);
}

// array.__pow__(L, R)
static KS_TFUNC(array, pow) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_pow->func(n_args, args);
}

KS_TYPE_DECLFWD(nx_T_array);


void nx_T_init_array() {
    ks_type_init_c(nx_T_array, "nx.array", ks_T_object, KS_KEYVALS(

        {"__new__",                (ks_obj)ks_cfunc_new_c_old(array_new_, "nx.array.__new__(obj, dtype=none)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(array_str_, "nx.array.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c_old(array_str_, "nx.array.__repr__(self)")},

        {"__free__",               (ks_obj)ks_cfunc_new_c_old(array_free_, "nx.array.__free__(self)")},

        {"__getattr__",            (ks_obj)ks_cfunc_new_c_old(array_getattr_, "nx.array.__getattr__(self, attr)")},

        {"__getitem__",            (ks_obj)ks_cfunc_new_c_old(array_getitem_, "nx.array.__getitem__(self, *idxs)")},
        {"__setitem__",            (ks_obj)ks_cfunc_new_c_old(array_setitem_, "nx.array.__setitem__(self, *idxs, val)")},


        /* ops */
        {"__add__",                (ks_obj)ks_cfunc_new_c_old(array_add_, "nx.array.__add__(L, R)")},
        {"__sub__",                (ks_obj)ks_cfunc_new_c_old(array_sub_, "nx.array.__sub__(L, R)")},
        {"__mul__",                (ks_obj)ks_cfunc_new_c_old(array_mul_, "nx.array.__mul__(L, R)")},
        {"__div__",                (ks_obj)ks_cfunc_new_c_old(array_div_, "nx.array.__div__(L, R)")},
        {"__pow__",                (ks_obj)ks_cfunc_new_c_old(array_pow_, "nx.array.__pow__(L, R)")},
    ));
}

