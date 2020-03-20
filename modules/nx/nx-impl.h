/* modules/nx/nx-impl.h - the internal implementation header for the 'nx' (numerics) library
 *
 * 
 * @author: Cade Brown
 */

#pragma once
#ifndef KS_M_NX_IMPL_H__

// include main header too
#include "./nx.h"



/* configuration options */


// Enable a 'fast path' for multidimensional arrays which can be represented as a single
//   1D array in memory
#define NX_BCAST_FAST_MDLIN



// calculate the linear index from a list of indices, in number of elements, including a stride
// 'stride' should be calculated absolutely; not relative stride
// NOTE: Returns < 0 in case of an error, or >= 0 with the successful
//   result, in elements (including stride)
// strides
static inline ks_ssize_t 
nx_calc_idx(
    int Ndim, ks_ssize_t* dims, ks_ssize_t* strides,
    int Nidx, ks_ssize_t* idxs
) {
    if (Nidx == 0) return 0;

    // the result index
    ks_ssize_t res = 0;

    int i;
    // TODO: see if we need to go through all dimensions any way, since that could be problematic
    //   for slices
    for (i = 0; i < Nidx; ++i) {

        ks_ssize_t cidx = idxs[i];
        // allow indices relative to end
        if (cidx < 0) cidx += dims[i];

        // do check
        if (cidx < 0 || cidx >= dims[i]) {
            ks_throw_fmt(ks_type_KeyError, "Index %i (%l) out of range!", i, idxs[i]);
            return -1; // error
        }

        // accumulate the index
        res += cidx * strides[i];
    }

    return res;
}


// Treat 'ptr' as a pointer to a value of type 'dtype', then convert that object
//   to its corresponding kscript object.
// fp32, fp64 -> ks_float
// si8, ..., ui8, ... ui64 -> ks_int
// otherwise, an exception is raised
static inline ks_obj 
nx_bin_get(nx_dtype dtype, void* ptr) {

    /**/ if (dtype == NX_DTYPE_FP32) return (ks_obj)ks_float_new(*(nx_fp32*)ptr);
    else if (dtype == NX_DTYPE_FP64) return (ks_obj)ks_float_new(*(nx_fp64*)ptr);

    else if (dtype == NX_DTYPE_SI8 ) return (ks_obj)ks_int_new(*(nx_si8*)ptr);
    else if (dtype == NX_DTYPE_SI16) return (ks_obj)ks_int_new(*(nx_si16*)ptr);
    else if (dtype == NX_DTYPE_SI32) return (ks_obj)ks_int_new(*(nx_si32*)ptr);
    else if (dtype == NX_DTYPE_SI64) return (ks_obj)ks_int_new(*(nx_si64*)ptr);

    else if (dtype == NX_DTYPE_UI8 ) return (ks_obj)ks_int_new(*(nx_ui8*)ptr);
    else if (dtype == NX_DTYPE_UI16) return (ks_obj)ks_int_new(*(nx_ui16*)ptr);
    else if (dtype == NX_DTYPE_UI32) return (ks_obj)ks_int_new(*(nx_ui32*)ptr);
    else if (dtype == NX_DTYPE_UI64) return (ks_obj)ks_int_new(*(nx_ui64*)ptr);

    else {
        return (ks_obj)ks_throw_fmt(ks_type_TypeError, "Could not create ks_obj from blob ('%s' @ %p)", nx_dtype_to_cstr(dtype), ptr);
    }
}



// Treat 'ptr' as a pointer to a valud of type 'dtype', then convert 'src' into that
//   type and store it
// Return 'true' if successful, 'false' otherwise
static inline bool
nx_bin_set(ks_obj src, nx_dtype dtype, void* ptr) {

    if (dtype == NX_DTYPE_FP32) {
        // dtype pointer
        nx_fp32* dptr = (nx_fp32*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {

            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_FP64) {
        // dtype pointer
        nx_fp64* dptr = (nx_fp64*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_SI8) {
        // dtype pointer
        nx_si8* dptr = (nx_si8*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_SI16) {
        // dtype pointer
        nx_si16* dptr = (nx_si16*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_SI32) {
        // dtype pointer
        nx_si32* dptr = (nx_si32*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_SI64) {
        // dtype pointer
        nx_si64* dptr = (nx_si64*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_UI8) {
        // dtype pointer
        nx_ui8* dptr = (nx_ui8*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_UI16) {
        // dtype pointer
        nx_ui16* dptr = (nx_ui16*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else if (dtype == NX_DTYPE_UI32) {
        // dtype pointer
        nx_ui32* dptr = (nx_ui32*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }
        
    } else if (dtype == NX_DTYPE_UI64) {
        // dtype pointer
        nx_ui64* dptr = (nx_ui64*)ptr;

        // check for builtins
        if (src->type == ks_type_float) {
            *dptr = ((ks_float)src)->val;
            return true;
        } else if (src->type == ks_type_int) {
            *dptr = ((ks_int)src)->val;
            return true;
        }

    } else {
        // unknown type
    }
    

    // was not handled; must be an error
    ks_throw_fmt(ks_type_TypeError, "Could not cast '%T' object into dtype '%s'", src, nx_dtype_to_cstr(dtype));
    return false;
}



/** INTERNAL ROUTINES: do not call **/
void nx_init__array();
void nx_init__view();

#endif
