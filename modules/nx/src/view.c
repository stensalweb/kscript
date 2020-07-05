/* src/view - implementation of the `nx.view` type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// forward declaring
KS_TYPE_DECLFWD(nx_type_view);

// Create a new view
nx_view nx_view_new(nx_array ref, nxar_t nxar) {
    // create a new result
    nx_view self = KS_ALLOC_OBJ(nx_view);
    KS_INIT_OBJ(self, nx_type_view);

    self->data = nxar.data;
    self->N = nxar.N;
    self->dtype = ref->dtype;
    self->dim = ks_malloc(sizeof(*self->dim) * nxar.N);
    self->stride = ks_malloc(sizeof(*self->stride) * nxar.N);

    // TODO: perhaps allow '-1' arguments in dim for the enter size of 'ref'?
    int i;
    for (i = 0; i < nxar.N; ++i) {
        self->dim[i] = nxar.dim[i];
        self->stride[i] = nxar.stride[i];
    }

    self->data_src = (nx_array)KS_NEWREF(ref);

    return self;
}


// nx.view.__new__(obj)
static KS_TFUNC(view, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];
    if (obj->type == nx_type_view) {
        return KS_NEWREF(obj);
    } else if (obj->type == nx_type_array) {
        nx_array obj_arr = (nx_array)obj;
        return (ks_obj)nx_view_new(obj_arr, NXAR_ARRAY(obj_arr));
    } else {
        KS_ERR_CONV(obj, nx_type_view);
    }
}

// nx.view.__free__(self)
static KS_TFUNC(view, free) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_view self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_view)) return NULL;

    KS_DECREF(self->data_src);

    ks_free(self->dim);
    ks_free(self->stride);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}



// view.__getattr__(self, attr)
static KS_TFUNC(view, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    nx_view self;
    ks_str attr;
    if (!ks_parse_params(n_args, args, "self%* attr%s", &self, nx_type_view, &attr)) return NULL;

    // attempt to get one of the attributes
    if (attr->len == 5 && strncmp(attr->chr, "shape", 5) == 0) {
        return (ks_obj)ks_build_tuple("%+z", self->N, self->dim);
    } else if (attr->len == 6 && strncmp(attr->chr, "stride", 6) == 0) {
        return (ks_obj)ks_build_tuple("%+z", self->N, self->stride);
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

// nx.view.__str__(obj)
static KS_TFUNC(view, str) {
    KS_REQ_N_ARGS(n_args, 1);
    nx_view self;
    if (!ks_parse_params(n_args, args, "self%*", &self, nx_type_view)) return NULL;

    return (ks_obj)nx_get_str(NXAR_VIEW(self));
}




/* OPERATORS */


// view.__add__(L, R)
static KS_TFUNC(view, add) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_add->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("+", args[0], args[1]);
    } else {
        return ret;
    }
}

// view.__sub__(L, R)
static KS_TFUNC(view, sub) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_sub->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("-", args[0], args[1]);
    } else {
        return ret;
    }
}

// view.__mul__(L, R)
static KS_TFUNC(view, mul) {
    printf("TEST\n");

    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_mul->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("*", args[0], args[1]);
    } else {
        return ret;
    }
}

// view.__div__(L, R)
static KS_TFUNC(view, div) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_obj ret = nx_F_div->func(n_args, args);
    if (!ret) {
        ks_catch_ignore();
        KS_ERR_BOP_UNDEF("/", args[0], args[1]);
    } else {
        return ret;
    }
}



void nx_type_view_init() {
    KS_INIT_TYPE_OBJ(nx_type_view, "nx.view");

    ks_type_set_cn(nx_type_view, (ks_dict_ent_c[]) {

        {"__new__",           (ks_obj)ks_cfunc_new2(view_new_, "nx.view.__new__(obj)")},
        {"__free__",           (ks_obj)ks_cfunc_new2(view_free_, "nx.view.__free__(self)")},
        {"__str__",           (ks_obj)ks_cfunc_new2(view_str_, "nx.view.__str__(self)")},

        {"__getattr__",           (ks_obj)ks_cfunc_new2(view_getattr_, "nx.view.__getattr__(self, attr)")},

        {"__add__",           (ks_obj)ks_cfunc_new2(view_add_, "nx.view.__add__(L, R)")},
        {"__sub__",           (ks_obj)ks_cfunc_new2(view_sub_, "nx.view.__sub__(L, R)")},
        {"__mul__",           (ks_obj)ks_cfunc_new2(view_mul_, "nx.view.__mul__(L, R)")},
        {"__div__",           (ks_obj)ks_cfunc_new2(view_div_, "nx.view.__div__(L, R)")},


        {NULL, NULL}
    });
}

