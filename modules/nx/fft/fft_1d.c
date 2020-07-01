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


// internal plan specifier for an FFT
// Unlike most 'plans' this datastructure
struct nx_fft_plan_t {

    // enum describing what type of plan it is
    enum {

        // none/error type
        NX_FFT_PLAN_TYPE_NONE    = 0,

        // Cooley-Tukey, only valid for N==power-of-two
        NX_FFT_PLAN_TYPE_CT      = 1,

        // Bluestein algo, 
        NX_FFT_PLAN_TYPE_BLUE    = 2,


    } ptype;


    // Cooley-Tukey algorithm
    struct {

        // size of transform (which is a power-of-two)
        nx_size_t N;

        // roots of unity computed for fwd & inv transforms
        double complex* W_fwd;
        double complex* W_inv;

    } CT;

    // Bluestein algorithm
    struct {

        // size of transform (can be any size)
        nx_size_t N;

        // Convolution length such that M >= 2 * N + 1, and M is a power-of-two
        nx_size_t M;

        // Cooley-Tukey FFT plan for size 'M'
        struct nx_fft_plan_t* M_CT;

        // roots of unity of squared indexes:
        // Ws_fwd[j] = exp(-PI * i * j^2 / N)
        // is of size 'N'
        double complex* Ws_fwd;
        double complex* Ws_inv;

        // temporary buffer, of 2M elements
        // These are used for the convolution step,
        // it is partitioned like [tmp.A tmp.B]
        double complex* tmpbuf;


    } blue;


    bool hasBluestein;


};


// calculate floor(log_2(N))
static int calc_log2(nx_size_t N) {
    int r = 0;
    while (N > 0) { N /= 2; r++; }
    return r;
}

// calculate the next power of 2 >= N
static nx_size_t calc_nextpow2(nx_size_t N) {
    nx_size_t r = 1;
    while (N > 0) { N /= 2; r *= 2; }
    return r;
}


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

static void nx_fft_plan_free(struct nx_fft_plan_t* plan);

