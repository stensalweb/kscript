/* fft/fft_1d.c - implementation of the 1D FFT
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


// FFT data structure for multi-loop iteration
struct my_loop_fft_data {

    //plan for executing inner loop
    nx_fft_plan_t* plan;

    void* Ad, *Bd;

    enum nx_dtype Adtype, Bdtype;

    // how big is the loop?
    int loop_N;

    // stride of the loop
    nx_size_t* lp_A_stride;
    nx_size_t* lp_B_stride;


    // FFT dimension
    int fft_N;

    // FFT sizes
    nx_size_t* fft_dim;

    // strides for A
    nx_size_t* fft_A_stride;
    nx_size_t* fft_B_stride;

    // temporary array
    nxar_t Ra;

};


static bool my_loop_fft(int loop_N, nx_size_t* loop_dim, nx_size_t* idx, void* _user_data) {
    struct my_loop_fft_data* data = (struct my_loop_fft_data*)_user_data;

    // get pointer to start of this thing
    void* dp_A = nx_get_ptr(data->Ad, nx_dtype_size(data->Adtype), loop_N, loop_dim, data->lp_A_stride, idx);
    void* dp_B = nx_get_ptr(data->Bd, nx_dtype_size(data->Bdtype), loop_N, loop_dim, data->lp_B_stride, idx);

    nxar_t Ar = (nxar_t){
        .data = dp_A,
        .dtype = data->Adtype,
        .N = data->fft_N,
        .dim = data->fft_dim,
        .stride = data->fft_A_stride
    };

    nxar_t Br = (nxar_t){
        .data = dp_B,
        .dtype = data->Bdtype,
        .N = data->fft_N,
        .dim = data->fft_dim,
        .stride = data->fft_B_stride
    };

    // Now, fill 'R" with numbers from 'A'
    // TODO: allow multi-dimensional input, and then loop over the last dimension
    if (!nx_T_cast(Ar, data->Ra)) return false;


    // perform the computation
    if (!nx_fft_plan_do(data->plan, data->Ra.data)) return false;

    // now, set 'B' to the value, casted
    if (!nx_T_cast(data->Ra, Br)) return false;

    return true;
}

// Compute B = FFT(A)
bool nx_T_fft_Nd(nx_fft_plan_t* plan0, int N, int* axis, nxar_t A, nxar_t B) {

    int fft_N = N;
    int lp_N = A.N - fft_N;

    // stride of loop dimensions
    nx_size_t* lp_dim = ks_malloc(sizeof(*lp_dim) * lp_N);

    // strides
    nx_size_t* lp_A_stride = ks_malloc(sizeof(*lp_A_stride) * lp_N);
    nx_size_t* lp_B_stride = ks_malloc(sizeof(*lp_B_stride) * lp_N);

    // FFT dimensions
    nx_size_t* fft_dim = ks_malloc(sizeof(*fft_dim) * fft_N);

    // strides
    nx_size_t* fft_A_stride = ks_malloc(sizeof(*fft_A_stride) * fft_N);
    nx_size_t* fft_B_stride = ks_malloc(sizeof(*fft_B_stride) * fft_N);


    int i, j, fi = 0, ri = 0;
    for (i = 0; i < A.N; ++i) {

        bool isAxis = false;
        for (j = 0; j < N; ++j) {
            if (axis[j] == i) {
                isAxis = true;
                break;
            }
        }
    
        if (isAxis) {
            
            // in FFT dims
            fft_dim[fi] = A.dim[i];
            fft_A_stride[fi] = A.stride[i];
            fft_B_stride[fi] = B.stride[i];
            fi++;

        } else {

            // add to loop dimensions
            lp_dim[ri] = A.dim[i];
            lp_A_stride[ri] = A.stride[i];
            lp_B_stride[ri] = B.stride[i];
            ri++;

        }
    }

    struct my_loop_fft_data my_data;
    my_data.plan = plan0;
    my_data.loop_N = lp_N;

    my_data.Ad = A.data;
    my_data.Bd = B.data;

    my_data.Adtype = A.dtype;
    my_data.Bdtype = B.dtype;

    my_data.lp_A_stride = lp_A_stride;
    my_data.lp_B_stride = lp_B_stride;

    my_data.fft_N = fft_N;

    my_data.fft_dim = fft_dim;
    my_data.fft_A_stride = fft_A_stride;
    my_data.fft_B_stride = fft_B_stride;

    nx_size_t Rsz = 1;
    for (i = 0; i < fft_N; ++i) Rsz *= fft_dim[i];

    double complex* Rdata = ks_malloc(sizeof(*Rdata) * Rsz);

    nx_size_t* Rstride = ks_malloc(sizeof(*Rstride) * fft_N);
    Rstride[fft_N - 1] = 1;
    for (i = fft_N - 2; i >= 0; --i) Rstride[i] = Rstride[i + 1] * fft_dim[i + 1];

  
    my_data.Ra = (nxar_t){
        .data = Rdata,
        .dtype = NX_DTYPE_CPLX_FP64,
        .N = fft_N,
        .dim = fft_dim,
        .stride = Rstride
    };

    bool ret = nx_T_apply_loop(my_data.loop_N, lp_dim, my_loop_fft, (void*)&my_data);

    ks_free(lp_dim);
    ks_free(lp_A_stride);
    ks_free(lp_B_stride);

    ks_free(fft_dim);
    ks_free(fft_A_stride);
    ks_free(fft_B_stride);

    ks_free(Rdata);
    ks_free(Rstride);

    return ret;
}



// 1D wrapper
bool nx_T_fft_1d(nx_fft_plan_t* plan0, int axis0, nxar_t A, nxar_t B)  {
    return nx_T_fft_Nd(plan0, 1, (int[]){ axis0 }, A, B);
}
// 2D wrapper
bool nx_T_fft_2d(nx_fft_plan_t* plan0, int axis0, int axis1, nxar_t A, nxar_t B)  {
    return nx_T_fft_Nd(plan0, 2, (int[]){ axis0, axis1 }, A, B);
}





