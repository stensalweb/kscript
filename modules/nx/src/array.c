/* modules/nx/array.c - the tensor/array class definition
 *
 * 
 * @author: Cade Brown
 */

#include "nx-impl.h"


// declare the array type as residing in this file
KS_TYPE_DECLFWD(nx_type_array);



/* C API */

// Construct a new array
nx_array nx_array_new(int n_dim, int* dims, void* data_ptr, nx_dtype dtype) {
    assert(n_dim >= 0 && "'nx_array_new' given negative number of dimensions!");
    assert(dtype >= NX_DTYPE__FIRST && dtype <= NX_DTYPE__LAST && "'nx_array_new' given invalid dtype!");

    int sizeof_dtype = nx_dtype_sizeof(dtype);
    assert(sizeof_dtype > 0 && "'nx_array_new': sizeof(dtype) was negative! (there was an error)");

    // allocate just the base of the array
    nx_array self = KS_ALLOC_OBJ(nx_array);
    KS_INIT_OBJ(self, nx_type_array);

    // data type
    self->dtype = dtype;

    // construct with given dimensions
    self->n_dim = n_dim;
    self->dims = ks_malloc(sizeof(*self->dims) * self->n_dim);

    int i;

    int n_elem = 1;

    if (!dims) {
        // given NULL dimensions, default to '0' in all
        for (i = 0; i < n_dim; ++i) self->dims[i] = 0;
        // number of elements will be 0
        n_elem = 0;
    } else {
        // read in from 'dims'
        for (i = 0; i < n_dim; ++i) {
            // each new dimension multiplies the number of elements
            n_elem *= dims[i];
            self->dims[i] = dims[i];
        }
    }


    // now, allocate the data
    self->data_ptr = ks_malloc(sizeof_dtype * n_elem);

    // TODO: initialize
    memset(self->data_ptr, 0, sizeof_dtype * n_elem);

    return self;
}


// Get a single item from the array, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]]
ks_obj nx_array_getitem(nx_array self, int n_idx, int* idxs) {
    assert(n_idx > 0 && "'nx_array_getitem' given invalid number of indices");
    assert(n_idx <= self->n_dim && "'nx_array_getitem' given too many indices");

    // linear index
    int lidx = nx_calc_idx(self->n_dim, self->dims, n_idx, idxs);
    if (lidx < 0) return NULL;

    assert (lidx >= 0 && "Internal index error!");
 
    int sizeof_dtype = nx_dtype_sizeof(self->dtype);

    uintptr_t addr = (uintptr_t)self->data_ptr;
    addr += lidx * sizeof_dtype;

    // get a binary cast of it
    return nx_bincast(self->dtype, (void*)addr);
}


// Set a single item from the array, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]] = obj
// return 0 on success, otherwise for failure
int nx_array_setitem(nx_array self, int n_idx, int* idxs, ks_obj obj) {
    assert(n_idx > 0 && "'nx_array_setitem' given invalid number of indices");
    assert(n_idx <= self->n_dim && "'nx_array_setitem' given too many indices");

    // linear index
    int lidx = nx_calc_idx(self->n_dim, self->dims, n_idx, idxs);
    if (lidx < 0) return 1;

    assert (lidx >= 0 && "Internal index error!");
 
    int sizeof_dtype = nx_dtype_sizeof(self->dtype);

    uintptr_t addr = (uintptr_t)self->data_ptr;
    addr += lidx * sizeof_dtype;


    if (nx_tobin(self->dtype, obj, (void*)addr) != 0) return 1;

    // success
    return 0;
}





/* KSCRIPT API */

// nx.array.__new__(elems, dtype=None) -> construct a new array
static KS_TFUNC(array, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj elems = args[0];
    KS_REQ_ITERABLE(elems, "elems");

    // assume nothing
    nx_dtype dtype = NX_DTYPE_ERR;

    if (n_args == 2 && args[2] != KSO_NONE) {
        // parse out a string dtype
        ks_str dtype_str = (ks_str)args[2];
        KS_REQ_TYPE(dtype_str, ks_type_str, "dtype");

        dtype = nx_dtype_from_cstr(dtype_str->chr);

        if (dtype == NX_DTYPE_ERR) {
            return ks_throw_fmt(ks_type_Error, "Unknown dtype '%S'", dtype_str);
        }

    } else {
        // determine dtype automatically, default: fp32
        dtype = NX_DTYPE_FP32;
    } 

    // now, we have a valid dtype, so check through 'elems' to see
    // TODO: iterate through

    return (ks_obj)nx_array_new(2, (int[]){2, 2}, NULL, dtype);
}

// nx.array.__repr__(self) -> return string representation
static KS_TFUNC(array, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    ks_str_b SB;
    ks_str_b_init(&SB);

    ks_str_b_add_fmt(&SB, "<'%T' [", self);

    int i;
    for (i = 0; i < self->n_dim; ++i) {
        if (i != 0) ks_str_b_add_c(&SB, ",");
        ks_str_b_add_fmt(&SB, "%i", self->dims[i]);
    }

    ks_str_b_add_fmt(&SB, "] of %s", nx_dtype_to_cstr(self->dtype));

    ks_str_b_add_c(&SB, ">");

    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    return (ks_obj)ret;
}



// nx.array.__getitem__(self, *idxs) -> return the item at a given index
static KS_TFUNC(array, getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    nx_array self = (nx_array)args[0];
    KS_REQ_TYPE(self, nx_type_array, "self");

    int* idxs = ks_malloc(sizeof(*idxs) * (n_args - 1));

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

    int* idxs = ks_malloc(sizeof(*idxs) * (n_args - 2));

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

// initialize the array type
void nx_init__array() {
    KS_INIT_TYPE_OBJ(nx_type_array, "nx.array");

    ks_type_set_cn(nx_type_array, (ks_dict_ent_c[]){
        {"__new__",        (ks_obj)ks_cfunc_new2(array_new_, "nx.array.__new__(elems, dtype=None)")},

        {"__repr__",       (ks_obj)ks_cfunc_new2(array_repr_, "nx.array.__repr__(self)")},
        {"__str__",        (ks_obj)ks_cfunc_new2(array_repr_, "nx.array.__str__(self)")},

        {"__getitem__",    (ks_obj)ks_cfunc_new2(array_getitem_, "nx.array.__getitem__(self, *idxs)")},
        {"__setitem__",    (ks_obj)ks_cfunc_new2(array_setitem_, "nx.array.__setitem__(self, *idxs)")},

        {NULL, NULL},
    });
}




