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


// enumerates the different types of plans
// used in the discriminated union
enum nx_fft_plan_type {

    // none/error type
    NX_FFT_PLAN_NONE           =  0,

    // NdD FFT plan, which (recursively) uses:
    // an (rank-1)D FFT over dims[1:], and then a 1D over dim[0]
    NX_FFT_PLAN_ND_DEFAULT     =  1,

    /* 1 DIMENSIONAL SPECIAL CASES */

    // 1D Dense plan, which is a matrix-vector product between the matrix of roots of unity
    //   and the input vector
    // Works for any size (including large primes), but 
    // O(N^2), but can be fast for very small sizes (<10)
    NX_FFT_PLAN_1D_DENSE       = 10,

    // 1D Bluestein algorithm (i.e. Chirp-Z Transform)
    // Works for any size (including large primes)
    // NOTE: This is only valid when rank==1
    // O(N*log(N)), but has a larger constant factor due to 
    NX_FFT_PLAN_1D_BLUE        = 11,

    // 1D Cooley-Tukey butterfly algorithm
    // O(N*log(N))
    // NOTE: This is only valid when rank==1 && dims[0]==2**n
    NX_FFT_PLAN_1D_BFLY        = 12,

};


// nx_fft_plan_t - C-style FFT plan for a given size
typedef struct nx_fft_plan_s* nx_fft_plan_t;

// nx_fft_plan_t - C-style FFT plan for a given size
struct nx_fft_plan_s {

    // number of dimensions of the transform
    int rank;

    // dimensions of the transform 
    // NOTE: for images, it is (h, w), or (h, w, dep)
    nx_size_t* dim;

    // whether or not the transform is an 'inverse' transform
    bool isInv;

    // enumeration of different plan types
    enum nx_fft_plan_type ptype;

    // discriminated union, check `ptype`
    union {

        // rankD plan, when ptype==NX_FFT_PLAN_ND_DEFAULT
        struct {

            // 1D plans for each dimension
            nx_fft_plan_t* plan_dims;

        } default_nd;

        // 1D dense matrix-vector product, when ptype==NX_FFT_PLAN_1D_DENSE
        // computes the matrix product:
        // FFT(X) = Wx, where W[j,k] = exp(-2*pi*i*j*k/N)
        //   (or, for inverse, its reciprocal)
        struct {

            // (dim[0], dim[0]) matrix W
            double complex* W;

        } dense_1d;


        // 1D butterfly transform, based on Cooley-Tukey algorithm, when ptype==NX_FFT_PLAN_1D_BFLY
        // TODO: perhaps also precompute a lookup table for bit-reversal indices, another O(N) cost,
        //   but saves O(N*log(N)) operation on each invocation, but also this means it can't be done
        //   in place
        struct {

            // size (dim[0],) vector of powers of the root of unity 'w',
            // where 'w = exp(-2*pi*i*j*k/N)'
            // so W[j] = w^j
            //   (or, for inverse, its reciprocal)
            double complex* W;

        } bfly_1d;


        // 1D Bluestein's algorithm when ptype==NX_FFT_PLAN_1D_BLUE
        struct {

            // Internal convolution length, chosen such that M >= 2 * N + 1, and M=2**n
            nx_size_t M;

            // FFT plan for (M,) size
            // Internally, Bluestein's algorithm uses a convolution of length 2**n, so
            //   we just compute the plan once, and observe that:
            // CONVOLVE(x, y) = IFFT(FFT(x)*FFT(y))
            // And, that IFFT(x) = (FFT(x)[0], FFT(x)[1:][::-1]) / N,
            // so we can just apply the forward plan twice and adjust
            nx_fft_plan_t M_plan;

            // Roots of unity to squared indices power, i.e.:
            // Ws[j] = exp(-PI * i * j^2 / N)
            //   (or, for inverse, its reciprocal)
            double complex* Ws;

            // temporary buffer, of size 2*M, for tA & tB, temporary arrays
            // it is partitioned like [tA tB]
            // i.e. tA=&tmpbuf[0], tB=&tmpbuf[M]
            double complex* tmpbuf;


        } blue_1d;
    };

};



// Create an N-d FFT plan (of a given rank, and dimensions), of type 'plan_type'
// NOTE: You can pass `NX_FFT_PLAN_TYPE_NONE` to auto-detect and create the best plan for your given dimensions
// NOTE: Returns NULL if there was an error
KS_API nx_fft_plan_t nx_fft_plan_create(enum nx_fft_plan_type plan_type, int rank, nx_size_t* dim, bool isInv);

// Free resources allocated by a given FFT plan
KS_API void nx_fft_plan_free(nx_fft_plan_t self);

// Compute B = A, on axes, where `axes` are `plan->rank` number of axes
// NOTE: Returns success, or false and throws error
KS_API bool nx_fft_plan_do(nx_fft_plan_t plan, nxar_t A, nxar_t B, int* axes);

/*
// Perform a 1D in-place FFT, i.e.:
// A' = FFT(A)
// Where 'A' is the length of the plan that it was created with (i.e. self->rank)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_fft_plan_do(nx_fft_plan_t* self, nxar_t A);

// Compute: B = FFT(A[:]) (with given flags)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_1d(nx_fft_plan_t* plan0, int axis0, nxar_t A, nxar_t B);

// Compute: B = FFT(A[:]) (with given flags)
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_2d(nx_fft_plan_t* plan0, int axis0, int axis1, nxar_t A, nxar_t B);

// Compute B = FFT(A), on given axes
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_Nd(nx_fft_plan_t* plan0, int N, int* axis, nxar_t A, nxar_t B);
*/

#endif /* NX_FFT_H__ */

