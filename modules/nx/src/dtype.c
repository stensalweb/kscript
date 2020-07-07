/* src/dtype.c - data-types
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// forward declaring
KS_TYPE_DECLFWD(nx_type_dtype);

// dtypes
nx_dtype
    nx_dtype_sint8,
    nx_dtype_uint8,
    nx_dtype_sint16,
    nx_dtype_uint16,
    nx_dtype_sint32,
    nx_dtype_uint32,
    nx_dtype_sint64,
    nx_dtype_uint64,

    nx_dtype_fp32,
    nx_dtype_fp64,

    nx_dtype_cplx_fp32,
    nx_dtype_cplx_fp64
;



nx_dtype nx_dtype_make_int(char* name, int bits, bool isSigned) {

    if (bits % 8 != 0) {
        return ks_throw_fmt(ks_type_ArgError, "`CINT` dtype must have bits%%8==0");
    }

    nx_dtype self = KS_ALLOC_OBJ(nx_dtype);
    KS_INIT_OBJ(self, nx_type_dtype);
    
    self->name = ks_str_new(name);

    self->kind = NX_DTYPE_KIND_CINT;
    self->size = bits / 8;
    self->s_cint.isSigned = isSigned;

    return self;
}


// Make a floating point type
// NOTE: Returns a new reference
nx_dtype nx_dtype_make_fp(char* name, int bits) {
    if (bits != 32 && bits != 64) {
        return ks_throw_fmt(ks_type_ArgError, "`CFLOAT` dtype must have bits in (32, 64)");
    }

    nx_dtype self = KS_ALLOC_OBJ(nx_dtype);
    KS_INIT_OBJ(self, nx_type_dtype);
    
    self->name = ks_str_new(name);

    self->kind = NX_DTYPE_KIND_CFLOAT;
    self->size = bits / 8;

    return self;
}

// Make a floating point complex type
// NOTE: Returns a new reference
nx_dtype nx_dtype_make_cplx(char* name, int bits) {
    if (bits != 32 && bits != 64) {
        return ks_throw_fmt(ks_type_ArgError, "`CCOMPLEX` dtype must have bits in (32, 64)");
    }

    nx_dtype self = KS_ALLOC_OBJ(nx_dtype);
    KS_INIT_OBJ(self, nx_type_dtype);
    
    self->name = ks_str_new(name);

    self->kind = NX_DTYPE_KIND_CCOMPLEX;
    self->size = 2 * bits / 8;

    return self;
}





// dtype.__free__(self)
static KS_TFUNC(dtype, free) {
    nx_dtype self = (nx_dtype)args[0];
    KS_REQ_TYPE(self, nx_type_dtype, "self");

    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// dtype.__str__(self)
static KS_TFUNC(dtype, str) {
    nx_dtype self = (nx_dtype)args[0];
    KS_REQ_TYPE(self, nx_type_dtype, "self");

    return KS_NEWREF(self->name);
}

void nx_type_dtype_init() {
    KS_INIT_TYPE_OBJ(nx_type_dtype, "nx.dtype");

    ks_type_set_cn(nx_type_dtype, (ks_dict_ent_c[]) {
        {"__str__", (ks_obj)ks_cfunc_new2(dtype_str_, "nx.dtype.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new2(dtype_str_, "nx.dtype.__repr__(self)")},

        {"__free__", (ks_obj)ks_cfunc_new2(dtype_free_, "nx.dtype.__free__(self)")},

        {NULL, NULL}
    });

    nx_dtype_sint8 = nx_dtype_make_int("sint8", 8, true);
    nx_dtype_uint8 = nx_dtype_make_int("uint8", 8, false);
    nx_dtype_sint16 = nx_dtype_make_int("sint16", 16, true);
    nx_dtype_uint16 = nx_dtype_make_int("uint16", 16, false);
    nx_dtype_sint32 = nx_dtype_make_int("sint32", 32, true);
    nx_dtype_uint32 = nx_dtype_make_int("uint32", 32, false);
    nx_dtype_sint64 = nx_dtype_make_int("sint64", 64, true);
    nx_dtype_uint64 = nx_dtype_make_int("uint64", 64, false);

    nx_dtype_fp32 = nx_dtype_make_fp("fp32", 32);
    nx_dtype_fp64 = nx_dtype_make_fp("fp64", 64);

    nx_dtype_cplx_fp32 = nx_dtype_make_cplx("cplx_fp32", 32);
    nx_dtype_cplx_fp64 = nx_dtype_make_cplx("cplx_fp64", 64);

}
