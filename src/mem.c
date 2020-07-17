/* mem.c - memory-related functions
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// TODO: also support other implementations of malloc
#define KS_MALLOC malloc
#define KS_REALLOC realloc
#define KS_FREE free

// Allocate a block of memory guaranteed to hold at least `sz` bytes
// This pointer should only be used with `ks_realloc()` and `ks_free()` and `ks_*` functions!
// NOTE: Returns `NULL` and throws an error if there was a problem
void* ks_malloc(ks_size_t sz) {

    void* ret = KS_MALLOC(sz);


    return ret;

}

// Allocate a block of memory of size `n * sz`, i.e. `n` elements of size `sz`
// NOTE: Returns `NULL` and throws an error if there was a problem
void* ks_calloc(ks_size_t n, ks_size_t sz) {
    return ks_malloc(n * sz);
}

// Attempt to reallocate a given pointer to fit at least `sz` bytes, and return the new pointer
// NOTE: Returns `NULL` and throws an error if there was a problem, but the original pointer is not freed!
void* ks_realloc(void* ptr, ks_size_t sz) {

    // create a new pointer
    void* new_ptr = KS_REALLOC(ptr, sz);

    // return it
    return new_ptr;

}

// Free a pointer allocated by `ks_malloc`, `ks_calloc` or `ks_realloc`
void ks_free(void* ptr) {
    KS_FREE(ptr);
}


