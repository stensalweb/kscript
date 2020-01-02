/* util.c - utility functions are defined here */

#include "ks.h"


// time value recording when the library was initialized
static struct timeval ks_stime = (struct timeval){ .tv_sec = 0, .tv_usec = 0 };


void ks_init() {

    // record the start time
    gettimeofday(&ks_stime, NULL);

    // initialize objects
    kso_init();

    // initialize builtin functions
    ksf_init();

    // initialize the error system
    kse_init();

}

// returns time in seconds
double ks_time() {
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    return (curtime.tv_sec - ks_stime.tv_sec) + 1.0e-6 * (curtime.tv_usec - ks_stime.tv_usec);
}


// returns a hash from some bytes
uint64_t ks_hash_bytes(uint8_t* chr, int len) {
    uint64_t ret = 7;
    int i;
    for (i = 0; i < len; ++i) {
        ret = ret * 31 + chr[i];
    }
    return ret;
}
