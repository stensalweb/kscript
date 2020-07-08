/* src/module.c - main module for nx
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "nx"


// include this since this is a module.
#include "ks-module.h"

#include "../nx-impl.h"


ks_cfunc
    nx_F_add,
    nx_F_sub,
    nx_F_mul,
    nx_F_div,
    nx_F_pow,
    nx_F_abs
;


// nx.size(obj) -> return the size (in bytes) of the data type or array
static KS_TFUNC(nx, size) {
    KS_REQ_N_ARGS(n_args, 1);

    ks_obj obj = args[0];
    if (obj->type == nx_type_dtype) {
        nx_dtype obj_dt = (nx_dtype)obj;
        return (ks_obj)ks_int_new(obj_dt->size);
    } else {
        // todo
        return ks_throw_fmt(ks_type_ToDoError, "size of objects other than dtype is not implemented!");
    }
}

// TODO: improve this casting rule
static nx_dtype get_cast(nx_dtype A, nx_dtype B) {
    if (A->kind == NX_DTYPE_KIND_CCOMPLEX && B->kind == NX_DTYPE_KIND_CCOMPLEX) {
        return A->size > B->size ? A : B;
    } else if (A->kind == NX_DTYPE_KIND_CCOMPLEX) {
        return A;
    } else if (B->kind == NX_DTYPE_KIND_CCOMPLEX) {
        return B;
    } else if (A->kind == NX_DTYPE_KIND_CFLOAT && B->kind == NX_DTYPE_KIND_CFLOAT) {
        return A->size > B->size ? A : B;
    } else if (A->kind == NX_DTYPE_KIND_CFLOAT) {
        return A;
    } else if (B->kind == NX_DTYPE_KIND_CFLOAT) {
        return B;
    } else {
        // probably integer
        return A->size > B->size ? A : B;
    }
}



// nx.zeros(shape, dtype=nx.fp32)
static KS_TFUNC(nx, zeros) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj shape;
    nx_dtype dtype = nx_dtype_fp32;
    if (!ks_parse_params(n_args, args, "shape%iter ?dtype%*", &shape, &dtype, nx_type_dtype)) return NULL;


    // convert to a list
    ks_list shape_list = ks_list_from_iterable(shape);
    if (!shape_list) return NULL;

    if (shape_list->len < 1) {
        KS_DECREF(shape_list);
        return ks_throw_fmt(ks_type_SizeError, "Expected len(shape) > 0");
    }

    nx_size_t* shape_sz = ks_malloc(sizeof(*shape_sz) * shape_list->len);

    int i;
    for (i = 0; i < shape_list->len; ++i) {
        int64_t v64;
        if (!ks_num_get_int64(shape_list->elems[i], &v64)) {
            ks_free(shape_sz);
            KS_DECREF(shape_list);
            return NULL;
        }

        shape_sz[i] = v64;
    }

    nx_array res = nx_array_new((nxar_t){
        .data = NULL,
        .dtype = dtype,
        .rank = shape_list->len,
        .dim = shape_sz,
        .stride = NULL
    });

    ks_free(shape_sz);
    KS_DECREF(shape_list);

    return (ks_obj)res;

}

// nx.add(A, B, C=none)
// Compute C=A+B, and return C (C is created if C==none)
static KS_TFUNC(nx, add) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;


    // array designators
    nxar_t Ar, Br, Cr;

    // references to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || !nx_get_nxar(aB, &Br, dels) || (aC && !nx_get_nxar(aC, &Cr, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    // create results array
    if (!aC) {
        // dimension will be the maximum dimension
        int maxN = Ar.rank > Br.rank ? Ar.rank : Br.rank;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.rank, Br.rank }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .rank = maxN,
            .dim = Cdim,
            .stride = NULL
        });

        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Cr = NXAR_ARRAY(newC);
        ks_list_push(dels, (ks_obj)newC);
        to_ret = KS_NEWREF(newC);
    } else {
        to_ret = KS_NEWREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_add(Ar, Br, Cr)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    return (ks_obj)to_ret;
}


// nx.sub(A, B, C=none)
// Compute C=A-B, and return C (C is created if C==none)
static KS_TFUNC(nx, sub) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    // array designators
    nxar_t Ar, Br, Cr;

    // references to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || !nx_get_nxar(aB, &Br, dels) || (aC && !nx_get_nxar(aC, &Cr, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    // create results array
    if (!aC) {
        // dimension will be the maximum dimension
        int maxN = Ar.rank > Br.rank ? Ar.rank : Br.rank;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.rank, Br.rank }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .rank = maxN,
            .dim = Cdim,
            .stride = NULL
        });

        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Cr = NXAR_ARRAY(newC);
        ks_list_push(dels, (ks_obj)newC);
        to_ret = KS_NEWREF(newC);
    } else {
        to_ret = KS_NEWREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_sub(Ar, Br, Cr)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    return (ks_obj)to_ret;
}


// nx.mul(A, B, C=none)
// Compute C=A*B, and return C (C is created if C==none)
static KS_TFUNC(nx, mul) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    // array designators
    nxar_t Ar, Br, Cr;

    // references to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || !nx_get_nxar(aB, &Br, dels) || (aC && !nx_get_nxar(aC, &Cr, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    // create results array
    if (!aC) {
        // dimension will be the maximum dimension
        int maxN = Ar.rank > Br.rank ? Ar.rank : Br.rank;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.rank, Br.rank }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .rank = maxN,
            .dim = Cdim,
            .stride = NULL
        });

        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Cr = NXAR_ARRAY(newC);
        ks_list_push(dels, (ks_obj)newC);
        to_ret = KS_NEWREF(newC);
    } else {
        to_ret = KS_NEWREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_mul(Ar, Br, Cr)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    return (ks_obj)to_ret;
}

// nx.div(A, B, C=none)
// Compute C=A/B, and return C (C is created if C==none)
static KS_TFUNC(nx, div) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    // array designators
    nxar_t Ar, Br, Cr;

    // references to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || !nx_get_nxar(aB, &Br, dels) || (aC && !nx_get_nxar(aC, &Cr, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    // create results array
    if (!aC) {
        // dimension will be the maximum dimension
        int maxN = Ar.rank > Br.rank ? Ar.rank : Br.rank;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.rank, Br.rank }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .rank = maxN,
            .dim = Cdim,
            .stride = NULL
        });

        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Cr = NXAR_ARRAY(newC);
        ks_list_push(dels, (ks_obj)newC);
        to_ret = KS_NEWREF(newC);
    } else {
        to_ret = KS_NEWREF(aC);
    }


    // try to add them, if not throw an error
    if (!nx_T_div(Ar, Br, Cr)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    return (ks_obj)to_ret;
}

// nx.pow(A, B, C=none)
// Compute C=A**B, and return C (C is created if C==none)
static KS_TFUNC(nx, pow) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    // array designators
    nxar_t Ar, Br, Cr;

    // references to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || !nx_get_nxar(aB, &Br, dels) || (aC && !nx_get_nxar(aC, &Cr, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    // create results array
    if (!aC) {
        // dimension will be the maximum dimension
        int maxN = Ar.rank > Br.rank ? Ar.rank : Br.rank;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.rank, Br.rank }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .rank = maxN,
            .dim = Cdim,
            .stride = NULL
        });

        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Cr = NXAR_ARRAY(newC);
        ks_list_push(dels, (ks_obj)newC);
        to_ret = KS_NEWREF(newC);
    } else {
        to_ret = KS_NEWREF(aC);
    }


    // try to add them, if not throw an error
    if (!nx_T_pow(Ar, Br, Cr)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    return (ks_obj)to_ret;
}



// nx.abs(A, B=none)
static KS_TFUNC(nx, abs) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj aA, aB = NULL;
    if (!ks_parse_params(n_args, args, "A%any ?B%any", &aA, &aB)) return NULL;

    // array designators
    nxar_t Ar, Br;

    // references to delete
    ks_list dels = ks_list_new(0, NULL);

    // convert them to C arrays
    if (!nx_get_nxar(aA, &Ar, dels) || (aB && !nx_get_nxar(aB, &Br, dels))) {
        KS_DECREF(dels);
        return NULL;
    }

    ks_obj to_ret = NULL;

    // create results array
    if (!aB) {
        // dimension will be the maximum dimension

        nx_dtype rdt = Ar.dtype;
        if (rdt == nx_dtype_cplx_fp32) rdt = nx_dtype_fp32;
        else if (rdt == nx_dtype_cplx_fp64) rdt = nx_dtype_fp64;

        // create new array
        // TODO: auto-detect type as well
        nx_array newB = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = rdt,
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


    // try to add them, if not throw an error
    if (!nx_T_abs(Ar, Br)) {
        KS_DECREF(to_ret);
        KS_DECREF(dels);
        return NULL;
    }
    
    KS_DECREF(dels);
    return (ks_obj)to_ret;
}


// now, export them all
static ks_module get_module() {

    // create enum

    ks_module mod = ks_module_new(MODULE_NAME);

    // set up types
    nx_type_dtype_init();
    nx_type_array_init();
    nx_type_view_init();

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){

        {"dtype",                 (ks_obj)nx_type_dtype},

        {"array",                 (ks_obj)nx_type_array},
        {"view",                  (ks_obj)nx_type_view},

        {"size",                  (ks_obj)ks_cfunc_new2(nx_size_, "nx.size(obj)")},

        {"zeros",                 (ks_obj)ks_cfunc_new2(nx_zeros_, "nx.zeros(shape, dtype=nx.fp32)")},

        {"add",                   (ks_obj)(nx_F_add = ks_cfunc_new2(nx_add_, "nx.add(A, B, C=none)"))},
        {"sub",                   (ks_obj)(nx_F_sub = ks_cfunc_new2(nx_sub_, "nx.sub(A, B, C=none)"))},
        {"mul",                   (ks_obj)(nx_F_mul = ks_cfunc_new2(nx_mul_, "nx.mul(A, B, C=none)"))},
        {"div",                   (ks_obj)(nx_F_div = ks_cfunc_new2(nx_div_, "nx.div(A, B, C=none)"))},
        {"pow",                   (ks_obj)(nx_F_pow = ks_cfunc_new2(nx_pow_, "nx.pow(A, B, C=none)"))},

        {"abs",                   (ks_obj)(nx_F_abs = ks_cfunc_new2(nx_abs_, "nx.abs(A, B=none)"))},

        {"sint8",                 (ks_obj)nx_dtype_sint8},
        {"uint8",                 (ks_obj)nx_dtype_sint8},
        {"sint16",                (ks_obj)nx_dtype_sint16},
        {"uint16",                (ks_obj)nx_dtype_sint16},
        {"sint32",                (ks_obj)nx_dtype_sint32},
        {"uint32",                (ks_obj)nx_dtype_sint32},
        {"sint64",                (ks_obj)nx_dtype_sint64},
        {"uint64",                (ks_obj)nx_dtype_sint64},

        {"fp32",                  (ks_obj)nx_dtype_fp32},
        {"fp64",                  (ks_obj)nx_dtype_fp64},
        
        {"cplx_fp64",             (ks_obj)nx_dtype_cplx_fp32},
        {"cplx_fp64",             (ks_obj)nx_dtype_cplx_fp64},


        {NULL, NULL}
    });


    /* SUBMODULES */

    nx_mod_add_fft(mod);
    nx_mod_add_la(mod);

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
