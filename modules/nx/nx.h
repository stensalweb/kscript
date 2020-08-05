/* modules/nx/nx.h - the kscript numerics ('nx') library header, for exposing the C-API
 *
 * 
 * Essentially, 'nx' is a tensor library similar to numpy. It supports the following datatypes:
 *   - signed & unsigned 8, 16, 32, and 64 bit integers
 *   - 32 and 64 bit floating point
 *   - 32 and 64 bit floating point complex
 * 
 * And, it supports arbitrary dimensional data-types for most operations
 * 
 * There are also the submodules:
 *   - fft (Fast-Fourier-Transform related routines)
 *   - la (Linear Algebra routines)
 * 
 * You can see them in their own headers (i.e. 'nx-fft.h')
 * 
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef NX_H__
#define NX_H__

// include the main kscript API
#include <ks.h>

// for all sizes of integers
#include <stdint.h>


// include FFTW3, which is more efficient at FFT computations
// We include fallback methods, but FFTW3 is very fast
#ifdef KS_HAVE_FFTW3
#include <fftw3.h>
#endif



/* TYPES */

// size type, for indices & sizes
typedef ks_ssize_t nx_size_t;



enum nx_dtype_kind {
    // none/error kind
    NX_DTYPE_KIND_NONE      = 0,

    // C-type integer
    NX_DTYPE_KIND_CINT      = 1,

    // float/double (IEEE format)
    NX_DTYPE_KIND_CFLOAT    = 2,

    // float complexd/double complex
    NX_DTYPE_KIND_CCOMPLEX  = 3,

    // TODO add structs

};


// nx_dtype - type representing a data-type
typedef struct {
    KS_OBJ_BASE

    // name of the data-type
    ks_str name;

    // size (in bytes) of the data-type
    int size;

    // what kind of datatype is it?
    enum nx_dtype_kind kind;

    union {

        // if kind==NX_DTYPE_INT, describes the integer
        struct {
            
            // whether or not the integer type is signed
            bool isSigned;

        } s_cint;

    };

}* nx_dtype;


// nxar_t - minimal array designation, which means that any other valid type (nx.array, nx.view, C pointers)
//   should be expressible as this. Therefore, functions that take this can support any of the above
// Use the macros `NXAR_*` to create `nxar_t` from other objects
// NOTE: nxar's don't hold references to their objects
typedef struct {
    
    // pointer to the start of the data
    void* data;

    nx_dtype dtype;
    
    // number of dimensions of the array
    int rank;
    
    // array of dimensions (i.e. sizes)
    nx_size_t* dim;
    
    // array of strides (i.e. byte sizes)
    nx_size_t* stride;

    // source object
    ks_obj src_obj;

} nxar_t;


// Create an `nxar_t` from an `nx_array` object
#define NXAR_ARRAY(_array) ((nxar_t){ \
    .data = (_array)->data, \
    .dtype = (_array)->dtype, \
    .rank = (_array)->rank, \
    .dim = (_array)->dim, \
    .stride = (_array)->stride, \
    .src_obj = (ks_obj)(_array), \
})


// Create an `nxar_t` from an `nx_view` object
#define NXAR_VIEW(_view) ((nxar_t){ \
    .data = (_view)->data, \
    .dtype = (_view)->dtype, \
    .rank = (_view)->rank, \
    .dim = (_view)->dim, \
    .stride = (_view)->stride, \
    .src_obj = (ks_obj)(_view), \
})


// nx.array - N-dimensional tensor array
typedef struct {
    KS_OBJ_BASE

    // pointer to the array 
    // NOTE: this must be ks_free'd by the array, as it owns this memory
    void* data;

    // what type of data does the tensor store
    nx_dtype dtype;

    // number of dimensions in the array
    int rank;

    // array of [N] dimensions, detailing the size in each dimension
    // so, total number of elements == product(dim[:])
    nx_size_t* dim;

    // array of [N] strides, detailing the number of elements per each index in the given direction
    // For a dense array, stride[0] == 1, stride[1] == dim[0], ... stride[i] = stride[i - 1] * dim[i]
    // Or, stride[i] = product(dim[0], ..., dim[i - 1])
    nx_size_t* stride;

}* nx_array;

