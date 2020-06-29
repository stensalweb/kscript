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
    KS_REQ_N_ARGS(n_args, 3);
    nx_array A, B, C;
    if (!ks_parse_params(n_args, args, "A%* B%* C%*", &A, nx_type_array, &B, nx_type_array, &C, nx_type_array)) return NULL;

    // try to add them, if not throw an error
    if (!nx_T_add(
        _NXAR_(A),
        _NXAR_(B),
        _NXAR_(C)
    )) {
        return NULL;
    }

    return KS_NEWREF(C);
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
        
        {"add",                   (ks_obj)ks_cfunc_new2(nx_add_, "nx.add(A, B, C=none)")},

        /*{"Result",     (ks_obj)req_type_Result},

        {"GET",        (ks_obj)ks_cfunc_new2(req_GET_, "req.GET(url, data=none)")},
        {"POST",       (ks_obj)ks_cfunc_new2(req_POST_, "req.POST(url, data=none)")},
        {"download",   (ks_obj)ks_cfunc_new2(req_download_, "req.download(url, dest, data=none)")},*/

        {NULL, NULL}
    });

    ks_module_add_enum_members(mod, nx_enum_dtype);

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
