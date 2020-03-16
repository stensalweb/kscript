/* mem.c - kscript's memory management routines
 *
 * 
 * Internally, I use an exponential reallocation algorithm,
 *   so essentially every time a buffer needs to be reallocated, instead of just reallocating to the new size,
 *   we realloc to a certain factor of the new buffer (see _MEM_REALLOC_FAC)
 * : new_sz = req_sz * _MEM_REALLOC_FAC + _MEM_REALLOC_CON
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"



// reallocation factor for new memory
#define _MEM_REALLOC_FAC 1.5

// new reallocation constant
#define _MEM_REALLOC_CON 8


// whether or not to memory trace
#define KS_MEM_TRACE


// only enable trace calls if the build enabled memory tracing
#ifdef KS_MEM_TRACE
#define memtrace(...) if (ks_log_level() == KS_LOG_TRACE) { fprintf(stderr, "[ks_mem] " __VA_ARGS__); }
#else
#define memtrace(...)
#endif

// keep track of the current & maximum amount of memory that has been allocated by kscript
static ks_size_t cur_mem = 0, max_mem = 0;

// mutex requiored for memory operations
static ks_mutex mut = NULL;

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

// initialize the memory system
void ks_mem_init() {
    mut = NULL;
    ks_mutex _mut = ks_mutex_new();

    mut = _mut;
}


// ks_ibuf - internal buffer datastructure used for storing meta-data about the allocated buffer
struct ks_ibuf {
    ks_size_t size;

    void* data[0];
};


// allocate 'sz' bytes of memory, return a pointer to it
void* ks_malloc(ks_size_t sz) {
    if (sz == 0) return NULL;

    if (mut) ks_mutex_lock(mut);

    // attempt to allocate a buffer
    struct ks_ibuf* buf = malloc(sizeof(struct ks_ibuf) + sz);

    if (!buf) {
        fprintf(stderr, "ks_malloc(%zu) failed!\n", sz);
        return NULL;
    }

    // record the memory transaction
    add_mem(buf->size = sz);

    // get what the user sees
    void* usr_ptr = (void*)&buf->data;

    // trace out the result
    memtrace("ks_malloc(%zu) -> %p\n", sz, usr_ptr);


    if (mut) ks_mutex_unlock(mut);

    return usr_ptr;

}

// attempt to reallocate 'ptr' to 'new_sz' bytes, return a pointer to it,
//   or NULL if the reallocation failed
void* ks_realloc(void* ptr, ks_size_t new_sz) {

    // handle simple cases
    if (new_sz <= 0) return ptr;
    if (ptr == NULL) return ks_malloc(new_sz);

    if (mut) ks_mutex_lock(mut);

    // go 'underneath' the buffer to our internal datastructure
    struct ks_ibuf* buf = (struct ks_ibuf*)ptr - 1;

    // capture the starting size
    ks_size_t start_sz = buf->size;

    ks_size_t new_new_sz = new_sz;


    if (buf->size >= new_sz) {
        if (mut) ks_mutex_unlock(mut);

        // no need for reallocation, it holds enough memory already
        return ptr;
    } else {
        // otherwise, call C realloc() to get enough data

        // oversize it 
        new_new_sz = new_sz * _MEM_REALLOC_FAC + _MEM_REALLOC_CON;

        struct ks_ibuf* new_buf = realloc(buf, sizeof(struct ks_ibuf) + new_new_sz);

        if (new_buf == NULL) {
            // realloc failed

            fprintf(stderr, "ks_realloc(%p, %zu) failed!\n", ptr, new_sz);

            // free our buffer we started with
            free(buf);

            return NULL;
        }

        new_buf->size = new_new_sz;

        // record memory difference
        add_mem((int64_t)new_new_sz - start_sz);

        // get what the user sees
        void* usr_ptr = (void*)&new_buf->data;

        // memory trace it out
        memtrace("ks_realloc(%p, %zu) -> %p (new_new_sz: %zu)\n", ptr, new_sz, usr_ptr, new_new_sz);

        if (mut) ks_mutex_unlock(mut);

        // return backl what the user sees
        return usr_ptr;
    }
}


// attempt to free 'ptr'
void ks_free(void* ptr) {
    if (ptr == NULL) return;

    if (mut) ks_mutex_lock(mut);


    // go 'underneath' the buffer to our internal datastructure
    struct ks_ibuf* buf = (struct ks_ibuf*)ptr - 1;

    // get the current memory amount
    ks_size_t sz = buf->size;

    memtrace("ks_free(%p) # sz: %i\n", ptr, (int)sz);

    // remove memory
    add_mem(-(int64_t)sz);

    // actually free the memory
    free(buf);

    if (mut) ks_mutex_unlock(mut);
}

