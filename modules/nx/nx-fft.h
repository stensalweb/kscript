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



// nx_fft_plan_t - C-style FFT plan for a given size
typedef struct nx_fft_plan_s {

    // the number of dimensios of the transform (i.e. 1 for 1D transform)
    int Nd;

    // the actual sizes of the transform (there should be Nd of them)
    nx_size_t* dim;

    // enumeration of FFT flags describe a transform
    enum nx_fft_flags {

        NX_FFT_NONE           = 0x00,

        // if this is set, do the inverse transform rather than the forward transform
        NX_FFT_INVERSE        = 0x01,

    } flags;

    // enumeration of different plan types
    enum nx_fft_plan_type {

        // none/error type
        NX_FFT_PLAN_NONE       = 0,

        // N-dimensional data plan
        NX_FFT_PLAN_ND         = 1,

        /* 1 DIMENSIONAL */

        // Cooley-Tukey, only valid for N==power-of-two
        NX_FFT_PLAN_1D_CT      = 2,

        // Bluestein algo, which works for any size, but isn't as optimized
        NX_FFT_PLAN_1D_BLUE    = 3,

    } ptype;


    union {

        // used when Nd>0
        struct {

            // array of FFT plans for each dimension, i.e. `sub_plans[0]` is for size `dim[0]`, and so on
            struct nx_fft_plan_s** sub_plans;

        } ND;

        // Cooley-Tukey algorithm data
        struct {

            // roots of unity computed for fwd & inv transforms
            // W_fwd[j] = exp(-2 * PI * i * j / N)
            // W_inv[j] = 1.0 / W_fwd[j]
            // NOTE: if either is 'NULL', then it hasn't been calculated yet
            double complex* W;

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
            double complex* Ws;

            // temporary buffer, of 2*M elements
            // These are used for the convolution step,
            // it is partitioned like [tmp.A tmp.B]
            // i.e. A=&tmpbuf[0], B=&tmpbuf[M]
            double complex* tmpbuf;

            // Cooley-Tukey FFT plan for size 'M'
            // 'M' is defined as a power-of-two, so this should always have type `NX_FFT_PLAN_1D_CT`
            struct nx_fft_plan_s* M_plan_fwd, *M_plan_inv;

        } blue;
    };

} nx_fft_plan_t;



// Create a plan for Nd (dim...) sized FFT
// NOTE: Returns a pointer to the plan, or NULL and throws an error
KS_API nx_fft_plan_t* nx_fft_plan_create(enum nx_fft_flags flags, int Nd, nx_size_t* dim);

// Free the FFT plan created
KS_API void nx_fft_plan_free(nx_fft_plan_t* self);

// Perform a 1D in-place FFT, i.e.:
// A' = FFT(A)
// Where 'A' is the length of the plan that it was created with (i.e. self->N)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_fft_plan_do(nx_fft_plan_t* self, double complex* A);

// Perform a 1D in-place FFT, i.e.:
// A' = FFT(A)
// Where 'A' is the length of the plan that it was created with (i.e. self->N)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_fft_plan_do_1Dip(nx_fft_plan_t* self, double complex* A);


/* HIGHER LEVEL API FUNCTIONS */


// Compute: B = FFT(A[:]) (with given flags)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_1d(nx_fft_plan_t* plan0, int axis0, nxar_t A, nxar_t B);

// Compute: B = FFT(A[:]) (with given flags)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_2d(nx_fft_plan_t* plan0, int axis0, int axis1, nxar_t A, nxar_t B);

// Compute B = FFT(A), on given axes
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_Nd(nx_fft_plan_t* plan0, int N, int* axis, nxar_t A, nxar_t B);


#endif /* NX_FFT_H__ */

