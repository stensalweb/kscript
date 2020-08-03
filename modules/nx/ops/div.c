/* src/div.c - divide elementwise
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for adding elementwise
// datas[0] / datas[1] -> datas[2]
static bool my_div_1d(int N, nxar_t* arrs, int len, void* _user_data) {
    NX_ASSERT_CHECK(N == 3);

    // loop vars
    nx_size_t i;

    // convert to integers for math
    intptr_t dptr_0 = (intptr_t)arrs[0].data, dptr_1 = (intptr_t)arrs[1].data, dptr_2 = (intptr_t)arrs[2].data;
    nx_size_t strd_0 = arrs[0].stride[0], strd_1 = arrs[1].stride[0], strd_2 = arrs[2].stride[0];

    // 1D computation loop template
    #define INNER_LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1, NXT_DTYPE_2, NXT_TYPE_2) for (i = 0; i < len; ++i, dptr_0 += strd_0, dptr_1 += strd_1, dptr_2 += strd_2) { \
        *(NXT_TYPE_2*)dptr_2 = *(NXT_TYPE_0*)dptr_0 / *(NXT_TYPE_1*)dptr_1; \
    }

    // generate all combinations
    NXT_COMBO_3A(INNER_LOOP, arrs[0].dtype, arrs[1].dtype, arrs[2].dtype);

    // stop using the macro
    #undef INNER_LOOP

    // success
    return true;
}

// calc A + B -> C
bool nx_T_div(nxar_t A, nxar_t B, nxar_t C) {

    // apply the ufunc
    return nx_T_ufunc_apply(3, (nxar_t[]){ A, B, C }, my_div_1d, NULL);
}


