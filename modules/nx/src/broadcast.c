/* modules/nx/src/broadcast.c - broadcasting utilities for vectorized inputs
 *
 * Much of this code uses VLAs (Variable Length Arrays), to help readability as well as
 *   performance (i.e. no heap allocations, and no possibility for memory leaks in inner loops)
 * 
 * As a result, it pretty much requires a C99 compiler (for example, C11 makes VLAs an optional feature)
 * 
 * However, C99 is God's language, so this shouldn't be any problem ;)
 * 
 * 
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "../nx-impl.h"


// Internal inner loop to iterate things,
// Essentially, if we view the dimensions as a matrix:
// args[0] | args[1] | args[2]
// A 
// B D
// ....
// .....
// C E G
//
// We need to start at the top, and iteratively re-call ourselves with every combination of array at that row
//
// To explain further:
// Broadcasting is essentially taking every combination of >1 dimensions, and then iterating through all those and calling
//   'ufunc' on all of those 1D vectors that are generated as a result.
//
// NOTE: 'dims' and 'strides' are really 2D arrays; their dimensions are [n-args X Ndim], and they must be gove as such!
//   I use the 'dims__' and 'strides__' macros to get elements of them
static int
nxi_bcast_iter(nx_ufunc_f ufunc, int n_args, nx_dtype* dtypes, ks_ssize_t* sizeof_dtypes, void** args, int Ndim, ks_ssize_t* dims, ks_ssize_t* strides) {

    // macros for the 2D arrays, returns 'args[_a]' 's dimension/stride for the '_i'th axis, but
    // VLAs with 2 dimensions are a single pointer, so they must be passed in this way
    #define dims__(_a, _i) (dims[(_a) * Ndim + (_i)])
    #define strides__(_a, _i) (strides[(_a) * Ndim + (_i)])

    // if we are already at 1 dimensions, we just need to call the 'ufunc' with the given arguments
    //   and return its status code
    if (Ndim == 1) {
        // the length is the maximum of the lengths
        ks_ssize_t len = dims__(0, 0);

        int i;
        for (i = 1; i < n_args; ++i) {
            if (dims__(i, 0) > len) len = dims__(i, 0);
        }

        // the strides (i.e. change in elements) per axis
        ks_ssize_t P_strides[n_args];

        // cop them over
        for (i = 0; i < n_args; ++i) {
            // if there is only 1 item, the stride should be 0 so it is the same value over and over
            if (dims__(i, 0) == 1) {
                // just repeat the same value
                P_strides[i] = 0;
            } else {
                // calculate the offset
                P_strides[i] = strides__(i, 0);
            }
        }

        // return its status code
        return ufunc(n_args, len, dtypes, args, P_strides);
    }

    // get current broadcast ndimensions, which should be the maximum of the current
    //   dimensions
    int iterNdim = dims__(0, 0);
    int i;
    for (i = 1; i < n_args; ++i) {
        if (dims__(i, 0) > iterNdim) iterNdim = dims__(i, 0);
    }

    // adjusted parameters, so all the parameters of the next dimensions
    void* P_args[n_args];
    ks_ssize_t P_dims[n_args][Ndim-1];
    ks_ssize_t P_strides[n_args][Ndim-1];

    // whether or not the array is linear, even in multiple dimensions,
    // so it can be represented as a single 1D array with no memory management
    bool isLinear = true;

    // shift dimensions, ignoring the leftmost, since we are currently iterating that
    for (i = 0; i < n_args; ++i) {
        int j;
        for (j = 0; j < Ndim-1; ++j) {
            //printf("TEST: %p\n", dims[i]);
            P_dims[i][j] = dims__(i, j + 1);

            if (P_dims[i][j] == 1) {
                // don't move the entire time, i.e. broadcast the value
                P_strides[i][j] = 0;
            } else {
                P_strides[i][j] = strides__(i, j + 1);
            }
        }

        // only enable if the fast path is enabled
        #ifdef NX_BCAST_FAST_MDLIN
    
        // don't continue checking
        if (!isLinear) continue;

        // check each dimension for the argument:
        // for example, strides[-2] == strides[-1] * dims[-1], then
        // the array is actually linear in memory, and so on
        // start with the last stride
        ks_ssize_t req_stride = P_strides[i][Ndim - 2];
        for (j = Ndim - 3; j >= 0; --j) {
            if (P_strides[i][j] != req_stride) {
                isLinear = false;
                break;
            }
            req_stride *= P_strides[i][j];
        }

        #endif

    }
    
    
    #ifdef NX_BCAST_FAST_MDLIN

    // fastpath for arrays which are multidimensional but linear in memory
    if (isLinear) {

        // the amount of linear elements, which should be the product of the maximum of the dimensions
        ks_ssize_t lin_len = 1;

        for (i = 0; i < Ndim; ++i) {
            ks_ssize_t maxDim = dims__(0, i);
            int j;
            for (j = 1; j < n_args; ++j) {
                if (dims__(j, i) > maxDim) maxDim = dims__(j, i);
            }
            lin_len *= maxDim;
        }

        // array of the linear strides (the last dimension, since all others are linear in memory)
        ks_ssize_t lin_strides[n_args];
        for (i = 0; i < n_args; ++i) {
            lin_strides[i] = P_strides[i][Ndim - 2];
        }

        // now, just do 1D iteration
        return ufunc(n_args, lin_len, dtypes, args, lin_strides);
    }

    #endif


    // otherwise, recursively call ourselves until we get to 1D loops
    for (i = 0; i < iterNdim; ++i) {
        int j;
        // prepare arguments
        for (j = 0; j < n_args; ++j) {
            if (dims__(j, 0) == 1) {
                // broadcast the value over and over with no stride
                P_args[j] = args[j];
            } else {
                // adjust strides off, since we are looping through different chunks of the array
                P_args[j] = (void*)((uintptr_t)args[j] + strides__(j, 0) * sizeof_dtypes[j] * i);
            }
        }

        // recursively try to iterate through the functions
        if (nxi_bcast_iter(ufunc, n_args, dtypes, sizeof_dtypes, P_args, Ndim-1, &P_dims[0][0], &P_strides[0][0]) != 0) return 1;
    }

    return 0;


/*  TODO: reuse this old code, which is the outer loop for computing all combinations of an ND vector

    // otherwise, iterate through all of the current dimensions
    // yay! variable depth iteration!
    int ii[n_args];
    for (i = 0; i < n_args; ++i) {
        ii[i] = 0;
    }

    while (true) {
        if (ii[0] >= dims__(0, 0)) {
            // hit the end of the lowest level
            ii[0] = 0;

            // attempt to carry the '1', and propogate, much like addition
            int j = 1;
            while (j < n_args) {
                ii[j]++;
                if (ii[j] >= dims__(j, 0)) {
                    // we have hit the end of this axis, so update the next axis
                    ii[j] = 0;
                    j++;
                } else {

                    // end of the line, it's not exhausted yet
                    break;
                }
            }

            // propogated out of bounds, so we are done
            if (j == n_args) break;

            continue;
        }
        //printf("TEST: %i:%i\n", (int)ii[0], (int)ii[1]);

        // now, actually calculate P_args, and recursively call ourself
        int j;
        for (j = 0; j < n_args; ++j) {
            P_args[j] = args[j] + strides__(j, 0) * ii[j];
        }

        // recursively call
        if (nxi_bcast_iter(ufunc, n_args, dtypes, P_args, Ndim-1, P_dims, P_strides) != 0) return 1;

        // next iteration
        ii[0]++;
    }

    // success
    return 0;

    */
}


