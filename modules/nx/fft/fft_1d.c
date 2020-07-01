/* fft/fft_1d.c - implementation of the 1D FFT
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// Compute B = FFT(A)
bool nx_T_fft_1d(
    enum nx_fft_flags flags,
    nx_fft_plan_t* plan0, 
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
) {
    // checks
    if (A_N != 1 || B_N != 1) {
        ks_throw_fmt(ks_type_ToDoError, "nx_T_fft_1d not implemented for larger-than-1 dimension tensors");
        return false;
    }

    // what axis is the FFT computed over
    int axis_0 = A_N - 1;

    // Result size & stride (stride==1, since it is a dense 1D array)
    nx_size_t R_dim0 = A_dim[axis_0], R_stride = 1;

    // if the given plan was NULL, we need to create a plan just for this invocation
    bool createdPlan0 = false;

    // return status
    bool rstat = false;

    /* 1: assure the FFT plans are valid (create them if NULL is given, error if they are the wrong size) */
    if (createdPlan0 = plan0 == NULL) {
        plan0 = ks_malloc(sizeof(*plan0));
        if (!nx_fft_plan_init(plan0, R_dim0)) {
            ks_free(plan0);
            plan0 = NULL;
            goto cleanup;
        }
    } else if (plan0->N != R_dim0) {
        ks_throw_fmt(ks_type_InternalError, "`nx_T_fft_1d()` given `plan0 != NULL`, but `plan0->N != dim0` (%i != %i)", (int)plan0->N, (int)R_dim0);
        goto cleanup;
    }

    // Result buffer for a single 1D slice
    double complex* R = ks_malloc(sizeof(*R) * R_dim0);

    // Now, fill 'R" with numbers from 'A'
    // TODO: allow multi-dimensional input, and then loop over the last dimension
    if (!nx_T_cast(
        A, A_dtype, A_N, A_dim, A_stride,
        R, NX_DTYPE_CPLX_FP64, 1, &R_dim0, &R_stride
    )) goto cleanup;

    // perform the computation
    if (!nx_fft_plan_do(plan0, flags, R)) goto cleanup;

    // now, set 'B' to the value, casted
    if (!nx_T_cast(
        R, NX_DTYPE_CPLX_FP64, 1, &R_dim0, &R_stride,
        B, B_dtype, B_N, B_dim, B_stride
    )) goto cleanup;

    // success
    rstat = true;

    cleanup:

    // free temporary resources
    if (createdPlan0) { nx_fft_plan_free(plan0); ks_free(plan0); }
    if (R) ks_free(R);

    return rstat;
}








