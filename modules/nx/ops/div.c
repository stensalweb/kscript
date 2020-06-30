/* src/div.c - divide elementwise
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for dividing elementwise
// datas[0] / datas[1] -> datas[2]
static int my_div_1d(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 3);

    // loop vars
    int i;

    // convert to integers for math
    intptr_t dptr_A = (intptr_t)datas[0], dptr_B = (intptr_t)datas[1], dptr_C = (intptr_t)datas[2];
    // stride (in bytes)
    intptr_t sb_A = strides[0] * dtype_sizes[0], sb_B = strides[1] * dtype_sizes[1], sb_C = strides[2] * dtype_sizes[2];


    // inner loop
    #define INNER_LOOP(NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B, NXT_TYPE_ENUM_C, NXT_TYPE_C) { \
        *(NXT_TYPE_C*)dptr_C = *(NXT_TYPE_A*)dptr_A - *(NXT_TYPE_B*)dptr_B; \
    }

    // generate a huge body containing all the data combinations
    NXT_GENERATE_3A(dim, dtypes, dptr_A, dptr_B, dptr_C, sb_A, sb_B, sb_C, INNER_LOOP)


    // stop using the macro
    #undef INNER_LOOP

    // success
    return 0;
}

// compute A / B -> C
bool nx_T_div(
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride, 
    void* C, enum nx_dtype C_dtype, int C_N, nx_size_t* C_dim, nx_size_t* C_stride) {

    // apply the ufunc
    return nx_T_apply_ufunc(3, (void*[]){ A, B, C }, 
        (enum nx_dtype[]){ A_dtype, B_dtype, C_dtype }, 
        (int[]){ A_N, B_N, C_N }, 
        (nx_size_t*[]){ A_dim, B_dim, C_dim }, 
        (nx_size_t*[]) { A_stride, B_stride, C_stride },
        my_div_1d,
        NULL
    ) == 0;
}


