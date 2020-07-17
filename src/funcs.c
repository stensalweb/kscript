/* funcs.c - implementation of standard/builtin functions
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


ks_cfunc
    ks_F_print = NULL

;


// print (*args) -> print arguments
static KS_FUNC(print) {
    int n_extra;
    ks_obj* extra;
    if (!ks_getargs(n_args, args, "*args", &n_extra, &extra)) return false;

    ks_str_builder sb = ks_str_builder_new();

    int i;
    for (i = 0; i < n_extra; ++i) {
        if (i > 0) ks_str_builder_add(sb, " ", 1);
        if (!ks_str_builder_add_str(sb, extra[i])) {
            KS_DECREF(sb);
            return NULL;
        }
    }

    ks_str toprint = ks_str_builder_get(sb);
    KS_DECREF(sb);

    // print out to console
    printf("%s\n", toprint->chr);
    KS_DECREF(toprint);

    return KSO_NONE;
}




// export

void ks_init_funcs() {

    ks_F_print = ks_cfunc_new_c(print_, "print(*args)");

}

