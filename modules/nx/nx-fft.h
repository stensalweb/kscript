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


// Compute: B = FFT(A[:])
// Conditions: A_N == B_N, B_dtype is complex
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_fft_1d(
    enum nx_fft_flags flags,
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
);

// Compute: B = IFFT(A[:])
// Conditions: A_N == B_N, B_dtype is complex
// NOTE: Returns whether it was successful or not, and if not, throw an error
KS_API bool nx_T_ifft_1d(
    enum nx_fft_flags flags,
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
);




#endif /* NX_FFT_H__ */

