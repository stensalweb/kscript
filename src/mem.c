/* mem.c - memory management routines

Uses exponential reallocation for better performance,
instead of reallocing `size`, it reallocs `A*size+B`


*/

#include "kscript.h"

// A in A*size+B
#define MEM_EQ_A 1.2
// B in A*size+B
#define MEM_EQ_B 8

// trace the memory
//#define memtrace(...) ks_trace(__VA_ARGS__)
#define memtrace(...) 

static const char* size_pfx[] = {
    "B ",
    "KB",
    "MB",
    "GB",
    "TB",
    "PB"
};

static const char* bs_pfx(size_t bytes) {
    int idx = 0;
    while (bytes > 1024) {
        idx++;
        bytes /= 1024;
    }
    return size_pfx[idx];
}

static int bs_mantissa(size_t bytes) {
    int idx = 0;
    while (bytes > 1024) {
        idx++;
        bytes /= 1024;
    }
    return (int)bytes;
}


static size_t total_mem = 0;

// internal buffer structure
struct ksi_buf {

    uint32_t size;
    char data[1];

};


void* ks_malloc(size_t bytes) {
    if (bytes == 0) return NULL;

    // 500MB info
    if (bytes > 500 * 1024 * 1024) {
        ks_info("[LARGE] allocating %lu%s", bs_mantissa(bytes), bs_pfx(bytes));
    }
    
    struct ksi_buf* p = malloc(sizeof(uint32_t) + bytes);
    if (p == NULL) {
        ks_error("ks_malloc(%lu%s) failed!", bs_mantissa(bytes), bs_pfx(bytes));
    }

    total_mem += bytes;
    p->size = bytes;
    void* res = (void*)&p->data;

    memtrace("ks_malloc(%lu) -> %p", bytes, res);

    return res;
}

void* ks_realloc(void* ptr, size_t bytes) {
    if (ptr == NULL) return ks_malloc(bytes);

    // 500MB info
    if (bytes > 500 * 1024 * 1024) {
        ks_info("[LARGE] re-allocating %lu%s", bs_mantissa(bytes), bs_pfx(bytes));
    }

    struct ksi_buf* p = (struct ksi_buf*)(((uintptr_t)ptr) - sizeof(uint32_t));
    void* res = NULL;

    if (p->size < bytes) {
        bytes = (size_t)(MEM_EQ_A * bytes + MEM_EQ_B);
        p = realloc(p, sizeof(uint32_t) + bytes);

        if (p == NULL) {
            ks_error("ks_realloc(%p, %lu) failed!", ptr, bytes);
            return NULL;
        }
    }

    res = (void*)&p->data;

    total_mem += bytes - p->size; 
    p->size = bytes;

    memtrace("ks_realloc(%p, %lu) -> %p", ptr, bytes, res);

    return res;
}


void ks_free(void* ptr) {
    if (ptr == NULL) return;

    struct ksi_buf* p = (struct ksi_buf*)((uintptr_t)ptr - sizeof(uint32_t));
    total_mem -= p->size;
    free(p);

    memtrace("ks_free(%p)", ptr);

}



size_t ks_memuse() {
    return total_mem;
}


