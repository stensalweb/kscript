/* src/div.c - divide elementwise
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for dividing elementwise
// datas[0] / datas[1] -> datas[2]
static bool my_div_1d(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 3);

    // loop vars
    int i;

    // convert to integers for math
    intptr_t dptr_A = (intptr_t)datas[0], dptr_B = (intptr_t)datas[1], dptr_C = (intptr_t)datas[2];
    // stride (in bytes)
    intptr_t sb_A = strides[0] * dtype_sizes[0], sb_B = strides[1] * dtype_sizes[1], sb_C = strides[2] * dtype_sizes[2];


    // inner loop
    #define INNER_LOOP(NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B, NXT_TYPE_ENUM_C, NXT_TYPE_C) { \
        *(NXT_TYPE_C*)dptr_C = *(NXT_TYPE_A*)dptr_A / *(NXT_TYPE_B*)dptr_B; \
    }

    // generate a huge body containing all the data combinations
    NXT_GENERATE_3A(dim, dtypes, dptr_A, dptr_B, dptr_C, sb_A, sb_B, sb_C, INNER_LOOP)


    // stop using the macro
    #undef INNER_LOOP

    // success
    return true;
}

// calc A / B -> C
bool nx_T_div(nxar_t A, nxar_t B, nxar_t C) {

    // apply the ufunc
    return nx_T_apply_ufunc(3, 
        (void*[]){ A.data, B.data, C.data }, 
        (enum nx_dtype[]){ A.dtype, B.dtype, C.dtype }, 
        (int[]){ A.N, B.N, C.N }, 
        (nx_size_t*[]){ A.dim, B.dim, C.dim }, 
        (nx_size_t*[]) { A.stride, B.stride, C.stride },
        my_div_1d,
        NULL
    );
}





