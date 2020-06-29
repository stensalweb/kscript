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
        return 0;
    }
}




/** INTERNAL ROUTINES FOR INITIALIZATION **/

#endif
