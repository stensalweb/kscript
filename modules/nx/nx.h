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


/* CONSTANTS */

// Enum describing which data-type
enum nx_dtype {

    // None/error dtype
    NX_DTYPE_NONE       =   0,

    // signed/unsigned integers
    NX_DTYPE_SINT8      =   1,
    NX_DTYPE_UINT8      =   2,
    NX_DTYPE_SINT16     =   3,
    NX_DTYPE_UINT16     =   4,
    NX_DTYPE_SINT32     =   5,
    NX_DTYPE_UINT32     =   6,
    NX_DTYPE_SINT64     =   7,
    NX_DTYPE_UINT64     =   8,

    // floating point numbers
    NX_DTYPE_FP32       =   9,
    NX_DTYPE_FP64       =  10,

    // floating point complex
    NX_DTYPE_CPLX_FP32  =  11,
    NX_DTYPE_CPLX_FP64  =  12,

};


// the maximum size of an individual element
#define NX_MAX_SIZEOF 64


/* TYPES */

// size type, for indices & sizes
typedef ks_ssize_t nx_size_t;

// nx_any_t : representing any specific data type
// NOTE: this is a union, so not good for arrays, but good for declaring
//   a local variable
typedef union {

    int8_t v_sint8;
    uint8_t v_uint8;
    int16_t v_sint16;
    uint16_t v_uint16;
    int32_t v_sint32;
    uint32_t v_uint32;
    int64_t v_sint64;
    uint64_t v_uint64;

    float v_fp32;
    double v_fp64;

    float complex v_cplx_fp32;
    double complex v_cplx_fp64;

    // raw data
    uint8_t v_raw[NX_MAX_SIZEOF];

} nx_any_t;



// nxar_t - minimal array designation, which means that any other valid type (nx.array, nx.view, C pointers)
//   should be expressible as this. Therefore, functions that take this can support any of the above
// Use the macros `NXAR_*` to create `nxar_t` from other objects
typedef struct {
    
    // pointer to the start of the data
    void* data;
    
    // what type is the data?
    enum nx_dtype dtype;
    
    // number of dimensions of the array
    int N;
    
    // array of dimensions (i.e. sizes)
    nx_size_t* dim;
    
    // array of strides (in number of elements)
    // TODO: should this be in bytes instead?
    nx_size_t* stride;

} nxar_t;


// Create an `nxar_t` from an `nx_array` object
#define NXAR_ARRAY(_array) ((nxar_t){ \
    .data = (_array)->data, \
    .dtype = (_array)->dtype, \
    .N = (_array)->N, \
    .dim = (_array)->dim, \
    .stride = (_array)->stride, \
})


// Create an `nxar_t` from an `nx_view` object
#define NXAR_VIEW(_view) ((nxar_t){ \
    .data = (_view)->data, \
    .dtype = (_view)->dtype, \
    .N = (_view)->N, \
    .dim = (_view)->dim, \
    .stride = (_view)->stride, \
})



// nx.array - N-dimensional tensor array
typedef struct {
    KS_OBJ_BASE

    // what type of data does the tensor store
    enum nx_dtype dtype;

    // pointer to the array 
    // NOTE: this must be ks_free'd by the array, as it owns this memory
    void* data;

    // number of dimensions
    // i.e. N==1 -> 'vector', N==2 -> 'matrix', N > 2 == 'tensor'
    int N;

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

    // what type of data does the tensor store
    enum nx_dtype dtype;

    // pointer to the start of the array
    // NOTE: this is just a reference to 'data_src->data + offset`, so it 
    //   should not be freed
    void* data;

    // number of dimensions
    // i.e. N==1 -> 'vector', N==2 -> 'matrix', N > 2 == 'tensor'
    int N;

    // array of [N] dimensions, detailing the size in each dimension
    // so, total number of elements == product(dim[:])
    nx_size_t* dim;

    // array of [N] strides, detailing the number of elements per each index in the given direction
    // For a dense array, stride[N-1] == 1, stride[N-2] == dim[N-1], ... stride[i] = stride[i + 1] * dim[i + 1]
    // Or, stride[i] = product(dim[i + 1], ..., dim[N - 1])
    nx_size_t* stride;

    // pointer to the array data (which the view does not own)
    nx_array data_src;

}* nx_view;

