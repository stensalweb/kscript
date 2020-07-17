/* ks.c - source code for the binary executable `ks`
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


int main(int argc, char** argv) {
    if (!ks_init()) return -1;

    // now, do stuff with kscript
    ks_int x = ks_int_new_s("123", 10);
    if (!x) ks_exit_if_err();

    //printf("%i\n", (int)x->v64);

    ks_obj ret = ks_obj_call((ks_obj)ks_F_print, 1, (ks_obj[]){ (ks_obj)x });
    if (!ret) ks_exit_if_err();

    KS_DECREF(ret);

    KS_DECREF(x);

}


