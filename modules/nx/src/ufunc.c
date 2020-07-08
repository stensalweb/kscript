/* src/ufunc_new.c - new implementation of func
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

// internal application functions, where arrs[:].rank is constant
static bool my_apply(int N, nxar_t* arrs, nxar_t* arrs_1d, nx_size_t* dims, nx_size_t* idx, nx_ufunc_f ufunc, void* _user_data) {

    // loop over everything but the inner-most index
    int loop_N = arrs[0].rank - 1;

    // loop vars
    nx_size_t i;

    // zero out indices
    for (i = 0; i < loop_N; ++i) idx[i] = 0;

    nx_size_t sz0 = 0;

    while (true) {

        for (i = 0; i < N; ++i) {
            arrs_1d[i] = arrs[i];

            // convert to 1D slice
            arrs_1d[i].rank = 1;
            arrs_1d[i].dim = &arrs[i].dim[arrs[i].rank - 1];
            // either have stride of 0 (repeat element), or the stride that was given
            arrs_1d[i].stride = arrs_1d[i].dim[0] == 1 ? &sz0 : &arrs[i].stride[arrs[i].rank - 1];
            arrs_1d[i].data = nx_get_ptr(arrs[i].data, loop_N, arrs[i].dim, arrs[i].stride, idx);
        }


        // calculate 1D offsets
        if (!ufunc(N, arrs_1d, dims[arrs[0].rank - 1], _user_data)) return false;

        // increase least significant index
        i = loop_N - 1;
        if (i < 0) break;
        idx[i]++;

        while (i >= 0 && idx[i] >= dims[i]) {
            idx[i] = 0;
            i--;
            if (i >= 0) idx[i]++;
        }

        // done; we have overflowed
        if (i < 0) break;
    }

    
    // success
    return true;
}


// apply ufunc
bool nx_T_ufunc_apply(int N, nxar_t* arrs, nx_ufunc_f ufunc, void* _user_data) {

    // loop var
    nx_size_t i;

    // find the maximum rank
    int max_rank = arrs[0].rank;
    for (i = 1; i < N; ++i) if (arrs[i].rank > max_rank) max_rank = arrs[i].rank;

    // create new arrays with padded dims & strides
    nxar_t* new_arrs = ks_malloc(sizeof(*new_arrs) * N);
    nxar_t* new_arrs_1d = ks_malloc(sizeof(*new_arrs_1d) * N);
    nx_size_t* max_dims = ks_malloc(sizeof(*max_dims) * max_rank);
    nx_size_t* idxs = ks_malloc(sizeof(*idxs) * max_rank);

    for (i = 0; i < max_rank; ++i) max_dims[i] = 0;

    for (i = 0; i < N; ++i) {
        new_arrs[i] = arrs[i];

        // pad to maximum rank
        new_arrs[i].rank = max_rank;
        new_arrs[i].dim = ks_malloc(sizeof(*new_arrs[i].dim) * max_rank);
        new_arrs[i].stride = ks_malloc(sizeof(*new_arrs[i].stride) * max_rank);

        int j, jr = 0;
    
        // now, shift all shapes so that they are padded with '1''s on the left side (and their stride should be set to 0 in those places)
        // prepad:
        for (j = 0; j < max_rank - arrs[i].rank; ++j) {
            new_arrs[i].dim[j] = 1;
            new_arrs[i].stride[j] = 0;
        }

        // now, copy the rest so that it is aligned on the right
        for (jr = 0; j < max_rank; ++j, ++jr) {
            new_arrs[i].dim[j] = arrs[i].dim[jr];
            new_arrs[i].stride[j] = arrs[i].stride[jr];
        }

        // now, update max dimensions
        for (j = 0; j < max_rank; ++j) if (new_arrs[i].dim[j] > max_dims[j]) max_dims[j] = new_arrs[i].dim[j];
    }


    // now, apply:
    bool rst = my_apply(N, new_arrs, new_arrs_1d, max_dims, idxs, ufunc, _user_data);

    for (i = 0; i < N; ++i) {
//        ks_free(new_arrs[i].dim);
//        ks_free(new_arrs[i].stride);
    }

//    ks_free(new_arrs);
    ks_free(new_arrs_1d);
//    ks_free(max_dims);
//    ks_free(idxs);
    return rst;

}
