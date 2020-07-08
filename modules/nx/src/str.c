/* src/str.c - string implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal, recursive string builder
static bool my_get_str(ks_str_b* SB, void* data, nx_dtype dtype, int N, nx_size_t* dim, nx_size_t* stride, int depth) {

    // truncation size
    nx_size_t trunc_size = 20;

    if (N == 0) {
        // 0-dimensional data, special case
        ks_str_b_add_c(SB, "[]");
        
        return true;
    } else if (N == 1) {
        // 1-dimensional data

        ks_str_b_add_c(SB, "[");


        // loop vars
        int i;

        // pointer & stride (in bytes)
        intptr_t dptr_A = (intptr_t)data;

        // temporary storage
        char tmp[256];

        nx_size_t dim0 = dim[0];
        if (dim0 >= trunc_size) dim0 = trunc_size - 1;

        #define GET_STR_LOOP(NXT_DTYPE_0, NXT_TYPE_0) for (i = 0; i < dim0; ++i, dptr_A += stride[0]) { \
            if (i > 0) ks_str_b_add_c(SB, ", "); \
            if (NXT_DTYPE_0->kind == NX_DTYPE_KIND_CINT) ks_str_b_add_fmt(SB, "%z", (ks_ssize_t)*(NXT_TYPE_0*)dptr_A); \
            else if (NXT_DTYPE_0->kind == NX_DTYPE_KIND_CFLOAT) { \
                int len = snprintf(tmp, sizeof(tmp) - 1, "%5.2f", (double)*(NXT_TYPE_0*)dptr_A); \
                ks_str_b_add(SB, len, tmp); \
            } \
            else if (NXT_DTYPE_0->kind == NX_DTYPE_KIND_CCOMPLEX) { \
                int len = snprintf(tmp, sizeof(tmp) - 1, "%5.2f%+5.2fi", (double)creal(*(NXT_TYPE_0*)dptr_A), (double)cimag(*(NXT_TYPE_0*)dptr_A)); \
                ks_str_b_add(SB, len, tmp); \
            } else { \
                ks_throw_fmt(ks_type_ToDoError, "Internal dtype '%S' not handled in get_str", NXT_DTYPE_0); \
                return false; \
            } \
        }

        NXT_COMBO_1A(GET_STR_LOOP, dtype);

        if (dim[0] >= trunc_size) {
            ks_str_b_add_fmt(SB, " ...(%z) ", dim[0] - trunc_size);

            dptr_A = (intptr_t)data + stride[0] * (dim[0] - 1);
            nx_size_t d1 = 1;

            dim0 = 1;
            NXT_COMBO_1A(GET_STR_LOOP, dtype);
        }


        // end array
        ks_str_b_add_c(SB, "]");

        return true;

    } else {
        // otherwise, recrusively call the my_get_str with sub-arrays

        // convert to an integer for math, and compute the amount changed per entry
        intptr_t data_i = (intptr_t)data;

        ks_str_b_add_c(SB, "[");

        nx_size_t dim0 = dim[0];
        if (dim0 >= trunc_size) dim0 = trunc_size - 1;


        // loop through all outer dimensions
        int i;
        for (i = 0; i < dim0; i++, data_i += stride[0]) {
            if (i > 0) ks_str_b_add_fmt(SB, "\n%*c", depth + 1, ' ');

            bool stat = my_get_str(SB, (void*)data_i, dtype, N-1, dim+1, stride+1, depth + 1);
            if (!stat) return false;
        }

        if (dim[0] >= trunc_size) {
            ks_str_b_add_fmt(SB, "\n%*c...(%z)", depth + 1, ' ', dim[0] - trunc_size);
            ks_str_b_add_fmt(SB, "\n%*c", depth + 1, ' ');

            data_i = (intptr_t)data + stride[0] * (dim[0] - 1);
            
            bool stat = my_get_str(SB, (void*)data_i, dtype, N-1, dim+1, stride+1, depth + 1);
            if (!stat) return false;
        }

        ks_str_b_add_c(SB, "]");

        return true;
    }


}


// generate string for it
ks_str nx_get_str(nxar_t A) {


    ks_str_b SB;
    ks_str_b_init(&SB);

    bool stat = my_get_str(&SB, A.data, A.dtype, A.rank, A.dim, A.stride, 0);

    ks_str res = ks_str_b_get(&SB);
    ks_str_b_free(&SB);


    return res;
}



