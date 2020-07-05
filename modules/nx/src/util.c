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
        return true;
    } else {
        ks_throw_fmt(ks_type_TypeError, "Cannot create nxar_t from type '%T'", obj);
        return false;
    }
}


ks_obj nx_nxar_getitem(nxar_t nxar, int N, ks_obj* idxs) {

    if (N == 0) {
        // return view of the entire array
        return (ks_obj)nx_view_new(nxar.src_obj, nxar);
    } else if (N <= nxar.N) {

        // the index, or -1 if we need to calculate a slice
        int64_t* idxis = ks_malloc(N * sizeof(*idxs));

        // whether or not slices are required
        bool needSlice = false;

        int i;
        for (i = 0; i < N; ++i) {
            if (!ks_num_get_int64(idxs[i], &idxis[i])) {
                // not found as int
                ks_catch_ignore();
                idxis[i] = -1;
                needSlice = true;
            } else {

                // wrap around
                idxis[i] = ((idxis[i] % nxar.dim[i]) + nxar.dim[i]) % nxar.dim[i];
            }
        }

        if (!needSlice && N == nxar.N) {
            // return single element
            void* addr = nx_get_ptr(nxar.data, nx_dtype_size(nxar.dtype), N, nxar.dim, nxar.stride, idxis);
            ks_free(idxs);

            return nx_cast_from(nxar.dtype, addr);
        } else {

            // result dimension is the number of indexes which were not integers + those not referenced
            int rN = nxar.N - N;
            for (i = 0; i < N; ++i) if (idxis[i] < 0) rN++;

            // otherwise, calculate slices & return view
            nx_size_t* dim = ks_malloc(sizeof(*dim) * rN);
            nx_size_t* stride = ks_malloc(sizeof(*stride) * rN);

            // offset from the base pointer
            nx_size_t total_offset = 0;

            // dtype size
            nx_size_t dtsz = nx_dtype_size(nxar.dtype);


            // return index
            int ri = 0;

            for (i = 0; i < N; ++i) {


                if (idxis[i] >= 0) {
                    // single index here, just bump the array off
                    total_offset += dtsz * idxis[i] * nxar.stride[i];

                    //dim[ri] = self->dim[i];
                    //stride[ri] = self->stride[i];

                    //ri++;
                } else if (idxs[i]->type == ks_type_slice) {
                    // add dimension from slice argument
                    ks_slice arg_slice = (ks_slice)idxs[i];

                    int64_t first, last, delta;

                    if (!ks_slice_getci(arg_slice, nxar.dim[i], &first, &last, &delta)) {

                        // error
                        ks_free(idxis);
                        ks_free(dim);
                        ks_free(stride);

                        return NULL;
                    }

                    // otherwise, calculate number of elements
                    int num = (last - first) / delta;
                    if (num == 0) {
                        // TODO: determine what to do
                        ks_free(idxis);
                        ks_free(dim);
                        ks_free(stride);

                        return ks_throw_fmt(ks_type_ToDoError, "Need to determine what to return if size is 0 in a dimension");
                    }

                    // there will be this many
                    dim[ri] = num;

                    // stride offset
                    stride[ri] = nxar.stride[i] * delta;

                    // and add total offset to the first
                    total_offset += dtsz * first * nxar.stride[i];

                    // claim this dimension
                    ri++;
                } else {
                    ks_free(idxis);
                    ks_free(dim);
                    ks_free(stride);
                    return ks_throw_fmt(ks_type_TypeError, "Expected all indices to be either 'int' or 'slice', but got '%T'", idxs[i]);

                }
            }
            
            ks_free(idxis);

            // fill out the rest, copying
            while (ri < rN) {

                dim[ri] = nxar.dim[i];
                stride[ri] = nxar.stride[i];

                i++;
                ri++;
            }

            nx_view ret = nx_view_new(nxar.src_obj, 
                (nxar_t){
                    .data = (void*)((intptr_t)nxar.data + total_offset),
                    .dtype = nxar.dtype,
                    .N = rN,
                    .dim = dim,
                    .stride = stride
                }
            );
            ks_free(dim);
            ks_free(stride);
            return (ks_obj)ret;
        }

    } else {
        return ks_throw_fmt(ks_type_KeyError, "nx.array[...] expected %i indices (for %iD array), but only got %i", nxar.N, nxar.N, N);
    }

}
