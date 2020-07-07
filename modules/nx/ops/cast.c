/* src/cast.c - cast elementwise
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for adding elementwise
// datas[1] = (dtypes[1])datas[0]
static bool my_cast_1d(int Nin, void** datas, nx_dtype* dtypes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 2);

    // loop vars
    nx_size_t i;

    // convert to integers for math
    intptr_t dptr_0 = (intptr_t)datas[0], dptr_1 = (intptr_t)datas[1];

    // 1D computation loop template
    #define INNER_LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1) for (i = 0; i < dim; ++i, dptr_0 += strides[0], dptr_1 += strides[1]) { \
        *(NXT_TYPE_1*)dptr_1 = *(NXT_TYPE_0*)dptr_0; \
    }

    // generate all combinations
    NXT_COMBO_2A(INNER_LOOP, dtypes);

    // stop using the macro
    #undef INNER_LOOP

    // success
    return true;
}

// calc B = (type(B))A
bool nx_T_cast(nxar_t A, nxar_t B) {

    // apply the ufunc
    return nx_T_apply_ufunc(2, 
        (void*[]){ A.data, B.data }, 
        (nx_dtype[]){ A.dtype, B.dtype }, 
        (int[]){ A.rank, B.rank }, 
        (nx_size_t*[]){ A.dim, B.dim }, 
        (nx_size_t*[]) { A.stride, B.stride },
        my_cast_1d,
        NULL
    );
}


