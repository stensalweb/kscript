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

