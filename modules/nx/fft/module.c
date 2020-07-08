/* fft/module.c - `nx.fft` module
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

#define SUBMOD "fft"


// nx.fft.fftN(axes, A, B=none)
static KS_TFUNC(fft, fftN) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 3);
    ks_obj a_axes;
    ks_obj aA, aB = KSO_NONE;
    if (!ks_parse_params(n_args, args, "axes%any A%any ?B%any", &a_axes, &aA, &aB)) return NULL;

    int i;

    // convert axes to a list
    ks_list l_axes = ks_list_from_iterable(a_axes);
    if (!l_axes) return NULL;

    // the FFT rank
    int fft_rank = l_axes->len;

    // determine axes
    int* axes = ks_malloc(sizeof(*axes) * fft_rank);
    for (i = 0; i < fft_rank; ++i) {
        int64_t v64;
        if (!ks_num_get_int64(l_axes->elems[i], &v64)) {
            KS_DECREF(l_axes);
            ks_free(axes);
            return NULL;
        }
        axes[i] = v64;
    }

    // done with the list; all are now converted to ints
    KS_DECREF(l_axes);


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
            .dtype = nx_dtype_cplx_fp64,
            .rank = Ar.rank,
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

// nx.fft.ifftN(axes, A, B=none)
static KS_TFUNC(fft, ifftN) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 3);
    ks_obj a_axes;
    ks_obj aA, aB = KSO_NONE;
    if (!ks_parse_params(n_args, args, "axes%any A%any ?B%any", &a_axes, &aA, &aB)) return NULL;

    int i;

    // convert axes to a list
    ks_list l_axes = ks_list_from_iterable(a_axes);
    if (!l_axes) return NULL;

    // the FFT rank
    int fft_rank = l_axes->len;

    // determine axes
    int* axes = ks_malloc(sizeof(*axes) * fft_rank);
    for (i = 0; i < fft_rank; ++i) {
        int64_t v64;
        if (!ks_num_get_int64(l_axes->elems[i], &v64)) {
            KS_DECREF(l_axes);
            ks_free(axes);
            return NULL;
        }
        axes[i] = v64;
    }

    // done with the list; all are now converted to ints
    KS_DECREF(l_axes);


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
            .dtype = nx_dtype_cplx_fp64,
            .rank = Ar.rank,
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

    nx_type_fft_plan_init();

    ks_dict_set_cn(submod->attr, (ks_dict_ent_c[]){

        {"Plan",        (ks_obj)nx_type_fft_plan},

        {"fftN",        (ks_obj)ks_cfunc_new2(fft_fftN_, "nx.fft.fftN(axes, A, B=none)")},
        {"ifftN",        (ks_obj)ks_cfunc_new2(fft_ifftN_, "nx.fft.fftN(axes, A, B=none)")},
        //{"ifft2d",       (ks_obj)ks_cfunc_new2(fft_ifft2d_, "nx.fft.ifft2d(A, B=none, axis0=-2, axis1=-1)")},


        {NULL, NULL}
    });


    ks_dict_set_cn(nxmod->attr, (ks_dict_ent_c[]){

        {SUBMOD,        (ks_obj)submod},

        {NULL, NULL}
    });

}



