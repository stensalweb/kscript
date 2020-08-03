/* src/set.c - setting routines for nx
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


struct my_setelem_1d_data {

    // the element that it is supposed to be set to
    void* target_elem;

};


static bool my_set_all_1d(int N, nxar_t* arrs, int len, void* _user_data) {
    NX_ASSERT_CHECK(N == 1);

    // convert to recognizable type
    struct my_setelem_1d_data* data = _user_data;

    // loop vars
    nx_size_t i;

    // data pointers
    intptr_t dptr_0 = (intptr_t)arrs[0].data;
    nx_size_t strd_0 = arrs[0].stride[0];


    // set all elements
    for (i = 0; i < len; ++i, dptr_0 += strd_0) {
        memcpy((void*)dptr_0, data->target_elem, arrs[0].dtype->size);
    }

    return true;
}


// set all elements to the correct type
bool nx_T_set_all(nxar_t A, ks_obj obj) {

    void* obj_casted = ks_malloc(A.dtype->size);

    // attempt to cast it
    if (!nx_cast_to(obj, A.dtype, obj_casted)) return false;

    // now, copy it in every where
    struct my_setelem_1d_data user_data = (struct my_setelem_1d_data){
        .target_elem = obj_casted
    };

    bool rst = nx_T_ufunc_apply(1, &A, my_set_all_1d, (void*)&user_data);

    ks_free(obj_casted);

    return rst;
}