// initialize an FFT plan
static bool nx_fft_plan_init(struct nx_fft_plan_t* plan, nx_size_t N) {
    if ((N & (N - 1)) == 0) {
        // N is a power of two, so set up a CT plan
        plan->ptype = NX_FFT_PLAN_TYPE_CT;

        // size of transform
        plan->CT.N = N;

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
        plan->blue.N = N;

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
        plan->blue.M_CT = ks_malloc(sizeof(*plan->blue.M_CT));
        if (!nx_fft_plan_init(plan->blue.M_CT, plan->blue.M)) {
            ks_free(plan->blue.M_CT);
            ks_free(plan->blue.Ws_fwd);
            ks_free(plan->blue.Ws_inv);
            return false;
        } else if (plan->blue.M_CT->ptype != NX_FFT_PLAN_TYPE_CT) {
            ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_init` tried to create CT subplan for blue plan, but it was not of type NX_FFT_PLAN_TYPE_CT!");
            nx_fft_plan_free(plan->blue.M_CT);
            ks_free(plan->blue.M_CT);
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
static void nx_fft_plan_free(struct nx_fft_plan_t* plan) {
    if (plan->ptype == NX_FFT_PLAN_TYPE_CT) {
        ks_free(plan->CT.W_fwd);
        ks_free(plan->CT.W_inv);
    } else if (plan->ptype == NX_FFT_PLAN_TYPE_BLUE) {
        ks_free(plan->blue.Ws_fwd);
        ks_free(plan->blue.Ws_inv);
        ks_free(plan->blue.tmpbuf);
        nx_fft_plan_free(plan->blue.M_CT);
        ks_free(plan->blue.M_CT);
    }

}


// Cooley-Tukey algorithm, which only accepts power-of-two inputs
static bool _fft_1d_CT(struct nx_fft_plan_t* plan, enum nx_fft_flags flags, double complex* A) {

    if (plan->ptype != NX_FFT_PLAN_TYPE_CT) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1d_CT` was called with `plan->ptype != NX_FFT_PLAN_TYPE_CT`");
        return false;
    }

    // whether or not it is inversed
    bool isInv = flags & NX_FFT_INVERSE;

    // size of transform
    nx_size_t N = plan->CT.N;

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

    // success
    return true;
}


// Bluestein algorithm
static bool _fft_1d_blue(struct nx_fft_plan_t* plan, enum nx_fft_flags flags, double complex* A) {
    if (plan->ptype != NX_FFT_PLAN_TYPE_BLUE) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1d_blue` was called with `plan->ptype != NX_FFT_PLAN_TYPE_BLUE`");
        return false;
    }

    // whether or not it is inversed
    bool isInv = flags & NX_FFT_INVERSE;

    // size of transform
    nx_size_t N = plan->blue.N, M = plan->blue.M;

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
        !_fft_1d_CT(plan->blue.M_CT, NX_FFT_NONE, tmpA) ||
        !_fft_1d_CT(plan->blue.M_CT, NX_FFT_NONE, tmpB)
    ) {
        return false;
    }

    // pointwise multiply
    for (i = 0; i < M; ++i) {
        tmpA[i] *= tmpB[i];
    }

    // now, do inverse on tmpA, to get convolution result
    if (!_fft_1d_CT(plan->blue.M_CT, NX_FFT_INVERSE, tmpA)) {
        return false;
    }

    // scale back
    for (i = 0; i < M; ++i) {
        tmpA[i] = tmpA[i] / M;
    }

    /* POST PROCESS */

    // copy to output
    for (i = 0; i < N; ++i) {
        A[i] = tmpA[i] * plan->blue.Ws_fwd[i];
    }

    // success
    return true;
}




// Compute B = FFT(A)
bool nx_T_fft_1d(
    enum nx_fft_flags flags,
    void* A, enum nx_dtype A_dtype, int A_N, nx_size_t* A_dim, nx_size_t* A_stride, 
    void* B, enum nx_dtype B_dtype, int B_N, nx_size_t* B_dim, nx_size_t* B_stride
) {

    // TODO: support the case where B has stride==1 & type==NX_DTYPE_CPLX_FP64

    // Result size    
    nx_size_t R_dim = A_dim[0], R_stride = 1;

    // Result buffer
    double complex* R = ks_malloc(sizeof(*R) * R_dim);

    // Now, fill 'R" with numbers
    if (!nx_T_cast(
        A, A_dtype, A_N, A_dim, A_stride,
        R, NX_DTYPE_CPLX_FP64, 1, &R_dim, &R_stride
    )) {
        ks_free(R);
        return false;
    }

    // create a plan
    struct nx_fft_plan_t plan;
    if (!nx_fft_plan_init(&plan, R_dim)) {
        ks_free(R);
        return false;
    }

    bool stat = false;

    // execute plan
    if (plan.ptype == NX_FFT_PLAN_TYPE_CT) {
        stat = _fft_1d_CT(&plan, flags, R);
    } else if (plan.ptype == NX_FFT_PLAN_TYPE_BLUE) {
        stat = _fft_1d_blue(&plan, flags, R);
    } else {
        ks_throw_fmt(ks_type_InternalError, "Some other plan type was given, other than CT or BLUE");
    }

    // check for errors
    if (!stat) {
        ks_free(R);
        nx_fft_plan_free(&plan);
        return false;
    }

    // now, set 'B'
    if (!nx_T_cast(
        R, NX_DTYPE_CPLX_FP64, 1, &R_dim, &R_stride,
        B, B_dtype, B_N, B_dim, B_stride
    )) {

        ks_free(R);
        nx_fft_plan_free(&plan);
        return false;
    }

    // free temporary resources
    ks_free(R);
    nx_fft_plan_free(&plan);

    return true;

}








