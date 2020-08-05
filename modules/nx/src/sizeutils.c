/* src/sizeutils.c - basic utilities for shapes & sizes
 *
 *
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// check whether the list of arguments (of which there are Nin) are broadcastable together
bool nx_can_bcast(int Nin, int* N, nx_size_t** dims) {
    NX_ASSERT_CHECK(Nin > 0 && "no arguments in broadcast!");

    // when considering broadcastability, we borrow from the rules from NumPy:
    // Sizes are right aligned, i.e. begin from the right, so comparing tensors
    //   of shapes '[5x3x2]' and '[3x1]', we would write:
    // 5x3x2
    //   3x1
    // Missing entries to the left are considered '1' (since there would be exactly 1 element,
    //   containing the entire array)
    //
    // Then, compare each entry the arrays are able to be broadcast if the columns
    //   are either 1, or 1 other number
    // if they are 1, then the same value is interpreted for all entries on that axis,
    //   otherwise they are iterated per each


    // negative index (from end end of each dims[i]), so we can
    //   easily do logic beginning from the right of each array
    int ni = -1;


    while (true) {
        // the size (other than 1) required for the current dimension
        //   or -1 if a number other than 1 has not been encountered yet
        nx_size_t req_size = -1;

        // keep track if we checked any dimensions
        // if not, we need to stop since we are out of range for all of them,
        //   and have checked everything we need
        bool hadDim = false;

        int i;
        // loop through and check each argument's 'ni'th dimension for compatibility
        for (i = 0; i < Nin; ++i) {
            if (N[i] < -ni) {
                // since we are right-aligned, we are out of bounds for this array
                // in this case, interpret the empty index as 1, and keep going
                continue;
            } else {
                // get the current dimension value
                // NOTE: since 'ni' is always negative, adding it to the size effectively
                //   computes the index from the right of 'dims'
                nx_size_t this_dim = dims[i][N[i] + ni];
                hadDim = true;

                if (this_dim == 1) {
                    // single value, which can be broadcasted with every other size,
                    //   so it is still valid
                    continue;
                } else if (req_size < 0) {
                    // no size requirement exists yet, so this one
                    //   will be the requirement
                    // now, any other sizes for this dimension must be either '1' or 'this_dim'
                    req_size = this_dim;
                    continue;
                } else if (this_dim == req_size) {
                    // it matches the size requirement set, all is good
                    continue;
                } else {
                    // something else happened, and the shapes are not broadcastable
                    ks_str_builder sb = ks_str_builder_new();

                    for (i = 0; i < Nin; ++i) {
                        if (i > 0) ks_str_builder_add(sb, ", ", 2);
                        ks_str_builder_add_fmt(sb, "(%+z,)", N[i], dims[i]);
                    }

                    ks_str rstr = ks_str_builder_get(sb);
                    KS_DECREF(sb);
                    ks_throw(ks_T_SizeError, "Shapes %S were not broadcastable!", rstr);
                    KS_DECREF(rstr);
                    return false;
                }
            }
        } 

        // if no dimensions were processed, then we are done checking
        if (!hadDim) break;

        // keep moving back 1 dimension every loop
        ni--;

    }

    // success, they can be broadcasted
    return true;
}

// compute the broadcast dimensions (i.e. the result dimensions) from a list of inputs
bool nx_compute_bcast(int Nin, int* N, nx_size_t** dims, int R_N, nx_size_t* R_dims) {
    // loop vars
    int i;

    // compute the maximum dimension of any tensor, which will be the dimension of the result
    int max_N = N[0];
    for (i = 1; i < Nin; ++i) if (N[i] > max_N) max_N = N[i];

    if (max_N != R_N) {
        ks_throw(ks_T_SizeError, "nx_compute_broadcast given R_N != max(N) (%i != %i)!", max_N, R_N);
        return false;
    }

    // negative index (from end end of each dims[i]), so we can
    //   easily do logic beginning from the right of each array
    int ni = -1;

    // keep going while valid
    while (true) {
        // the size (other than 1) required for the current dimension
        //   or -1 if a number other than 1 has not been encountered yet
        nx_size_t req_size = -1;

        // keep track if we checked any dimensions
        // if not, we need to stop since we are out of range for all of them,
        //   and have checked everything we need
        bool hadDim = false;
        
        // loop through and check each argument's 'ni'th dimension for compatibility
        for (i = 0; i < Nin; ++i) {
            if (N[i] < -ni) {
                // since we are right-aligned, we are out of bounds for this array
                // in this case, interpret the empty index as 1, and keep going
                continue;
            } else {
                // get the current dimension value
                // NOTE: since 'ni' is always negative, adding it to the size effectively
                //   computes the index from the right of 'dims'
                nx_size_t this_dim = dims[i][N[i] + ni];
                hadDim = true;

                if (this_dim == 1) {
                    // single value, which can be broadcasted with every other size,
                    //   so it is still valid
                    continue;
                } else if (req_size < 0) {
                    // no size requirement exists yet, so this one
                    //   will be the requirement
                    // now, any other sizes for this dimension must be either '1' or 'this_dim'
                    req_size = this_dim;
                    continue;
                } else if (this_dim == req_size) {
                    // it matches the size requirement set, all is good
                    continue;
                } else {
                    // something else happened, and the shapes are not broadcastable
                    ks_str_builder sb = ks_str_builder_new();

                    for (i = 0; i < Nin; ++i) {
                        if (i > 0) ks_str_builder_add(sb, ", ", 2);
                        ks_str_builder_add_fmt(sb, "(%+z,)", N[i], dims[i]);
                    }

                    ks_str rstr = ks_str_builder_get(sb);
                    KS_DECREF(sb);
                    ks_throw(ks_T_SizeError, "Shapes %S were not broadcastable!", rstr);
                    KS_DECREF(rstr);
                    return false;
                }
            }
        } 

        // if there were no indices checked, every single one must be out of range, so return
        if (!hadDim) break;

        // set the result dimension to the required size (or 1, if none was set)
        R_dims[R_N + ni] = req_size < 0 ? 1 : req_size;

        // otherwise, continue moving back to the left to check more
        ni--;
    }

    // success, and 'R_dims' was set
    return true;
}


// size,stride dot
nx_size_t nx_szsdot(int N, nx_size_t* dim, nx_size_t* stride, nx_size_t* idxs) {

    // result offset (in elements)
    nx_size_t r = 0;

    int i;
    for (i = 0; i < N; ++i) {
        int64_t ci = idxs[i];
        ci = ((ci % dim[i]) + dim[i]) % dim[i];
        r += stride[i] * ci;
    }

    // convert to bytes
    return r;
}


// get pointer to element
void* nx_get_ptr(void* data, int N, nx_size_t* dim, nx_size_t* stride, nx_size_t* idx) {
    return (void*)((intptr_t)data + nx_szsdot(N, dim, stride, idx));
}
