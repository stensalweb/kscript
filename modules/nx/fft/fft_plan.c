/* fft/fft_plan.c - implementing various plan types for FFT
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
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// Shuffle 'A' according to bit-reversal of the index, i.e.:
// A'[i] = A[reverse_bits(i)]
// REQUIRES: N is a power of 2
static bool do_shfl_btrv(nx_size_t N, double complex* A) {
    // check if power of 2
    if (N & (N - 1)) {
        ks_throw_fmt(ks_type_SizeError, "Internally, `do_shfl_btrv` was called with non-power-of-two `N` (%z)", N);
        return false;
    }

    // loop indexes
    nx_size_t i, j = 0;

    for (i = 1; i < N; ++i) {
        nx_size_t b = N >> 1;
        while (j >= b) {
            j -= b;
            b >>= 1;
        }

        j += b;

        // swap if first matching
        if (j > i) {
            double complex t = A[i];
            A[i] = A[j];
            A[j] = t;
        }
    }

    // success
    return true;
}

// initialize an FFT plan
bool nx_fft_plan_init(nx_fft_plan_t* plan, nx_size_t N) {
    if ((N & (N - 1)) == 0) {
        // N is a power of two, so set up a CT plan
        plan->ptype = NX_FFT_PLAN_TYPE_CT;

        // size of transform
        plan->N = N;

        // fwd & inverse twiddle tables
        plan->CT.W_fwd = ks_malloc(sizeof(*plan->CT.W_fwd) * N);
        plan->CT.W_inv = ks_malloc(sizeof(*plan->CT.W_inv) * N);

        nx_size_t i;
        // fill them up
        for (i = 0; i < N; ++i) {
            plan->CT.W_fwd[i] = cexp(-2.0 * KS_M_PI * I * i / N);
            plan->CT.W_inv[i] = cexp(2.0 * KS_M_PI * I * i / N);
        }

        // success
        return true;
    } else {
        // not a power-of-two, must use Bluestein algorithm
        plan->ptype = NX_FFT_PLAN_TYPE_BLUE;

        // size of transform
        plan->N = N;

        // twiddle table, but of quadratically indexed roots of unity
        plan->blue.Ws_fwd = ks_malloc(sizeof(*plan->blue.Ws_fwd) * N);
        plan->blue.Ws_inv = ks_malloc(sizeof(*plan->blue.Ws_inv) * N);

        nx_size_t i;
        for (i = 0; i < N; ++i) {
            // to avoid round-off errors, perform modulo here
            nx_size_t j = (i * i) % (N * 2);

            // comute exp(-PI * i * j / N)
            plan->blue.Ws_fwd[i] = cexp(-KS_M_PI * I * j / N);
            plan->blue.Ws_inv[i] = cexp(KS_M_PI * I * j / N);
        }

        // now, find M, the smallest power of 2 >= 2*N+1
        nx_size_t M = 1;
        while (M < 2 * N + 1) {
            // out of index
            if (M > SIZE_MAX / 2) {
                ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_init` got N=%z, which was too large", N);
                ks_free(plan->blue.Ws_fwd);
                ks_free(plan->blue.Ws_inv);
                return false;
            }

            // next power of 2
            M *= 2;
        }

        // store M
        plan->blue.M = M;

        // create power-of-two-plan for M-sized transforms
        plan->blue.M_plan = ks_malloc(sizeof(*plan->blue.M_plan));
        if (!nx_fft_plan_init(plan->blue.M_plan, plan->blue.M)) {
            ks_free(plan->blue.M_plan);
            ks_free(plan->blue.Ws_fwd);
            ks_free(plan->blue.Ws_inv);
            return false;
        } else if (plan->blue.M_plan->ptype != NX_FFT_PLAN_TYPE_CT) {
            ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_init` tried to create CT subplan for blue plan, but it was not of type NX_FFT_PLAN_TYPE_CT!");
            nx_fft_plan_free(plan->blue.M_plan);
            ks_free(plan->blue.M_plan);
            ks_free(plan->blue.Ws_fwd);
            ks_free(plan->blue.Ws_inv);
            return false;
        }

        // allocate temporary buffer
        plan->blue.tmpbuf = ks_malloc(2 * M * sizeof(*plan->blue.tmpbuf));

        return true;
    }

}

// free resources used by that plan
void nx_fft_plan_free(nx_fft_plan_t* plan) {
    if (!plan) return;
    
    if (plan->ptype == NX_FFT_PLAN_TYPE_CT) {
        ks_free(plan->CT.W_fwd);
        ks_free(plan->CT.W_inv);
    } else if (plan->ptype == NX_FFT_PLAN_TYPE_BLUE) {
        ks_free(plan->blue.Ws_fwd);
        ks_free(plan->blue.Ws_inv);
        ks_free(plan->blue.tmpbuf);
        nx_fft_plan_free(plan->blue.M_plan);
        ks_free(plan->blue.M_plan);
    }

}


/* INTERNAL ROUTINES */


