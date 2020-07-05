/* src/ufunc.c - implementation of ufunc logic
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// apply the ufunc, which may call itself recursively
// Both 'dims' and 'strides' are interpreted as a row-major 2D array containing:
// [data0...]
// [data1...]
// i.e. the size information for each 'datas'
// use the dims__ and strides__ macro to index them
// NOTE: this assumes that they are all the same dimensions, which their dimensions should have been padded and normalized
//   so that extra dimensions are created to be '1'
static int my_apply(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, int N, nx_size_t* _dims, nx_size_t* _strides, nx_ufunc_f ufunc, void* _user_data) {

    // macros to turn 1D arrays into 2D
    #define dims__(_i, _j) _dims[N * (_i) + (_j)]
    #define strides__(_i, _j) _strides[N * (_i) + (_j)]

    if (N == 0) {
        // 0-dimensional data, special case
        // don't do anything
        return 0;
    } else if (N == 1) {
        // 1-dimensional data, call the ufunc
        // we need to determine the largest size of any,
        // as there should be dims of either 1 or `len`
        // the sizes of '1' should have a stride of 0

        // loop variables
        int i;


        // find maximum length
        nx_size_t c_len = dims__(0, 0);
        for (i = 1; i < Nin; ++i) if (dims__(i, 0) > c_len) c_len = dims__(i, 0);

        // now, calculate the relevant strides
        nx_size_t* g_strides = ks_malloc(sizeof(*g_strides) * Nin);

        // cop them over
        for (i = 0; i < Nin; ++i) {
            // if there is only 1 item, the stride should be 0 so it is the same value over and over
            if (dims__(i, 0) == 1) {
                // continually repeat the same value
                g_strides[i] = 0;
            } else {
                // take the offset given
                g_strides[i] = strides__(i, 0);
            }
        }
        // actually call ufunc
        int stat = ufunc(Nin, datas, dtypes, dtype_sizes, c_len, g_strides, _user_data);

        // free tmp resources
        ks_free(g_strides);
      
        return stat;
    } else {
        // otherwise, recrusively reorganize calls and reduce further to 1D runs

        // loop variables
        int i, j;

        // calculate the max dimension
        int max_dim = dims__(0, 0);
        for (i = 1; i < Nin; ++i) if (dims__(i, 0) > max_dim) max_dim = dims__(i, 0);

        // temporary data array
        void** g_datas = ks_malloc(sizeof(*g_datas) * Nin);

        // now, allocate temporary variables for looping
        nx_size_t* g_dims = ks_malloc(sizeof(*g_dims) * Nin * (N - 1));
        nx_size_t* g_strides = ks_malloc(sizeof(*g_strides) * Nin * (N - 1));

        // macro to turn the 1D arrays into 2D arrays
        #define g_dims__(_i, _j) g_dims[(N - 1) * (_i) + (_j)]
        #define g_strides__(_i, _j) g_strides[(N - 1) * (_i) + (_j)]

        // shift dimensions, ignoring the leftmost, since we are currently iterating on that
        // thus, we reduce the problem by 1 dimension
        for (i = 0; i < Nin; ++i) {
            for (j = 0; j < N - 1; ++j) {
                //printf("TEST: %p\n", dims[i]);
                g_dims__(i, j) = dims__(i, j + 1);

                if (g_dims__(i, j) == 1) {
                    // don't move the entire time, i.e. broadcast the value
                    g_strides__(i, j) = 0;
                } else {
                    g_strides__(i, j) = strides__(i, j + 1);
                }
            }

        }

        // now, recursively call ourselves with each slice
        for (i = 0; i < max_dim; ++i) {
            // prepare arguments
            for (j = 0; j < Nin; ++j) {
                if (dims__(j, 0) == 1) {
                    // broadcast the value over and over with no stride
                    g_datas[j] = datas[j];
                } else {
                    // adjust strides off, since we are looping through different chunks of the array
                    g_datas[j] = (void*)( (uintptr_t)datas[j] + strides__(j, 0) * dtype_sizes[j] * i );
                }
            }

            // recursively apply
            int stat = my_apply(Nin, g_datas, dtypes, dtype_sizes, N-1, g_dims, g_strides, ufunc, _user_data);
            if (stat != 0) return stat;

        }

        // destroy temporary arrays
        ks_free(g_datas);
        ks_free(g_dims);
        ks_free(g_strides);


        // undefine utility macros
        #undef g_dims__
        #undef g_strides__

        return 0;
    }

}


// API function to apply a ufunc
bool nx_T_apply_ufunc(int Nin, void** datas, enum nx_dtype* dtypes, int* N, nx_size_t** dims, nx_size_t** strides, nx_ufunc_f ufunc, void* _user_data) {

    // ensure they can broadcast together
    if (!nx_can_bcast(Nin, N, dims)) return 1;

    // loop vars
    int i, j, jr;

    // calculate the maximum dimension
    nx_size_t max_N = N[0];
    for (i = 1; i < Nin; ++i) if (N[i] > max_N) max_N = N[i];

    // store dtype sizes for efficiency
    nx_size_t* dtype_sizes = ks_malloc(sizeof(*dtype_sizes) * Nin);

    // create the arrays that will be passed to calls of the ufunc
    nx_size_t* g_dims = ks_malloc(sizeof(*g_dims) * Nin * max_N);
    nx_size_t* g_strides = ks_malloc(sizeof(*g_strides) * Nin * max_N);

    // macro to turn the 1D arrays into 2D arrays
    #define g_dims__(_i, _j) g_dims[max_N * (_i) + (_j)]
    #define g_strides__(_i, _j) g_strides[max_N * (_i) + (_j)]

    // calculate padded dimensions & strides
    for (i = 0; i < Nin; ++i) {
        // calculate size
        dtype_sizes[i] = nx_dtype_size(dtypes[i]);

        // now, shift all shapes so that they are padded with '1''s on the left side (and their stride should be set to 0 in those places)
        // prepad:
        for (j = 0; j < max_N - N[i]; ++j) {
            g_dims__(i, j) = 1;
            g_strides__(i, j) = 0;
        }

        // now, copy the rest so that it is aligned on the right
        for (jr = 0; j < max_N; ++j, ++jr) {
            g_dims__(i, j) = dims[i][jr];
            g_strides__(i, j) = strides[i][jr];
        }
    }


    int stat = my_apply(Nin, datas, dtypes, dtype_sizes, max_N, g_dims, g_strides, ufunc, _user_data);

    // free temporary resources
    ks_free(dtype_sizes);
    ks_free(g_dims);
    ks_free(g_strides);

    // undefine utility macros
    #undef g_dims__
    #undef g_strides__

    return stat == 0;

}


