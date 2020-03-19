/* modules/nx/view.c - type describing a 'view' of an array
 *
 * 
 * @author: Cade Brown
 */

#include "nx-impl.h"


// declare the array type as residing in this file
KS_TYPE_DECLFWD(nx_type_view);


/* C API */

// Construct a new array from C-style (packed) data
nx_view nx_view_new_whole(nx_array source) {
    // allocate just the base of the array
    nx_view self = KS_ALLOC_OBJ(nx_view);
    KS_INIT_OBJ(self, nx_type_view);

    self->source = source;
    KS_INCREF(source);

    self->Ndim = source->Ndim;
    self->dims = ks_malloc(sizeof(*self->dims) * source->Ndim);
    self->strides = ks_malloc(sizeof(*self->strides) * source->Ndim);

    // no offset
    self->elem_offset = 0;

    int i;
    for (i = 0; i < source->Ndim; ++i) {
        self->dims[i] = source->dims[i];
        self->strides[i] = source->strides[i];
    }

    return self;
}

// Construct a view over a range from [start, start+dims) of source
nx_view nx_view_new_slice(nx_array source, int Ndim, ks_ssize_t* start, ks_ssize_t* dims) {
    // allocate just the base of the array
    nx_view self = KS_ALLOC_OBJ(nx_view);
    KS_INIT_OBJ(self, nx_type_view);

    self->source = source;
    KS_INCREF(source);

    ks_ssize_t elem_offset = nx_calc_idx(
        source->Ndim, source->dims, source->strides,
        Ndim, start
    );
    // there was an error
    if (elem_offset < 0) return NULL;

    // calculated offset (in elements)
    self->elem_offset = elem_offset;

    self->Ndim = Ndim;
    self->dims = ks_malloc(sizeof(*self->dims) * self->Ndim);
    self->strides = ks_malloc(sizeof(*self->strides) * self->Ndim);

    int i;
    for (i = 0; i < source->Ndim; ++i) {
        self->dims[i] = dims[i];
        self->strides[i] = source->strides[i];
    }

    return self;
}



// Get a single item from the view, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]]
ks_obj nx_view_getitem(nx_view self, int n_idx, ks_ssize_t* idxs) {
    assert(n_idx > 0 && "'nx_view_getitem' given invalid number of indices");
    assert(n_idx <= self->Ndim && "'nx_view_getitem' given too many indices");

    // linear index
    int lidx = nx_calc_idx(self->Ndim, self->dims, self->strides, n_idx, idxs);
    if (lidx < 0) return NULL;


    assert (lidx >= 0 && "Internal index error!");
 
    int sizeof_dtype = nx_dtype_sizeof(self->source->dtype);

    uintptr_t addr = (uintptr_t)self->source->data_ptr;
    addr += (lidx + self->elem_offset) * sizeof_dtype;

    // attempt to convert from binary
    return nx_bin_get(self->source->dtype, (void*)addr);
}


// Set a single item from the view, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]] = obj
// return 0 on success, otherwise for failure
bool nx_view_setitem(nx_view self, int n_idx, ks_ssize_t* idxs, ks_obj obj) {
    assert(n_idx > 0 && "'nx_view_setitem' given invalid number of indices");
    assert(n_idx <= self->Ndim && "'nx_view_setitem' given too many indices");

    // linear index
    int lidx = nx_calc_idx(self->Ndim, self->dims, self->strides, n_idx, idxs);
    if (lidx < 0) return 1;

    assert (lidx >= 0 && "Internal index error!");
 
    int sizeof_dtype = nx_dtype_sizeof(self->source->dtype);

    uintptr_t addr = (uintptr_t)self->source->data_ptr;
    addr += (lidx + self->elem_offset) * sizeof_dtype;

    // check for success
    return nx_bin_set(obj, self->source->dtype, (void*)addr);
}



/* KSCRIPT API */

