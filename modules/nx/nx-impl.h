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

// templating library
#include "./gen/nxt.h"


/* EXPANSION MACROS */

// Expands to the standard arguments passed to a function (data, dtype, N, dim, stride)
#define _NXAR_(_arr) (_arr)->data, (_arr)->dtype, (_arr)->N, (_arr)->dim, (_arr)->stride

// Expands to the standard arguments passed to a function (data, dtype, N, dim, stride)
#define _NXARS_(_arr) (_arr).data, (_arr).dtype, (_arr).N, (_arr).dim, (_arr).stride

// minimal NX array designation
struct nxar_t {
    void* data;
    enum nx_dtype dtype;
    int N;
    nx_size_t* dim;
    nx_size_t* stride;
};

// Get a 'nxar_t' from a single nx_array variable
#define GET_NXAR_ARRAY(_arr) ((struct nxar_t){ _NXAR_(_arr) })

// Calculate a 'nxar' from a given object, setting (_nxar).data == NULL if there was an error
// You should always include `if (_delobj) KS_DECREF(_delobj)` to clean up any temporary arrays created
#define NX_CALC_NXAR(_nxar, _obj, _delobj) { \
    /**/ if ((_obj)->type == nx_type_array) _nxar = GET_NXAR_ARRAY(((nx_array)(_obj))); \
    else if (ks_num_is_numeric((_obj)) || ks_is_iterable((_obj))) { \
        _delobj = (ks_obj)nx_array_from_obj((_obj), NX_DTYPE_NONE); \
        if (!_delobj) _nxar.data = NULL; \
        else _nxar = GET_NXAR_ARRAY(((nx_array)_delobj)); \
    } else { \
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", (_obj)); \
        _nxar.data = NULL; \
    } \
}


//#define NX_ASSERT_CHECK(_expr) { if (!(_expr)) { fprintf(stderr, "INTERNAL NX ASSERT ERROR: " #_expr "\n"); exit(1); } }
#define NX_ASSERT_CHECK assert

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
        printf("ERR\n");
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



// check whether the list of arguments (of which there are Nin) are broadcastable together
static bool nx_can_bcast(int Nin, int* N, nx_size_t** dims) {
    NX_ASSERT_CHECK(Nin > 0 && "no arguments in broadcast!");

    // when considering broadcastability, we borrow from the rules from NumPy:
    // Sizes are right aligned, i.e. begin from the right, so comparing tensors
    //   of shapes '[5x3x2]' and '[3x1]', we would write:
    // 5x3x2
    //   3x1
    // Missing entries to the left are considered '1' (since there would be exactly 1 element,
    //   containing the entire array)
    //
    // Then, compare each entry the arrays are able to be broadcast if the columns
    //   are either 1, or 1 other number
    // if they are 1, then the same value is interpreted for all entries on that axis,
    //   otherwise they are iterated per each


    // negative index (from end end of each dims[i]), so we can
    //   easily do logic beginning from the right of each array
    int ni = -1;


    while (true) {
        // the size (other than 1) required for the current dimension
        //   or -1 if a number other than 1 has not been encountered yet
        nx_size_t req_size = -1;

        // keep track if we checked any dimensions
        // if not, we need to stop since we are out of range for all of them,
        //   and have checked everything we need
        bool hadDim = false;

        int i;
        // loop through and check each argument's 'ni'th dimension for compatibility
        for (i = 0; i < Nin; ++i) {
            if (N[i] < -ni) {
                // since we are right-aligned, we are out of bounds for this array
                // in this case, interpret the empty index as 1, and keep going
                continue;
            } else {
                // get the current dimension value
                // NOTE: since 'ni' is always negative, adding it to the size effectively
                //   computes the index from the right of 'dims'
                nx_size_t this_dim = dims[i][N[i] + ni];
                hadDim = true;

                if (this_dim == 1) {
                    // single value, which can be broadcasted with every other size,
                    //   so it is still valid
                    continue;
                } else if (req_size < 0) {
                    // no size requirement exists yet, so this one
                    //   will be the requirement
                    // now, any other sizes for this dimension must be either '1' or 'this_dim'
                    req_size = this_dim;
                    continue;
                } else if (this_dim == req_size) {
                    // it matches the size requirement set, all is good
                    continue;
                } else {
                    // something else happened, and the shapes are not broadcastable
                    ks_str_b SB;
                    ks_str_b_init(&SB);

                    for (i = 0; i < Nin; ++i) {
                        if (i > 0) ks_str_b_add_c(&SB, ", ");
                        ks_str_b_add_fmt(&SB, "(%+z)", N[i], dims[i]);
                    }

                    ks_str rstr = ks_str_b_get(&SB);
                    ks_str_b_free(&SB);
                    ks_throw_fmt(ks_type_SizeError, "Shapes %S were not broadcastable!", rstr);
                    KS_DECREF(rstr);
                    return false;
                }
            }
        } 

        // if no dimensions were processed, then we are done checking
        if (!hadDim) break;

        // keep moving back 1 dimension every loop
        ni--;

    }

    // success, they can be broadcasted
    return true;
}



// compute the broadcast dimensions (i.e. the result dimensions) from a list of inputs
static bool nx_compute_bcast(int Nin, int* N, nx_size_t** dims, int R_N, nx_size_t* R_dims) {
    // loop vars
    int i;

    // compute the maximum dimension of any tensor, which will be the dimension of the result
    int max_N = N[0];
    for (i = 1; i < Nin; ++i) if (N[i] > max_N) max_N = N[i];

    // negative index (from end end of each dims[i]), so we can
    //   easily do logic beginning from the right of each array
    int ni = -1;

    // keep going while valid
    while (true) {
        // the size (other than 1) required for the current dimension
        //   or -1 if a number other than 1 has not been encountered yet
        nx_size_t req_size = -1;

        // keep track if we checked any dimensions
        // if not, we need to stop since we are out of range for all of them,
        //   and have checked everything we need
        bool hadDim = false;
        
        // loop through and check each argument's 'ni'th dimension for compatibility
        for (i = 0; i < Nin; ++i) {
            if (N[i] < -ni) {
                // since we are right-aligned, we are out of bounds for this array
                // in this case, interpret the empty index as 1, and keep going
                continue;
            } else {
                // get the current dimension value
                // NOTE: since 'ni' is always negative, adding it to the size effectively
                //   computes the index from the right of 'dims'
                nx_size_t this_dim = dims[i][N[i] + ni];
                hadDim = true;

                if (this_dim == 1) {
                    // single value, which can be broadcasted with every other size,
                    //   so it is still valid
                    continue;
                } else if (req_size < 0) {
                    // no size requirement exists yet, so this one
                    //   will be the requirement
                    // now, any other sizes for this dimension must be either '1' or 'this_dim'
                    req_size = this_dim;
                    continue;
                } else if (this_dim == req_size) {
                    // it matches the size requirement set, all is good
                    continue;
                } else {
                    // something else happened, and the shapes are not broadcastable
                    ks_str_b SB;
                    ks_str_b_init(&SB);

                    for (i = 0; i < Nin; ++i) {
                        if (i > 0) ks_str_b_add_c(&SB, ", ");
                        ks_str_b_add_fmt(&SB, "(%+z)", N[i], dims[i]);
                    }

                    ks_str rstr = ks_str_b_get(&SB);
                    ks_str_b_free(&SB);
                    ks_throw_fmt(ks_type_SizeError, "Shapes %S were not broadcastable!", rstr);
                    KS_DECREF(rstr);
                    return false;
                }
            }
        } 

        // set the result dimension to the required size (or 1, if none was set)
        R_dims[R_N + ni] = req_size < 0 ? 1 : req_size;

        // if there were no indices checked, every single one must be out of range, so return
        if (!hadDim) break;

        // otherwise, continue moving back to the left to check more
        ni--;
    }

    // success, and 'R_dims' was set
    return true;
}





// enumeration values
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

// adding submodules
void nx_mod_add_fft(ks_module nxmod);
void nx_mod_add_la(ks_module nxmod);




#endif
