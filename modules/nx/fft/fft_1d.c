/* fft/fft_1d.c - implementation of the 1D FFT
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// Compute B = FFT(A)
bool nx_T_fft_1d(nx_fft_plan_t* plan0, int axis0, nxar_t A, nxar_t B) {

    // checks
    if (A.N != 1 || B.N != 1) {
        ks_throw_fmt(ks_type_ToDoError, "nx_T_fft_1d not implemented for larger-than-1 dimension tensors");
        return false;
    }

    // Result size & stride (stride==1, since it is a dense 1D array)
    nx_size_t R_dim0 = A.dim[axis0], R_stride = 1;

    // return status
    bool rstat = false;

    // Result buffer for a single 1D slice
    double complex* R = ks_malloc(sizeof(*R) * R_dim0);

    // nxar for R
    nxar_t Ra = (nxar_t){ .data = R, .dtype = NX_DTYPE_CPLX_FP64, .N = 1, .dim = &R_dim0, .stride = &R_stride };

    // Now, fill 'R" with numbers from 'A'
    // TODO: allow multi-dimensional input, and then loop over the last dimension
    if (!nx_T_cast(A, Ra)) goto cleanup;

    // perform the computation
    if (!nx_fft_plan_do_1Dip(plan0, R)) goto cleanup;

    // now, set 'B' to the value, casted
    if (!nx_T_cast(Ra, B)) goto cleanup;

    // success
    rstat = true;

    cleanup:

    // free temporary resources
    if (R) ks_free(R);

    return rstat;
}








