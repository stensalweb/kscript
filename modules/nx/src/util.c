/* src/util.c - basic utilities for the nx library
 *
 *
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "../nx-impl.h"

// get by name
enum nx_dtype nx_dtype_get(char* name) {

    int sl = strlen(name);

    // macro for string matching
    #define ISS(_str) (sl == ((sizeof(_str)) - 1) && strncmp(_str, name, sl) == 0)

    if (ISS("int8") || ISS("char")) {
        return NX_DTYPE_SINT8;
    } else if (ISS("uint8") || ISS("uchar")) {
        return NX_DTYPE_UINT8;
    } else if (ISS("int16") || ISS("short")) {
        return NX_DTYPE_SINT16;
    } else if (ISS("uint16") || ISS("ushort")) {
        return NX_DTYPE_UINT16;
    } else if (ISS("int32") || ISS("int") || ISS("i")) {
        return NX_DTYPE_SINT32;
    } else if (ISS("uint32") || ISS("uint")) {
        return NX_DTYPE_UINT32;
    } else if (ISS("int64") || ISS("long")) {
        return NX_DTYPE_SINT64;
    } else if (ISS("uint64") || ISS("ulong")) {
        return NX_DTYPE_UINT64;
    } else if (ISS("float32") || ISS("float") || ISS("f") || ISS("fp32")) {
        return NX_DTYPE_FP32;
    } else if (ISS("float64") || ISS("double") || ISS("d") || ISS("fp64")) {
        return NX_DTYPE_FP64;
    } else if (ISS("complex") || ISS("complex32") || ISS("cfp32")) {
        return NX_DTYPE_CPLX_FP32;
    } else if (ISS("dcomplex") || ISS("complex64") || ISS("cfp64")) {
        return NX_DTYPE_CPLX_FP64;
    } else {
        // throw an error
        ks_throw_fmt(ks_type_TypeError, "Unknown dtype specifier '%s'", name);
        return 0;
    }

    #undef ISS

}

// Return an enumeration object
// NOTE: Returns a new reference
KS_API ks_Enum nx_dtype_get_enum(enum nx_dtype dtype) {
    /*  */ if (dtype == NX_DTYPE_SINT8) {
        return (ks_Enum)KS_NEWREF(nx_SINT8);
    } else if (dtype == NX_DTYPE_UINT8) {
        return (ks_Enum)KS_NEWREF(nx_UINT8);
    } else if (dtype == NX_DTYPE_SINT16) {
        return (ks_Enum)KS_NEWREF(nx_SINT16);
    } else if (dtype == NX_DTYPE_UINT16) {
        return (ks_Enum)KS_NEWREF(nx_UINT16);
    } else if (dtype == NX_DTYPE_SINT32) {
        return (ks_Enum)KS_NEWREF(nx_SINT32);
    } else if (dtype == NX_DTYPE_UINT32) {
        return (ks_Enum)KS_NEWREF(nx_UINT32);
    } else if (dtype == NX_DTYPE_SINT64) {
        return (ks_Enum)KS_NEWREF(nx_SINT64);
    } else if (dtype == NX_DTYPE_UINT64) {
        return (ks_Enum)KS_NEWREF(nx_UINT64);
    } else if (dtype == NX_DTYPE_FP32) {
        return (ks_Enum)KS_NEWREF(nx_FP32);
    } else if (dtype == NX_DTYPE_FP64) {
        return (ks_Enum)KS_NEWREF(nx_FP64);
    } else if (dtype == NX_DTYPE_CPLX_FP32) {
        return (ks_Enum)KS_NEWREF(nx_CPLX_FP32);
    } else if (dtype == NX_DTYPE_CPLX_FP64) {
        return (ks_Enum)KS_NEWREF(nx_CPLX_FP64);
    } else {
        return ks_throw_fmt(ks_type_Error, "Given unknown dtype enum (int: %i)", (int)dtype);
    }
}


static char* _dtype_names[] = {
    [NX_DTYPE_NONE]         = "error-type",

    [NX_DTYPE_SINT8]        = "sint8",
    [NX_DTYPE_UINT8]        = "uint8",
    [NX_DTYPE_SINT16]       = "sint16",
    [NX_DTYPE_UINT16]       = "uint16",
    [NX_DTYPE_SINT32]       = "sint32",
    [NX_DTYPE_UINT32]       = "uint32",
    [NX_DTYPE_SINT64]       = "sint64",
    [NX_DTYPE_UINT64]       = "uint64",

    [NX_DTYPE_FP32]         = "fp32",
    [NX_DTYPE_FP64]         = "fp64",

    [NX_DTYPE_CPLX_FP32]    = "complex_fp32",
    [NX_DTYPE_CPLX_FP64]    = "complex_fp64",
};

// get name
char* nx_dtype_get_name(enum nx_dtype dtype) {
    return _dtype_names[dtype];
}

bool nx_get_nxar(ks_obj obj, nxar_t* nxar, ks_list refadd) {

    if (obj->type == nx_type_array) {
        *nxar = NXAR_ARRAY((nx_array)obj);
        return true;
    } else if (obj->type == nx_type_view) {
        *nxar = NXAR_VIEW((nx_view)obj);
        return true;
    } else if (ks_num_is_numeric(obj) || ks_is_iterable(obj)) {
        // convert to object
        nx_array new_arr = nx_array_from_obj(obj, NX_DTYPE_NONE);
        if (!new_arr) return false;

        *nxar = NXAR_ARRAY(new_arr);
        ks_list_push(refadd, (ks_obj)new_arr);
        return false;
    } else {
        ks_throw_fmt(ks_type_TypeError, "Cannot create nxar_t from type '%T'", obj);
        return false;
    }
}


