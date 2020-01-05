/* util.c - utility functions are defined here */

#include "ks.h"

#include<signal.h>
#include<stdio.h>


// time value recording when the library was initialized
static struct timeval ks_stime = (struct timeval){ .tv_sec = 0, .tv_usec = 0 };

// handle segfaults
static void ks_segfault_handle(int sg) {
    fprintf(stderr, "SEGFAULT!");

    kse_dumpall();

    exit(1);
}


/* RANDOM NUMBER GENERATION */

// random state
static uint64_t random_state[4] = { 
    (uint64_t)0, 
    (uint64_t)0, 
    (uint64_t)0, 
    (uint64_t)0 
};

// performs one forward pass on the random state
static inline void random_fwd() {

    // some nice xoring to quickly and efficiently scatter values
    // this uses an xorshf (xor+shift) calculations
    random_state[0] ^= random_state[0] << 11;
    random_state[0] ^= random_state[0] >> 8;
    random_state[0] ^= random_state[0] << 1;

    // scoot them all over. TODO: also make a 4fwd pass that replaces all 4 values of the state
    random_state[3] = random_state[0];
    random_state[0] = random_state[1];
    random_state[1] = random_state[2];

    random_state[2] = random_state[3] ^ random_state[0] ^ random_state[1];
}

// performs a whole forward pass, replacing all 4 values of the random state
static inline void random_4fwd() {
    random_fwd();
    random_fwd();
    random_fwd();
    random_fwd();
}



// seeds the random number generator with a given value
static void random_seed(uint64_t value) {
    random_state[0] = random_state[2] = value;

    // 4 complete forward passes
    random_4fwd();
    random_4fwd();
    random_4fwd();
    random_4fwd();

}



// main initialization method
void ks_init() {

    // register this function to take care of segmentation faults
    signal(SIGSEGV, ks_segfault_handle);

    // record the start time
    gettimeofday(&ks_stime, NULL);

    // initialize objects
    kso_init();

    // seed the random number generator
    uint64_t seed = ((uint64_t)time(NULL));
    random_seed(seed + seed << 17);

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

// generate a random 64 bit integer
int64_t ks_random_i64() {
    // update values
    random_fwd();
    // pick a new value out of the state
    return (int64_t)random_state[0];
}