// nx.view - a view over ray data
typedef struct {
    KS_OBJ_BASE

    // pointer to the start of the array
    // NOTE: this is just a reference to 'data_src->data + offset`, so it 
    //   should not be freed
    void* data;

    // what type of data does the tensor store
    nx_dtype dtype;

    // number of dimensions in the array
    int rank;

    // array of [N] dimensions, detailing the size in each dimension
    // so, total number of elements == product(dim[:])
    nx_size_t* dim;

    // array of [N] strides, detailing the number of elements per each index in the given direction
    // For a dense array, stride[N-1] == 1, stride[N-2] == dim[N-1], ... stride[i] = stride[i + 1] * dim[i + 1]
    // Or, stride[i] = product(dim[i + 1], ..., dim[N - 1])
    nx_size_t* stride;

    // pointer to the array data (which the view does not own)
    ks_obj data_src;

}* nx_view;

// declaring the types
extern ks_type nx_T_array, nx_T_view;

// enumeration of the dtypes
extern ks_type nx_T_dtype;


// dtypes (builtin)
extern nx_dtype
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


/* UFUNCS */


/* 'ufuncs' or 'universal functions' are simple, (typically) 1D loops that process data. They can be
 *   invoked with different arguments & using recursion to apply a simple operation to, so implementations
 *   of trivial operations do not contain multi-dimensional loops (keeps things bug free, and allows optimization in one spot)
 *
 *
 * 
 * 
 */

/* nx_ufunc_f - describes the function signature required for a 1D ufunc, operating on a variable number of arrays
 *
 * 
 * Should return success, or false on an error (and throw an error)
 * 
 */
typedef bool (*nx_ufunc_f)(int N, nxar_t* arrs, int len, void* _user_data);


/* nx_loopfunc_f - describes a function which can be called in an N-dimensional loop
 * 
 * `loop_N` is the loop dimension
 * `loop_dim` are the sizes of the loop in each axis
 * `loop_idx` are the current indices
 * `_user_data` is a user-provided pointer
 * 
 * Should return success, or false on an error (and throw an error)
 * 
 */
typedef bool (*nx_loopfunc_f)(int loop_N, nx_size_t* loop_dim, nx_size_t* loop_idx, void* _user_data);



/* DTYPE */


// Make an integer type
// NOTE: Returns a new reference
KS_API nx_dtype nx_dtype_make_int(char* name, int bits, bool isSigned);

// Make a floating point type
// NOTE: Returns a new reference
KS_API nx_dtype nx_dtype_make_fp(char* name, int bits);

// Make a floating point complex type
// NOTE: Returns a new reference
KS_API nx_dtype nx_dtype_make_cplx(char* name, int bits);




/* ARRAY */

/*
// Create a new array with a given data type, and dimensions
// If 'data' is NULL, it is initialized to 0, otherwise 'data' must be a dense block
// NOTE: Returns a new reference
KS_API nx_array nx_array_new(enum nx_dtype dtype, int N, nx_size_t* dim, void* data);
*/

// Create a new array from a given nxar_t
// NOTE: Returns a new reference
KS_API nx_array nx_array_new(nxar_t nxar);

// Create a new nx array from a kscript object (use NX_DTYPE_KIND_NONE to auto-detect)
// The rules are:
// If 'obj' is iterable:
//   * recursively iterate through and convert each object over
// Else:
//   * Create a 1-D array of size (1,) containing the singular element
// NOTE: Returns a new reference
KS_API nx_array nx_array_from_obj(ks_obj obj, nx_dtype dtype);


/* VIEW */

// Create a new view, from a given array & data
// NOTE: Returns a new reference
KS_API nx_view nx_view_new(ks_obj ref, nxar_t nxar);



/* nxar_t */


