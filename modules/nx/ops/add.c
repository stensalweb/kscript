/* modules/nx/ops/add.c - the elementwise 'add' operation
 *
 * 
 * @author: Cade Brown
 */

#include "../nx-impl.h"

#define OP_NAME "add"



// the 'ufunc' for vectorized adding of arrays
// ARGS: A B D
// CALC: D[0:len] = A[0:len] + B[0:len]
static int nx_op_add_1d(int n_args, int len, nx_dtype* dtypes, void** args, ks_ssize_t* strides) {

    assert(n_args == 3 && "nx.internal: given incorrect number of arguments!");

    // get the data type for each argument
    nx_dtype Ad = dtypes[0], Bd = dtypes[1], Dd = dtypes[2];

    // get a 'uintptr' so we can easily do math with them
    uintptr_t Ap = (uintptr_t)args[0], Bp = (uintptr_t)args[1], Dp = (uintptr_t)args[2];

    // get amount to add to the pointer each iteration
    ks_ssize_t
        As = strides[0] * nx_dtype_sizeof(Ad),
        Bs = strides[1] * nx_dtype_sizeof(Bd),
        Ds = strides[2] * nx_dtype_sizeof(Dd);

    // number of items
    int i, N = len;

    // and go!
    // TODO: add fast paths for all C-types
    for (i = 0; i < N; ++i, Ap += As, Bp += Bs, Dp += Ds) {
        // most generic loop, but inefficient:

        // convert A[i] and B[i] to jscruot ibhects
        ks_obj Ai = nx_bin_get(Ad, (void*)Ap);
        if (!Ai) return 1;
        ks_obj Bi = nx_bin_get(Bd, (void*)Bp);
        if (!Bi) { KS_DECREF(Ai); return 1; }

        // do operator overloading:
        ks_obj Di = ks_F_add->func(2, (ks_obj[]){Ai, Bi});
        KS_DECREF(Ai);
        KS_DECREF(Bi);

        // ensure this succeeded
        if (!Di) return 1;

        // attempt to convert back to binary
        if (!nx_bin_set(Di, Dd, (void*)Dp)) {
            KS_DECREF(Di);
            return 1;
        }

        KS_DECREF(Di);
    }

    // success
    return 0;
}

// set 'D = A + B', elementwise
int nx_op_add(
    // 'A' variable
    nx_dtype A_dt, void* A_p, int A_Ndim, ks_ssize_t* A_dims, ks_ssize_t* A_strides,
    // 'B' variable
    nx_dtype B_dt, void* B_p, int B_Ndim, ks_ssize_t* B_dims, ks_ssize_t* B_strides,
    // 'dest' variable
    nx_dtype D_dt, void* D_p, int D_Ndim, ks_ssize_t* D_dims, ks_ssize_t* D_strides
) {
    // first, do size check
    if (D_Ndim < A_Ndim || D_Ndim < B_Ndim) {
        // error
        ks_throw_fmt(ks_type_SizeError, "Invalid sizes for: add(A=[%+,z], B=[%+,z], D=[%+,z])", A_Ndim, A_dims, B_Ndim, B_dims, D_Ndim, D_dims);
        return 1;
    }

    // do specific check
    if (!nx_bcast_check(3, 
        (int[]){ A_Ndim, B_Ndim, D_Ndim }, 
        (ks_ssize_t*[]){ A_dims, B_dims, D_dims })) {

        ks_throw_fmt(ks_type_SizeError, "Invalid sizes for: add(A=[%+,z], B=[%+,z], D=[%+,z])", A_Ndim, A_dims, B_Ndim, B_dims, D_Ndim, D_dims);
        return 1;
    }

    // attempt to broadcast it
    return nx_bcast(nx_op_add_1d, 3, 
        (nx_dtype[]){ A_dt, B_dt, D_dt }, 
        (void*[]){ A_p, B_p, D_p }, 
        (int[]){ A_Ndim, B_Ndim, D_Ndim }, 
        (ks_ssize_t*[]){ A_dims, B_dims, D_dims }, 
        (ks_ssize_t*[]){ A_strides, B_strides, D_strides }
    );
}


// external function
KS_TFUNC(nx, add) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 2);
    nx_array A = (nx_array)args[0];
    KS_REQ_TYPE(A, nx_type_array, "A");

    ks_obj B = args[1];
    if (B->type == nx_type_array) {
        // we have an array
        nx_array Barr = (nx_array)B;

        nx_dtype rAB = nx_dtype_opres(A->dtype, Barr->dtype);
        if (!rAB) return NULL;

        nx_array D = nx_array_new(A->Ndim, A->dims, NULL, rAB);
        if (!D) return NULL;

        if (nx_op_add(
            A->dtype, A->data_ptr, A->Ndim, A->dims, A->strides,
            Barr->dtype, Barr->data_ptr, Barr->Ndim, Barr->dims, Barr->strides,
            D->dtype, D->data_ptr, D->Ndim, D->dims, D->strides
        ) != 0) {
            KS_DECREF(D);
            return NULL;
        }
        return (ks_obj)D;

    } else if (ks_is_iterable(B)) {
        nx_array Barr = (nx_array)ks_call((ks_obj)nx_type_array, 1, &B);
        if (!Barr) return NULL;

        // now, attempt to add it
        nx_dtype rAB = nx_dtype_opres(A->dtype, Barr->dtype);
        if (!rAB) return NULL;

        nx_array D = nx_array_new(A->Ndim, A->dims, NULL, rAB);
        if (!D) return NULL;

        // now, try and add it
        if (nx_op_add(
            A->dtype, A->data_ptr, A->Ndim, A->dims, A->strides,
            Barr->dtype, Barr->data_ptr, Barr->Ndim, Barr->dims, Barr->strides,
            D->dtype, D->data_ptr, D->Ndim, D->dims, D->strides
        ) != 0) {
            KS_DECREF(D);
            return NULL;
        }

        KS_DECREF(Barr);
        return (ks_obj)D;

    } else {
        // otherwise, attempt to convert to an array

        nx_dtype detec = nx_dtype_obj(B);
        if (!detec) return NULL;

        ks_ssize_t sizeof_detec = nx_dtype_sizeof(detec);
        if (sizeof_detec < 0) return NULL;

        // now, initialize just one element
        void* B0 = ks_malloc(sizeof_detec);

        // attempt to convert to a dtype
        if (!nx_bin_set(B, detec, B0)) {
            ks_free(B0);
            return NULL;
        }

        // now, attempt to add it
        nx_dtype rAB = nx_dtype_opres(A->dtype, detec);
        if (!rAB) return NULL;

        nx_array D = nx_array_new(A->Ndim, A->dims, NULL, rAB);
        if (!D) return NULL;

        // now, try and add it
        if (nx_op_add(
            A->dtype, A->data_ptr, A->Ndim, A->dims, A->strides,
            detec, B0, 1, (ks_ssize_t[]){ 1 }, (ks_ssize_t[]){ 0 },
            D->dtype, D->data_ptr, D->Ndim, D->dims, D->strides
        ) != 0) {
            KS_DECREF(D);
            return NULL;
        }

        ks_free(B0);
        return (ks_obj)D;
    }
}