// nx.view.__new__(arr, start=(,), dims=(,)) -> construct a new view of an array
static KS_TFUNC(view, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 3);
    nx_array arr = (nx_array)args[0];
    KS_REQ_TYPE(arr, nx_type_array, "arr");

    // the number of dimensions, start, and length of the slice
    int Ndim = arr->Ndim;
    ks_ssize_t* start = NULL;
    ks_ssize_t* dims = NULL;
    int i;

    if (n_args > 1) {
        // allow arguments to be passed in
        ks_list l_start = ks_list_from_iterable(args[1]);
        if (!l_start) return NULL;

        if (l_start->len != arr->Ndim) {
            ks_throw_fmt(ks_type_Error, "'start' was %i dimensions, but 'arr' was %i dimensions", l_start->len, arr->Ndim);
            KS_DECREF(l_start);
            return NULL;
        }

        start = ks_malloc(sizeof(*start) * Ndim);
        dims = ks_malloc(sizeof(*start) * Ndim);

        if (n_args == 3) {
            // also, length is passed in
            ks_list l_dim = ks_list_from_iterable(args[2]);
            if (!l_dim) {
                KS_DECREF(l_start);
                return NULL;
            }

            if (l_dim->len != arr->Ndim) {
                ks_throw_fmt(ks_type_Error, "'dim' was %i dimensions, but 'arr' was %i dimensions", l_dim->len, arr->Ndim);
                KS_DECREF(l_start);
                KS_DECREF(l_dim);
                return NULL;
            }

            for (i = 0; i < Ndim; ++i) {
                ks_int start_o = (ks_int)l_start->elems[i];
                KS_REQ_TYPE(start_o, ks_type_int, "start[*]");

                ks_int dim_o = (ks_int)l_dim->elems[i];
                KS_REQ_TYPE(dim_o, ks_type_int, "dims[*]");

                start[i] = start_o->val;
                dims[i] = dim_o->val;
            }

            KS_DECREF(l_dim);

        } else {
            // no end; go all the way to end

            for (i = 0; i < Ndim; ++i) {
                ks_int start_o = (ks_int)l_start->elems[i];
                KS_REQ_TYPE(start_o, ks_type_int, "start[*]");

                start[i] = start_o->val;
                // fill in the rest
                dims[i] = arr->dims[i] - start_o->val;
            }
        }

        KS_DECREF(l_start);


    } else {
        // default, entire array
        Ndim = arr->Ndim;

        start = ks_malloc(sizeof(*start) * Ndim);
        dims = ks_malloc(sizeof(*start) * Ndim);

        for (i = 0; i < Ndim; ++i) {
            start[i] = 0;
            dims[i] = arr->dims[i];
        }

    }

    // construct the slice
    nx_view ret = nx_view_new_slice(arr, 
        Ndim, start, dims
    );

    ks_free(start);
    ks_free(dims);

    return (ks_obj)ret;
}


// nx.view.__free__(self) -> free array and resources
static KS_TFUNC(view, free) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_view self = (nx_view)args[0];
    KS_REQ_TYPE(self, nx_type_view, "self");

    // free buffers
    ks_free(self->dims);
    ks_free(self->strides);

    // dereference the source
    KS_DECREF(self->source);


    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// nx.view.__repr__(self) -> return string representation
static KS_TFUNC(view, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_view self = (nx_view)args[0];
    KS_REQ_TYPE(self, nx_type_view, "self");

    return (ks_obj)ks_fmt_c("<'%T' of %s[%+,z]>", self, nx_dtype_to_cstr(self->source->dtype), self->Ndim, self->dims);
}


// nx.view.__getitem__(self, *idxs) -> return the item at a given index
static KS_TFUNC(view, getitem) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    nx_view self = (nx_view)args[0];
    KS_REQ_TYPE(self, nx_type_view, "self");

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

    ks_obj ret = nx_view_getitem(self, n_args - 1, idxs);
    ks_free(idxs);;
    return ret;

}

// nx.view.__setitem__(self, *idxs, val) -> set the item at a given index
static KS_TFUNC(view, setitem) {
    KS_REQ_N_ARGS_MIN(n_args, 2);
    nx_view self = (nx_view)args[0];
    KS_REQ_TYPE(self, nx_type_view, "self");
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


    nx_view_setitem(self, n_args - 2, idxs, val);

    ks_free(idxs);
    return KS_NEWREF(val);
}

// nx.view.shape(self) -> return the shape of the view
static KS_TFUNC(view, shape) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_view self = (nx_view)args[0];
    KS_REQ_TYPE(self, nx_type_view, "self");

    ks_int* shape_dirs = ks_malloc(sizeof(*shape_dirs) * self->Ndim);

    int i;
    for (i = 0; i < self->Ndim; ++i) {
        shape_dirs[i] = ks_int_new(self->dims[i]);
    }

    ks_tuple res = ks_tuple_new_n(self->Ndim, (ks_obj*)shape_dirs);
    ks_free(shape_dirs);

    return (ks_obj)res;

}



// initialize the view type
void nx_init__view() {
    KS_INIT_TYPE_OBJ(nx_type_view, "nx.view");

    ks_type_set_cn(nx_type_view, (ks_dict_ent_c[]){
        {"__new__",        (ks_obj)ks_cfunc_new2(view_new_, "nx.view.__new__(arr)")},
        {"__free__",       (ks_obj)ks_cfunc_new2(view_free_, "nx.view.__free__(self)")},

        {"__str__",        (ks_obj)ks_cfunc_new2(view_repr_, "nx.view.__str__(self)")},

        {"__getitem__",    (ks_obj)ks_cfunc_new2(view_getitem_, "nx.view.__getitem__(self, *idxs)")},
        {"__setitem__",    (ks_obj)ks_cfunc_new2(view_setitem_, "nx.view.__setitem__(self, *idxs, val)")},

        {"shape",          (ks_obj)ks_cfunc_new2(view_shape_, "nx.view.shape(self)")},

        {NULL, NULL},
    });
}




