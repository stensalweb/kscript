/* fft/fft_1d.c - implementation of the 1D FFT
 *
 * For power-of-two length sequences, standard Cooley-Tukey DIT algorithm is used
 * Otherwise, Bluestiein's algorithm (https://ccrma.stanford.edu/~jos/mdft/Bluestein_s_FFT_Algorithm.html) is used
 *
 * 
 * This code may not be maximal performance; in the future I want to optionally support using the FFTW backend, if
 *   kscript is compiled with it
 * 
 * 
 * Currently, twiddle tables are recomputed per invocation, but I would like to detect commonly used sizes & cache them
 * 
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// Compute B = FFT(A)
bool nx_T_fft_1d(
    nx_fft_plan_t* plan, enum nx_fft_flags flags,
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
) {

    // Result size & stride
    nx_size_t R_dim = A_dim[0], R_stride = 1;

    // if the given plan was NULL, we need to create a plan just for this invocation
    bool createdPlan = plan == NULL;

    if (createdPlan) {
        plan = ks_malloc(sizeof(*plan));
        if (!nx_fft_plan_init(plan, R_dim)) {
            ks_free(plan);
            return false;
        }
    }

    // Result buffer for a single 1D slice
    double complex* R = ks_malloc(sizeof(*R) * R_dim);

    // Now, fill 'R" with numbers from 'A'
    // TODO: allow multi-dimensional input, and then loop over the last dimension
    if (!nx_T_cast(
        A, A_dtype, A_N, A_dim, A_stride,
        R, NX_DTYPE_CPLX_FP64, 1, &R_dim, &R_stride
    )) {
        if (createdPlan) { nx_fft_plan_free(plan); ks_free(plan); }
        ks_free(R);
        return false;
    }


    // perform the computation
    if (!nx_fft_plan_do(plan, flags, R)) {
        if (createdPlan) { nx_fft_plan_free(plan); ks_free(plan); }
        ks_free(R);
        return false;
    }

    // now, set 'B' to the value, casted
    if (!nx_T_cast(
        R, NX_DTYPE_CPLX_FP64, 1, &R_dim, &R_stride,
        B, B_dtype, B_N, B_dim, B_stride
    )) {
        if (createdPlan) { nx_fft_plan_free(plan); ks_free(plan); }
        ks_free(R);
        return false;
    }

    // free temporary resources
    ks_free(R);
    if (createdPlan) { nx_fft_plan_free(plan); ks_free(plan); }

    return true;

}








