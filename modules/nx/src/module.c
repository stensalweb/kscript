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



// nx.add(A, B, C=none)
// Compute C=A+B, and return C (C is created if C==none)
static KS_TFUNC(nx, add) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    struct nxar_t Aar, Bar, Car;
    
    // pointers to dereference at the end (NULL if none)
    ks_obj delA = NULL, delB = NULL;


    // convert aA and aB to nxar variables
    /**/ if (aA->type == nx_type_array) Aar = GET_NXAR_ARRAY(((nx_array)aA));
    else if (ks_num_is_numeric(aA) || ks_is_iterable(aA)) {
        delA = (ks_obj)nx_array_from_obj(aA, NX_DTYPE_NONE);
        if (!delA) return NULL;
        Aar = GET_NXAR_ARRAY(((nx_array)delA));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aA);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aB->type == nx_type_array) Bar = GET_NXAR_ARRAY(((nx_array)aB));
    else if (ks_num_is_numeric(aB) || ks_is_iterable(aB)) {
        delB = (ks_obj)nx_array_from_obj(aB, NX_DTYPE_NONE);
        if (!delB) return NULL;
        Bar = GET_NXAR_ARRAY(((nx_array)delB));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aB);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aC == NULL) {} // it will be created
    else if (aC->type == nx_type_array) Car = GET_NXAR_ARRAY(((nx_array)aC));
    else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T' as destination", aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    if (aC == NULL) {
        // create 'C'

        // dimension will be the maximum dimension
        int maxN = Aar.N > Bar.N ? Aar.N : Bar.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Aar.N, Bar.N }, (nx_size_t*[]){ Aar.dim, Bar.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            if (delA) KS_DECREF(delA);
            if (delB) KS_DECREF(delB);
            return NULL;
        }

        // create new array
        // TODO: figure out casting rules
        aC = (ks_obj)nx_array_new(Aar.dtype, maxN, Cdim);
        
        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Car = GET_NXAR_ARRAY(((nx_array)aC));

    } else {
        // add another reference to keep it even with the above
        KS_INCREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_add(
        _NXARS_(Aar),
        _NXARS_(Bar),
        _NXARS_(Car)
    )) {
        KS_DECREF(aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }
    
    if (delA) KS_DECREF(delA);
    if (delB) KS_DECREF(delB);
    return (ks_obj)aC;
}


// nx.sub(A, B, C=none)
// Compute C=A-B, and return C (C is created if C==none)
static KS_TFUNC(nx, sub) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    struct nxar_t Aar, Bar, Car;
    
    // pointers to dereference at the end (NULL if none)
    ks_obj delA = NULL, delB = NULL;


    // convert aA and aB to nxar variables
    /**/ if (aA->type == nx_type_array) Aar = GET_NXAR_ARRAY(((nx_array)aA));
    else if (ks_num_is_numeric(aA) || ks_is_iterable(aA)) {
        delA = (ks_obj)nx_array_from_obj(aA, NX_DTYPE_NONE);
        if (!delA) return NULL;
        Aar = GET_NXAR_ARRAY(((nx_array)delA));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aA);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aB->type == nx_type_array) Bar = GET_NXAR_ARRAY(((nx_array)aB));
    else if (ks_num_is_numeric(aB) || ks_is_iterable(aB)) {
        delB = (ks_obj)nx_array_from_obj(aB, NX_DTYPE_NONE);
        if (!delB) return NULL;
        Bar = GET_NXAR_ARRAY(((nx_array)delB));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aB);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aC == NULL) {} // it will be created
    else if (aC->type == nx_type_array) Car = GET_NXAR_ARRAY(((nx_array)aC));
    else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T' as destination", aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    if (aC == NULL) {
        // create 'C'

        // dimension will be the maximum dimension
        int maxN = Aar.N > Bar.N ? Aar.N : Bar.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Aar.N, Bar.N }, (nx_size_t*[]){ Aar.dim, Bar.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            if (delA) KS_DECREF(delA);
            if (delB) KS_DECREF(delB);
            return NULL;
        }

        // create new array
        // TODO: figure out casting rules
        aC = (ks_obj)nx_array_new(Aar.dtype, maxN, Cdim);
        
        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Car = GET_NXAR_ARRAY(((nx_array)aC));

    } else {
        // add another reference to keep it even with the above
        KS_INCREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_sub(
        _NXARS_(Aar),
        _NXARS_(Bar),
        _NXARS_(Car)
    )) {
        KS_DECREF(aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }
    
    if (delA) KS_DECREF(delA);
    if (delB) KS_DECREF(delB);
    return (ks_obj)aC;
}


