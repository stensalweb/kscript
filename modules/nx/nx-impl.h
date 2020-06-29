/* modules/nx/nx-impl.h - the internal implementation header for the 'nx' (numerics) library
 *
 * This includes internal routines, static functions, and defines not meant to be exported by the library
 * 
 * @author: Cade Brown
 */

#pragma once
#ifndef NX_IMPL_H__

// include main library
#include "./nx.h"


#define NX_ASSERT_CHECK(_expr) { if (!(_expr)) { fprintf(stderr, "INTERNAL NX ASSERT ERROR: " #_expr "\n"); exit(1); } }

// return the size of a given dtype
static int nx_dtype_size(enum nx_dtype dtype) {
    /*  */ if (dtype == NX_DTYPE_SINT8) {
        return sizeof(int8_t);
    } else if (dtype == NX_DTYPE_UINT8) {
        return sizeof(uint8_t);
    } else if (dtype == NX_DTYPE_SINT16) {
        return sizeof(int16_t);
    } else if (dtype == NX_DTYPE_UINT16) {
        return sizeof(uint16_t);
    } else if (dtype == NX_DTYPE_SINT32) {
        return sizeof(int32_t);
    } else if (dtype == NX_DTYPE_UINT32) {
        return sizeof(uint32_t);
    } else if (dtype == NX_DTYPE_SINT64) {
        return sizeof(int64_t);
    } else if (dtype == NX_DTYPE_UINT64) {
        return sizeof(uint64_t);
    } else if (dtype == NX_DTYPE_FP32) {
        return sizeof(float);
    } else if (dtype == NX_DTYPE_FP64) {
        return sizeof(double);
    } else if (dtype == NX_DTYPE_CPLX_FP32) {
        return sizeof(float complex);
    } else if (dtype == NX_DTYPE_CPLX_FP64) {
        return sizeof(double complex);
    } else {
        // indicates error
        ks_throw_fmt(ks_type_TypeError, "Invalid nx dtype enumeration '%i'", (int)dtype);
        return 0;
    }
}

// return a tuple of a size array
static ks_tuple nx_wrap_szar(int N, nx_size_t* sz) {

    ks_tuple res = ks_tuple_new_n(N, NULL);

    int i;
    for (i = 0; i < N; ++i) {
        res->elems[i] = (ks_obj)ks_int_new(sz[i]);
    }

    return res;
}


// cast an object to a specific dtype
static bool nx_cast_to(ks_obj obj, enum nx_dtype dtype, nx_any_t* to) {

    // cast-to error
    #define CTER() { \
        ks_catch_ignore(); \
        ks_throw_fmt(ks_type_TypeError, "Could not cast '%T' object to nx type '%s'", obj, nx_dtype_get_name(dtype)); \
        return false; \
    }

    // C-style values
    int64_t v64;
    double vfp;
    double complex vcfp;
    /*  */ if (dtype == NX_DTYPE_SINT8) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_sint8 = v64;
    } else if (dtype == NX_DTYPE_UINT8) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_uint8 = v64;
    } else if (dtype == NX_DTYPE_SINT16) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_sint16 = v64;
    } else if (dtype == NX_DTYPE_UINT16) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_uint16 = v64;
    } else if (dtype == NX_DTYPE_SINT32) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_sint32 = v64;
    } else if (dtype == NX_DTYPE_UINT32) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_uint32 = v64;
    } else if (dtype == NX_DTYPE_SINT64) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_sint64 = v64;
    } else if (dtype == NX_DTYPE_UINT64) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        to->v_uint64 = v64;

    } else if (dtype == NX_DTYPE_FP32) {
        if (!ks_num_get_double(obj, &vfp)) CTER()
        to->v_fp32 = vfp;
    } else if (dtype == NX_DTYPE_FP64) {
        if (!ks_num_get_double(obj, &vfp)) CTER()
        to->v_fp64 = vfp;

    } else if (dtype == NX_DTYPE_CPLX_FP32) {
        if (!ks_num_get_double_complex(obj, &vcfp)) CTER()
        to->v_cplx_fp64 = vcfp;

    } else if (dtype == NX_DTYPE_CPLX_FP64) {
        if (!ks_num_get_double_complex(obj, &vcfp)) CTER()
        to->v_cplx_fp64 = vcfp;

    } else {
        // indicates error
        ks_throw_fmt(ks_type_TypeError, "Could not cast '%T' object to nx type '%s'", obj, nx_dtype_get_name(dtype));
        return false;
    }
    return true;

    #undef CTER
}


// set blocks of memory, i.e.
// memset(&dest[stride * i], data, size)
static void nx_memset_block(void* dest, void* data, nx_size_t size, nx_size_t stride, nx_size_t n_elems) {

    intptr_t dest_p = (intptr_t)dest;

    nx_size_t i;
    for (i = 0; i < n_elems; ++i, dest_p += stride) {

        memcpy((void*)dest_p, data, size);
    }

}

// enumeration value
extern ks_Enum
    nx_SINT8,
    nx_UINT8,
    nx_SINT16,
    nx_UINT16,
    nx_SINT32,
    nx_UINT32,
    nx_SINT64,
    nx_UINT64,

    nx_FP32,
    nx_FP64,

    nx_CPLX_FP32,
    nx_CPLX_FP64
;



/** INTERNAL ROUTINES FOR INITIALIZATION **/

void nx_type_array_init();


#endif
