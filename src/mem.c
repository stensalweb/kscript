/* mem.c - memory management routines

Uses exponential reallocation for better performance,
instead of reallocing `size`, it reallocs `A*size+B`

1 <= A <= 2, typically
and
0 <= B <= 16, typically

Obviously, this uses slighly more memory, but only `A`x more memory.

Thus, with `A=1.25`, the system may use 125% of the actual amount required.
This is not a bad trade, for the speed increase, as well as the speedup from
resizing an array.

For example, take linearly resizing an array every so many bytes in a loop, gradually.

It would take `n` steps, where `a(n)=a(n-1)*A+B>=size`, which (discounting B), is:
`a(n)~a(0)*A^n+B*n`, and thus `n (~ or <) log(size)/log(A)`, if A > 1, else:
`n ~ size/B`. This reduces complexity from O(size/B*iter) to O(iter*log(size)/log(A)),
i.e. from O(N^2) to O(NlogN). This is well worth the extra memory usage

TODO: perhaps have different allocation algorithms for different sizes?

*/

#include "ks.h"

// uncomment this line to enable memory tracing
//#define DO_MEM_TRACE 

#ifdef DO_MEM_TRACE
#define memtrace(...) ks_trace("[MEM] " __VA_ARGS__);
#else
#define memtrace(...)
#endif
// rounds up to the next memory size for the reallocation scheme
#define MEM_NEXT_SIZE(_num) ((uint64_t)((_num) * 1.25 + 8))

/* byte size formatting */

// prefixes for sizes
static const char* size_pfx[] = {
    "B ",
    "KB",
    "MB",
    "GB",
    "TB",
    "PB",
    "EB"
};

// return the char for the unit string
const char* ks_mem_us(size_t bytes) {
    int idx = 0;
    while (bytes > 1024 && idx < sizeof(size_pfx) / sizeof(*size_pfx)) {
        idx++;
        bytes /= 1024;
    }
    return size_pfx[idx];
}

// return the value of the unit string for an amount of bytes
int ks_mem_uv(size_t bytes) {
    int idx = 0;
    while (bytes > 1024 && idx < sizeof(size_pfx) / sizeof(*size_pfx)) {
        idx++;
        bytes /= 1024;
    }
    return (int)bytes;
}


// keep track of the sum of all memory allocated - freed
static size_t mem_cur = 0;

// keep track of the maximum memory allocated at one time
static size_t mem_max = 0;

// record a change in the memory, by a given amount
static inline void rec_mem(int64_t amt) {
    mem_cur += amt;

    // update the maximum, if exceeded
    if (mem_cur > mem_max) {
        mem_max = mem_cur;
    }
}

// internal buffer structure, which holds meta-data about how many bytes were actually allocated,
// and then the pointer is right on top of it
struct ksi_buf {

    uint64_t size;

    void* data[0];

};

// allocate `bytes` bytes of memories
void* ks_malloc(size_t bytes) {
    // ks_malloc(0) -> NULL
    if (bytes == 0) return NULL;

    // give information about allocations > 500 MB
    if (bytes > 500 * 1024 * 1024) {
        ks_debug("[MEM_LARGE] allocating %i%s ...", ks_mem_uv(bytes), ks_mem_us(bytes));
    }
    
    // use the C standard library malloc to get a large enough buffer to hold the meta and data size requestd
    struct ksi_buf* buf = malloc(sizeof(struct ksi_buf) + bytes);

    // check for a problem
    if (buf == NULL) {
        ks_error("ks_malloc(%l) failed!", bytes);
    }
    
    // set the size
    buf->size = bytes;

    // record the allocation
    rec_mem(buf->size);

    // get the offset giving the user pointer
    void* usr_ptr = (void*)&buf->data;

    // trace what happened (making sure to mention the user data, rather than the internal buffer)
    memtrace("ks_malloc(%l) -> %p", bytes, usr_ptr);

    // return the user their pointer
    return usr_ptr;
}

// reallocate a pointer (may return a different pointer)
void* ks_realloc(void* ptr, size_t bytes) {
    // ks_realloc(ptr, 0) will free ptr, and return NULL
    if (bytes == 0) {
        ks_free(ptr);
        return NULL;
    }
    // ks_realloc(NULL, bytes) == ks_malloc(bytes)
    if (ptr == NULL) return ks_malloc(bytes);

    // first, rewind behind the buffer and read our internal buffer structure
    struct ksi_buf* buf = (struct ksi_buf*)ptr - 1;

    // get what size we are currently at
    uint64_t start_size = buf->size;

    if (buf->size < bytes) {

        // debug large reallocations
        if (bytes > 500 * 1024 * 1024) {
            ks_debug("[MEM_LARGE] reallocating %i%s ... (ptr: %p)", ks_mem_uv(bytes), ks_mem_us(bytes), ptr);
        }

        // calculate the next size up
        buf->size = MEM_NEXT_SIZE(bytes);

        // keep track of the original buffer
        struct ksi_buf* orig_buf = buf;

        // now, reallocate it
        buf = realloc(buf, sizeof(struct ksi_buf) + buf->size);

        // check for failure
        if (buf == NULL) {
            ks_error("ks_realloc(%p, %l) failed!", ptr, orig_buf->size);

            // record the free
            rec_mem(- start_size);

            // by the spec, realloc is not supposed to touch the original buffer,
            // so we need to free it
            free(orig_buf);
            return NULL;
        } else {

            // record memory changes
            rec_mem(buf->size - start_size);

            // get the user pointer
            void* usr_ptr = (void*)&buf->data;


            //trace out the memory allocation
            memtrace("ks_realloc(%p, %l) -> %p", ptr, bytes, usr_ptr, ks_mem_uv(bytes), ks_mem_us(bytes));

            return usr_ptr;

        }
    } else {
        // no change is neccessary, the allocated area already has enough bytes to hold the requested amount
        return ptr;
    }
}


void ks_free(void* ptr) {
    // ks_free(NULL) -> no-op
    if (ptr == NULL) return;

    // get the buffer from underneath the data
    struct ksi_buf* buf = (struct ksi_buf*)ptr - 1;

    // record number of bytes
    size_t bytes = buf->size;

    // internally free it
    free(buf);

    // record memory difference
    rec_mem(-bytes);

    memtrace("ks_free(%p)", ptr);
}

size_t ks_memuse() {
    return mem_cur;
}

size_t ks_memuse_max() {
    return mem_max;
}


