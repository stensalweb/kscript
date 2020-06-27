/* modules/nx/array.c - the tensor/array class definition
 *
 * 
 * @author: Cade Brown
 */

#include "../nx-impl.h"


// declare the array type as residing in this file
KS_TYPE_DECLFWD(nx_type_array);


/* C API */

// Construct a new array from C-style (packed) data
nx_array nx_array_new(int Ndim, ks_ssize_t* dims, void* data_ptr, nx_dtype dtype) {
    assert(Ndim >= 0 && "'nx_array_new' given negative number of dimensions!");
    assert(dtype >= NX_DTYPE__FIRST && dtype <= NX_DTYPE__LAST && "'nx_array_new' given invalid dtype!");

    int sizeof_dtype = nx_dtype_sizeof(dtype);
    assert(sizeof_dtype > 0 && "'nx_array_new': sizeof(dtype) was negative! (there was an error)");

    // allocate just the base of the array
    nx_array self = KS_ALLOC_OBJ(nx_array);
    KS_INIT_OBJ(self, nx_type_array);

    // data type
    self->dtype = dtype;

    // construct with given dimensions
    self->Ndim = Ndim;
    self->dims = ks_malloc(sizeof(*self->dims) * self->Ndim);
    self->strides = ks_malloc(sizeof(*self->strides) * self->Ndim);
    


    // number of elements
    int n_elem = 1, i;

    if (!dims) {
        // given NULL dimensions, default to '0' in all
        for (i = 0; i < Ndim; ++i) self->dims[i] = 0;
        // number of elements will be 0
        n_elem = 0;
    } else {
        // read in from 'dims'
        for (i = 0; i < Ndim; ++i) {
            // each new dimension multiplies the number of elements
            n_elem *= dims[i];
            self->dims[i] = dims[i];
        }
    }

    // fill in the strides
    for (i = 0; i < Ndim; ++i) {
        // initialize to 1
        self->strides[i] = 1;

        int j;
        for (j = i + 1; j < Ndim; ++j) {
            self->strides[i] *= self->dims[j];
        }
    }

    // now, allocate the data
    self->data_ptr = ks_malloc(sizeof_dtype * n_elem);

    // TODO: initialize
    memset(self->data_ptr, 0, sizeof_dtype * n_elem);

    return self;
}



