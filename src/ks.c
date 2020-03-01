
#include "ks.h"


int main(int argc, char** argv) {

    if (!ks_init()) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }

    int64_t smem = ks_mem_cur();

    ks_dict d = ks_new_dict(0, NULL);

    ks_dict_set_cn(d, (ks_dict_ent_c[]){
        {"A", (ks_obj)ks_new_int(42)},
        {"B", (ks_obj)ks_new_int(43)},
        {"C", (ks_obj)ks_new_int(46)},
        {NULL, NULL}
    });

    KS_DECREF(d);

    ks_info("start: %l, cur: %l, max: %l", smem, ks_mem_cur(), ks_mem_max());

    return 0;
}
