/* modules/nx/src/broadcast.c - broadcasting utilities for vectorized inputs
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "nx-impl.h"


int nx_broadcast(nx_ufunc_f ufunc, int n_args, nx_dtype* dtypes, uintptr_t* args, int* Ndims, ks_ssize_t** dims, ks_ssize_t** strides) {
    

    //ks_ssize_t* idxs = ks_malloc(sizeof(*idxs) * 0);

    ks_throw_fmt(ks_type_ToDoError, "Broadcast not done!");
    return 1;

    // success
    return 0;
}

