/* fft/module.c - `nx.fft` module
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

#define SUBMOD "fft"



// nx.fft.fft1d(A, B=none)
static KS_TFUNC(fft, fft1d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj aA, aB = NULL;
    if (!ks_parse_params(n_args, args, "A%any ?B%any", &aA, &aB)) return NULL;

    nxar_t Ar, Br;

    // refs to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    if (!aB) {

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = Ar.dtype,
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

    nx_fft_plan_t* plan = nx_fft_plan_create(NX_FFT_NONE, 1, (nx_size_t[]){ Ar.dim[0] });

    // try to add them, if not throw an error
    if (!nx_T_fft_1d(plan, 0, Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        nx_fft_plan_free(plan);
        return NULL;
    }
    
    KS_DECREF(dels);
    nx_fft_plan_free(plan);
    return (ks_obj)to_ret;
}

/*

// nx.fft.ifft1d(A, B=none)
static KS_TFUNC(fft, ifft1d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    ks_obj A = args[0], B = n_args > 1 ? args[1] : NULL;

    ks_obj delA = NULL, delB = NULL;
    struct nxar_t A_nxar, B_nxar;
    NX_CALC_NXAR(A_nxar, A, delA);

    if (!B) {
        // calculate B
        B = (ks_obj)nx_array_new(NX_DTYPE_CPLX_FP64, 1, &A_nxar.dim[0], NULL);
    } else {
        KS_INCREF(B);
    }

    NX_CALC_NXAR(B_nxar, B, delB);

    if (!A_nxar.data || !B_nxar.data) {
        if (delA) KS_DECREF(delA); if (delB) KS_DECREF(delB);
        return NULL;
    }

    bool stat = nx_T_fft_1d(
        NX_FFT_INVERSE,
        NULL,
        _NXARS_(A_nxar), 
        _NXARS_(B_nxar)
    );

    if (delA) KS_DECREF(delA); if (delB) KS_DECREF(delB);
    if (!stat) return NULL;

    return KS_NEWREF(B);
}
*/

/*

// nx.fft.fft2d(A, B=none)
static KS_TFUNC(fft, fft2d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    ks_obj A = args[0], B = n_args > 1 ? args[1] : NULL;

    ks_obj delA = NULL, delB = NULL;
    struct nxar_t A_nxar, B_nxar;
    NX_CALC_NXAR(A_nxar, A, delA);

    if (!A_nxar.data) {
        return NULL;
    }

    if (!B) {
        // calculate B
        B = (ks_obj)nx_array_new(NX_DTYPE_CPLX_FP64, 2, A_nxar.dim, NULL);
    } else {
        KS_INCREF(B);
    }


    NX_CALC_NXAR(B_nxar, B, delB);

    if (!A_nxar.data || !B_nxar.data) {
        if (delA) KS_DECREF(delA); if (delB) KS_DECREF(delB);
        return NULL;
    }

    bool stat = nx_T_fft_2d(
        NX_FFT_NONE,
        NULL, NULL,
        _NXARS_(A_nxar), 
        _NXARS_(B_nxar)
    );

    if (delA) KS_DECREF(delA); if (delB) KS_DECREF(delB);
    if (!stat) return NULL;

    return KS_NEWREF(B);
}


// nx.fft.ifft2d(A, B=none)
static KS_TFUNC(fft, ifft2d) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);

    ks_obj A = args[0], B = n_args > 1 ? args[1] : NULL;

    ks_obj delA = NULL, delB = NULL;
    struct nxar_t A_nxar, B_nxar;
    NX_CALC_NXAR(A_nxar, A, delA);

    if (!B) {
        // calculate B
        B = (ks_obj)nx_array_new(NX_DTYPE_CPLX_FP64, 2, A_nxar.dim, NULL);
    } else {
        KS_INCREF(B);
    }

    NX_CALC_NXAR(B_nxar, B, delB);

    if (!A_nxar.data || !B_nxar.data) {
        if (delA) KS_DECREF(delA); if (delB) KS_DECREF(delB);
        return NULL;
    }

    bool stat = nx_T_fft_2d(
        NX_FFT_INVERSE,
        NULL, NULL,
        _NXARS_(A_nxar), 
        _NXARS_(B_nxar)
    );

    if (delA) KS_DECREF(delA); if (delB) KS_DECREF(delB);
    if (!stat) return NULL;

    return KS_NEWREF(B);
}


*/

// add the FFT module to `nxmod`
void nx_mod_add_fft(ks_module nxmod) {

    ks_module submod = ks_module_new("nx." SUBMOD);

    ks_dict_set_cn(submod->attr, (ks_dict_ent_c[]){

        {"fft1d",        (ks_obj)ks_cfunc_new2(fft_fft1d_, "nx.fft.fft1d(A, B=none)")},
       /// {"ifft1d",       (ks_obj)ks_cfunc_new2(fft_ifft1d_, "nx.fft.ifft1d(A, B=none)")},

/*
        {"fft2d",        (ks_obj)ks_cfunc_new2(fft_fft2d_, "nx.fft.fft2d(A, B=none)")},
        {"ifft2d",       (ks_obj)ks_cfunc_new2(fft_ifft2d_, "nx.fft.ifft2d(A, B=none)")},
*/



        {NULL, NULL}
    });


    ks_dict_set_cn(nxmod->attr, (ks_dict_ent_c[]){

        {SUBMOD,        (ks_obj)submod},

        {NULL, NULL}
    });

}



