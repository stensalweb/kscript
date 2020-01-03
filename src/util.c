/* util.c - utility functions are defined here */

#include "ks.h"

#include<signal.h>
#include<stdio.h>


// time value recording when the library was initialized
static struct timeval ks_stime = (struct timeval){ .tv_sec = 0, .tv_usec = 0 };

// handle segfaults
static void ks_segfault_handle(int sg) {
    ks_error("SEGFAULT");

    kse_dumpall();

    exit(1);
}
void ks_init() {

    //signal(SIGSEGV, ks_segfault_handle);

    // record the start time
    gettimeofday(&ks_stime, NULL);

    // initialize objects
    kso_init();

    /* initialize types */
    ks_init__type();
    ks_init__none();
    ks_init__bool();
    ks_init__int();
    ks_init__str();
    ks_init__tuple();
    ks_init__list();
    ks_init__dict();
    ks_init__code();
    ks_init__kfunc();
    ks_init__cfunc();
    ks_init__parser();
    ks_init__ast();

    // initialize builtin functions
    ksf_init();

    // initialize the error system
    kse_init();

    // initialize execution engine
    ks_init__EXEC();

}

// returns time in seconds
double ks_time() {
    struct timeval curtime;
    gettimeofday(&curtime, NULL);
    return (curtime.tv_sec - ks_stime.tv_sec) + 1.0e-6 * (curtime.tv_usec - ks_stime.tv_usec);
}

