/* fft/fft_2d.c - implementation of the 2D FFT
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

/*

// Compute B = FFT(A)
bool nx_T_fft_2d(
    enum nx_fft_flags flags,
    nx_fft_plan_t* plan0, nx_fft_plan_t* plan1, 
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
) {
    // checks
    if (A_N != 2 || B_N != 2) {
        ks_throw_fmt(ks_type_ToDoError, "nx_T_fft_2d not implemented for other-than-2 dimension tensors");
        return false;
    }

    // what axis is the FFT computed over
    int axis_0 = A_N - 2, axis_1 = A_N - 1;

    // calculate sizes of each type
    nx_size_t sizeof_A = nx_dtype_size(A_dtype), sizeof_B = nx_dtype_size(B_dtype);

    //  Get result dimensions
    nx_size_t R_dim0 = A_dim[axis_0], R_dim1 = A_dim[axis_1];
    
    // stride==1, since it is just a small array for a single row/column result
    nx_size_t R_stride = 1;

    // maximum size
    nx_size_t R_dim_max = R_dim0 > R_dim1 ? R_dim0 : R_dim1;

    // if the given plan was NULL, we need to create a plan just for this invocation
    bool createdPlan0 = false, createdPlan1 = false;
    
    // result buffer
    double complex* R = NULL;

    // result
    bool rstat = false;
    if (createdPlan0 = plan0 == NULL) {
        plan0 = ks_malloc(sizeof(*plan0));
        if (!nx_fft_plan_init(plan0, R_dim0)) {
            ks_free(plan0);
            plan0 = NULL;
            goto cleanup;
        }
    } else if (plan0->N != R_dim0) {
        ks_throw_fmt(ks_type_InternalError, "`nx_T_fft_2d()` given `plan0 != NULL`, but `plan0->N != dim0` (%i != %i)", (int)plan0->N, (int)R_dim0);
        goto cleanup;
    }

    if (createdPlan1 = plan1 == NULL) {
        plan1 = ks_malloc(sizeof(*plan1));
        if (!nx_fft_plan_init(plan1, R_dim1)) {
            ks_free(plan1);
            plan1 = NULL;
            goto cleanup;
        }
    } else if (plan1->N != R_dim1) {
        ks_throw_fmt(ks_type_InternalError, "`nx_T_fft_2d()` given `plan1 != NULL`, but `plan1->N != dim1` (%i != %i)", (int)plan1->N, (int)R_dim1);
        goto cleanup;
    }

    // Result buffer for a single 1D slice
    // (used for both horizontal & vertical stripes, so must contain the max)
    R = ks_malloc(sizeof(*R) * R_dim_max);

    // loov ars
    nx_size_t i;

    // current row & column pointers
    intptr_t cur_row_A, cur_row_B;
    intptr_t cur_col_B;

    // first, compute row-wise FFT:
    // B[i, :] = FFT(A[i, :])
    for (i = 0, cur_row_A = (intptr_t)A, cur_row_B = (intptr_t)B; i < A_dim[0]; ++i, cur_row_A += sizeof_A * A_stride[0], cur_row_B += sizeof_B * B_stride[0]) {
        // Now, fill 'R' with the current row of 'A'
        if (!nx_T_cast(
            (void*)cur_row_A, A_dtype, 1, (nx_size_t[]){ A_dim[1] }, (nx_size_t[]){ A_stride[1] },
            R, NX_DTYPE_CPLX_FP64, 1, &R_dim1, &R_stride
        )) goto cleanup;

        // perform the computation on each row
        if (!nx_fft_plan_do(plan0, flags, R)) goto cleanup;

        // Now, set B[i, :] to R
        if (!nx_T_cast(
            R, NX_DTYPE_CPLX_FP64, 1, &R_dim1, &R_stride,
            (void*)cur_row_B, B_dtype, 1, (nx_size_t[]){ B_dim[1] }, (nx_size_t[]){ B_stride[1] }
        )) goto cleanup;
    }

    // now, compute column-wise FFT on the row-wise FFT:
    // B[:, i] = FFT(B[:, i])
    for (i = 0, cur_col_B = (intptr_t)B; i < A_dim[1]; ++i, cur_col_B += sizeof_B * B_stride[1]) {
        // Now, fill 'R' with the current col of 'B'
        if (!nx_T_cast(
            (void*)cur_col_B, B_dtype, 1, (nx_size_t[]){ B_dim[0] }, (nx_size_t[]){ B_stride[0] },
            R, NX_DTYPE_CPLX_FP64, 1, &R_dim0, &R_stride
        )) goto cleanup;

        // perform the computation on each column
        if (!nx_fft_plan_do(plan1, flags, R)) goto cleanup;
        
        // Now, set B[:, i] to R
        if (!nx_T_cast(
            R, NX_DTYPE_CPLX_FP64, 1, &R_dim0, &R_stride,
            (void*)cur_col_B, B_dtype, 1, (nx_size_t[]){ B_dim[0] }, (nx_size_t[]){ B_stride[0] }
        )) goto cleanup;
    }

    // success, keep on to cleanup
    rstat = true;

    cleanup:

    // free temporary resources
    if (createdPlan0) { nx_fft_plan_free(plan0); ks_free(plan0); }
    if (createdPlan1) { nx_fft_plan_free(plan1); ks_free(plan1); }
    if (R) ks_free(R);

    return rstat;
}

*/






