/* src/loopfunc.c - implement N dimensional loops
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"


bool nx_T_apply_loop(int loop_N, nx_size_t* loop_dim, nx_loopfunc_f loopfunc, void* _user_data) {

    int i;
    for (i = 0; i < loop_N; ++i) if (loop_dim[i] == 0) return true;


    // allocate indexes
    nx_size_t* idx = ks_malloc(sizeof(*idx) * loop_N);

    // zero them out
    for (i = 0; i < loop_N; ++i) idx[i] = 0;

    while (true) {
        // now, call the function with current indexes
        if (!loopfunc(loop_N, loop_dim, idx, _user_data)) {
            ks_free(idx);
            return false;
        }


        // increase least significant index
        i = loop_N - 1;
        if (i < 0) break;
        idx[i]++;

        while (i >= 0 && idx[i] >= loop_dim[i]) {
            idx[i] = 0;
            i--;
            if (i >= 0) idx[i]++;
        }

        // done; we have overflowed
        if (i < 0) break;

    }

    // success
    ks_free(idx);
    return true;
}

