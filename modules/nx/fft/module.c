/* fft/module.c - `nx.fft` module
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

#define SUBMOD "fft"



// nx.fft.fft1d(A, B=none, axis=-1)
static KS_TFUNC(fft, fft1d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 3);
    ks_obj aA, aB = KSO_NONE;
    int64_t axis0 = -1;
    if (!ks_parse_params(n_args, args, "A%any ?B%any ?axis%i64", &aA, &aB, &axis0)) return NULL;

    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB != KSO_NONE && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = NX_DTYPE_CPLX_FP64,
            .N = Ar.N,
            .dim = Ar.dim,
            .stride = NULL
        });

        // set nxar
        Br = NXAR_ARRAY(newB);
        ks_list_push(dels, (ks_obj)newB);
        to_ret = KS_NEWREF(newB);
    } else {
        to_ret = KS_NEWREF(aB);
    }

    axis0 = ((axis0 % Ar.N) + Ar.N) % Ar.N;

    nx_fft_plan_t* plan = nx_fft_plan_create(NX_FFT_NONE, 1, (nx_size_t[]){ Ar.dim[axis0] });
    if (!plan) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    // try to add them, if not throw an error
    if (!nx_T_fft_1d(plan, axis0, Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    nx_fft_plan_free(plan);
    return (ks_obj)to_ret;
}

// nx.fft.ifft1d(A, B=none, axis=-1)
static KS_TFUNC(fft, ifft1d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 3);
    ks_obj aA, aB = KSO_NONE;
    int64_t axis0 = -1;
    if (!ks_parse_params(n_args, args, "A%any ?B%any ?axis%i64", &aA, &aB, &axis0)) return NULL;

    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = NX_DTYPE_CPLX_FP64,
            .N = Ar.N,
            .dim = Ar.dim,
            .stride = NULL
        });

        // set nxar
        Br = NXAR_ARRAY(newB);
        ks_list_push(dels, (ks_obj)newB);
        to_ret = KS_NEWREF(newB);
    } else {
        to_ret = KS_NEWREF(aB);
    }

    axis0 = ((axis0 % Ar.N) + Ar.N) % Ar.N;

    nx_fft_plan_t* plan = nx_fft_plan_create(NX_FFT_INVERSE, 1, (nx_size_t[]){ Ar.dim[axis0] });
    if (!plan) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }

    // try to add them, if not throw an error
    if (!nx_T_fft_1d(plan, axis0, Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        nx_fft_plan_free(plan);
        return NULL;
    }
    
    KS_DECREF(dels);
    nx_fft_plan_free(plan);
    return (ks_obj)to_ret;
}



// nx.fft.fft2d(A, B=none, axis0=-2, axis1=-1)
static KS_TFUNC(fft, fft2d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 4);
    ks_obj aA, aB = KSO_NONE;
    int64_t axis0 = -2, axis1 = -1;
    if (!ks_parse_params(n_args, args, "A%any ?B%any ?axis0%i64 ?axis1%i64", &aA, &aB, &axis0, &axis1)) return NULL;

    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB != KSO_NONE && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = NX_DTYPE_CPLX_FP64,
            .N = Ar.N,
            .dim = Ar.dim,
            .stride = NULL
        });

        // set nxar
        Br = NXAR_ARRAY(newB);
        ks_list_push(dels, (ks_obj)newB);
        to_ret = KS_NEWREF(newB);
    } else {
        to_ret = KS_NEWREF(aB);
    }

    axis0 = ((axis0 % Ar.N) + Ar.N) % Ar.N;
    axis1 = ((axis1 % Ar.N) + Ar.N) % Ar.N;

    nx_fft_plan_t* plan = nx_fft_plan_create(NX_FFT_NONE, 2, (nx_size_t[]){ Ar.dim[axis0], Ar.dim[axis1] });
    if (!plan) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }

    // try to add them, if not throw an error
    if (!nx_T_fft_2d(plan, axis0, axis1, Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    nx_fft_plan_free(plan);
    return (ks_obj)to_ret;
}

// nx.fft.ifft2d(A, B=none, axis0=-2, axis1=-1)
static KS_TFUNC(fft, ifft2d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 4);
    ks_obj aA, aB = KSO_NONE;
    int64_t axis0 = -2, axis1 = -1;
    if (!ks_parse_params(n_args, args, "A%any ?B%any ?axis0%i64 ?axis1%i64", &aA, &aB, &axis0, &axis1)) return NULL;

    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB != KSO_NONE && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = NX_DTYPE_CPLX_FP64,
            .N = Ar.N,
            .dim = Ar.dim,
            .stride = NULL
        });

        // set nxar
        Br = NXAR_ARRAY(newB);
        ks_list_push(dels, (ks_obj)newB);
        to_ret = KS_NEWREF(newB);
    } else {
        to_ret = KS_NEWREF(aB);
    }

    axis0 = ((axis0 % Ar.N) + Ar.N) % Ar.N;
    axis1 = ((axis1 % Ar.N) + Ar.N) % Ar.N;

    nx_fft_plan_t* plan = nx_fft_plan_create(NX_FFT_INVERSE, 2, (nx_size_t[]){ Ar.dim[axis0], Ar.dim[axis1] });
    if (!plan) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }


    // try to add them, if not throw an error
    if (!nx_T_fft_2d(plan, axis0, axis1, Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        nx_fft_plan_free(plan);
        return NULL;
    }

    KS_DECREF(dels);
    nx_fft_plan_free(plan);
    return (ks_obj)to_ret;
}




// nx.fft.fftNd(N, A, B=none, axes=none)
static KS_TFUNC(fft, fftNd) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 4);
    int64_t N;
    ks_obj aA, aB = KSO_NONE;
    ks_obj a_axes = KSO_NONE;
    if (!ks_parse_params(n_args, args, "N%i64 A%any ?B%any ?axes%any", &N, &aA, &aB, &a_axes)) return NULL;

    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB != KSO_NONE && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = NX_DTYPE_CPLX_FP64,
            .N = Ar.N,
            .dim = Ar.dim,
            .stride = NULL
        });

        // set nxar
        Br = NXAR_ARRAY(newB);
        //ks_list_push(dels, (ks_obj)newB);
        to_ret = KS_NEWREF(newB);
    } else {
        to_ret = KS_NEWREF(aB);
    }


    // axes to compute on
    int* axes = ks_malloc(sizeof(*axes) * N);
    int i;

    if (a_axes == KSO_NONE) {
        // default to last axes

        for (i = 0; i < N; ++i) {
            axes[i] = -i - 1;
        }

    } else {
        // otherwise, read integers
        ks_list l_axes = ks_list_from_iterable(a_axes);
        if (!l_axes) {
            KS_DECREF(to_ret);
            KS_DECREF(dels);
            ks_free(axes);
            return NULL;
        }

        if (l_axes->len != N) {
            KS_DECREF(to_ret);
            KS_DECREF(dels);
            KS_DECREF(l_axes);
            ks_free(axes);
            return ks_throw_fmt(ks_type_SizeError, "Expected len(axes)==%i for an %iD FFT", (int)N, (int)N);
        }

        for (i = 0; i < N; ++i) {
            int64_t v64;
            if (!ks_num_get_int64(l_axes->elems[i], &v64)) {
                KS_DECREF(to_ret);
                KS_DECREF(dels);
                KS_DECREF(l_axes);
                ks_free(axes);
                return NULL;
            }
            axes[i] = v64;
        }

        KS_DECREF(l_axes);
    }


    // make sure they are in bounds
    for (i = 0; i < N; ++i) axes[i] = ((axes[i] % Ar.N) + Ar.N) % Ar.N;

    nx_size_t* fft_dims = ks_malloc(sizeof(*fft_dims) * N * 100);

    for (i = 0; i < N; ++i) {
        fft_dims[i] = Ar.dim[axes[i]];
    }

    // create plan
    nx_fft_plan_t* plan = nx_fft_plan_create(NX_FFT_NONE, N, fft_dims);
    if (!plan) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        ks_free(axes);
        return NULL;
    }


    // try to add them, if not throw an error
    if (!nx_T_fft_Nd(plan, N, axes, Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        ks_free(axes);
        nx_fft_plan_free(plan);
        return NULL;
    }

    //KS_DECREF(dels);
    //nx_fft_plan_free(plan);
    return (ks_obj)to_ret;
}



// add the FFT module to `nxmod`
void nx_mod_add_fft(ks_module nxmod) {

    ks_module submod = ks_module_new("nx." SUBMOD);

    ks_dict_set_cn(submod->attr, (ks_dict_ent_c[]){

        {"fft1d",        (ks_obj)ks_cfunc_new2(fft_fft1d_, "nx.fft.fft1d(A, B=none, axis=-1)")},
        {"ifft1d",       (ks_obj)ks_cfunc_new2(fft_ifft1d_, "nx.fft.ifft1d(A, B=none, axis=-2)")},

        {"fft2d",        (ks_obj)ks_cfunc_new2(fft_fft2d_, "nx.fft.fft2d(A, B=none, axis0=-2, axis1=-1)")},
        {"ifft2d",       (ks_obj)ks_cfunc_new2(fft_ifft2d_, "nx.fft.ifft2d(A, B=none, axis0=-2, axis1=-1)")},


        {"fftNd",        (ks_obj)ks_cfunc_new2(fft_fftNd_, "nx.fft.fftNd(N, A, B=none, axes=none)")},
        //{"ifft2d",       (ks_obj)ks_cfunc_new2(fft_ifft2d_, "nx.fft.ifft2d(A, B=none, axis0=-2, axis1=-1)")},


        {NULL, NULL}
    });


    ks_dict_set_cn(nxmod->attr, (ks_dict_ent_c[]){

        {SUBMOD,        (ks_obj)submod},

        {NULL, NULL}
    });

}



