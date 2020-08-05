/* fft/module.c - `nx.fft` module
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

#define SUBMOD "fft"


// nx.fft.fftN(A, axes=none, B=none)
static KS_TFUNC(fft, fftN) {
    ks_obj a_axes = NULL;
    ks_obj aA , aB = KSO_NONE;
    KS_GETARGS("A ?axes ?B", &aA, &a_axes, &aB);

    int i;
    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB != KSO_NONE && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    // rank FFT
    int fft_rank = -1;

    // determine axes
    int* axes = NULL;

    // convert axes to a list
    if (!a_axes || a_axes == KSO_NONE) {

        // do it over all axes
        fft_rank = Ar.rank;
        axes = ks_malloc(sizeof(*axes) * fft_rank);

        // all of them
        for (i = 0; i < fft_rank; ++i) axes[i] = i;

    } else {
        ks_list l_axes = ks_list_new_iter(a_axes);
        if (!l_axes) {
            KS_DECREF(dels);
            return NULL;
        }
        
        // FFT rank & axes
        fft_rank = l_axes->len;
        axes = ks_malloc(sizeof(*axes) * fft_rank);

        for (i = 0; i < fft_rank; ++i) {
            int64_t v64;
            if (!ks_num_get_int64(l_axes->elems[i], &v64)) {
                KS_DECREF(l_axes);
                ks_free(axes);
                KS_DECREF(dels);
                return NULL;
            }
            axes[i] = v64;
        }

        // done with the list; all are now converted to ints
        KS_DECREF(l_axes);
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {
        // create new array
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = nx_dtype_cplx_fp64,
            .rank = Ar.rank,
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

    // wrap axes around
    for (i = 0; i < fft_rank; ++i) axes[i] = ((axes[i] % Ar.rank) + Ar.rank) % Ar.rank;

    // now, create FFT dimensions
    nx_size_t* fft_dims = ks_malloc(sizeof(*fft_dims) * fft_rank);

    for (i = 0; i < fft_rank; ++i) {
        fft_dims[i] = Ar.dim[axes[i]];
    }

    // create plan
    nx_fft_plan plan = nx_fft_plan_create(NX_FFT_PLAN_NONE, fft_rank, fft_dims, false);
    if (!plan) {
        ks_free(axes);
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }

    // try to add them, if not throw an error
    if (!nx_fft_plan_do(plan, Ar, Br, axes)) {
        ks_free(axes);
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        KS_DECREF(plan);
        return NULL;
    }

    KS_DECREF(dels);
    KS_DECREF(plan);
    return (ks_obj)to_ret;
}

// nx.fft.ifftN(A, axes=none, B=none)
static KS_TFUNC(fft, ifftN) {
    ks_obj a_axes = NULL;
    ks_obj aA , aB = KSO_NONE;
    KS_GETARGS("A ?axes ?B", &aA, &a_axes, &aB);

    int i;
    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB != KSO_NONE && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    // rank FFT
    int fft_rank = -1;

    // determine axes
    int* axes = NULL;

    // convert axes to a list
    if (!a_axes || a_axes == KSO_NONE) {

        // do it over all axes
        fft_rank = Ar.rank;
        axes = ks_malloc(sizeof(*axes) * fft_rank);

        // all of them
        for (i = 0; i < fft_rank; ++i) axes[i] = i;

    } else {
        ks_list l_axes = ks_list_new_iter(a_axes);
        if (!l_axes) {
            KS_DECREF(dels);
            return NULL;
        }
        
        // FFT rank & axes
        fft_rank = l_axes->len;
        axes = ks_malloc(sizeof(*axes) * fft_rank);

        for (i = 0; i < fft_rank; ++i) {
            int64_t v64;
            if (!ks_num_get_int64(l_axes->elems[i], &v64)) {
                KS_DECREF(l_axes);
                ks_free(axes);
                KS_DECREF(dels);
                return NULL;
            }
            axes[i] = v64;
        }

        // done with the list; all are now converted to ints
        KS_DECREF(l_axes);
    }

    ks_obj to_ret = NULL;

    if (aB == KSO_NONE) {
        // create new array
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = nx_dtype_cplx_fp64,
            .rank = Ar.rank,
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

    // wrap axes around
    for (i = 0; i < fft_rank; ++i) axes[i] = ((axes[i] % Ar.rank) + Ar.rank) % Ar.rank;

    // now, create FFT dimensions
    nx_size_t* fft_dims = ks_malloc(sizeof(*fft_dims) * fft_rank);

    for (i = 0; i < fft_rank; ++i) {
        fft_dims[i] = Ar.dim[axes[i]];
    }

    // create plan
    nx_fft_plan plan = nx_fft_plan_create(NX_FFT_PLAN_NONE, fft_rank, fft_dims, true);
    if (!plan) {
        ks_free(axes);
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }

    // try to add them, if not throw an error
    if (!nx_fft_plan_do(plan, Ar, Br, axes)) {
        ks_free(axes);
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        KS_DECREF(plan);
        return NULL;
    }

    KS_DECREF(dels);
    KS_DECREF(plan);
    return (ks_obj)to_ret;
}


// add the FFT module to `nxmod`
void nx_mod_add_fft(ks_module nxmod) {

    ks_module submod = ks_module_new("nx." SUBMOD);

    nx_T_init_fft_plan();

    ks_dict_set_c(submod->attr, KS_KEYVALS(

        {"Plan",        (ks_obj)nx_T_fft_plan},

        {"fftN",        (ks_obj)ks_cfunc_new_c(fft_fftN_, "nx.fft.fftN(A, axes=none, B=none)")},
        {"ifftN",        (ks_obj)ks_cfunc_new_c(fft_ifftN_, "nx.fft.fftN(A, axes=none, B=none)")},
        //{"ifft2d",       (ks_obj)ks_cfunc_new_c(fft_ifft2d_, "nx.fft.ifft2d(A, B=none, axis0=-2, axis1=-1)")},


    ));

    ks_dict_set_c(nxmod->attr, KS_KEYVALS(

        {SUBMOD,        (ks_obj)submod},

    ));

}



