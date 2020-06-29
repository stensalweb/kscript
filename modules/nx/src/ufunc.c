/* src/ufunc.c - implementation of ufunc logic
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// apply the ufunc, which may call itself recursively
static int my_apply() {
    return 0;
}



int nx_T_apply_ufunc(int Nin, enum nx_dtype* dtypes, void** datas, int* N, nx_size_t** dims, nx_size_t** strides, nx_ufunc_f ufunc, void* _user_data) {


    // create the arrays that will be passed to calls of the ufunc
    nx_size_t* dtype_sizes = ks_malloc(sizeof(*dtype_sizes) * Nin);
    nx_size_t* g_dims = ks_malloc(sizeof(*g_dims) * Nin);
    nx_size_t* g_strides = ks_malloc(sizeof(*g_strides) * Nin);

    int i, j;
    for (i = 0; i < Nin; ++i) {
        dtype_sizes[i] = nx_dtype_size(dtypes[i]);
    }

    for (i = 0; i < Nin; ++i) {
        g_dims[i] = 10;
        g_strides[i] = 1;
    }


    int stat = ufunc(Nin, dtypes, dtype_sizes, datas, g_dims, g_strides, _user_data);

    // free temporary resources
    ks_free(dtype_sizes);
    ks_free(g_dims);
    ks_free(g_strides);

    return stat;

}


