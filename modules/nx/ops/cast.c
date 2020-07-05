/* src/cast.c - cast elementwise
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal 1D loop for adding elementwise
// datas[1] = (dtypes[1])datas[0]
static bool my_cast_1d(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 2);

    // loop vars
    int i;

    // convert to integers for math
    intptr_t dptr_A = (intptr_t)datas[0], dptr_B = (intptr_t)datas[1];
    // stride (in bytes)
    intptr_t sb_A = strides[0] * dtype_sizes[0], sb_B = strides[1] * dtype_sizes[1];

    // inner loop
    #define INNER_LOOP(NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B) { \
        *(NXT_TYPE_B*)dptr_B = *(NXT_TYPE_A*)dptr_A; \
    }

    // generate a huge body containing all the data combinations
    NXT_GENERATE_2A(dim, dtypes, dptr_A, dptr_B, sb_A, sb_B, INNER_LOOP)


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
        (enum nx_dtype[]){ A.dtype, B.dtype }, 
        (int[]){ A.N, B.N }, 
        (nx_size_t*[]){ A.dim, B.dim }, 
        (nx_size_t*[]) { A.stride, B.stride },
        my_cast_1d,
        NULL
    );
}


