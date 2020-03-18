/* modules/nx/nx.h - the kscript numerics ('nx') library header, for exposing the C-API
 *
 * 
 * MEMORY:
 * Many functions have a signature like:
 * ...(int Ndim, ks_ssize_t* dims, ks_ssize_t* strides)... {...}
 * 
 * This means that the input is 'Ndim' dimensional (i.e. a matrix has Ndim==2), with the dimensions (row-major-first) being stored in `dims[0], ...`,
 *   and the strides for each axis in `strides[0], ...`
 * 
 * For instance, imagine we have the matrix:
 * [1  2  3  4]
 * [5  6  7  8]
 * [9 10 11 12]
 * 
 * The shape is [3,4] (3 rows of 4, and this is always row-major)
 * The stride is [4,1]
 * 
 * For 3 dimensional (and above) arrays, we have that the shape is [X,Y,Z,...]
 * And, the stride is (without views/slicing): [Y*Z*...,Z*...,1], i.e. each stride is the product of all of the shape elements that come after it
 * 
 * 
 * @author: Cade Brown
 */

#pragma once
#ifndef KS_M_NX_H__

// main library
#include <ks.h>

// for sizes of integers
#include <stdint.h>


/* actual type definitions */

// signed integers
typedef int8_t     nx_si8;
typedef int16_t    nx_si16;
typedef int32_t    nx_si32;
typedef int64_t    nx_si64;

// unsigned integers
typedef uint8_t    nx_ui8;
typedef uint16_t   nx_ui16;
typedef uint32_t   nx_ui32;
typedef uint64_t   nx_ui64;

// floating point types
typedef float      nx_fp32;
typedef double     nx_fp64;


enum {
    // Represents an error/invalid type
    NX_DTYPE_ERR = 0,

    /* INTEGERS */

    NX_DTYPE_SI8 ,
    NX_DTYPE_SI16,
    NX_DTYPE_SI32,
    NX_DTYPE_SI64,

    NX_DTYPE_UI8 ,
    NX_DTYPE_UI16,
    NX_DTYPE_UI32,
    NX_DTYPE_UI64,

    /* FLOATS */

    NX_DTYPE_FP32,
    NX_DTYPE_FP64,

};

// the first valid datatype
#define NX_DTYPE__FIRST NX_DTYPE_SI8
// the last valid datatype
#define NX_DTYPE__LAST  NX_DTYPE_FP64

// nx_dtype - an enumeration value representing the 'data type' of a tensor/piece of data
// SEE: `NX_DTYPE_*` enumerations in `nx.h`
typedef int nx_dtype;

// Return a dtype from a C-string tag, which can be loosely interpreted
nx_dtype nx_dtype_from_cstr(const char* name);

// Return a C-string name from a dtype
// NOTE: Do not free or modify the returned value!
const char* nx_dtype_to_cstr(nx_dtype dtype);

// Return the size (in bytes) of a given datatype, or '-1' if there is some error
ks_ssize_t nx_dtype_sizeof(nx_dtype dtype);




// nx_array - a multidimensional array of a given data type
typedef struct {
    KS_OBJ_BASE

    // number of dimensions of an array
    int Ndim;

    // the actual size dimensions of the tensor (in elements)
    ks_ssize_t* dims;

    // the actual stride values for every axis (in elements)
    // stride[i] = product(dims[j] for i < j < Ndim)
    ks_ssize_t* strides;



    // the data type of the array
    nx_dtype dtype;

    // a pointer to the actual data, allocated via 
    void* data_ptr;


}* nx_array;

// the type of an array/tensor
extern ks_type nx_type_array;

// Construct a new empty array of a given datatype, with the given dimensions
// If 'data_ptr==NULL', then the resulting data is set to the 'default' state (0)
// NOTE: Returns a new reference
nx_array nx_array_new(int Ndim, ks_ssize_t* dims, void* data_ptr, nx_dtype dtype);




// nx_view - a 'view' of an nx_array, which does not copy values, and can mutate the array itself
typedef struct {
    KS_OBJ_BASE

    // what the 'view' is looking at, i.e. the source
    nx_array source;


    // number of dimensions of the view ( <= source->Ndim )
    int Ndim;

    // the dimensions of the view
    ks_ssize_t* dims;

    // the strides (in elements) of each axis (default=1=no extra stride)
    ks_ssize_t* strides;


    // the offsets (in elements) of where the view starts relative to 'src'
    ks_ssize_t elem_offset;


}* nx_view;


// the type of a view of an array
extern ks_type nx_type_view;

// Create a view representing the entire 'source' array
// NOTE: Returns a new reference
nx_view nx_view_new_whole(nx_array source);

// Create a view representing a slice of the 'source' array,
// starting at 'start', and going through 'start+dims'
// NOTE: Returns a new reference
nx_view nx_view_new_slice(nx_array source, int Ndim, ks_ssize_t* start, ks_ssize_t* dims);


// Get a single item from the view, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]]
ks_obj nx_view_getitem(nx_view self, int n_idx, ks_ssize_t* idxs);

// Set a single item from the view, i.e.:
// self[idxs[0], idxs[1], ..., idxs[n_idx - 1]] = obj
// return 0 on success, otherwise for failure
bool nx_view_setitem(nx_view self, int n_idx, ks_ssize_t* idxs, ks_obj obj);


#endif /* KS_M_NX_H__ */

