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


ks_type nx_enum_dtype = NULL;

ks_Enum
    nx_SINT8        = NULL,
    nx_UINT8        = NULL,
    nx_SINT16       = NULL,
    nx_UINT16       = NULL,
    nx_SINT32       = NULL,
    nx_UINT32       = NULL,
    nx_SINT64       = NULL,
    nx_UINT64       = NULL,

    nx_FP32         = NULL,
    nx_FP64         = NULL,

    nx_CPLX_FP32    = NULL,
    nx_CPLX_FP64    = NULL
;

ks_cfunc
    nx_F_add,
    nx_F_sub,
    nx_F_mul,
    nx_F_div
;



// nx.size(obj) -> return the size (in bytes) of the data type or array
static KS_TFUNC(nx, size) {
    KS_REQ_N_ARGS(n_args, 1);

    ks_obj obj = args[0];
    if (obj->type == nx_enum_dtype) {
        ks_Enum obj_enum = (ks_Enum)obj;
        
        int size_of = nx_dtype_size(obj_enum->enum_idx);
        if (!size_of) return NULL;
        else {
            return (ks_obj)ks_int_new(size_of);
        }

    } else {
        // todo
        return ks_throw_fmt(ks_type_ToDoError, "size of objects other than dtype is not implemented!");
    }
}

// TODO: improve this casting rule, even though its pretty dang good
static enum nx_dtype get_cast(enum nx_dtype A, enum nx_dtype B) {

    if (A > B) return A;
    else return B;

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
        int maxN = Ar.N > Br.N ? Ar.N : Br.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.N, Br.N }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .N = maxN,
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
        int maxN = Ar.N > Br.N ? Ar.N : Br.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.N, Br.N }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .N = maxN,
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
        int maxN = Ar.N > Br.N ? Ar.N : Br.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.N, Br.N }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .N = maxN,
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
        int maxN = Ar.N > Br.N ? Ar.N : Br.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Ar.N, Br.N }, (nx_size_t*[]){ Ar.dim, Br.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            KS_DECREF(dels);
            return NULL;
        }

        // create new array
        // TODO: auto-detect type as well
        nx_array newC = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = get_cast(Ar.dtype, Br.dtype),
            .N = maxN,
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






// now, export them all
static ks_module get_module() {

    // create enum

    nx_enum_dtype = ks_Enum_create_c("dtype", (ks_enum_entry_c[]){
        {"NONE",           NX_DTYPE_NONE},

        {"SINT8",          NX_DTYPE_SINT8},
        {"UINT8",          NX_DTYPE_UINT8},
        {"SINT16",         NX_DTYPE_SINT16},
        {"UINT16",         NX_DTYPE_UINT16},
        {"SINT32",         NX_DTYPE_SINT32},
        {"UINT32",         NX_DTYPE_UINT32},
        {"SINT64",         NX_DTYPE_SINT64},
        {"UINT64",         NX_DTYPE_UINT64},

        {"FP32",           NX_DTYPE_FP32},
        {"FP64",           NX_DTYPE_FP64},

        {"CPLX_FP32",      NX_DTYPE_CPLX_FP32},
        {"CPLX_FP64",      NX_DTYPE_CPLX_FP64},

        {NULL, -1},
    });


    nx_SINT8     = ks_Enum_get_c(nx_enum_dtype, "SINT8");
    nx_UINT8     = ks_Enum_get_c(nx_enum_dtype, "UINT8");
    nx_SINT16    = ks_Enum_get_c(nx_enum_dtype, "SINT16");
    nx_UINT16    = ks_Enum_get_c(nx_enum_dtype, "UINT16");
    nx_SINT32    = ks_Enum_get_c(nx_enum_dtype, "SINT32");
    nx_UINT32    = ks_Enum_get_c(nx_enum_dtype, "UINT32");
    nx_SINT64    = ks_Enum_get_c(nx_enum_dtype, "SINT64");
    nx_UINT64    = ks_Enum_get_c(nx_enum_dtype, "UINT64");

    nx_FP32      = ks_Enum_get_c(nx_enum_dtype, "FP32");
    nx_FP64      = ks_Enum_get_c(nx_enum_dtype, "FP64");

    nx_CPLX_FP32 = ks_Enum_get_c(nx_enum_dtype, "CPLX_FP32");
    nx_CPLX_FP64 = ks_Enum_get_c(nx_enum_dtype, "CPLX_FP64");


    ks_module mod = ks_module_new(MODULE_NAME);

    // set up types
    nx_type_array_init();
    nx_type_view_init();

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){

        {"dtype",                 (ks_obj)nx_enum_dtype},

        {"array",                 (ks_obj)nx_type_array},
        {"view",                  (ks_obj)nx_type_view},

        {"size",                  (ks_obj)ks_cfunc_new2(nx_size_, "nx.size(obj)")},

        {"add",                   (ks_obj)(nx_F_add = ks_cfunc_new2(nx_add_, "nx.add(A, B, C=none)"))},
        {"sub",                   (ks_obj)(nx_F_sub = ks_cfunc_new2(nx_sub_, "nx.sub(A, B, C=none)"))},
        {"mul",                   (ks_obj)(nx_F_mul = ks_cfunc_new2(nx_mul_, "nx.mul(A, B, C=none)"))},
        {"div",                   (ks_obj)(nx_F_div = ks_cfunc_new2(nx_div_, "nx.div(A, B, C=none)"))},


        {NULL, NULL}
    });

    ks_module_add_enum_members(mod, nx_enum_dtype);

    /* SUBMODULES */


    nx_mod_add_fft(mod);
    nx_mod_add_la(mod);

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
