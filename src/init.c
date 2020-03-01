/* init.c - holds code for the initialization of the kscript library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com> 
 */

#include "ks-impl.h"


// get the start time (initialize it in 'ks_init')
static struct timeval ks_start_time = (struct timeval){ .tv_sec = 0, .tv_usec = 0 };

// initialize the whole library
bool ks_init() {
    gettimeofday(&ks_start_time, NULL);

    // initialize the builtin types
    ks_type_type_init();
    ks_type_none_init();
    ks_type_bool_init();
    ks_type_int_init();
    ks_type_str_init();
    ks_type_list_init();
    ks_type_dict_init();

    ks_type_cfunc_init();

    // success
    return true;
}


// return the time since it started
double ks_time() {
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    return (curtime.tv_sec - ks_start_time.tv_sec) + 1.0e-6 * (curtime.tv_usec - ks_start_time.tv_usec);
}

