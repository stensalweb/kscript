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
static bool _fft_1D_BFLY(nx_fft_plan_t plan, double complex* A) {

    if (plan->ptype != NX_FFT_PLAN_1D_BFLY) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1D_BFLY` was called with `plan->ptype != NX_FFT_PLAN_1D_BFLY`");
        return false;
    }

    // size of transform
    nx_size_t N = plan->dim[0];

    // ensure it is a power of two
    if (N & (N - 1)) {
        ks_throw_fmt(ks_type_SizeError, "`_fft_1D_BFLY` had a plan with a non-power-of-two `N`");
        return false;
    }

    // 1: shuffle acording to bit reversal of the index
    if (!do_shfl_btrv(N, A)) return false;

    // temporary vars
    double complex U, V;

    double complex* W = plan->bfly_1d.W;

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
    if (plan->isInv) for (i = 0; i < N; ++i) A[i] = A[i] / N;

    // success
    return true;
}


// Bluestein algorithm
static bool _fft_1D_BLUE(nx_fft_plan_t plan, double complex* A) {
    if (plan->ptype != NX_FFT_PLAN_1D_BLUE) {
        ks_throw_fmt(ks_type_InternalError, "`_fft_1D_BLUE` was called with `plan->ptype != NX_FFT_PLAN_1D_BLUE`");
        return false;
    }

    // size of transform
    nx_size_t N = plan->dim[0], M = plan->blue_1d.M;

    // temporary buffer partitions
    double complex* tmpA = &plan->blue_1d.tmpbuf[0];
    double complex* tmpB = &plan->blue_1d.tmpbuf[plan->blue_1d.M];

    double complex* Ws = plan->blue_1d.Ws;

    /* PREPROCESS */

    // zero out tmps
    nx_size_t i;
    for (i = 0; i < M; ++i) tmpA[i] = tmpB[i] = 0;

    // preprocess by calculating temporary vectors
    for (i = 0; i < N; ++i) {
        tmpA[i] = A[i] * Ws[i];
    }

    tmpB[0] = Ws[0];
    for (i = 1; i < N; ++i) {
        tmpB[i] = tmpB[M - i] = conj(Ws[i]);
    }

    /* CONVOLVE(tmpA, tmpB) -> A */

    // compute in-place power-of-two FFT on tmpA and tmpB (of size M respectively)
    if (
        !_fft_1D_BFLY(plan->blue_1d.M_plan, tmpA) ||
        !_fft_1D_BFLY(plan->blue_1d.M_plan, tmpB)
    ) {
        return false;
    }

    // pointwise multiply
    for (i = 0; i < M; ++i) {
        tmpA[i] *= tmpB[i];
    }

    // now, do inverse on tmpA, to get convolution result
    if (!_fft_1D_BFLY(plan->blue_1d.M_plan, tmpA)) {
        return false;
    }

    // now, reverse tmpA[1:]

    //for (i = 1; i < M; ++)


    /* POST PROCESS */

    // copy to output
    for (i = 0; i < N; ++i) {
        A[i] = tmpA[i] * Ws[i];
    }

    if (plan->isInv) for (i = 0; i < N; ++i) A[i] = A[i] / N;

    // inverse transforms are normalized by 1/N
    /*if (isInv) {
        A[0] = A[0] / N;
        // also, bluestein's algorithm requires elements [1+] are reversed in the inverse FFT
        for (i = 1; 2 * i < N; ++i) {
            double complex tmp = A[i];
            A[i] = A[N - i] / N;
            A[N - i] = tmp / N;
        }
    }*/

    // success
    return true;
}




/* exported funcs */

nx_fft_plan_t nx_fft_plan_create(enum nx_fft_plan_type plan_type, int rank, nx_size_t* dim, bool isInv) {
    nx_fft_plan_t self = ks_malloc(sizeof(*self));

    self->ptype = plan_type;
    self->rank = rank;
    self->dim = ks_malloc(sizeof(*self->dim) * rank);
    self->isInv = isInv;

    nx_size_t i;
    for (i = 0; i < rank; ++i) self->dim[i] = dim[i];

    if (rank == 1) {

        // 1D transform
        nx_size_t dim0 = dim[0];

        if ((dim0 & (dim0 - 1)) == 0) {
            // dim0 is a power of 2, so set up CT plan
            self->ptype = NX_FFT_PLAN_1D_BFLY;

            // fwd & inverse twiddle tables
            self->bfly_1d.W = ks_malloc(sizeof(*self->bfly_1d.W) * dim0);
            //plan->CT.W_inv = ks_malloc(sizeof(*plan->CT.W_inv) * N);

            nx_size_t i;
            // fill them up
            for (i = 0; i < dim0; ++i) {
                self->bfly_1d.W[i] = cexp(-2.0 * KS_M_PI * I * i / dim0);
                if (isInv) self->bfly_1d.W[i] = 1.0 / self->bfly_1d.W[i];
            }
            
            // success
            return self;

        } else {
            // we need to use Bluestein's algorithm
            // not a power-of-two, must use Bluestein algorithm
            self->ptype = NX_FFT_PLAN_1D_BLUE;

            // twiddle table, but of quadratically indexed roots of unity
            self->blue_1d.Ws = ks_malloc(sizeof(*self->blue_1d.Ws) * dim0);

            for (i = 0; i < dim0; ++i) {
                // to avoid round-off errors, perform modulo here
                nx_size_t j = (i * i) % (dim0 * 2);

                // comute exp(-PI * i * j / N)
                self->blue_1d.Ws[i] = cexp(-KS_M_PI * I * j / dim0);
                //if (flags & NX_FFT_INVERSE) self->blue.Ws[i] = 1.0 / self->blue.Ws[i];
            }

            // now, find M, the smallest power of 2 >= 2*N+1
            nx_size_t M = 1;
            while (M < 2 * dim0 + 1) {
                // out of index
                if (M > SIZE_MAX / 2) {
                    ks_throw_fmt(ks_type_InternalError, "`nx_fft_plan_create` got dim0=%z, which was too large", dim0);
                    ks_free(self->blue_1d.Ws);
                    ks_free(self);
                    return NULL;
                }

                // next power of 2
                M *= 2;
            }

            // store M
            self->blue_1d.M = M;

            // create power-of-two-plan for M-sized transforms
            self->blue_1d.M_plan = nx_fft_plan_create(NX_FFT_PLAN_NONE, 1, (nx_size_t[]){ self->blue_1d.M }, false);

            // allocate temporary buffer
            self->blue_1d.tmpbuf = ks_malloc(2 * M * sizeof(*self->blue_1d.tmpbuf));

            return self;
        }

    } else {
        self->ptype = NX_FFT_PLAN_ND_DEFAULT;

        // calculate 1D base plans
        self->default_nd.plan_dims = ks_malloc(sizeof(*self->default_nd.plan_dims) * rank);
    
        for (i = 0; i < rank; ++i) {
            self->default_nd.plan_dims[i] = nx_fft_plan_create(NX_FFT_PLAN_NONE, 1, &self->dim[i], isInv);
        }

        //self->rank.sub_plan = nx_fft_plan_create(flags, Nd - 1, &self->dim[1]);
        //self->rank.dim0_plan = nx_fft_plan_create(flags, 1, &self->dim[0]);

        return self;
    }
}


// free resources used by that plan
void nx_fft_plan_free(nx_fft_plan_t plan) {
    if (!plan) return;
   /* 
    if (plan->ptype == NX_FFT_PLAN_1D_CT) {
        ks_free(plan->CT.W);
    } else if (plan->ptype == NX_FFT_PLAN_1D_BLUE) {
        ks_free(plan->blue.Ws);
        ks_free(plan->blue.tmpbuf);
        nx_fft_plan_free(plan->blue.M_plan_fwd);
        nx_fft_plan_free(plan->blue.M_plan_inv);
    }*/
    ks_free(plan);
}


// data structure for 1D transform
struct my_fft_data {

    // input, output
    nxar_t A, B;

    // 1D transform plan size
    nx_fft_plan_t plan;

    // loop index stride
    nx_size_t* Astride;
    nx_size_t* Bstride;

    // stride for the selected axis
    nx_size_t Aas, Bas;

    // a single vector to be transformed, of size max(plan->dims)
    double complex* tmp;

};

static bool my_fft_loop(int loop_N, nx_size_t* loop_dim, nx_size_t* idx, void* _user_data) {
    struct my_fft_data* data = (struct my_fft_data*)_user_data;


    // A start
    intptr_t A_s = (intptr_t)nx_get_ptr(data->A.data, loop_N, loop_dim, data->Astride, idx);
    intptr_t B_s = (intptr_t)nx_get_ptr(data->B.data, loop_N, loop_dim, data->Bstride, idx);

    // result
    nxar_t R = (nxar_t) {
        .data = (void*)data->tmp,
        .dtype = nx_dtype_cplx_fp64,
        .rank = 1,
        .dim = (nx_size_t[]){ data->plan->dim[0] },
        .stride = (nx_size_t[]){ nx_dtype_cplx_fp64->size },
    };

    if (!nx_T_cast(
        (nxar_t) {
            .data = (void*)A_s,
            .dtype = data->A.dtype,
            .rank = 1,
            .dim = (nx_size_t[]){ data->plan->dim[0] },
            .stride = (nx_size_t[]){ data->Aas },
        },
        R
    )) return false;

    // perform 1D FFt
    if (data->plan->ptype == NX_FFT_PLAN_1D_BFLY) {
        if (!_fft_1D_BFLY(data->plan, data->tmp)) return false;
    } else if (data->plan->ptype == NX_FFT_PLAN_1D_BLUE) {
        if (!_fft_1D_BLUE(data->plan, data->tmp)) return false;
    } else {
        ks_throw_fmt(ks_type_InternalError, "Unknown switch in my_fft_loop");
        return false;
    }

    if (!nx_T_cast(
        R,
        (nxar_t) {
            .data = (void*)B_s,
            .dtype = data->B.dtype,
            .rank = 1,
            .dim = (nx_size_t[]){ data->plan->dim[0] },
            .stride = (nx_size_t[]){ data->Bas },
        }
    )) return false;

    return true;

}

// perform FFT
bool nx_fft_plan_do(nx_fft_plan_t plan, nxar_t A, nxar_t B, int* axes) {


    // do loop FFT; perform transform over every axis

    // dims & strides
    int loop_N = plan->rank - 1;
    nx_size_t* dim = ks_malloc(sizeof(*dim) * loop_N);
    nx_size_t* Astride = ks_malloc(sizeof(*Astride) * loop_N);
    nx_size_t* Bstride = ks_malloc(sizeof(*Bstride) * loop_N);

    nx_size_t max_dim = plan->dim[0];
    int i;
    for (i = 1; i < plan->rank; ++i) if (plan->dim[i] > max_dim) max_dim = plan->dim[i];

    double complex* tmp = ks_malloc(sizeof(*tmp) * max_dim);

    for (i = 0; i < plan->rank; ++i) {
        // perform over axes[i]
        int axis = axes[i];
        axis = ((axis % A.rank) + A.rank) % A.rank;

        // set loop dimensions
        int j, lj = 0;
        for (j = 0; j < plan->rank; ++j) {
            if (j != axis) {
                // add to loop dimensions
                dim[lj] = A.dim[j];
                Astride[lj] = A.stride[j];
                Bstride[lj] = B.stride[j];
                lj++;
            }
        }

        // construct data
        struct my_fft_data data;

        data.A = A;
        data.B = B;
        if (plan->ptype == NX_FFT_PLAN_ND_DEFAULT) {
            data.plan = plan->default_nd.plan_dims[i];
        } else {
            data.plan = plan;
        }

        data.Astride = Astride;
        data.Bstride = Bstride;
        data.Aas = A.stride[axis];
        data.Bas = B.stride[axis];

        data.tmp = tmp;

        if (!nx_T_apply_loop(loop_N, dim, my_fft_loop, (void*)&data)) {
            return false;
        }
    }

    return true;

}

/*
// data for FFT
struct my_fft_outer_data {

    // plan for the FFT
    nx_fft_plan_t* plan;

    // entire data array
    double complex* A;

    // the stride (which should imply the array is be dense)
    nx_size_t* stride;

    // outer dimension
    nx_size_t outer_dim;

    // temporary storage
    double complex* tmp;

};


// internal function to perform an FFT over the outer dimension of the FFT
// i.e. will perform 1D FFT over every slice and combination of lower dimensions
static bool my_fft_outer(int loop_N, nx_size_t* dim, nx_size_t* idx, void* _user_data) {

    struct my_fft_outer_data* data = (struct my_fft_outer_data*)_user_data;

    // get pointer to start
    //double complex* A_start = data->A + nx_szsdot(loop_N, 1, dim, &data->stride[1], idx);
    double complex* A_start = nx_get_ptr(data->A, sizeof(*A_start), loop_N, dim, &data->stride[1], idx);


    int i;

    // get into contiguous memory
    for (i = 0; i < data->outer_dim; ++i) {
        data->tmp[i] = A_start[i * data->stride[0]];
    }

    // perform 1D FFT
    if (!nx_fft_plan_do_1Dip(data->plan, data->tmp)) return false;

    // store back into normal mem
    for (i = 0; i < data->outer_dim; ++i) {
        A_start[i * data->stride[0]] = data->tmp[i];
    }

    // success
    return true;
}


// Perform an ND in-place FFT, i.e.:
// A' = FFT(A)
bool nx_fft_plan_do(nx_fft_plan_t* self, double complex* A) {

    if (self->rank == 1) {
        // do one dimensional
        return nx_fft_plan_do_1Dip(self, A);

    } else {

        // stride is the product of all lower slices
        nx_size_t slice_stride = 1;
        int i;

        for (i = 1; i < self->rank; ++i) {
            slice_stride *= self->dim[i];
        }

        // perform (N-1)d FFTs on the slices
        for (i = 0; i < self->dim[0]; ++i) {
            // do sub slices
            if (!nx_fft_plan_do(self->rank.sub_plan, A + slice_stride * i)) return false;
        }

        nx_size_t* stride = ks_malloc(sizeof(*stride) * self->rank);
        stride[self->rank - 1] = 1;
        for (i = self->rank - 2; i >= 0; i--) {
            stride[i] = stride[i + 1] * self->dim[i + 1];
        }

        // loop dimensions, since we are looping over 1D sections
        int loop_N = self->rank - 1;

        struct my_fft_outer_data data;

        data.plan = self->rank.dim0_plan;
        data.A = A;
        data.stride = stride;

        // compute size & temporary buffer for a single 1D iteration through the outer dimension
        data.outer_dim = self->dim[0];
        data.tmp = ks_malloc(sizeof(double complex) * data.outer_dim);

        // now, apply FFT on outer loops
        if (!nx_T_apply_loop(self->rank - 1, &self->dim[1], my_fft_outer, (void*)&data)) {
            ks_free(data.tmp);
            ks_free(stride);
            return false;
        }

        //ks_free(data.tmp);
        //ks_free(all_strides);
        return true;
    }
}


*/