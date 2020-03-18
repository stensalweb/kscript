/* modules/nx/nx.h - the kscript numerics ('nx') library header, for exposing the C-API
 *
 * 
 * @author: Cade Brown
 */

#pragma once
#ifndef KS_M_NX_H__

// main library
#include <ks.h>


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
nx_dtype nx_dtype_from_cstr(char* name);

// Return a C-string name from a dtype
// NOTE: Do not free or modify the returned value!
const char* nx_dtype_to_cstr(nx_dtype dtype);

// Return the size (in bytes) of a given datatype, or '-1' if there is some error
int nx_dtype_sizeof(nx_dtype dtype);


// nx_array - a multidimensional array of a given data type
typedef struct {
    KS_OBJ_BASE

    // number of dimensions of an array
    int n_dim;

    // the actual size dimensions, row major by default
    int* dims;


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
nx_array nx_array_new(int n_dim, int* dims, void* data_ptr, nx_dtype dtype);




/** INTERNAL ROUTINES: do not call **/
void nx_init__array();


#endif /* KS_M_NX_H__ */