// nx.mul(A, B, C=none)
// Compute C=A*B, and return C (C is created if C==none)
static KS_TFUNC(nx, mul) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    struct nxar_t Aar, Bar, Car;
    
    // pointers to dereference at the end (NULL if none)
    ks_obj delA = NULL, delB = NULL;


    // convert aA and aB to nxar variables
    /**/ if (aA->type == nx_type_array) Aar = GET_NXAR_ARRAY(((nx_array)aA));
    else if (ks_num_is_numeric(aA) || ks_is_iterable(aA)) {
        delA = (ks_obj)nx_array_from_obj(aA, NX_DTYPE_NONE);
        if (!delA) return NULL;
        Aar = GET_NXAR_ARRAY(((nx_array)delA));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aA);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aB->type == nx_type_array) Bar = GET_NXAR_ARRAY(((nx_array)aB));
    else if (ks_num_is_numeric(aB) || ks_is_iterable(aB)) {
        delB = (ks_obj)nx_array_from_obj(aB, NX_DTYPE_NONE);
        if (!delB) return NULL;
        Bar = GET_NXAR_ARRAY(((nx_array)delB));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aB);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aC == NULL) {} // it will be created
    else if (aC->type == nx_type_array) Car = GET_NXAR_ARRAY(((nx_array)aC));
    else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T' as destination", aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    if (aC == NULL) {
        // create 'C'

        // dimension will be the maximum dimension
        int maxN = Aar.N > Bar.N ? Aar.N : Bar.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Aar.N, Bar.N }, (nx_size_t*[]){ Aar.dim, Bar.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            if (delA) KS_DECREF(delA);
            if (delB) KS_DECREF(delB);
            return NULL;
        }

        // create new array
        // TODO: figure out casting rules
        aC = (ks_obj)nx_array_new(Aar.dtype, maxN, Cdim);
        
        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Car = GET_NXAR_ARRAY(((nx_array)aC));

    } else {
        // add another reference to keep it even with the above
        KS_INCREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_mul(
        _NXARS_(Aar),
        _NXARS_(Bar),
        _NXARS_(Car)
    )) {
        KS_DECREF(aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }
    
    if (delA) KS_DECREF(delA);
    if (delB) KS_DECREF(delB);
    return (ks_obj)aC;
}

// nx.div(A, B, C=none)
// Compute C=A/B, and return C (C is created if C==none)
static KS_TFUNC(nx, div) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_obj aA, aB, aC = NULL;
    if (!ks_parse_params(n_args, args, "A%any B%any ?C%any", &aA, &aB, &aC)) return NULL;

    struct nxar_t Aar, Bar, Car;
    
    // pointers to dereference at the end (NULL if none)
    ks_obj delA = NULL, delB = NULL;


    // convert aA and aB to nxar variables
    /**/ if (aA->type == nx_type_array) Aar = GET_NXAR_ARRAY(((nx_array)aA));
    else if (ks_num_is_numeric(aA) || ks_is_iterable(aA)) {
        delA = (ks_obj)nx_array_from_obj(aA, NX_DTYPE_NONE);
        if (!delA) return NULL;
        Aar = GET_NXAR_ARRAY(((nx_array)delA));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aA);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aB->type == nx_type_array) Bar = GET_NXAR_ARRAY(((nx_array)aB));
    else if (ks_num_is_numeric(aB) || ks_is_iterable(aB)) {
        delB = (ks_obj)nx_array_from_obj(aB, NX_DTYPE_NONE);
        if (!delB) return NULL;
        Bar = GET_NXAR_ARRAY(((nx_array)delB));
    } else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T'", aB);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    /**/ if (aC == NULL) {} // it will be created
    else if (aC->type == nx_type_array) Car = GET_NXAR_ARRAY(((nx_array)aC));
    else {
        ks_throw_fmt(ks_type_TypeError, "nx operation cannot take objects of type '%T' as destination", aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }

    if (aC == NULL) {
        // create 'C'

        // dimension will be the maximum dimension
        int maxN = Aar.N > Bar.N ? Aar.N : Bar.N;

        nx_size_t* Cdim = ks_malloc(sizeof(*Cdim) * maxN);

        // try to compute the size of 'C'
        if (!nx_compute_bcast(2, (int[]){ Aar.N, Bar.N }, (nx_size_t*[]){ Aar.dim, Bar.dim }, maxN, Cdim)) {
            ks_free(Cdim);
            if (delA) KS_DECREF(delA);
            if (delB) KS_DECREF(delB);
            return NULL;
        }

        // create new array
        // TODO: figure out casting rules
        aC = (ks_obj)nx_array_new(Aar.dtype, maxN, Cdim);
        
        // free temporary resources
        ks_free(Cdim);

        // set nxar
        Car = GET_NXAR_ARRAY(((nx_array)aC));

    } else {
        // add another reference to keep it even with the above
        KS_INCREF(aC);
    }

    // try to add them, if not throw an error
    if (!nx_T_div(
        _NXARS_(Aar),
        _NXARS_(Bar),
        _NXARS_(Car)
    )) {
        KS_DECREF(aC);
        if (delA) KS_DECREF(delA);
        if (delB) KS_DECREF(delB);
        return NULL;
    }
    
    if (delA) KS_DECREF(delA);
    if (delB) KS_DECREF(delB);
    return (ks_obj)aC;
}






// now, export them all
static ks_module get_module() {

    // create enum

    nx_enum_dtype = ks_Enum_create_c("dtype", (struct ks_enum_entry_c[]){
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

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* constants */

        {"array",                 (ks_obj)nx_type_array},
        {"dtype",                 (ks_obj)nx_enum_dtype},

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