// declaring the types
extern ks_type nx_type_array, nx_type_view;

// enumeration of the dtypes
extern ks_type nx_enum_dtype;


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
 * 'Nin' means the number of inputs to the function
 * 'dtypes' is an array of the data types of the various inputs
 * 'dtype_sizes' is an array of the size of each dtype, kept for efficiency reasons
 * 'datas' is an array of pointers to the data representing the respective inputs
 * 'dim' is the length of each 'data' (in elements). keep in mind that this is for 1D-only loops
 * 'strides' is the stride (in elements) of each array
 * 
 * 'user_data' is a user-defined pointer that was invoked when the function was applied
 * 
 * Should return success, or false on an error (and throw an error)
 * 
 */
typedef bool (*nx_ufunc_f)(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data);


/* DTYPE META */


// Return a data type enumeration from a given name
// NOTE: Returns 0 and throws an error if there was an invalid name
KS_API enum nx_dtype nx_dtype_get(char* name);

// Return the enum value for the name
// NOTE: Returns a string that should not be modified or freed
KS_API char* nx_dtype_get_name(enum nx_dtype dtype);

// Return an enumeration object
// NOTE: Returns a new reference
KS_API ks_Enum nx_dtype_get_enum(enum nx_dtype dtype);


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

// Create a new nx array from a kscript object (use NX_DTYPE_NONE to auto-detect)
// The rules are:
// If 'obj' is iterable:
//   * recursively iterate through and convert each object over
// Else:
//   * Create a 1-D array of size (1,) containing the singular element
// NOTE: Returns a new reference
KS_API nx_array nx_array_from_obj(ks_obj obj, enum nx_dtype dtype);


/* VIEW */

// Create a new view, from a given array & data
// NOTE: Returns a new reference
KS_API nx_view nx_view_new(nx_array ref, nxar_t nxar);



/* nxar_t */


// Convert 'obj' into a nxar_t, somehow, and add any created references to `refadd`
// So, you just need to `KS_DECREF(refadd)` once you're done with everything
// NOTE: Returns whether it was successful, or false and throws an error
KS_API bool nx_get_nxar(ks_obj obj, nxar_t* nxar, ks_list refadd);


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
// returns dtype_size * sum((idxs[:] % dim[:]) * stride[:])
// NOTE: allows out of bounds indexes by wrapping (i.e. -1 becomes dim[i] - 1)
KS_API nx_size_t nx_szsdot(int N, nx_size_t dtype_sz, nx_size_t* dim, nx_size_t* stride, nx_size_t* idxs);


// Get pointer to the element of data[*idx]
// NOTE: allows out of bounds indexes by wrapping (i.e. -1 becomes dim[i] - 1)
KS_API void* nx_get_ptr(void* data, nx_size_t dtype_sz, int N, nx_size_t* dim, nx_size_t* stride, nx_size_t* idx);



/* GENERIC APPLICATION */

// Apply 'ufunc' to 'datas', returns either 0 if there was no error, or the first error code generated
KS_API bool nx_T_apply_ufunc(int Nin, void** datas, enum nx_dtype* dtypes, int* N, nx_size_t** dims, nx_size_t** strides, nx_ufunc_f ufunc, void* _user_data);


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


/* Cfunc objects */


// declare functions here
extern ks_cfunc
    nx_F_add,
    nx_F_sub,
    nx_F_mul,
    nx_F_div
;



/** SUBMODULES **/


// FFT library
#include <nx-fft.h>
// LA (Linear Algebra) library
#include <nx-la.h>




#endif /* NX_H__ */
