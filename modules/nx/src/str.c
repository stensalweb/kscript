/* src/str.c - string implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// internal, recursive string builder
static bool my_get_str(ks_str_b* SB, void* data, enum nx_dtype dtype, nx_size_t dtype_size, int N, nx_size_t* dim, nx_size_t* stride, int depth) {

    if (N == 0) {
        // 0-dimensional data, special case
        ks_str_b_add_c(SB, "[]");
        
        return true;
    } else if (N == 1) {
        // 1-dimensional data

        ks_str_b_add_c(SB, "[");

        // TODO: add more generic string generation
        intptr_t data_i = (intptr_t)data;

        // add in data
        int i;
        for (i = 0; i < dim[0]; ++i, data_i += stride[0] * dtype_size) {
            if (i > 0) ks_str_b_add_c(SB, ", ");
            ks_str_b_add_fmt(SB, "%f", *(float*)data_i);
        }

        ks_str_b_add_c(SB, "]");

        return true;
    } else {
        // otherwise, recrusively call the my_get_str with sub-arrays

        // convert to an integer for math, and compute the amount changed per entry
        intptr_t data_i = (intptr_t)data, data_i_delta = stride[0] * dtype_size;

        ks_str_b_add_c(SB, "[");


        // loop through all outer dimensions
        int i;
        for (i = 0; i < dim[0]; i++, data_i += data_i_delta) {
            if (i > 0) ks_str_b_add_fmt(SB, "\n%*c", depth + 1, ' ');

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