// convert to tensor: fill
static bool tconv_fill(nx_dtype dtype, nx_array* resp, ks_obj cur, int* idx, int dep, ks_ssize_t** dims) {
    if (ks_is_iterable(cur)) {

        // we have a list, so need to continue recursively calling it
        ks_list elems = ks_list_from_iterable(cur);
        if (!elems) return false;

        if (*resp) {
            // already been created, so ensure it is the correct length
            if ((*dims)[dep] != elems->len) {
                ks_throw_fmt(ks_type_SizeError, "Initialize entries had differing size!");
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
            if (!tconv_fill(dtype, resp, elems->elems[i], idx, dep + 1, dims)) {
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
            *resp = nx_array_new(dep, *dims, NULL, dtype);
        } else {
            // already created, ensure we are at maximum depth
            if (dep != (*resp)->Ndim) {
                ks_throw_fmt(ks_type_SizeError, "Initialize entries had differing size!");
                return false;
            }
        }

        ks_ssize_t sizeof_dtype = nx_dtype_sizeof(dtype);
        if (sizeof_dtype < 0) return false;

        // get address of result:
        uintptr_t addr = sizeof_dtype * my_idx + (uintptr_t)(*resp)->data_ptr;

        // now, set the object binarily
        if (!nx_bin_set(cur, dtype, (void*)addr)) {
            return false;
        }
    }

    return true;
}

// convert kscript object to tensor array
nx_array nx_array_from_obj(ks_obj obj, nx_dtype dtype) {
    // assume nothing
    nx_array res = NULL;

    if (ks_is_iterable(obj)) {

        // the current index
        int idx = 0;

        // a list of dimensions (which can be reallocated inside 'tconv_fill')
        ks_ssize_t* dims = NULL;

        // attempt to convert & fill it up
        if (!tconv_fill(dtype, &res, obj, &idx, 0, &dims)) {
            if (res != NULL) KS_DECREF(res);
            ks_free(dims);
            return NULL;
        }

        ks_free(dims);

    } else {


        // construct a 1-dimensional tensor from a single object
        res = nx_array_new(1, (ks_ssize_t[]){1}, NULL, dtype);

        // now, try binary casting it
        if (!nx_bin_set(obj, dtype, res->data_ptr)) {
            KS_DECREF(res);
            return NULL;
        }
    }

    return res;
}

// Get a single item from the array, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]]
ks_obj nx_array_getitem(nx_array self, int n_idx, ks_ssize_t* idxs) {
    assert(n_idx > 0 && "'nx_array_getitem' given invalid number of indices");
    assert(n_idx <= self->Ndim && "'nx_array_getitem' given too many indices");

    // linear index
    int lidx = nx_calc_idx(self->Ndim, self->dims, self->strides, n_idx, idxs);
    if (lidx < 0) return NULL;

    assert (lidx >= 0 && "Internal index error!");
 
    int sizeof_dtype = nx_dtype_sizeof(self->dtype);

    uintptr_t addr = (uintptr_t)self->data_ptr;
    addr += lidx * sizeof_dtype;

    // attempt to convert from binary
    return nx_bin_get(self->dtype, (void*)addr);
}


// Set a single item from the array, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]] = obj
// return 0 on success, otherwise for failure
bool nx_array_setitem(nx_array self, int n_idx, ks_ssize_t* idxs, ks_obj obj) {
    assert(n_idx > 0 && "'nx_array_setitem' given invalid number of indices");
    assert(n_idx <= self->Ndim && "'nx_array_setitem' given too many indices");

    // linear index
    int lidx = nx_calc_idx(self->Ndim, self->dims, self->strides, n_idx, idxs);
    if (lidx < 0) return 1;

    assert (lidx >= 0 && "Internal index error!");
 
    int sizeof_dtype = nx_dtype_sizeof(self->dtype);

    uintptr_t addr = (uintptr_t)self->data_ptr;
    addr += lidx * sizeof_dtype;

    // check for success
    return nx_bin_set(obj, self->dtype, (void*)addr);
}


/* KSCRIPT API */

// nx.array.__new__(elems, dtype=None) -> construct a new array
static KS_TFUNC(array, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj elems = args[0];

    // assume nothing
    nx_dtype dtype = NX_DTYPE_ERR;

    if (n_args == 2 && args[1] != KSO_NONE) {
        // parse out a string dtype
        ks_str dtype_str = (ks_str)args[1];
        KS_REQ_TYPE(dtype_str, ks_type_str, "dtype");

        dtype = nx_dtype_from_cstr(dtype_str->chr);

        if (dtype == NX_DTYPE_ERR) {
            return ks_throw_fmt(ks_type_Error, "Unknown dtype '%S'", dtype_str);
        }

    } else {
        // determine dtype automatically, default: fp32
        dtype = NX_DTYPE_FP32;
    } 

    return (ks_obj)nx_array_from_obj(elems, dtype);
}



// nx.array.__free__(self) -> free array and resources
static KS_TFUNC(array, free) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    // free buffers
    ks_free(self->data_ptr);
    ks_free(self->dims);


    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// nx.array.__repr__(self) -> return string representation
static KS_TFUNC(array, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    return (ks_obj)ks_fmt_c("<'%T' of %s[%+,z]>", self, nx_dtype_to_cstr(self->dtype), self->Ndim, self->dims);
}



// nx.array.__getitem__(self, *idxs) -> return the item at a given index
static KS_TFUNC(array, getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    // make sure there are not too many
    if ((n_args - 1) > self->Ndim) return ks_throw_fmt(ks_type_KeyError, "Too many indices!");

    // for now, disable slices
    if ((n_args - 1) != self->Ndim) return ks_throw_fmt(ks_type_ToDoError, "Slices are not implemented yet!");

    ks_ssize_t* idxs = ks_malloc(sizeof(*idxs) * (n_args - 1));

    int i;
    for (i = 1; i < n_args; ++i) {
        ks_int c_idx_o = (ks_int)args[i];
        KS_REQ_TYPE(c_idx_o, ks_type_int, "idxs");

        idxs[i - 1] = c_idx_o->val;
    }

    ks_obj ret = nx_array_getitem(self, n_args - 1, idxs);
    ks_free(idxs);;
    return ret;

}

// nx.array.__setitem__(self, *idxs, val) -> set the item at a given index
static KS_TFUNC(array, setitem) {
    KS_REQ_N_ARGS_MIN(n_args, 2);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");
    ks_obj val = args[n_args - 1];

    // make sure there are not too many
    if ((n_args - 2) > self->Ndim) return ks_throw_fmt(ks_type_KeyError, "Too many indices!");

    // for now, disable slices
    if ((n_args - 2) != self->Ndim) return ks_throw_fmt(ks_type_ToDoError, "Slices are not implemented yet!");

    ks_ssize_t* idxs = ks_malloc(sizeof(*idxs) * (n_args - 2));

    int i;
    for (i = 1; i < n_args - 1; ++i) {
        ks_int c_idx_o = (ks_int)args[i];
        KS_REQ_TYPE(c_idx_o, ks_type_int, "idxs");

        idxs[i - 1] = c_idx_o->val;
    }


    nx_array_setitem(self, n_args - 2, idxs, val);

    ks_free(idxs);
    return KS_NEWREF(val);
}


static void my_tostr() {

}


// nx.array.__str__(self) -> convert to string
static KS_TFUNC(array, str) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    ks_str_b SB;
    ks_str_b_init(&SB);
    ks_str_b_add(&SB, 1, "[");
    
    // now, generate it out
    if (self->Ndim == 1) {
        // special cases
        int i;
        for (i = 0; i < self->dims[0]; ++i) {
            if (i) ks_str_b_add(&SB, 2, "  ");
            nx_appstr(&SB, self->dtype, (void*)(self->strides[0] * nx_dtype_sizeof(self->dtype) * i + (uintptr_t)self->data_ptr));
        }
    }

    ks_str_b_add(&SB, 1, "]");

    ks_str res = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    return (ks_obj)res;

}




// nx.array.shape(self) -> return the shape of the array
static KS_TFUNC(array, shape) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    ks_int* shape_dirs = ks_malloc(sizeof(*shape_dirs) * self->Ndim);

    int i;
    for (i = 0; i < self->Ndim; ++i) {
        shape_dirs[i] = ks_int_new(self->dims[i]);
    }

    ks_tuple res = ks_tuple_new_n(self->Ndim, (ks_obj*)shape_dirs);
    ks_free(shape_dirs);

    return (ks_obj)res;

}



// initialize the array type
void nx_init__array() {
    KS_INIT_TYPE_OBJ(nx_type_array, "nx.array");

    ks_type_set_cn(nx_type_array, (ks_dict_ent_c[]){
        {"__new__",        (ks_obj)ks_cfunc_new2(array_new_, "nx.array.__new__(elems, dtype=None)")},
        {"__free__",        (ks_obj)ks_cfunc_new2(array_free_, "nx.array.__free__(self)")},

        {"__repr__",       (ks_obj)ks_cfunc_new2(array_repr_, "nx.array.__repr__(self)")},
        {"__str__",        (ks_obj)ks_cfunc_new2(array_str_, "nx.array.__str__(self)")},

        {"__getitem__",    (ks_obj)ks_cfunc_new2(array_getitem_, "nx.array.__getitem__(self, *idxs)")},
        {"__setitem__",    (ks_obj)ks_cfunc_new2(array_setitem_, "nx.array.__setitem__(self, *idxs)")},

        {"shape",          (ks_obj)ks_cfunc_new2(array_shape_, "nx.array.shape(self)")},

        {NULL, NULL},
    });
}




