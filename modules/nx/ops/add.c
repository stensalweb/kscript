/* modules/nx/ops/add.c - the elementwise 'add' operation
 *
 * 
 * @author: Cade Brown
 */

#include "nx-impl.h"

#define OP_NAME "add"




// 1 dimensional add array:
// n_args == 3, and the arguments are:
// A B D
// So, args[0]==A.data, args[1]==B.data, args[2]==D.data,
// and 'dims' and 'strides' are the dimensions of each
static int nx_op_add_1d(int n_args, nx_dtype* dtypes, uintptr_t* args, ks_ssize_t* dims, ks_ssize_t* strides) {

    assert(n_args == 3 && "nx.internal: given incorrect number of arguments!");

    // read in argumnts
    nx_dtype Ad = dtypes[0], Bd = dtypes[1], Dd = dtypes[2];
    uintptr_t Ap = args[0], Bp = args[1], Dp = args[2];

    // get amount to add to the pointer each iteration
    ks_ssize_t
        As = strides[0] * nx_dtype_sizeof(Ad), 
        Bs = strides[1] * nx_dtype_sizeof(Bd), 
        Ds = strides[2] * nx_dtype_sizeof(Dd);

    int i, N = dims[0];

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
    int A_Ndim, ks_ssize_t* A_dims, ks_ssize_t* A_strides, nx_dtype A_dt, void* A_p,
    // 'B' variable
    int B_Ndim, ks_ssize_t* B_dims, ks_ssize_t* B_strides, nx_dtype B_dt, void* B_p,
    // 'dest' variable
    int D_Ndim, ks_ssize_t* D_dims, ks_ssize_t* D_strides, nx_dtype D_dt, void* D_p
) {

    // first, do size check
    if (D_Ndim < A_Ndim || D_Ndim < B_Ndim) {
        // error
        ks_throw_fmt(ks_type_SizeError, "Invalid sizes for: add(A=[%+,z], B=[%+,z], D=[%+,z])", A_Ndim, A_dims, B_Ndim, B_dims, D_Ndim, D_dims);
        return 1;
    }

    // do specific check

    // attempt to broadcast it
    return nx_broadcast(nx_op_add_1d, 3, 
        (nx_dtype[]){ A_dt, B_dt, D_dt }, 
        (uintptr_t[]){ (uintptr_t)A_p, (uintptr_t)B_p, (uintptr_t)D_p }, 
        (int[]){ A_Ndim, B_Ndim, D_Ndim }, 
        (ks_ssize_t*[]){ A_dims, B_dims, D_dims }, 
        (ks_ssize_t*[]){ A_strides, B_strides, D_strides }
    );
}


// external function
KS_TFUNC(nx, add) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    nx_array A = (nx_array)args[0];
    KS_REQ_TYPE(A, nx_type_array, "A");
    nx_array B = (nx_array)args[1];
    KS_REQ_TYPE(B, nx_type_array, "B");

    nx_dtype rAB = nx_dtype_opres(A->dtype, B->dtype);
    if (!rAB) return NULL;

    nx_array D = nx_array_new(A->Ndim, A->dims, NULL, rAB);
    if (!D) return NULL;

    if (nx_op_add(
        A->Ndim, A->dims, A->strides, A->dtype, A->data_ptr,
        B->Ndim, B->dims, B->strides, B->dtype, B->data_ptr,
        D->Ndim, D->dims, D->strides, D->dtype, D->data_ptr
    ) != 0) {
        KS_DECREF(D);
        return NULL;
    }


    return (ks_obj)D;
}


