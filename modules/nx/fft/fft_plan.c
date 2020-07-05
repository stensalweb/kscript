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






/* INTERNAL ROUTINES */


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

// Cooley-Tukey algorithm, which only accepts power-of-two inputs
static bool _fft_1d_CT(nx_fft_plan_t* plan, double complex* A) {

    if (plan->ptype != NX_FFT_PLAN_1D_CT) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1d_CT` was called with `plan->ptype != NX_FFT_PLAN_TYPE_CT`");
        return false;
    }

    // whether or not it is inversed
    bool isInv = plan->flags & NX_FFT_INVERSE;

    // size of transform
    nx_size_t N = plan->dim[0];

    // ensure it is a power of two
    if (N & (N - 1)) {
        ks_throw_fmt(ks_type_SizeError, "`_fft_1d_CT` had a plan with a non-power-of-two `N`");
        return false;
    }

    // 1: shuffle acording to bit reversal of the index
    if (!do_shfl_btrv(N, A)) return false;

    // temporary vars
    double complex U, V;

    double complex* W = plan->CT.W;

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
static bool _fft_1d_blue(nx_fft_plan_t* plan, double complex* A) {
    if (plan->ptype != NX_FFT_PLAN_1D_BLUE) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1d_blue` was called with `plan->ptype != NX_FFT_PLAN_TYPE_BLUE`");
        return false;
    }

    // whether or not it is inversed
    bool isInv = plan->flags & NX_FFT_INVERSE;

    // size of transform
    nx_size_t N = plan->dim[0], M = plan->blue.M;

    // temporary buffer partitions
    double complex* tmpA = &plan->blue.tmpbuf[0];
    double complex* tmpB = &plan->blue.tmpbuf[plan->blue.M];

    /* PREPROCESS */

    // zero out tmps
    nx_size_t i;
    for (i = 0; i < M; ++i) tmpA[i] = tmpB[i] = 0;

    // preprocess by calculating temporary vectors
    for (i = 0; i < N; ++i) {
        tmpA[i] = A[i] * plan->blue.Ws[i];
    }

    tmpB[0] = plan->blue.Ws[0];
    for (i = 1; i < N; ++i) {
        tmpB[i] = tmpB[M - i] = conj(plan->blue.Ws[i]);
    }

    /* CONVOLVE(tmpA, tmpB) -> A */

    // compute in-place power-of-two FFT on tmpA and tmpB (of size M respectively)
    if (
        !_fft_1d_CT(plan->blue.M_plan_fwd, tmpA) ||
        !_fft_1d_CT(plan->blue.M_plan_fwd, tmpB)
    ) {
        return false;
    }

    // pointwise multiply
    for (i = 0; i < M; ++i) {
        tmpA[i] *= tmpB[i];
    }

    // now, do inverse on tmpA, to get convolution result
    if (!_fft_1d_CT(plan->blue.M_plan_inv, tmpA)) {
        return false;
    }

    /* POST PROCESS */

    // copy to output
    for (i = 0; i < N; ++i) {
        A[i] = tmpA[i] * plan->blue.Ws[i];
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





/* exported funcs */

nx_fft_plan_t* nx_fft_plan_create(enum nx_fft_flags flags, int Nd, nx_size_t* dim) {
    nx_fft_plan_t* self = ks_malloc(sizeof(*self));

    self->Nd = Nd;
    self->dim = ks_malloc(sizeof(self->dim) * Nd);
    self->flags = flags;

    nx_size_t i;
    for (i = 0; i < Nd; ++i) self->dim[i] = dim[i];

    if (Nd == 1) {

        // 1D transform
        nx_size_t dim0 = dim[0];

        if ((dim0 & (dim0 - 1)) == 0) {
            // dim0 is a power of 2, so set up CT plan
            self->ptype = NX_FFT_PLAN_1D_CT;

            // fwd & inverse twiddle tables
            self->CT.W = ks_malloc(sizeof(*self->CT.W) * dim0);
            //plan->CT.W_inv = ks_malloc(sizeof(*plan->CT.W_inv) * N);

            nx_size_t i;
            // fill them up
            for (i = 0; i < dim0; ++i) {
                self->CT.W[i] = cexp(-2.0 * KS_M_PI * I * i / dim0);
                if (flags & NX_FFT_INVERSE) self->CT.W[i] = 1.0 / self->CT.W[i];
            }
            
            // success
            return self;

        } else {
            // we need to use Bluestein's algorithm
            // not a power-of-two, must use Bluestein algorithm
            self->ptype = NX_FFT_PLAN_1D_BLUE;

            // twiddle table, but of quadratically indexed roots of unity
            self->blue.Ws = ks_malloc(sizeof(*self->blue.Ws) * dim0);

            for (i = 0; i < dim0; ++i) {
                // to avoid round-off errors, perform modulo here
                nx_size_t j = (i * i) % (dim0 * 2);

                // comute exp(-PI * i * j / N)
                self->blue.Ws[i] = cexp(-KS_M_PI * I * j / dim0);
                if (flags & NX_FFT_INVERSE) self->blue.Ws[i] = 1.0 / self->blue.Ws[i];
            }

            // now, find M, the smallest power of 2 >= 2*N+1
            nx_size_t M = 1;
            while (M < 2 * dim0 + 1) {
                // out of index
                if (M > SIZE_MAX / 2) {
                    ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_create` got dim0=%z, which was too large", dim0);
                    ks_free(self->blue.Ws);
                    ks_free(self);
                    return NULL;
                }

                // next power of 2
                M *= 2;
            }

            // store M
            self->blue.M = M;

            // create power-of-two-plan for M-sized transforms
            self->blue.M_plan_fwd = nx_fft_plan_create(flags, 1, (nx_size_t[]){ self->blue.M });

            if (self->blue.M_plan_fwd->ptype != NX_FFT_PLAN_1D_CT) {
                ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_create` tried to create CT subplan for blue plan, but it was not of type NX_FFT_PLAN_1D_CT!");
                nx_fft_plan_free(self->blue.M_plan_inv);
                ks_free(self->blue.Ws);
                ks_free(self);
                return NULL;
            }

            bool isInv = flags & NX_FFT_INVERSE;

            self->blue.M_plan_inv = nx_fft_plan_create(isInv ? NX_FFT_NONE : NX_FFT_INVERSE, 1, (nx_size_t[]){ self->blue.M });

            if (self->blue.M_plan_inv->ptype != NX_FFT_PLAN_1D_CT) {
                ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_create` tried to create CT subplan for blue plan, but it was not of type NX_FFT_PLAN_1D_CT!");
                nx_fft_plan_free(self->blue.M_plan_fwd);
                nx_fft_plan_free(self->blue.M_plan_inv);
                ks_free(self->blue.Ws);
                ks_free(self);
                return NULL;
            }


            // allocate temporary buffer
            self->blue.tmpbuf = ks_malloc(2 * M * sizeof(*self->blue.tmpbuf));

            return self;
        }

    } else {

        return ks_throw_fmt(ks_type_ToDoError, "Need to implement multidim FFT plans");
    }
}


// free resources used by that plan
void nx_fft_plan_free(nx_fft_plan_t* plan) {
    if (!plan) return;
    
    if (plan->ptype == NX_FFT_PLAN_1D_CT) {
        ks_free(plan->CT.W);
    } else if (plan->ptype == NX_FFT_PLAN_1D_BLUE) {
        ks_free(plan->blue.Ws);
        ks_free(plan->blue.tmpbuf);
        nx_fft_plan_free(plan->blue.M_plan_fwd);
        nx_fft_plan_free(plan->blue.M_plan_inv);
    }
    ks_free(plan);
}


// Perform an 1D in-place FFT, i.e.:
// A' = FFT(A)
bool nx_fft_plan_do_1Dip(nx_fft_plan_t* self, double complex* A) {

    // execute plan based on type
    if (self->ptype == NX_FFT_PLAN_1D_CT) {
        return _fft_1d_CT(self, A);
    } else if (self->ptype == NX_FFT_PLAN_1D_BLUE) {
        return _fft_1d_blue(self, A);
    } else {
        ks_throw_fmt(ks_type_InternalError, "In `nx_fft_plan_do_1Dip` Some other plan type was given, other than CT or BLUE for 1D");
        return false;
    }

}


