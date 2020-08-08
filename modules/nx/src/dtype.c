/* src/dtype.c - data-types
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// forward declaring
KS_TYPE_DECLFWD(nx_T_dtype);

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


// dtype cache, of already existing data-types
static ks_dict dtype_cache = NULL;


nx_dtype nx_dtype_make_int(char* name, int bits, bool isSigned) {
    if (bits % 8 != 0) {
        return ks_throw(ks_T_ArgError, "`CINT` dtype must have bits%%8==0");
    }

    ks_str sname = ks_str_new(name);
    ks_obj ret = ks_dict_get_h(dtype_cache, (ks_obj)sname, sname->v_hash);

    if (ret) {
        if (ret->type == nx_T_dtype) {
            KS_DECREF(sname);
            return (nx_dtype)ret;
        } else {
            KS_DECREF(ret);
        }
    }

    nx_dtype self = KS_ALLOC_OBJ(nx_dtype);
    KS_INIT_OBJ(self, nx_T_dtype);
    
    self->name = sname;

    self->kind = NX_DTYPE_KIND_CINT;
    self->size = bits / 8;
    self->s_cint.isSigned = isSigned;

    ks_dict_set_h(dtype_cache, (ks_obj)sname, sname->v_hash, (ks_obj)self);

    return self;
}


// Make a floating point type
// NOTE: Returns a new reference
nx_dtype nx_dtype_make_fp(char* name, int bits) {
    if (bits != 32 && bits != 64) {
        return ks_throw(ks_T_ArgError, "`CFLOAT` dtype must have bits in (32, 64)");
    }

    ks_str sname = ks_str_new(name);
    ks_obj ret = ks_dict_get_h(dtype_cache, (ks_obj)sname, sname->v_hash);

    if (ret) {
        if (ret->type == nx_T_dtype) {
            KS_DECREF(sname);
            return (nx_dtype)ret;
        } else {
            KS_DECREF(ret);
        }
    }

    nx_dtype self = KS_ALLOC_OBJ(nx_dtype);
    KS_INIT_OBJ(self, nx_T_dtype);
    
    self->name = sname;

    self->kind = NX_DTYPE_KIND_CFLOAT;
    self->size = bits / 8;

    ks_dict_set_h(dtype_cache, (ks_obj)sname, sname->v_hash, (ks_obj)self);

    return self;
}

// Make a floating point complex type
// NOTE: Returns a new reference
nx_dtype nx_dtype_make_cplx(char* name, int bits) {
    if (bits != 32 && bits != 64) {
        return ks_throw(ks_T_ArgError, "`CCOMPLEX` dtype must have bits in (32, 64)");
    }


    ks_str sname = ks_str_new(name);
    ks_obj ret = ks_dict_get_h(dtype_cache, (ks_obj)sname, sname->v_hash);

    if (ret) {
        if (ret->type == nx_T_dtype) {
            KS_DECREF(sname);
            return (nx_dtype)ret;
        } else {
            KS_DECREF(ret);
        }
    }

    nx_dtype self = KS_ALLOC_OBJ(nx_dtype);
    KS_INIT_OBJ(self, nx_T_dtype);
    
    self->name = sname;

    self->kind = NX_DTYPE_KIND_CCOMPLEX;
    self->size = 2 * bits / 8;

    ks_dict_set_h(dtype_cache, (ks_obj)sname, sname->v_hash, (ks_obj)self);

    return self;
}


// dtype.__new__(obj)
static KS_TFUNC(dtype, new) {
    ks_obj obj;
    KS_GETARGS("obj", ks_T_type, &obj);

    if (obj->type == ks_T_str) {
        ks_str sobj = (ks_str)obj;
        ks_obj ret = ks_dict_get_h(dtype_cache, (ks_obj)sobj, sobj->v_hash);
        if (!ret) {
            return ks_throw(ks_T_KeyError, "Unknown dtype: %S", args[0]);
        }
        return ret;

    } else {
        return ks_throw(ks_T_TypeError, "Could not create dtype from object '%S'", obj);
    }
}


// dtype.__free__(self)
static KS_TFUNC(dtype, free) {
    nx_dtype self;
    KS_GETARGS("self:*", &self, nx_T_dtype);

    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// dtype.__str__(self)
static KS_TFUNC(dtype, str) {
    nx_dtype self;
    KS_GETARGS("self:*", &self, nx_T_dtype);

    return KS_NEWREF(self->name);
}

void nx_T_init_dtype() {
    // create cache
    dtype_cache = ks_dict_new(0, NULL);
    ks_type_init_c(nx_T_dtype, "nx.dtype", ks_T_obj, KS_KEYVALS(

        {"__new__", (ks_obj)ks_cfunc_new_c(dtype_new_, "nx.dtype.__new__(obj)")},
        {"__free__", (ks_obj)ks_cfunc_new_c(dtype_free_, "nx.dtype.__free__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new_c(dtype_str_, "nx.dtype.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new_c(dtype_str_, "nx.dtype.__repr__(self)")},

        {"_dtype_cache", (ks_obj)dtype_cache},
    ));


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