// Internal utility to broadcast arguments
static int 
nxi_bcast(nx_ufunc_f ufunc, int n_args, nx_dtype* dtypes, void** args, int* Ndims, ks_ssize_t** dims, ks_ssize_t** strides) {

    // set up a filled matrix, first determine the max number of dimensions:
    int maxNdim = Ndims[0];
    int i;
    for (i = 1; i < n_args; ++i) if (Ndims[i] > maxNdim) maxNdim = Ndims[i];

    // now, create total, packed arrays
    ks_ssize_t P_dims[n_args][maxNdim];
    ks_ssize_t P_strides[n_args][maxNdim];
    ks_ssize_t P_sizeofs[n_args];

    // calculate filled parametters
    for (i = 0; i < n_args; ++i) {

        // calculate sizeof once
        P_sizeofs[i] = nx_dtype_sizeof(dtypes[i]);
        if (P_sizeofs[i] < 0) return 1;

        // now, shift all shapes so that they are padded with '1''s on the left side (and their stride should be set to 0 in those places)
        int j;
        // prepad:
        for (j = 0; j < maxNdim - Ndims[i]; ++j) {
            P_dims[i][j] = 1;
            P_strides[i][j] = 0;
        }

        int fj;
        for (fj = 0; j < maxNdim; ++j, ++fj) {
            P_dims[i][j] = dims[i][fj];
            P_strides[i][j] = strides[i][fj];
        }

    }

    // now, call internal iterator
    return nxi_bcast_iter(ufunc, n_args, dtypes, P_sizeofs, args, maxNdim, &P_dims[0][0], &P_strides[0][0]);
}

