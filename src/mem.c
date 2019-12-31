/* mem.c - memory management routines

Uses exponential reallocation for better performance,
instead of reallocing `size`, it reallocs `A*size+B`

*/

#include "ks.h"

// constants for the memory upsizing equation
#define MEM_CONST_A 1.3
#define MEM_CONST_B 4

// rounds up to the next memory size for the reallocation scheme
#define MEM_NEXT_SIZE(_num) ((uint64_t)((_num) * MEM_CONST_A + MEM_CONST_B))

// trace the memory allocations
#define memtrace(...) ks_trace(__VA_ARGS__)
//#define memtrace(...) 

// prefixes for sizes
static const char* size_pfx[] = {
    "B ",
    "KB",
    "MB",
    "GB",
    "TB",
    "PB"
};

// get the prefix for a byte-size
static const char* bs_pfx(size_t bytes) {
    int idx = 0;
    while (bytes > 1024) {
        idx++;
        bytes /= 1024;
    }
    return size_pfx[idx];
}

// get the manissa value for a given size
static int bs_mantissa(size_t bytes) {
    int idx = 0;
    while (bytes > 1024) {
        idx++;
        bytes /= 1024;
    }
    return (int)bytes;
}


// keep track of the sum of all memory allocated - freed
static size_t total_mem = 0;

// internal buffer structure, which holds meta-data about how many bytes were actually allocated,
// and then the pointer is right on top of it
struct ksi_buf {

    uint64_t size;
    void* data[0];

};

// allocate `bytes` bytes of memories
void* ks_malloc(size_t bytes) {
    if (bytes == 0) return NULL;

    // give information about allocations > 500 MB
    if (bytes > 500 * 1024 * 1024) {
        ks_debug("[LARGE] allocating %lu%s...", bs_mantissa(bytes), bs_pfx(bytes));
    }
    
    // use the C standard library malloc to get a large enough buffer to hold the meta and data size requestd
    struct ksi_buf* buf = malloc(sizeof(struct ksi_buf) + bytes);

    // check for a problem
    if (buf == NULL) {
        ks_error("ks_malloc(%lu%s) failed!", bs_mantissa(bytes), bs_pfx(bytes));
    }
    // set the size
    buf->size = bytes;

    // get the offset giving the user pointer
    void* usr_ptr = (void*)&buf->data;

    // do tracing
    memtrace("ks_malloc(%lu) -> %p # size: %lu%s", bytes, usr_ptr, bs_mantissa(bytes), bs_pfx(bytes));

    // now, add the amount of memory to our totals
    total_mem += buf->size;

    return usr_ptr;
}

void* ks_realloc(void* ptr, size_t bytes) {
    if (bytes == 0) return NULL;
    if (ptr == NULL) return ks_malloc(bytes);

    // give information when reallocing 500MB or larger
    if (bytes > 500 * 1024 * 1024) {
        ks_info("[LARGE] re-allocating %lu%s", bs_mantissa(bytes), bs_pfx(bytes));
    }

    // first, rewind behind the buffer and read the metadata
    struct ksi_buf* buf = &((struct ksi_buf*)ptr)[-1];

    uint64_t start_size = buf->size;

    if (buf->size < bytes) {
        // calculate the next size up
        buf->size = MEM_NEXT_SIZE(bytes);
        /*buf->size = MEM_NEXT_SIZE(buf->size);
        // keep going until its large enough
        while (buf->size < bytes) {
            buf->size = MEM_NEXT_SIZE(buf->size);
        }*/

        // now, reallocate it
        buf = realloc(buf, sizeof(struct ksi_buf) + buf->size);

        if (buf == NULL) {
            ks_error("ks_realloc(%p, %lu) failed!", ptr, buf->size);
            return NULL;
        }
    }

    // get the user pointer
    void* usr_ptr = (void*)&buf->data;

    memtrace("ks_realloc(%p, %lu) -> %p", ptr, bytes, usr_ptr);

    // record memory changes
    total_mem += buf->size - start_size; 

    return usr_ptr;
}


void ks_free(void* ptr) {
    if (ptr == NULL) return;

    // get the buffer from underneath the data
    struct ksi_buf* buf = &((struct ksi_buf*)ptr)[-1];

    // record memory difference
    total_mem -= buf->size;

    memtrace("ks_free(%p) # size: %lu%s", ptr, bs_mantissa(buf->size), bs_pfx(buf->size));

    // internally free it
    free(buf);

}



size_t ks_memuse() {
    return total_mem;
}


