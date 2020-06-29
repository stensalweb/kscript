/* src/set.c - setting routines for nx
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


struct my_setelem_1d_data {

    // the element that it is supposed to be set to
    nx_any_t target_elem;


    // sizeof(target_elem)
    nx_size_t target_elem_size;

};


// internal 1D loop for setting 'data'
static int my_setelem_1d(int Nin, void** datas, enum nx_dtype* dtypes, nx_size_t* dtype_sizes, nx_size_t dim, nx_size_t* strides, void* _user_data) {
    NX_ASSERT_CHECK(Nin == 1);

    // convert to recognizable type
    struct my_setelem_1d_data* user_data = _user_data;

    // set it as blocks
    nx_memset_block(datas[0], (void*)&user_data->target_elem, dtype_sizes[0], strides[0] * dtype_sizes[0], dim);

    // success
    return 0;
}

// set all elements to the correct type
bool nx_T_set_all(void* data, enum nx_dtype dtype, int N, nx_size_t* dim, nx_size_t* stride, ks_obj obj) {

    // size of individual element
    int szof = nx_dtype_size(dtype);

    // now, cast 'obj' to this any_t
    nx_any_t obj_casted;

    // attempt to cast it
    if (!nx_cast_to(obj, dtype,&obj_casted)) return false;

    // now, copy it in every where
    struct my_setelem_1d_data user_data = (struct my_setelem_1d_data){
        .target_elem = obj_casted,
        .target_elem_size = szof
    };

    int stat = nx_T_apply_ufunc(1, &data, &dtype, &N, &dim, &stride, my_setelem_1d, (void*)&user_data);

    return stat == 0;
}


