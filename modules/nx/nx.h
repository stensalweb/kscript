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

// nx.array - N-dimensional tensor array
typedef struct {
    KS_OBJ_BASE

    // what type of data does the tensor store
    enum nx_dtype dtype;

    // pointer to the array data
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
 * Should return '0' on success, or a non-zero error code if there was a problem
 * 
 */
typedef int (*nx_ufunc_f)(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data);



/* FUNCTIONS/OPS */


// Return a data type enumeration from a given name
// NOTE: Returns 0 and throws an error if there was an invalid name
KS_API enum nx_dtype nx_dtype_get(char* name);

// Return the enum value for the name
// NOTE: Returns a string that should not be modifyed
KS_API char* nx_dtype_get_name(enum nx_dtype dtype);

// Return an enumeration object
// NOTE: Returns a new reference
KS_API ks_Enum nx_dtype_get_enum(enum nx_dtype dtype);


/* ARRAY */

// Create a new array with a given data type, and dimensions
// If 'data' is NULL, it is initialized to 0, otherwise 'data' must be a dense block
// NOTE: Returns a new reference
KS_API nx_array nx_array_new(enum nx_dtype dtype, int N, nx_size_t* dim, void* data);

// Create a new nx array from a kscript object (use NX_DTYPE_NONE to auto-detect)
// The rules are:
// If 'obj' is iterable:
//   * recursively iterate through and convert each object over
// Else:
//   * Create a 1-D array of size (1,) containing the singular element
// NOTE: Returns a new reference
KS_API nx_array nx_array_from_obj(ks_obj obj, enum nx_dtype dtype);



/* VIEW */

// Create a new view, from a given array
// NOTE: Returns a new reference
KS_API nx_view nx_view_new(nx_array ref, void* data, int N, nx_size_t* dim, nx_size_t* stride);



/* GENERIC OPS */

// Apply 'ufunc' to 'data', returns either 0 if there was no error, or the first error code generated
KS_API int nx_T_apply_ufunc(int Nin, void** datas, enum nx_dtype* dtypes, int* N, nx_size_t** dims, nx_size_t** strides, nx_ufunc_f ufunc, void* _user_data);


/* STRING */


// Return the string representation of the data
// NOTE: Returns a new reference, or NULL if there was an error
KS_API ks_str nx_get_str(void* data, enum nx_dtype dtype, int N, nx_size_t* dim, nx_size_t* stride);



/* BASIC OPS */


// Set every element of the given array to the given object ('obj') (casted to the correct type)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_set_all(void* data, enum nx_dtype dtype, int N, nx_size_t* dim, nx_size_t* stride, ks_obj obj);


// Compute B = (B_dtype)A, i.e. casting types
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_cast(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride);




/* SIMPLE MATH OPS */

// Compute: A + B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_add(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride, 
    void* C, enum nx_dtype C_dtype, int C_N, nx_size_t* C_dim, nx_size_t* C_stride);

// Compute: A - B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_sub(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride, 
    void* C, enum nx_dtype C_dtype, int C_N, nx_size_t* C_dim, nx_size_t* C_stride);

// Compute: A * B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_mul(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride, 
    void* C, enum nx_dtype C_dtype, int C_N, nx_size_t* C_dim, nx_size_t* C_stride);

// Compute: A / B -> C
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_div(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride, 
    void* C, enum nx_dtype C_dtype, int C_N, nx_size_t* C_dim, nx_size_t* C_stride);


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
