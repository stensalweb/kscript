/* src/add.c - adding arrays together
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for adding together things
// datas[0] + datas[1] -> datas[2]
static int my_add_1d(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 3);

    // loop vars
    int i;

    // TODO: support other data types
    for (i = 0; i < Nin; ++i) NX_ASSERT_CHECK(dtypes[i] == NX_DTYPE_FP32);


    // convert to integers for math
    intptr_t d0 = (intptr_t)datas[0], d1 = (intptr_t)datas[1], d2 = (intptr_t)datas[2];
    intptr_t s0 = strides[0] * dtype_sizes[0], s1 = strides[1] * dtype_sizes[1], s2 = strides[2] * dtype_sizes[2];


    // actually add together
    for (i = 0; i < dim; ++i, d0 += s0, d1 += s1, d2 += s2) {
        *(float*)d2 = *(float*)d0 + *(float*)d1;
    }


    // success
    return 0;
}

// add A + B -> C
bool nx_T_add(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride, 
    void* C, enum nx_dtype C_dtype, int C_N, nx_size_t* C_dim, nx_size_t* C_stride) {

    // apply the ufunc
    return nx_T_apply_ufunc(3, (void*[]){ A, B, C }, 
        (enum nx_dtype[]){ A_dtype, B_dtype, C_dtype }, 
        (int[]){ A_N, B_N, C_N }, 
        (nx_size_t*[]){ A_dim, B_dim, C_dim }, 
        (nx_size_t*[]) { A_stride, B_stride, C_stride },
        my_add_1d,
        NULL
    ) == 0;
}