// Convert 'obj' into a nxar_t, somehow, and add any created references to `refadd`
// So, you just need to `KS_DECREF(refadd)` once you're done with everything
// NOTE: Returns whether it was successful, or false and throws an error
KS_API bool nx_get_nxar(ks_obj obj, nxar_t* nxar, ks_list refadd);


// Implementation of nxar[*idxs]
// NOTE: returns new reference, or NULL and throws an error
KS_API ks_obj nx_nxar_getitem(nxar_t nxar, int N, ks_obj* idxs);

// Implementation of nxar[*idxs] = obj
// NOTE: returns success, or false and throws an error
KS_API bool nx_nxar_setitem(nxar_t nxar, int N, ks_obj* idxs, ks_obj obj);





/* SIZE/SHAPE UTILS */


// Check whether the list of arguments (of which there are Nin) are broadcastable together
// NOTE: Returns true if they can, false and throws error if they cannot
KS_API bool nx_can_bcast(int Nin, int* N, nx_size_t** dims);


// Compute the broadcast dimensions (i.e. the result dimensions) from a list of inputs, and store in `R_dims`
// There are `Nin` inputs, and their dimensions are in `N`
// R_N must be max(N[:])
// NOTE: Returns false and throws error if there is an error
KS_API bool nx_compute_bcast(int Nin, int* N, nx_size_t** dims, int R_N, nx_size_t* R_dims);



/* OFFSET CALCULATIONS */


// stride,size dot product, to calculate offset (in bytes) of a given N
// returns sum((idxs[:] % dim[:]) * stride[:])
// NOTE: allows out of bounds indexes by wrapping (i.e. -1 becomes dim[i] - 1)
KS_API nx_size_t nx_szsdot(int N, nx_size_t* dim, nx_size_t* stride, nx_size_t* idxs);


// Get pointer to the element of data[*idx]
// NOTE: allows out of bounds indexes by wrapping (i.e. -1 becomes dim[i] - 1)
KS_API void* nx_get_ptr(void* data, int N, nx_size_t* dim, nx_size_t* stride, nx_size_t* idx);



/* GENERIC APPLICATION */


// Apply `ufunc(*datas)`
// NOTE: returns success, or false and throws an error
KS_API bool nx_T_ufunc_apply(int N, nxar_t* datas, nx_ufunc_f ufunc, void* _user_data);


/* LOOP UTILS */

// Apply a `loop_N` dimensional loop (of dimensions `loop_dim`)
// NOTE: returns success, or false and throws an error
KS_API bool nx_T_apply_loop(int loop_N, nx_size_t* loop_dim, nx_loopfunc_f loopfunc, void* _user_data);



/* STRING */


// Return the string representation of the data
// NOTE: Returns a new reference, or NULL if there was an error
KS_API ks_str nx_get_str(nxar_t A);


/* BASIC OPS */

// Set every element of the given array to the given object ('obj') (casted to the correct type)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_set_all(nxar_t A, ks_obj obj);


// Compute B = (B_dtype)A, i.e. casting types
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_cast(nxar_t A, nxar_t B);




/* SIMPLE MATH OPS */

// Compute: A + B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_add(nxar_t A, nxar_t B, nxar_t C);

// Compute: A - B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_sub(nxar_t A, nxar_t B, nxar_t C);

// Compute: A * B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_mul(nxar_t A, nxar_t B, nxar_t C);

// Compute: A / B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_div(nxar_t A, nxar_t B, nxar_t C);

// Compute: A ** B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_pow(nxar_t A, nxar_t B, nxar_t C);


// Compute: abs(A) -> B
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_abs(nxar_t A, nxar_t B);



/* Cfunc objects */


// declare functions here
extern ks_cfunc
    nx_F_add,
    nx_F_sub,
    nx_F_mul,
    nx_F_div,
    nx_F_pow,

    nx_F_abs

;



/** SUBMODULES **/


// FFT library
#include <nx-fft.h>
// LA (Linear Algebra) library
#include <nx-la.h>




#endif /* NX_H__ */
