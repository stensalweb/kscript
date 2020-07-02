/* modules/nx/nx-fft.h - Fast-Fourier-Transform related functions for the `nx` package
 *
 * The DFT is defined as:
 * 
 * DFT(A)[j] 1/N * sum(A[k] * exp(2*PI*i * (j * k) / N))
 * 
 * And the IDFT is defined as:
 * 
 * IDFT(DFT(A)) == A, given the same flags
 * 
 * 
 * The scaling factor by default is 1/N, 1
 * 
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef NX_FFT_H__
#define NX_FFT_H__

// include the main `nx` header
#include <nx.h>


// enumeration of FFT flags that are passed to internal routines
enum nx_fft_flags {

    NX_FFT_NONE           = 0x00,

    // if this is set, do the inverse transform rather than the forward transform
    NX_FFT_INVERSE        = 0x01,

};


// nx_fft_plan_t - C-style FFT plan for a given size
typedef struct nx_fft_plan_s {

    // enum describing what type of plan it is
    enum {

        // none/error type
        NX_FFT_PLAN_TYPE_NONE    = 0,

        // Cooley-Tukey, only valid for N==power-of-two
        NX_FFT_PLAN_TYPE_CT      = 1,

        // Bluestein algo, which works for any size, but isn't as optimized
        NX_FFT_PLAN_TYPE_BLUE    = 2,

    } ptype;

    // size of transform (can be any size)
    nx_size_t N;

    union {

        // Cooley-Tukey algorithm data
        struct {

            // roots of unity computed for fwd & inv transforms
            // W_fwd[j] = exp(-2 * PI * i * j / N)
            // W_inv[j] = 1.0 / W_fwd[j]
            // NOTE: if either is 'NULL', then it hasn't been calculated yet
            double complex* W_fwd;
            double complex* W_inv;

        } CT;

        // Bluestein algorithm data
        struct {

            // Convolution length such that M >= 2 * N + 1, and M is a power-of-two
            nx_size_t M;


            // roots of unity of squared indexes:
            // Ws_fwd[j] = exp(-PI * i * j^2 / N)
            // Ws_inv[j] = 1.0 / Ws_fwd[j]
            // is of size 'N'
            // NOTE: if either is 'NULL', then it hasn't been calculated yet
            double complex* Ws_fwd;
            double complex* Ws_inv;

            // temporary buffer, of 2*M elements
            // These are used for the convolution step,
            // it is partitioned like [tmp.A tmp.B]
            // i.e. A=&tmpbuf[0], B=&tmpbuf[M]
            double complex* tmpbuf;

            // Cooley-Tukey FFT plan for size 'M'
            // 'M' is defined as a power-of-two, so this should always have type `NX_FFT_PLAN_TYPE_CT`
            struct nx_fft_plan_s* M_plan;

        } blue;
    };

} nx_fft_plan_t;


// Compute the optimal plan for a size-N transform
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_fft_plan_init(nx_fft_plan_t* self, nx_size_t N);

// Perform an in-place FFT, i.e.:
// A' = FFT(A)
// Where 'A' is the length of the plan that it was created with (i.e. self->N)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_fft_plan_do(nx_fft_plan_t* self, enum nx_fft_flags flags, double complex* A);

// Frees resources used by the plan
KS_API void nx_fft_plan_free(nx_fft_plan_t* self);


/* HIGHER LEVEL API FUNCTIONS */


// Compute: B = FFT(A[:]) (with given flags)
// If flags contains `NX_FFT_INVERSE`, the computation is an inverse FFT
// Conditions: A_N == B_N, B_dtype is complex
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_1d(
    enum nx_fft_flags flags,
    nx_fft_plan_t* plan0, 
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
);

// Compute: B = FFT(A[:]) (with given flags)
// If flags contains `NX_FFT_INVERSE`, the computation is an inverse FFT
// Conditions: A_N == B_N, B_dtype is complex
// NOTE: Returns whether it was successful or not, and if not, throw an error
bool nx_T_fft_2d(
    enum nx_fft_flags flags,
    nx_fft_plan_t* plan0, nx_fft_plan_t* plan1, 
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
);


#endif /* NX_FFT_H__ */

