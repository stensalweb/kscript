/* mem.c - kscript's memory management routines
 *
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


#define KS_MEM_TRACE


// only enable trace calls if the build enabled memory tracing
#ifdef KS_MEM_TRACE
#define memtrace(...) if (ks_log_level() == KS_LOG_TRACE) fprintf(stderr, "[ks_mem] " __VA_ARGS__);
#else
#define memtrace(...)
#endif

// keep track of the current & maximum amount of memory that has been allocated by kscript
static ks_size_t cur_mem = 0, max_mem = 0;

// record a change in the given memory
static void add_mem(int64_t amt) {
    cur_mem += amt;
    if (cur_mem > max_mem) max_mem = cur_mem;
}

// Return the current amount of memory being used, or -1 if memory usage is not being tracked
int64_t ks_mem_cur() {
    return cur_mem;
}
// Return the maximum amount of memory that has been used at one time, or -1 if memory usage is not being tracked
int64_t ks_mem_max() {
    return max_mem;
}


// ks_ibuf - internal buffer datastructure used for storing meta-data about the allocated buffer
struct ks_ibuf {
    ks_size_t size;

    void* data[0];
};


// allocate 'sz' bytes of memory, return a pointer to it
void* ks_malloc(ks_size_t sz) {
    if (sz == 0) return NULL;

    // attempt to allocate a buffer
    struct ks_ibuf* buf = malloc(sizeof(struct ks_ibuf) + sz);

    if (!buf) {
        ks_error("ks_malloc(%zu) failed!\n", sz);
        return NULL;
    }

    // record the memory transaction
    add_mem(buf->size = sz);

    // get what the user sees
    void* usr_ptr = (void*)&buf->data;

    // trace out the result
    memtrace("ks_malloc(%zu) -> %p\n", sz, usr_ptr);

    return usr_ptr;

}

// attempt to reallocate 'ptr' to 'new_sz' bytes, return a pointer to it,
//   or NULL if the reallocation failed
void* ks_realloc(void* ptr, ks_size_t new_sz) {

    // handle simple cases
    if (new_sz <= 0) return ptr;
    if (ptr == NULL) return ks_malloc(new_sz);


    // go 'underneath' the buffer to our internal datastructure
    struct ks_ibuf* buf = (struct ks_ibuf*)ptr - 1;

    // capture the starting size
    ks_size_t start_sz = buf->size;


    if (buf->size >= new_sz) {
        // no need for reallocation, it holds enough memory already
        return ptr;
    } else {
        // otherwise, call C realloc() to get enough data

        struct ks_ibuf* new_buf = realloc(buf, sizeof(struct ks_ibuf) + new_sz);

        if (new_buf == NULL) {
            // realloc failed

            ks_error("ks_realloc(%p, %zu) failed!\n", ptr, new_sz);

            // free our buffer we started with
            free(buf);

            return NULL;
        }

        new_buf->size = new_sz;

        // record memory difference
        add_mem((int64_t)new_sz - start_sz);

        // get what the user sees
        void* usr_ptr = (void*)&new_buf->data;

        // memory trace it out
        memtrace("ks_realloc(%p, %zu) -> %p\n", ptr, new_sz, usr_ptr);

        // return backl what the user sees
        return usr_ptr;
    }
}


// attempt to free 'ptr'
void ks_free(void* ptr) {
    if (ptr == NULL) return;

    memtrace("ks_free(%p)\n", ptr);

    // go 'underneath' the buffer to our internal datastructure
    struct ks_ibuf* buf = (struct ks_ibuf*)ptr - 1;

    // get the current memory amount
    ks_size_t sz = buf->size;

    // remove memory
    add_mem(-(int64_t)sz);

    // actually free the memory
    free(buf);

}




