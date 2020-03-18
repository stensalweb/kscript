/* modules/nx/nx-impl.h - the internal implementation header for the 'nx' (numerics) library
 *
 * 
 * @author: Cade Brown
 */

#pragma once
#ifndef KS_M_NX_IMPL_H__

// include main header too
#include <nx.h>

// calculate the linear index into an array, in number of elements (not bytes!)
// return -1 if there was an error
static inline int nx_calc_idx(int n_dims, int* dims, int n_idxs, int* idxs) {

    if (n_idxs == 0) return 0;

    // the result index
    int res = idxs[0];
    if (res < 0) res += dims[0];
    if (res < 0 || res >= dims[0]) {
        ks_throw_fmt(ks_type_KeyError, "Index %i (%i) out of range!", 0, idxs[0]);
        return -1;
    }

    int i;
    for (i = 1; i < n_idxs; ++i) {
        int this_idx = idxs[i];
        if (this_idx < 0) this_idx += dims[i];
        if (this_idx < 0 || this_idx >= dims[i]) {
            ks_throw_fmt(ks_type_KeyError, "Index %i (%i) out of range!", i, idxs[i]);
            return -1;
        }

        res *= dims[i - 1];
        res += this_idx;
    }

    return res;
}


// get binary -> object
static inline ks_obj nx_bincast(nx_dtype dtype, void* ptr) {
    if (dtype == NX_DTYPE_FP32) {
        return (ks_obj)ks_float_new(*(float*)ptr);
    } else {
        return (ks_obj)ks_throw_fmt(ks_type_TypeError, "Could not do a binary cast for dtype '%s'", nx_dtype_to_cstr(dtype));
    }
}

// get object -> binary
// return 0 for success, non-zeo indicating an error was thrown
static inline int nx_tobin(nx_dtype dtype, ks_obj obj, void* ptr) {
    if (dtype == NX_DTYPE_FP32) {
        // convert pointer to correct type
        nx_fp32* ptr_d = (nx_fp32*)ptr;
        if (obj->type == ks_type_float) {
            *ptr_d = ((ks_float)obj)->val;
            return 0;
        }
    }
    
    ks_throw_fmt(ks_type_TypeError, "Could not do a 'to binary' for dtype '%s' from '%T' object", nx_dtype_to_cstr(dtype), obj);
    return 1;
}


#endif
