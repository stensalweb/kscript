/* modules/nx/nx.h - the kscript numerics ('nx') library header, for exposing the C-API
 *
 * 
 * Essentially, 'nx' is a tensor library similar to numpy. It supports the following datatypes:
 *   - signed & unsigned 8, 16, 32, and 64 bit integers
 *   - 32 and 64 bit floating point
 *   - 32 and 64 bit floating point complex
 * 
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef NX_H__

// include the main kscript API
#include <ks.h>

// for all sizes of integers
#include <stdint.h>


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





#endif /* NX_H__ */
