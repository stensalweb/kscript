/* src/view - implementation of the `nx.view` type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// forward declaring
KS_TYPE_DECLFWD(nx_T_view);

// Create a new view
nx_view nx_view_new(ks_obj ref, nxar_t nxar) {
    // create a new result
    nx_view self = KS_ALLOC_OBJ(nx_view);
    KS_INIT_OBJ(self, nx_T_view);

    self->data = nxar.data;
    self->rank = nxar.rank;
    self->dtype = nxar.dtype;
    KS_INCREF(nxar.dtype);
    self->dim = ks_malloc(sizeof(*self->dim) * nxar.rank);
    self->stride = ks_malloc(sizeof(*self->stride) * nxar.rank);

    // TODO: perhaps allow '-1' arguments in dim for the enter size of 'ref'?
    int i;
    for (i = 0; i < nxar.rank; ++i) {
        self->dim[i] = nxar.dim[i];
        self->stride[i] = nxar.stride[i];
    }

    self->data_src = KS_NEWREF(ref);

    return self;
}


// nx.view.__new__(obj)
static KS_TFUNC(view, new) {
    ks_obj obj;
    KS_GETARGS("obj", &obj);

    if (obj->type == nx_T_view) {
        return KS_NEWREF(obj);
    } else if (obj->type == nx_T_array) {
        nx_array obj_arr = (nx_array)obj;
        return (ks_obj)nx_view_new((ks_obj)obj_arr, NXAR_ARRAY(obj_arr));
    } else {
        KS_THROW_TYPE_ERR(obj, nx_T_view);
    }
}

// nx.view.__free__(self)
static KS_TFUNC(view, free) {
    nx_view self;
    KS_GETARGS("self:*", &self, nx_T_view);

    KS_DECREF(self->data_src);

    ks_free(self->dim);
    ks_free(self->stride);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}



// view.__getattr__(self, attr)
static KS_TFUNC(view, getattr) {
    nx_view self;
    ks_str attr;
    KS_GETARGS("self:* attr:*", &self, nx_T_view, &attr, ks_T_str);

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

// nx.view.__str__(obj)
static KS_TFUNC(view, str) {
    nx_view self;
    KS_GETARGS("self:*", &self, nx_T_view);
    return (ks_obj)nx_get_str(NXAR_VIEW(self));
}

// view.__getitem__(self, *idxs)
static KS_TFUNC(view, getitem) {
    nx_view self;
    int n_idxs;
    ks_obj* idxs;
    KS_GETARGS("self:* *idxs", &self, nx_T_view, &n_idxs, &idxs);

    return nx_nxar_getitem(NXAR_VIEW(self), n_idxs, idxs);
}



/* OPERATORS */

// view.__add__(L, R)
static KS_TFUNC(view, add) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_add->func(n_args, args);
}

// view.__sub__(L, R)
static KS_TFUNC(view, sub) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_sub->func(n_args, args);
}

// view.__mul__(L, R)
static KS_TFUNC(view, mul) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_mul->func(n_args, args);
}

// view.__div__(L, R)
static KS_TFUNC(view, div) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_div->func(n_args, args);
}

// view.__pow__(L, R)
static KS_TFUNC(view, pow) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R);

    return nx_F_pow->func(n_args, args);
}


void nx_T_init_view() {

    ks_type_init_c(nx_T_view, "nx.view", ks_T_object, KS_KEYVALS(

        {"__new__",           (ks_obj)ks_cfunc_new_c(view_new_, "nx.view.__new__(obj)")},
        {"__free__",           (ks_obj)ks_cfunc_new_c(view_free_, "nx.view.__free__(self)")},
        {"__str__",           (ks_obj)ks_cfunc_new_c(view_str_, "nx.view.__str__(self)")},

        {"__getattr__",           (ks_obj)ks_cfunc_new_c(view_getattr_, "nx.view.__getattr__(self, attr)")},
        {"__getitem__",           (ks_obj)ks_cfunc_new_c(view_getitem_, "nx.view.__getitem__(self, *idxs)")},

        {"__add__",           (ks_obj)ks_cfunc_new_c(view_add_, "nx.view.__add__(L, R)")},
        {"__sub__",           (ks_obj)ks_cfunc_new_c(view_sub_, "nx.view.__sub__(L, R)")},
        {"__mul__",           (ks_obj)ks_cfunc_new_c(view_mul_, "nx.view.__mul__(L, R)")},
        {"__div__",           (ks_obj)ks_cfunc_new_c(view_div_, "nx.view.__div__(L, R)")},
        {"__pow__",           (ks_obj)ks_cfunc_new_c(view_pow_, "nx.view.__pow__(L, R)")},
    ));

}

