/* src/sub.c - subtract elementwise
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for subtracting elementwise
// datas[0] - datas[1] -> datas[2]
static bool my_sub_1d(int Nin, void** datas, nx_dtype* dtypes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 3);

    // loop vars
    nx_size_t i;

    // convert to integers for math
    intptr_t dptr_0 = (intptr_t)datas[0], dptr_1 = (intptr_t)datas[1], dptr_2 = (intptr_t)datas[2];

    // 1D computation loop template
    #define INNER_LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1, NXT_DTYPE_2, NXT_TYPE_2) for (i = 0; i < dim; ++i, dptr_0 += strides[0], dptr_1 += strides[1], dptr_2 += strides[2]) { \
        *(NXT_TYPE_2*)dptr_2 = *(NXT_TYPE_0*)dptr_0 - *(NXT_TYPE_1*)dptr_1; \
    }

    // generate all combinations
    NXT_COMBO_3A(INNER_LOOP, dtypes);

    // stop using the macro
    #undef INNER_LOOP

    // success
    return true;
}

// calc A - B -> C
bool nx_T_sub(nxar_t A, nxar_t B, nxar_t C) {

    // apply the ufunc
    return nx_T_apply_ufunc(3, 
        (void*[]){ A.data, B.data, C.data }, 
        (nx_dtype[]){ A.dtype, B.dtype, C.dtype }, 
        (int[]){ A.rank, B.rank, C.rank }, 
        (nx_size_t*[]){ A.dim, B.dim, C.dim }, 
        (nx_size_t*[]) { A.stride, B.stride, C.stride },
        my_sub_1d,
        NULL
    );
}


