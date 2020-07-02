/* src/str.c - string implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal, recursive string builder
static bool my_get_str(ks_str_b* SB, void* data, enum nx_dtype dtype, nx_size_t dtype_size, int N, nx_size_t* dim, nx_size_t* stride, int depth) {

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
        nx_size_t sb_A = stride[0] * dtype_size;

        char tmp[256];

        // inner loop
        #define INNER_LOOP(NXT_TYPE_ENUM_A, NXT_TYPE_A) { \
            if (i > 0) ks_str_b_add_c(SB, ", "); \
            if (NXT_TYPE_ENUM_A >= NX_DTYPE_SINT8 && NXT_TYPE_ENUM_A <= NX_DTYPE_UINT64) ks_str_b_add_fmt(SB, "%z", (ks_ssize_t)*(NXT_TYPE_A*)dptr_A); \
            else if (NXT_TYPE_ENUM_A >= NX_DTYPE_FP32 && NXT_TYPE_ENUM_A <= NX_DTYPE_FP64) { \
                int len = snprintf(tmp, sizeof(tmp) - 1, "%5.2f", (double)*(NXT_TYPE_A*)dptr_A); \
                ks_str_b_add(SB, len, tmp); \
            } \
            else if (NXT_TYPE_ENUM_A >= NX_DTYPE_CPLX_FP32 && NXT_TYPE_ENUM_A <= NX_DTYPE_CPLX_FP64) { \
                int len = snprintf(tmp, sizeof(tmp) - 1, "%.4f%+.4fi", (double)creal(*(NXT_TYPE_A*)dptr_A), (double)cimag(*(NXT_TYPE_A*)dptr_A)); \
                ks_str_b_add(SB, len, tmp); \
            } else { \
                ks_throw_fmt(ks_type_ToDoError, "Internal enum idx '%i' not handled in get_str", (int)NXT_TYPE_ENUM_A); \
                return false; \
            } \
        }

        nx_size_t dim0 = dim[0];
        if (dim0 >= trunc_size) dim0 = trunc_size - 1;

        // actually generate the loop body with all optimizations possible
        NXT_GENERATE_1A(dim0, (&dtype), dptr_A, sb_A, INNER_LOOP)

        if (dim[0] >= trunc_size) {
            ks_str_b_add_fmt(SB, " ...(%z) ", dim[0] - trunc_size);

            dptr_A = (intptr_t)data + sb_A * (dim[0] - 1);
            nx_size_t d1 = 1;

            NXT_GENERATE_1A(d1, (&dtype), dptr_A, sb_A, INNER_LOOP)
        }


        // end array
        ks_str_b_add_c(SB, "]");

        return true;

    } else {
        // otherwise, recrusively call the my_get_str with sub-arrays

        // convert to an integer for math, and compute the amount changed per entry
        intptr_t data_i = (intptr_t)data, data_i_delta = stride[0] * dtype_size;

        ks_str_b_add_c(SB, "[");

        nx_size_t dim0 = dim[0];
        if (dim0 >= trunc_size) dim0 = trunc_size - 1;


        // loop through all outer dimensions
        int i;
        for (i = 0; i < dim0; i++, data_i += data_i_delta) {
            if (i > 0) ks_str_b_add_fmt(SB, "\n%*c", depth + 1, ' ');

            bool stat = my_get_str(SB, (void*)data_i, dtype, dtype_size, N-1, dim+1, stride+1, depth + 1);
            if (!stat) return false;
        }

        if (dim[0] >= trunc_size) {
            ks_str_b_add_fmt(SB, "\n%*c...(%z)", depth + 1, ' ', dim[0] - trunc_size);
            ks_str_b_add_fmt(SB, "\n%*c", depth + 1, ' ');

            data_i = (intptr_t)data + data_i_delta * (dim[0] - 1);
            
            bool stat = my_get_str(SB, (void*)data_i, dtype, dtype_size, N-1, dim+1, stride+1, depth + 1);
            if (!stat) return false;
        }



        ks_str_b_add_c(SB, "]");

        return true;
    }


}


// generate string for it
ks_str nx_get_str(void* data, enum nx_dtype dtype, int N, nx_size_t* dim, nx_size_t* stride) {


    ks_str_b SB;
    ks_str_b_init(&SB);

    bool stat = my_get_str(&SB, data, dtype, nx_dtype_size(dtype), N, dim, stride, 0);

    ks_str res = ks_str_b_get(&SB);
    ks_str_b_free(&SB);


    return res;
}



