/* str.c - implementation of the string type
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// List of global singletons representing single-character strings (i.e. 'a', 'b', 'c'), or the empty string (with length 0)
#define KS_STR_CHAR_MAX 255

// global singletons
static struct ks_str_s KS_STR_CHARS[KS_STR_CHAR_MAX];


// Construct a new 'str' object, from a C-style string.
ks_str ks_str_new_c(const char* cstr, ssize_t len) {
    // calculate length if it was negative
    if (len < 0) {
        /**/ if (!cstr || !*cstr) len = 0;
        else len = strlen(cstr);
    }

    // check for the NUL-string (i.e. empty, length==0)
    /**/ if (len == 0 || cstr == NULL || !*cstr) return &KS_STR_CHARS[0];
    else if (len == 1) return &KS_STR_CHARS[*cstr];
    else {
        // allocate the string and return a new one
        ks_str self = ks_malloc(sizeof(*self) + len);
        // TODO: check NULL return here

        KS_INIT_OBJ(self, ks_T_str);

        self->len = len;
        self->len_b = len;

        // copy and NUL-terminate it
        memcpy(self->chr, cstr, len);
        self->chr[len] = '\0';

        // calculate hash
        self->v_hash = ks_hash_bytes(self->chr, self->len_b);

        return self;
    }

}

// compare strings
int ks_str_cmp(ks_str A, ks_str B) {
    /**/ if (A == B) return 0;
    else if (A->len != B->len) return A->len - B->len;
    else return memcmp(A->chr, B->chr, A->len);
}

// get whether two strings equal each other
bool ks_str_eq(ks_str A, ks_str B) {
    return (A == B) || (A->len == B->len && A->v_hash == B->v_hash && memcmp(A->chr, B->chr, A->len) == 0);
}

/* export */

KS_TYPE_DECLFWD(ks_T_str);

void ks_init_T_str() {

}
