/* src/util.c - basic utilities for the nx library
 *
 *
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "../nx-impl.h"


bool nx_get_nxar(ks_obj obj, nxar_t* nxar, ks_list refadd) {

    if (obj->type == nx_T_array) {
        *nxar = NXAR_ARRAY((nx_array)obj);
        return true;
    } else if (obj->type == nx_T_view) {
        *nxar = NXAR_VIEW((nx_view)obj);
        return true;
    } else if (ks_num_is_numeric(obj) || ks_obj_is_iterable(obj)) {
        // convert to object
        nx_array new_arr = nx_array_from_obj(obj, NX_DTYPE_KIND_NONE);
        if (!new_arr) return false;

        *nxar = NXAR_ARRAY(new_arr);
        ks_list_push(refadd, (ks_obj)new_arr);
        return true;
    } else {
        ks_throw(ks_T_TypeError, "Cannot create nxar_t from type '%T'", obj);
        return false;
    }
}


ks_obj nx_nxar_getitem(nxar_t nxar, int N, ks_obj* idxs) {

    if (N == 0) {
        // return view of the entire array
        return (ks_obj)nx_view_new(nxar.src_obj, nxar);
    } else if (N <= nxar.rank) {

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

        if (!needSlice && N == nxar.rank) {
            // return single element
            void* addr = nx_get_ptr(nxar.data, N, nxar.dim, nxar.stride, idxis);
            ks_free(idxis);

            return nx_cast_from(nxar.dtype, addr);
        } else {

            // result dimension is the number of indexes which were not integers + those not referenced
            int rN = nxar.rank - N;
            for (i = 0; i < N; ++i) if (idxis[i] < 0) rN++;

            // otherwise, calculate slices & return view
            nx_size_t* dim = ks_malloc(sizeof(*dim) * rN);
            nx_size_t* stride = ks_malloc(sizeof(*stride) * rN);

            // offset from the base pointer
            nx_size_t total_offset = 0;


            // return index
            int ri = 0;

            for (i = 0; i < N; ++i) {


                if (idxis[i] >= 0) {
                    // single index here, just bump the array off
                    total_offset += idxis[i] * nxar.stride[i];

                    //dim[ri] = self->dim[i];
                    //stride[ri] = self->stride[i];

                    //ri++;
                } else if (idxs[i]->type == ks_T_slice) {
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

                        return ks_throw(ks_T_TodoError, "Need to determine what to return if size is 0 in a dimension");
                    }

                    // there will be this many
                    dim[ri] = num;

                    // stride offset
                    stride[ri] = nxar.stride[i] * delta;

                    // and add total offset to the first
                    total_offset += first * nxar.stride[i];

                    // claim this dimension
                    ri++;
                } else {
                    ks_free(idxis);
                    ks_free(dim);
                    ks_free(stride);
                    return ks_throw(ks_T_TypeError, "Expected all indices to be either 'int' or 'slice', but got '%T'", idxs[i]);

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
                    .rank = rN,
                    .dim = dim,
                    .stride = stride
                }
            );
            ks_free(dim);
            ks_free(stride);
            return (ks_obj)ret;
        }

    } else {
        return ks_throw(ks_T_KeyError, "nx.array[...] expected %i indices (for %iD array), but only got %i", nxar.rank, nxar.rank, N);
    }

}


bool nx_nxar_setitem(nxar_t nxar, int N, ks_obj* idxs, ks_obj obj) {
    if (N == 0) {
        // set entire array
        return nx_T_set_all(nxar, obj);
    } else if (N <= nxar.rank) {

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

        if (false && !needSlice && N == nxar.rank) {
            // set single value
            void* addr = nx_get_ptr(nxar.data, N, nxar.dim, nxar.stride, idxis);
            ks_free(idxis);

            // set it
            return nx_T_set_all((nxar_t){
                .data = addr,
                .dtype = nxar.dtype,
                .rank = 1,
                .dim = (nx_size_t[]){ 1 },
                .stride = (nx_size_t[]){ 0 },
            }, obj);

        } else {

            // result dimension is the number of indexes which were not integers + those not referenced
            int rN = nxar.rank - N;
            for (i = 0; i < N; ++i) if (idxis[i] < 0) rN++;

            // otherwise, calculate slices & return view
            nx_size_t* dim = ks_malloc(sizeof(*dim) * rN);
            nx_size_t* stride = ks_malloc(sizeof(*stride) * rN);

            // offset from the base pointer
            nx_size_t total_offset = 0;


            // return index
            int ri = 0;

            for (i = 0; i < N; ++i) {


                if (idxis[i] >= 0) {
                    // single index here, just bump the array off
                    total_offset += idxis[i] * nxar.stride[i];

                    //dim[ri] = self->dim[i];
                    //stride[ri] = self->stride[i];

                    //ri++;
                } else if (idxs[i]->type == ks_T_slice) {
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

                        return ks_throw(ks_T_TodoError, "Need to determine what to return if size is 0 in a dimension");
                    }

                    // there will be this many
                    dim[ri] = num;

                    // stride offset
                    stride[ri] = nxar.stride[i] * delta;

                    // and add total offset to the first
                    total_offset += first * nxar.stride[i];

                    // claim this dimension
                    ri++;
                } else {
                    ks_free(idxis);
                    ks_free(dim);
                    ks_free(stride);
                    return ks_throw(ks_T_TypeError, "Expected all indices to be either 'int' or 'slice', but got '%T'", idxs[i]);

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

            // the slice being assigned to
            nxar_t nxar_slice = (nxar_t){
                .data = (void*)((intptr_t)nxar.data + total_offset),
                .dtype = nxar.dtype,
                .rank = rN,
                .dim = dim,
                .stride = stride
            };

            // values to set them
            nxar_t vals;
            ks_list refs = ks_list_new(0, NULL);
            if (!nx_get_nxar(obj, &vals, refs)) {
                ks_free(dim);
                ks_free(stride);
                KS_DECREF(refs);
                return false;
            }

            // now, cast into

            if (!nx_T_cast(vals, nxar_slice)) {
                KS_DECREF(refs);
                ks_free(dim);
                ks_free(stride);
                return false;
            }


            // success

            KS_DECREF(refs);
            ks_free(dim);
            ks_free(stride);
            return true;
        }

    } else {
        ks_throw(ks_T_KeyError, "nx.array[...]=val expected %i indices (for %iD array), but only got %i", nxar.rank, nxar.rank, N);
        return false;
    }

}


