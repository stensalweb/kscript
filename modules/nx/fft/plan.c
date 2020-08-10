/* fft/plan.c - implement the `nx.fft.plan` type
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



// FFT plan cache, keyed on string:
// fmt("%i:%i:%i:%(%+z,)", (int)plan_type, isInv?1:0, rank, rank, dims)
static ks_dict fft_plan_cache = NULL;

/* INTERNAL ROUTINES */
// perform 1D FFT
static bool _fft_1D(nx_fft_plan plan, double complex* A);

// Shuffle 'A' according to bit-reversal of the index, i.e.:
// A'[i] = A[reverse_bits(i)]
// REQUIRES: N is a power of 2
static bool do_shfl_btrv(nx_size_t N, double complex* A) {
    // check if power of 2
    if (N & (N - 1)) {
        ks_throw(ks_T_SizeError, "Internally, `do_shfl_btrv` was called with non-power-of-two `N` (%z)", N);
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
static bool _fft_1D_BFLY(nx_fft_plan plan, double complex* A) {

    if (plan->ptype != NX_FFT_PLAN_1D_BFLY) {
        ks_throw(ks_T_InternalError, "`_fft_1D_BFLY` was called with `plan->ptype != NX_FFT_PLAN_1D_BFLY`");
        return false;
    }

    // size of transform
    nx_size_t N = plan->dim[0];

    // ensure it is a power of two
    if (N & (N - 1)) {
        ks_throw(ks_T_SizeError, "`_fft_1D_BFLY` had a plan with a non-power-of-two `N`");
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
// Since M ~= 3*N:
static bool _fft_1D_BLUE(nx_fft_plan plan, double complex* A) {
    if (plan->ptype != NX_FFT_PLAN_1D_BLUE) {
        ks_throw(ks_T_InternalError, "`_fft_1D_BLUE` was called with `plan->ptype != NX_FFT_PLAN_1D_BLUE`");
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
        !_fft_1D(plan->blue_1d.M_plan, tmpA) ||
        !_fft_1D(plan->blue_1d.M_plan, tmpB)
    ) {
        return false;
    }

    // pointwise multiply
    for (i = 0; i < M; ++i) {
        tmpA[i] *= tmpB[i];
    }

    // now, do inverse on tmpA, to get convolution result
    if (!_fft_1D(plan->blue_1d.M_plan, tmpA)) {
        return false;
    }

    // now, reverse tmpA[1:]

    // inverse transforms are normalized by 1/N
    if (plan->isInv) {

        double scl = 1.0f / (M * N);

        // copy to output
        A[0] = tmpA[0] * Ws[0] * scl;
        for (i = 1; i < N; ++i) {
            A[i] = -tmpA[i - N + M] * Ws[i] * scl;
        }

    } else {

        // copy to output
        A[0] = tmpA[0] * Ws[0] / M;
        for (i = 1; i < N; ++i) {
            A[i] = tmpA[M - i] * Ws[i] / M;
        }
    }

    // success
    return true;
}

// perform 1D FFT, in place
static bool _fft_1D(nx_fft_plan plan, double complex* A) {
    if (plan->ptype == NX_FFT_PLAN_1D_BFLY) {
        return _fft_1D_BFLY(plan, A);
    } else if (plan->ptype == NX_FFT_PLAN_1D_BLUE) {
        return _fft_1D_BLUE(plan, A);
    } else {
        ks_throw(ks_T_InternalError, "`_fft_1D` given plan->ptype==%i, which is unknown", plan->ptype);
        return false;
    }
}


/* exported funcs */

nx_fft_plan nx_fft_plan_create(enum nx_fft_plan_type plan_type, int rank, nx_size_t* dim, bool isInv) {

    // common
    nx_size_t i;

    if (plan_type == NX_FFT_PLAN_NONE) {
        // find default
        if (rank > 1) {
            #ifdef KS_HAVE_FFTW3
            plan_type = NX_FFT_PLAN_ND_FFTW3;
            #else
            plan_type = NX_FFT_PLAN_ND_DEFAULT;
            #endif
        } else {
            if ((dim[0] & (dim[0] - 1)) == 0) {
                plan_type = NX_FFT_PLAN_1D_BFLY;
            } else {
                plan_type = NX_FFT_PLAN_1D_BLUE;
            }
        }
    }

    // find cache key
    ks_str cache_key = ks_fmt_c("%i:%i:%i:(%+z,)", (int)plan_type, isInv ? 1 : 0, rank, rank, dim);

    nx_fft_plan cache_val = (nx_fft_plan)ks_dict_get_h(fft_plan_cache, (ks_obj)cache_key, cache_key->v_hash);
    if (!cache_val) {
        ks_catch_ignore();
    } else if (cache_val->type != nx_T_fft_plan) {
        KS_DECREF(cache_val);
    } else {
        return cache_val;
    }

    if (plan_type == NX_FFT_PLAN_ND_FFTW3) {
        #ifdef KS_HAVE_FFTW3

        // prefer FFT if the type is not specified
        nx_fft_plan self = KS_ALLOC_OBJ(nx_fft_plan);
        KS_INIT_OBJ(self, nx_T_fft_plan);
        self->ptype = plan_type;

        self->isInv = isInv;

        // now, set dimensions up
        self->rank = rank;
        self->dim = ks_malloc(sizeof(*self->dim) * rank);
        for (i = 0; i < rank; ++i) self->dim[i] = dim[i];

        // init to NULL so the type.__free__ doesn't free random addresses
        self->fftw3_nd.plan = NULL;
        self->fftw3_nd.tmp = NULL;
        self->fftw3_nd.stride = NULL;

        // convert to integer sizes, so FFTW knows what they are
        int* f_sizes = ks_malloc(sizeof(*f_sizes) * rank);
        for (i = 0; i < rank; ++i) f_sizes[i] = dim[i];

        // calculate the total size of a block of array needed
        nx_size_t total_sz = sizeof(double complex);
        for (i = 0; i < rank; ++i) total_sz *= dim[i];

        // create a temporary, dense array of complex numbers for FFTW to operate on
        self->fftw3_nd.tmp = ks_malloc(total_sz);

        // construct FFTW3 plan
        self->fftw3_nd.plan = fftw_plan_many_dft(rank, f_sizes, 1, 
            self->fftw3_nd.tmp, NULL, 1, 0, 
            self->fftw3_nd.tmp, NULL, 1, 0,
            isInv ? FFTW_FORWARD : FFTW_BACKWARD, 
            FFTW_ESTIMATE
        );

        // we don't need them any more
        ks_free(f_sizes);

        // ensure it was constructed correctly
        if (!self->fftw3_nd.plan) {
            KS_DECREF(self);
            KS_DECREF(cache_key);
            return ks_throw(ks_T_InternalError, "`fft_plan_many_dft()` failed; could not create FFTW3 plan");
        }


        // create stride array
        // (even though the stride is trivial, having it computed makes it easier for other nx routines)
        self->fftw3_nd.stride = ks_malloc(sizeof(*self->fftw3_nd.stride) * self->rank);

        // dense strides, row major
        self->fftw3_nd.stride[self->rank - 1] = sizeof(double complex);
        for (i = self->rank - 2; i >= 0; --i) self->fftw3_nd.stride[i] = self->fftw3_nd.stride[i + 1] * self->dim[i + 1];
        
        ks_dict_set_h(fft_plan_cache, (ks_obj)cache_key, cache_key->v_hash, (ks_obj)self);
        KS_DECREF(cache_key);

        return self;

        #else

        KS_DECREF(cache_key);
        return ks_throw(ks_T_InternalError, "FFT plan type NX_FFT_PLAN_ND_FFTW3 requested, but was not compiled with FFTW3");

        #endif
    } else if (plan_type == NX_FFT_PLAN_ND_DEFAULT) {
        nx_fft_plan self = KS_ALLOC_OBJ(nx_fft_plan);
        KS_INIT_OBJ(self, nx_T_fft_plan);
        self->ptype = plan_type;

        self->isInv = isInv;

        // now, set dimensions up
        self->rank = rank;
        self->dim = ks_malloc(sizeof(*self->dim) * rank);
        for (i = 0; i < rank; ++i) self->dim[i] = dim[i];


        // calculate 1D base plans
        self->default_nd.plan_dims = ks_malloc(sizeof(*self->default_nd.plan_dims) * rank);
        for (i = 0; i < rank; ++i) self->default_nd.plan_dims[i] = NULL;

        // recursively create plans for each axis
        for (i = 0; i < rank; ++i) {
            if (!(self->default_nd.plan_dims[i] = nx_fft_plan_create(NX_FFT_PLAN_NONE, 1, &self->dim[i], isInv))) {
                KS_DECREF(cache_key);
                KS_DECREF(self);
                return NULL;
            }
        }
        
        ks_dict_set_h(fft_plan_cache, (ks_obj)cache_key, cache_key->v_hash, (ks_obj)self);
        KS_DECREF(cache_key);
        
        return self;
    } else if (plan_type == NX_FFT_PLAN_1D_BFLY) {

        if (rank != 1) {
            KS_DECREF(cache_key);
            return ks_throw(ks_T_SizeError, "FFT plan type NX_FFT_PLAN_1D_BFLY requested, but rank (%i) was not 1", rank);
        }

        // N = transform length
        nx_size_t N = dim[0];

        // N is a power of 2, so set up Cooley-Tukey/Butterfly plan
        if ((N & (N - 1)) != 0) {
            KS_DECREF(cache_key);
            return ks_throw(ks_T_SizeError, "FFT plan type NX_FFT_PLAN_1D_BFLY requested, but size N (%z) was not a power of 2", N);
        }

        nx_fft_plan self = KS_ALLOC_OBJ(nx_fft_plan);
        KS_INIT_OBJ(self, nx_T_fft_plan);
        self->ptype = plan_type;

        self->isInv = isInv;

        // now, set dimensions up
        self->rank = rank;
        self->dim = ks_malloc(sizeof(*self->dim) * rank);
        for (i = 0; i < rank; ++i) self->dim[i] = dim[i];

        // fwd & inverse twiddle tables
        self->bfly_1d.W = ks_malloc(sizeof(*self->bfly_1d.W) * N);

        // fill them up
        for (i = 0; i < N; ++i) {
            self->bfly_1d.W[i] = cexp(-2.0 * KS_M_PI * I * i / N);
            if (isInv) self->bfly_1d.W[i] = 1.0 / self->bfly_1d.W[i];
        }
        
        ks_dict_set_h(fft_plan_cache, (ks_obj)cache_key, cache_key->v_hash, (ks_obj)self);
        KS_DECREF(cache_key);

        // success
        return self;
    } else if (plan_type == NX_FFT_PLAN_1D_BLUE) {
        if (rank != 1) {
            KS_DECREF(cache_key);
            return ks_throw(ks_T_SizeError, "FFT plan type NX_FFT_PLAN_1D_BLUE requested, but rank (%i) was not 1", rank);
        }

        // N = transform length
        nx_size_t N = dim[0];

        nx_fft_plan self = KS_ALLOC_OBJ(nx_fft_plan);
        KS_INIT_OBJ(self, nx_T_fft_plan);
        self->ptype = plan_type;

        self->isInv = isInv;

        // now, set dimensions up
        self->rank = rank;
        self->dim = ks_malloc(sizeof(*self->dim) * rank);
        for (i = 0; i < rank; ++i) self->dim[i] = dim[i];

        self->blue_1d.M_plan = NULL;
        self->blue_1d.tmpbuf = NULL;

        // twiddle table, but of quadratically indexed roots of unity
        self->blue_1d.Ws = ks_malloc(sizeof(*self->blue_1d.Ws) * N);

        for (i = 0; i < N; ++i) {
            // to avoid round-off errors, perform modulo here
            nx_size_t j = (i * i) % (N * 2);

            // comute exp(-PI * i * j / N)
            // NOTE: We don't need to change based on fwd/inv transform,
            //   because we can simply discriminate in the internal method
            //   (since we have to reverse parts of the array anyway)
            self->blue_1d.Ws[i] = cexp(-KS_M_PI * I * j / N);
        }


        // now, find M, the smallest power of 2 >= 2*N+1
        nx_size_t M = 1;
        while (M < 2 * N + 1) {
            // out of index
            if (M > SIZE_MAX / 2) {
                KS_DECREF(self);
                return ks_throw(ks_T_InternalError, "`nx_fft_plan_create` got N=%z, which was too large", N);
            }

            // next power of 2
            M *= 2;
        }

        // store M
        self->blue_1d.M = M;

        // create power-of-two-plan for M-sized transforms
        if (!(self->blue_1d.M_plan = nx_fft_plan_create(NX_FFT_PLAN_NONE, 1, (nx_size_t[]){ self->blue_1d.M }, false))) {
            KS_DECREF(cache_key);
            KS_DECREF(self);
            return NULL;
        }

        // allocate temporary buffer
        self->blue_1d.tmpbuf = ks_malloc(2 * M * sizeof(*self->blue_1d.tmpbuf));

        ks_dict_set_h(fft_plan_cache, (ks_obj)cache_key, cache_key->v_hash, (ks_obj)self);
        KS_DECREF(cache_key);

        return self;
    } else {
        KS_DECREF(cache_key);
        return ks_throw(ks_T_InternalError, "Could not create plan given plan_type==%i", (int)plan_type);
    }

}



// data structure for 1D transform
struct my_fft_data {

    // input, output
    nxar_t A, B;

    // 1D transform plan size
    nx_fft_plan plan;

    // loop index stride
    nx_size_t* Astride;
    nx_size_t* Bstride;

    // stride for the selected axis
    nx_size_t Aas, Bas;

    // a single vector to be transformed, of size max(plan->dims[:])
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

    // copt to 'R'
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


    // perform 1D FFT on thes slice
    if (!_fft_1D(data->plan, data->tmp)) return false;

    // copy to 'B'
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

    // success
    return true;
}


#ifdef KS_HAVE_FFTW3


struct my_fft_data_fftw3 {

    // the FFTW3 plan to use
    nx_fft_plan plan;

    // A & B inputs
    nxar_t A, B;

    // stride of `loop_N`-D chunks of A & B
    nx_size_t* Astride;
    nx_size_t* Bstride;

    // stride of `plan->rank`-D chunks of A & B to be FFT'd
    nx_size_t* Astride_fft;
    nx_size_t* Bstride_fft;

    // result/temporary data
    nxar_t R;

    // scaling factor (for inverse transforms only)
    double scale_inv;

};

// inner loop to execute plan->rank dimensional FFTs
static bool my_fft_loop_fftw3(int loop_N, nx_size_t* loop_dim, nx_size_t* idx, void* _user_data) {
    struct my_fft_data_fftw3* data = (struct my_fft_data_fftw3*)_user_data;

    if (data->plan->ptype != NX_FFT_PLAN_ND_FFTW3) {
        ks_throw(ks_T_InternalError, "`my_fft_loop_fftw3()` got data->plan->ptype != NX_FFT_PLAN_ND_FFTW3");
        return false;
    }

    // get data pointers to the slices
    void* dptr_A = nx_get_ptr(data->A.data, loop_N, loop_dim, data->Astride, idx);
    void* dptr_B = nx_get_ptr(data->B.data, loop_N, loop_dim, data->Bstride, idx);

    // copt to 'R'
    if (!nx_T_cast(
        (nxar_t) {
            .data = dptr_A,
            .dtype = data->A.dtype,
            .rank = data->plan->rank,
            .dim = data->plan->dim,
            .stride = data->Astride_fft,
        },
        data->R
    )) return false;

    // perform Nd FFT from fftw (we have already set the data) 
    fftw_execute(data->plan->fftw3_nd.plan);

    // perform normalization
    if (data->plan->isInv) {
        if (!nx_T_mul(
            data->R,
            (nxar_t){
                .data = (void*)&data->scale_inv,
                .dtype = nx_dtype_fp64,
                .rank = 1,
                .dim = (nx_size_t[]){ 1 },
                .stride = (nx_size_t[]){ 0 },
            },
            data->R
        )) {
            return false;
        }
    }

    // copy to 'B'
    if (!nx_T_cast(
        data->R,
        (nxar_t) {
            .data = dptr_B,
            .dtype = data->B.dtype,
            .rank = data->plan->rank,
            .dim = data->plan->dim,
            .stride = data->Bstride_fft,
        }
    )) return false;

    // success
    return true;
}

#endif

// perform FFT
bool nx_fft_plan_do(nx_fft_plan plan, nxar_t A, nxar_t B, int* axes) {

    // loop var
    int i;

    if (A.rank != B.rank) {
        ks_throw(ks_T_SizeError, "While doing FFT, A.rank != B.rank (%i != %i)", A.rank, B.rank);
        return false;
    }

    for (i = 0; i < A.rank; ++i) {
        if (A.dim[i] != B.dim[i]) {
            ks_throw(ks_T_SizeError, "While doing FFT, shape mismatch, A.shape != B.shape ((%+z,) != (%+z,))", A.rank, A.dim, B.rank, B.dim);
            return false;
        }
    }

    for (i = 0; i < plan->rank; ++i) {
        int axis = ((axes[i] % A.rank) + A.rank) % A.rank;

        if (A.dim[axis] != plan->dim[i]) {
            ks_throw(ks_T_SizeError, "Dimension mismatch between FFT plan, and axes; (A.dim[axes[%i]] == A.dim[%i] == %z) != (plan->dim[%i] == %z)", i, axis, A.dim[axis], i, plan->dim[i]);
            return false;
        }
        if (B.dim[axis] != plan->dim[i]) {
            ks_throw(ks_T_SizeError, "Dimension mismatch between FFT plan, and axes; (B.dim[axes[%i]] == B.dim[%i] == %z) != (plan->dim[%i] == %z)", i, axis, B.dim[axis], i, plan->dim[i]);
            return false;
        }
    }

    if (plan->ptype == NX_FFT_PLAN_ND_FFTW3) {
        #ifdef KS_HAVE_FFTW3
        // compute FFTW3 transforms

        // we loop over the axes not used by the plan
        int loop_N = A.rank - plan->rank;
        int fft_N = plan->rank;

        // loop sizes
        nx_size_t* dim = ks_malloc(sizeof(*dim) * loop_N);
        nx_size_t* Astride = ks_malloc(sizeof(*Astride) * loop_N);
        nx_size_t* Bstride = ks_malloc(sizeof(*Bstride) * loop_N);

        nx_size_t* Astride_fft = ks_malloc(sizeof(*Astride_fft) * fft_N);
        nx_size_t* Bstride_fft = ks_malloc(sizeof(*Bstride_fft) * fft_N);

        // calculate maximum dimension
        nx_size_t max_dim = plan->dim[0];
        for (i = 1; i < plan->rank; ++i) if (plan->dim[i] > max_dim) max_dim = plan->dim[i];

        double scale_inv = 1.0;
        for (i = 0; i < plan->rank; ++i) scale_inv *= plan->dim[i];
        scale_inv = 1.0 / scale_inv;

        // index into loop & FFT dims
        int li = 0, fi = 0;

        // now, set up the dimensions of the loop
        for (i = 0; i < A.rank; ++i) {

            // figure out whether `i` is a requested axis
            bool isAxis = false;

            int j;
            for (j = 0; j < plan->rank; ++j) {
                if (i == axes[j]) {
                    isAxis = true;
                    break;
                }
            }

            if (!isAxis) {
                // not an FFT axis, so must be a loop axis

                dim[li] = A.dim[i];
                Astride[li] = A.stride[i];
                Bstride[li] = B.stride[i];

                // claim index for loop
                li++;
            } else {
                Astride_fft[fi] = A.stride[i];
                Bstride_fft[fi] = B.stride[i];

                // claim index fo FFT
                fi++;
            }

        }

        struct my_fft_data_fftw3 data;

        data.plan = plan;

        data.A = A;
        data.B = B;

        data.Astride = Astride;
        data.Bstride = Bstride;

        data.Astride_fft = Astride_fft;
        data.Bstride_fft = Bstride_fft;

        data.scale_inv = scale_inv;

        data.R = (nxar_t){
            .data = (void*)plan->fftw3_nd.tmp,
            .dtype = nx_dtype_cplx_fp64,
            .rank = plan->rank,
            .dim = plan->dim,
            .stride = plan->fftw3_nd.stride
        };


        // apply loop
        bool rst = nx_T_apply_loop(loop_N, dim, my_fft_loop_fftw3, (void*)&data);

        ks_free(dim);
        ks_free(Astride);
        ks_free(Bstride);
        ks_free(Astride_fft);
        ks_free(Bstride_fft);

        return rst;

        #else

        ks_throw(ks_type_InternalError, "nx_fft_plan_t had ptype==NX_FFT_PLAN_ND_FFTW3, but it was not compiled with FFTW3 support!");
        return false;
        #endif


    } else {
        // apply our own custom FFT plans
        // do loop FFT; perform transform over every axis

        // dims & strides
        int loop_N = A.rank - 1;
        nx_size_t* dim = ks_malloc(sizeof(*dim) * loop_N);
        nx_size_t* Astride = ks_malloc(sizeof(*Astride) * loop_N);
        nx_size_t* Bstride = ks_malloc(sizeof(*Bstride) * loop_N);

        // calculate maximum dimension
        nx_size_t max_dim = plan->dim[0];
        for (i = 1; i < plan->rank; ++i) if (plan->dim[i] > max_dim) max_dim = plan->dim[i];

        // 1D buffer for the transform
        double complex* tmp = ks_malloc(sizeof(*tmp) * max_dim);

        // return status
        bool rstat = false;

        // now, do an FFT along every requested axis
        for (i = 0; i < plan->rank; ++i) {
            int axis = axes[i];
            axis = ((axis % A.rank) + A.rank) % A.rank;

            // set loop dimensions
            int j, lj = 0;
            for (j = 0; j < A.rank; ++j) {
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

            // set all strides
            data.Astride = Astride;
            data.Bstride = Bstride;
            data.Aas = A.stride[axis];
            data.Bas = B.stride[axis];

            // temporary buffer
            data.tmp = tmp;

            // attempt to apply the 1D fft in a loop over the other axes
            if (!nx_T_apply_loop(loop_N, dim, my_fft_loop, (void*)&data)) {
                rstat = false;
                goto end_plan_do;
            }

            // data has been copied and FFTd once, so now refer to B for the input
            A = B;
        }

        end_plan_do:

        ks_free(dim);
        ks_free(Astride);
        ks_free(Bstride);
        ks_free(tmp);

        return true;
    }
}



// plan.__free__(self)
static KS_TFUNC(plan, free) {
    nx_fft_plan self;
    KS_GETARGS("self:*", &self, nx_T_fft_plan)

    if (self->ptype == NX_FFT_PLAN_ND_FFTW3) {
        #ifdef KS_HAVE_FFTW3

        fftw_destroy_plan(self->fftw3_nd.plan);
        ks_free(self->fftw3_nd.tmp);
        ks_free(self->fftw3_nd.stride);

        #endif

    } else if (self->ptype == NX_FFT_PLAN_1D_BFLY) {
        ks_free(self->bfly_1d.W);
    } else if (self->ptype == NX_FFT_PLAN_1D_BLUE) {
        ks_free(self->blue_1d.Ws);
        ks_free(self->blue_1d.tmpbuf);
        KS_DECREF(self->blue_1d.M_plan);
    } else if (self->ptype == NX_FFT_PLAN_ND_DEFAULT) {
        int i;
        for (i = 0; i < self->rank; ++i) {
            KS_DECREF(self->default_nd.plan_dims[i]);
        }
        ks_free(self->default_nd.plan_dims);
    }
    ks_free(self->dim);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// plan.__str__(self)
static KS_TFUNC(plan, str) {
    nx_fft_plan self;
    KS_GETARGS("self:*", &self, nx_T_fft_plan)
    
    enum nx_fft_plan_type pt = self->ptype;

    return (ks_obj)ks_fmt_c("nx.fft.Plan(rank=%i, dim=(%+z), ptype='%s')", self->rank, self->rank, self->dim, 
        pt == NX_FFT_PLAN_NONE ? "NONE" : 
        pt == NX_FFT_PLAN_ND_DEFAULT ? "ND_DEFAULT" : 
        pt == NX_FFT_PLAN_ND_FFTW3 ? "ND_FFTW3" : 
        pt == NX_FFT_PLAN_1D_DENSE ? "1D_DENSE" : 
        pt == NX_FFT_PLAN_1D_BFLY ? "1D_BFLY" : 
        pt == NX_FFT_PLAN_1D_BLUE ? "1D_BLUE" :
        "UNKNOWN"
    );
}


// nx.fft.plan type
KS_TYPE_DECLFWD(nx_T_fft_plan);

// initialize the type
void nx_T_init_fft_plan() {

    fft_plan_cache = ks_dict_new(0, NULL);

    ks_type_init_c(nx_T_fft_plan, "nx.fft.Plan", ks_T_object, KS_KEYVALS(

        {"__free__", (ks_obj)ks_cfunc_new_c(plan_free_, "nx.array.__free__(self)")},
        {"__str__", (ks_obj)ks_cfunc_new_c(plan_str_, "nx.array.__str__(self)")},
        {"__repr__", (ks_obj)ks_cfunc_new_c(plan_str_, "nx.array.__repr__(self)")},

        {"_plan_cache", (ks_obj)fft_plan_cache},

    ));
}

