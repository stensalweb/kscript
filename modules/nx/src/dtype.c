/* modules/nx/dtype.c - data type functions
 *
 * 
 * @author: Cade Brown
 */

#include "nx-impl.h"

#include <ctype.h>

// return a dtype from a given C-string
nx_dtype nx_dtype_from_cstr(const char* name) {
    if (!name) return NX_DTYPE_ERR;

    int slen = strlen(name);

    char* lname = ks_malloc(slen + 1);
    int i;
    for (i = 0; i < slen; ++i) {
        lname[i] = tolower(name[i]);
    }

    lname[slen] = '\0';


    // macro for common substitutions
    #define CASE(_cstr, _dtype) else if (strncmp(lname, _cstr, sizeof(_cstr) - 1) == 0) { ks_free(lname); return _dtype; }

    if (false) {
    }

    // standard names
    CASE("si8"  ,   NX_DTYPE_SI8 )
    CASE("si16" ,   NX_DTYPE_SI16)
    CASE("si32" ,   NX_DTYPE_SI32)
    CASE("si64" ,   NX_DTYPE_SI64)

    CASE("ui8"  ,   NX_DTYPE_UI8 )
    CASE("ui16" ,   NX_DTYPE_UI16)
    CASE("ui32" ,   NX_DTYPE_UI32)
    CASE("ui64" ,   NX_DTYPE_UI64)

    CASE("fp32" ,   NX_DTYPE_FP32)
    CASE("fp64" ,   NX_DTYPE_FP64)

    // nick names
    CASE("char" ,   NX_DTYPE_SI8 )
    CASE("short",   NX_DTYPE_SI16)
    CASE("int"  ,   NX_DTYPE_SI32)
    CASE("long" ,   NX_DTYPE_SI64)

    CASE("uchar" ,   NX_DTYPE_UI8 )
    CASE("ushort",   NX_DTYPE_UI16)
    CASE("uint"  ,   NX_DTYPE_UI32)
    CASE("ulong" ,   NX_DTYPE_UI64)

    CASE("float" ,   NX_DTYPE_FP32)
    CASE("double" ,  NX_DTYPE_FP64)

    // not found
    ks_free(lname);
    return NX_DTYPE_ERR;

}


static const char* dtype_cstrs[] = {
    "err",
    "si8",
    "si16",
    "si32",
    "si64",
    
    "ui8",
    "ui16",
    "ui32",
    "ui64",

    "fp32",
    "fp64",
};


// Return a C-string name from a dtype
// NOTE: Do not free or modify the returned value!
const char* nx_dtype_to_cstr(nx_dtype dtype) {
    return dtype_cstrs[dtype];
}


// return the size (in bytes) of a datatype
ks_ssize_t nx_dtype_sizeof(nx_dtype dtype) {

    /**/ if (dtype == NX_DTYPE_SI8  || dtype == NX_DTYPE_UI8 ) return 1;
    else if (dtype == NX_DTYPE_SI16 || dtype == NX_DTYPE_UI16) return 2;
    else if (dtype == NX_DTYPE_SI32 || dtype == NX_DTYPE_UI32) return 4;
    else if (dtype == NX_DTYPE_SI64 || dtype == NX_DTYPE_UI64) return 8;
    
    else if (dtype == NX_DTYPE_FP32) return 4;
    else if (dtype == NX_DTYPE_FP64) return 8;
    else {
        return -1;
    }
}

// Get rule for 2 types
nx_dtype nx_dtype_opres(nx_dtype Ldt, nx_dtype Rdt) {
    if (Ldt == Rdt) {
        // always same in this case
        return Ldt;
    } else {
        ks_throw_fmt(ks_type_TypeError, "Cannot do operation between '%s' and '%s'", nx_dtype_to_cstr(Ldt), nx_dtype_to_cstr(Rdt));
        return NX_DTYPE_ERR;
    }
}