// Check whether given arguments could be broadcastable
bool nx_bcast_check(int n_args, int* Ndims, ks_ssize_t** dims) {
    assert(n_args > 0 && "no arguments in broadcast!");

    // when considering broadcastability, we borrow from the rules from NumPy:
    // Sizes are right aligned, i.e. begin from the right, so comparing tensors
    //   of shapes '[5x3x2]' and '[3x1]', we would write:
    // 5x3x2
    //   3x1
    // Missing entries to the left are considered '1' (since there would be exactly 1 element)
    //   containing the entire array)
    //
    // Then, compare each entry the arrays are able to be broadcast if the columns
    //   are either 1, or 1 number

    // index (negative, from end of each dims[i]), so we
    //   can easily do a right-comparison
    int negIdx = -1;

    while (true) {
        // the size (other than 1) required for the current dimension,
        //   or -1 if there were no strict requirements
        int req_size = -1;

        // keep track if there was an index
        bool hadIdx = false;

        int i;
        for (i = 0; i < n_args; ++i) {
            if (Ndims[i] < -negIdx) {
                // since we are right-aligned, and we are past the normal index
                // since we treat empty indices as 1, we are good to go
                continue;
            } else {
                // read the current right-wise dimension (keep in mind: negIdx is negative,
                //   so adding it to the length of that dimension is effectively computing it
                //   from the end)
                int this_dim = dims[i][Ndims[i] + negIdx];

                hadIdx = true;

                // no problem here; it will always be broadcastable
                if (this_dim == 1) continue;

                if (req_size < 0) {
                    // no current size set, so we get to be the first to set the requirement!
                    req_size = this_dim;
                    continue;
                } else if (this_dim != req_size) {
                    // error! the dimensions are not compatible
                    return false;
                }
            }
        }

        // if there were no indices checked, every single one must be out of range, so return
        if (!hadIdx) break;

        // otherwise, continue moving back to the left to check more
        negIdx--;
    }

    return true;
}

// compute the size of a broadcast result
bool nx_bcast_size(int n_args, int* Ndims, ks_ssize_t** dims, int R_Ndim, ks_ssize_t* R_dims) {
    // get the maximum dimension of any tensor, which will be the dimension of the result
    int maxNdim = Ndims[0];
    int i;
    for (i = 1; i < n_args; ++i) {
        if (Ndims[i] > maxNdim) maxNdim = Ndims[i];
    }

    // ensure they calculated it correctly
    assert(maxNdim == R_Ndim && "nx_bcast_getsize given incorrect result size!");


    // index (negative, from end of each dims[i]), so we
    //   can easily do a right-comparison
    int negIdx = -1;

    // keep going while valid
    while (true) {
        // the size (other than 1) required for the current dimension,
        //   or -1 if there were no strict requirements
        int req_size = -1;

        // keep track if there was an index
        bool hadIdx = false;

        int i;
        for (i = 0; i < n_args; ++i) {
            if (Ndims[i] < -negIdx) {
                // since we are right-aligned, and we are past the normal index
                // since we treat empty indices as 1, we are good to go
                continue;
            } else {
                // read the current right-wise dimension (keep in mind: negIdx is negative,
                //   so adding it to the length of that dimension is effectively computing it
                //   from the end)
                int this_dim = dims[i][Ndims[i] + negIdx];

                hadIdx = true;

                // no problem here; it will always be broadcastable
                if (this_dim == 1) continue;

                if (req_size < 0) {
                    // no current size set, so we get to be the first to set the requirement!
                    req_size = this_dim;
                    continue;
                } else if (this_dim != req_size) {
                    // error! the dimensions are not compatible
                    return false;
                }
            }
        }

        // set the result dimension to this
        R_dims[R_Ndim + negIdx] = req_size < 0 ? 1 : req_size;

        // if there were no indices checked, every single one must be out of range, so return
        if (!hadIdx) break;

        // otherwise, continue moving back to the left to check more
        negIdx--;
    }

    return true;
}

// broadcast a function to arguments
int nx_bcast(nx_ufunc_f ufunc, int n_args, nx_dtype* dtypes, void** args, int* Ndims, ks_ssize_t** dims, ks_ssize_t** strides) {
    assert(n_args > 0 && "nx_broadcast given no arguments!");


    // call the internal function
    return nxi_bcast(ufunc, n_args, dtypes, args, Ndims, dims, strides);
}

