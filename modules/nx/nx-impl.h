/* modules/nx/nx-impl.h - the internal implementation header for the 'nx' (numerics) library
 *
 * This includes internal routines, static functions, and defines not meant to be exported by the library
 * 
 * @author: Cade Brown
 */

#pragma once
#ifndef NX_IMPL_H__
#define NX_IMPL_H__

// include main library
#include "./nx.h"

// templating library
#include "./gen/nxt.h"


/* EXPANSION MACROS */



//#define NX_ASSERT_CHECK(_expr) { if (!(_expr)) { fprintf(stderr, "INTERNAL NX ASSERT ERROR: " #_expr "\n"); exit(1); } }
#define NX_ASSERT_CHECK assert

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
static bool nx_cast_to(ks_obj obj, nx_dtype dtype, void* to) {

    // cast-to error
    #define CTER() { \
        ks_catch_ignore(); \
        ks_throw(ks_T_TypeError, "Could not cast '%T' object to nx type '%S'", obj, dtype); \
        return false; \
    }

    // C-style values
    int64_t v64;
    double vfp;
    double complex vcfp;
    /*  */ if (dtype == nx_dtype_sint8) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(int8_t*)to = v64;
    } else if (dtype == nx_dtype_uint8) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(uint8_t*)to = v64;
    } else if (dtype == nx_dtype_sint16) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(int16_t*)to = v64;
    } else if (dtype == nx_dtype_uint16) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(uint16_t*)to = v64;
    } else if (dtype == nx_dtype_sint32) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(int32_t*)to = v64;
    } else if (dtype == nx_dtype_uint32) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(uint32_t*)to = v64;
    } else if (dtype == nx_dtype_sint64) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(int64_t*)to = v64;
    } else if (dtype == nx_dtype_uint64) {
        if (!ks_num_get_int64(obj, &v64)) CTER()
        *(uint64_t*)to = v64;

    } else if (dtype == nx_dtype_fp32) {
        if (!ks_num_get_double(obj, &vfp)) CTER()
        *(float*)to = vfp;
    } else if (dtype == nx_dtype_fp64) {
        if (!ks_num_get_double(obj, &vfp)) CTER()
        *(double*)to = vfp;

    } else if (dtype == nx_dtype_cplx_fp32) {
        if (!ks_num_get_double_complex(obj, &vcfp)) CTER()
        *(float complex*)to = vcfp;

    } else if (dtype == nx_dtype_cplx_fp64) {
        if (!ks_num_get_double_complex(obj, &vcfp)) CTER()
        *(double complex*)to = vcfp;

    } else {
        // indicates error
        ks_throw(ks_T_TypeError, "Could not cast '%T' object to nx type '%S'", obj, dtype);
        return false;
    }
    return true;

    #undef CTER
}


// convert from a Ctype into a ks_obj
static ks_obj nx_cast_from(nx_dtype dtype, void* from) {

    /*  */ if (dtype == nx_dtype_sint8) {
        return (ks_obj)ks_int_new(*(int8_t*)from);
    } else if (dtype == nx_dtype_uint8) {
        return (ks_obj)ks_int_new(*(uint8_t*)from);
    } else if (dtype == nx_dtype_sint16) {
        return (ks_obj)ks_int_new(*(int16_t*)from);
    } else if (dtype == nx_dtype_uint16) {
        return (ks_obj)ks_int_new(*(uint16_t*)from);
    } else if (dtype == nx_dtype_sint32) {
        return (ks_obj)ks_int_new(*(int32_t*)from);
    } else if (dtype == nx_dtype_uint32) {
        return (ks_obj)ks_int_new(*(uint32_t*)from);
    } else if (dtype == nx_dtype_sint64) {
        return (ks_obj)ks_int_new(*(int64_t*)from);
    } else if (dtype == nx_dtype_uint64) {
        return (ks_obj)ks_int_new(*(uint64_t*)from);

    } else if (dtype == nx_dtype_fp32) {
        return (ks_obj)ks_float_new(*(float*)from);
    } else if (dtype == nx_dtype_fp64) {
        return (ks_obj)ks_float_new(*(double*)from);

    } else if (dtype == nx_dtype_cplx_fp32) {
        return (ks_obj)ks_complex_new(*(float complex*)from);

    } else if (dtype == nx_dtype_cplx_fp64) {
        return (ks_obj)ks_complex_new(*(double complex*)from);

    } else {
        return ks_throw(ks_T_InternalError, "Did not handle dtype=%S", dtype);
    }
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


/** INTERNAL ROUTINES FOR INITIALIZATION **/

void nx_T_init_dtype();
void nx_T_init_array();
void nx_T_init_view();
void nx_T_init_fft_plan();

// adding submodules
void nx_mod_add_fft(ks_module nxmod);
void nx_mod_add_la(ks_module nxmod);


#endif