// Cooley-Tukey algorithm, which only accepts power-of-two inputs
static bool _fft_1d_CT(nx_fft_plan_t* plan, enum nx_fft_flags flags, double complex* A) {

    if (plan->ptype != NX_FFT_PLAN_TYPE_CT) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1d_CT` was called with `plan->ptype != NX_FFT_PLAN_TYPE_CT`");
        return false;
    }

    // whether or not it is inversed
    bool isInv = flags & NX_FFT_INVERSE;

    // size of transform
    nx_size_t N = plan->N;

    // ensure it is a power of two
    if (N & (N - 1)) {
        ks_throw_fmt(ks_type_SizeError, "`_fft_1d_CT` had a plan with a non-power-of-two `N`");
        return false;
    }

    // 1: shuffle acording to bit reversal of the index
    if (!do_shfl_btrv(N, A)) return false;

    // temporary vars
    double complex U, V;

    // TODO: select W_fwd/W_inv based on whether inverse FFT or not
    double complex* W = isInv ? plan->CT.W_inv : plan->CT.W_fwd;

    // loop vars
    nx_size_t i, j, k;

    // current sub-transform size
    nx_size_t m;

    // go through all the power-of-two sub-transform sizes
    for (m = 1; m <= N; m *= 2) {
        // half transform size
        nx_size_t m2 = m / 2;

        // how many steps in the twiddle table do we take?
        nx_size_t tbl_step = N / m;

        // inner-loop step
        nx_size_t jl_step = tbl_step * m;

        for (i = 0; i < m2; i++) {
            // W[i], current root of unity
            //double complex Wi = cexp(2 * KS_M_PI * I * i * );
            double complex Wi = W[i * N / m];

            // inner-most butterfly step
            for (j = i; j < jl_step + i; j += m) {
                U = A[j];
                V = A[j + m2] * Wi;

                A[j] = U + V;
                A[j + m2] = U - V;
            }
        }
    }

    // inverse transofrms are normalized by 1/N
    if (isInv) for (i = 0; i < N; ++i) A[i] = A[i] / N;

    // success
    return true;
}


// Bluestein algorithm
static bool _fft_1d_blue(nx_fft_plan_t* plan, enum nx_fft_flags flags, double complex* A) {
    if (plan->ptype != NX_FFT_PLAN_TYPE_BLUE) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1d_blue` was called with `plan->ptype != NX_FFT_PLAN_TYPE_BLUE`");
        return false;
    }

    // whether or not it is inversed
    bool isInv = flags & NX_FFT_INVERSE;

    // size of transform
    nx_size_t N = plan->N, M = plan->blue.M;

    double complex* Ws = isInv ? plan->blue.Ws_fwd : plan->blue.Ws_inv;

    // temporary buffer partitions
    double complex* tmpA = &plan->blue.tmpbuf[0];
    double complex* tmpB = &plan->blue.tmpbuf[plan->blue.M];

    /* PREPROCESS */

    // zero out tmps
    nx_size_t i;
    for (i = 0; i < M; ++i) tmpA[i] = tmpB[i] = 0;

    // preprocess by calculating temporary vectors
    for (i = 0; i < N; ++i) {
        tmpA[i] = A[i] * plan->blue.Ws_fwd[i];
    }

    tmpB[0] = plan->blue.Ws_fwd[0];
    for (i = 1; i < N; ++i) {
        tmpB[i] = tmpB[M - i] = conj(plan->blue.Ws_fwd[i]);
    }

    /* CONVOLVE(tmpA, tmpB) -> A */

    // compute in-place power-of-two FFT on tmpA and tmpB (of size M respectively)
    if (
        !_fft_1d_CT(plan->blue.M_plan, NX_FFT_NONE, tmpA) ||
        !_fft_1d_CT(plan->blue.M_plan, NX_FFT_NONE, tmpB)
    ) {
        return false;
    }

    // pointwise multiply
    for (i = 0; i < M; ++i) {
        tmpA[i] *= tmpB[i];
    }

    // now, do inverse on tmpA, to get convolution result
    if (!_fft_1d_CT(plan->blue.M_plan, NX_FFT_INVERSE, tmpA)) {
        return false;
    }

    /* POST PROCESS */

    // copy to output
    for (i = 0; i < N; ++i) {
        A[i] = tmpA[i] * plan->blue.Ws_fwd[i];
    }

    // inverse transforms are normalized by 1/N
    if (isInv) {
        A[0] = A[0] / N;
        // also, bluestein's algorithm requires elements [1+] are reversed in the inverse FFT
        for (i = 1; 2 * i < N; ++i) {
            double complex tmp = A[i];
            A[i] = A[N - i] / N;
            A[N - i] = tmp / N;
        }
    }



    // success
    return true;
}


// Perform an in-place FFT, i.e.:
// A' = FFT(A)
bool nx_fft_plan_do(nx_fft_plan_t* self, enum nx_fft_flags flags, double complex* A) {

    // execute plan based on type
    if (self->ptype == NX_FFT_PLAN_TYPE_CT) {
        return _fft_1d_CT(self, flags, A);
    } else if (self->ptype == NX_FFT_PLAN_TYPE_BLUE) {
        return _fft_1d_blue(self, flags, A);
    } else {
        ks_throw_fmt(ks_type_InternalError, "In `nx_fft_plan_do` Some other plan type was given, other than CT or BLUE");
        return false;
    }

}



