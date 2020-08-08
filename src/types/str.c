/* str.c - implementation of the string type
 *
 * Internally, data is stored as utf-8 encoded data in all cases (when the type is 'str')
 * Data may be encoded to byte objects, but that is not ever in a 'str' class functionality
 * 
 * utf8 is great for a number of reasons (see: http://utf8everywhere.org/); to summarize:
 *   * 
 * 
 * 
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// List of global singletons representing single-character strings (i.e. 'a', 'b', 'c'), or the empty string (with length 0)
#define KS_STR_CHAR_MAX 255

// global singletons
static struct ks_str_s KS_STR_CHARS[KS_STR_CHAR_MAX];


/* Raw Unicode Functions */


// Calculate the length (in unicode characters, defined via the number of sequences decoded) of a string
//   of (valid) UTF8. A negative return value indicates a unicode error was thrown
// NOTE: If `len_b<0`, then `src` is assumed to be NUL-terminated
ks_ssize_t ks_text_utf8_len_c(const char* src, ks_size_t len_b) {
    if (len_b == 0) return 0;
    else if (len_b < 0) len_b = strlen(src); // default to NUL-terminated

    // iterator
    const char* x = src;

    // length in characters
    ks_ssize_t len_c = 0;

    while (*x) {
        // count actual ending blocks
        if (((*x) & 0xC0) != 0x80) len_c++;
        x++;
    }

    return len_c;
}

// check whether it is a valid continuation byte (return negative if not), or return relevant bits
static ks_unich to_cont(ks_unich c) {
    if ((c & 0xC0) == 0x80) {
        // valid, return lower bytes
        return c & 0x3F;
    } else {
        // bad continuation byte
        return KS_UNICH_WASERR;
    }
}


// Transcode 'len_c' characters of utf8 text (located in 'src'), into utf32 (located in 'dest')
// NOTE: it is assumed that there is enough space in both arrays; `dest` should have been allocated for
//   at least `sizeof(ks_unich) * len_c`, and those are exactly the bytes that will be filled (no NULL-terminator) is given
// NOTE: Returns the number of bytes decoded in utf8, or a negative number to indicate an error
ks_ssize_t ks_text_utf8_to_utf32(const char* src, ks_unich* dest, ks_ssize_t len_c) {

    // iterators
    const char* srci = src;
    ks_unich* desti = dest;

    // temporary variables
    ks_unich c[4];

    // current character
    ks_ssize_t cur_c = 0;

    while (cur_c < len_c) {
        // decode single character
        c[0] = srci[0];

        // check first few bits
        if ((c[0] & 0x80) == 0) {

            // if it was a valid 7-bit ASCII byte (i.e. no continuation), just yield that character
            *desti = c[0];

            // 1 byte
            srci += 1;
        } else if ((c[0] & 0xE0) == 0xC0) {
            
            // decode bytes, 1 cont.
            c[1] = to_cont(srci[1]);

            // read another continuation byte, and combine them
            if (c[1] < 0) return c[1];

            ks_unich r = ((c[0] & 0x1F) << 6) | c[1];
            if (r >= 128) {
                *desti = r;
            } else {
                return KS_UNICH_WASERR;
            }

            // 2 bytes read
            srci += 2;


        } else if ((c[0] & 0xF0) == 0xE0) {

            // decode bytes, 2 cont.
            c[1] = to_cont(srci[1]), c[2] = to_cont(srci[2]);
            if (c[1] < 0) return c[1];
            if (c[2] < 0) return c[2];

            ks_unich r = ((c[0] & 0x0F) << 12) | (c[1] << 6) | c[2];
            if (r >= 2048 && (r < 55296 || r > 57343)) {
                *desti = r;
            } else {
                return KS_UNICH_WASERR;
            }

            // 3 bytes read
            srci += 3;

        } else if ((c[0] & 0xF8) == 0xF0) {
            // we need to read 3 continuation bytes, and combine them
            c[1] = to_cont(srci[1]), c[2] = to_cont(srci[2]), c[3] = to_cont(srci[3]);
            if (c[1] < 0) return c[1];
            if (c[2] < 0) return c[2];
            if (c[3] < 0) return c[3];


            ks_unich r = ((c[0] & 0x07) << 18) | (c[1] << 12) | (c[2] << 6) | c[3];
            if (r >= 65536 && r <= 1114111) {
                *desti = r;
            } else {
                return KS_UNICH_WASERR;
            }

            // 4 bytes read
            srci += 4;
        }

        // advance to next destination
        desti++;
        cur_c++;

    }

    // return number of bytes
    return (ks_ssize_t)(srci - src);
}

// Transcode 'len_c' characters of utf32 text (located in 'src'), into utf8 (located in 'dest')
// NOTE: it is assumed that there is enough space in both arrays; `dest` should have been allocated for
//   at least `sizeof(ks_unich) * len_c` (unless something is known about the input; but this is not recommended in general)
// NOTE: Returns the number of bytes decoded in utf8 (so, always >= len_c), or a negative number to indicate an error
// A NUL-terminator is NOT added
ks_ssize_t ks_text_utf32_to_utf8(const ks_unich* src, char* dest, ks_ssize_t len_c) {

    // iterators
    const ks_unich* srci = src;
    char* desti = dest;

    // current character
    ks_ssize_t cur_c = 0;

    while (cur_c < len_c) {

        // get current character
        ks_unich chr = *srci;

        // check range
        if (chr < 0 || chr > 0x10FFFFULL) {
            ks_throw(ks_T_ArgError, "Invalid unicode value! Expected between 0 and 11141111, but got '%z'", (ks_size_t)chr);
            return KS_UNICH_WASERR;
        }

        // temporary bytes for UTF8 encoding
        char utf8d[4];

        if (chr < 0) {
            // invalid
            ks_throw(ks_T_ArgError, "Invalid unicode value! Got '%z', which is not a valid unicode character!", (ks_ssize_t)chr);
            return KS_UNICH_WASERR;
        } else if (chr <= 0x007F) {

            // assumed to be ascii, so just copy the first byte
            desti[0] = chr;

            // single byte
            desti++;

        } else if (chr <= 0x07FF) {
            // calculate both bytes (0x80, 0xC0 are the beginning of continuation bytes)
            // current bytes are like:
            // 0000 0XXX XXYY YYYY
            // And we need to turn it into a 2 byte sequence:
            // 110X XXXX 10YY YYYY

            // first byte
            desti[0] = 0xC0 | ((chr >> 6) & 0x1F);

            // second byte
            desti[1] = 0x80 | ((chr >> 0) & 0x3F);

            // 2 bytes
            desti += 2;

        } else if (chr <= 0xFFFF) {

            // calculate both bytes (0x80, 0xC0 are the beginning of continuation bytes)
            // current bytes are like:
            // ZZZZ ZXXX XXYY YYYY
            // And we need to turn it into a 3 byte sequence:
            // 1110 ZZZZ 110X XXXX 10YY YYYY

            // first byte starts with the most
            desti[0] = 0xE0 | ((chr >> 12) & 0x0F);
            desti[1] = 0x80 | ((chr >>  6) & 0x3F);
            desti[2] = 0x80 | ((chr >>  0) & 0x3F);

            // 3 bytes
            desti += 3;

        } else if (chr <= 0x10FFFF) {

            // calculate both bytes (0x80, 0xC0 are the beginning of continuation bytes)
            // current bytes are like:
            // 0WWW ZZZZ ZXXX XXYY YYYY
            // And we need to turn it into a 4 byte sequence:
            // 1111 0WWW 1110 ZZZZ 110X XXXX 10YY YYYY

            desti[0] = 0xF0 | ((chr >> 18) & 0x07);
            desti[1] = 0x80 | ((chr >> 12) & 0x3F);
            desti[2] = 0x80 | ((chr >>  6) & 0x3F);
            desti[3] = 0x80 | ((chr >>  0) & 0x3F);

            // 4 bytes
            desti += 4;

        } else {
            ks_throw(ks_T_ArgError, "Invalid unicode value! Got '%z', which is not a valid unicode character!", (ks_ssize_t)chr);
            return KS_UNICH_WASERR;
        }

        // advance to next source
        srci++;
        cur_c++;

    }

    return (ks_ssize_t)(desti - dest);

}



/* C-style string iteration */

// Create a new C-style string iterator
struct ks_str_citer ks_str_citer_make(ks_str self) {
    struct ks_str_citer cit;
    cit.self = self;
    
    // done if empty string
    cit.done = self->len_c == 0;

    // no error
    cit.err = KS_UTF8_ERR_NONE;

    // both indicies start at 0
    cit.cbyi = cit.cchi = 0;

    return cit;
}

// Get the next byte (not necessarily a complete character) from an iterator, returning negative for error
static ks_unich my_getbyte(struct ks_str_citer* cit) {
    // check for out of bounds
    if (cit->cbyi >= cit->self->len_b) {
        cit->err = KS_UTF8_ERR_OUTOFBOUNDS;
        return KS_UNICH_WASERR;
    }

    // return that character
    return cit->self->chr[cit->cbyi++];

}

// Get the next continuation byte (not a complete character), returning negative for error
static ks_unich my_getcont(struct ks_str_citer* cit) {
    ks_unich ret = my_getbyte(cit);
    if ((ret & 0xC0) == 0x80) {
        return ret & 0x3F;
    } else {
        cit->err = KS_UTF8_ERR_OUTOFBOUNDS;
        return KS_UNICH_WASERR;
    }
}

// Get next complete unicode character from a given string
ks_unich ks_str_citer_next(struct ks_str_citer* cit) {

    // ensure we are still in range
    if (cit->cchi >= cit->self->len_c) {
        cit->err = KS_UTF8_ERR_OUTOFBOUNDS;
        return KS_UNICH_WASERR;
    }

    // we are going to read the character, so increment the index
    cit->cchi++;

    // calculate whether it is 'done'
    cit->done = cit->cchi >= cit->self->len_c;

    // current character
    ks_unich r;

    ks_ssize_t sz = ks_text_utf8_to_utf32(cit->self->chr + cit->cbyi, &r, 1);

    if (sz < 0) {
        // TODO: add more specific errors
        cit->err = KS_UTF8_ERR_OUTOFBOUNDS;
        return KS_UNICH_WASERR;
    } else {
        // advance pointer
        cit->cbyi += sz;
        // return decoded value
        return r;
    }
}

// peek at current character, but don't change state
ks_unich ks_str_citer_peek(struct ks_str_citer* cit) {
    // current character
    ks_unich r;

    ks_ssize_t sz = ks_text_utf8_to_utf32(cit->self->chr + cit->cbyi, &r, 1);

    if (sz < 0) {
        // TODO: add more specific errors
        cit->err = KS_UTF8_ERR_OUTOFBOUNDS;
        return KS_UNICH_WASERR;
    } else {
        // don't advance pointer
        // return decoded value
        return r;
    }
}

// Seek to a given position, return success
bool ks_str_citer_seek(struct ks_str_citer* cit, ks_ssize_t idx) {
    // check bounds
    if (idx < 0 || idx > cit->self->len_c) {
        cit->err = KS_UTF8_ERR_OUTOFBOUNDS;
        return false;
    }

    // nothing to do; already at correct position
    if (idx == cit->cchi) return true;

    // check for restarting the iterator
    if (idx == 0) {
        cit->cbyi = cit->cchi = 0;
        cit->done = cit->self->len_c == 0;
        cit->err = 0;
        return true;
    }

    if (KS_STR_ISASCII(cit->self)) {
        // ASCII only, so O(1)
        cit->cbyi = cit->cchi = idx;

        // calculate whether it was 'done'
        cit->done = cit->cchi >= cit->self->len_c;

        return true;
    } else {
        // offset index and remainder


        // the naive distance to travel (in characters)
        ks_ssize_t naive_dist = idx - cit->cchi;

        #if KS_STR_OFF_EVERY

        ks_ssize_t offi = idx / KS_STR_OFF_EVERY;
        ks_ssize_t offe = idx % KS_STR_OFF_EVERY;

        if (!cit->self->offs || (naive_dist > 0 && naive_dist < offe)) {
            // it's more efficient to just probe from the current position, so do nothing

        } else {
            // we should use offsets to do it, so fast forward the iterator to the given offset
            cit->cchi = KS_STR_OFF_EVERY * offi;
            cit->cbyi = cit->self->offs[offi];

        }
        #endif /* KS_STR_OFF_EVERY */

        // we need to seek forward
        while (cit->cchi < idx) {
            // check for errors
            ks_unich c = ks_str_citer_next(cit);
            if (c < 0) return false;
        }

        return true;
    }
}




/* Conversions */


// Convert a length one string to an ordinal, and return it
// NOTE: Returns the value, or a negative number indiciating an error was thrown
ks_unich ks_str_ord(ks_str str) {
    if (str->len_c != 1) {
        ks_throw(ks_T_ArgError, "ord() can only take strings of length 1!");
        return KS_UNICH_WASERR;
    }

    // get first character
    struct ks_str_citer cit = ks_str_citer_make(str);
    return ks_str_citer_next(&cit);
}

// Returns a string of length 1 from a given unicode character (NOT encoded in UTF8; decoded as a single value)
// NOTE: Returns new reference, or NULL if an error was thrown
ks_str ks_str_chr(ks_unich chr) {
    
    // encoded bytes
    char utf8[4];

    // get size & try to encode single byte
    ks_ssize_t sz = ks_text_utf32_to_utf8(&chr, &utf8[0], 1);


    if (sz < 0) {
        return (ks_str)ks_throw(ks_T_ArgError, "Error while decoding unicode for chr: %p", (intptr_t)chr);
    } else {
        // return new string
        return ks_str_utf8(utf8, sz);
    }
}


/* Creation Routines */

// construct new string from UTF-8 data, given length in bytes
ks_str ks_str_utf8(const char* cstr, ks_ssize_t len_b) {
    // calculate length if it was negative
    if (len_b < 0) {
        /**/ if (!cstr || !*cstr) len_b = 0;
        else len_b = strlen(cstr);
    }

    // check for the NUL-string (i.e. empty, length==0)
    /**/ if (len_b == 0 || cstr == NULL || !*cstr) return &KS_STR_CHARS[0];
    else if (len_b == 1) return &KS_STR_CHARS[*cstr];
    else {
        // allocate the string and return a new one
        ks_str self = ks_malloc(sizeof(*self) + len_b);
        KS_INIT_OBJ(self, ks_T_str);
        self->len_b = len_b;

        // copy and NUL-terminate it
        memcpy(self->chr, cstr, self->len_b);
        self->chr[self->len_b] = '\0';

        // calculate hash
        self->v_hash = ks_hash_bytes(self->chr, self->len_b);

        // now, calculate characters via reading the UTF-8
        self->len_c = ks_text_utf8_len_c(self->chr, self->len_b);


        // calculate byte offsets of every-so-often characters, if enabled
        #if KS_STR_OFF_EVERY
        if (self->len_b != self->len_c) {
            // non-ascii data
            
            // calculate offsets
            int n_offs = self->len_c / KS_STR_OFF_EVERY + 1;
            self->offs = ks_malloc(n_offs * sizeof(*self->offs));

            // create an iterator
            struct ks_str_citer cit = ks_str_citer_make(self);

            int i;
            for (i = 0; i < n_offs; ++i) {
                // set current offset
                self->offs[i] = cit.cbyi;

                // skip out early
                if (i >= n_offs - 1) break;

                int j;
                // advance a number of characters
                for (j = 0; j < KS_STR_OFF_EVERY; ++j) {
                    if (ks_str_citer_next(&cit) < 0) {
                        // internal error
                        ks_warn("ks", "While calculating offsets for UTF8 string '%s', ks_str_citer_next() gave an error!", self->chr);
                    }
                }
            }


        } else {
            // ASCII data, since the bytes are equal to the characters
            // no offsets required
            self->offs = NULL;
        
        }
        #endif /* KS_STR_OFF_EVERY */

        return self;
    }
}

// Construct a new 'str' object, from a C-style string.
// TODO: remove this and default everything to UTF8
ks_str ks_str_new_c(const char* cstr, ssize_t len) {
    return ks_str_utf8(cstr, len);
}

// compare strings, comparing memory
int ks_str_cmp(ks_str A, ks_str B) {
    /**/ if (A == B) return 0;
    //else if (A->len_c != B->len_c) return A->len_c - B->len_c;
    else return memcmp(A->chr, B->chr, A->len_b > B->len_b ? B->len_b : A->len_b);
}

// get whether two strings equal each other
bool ks_str_eq(ks_str A, ks_str B) {
    return (A == B) || (A->len_b == B->len_b && A->v_hash == B->v_hash && memcmp(A->chr, B->chr, A->len_b) == 0);
}

// Return whether a kscript string equals a C-style string (of length len, or strlen(cstr) if len<0)
// NOTE: Only byte-wise equality is checked
bool ks_str_eq_c(ks_str A, const char* cstr, ks_ssize_t len) {
    if (len < 0) len = strlen(cstr);
    return (A->chr == cstr) || (A->len_b == len && memcmp(A->chr, cstr, A->len_b) == 0);
}


// Escape the string 'A', i.e. replace '\' -> '\\', and newlines to '\n'
ks_str ks_str_escape(ks_str A) {
    ks_str_builder sb = ks_str_builder_new();

    char utf8[5];
    struct ks_str_citer cit = ks_str_citer_make(A);
    ks_unich ch;
    while (!cit.done && (ch = ks_str_citer_next(&cit))) {
        // escape the given character
        /**/ if (ch == '\\') ks_str_builder_add(sb, "\\\\", 2);
        else if (ch == '\n') ks_str_builder_add(sb, "\\n", 2);
        else if (ch == '\t') ks_str_builder_add(sb, "\\t", 2);
        else if (ch == '\a') ks_str_builder_add(sb, "\\a", 2);
        else if (ch == '\b') ks_str_builder_add(sb, "\\b", 2);

        else {
            // just convert back to UTF8
            int sz = ks_text_utf32_to_utf8(&ch, &utf8[0], 1);
            ks_str_builder_add(sb, utf8, sz);
        }
    }

    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);
    return ret;
}

// Undo the string escaping, i.e. replaces '\n' with a newline
// TODO: add unicode support and escaping
ks_str ks_str_unescape(ks_str A) {
    // generate a string representation
    ks_str_builder sb = ks_str_builder_new();
    
    char hexdigs[16];
    char utf8[5];
    struct ks_str_citer cit = ks_str_citer_make(A);
    ks_unich ch;
    while (!cit.done && (ch = ks_str_citer_next(&cit))) {
        if (ch == '\\') {
            ch = ks_str_citer_next(&cit);
            /**/ if (ch == '\\') ks_str_builder_add(sb, "\\", 1);
            else if (ch == 'n') ks_str_builder_add(sb, "\n", 1);
            else if (ch == 't') ks_str_builder_add(sb, "\t", 1);
            else if (ch == 'a') ks_str_builder_add(sb, "\a", 1);
            else if (ch == 'b') ks_str_builder_add(sb, "\b", 1);
            else if (ch == 'u') {
                // hex digits time, gather unicode escape code
                int i;
                for (i = 0; i < 4; ++i) {
                    char _c = hexdigs[i] = ks_str_citer_next(&cit);
                    if (!((_c >= '0' && _c <= '9') || (_c >= 'a' && _c <= 'f') || (_c >= 'A' && _c <= 'F'))) {
                        // truncated
                        return (ks_str)ks_throw(ks_T_ArgError, "Invalid unicode sequence (truncated): '\\U%*s'", i, hexdigs);
                    }
                }

                hexdigs[i] = '\0';

                // now, convert to ks_unich
                ks_unich read_hex = strtol(hexdigs, NULL, 16);
                int sz = ks_text_utf32_to_utf8(&read_hex, &utf8[0], 1);
                if (sz < 0) {
                    KS_DECREF(sb);
                    return NULL;
                }
                ks_str_builder_add(sb, utf8, sz);
            }

            else if (ch == 'U') {
                // hex digits time, gather unicode escape code
                int i;
                for (i = 0; i < 8; ++i) {
                    char _c = hexdigs[i] = ks_str_citer_next(&cit);
                    if (!((_c >= '0' && _c <= '9') || (_c >= 'a' && _c <= 'f') || (_c >= 'A' && _c <= 'F'))) {
                        // truncated
                        return (ks_str)ks_throw(ks_T_ArgError, "Invalid unicode sequence (truncated): '\\U%*s'", i, hexdigs);
                    }
                }

                hexdigs[i] = '\0';

                // now, convert to ks_unich
                ks_unich read_hex = strtol(hexdigs, NULL, 16);
                int sz = ks_text_utf32_to_utf8(&read_hex, &utf8[0], 1);
                if (sz < 0) {
                    KS_DECREF(sb);
                    return NULL;
                }
                ks_str_builder_add(sb, utf8, sz);

            } else if (ch == 'N') {
                // read a full unicode string name


                ks_unich next = ks_str_citer_next(&cit);
                if (next != '[') {
                    KS_DECREF(sb);
                    return (ks_str)ks_throw(ks_T_ArgError, "Invalid escape sequence: \\N expects '[' after it");
                }


                // current byte position
                int start_pos = cit.cbyi;

                // keep going until ']' is hit
                while (true) {
                    next = ks_str_citer_next(&cit);
                    if (cit.done || next == ']') break;

                }

                if (next != ']') {
                    KS_DECREF(sb);
                    return (ks_str)ks_throw(ks_T_ArgError, "Invalid escape sequence: \\N expects ']' to denote the end");
                }


                // now, transform into a chr
                ks_unich looked = ks_uni_lookup(cit.self->chr + start_pos, cit.cbyi - 1 - start_pos);

                if (looked < 0) {
                    KS_DECREF(sb);
                    return (ks_str)ks_throw(ks_T_ArgError, "Invalid unicode name: '%*s'", cit.cbyi - 1 - start_pos, cit.self->chr + start_pos);
                }

                int sz = ks_text_utf32_to_utf8(&looked, &utf8[0], 1);
                if (sz < 0) {
                    KS_DECREF(sb);
                    return NULL;
                }
                ks_str_builder_add(sb, utf8, sz);
            }

            else {
                // unknown escape code
                KS_DECREF(sb);
                int sz = ks_text_utf32_to_utf8(&ch, &utf8[0], 1);
                return (ks_str)ks_throw(ks_T_ArgError, "Invalid escape sequence: '\\%*s'", sz, utf8);
            }

        } else {
            // just convert back to UTF8
            int sz = ks_text_utf32_to_utf8(&ch, &utf8[0], 1);
            ks_str_builder_add(sb, utf8, sz);
        }

    }

    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);
    return ret;
}


// Get substring
ks_str ks_str_substr(ks_str self, ks_ssize_t start, ks_ssize_t len_c) {

    // check for ascii strings
    if (KS_STR_ISASCII(self)) {
        // len_c==len_b
        return ks_str_utf8(self->chr + start, len_c);
    }

    struct ks_str_citer cit = ks_str_citer_make(self);

    // zoom to the start
    if (!ks_str_citer_seek(&cit, start)) return (ks_str)ks_throw(ks_T_InternalError, "Could not seek with unicode!");

    // start index
    ks_ssize_t start_i = cit.cbyi;

    // go to end
    if (!ks_str_citer_seek(&cit, start + len_c)) return (ks_str)ks_throw(ks_T_InternalError, "Could not seek with unicode!");

    // calculate the length (in bytes) that it coveres
    ks_ssize_t len_b = cit.cbyi - start_i;

    // TODO: maybe more efficient method for substrings
    return ks_str_utf8(self->chr + start_i, len_b);
}

// str.__new__(obj, *args)
static KS_TFUNC(str, new) {
    ks_obj obj;
    int n_extra;
    ks_obj* extra;    
    KS_GETARGS("obj *extra", &obj, &n_extra, &extra);

    if (obj->type->__str__ != NULL) {
        return ks_obj_call(obj->type->__str__, n_args - 1, args + 1);
    } else if (obj->type->__repr__ != NULL) {
        return ks_obj_call(obj->type->__repr__, n_args - 1, args + 1);
    }

    KS_THROW_TYPE_ERR(obj, ks_T_str);
}

// str.__free__(self) - free obj
static KS_TFUNC(str, free) {
    ks_str self;
    KS_GETARGS("self:*", &self, ks_T_str)

    if (self >= &KS_STR_CHARS[0] && self <= &KS_STR_CHARS[KS_STR_CHAR_MAX]) {
        // global singleton
        self->refcnt = KS_REFS_INF;
        return KSO_NONE;
    }

    // nothing else is needed because the string is allocated with enough bytes for all the characters    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// str.__bytes__(self) - get utf8 bytes
static KS_TFUNC(str, bytes) {
    ks_str self;
    KS_GETARGS("self:*", &self, ks_T_str)

    return (ks_obj)ks_bytes_new(self->chr, self->len_b);
}

// str.__repr__(self) - get repr
static KS_TFUNC(str, repr) {
    ks_str self;
    KS_GETARGS("self:*", &self, ks_T_str)

    ks_str esc_self = ks_str_escape(self);
    ks_str ret = ks_fmt_c("'%S'", esc_self);
    KS_DECREF(esc_self);
    return (ks_obj)ret;
}

// str.__len__(self, mode='chars') - get string length
static KS_TFUNC(str, len) {
    ks_str self, mode = NULL;
    KS_GETARGS("self:* ?mode:*", &self, ks_T_str, &mode, ks_T_str)

    if (!mode || ks_str_eq_c(mode, "c", 1) || ks_str_eq_c(mode, "chars", 5)) {
        // length in characters (default)
        return (ks_obj)ks_int_new(self->len_c);
    } else if (ks_str_eq_c(mode, "b", 1) || ks_str_eq_c(mode, "bytes", 5)) {
        // length in bytes
        return (ks_obj)ks_int_new(self->len_b);
    } else {
        // unknown mode!
        return ks_throw(ks_T_ArgError, "Unknown length mode '%S', expected one of: 'chars', 'bytes'", mode);
    }
}

// str.__getitem__(self, idx) - get elements
static KS_TFUNC(str, getitem) {
    ks_str self;
    ks_obj idx;
    KS_GETARGS("self:* idx", &self, ks_T_str, &idx)

    if (ks_num_is_integral(idx)) {
        int64_t idx64;
        if (!ks_num_get_int64(idx, &idx64)) return NULL;

        return (ks_obj)ks_str_substr(self, idx64, 1);
    } else {
        return ks_throw(ks_T_TodoError, "Non-integer indexes not supported!");
    }
}



/* misc. string utilities */


// str.join(self, objs) - join an iterable by a seperator
static KS_TFUNC(str, join) {
    ks_str self;
    ks_obj objs;
    KS_GETARGS("self:* objs:iter", &self, ks_T_str, &objs);

    // create string builder
    ks_str_builder sb = ks_str_builder_new();

    // iterate through the objects
    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob;
    int ct = 0;
    while (ob = ks_citer_next(&cit)) {

        // add self in between them
        if (ct > 0) ks_str_builder_add_str(sb, (ks_obj)self);

        // add string representation
        ks_str_builder_add_str(sb, ob);

        ct++;
        KS_DECREF(ob);        
    }

    // done with iterator
    ks_citer_done(&cit);

    if (cit.threwErr) {
        // early return
        KS_DECREF(sb);
        return NULL;
    }

    // otherwise, collect the result
    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);

    return (ks_obj)ret;

};



/* Operators */

static KS_TFUNC(str, add) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)


    return (ks_obj)ks_fmt_c("%S%S", L, R);

    KS_THROW_BOP_ERR("+", L, R);
}


static KS_TFUNC(str, lt) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        return (ks_obj)KSO_BOOL(ks_str_cmp((ks_str)L, (ks_str)R) < 0);
    }

    KS_THROW_BOP_ERR("<", L, R);
}

static KS_TFUNC(str, gt) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        return (ks_obj)KSO_BOOL(ks_str_cmp((ks_str)L, (ks_str)R) > 0);
    }

    KS_THROW_BOP_ERR(">", L, R);
}

static KS_TFUNC(str, le) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        return (ks_obj)KSO_BOOL(ks_str_cmp((ks_str)L, (ks_str)R) <= 0);
    }

    KS_THROW_BOP_ERR("<=", L, R);
}

static KS_TFUNC(str, ge) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        return (ks_obj)KSO_BOOL(ks_str_cmp((ks_str)L, (ks_str)R) >= 0);
    }

    KS_THROW_BOP_ERR(">=", L, R);
}

static KS_TFUNC(str, eq) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        return (ks_obj)KSO_BOOL(ks_str_eq((ks_str)L, (ks_str)R));
    }

    KS_THROW_BOP_ERR("==", L, R);
}


static KS_TFUNC(str, ne) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        return (ks_obj)KSO_BOOL(!ks_str_eq((ks_str)L, (ks_str)R));
    }

    KS_THROW_BOP_ERR("!=", L, R);
}

static KS_TFUNC(str, cmp) {
    ks_obj L, R;
    KS_GETARGS("L R", &L, &R)

    if (L->type == ks_T_str && R->type == ks_T_str) {
        int sc = ks_str_cmp((ks_str)L, (ks_str)R);
        sc = sc > 0 ? 1 : sc;
        sc = sc < 0 ? -1 : sc;
        return (ks_obj)ks_int_new(sc);
    }

    KS_THROW_BOP_ERR("<=>", L, R);
}

/* string-specific functions */


// str.substr(self, start=0, len=none) - return substring
static KS_TFUNC(str, substr) {
    ks_str self;
    int64_t start, len;
    KS_GETARGS("self:* start:i64 ?len:i64", &self, ks_T_str, &start, &len)

    // default
    if (n_args < 3) len = self->len_c;

    // real values
    int64_t rS = start;

    if (rS < 0) rS += self->len_c;
    if (rS < 0 || rS >= self->len_c) return ks_throw(ks_T_ArgError, "Invalid 'start': %z was out of range!", start);

    if (len > self->len_c - rS) len = self->len_c - rS;

    return (ks_obj)ks_str_substr(self, rS, len);
}


/* 'check' functions that return true or false */


// str.startswith(self, val) - return whether or not the string starts with another one
static KS_TFUNC(str, startswith) {
    ks_str self, val;
    KS_GETARGS("self:* val:*", &self, ks_T_str, &val, ks_T_str)

    if (val->len_b > self->len_b) return KSO_FALSE;
    else if (val->len_b == 0) return KSO_TRUE;
    else return KSO_BOOL(memcmp(self->chr, val->chr, val->len_b) == 0);
}

// str.endswith(self, val) - return whether or not the string ends with another one
static KS_TFUNC(str, endswith) {
    ks_str self, val;
    KS_GETARGS("self:* val:*", &self, ks_T_str, &val, ks_T_str)

    if (val->len_b > self->len_b) return KSO_FALSE;
    else if (val->len_b == 0) return KSO_TRUE;
    else return KSO_BOOL(memcmp(self->chr + self->len_b - val->len_b, val->chr, val->len_b) == 0);
}





/* unicode data */

// str.unidata(self, key='all') - get unicode data (only works on lenghts of string 1)
static KS_TFUNC(str, unidata) {
    ks_str self, key = NULL;
    KS_GETARGS("self:* ?key:*", &self, ks_T_str, &key, ks_T_str)
    if (self->len_c != 1) return ks_throw(ks_T_ArgError, "unidata() only takes strings of length 1 (len was %z)", self->len_c);

    // get information
    const ks_unich_info* info = ks_uni_get_info(ks_str_citer_next((struct ks_str_citer[]){ ks_str_citer_make(self) }));

    // get requested property
    if (!key || ks_str_eq_c(key, "all", 3)) {
        return (ks_obj)ks_dict_new_c(KS_KEYVALS(
            {"name",               (ks_obj)ks_str_new(info->name)},
            //{"category",           (ks_obj)ks_str_new(info->cat_gen)},
            //{"can_com_class",      (ks_obj)ks_str_new(info->can_com_class)},
            //{"bidi_class",         (ks_obj)ks_int_new(info->bidi_class)},

            {"lower",              info->case_lower ? (ks_obj)ks_str_chr(info->case_lower) : KS_NEWREF(self)},
            {"upper",              info->case_upper ? (ks_obj)ks_str_chr(info->case_upper) : KS_NEWREF(self)},
            {"title",              info->case_title ? (ks_obj)ks_str_chr(info->case_title) : KS_NEWREF(self)},
        ));
    } else if (ks_str_eq_c(key, "name", 4)) {
        return (ks_obj)ks_str_new(info->name);

    } else {
        return ks_throw(ks_T_ArgError, "Unknown key for unidata; got '%S', but expected one of: 'name'", key);
    }
}



/* iterator type */

// ks_str_iter - type describing a string iterator
typedef struct {
    KS_OBJ_BASE

    // C-iterator
    struct ks_str_citer cit;

}* ks_str_iter;

// declare type
KS_TYPE_DECLFWD(ks_T_str_iter);

// str_iter.__free__(self) - free obj
static KS_TFUNC(str_iter, free) {
    ks_str_iter self;
    KS_GETARGS("self:*", &self, ks_T_str_iter)

    // remove reference to string
    KS_DECREF(self->cit.self);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

// str_iter.__next__(self) - return next character
static KS_TFUNC(str_iter, next) {
    ks_str_iter self;
    KS_GETARGS("self:*", &self, ks_T_str_iter)
    
    // check if the iterator is done
    if (self->cit.done) return ks_throw(ks_T_OutOfIterError, "");

    ks_unich next_chr = ks_str_citer_next(&self->cit);
    if (next_chr < 0) {
        // error
        return ks_throw(ks_T_TodoError, "There was some unicode error... need to document this!");
    }

    return (ks_obj)ks_str_chr(next_chr);
}

// str.__iter__(self) - return iterator
static KS_TFUNC(str, iter) {
    ks_str self;
    KS_GETARGS("self:*", &self, ks_T_str)

    ks_str_iter ret = KS_ALLOC_OBJ(ks_str_iter);
    KS_INIT_OBJ(ret, ks_T_str_iter);

    ret->cit = ks_str_citer_make(self);
    KS_INCREF(self);

    return (ks_obj)ret;
}



/* export */

KS_TYPE_DECLFWD(ks_T_str);

void ks_init_T_str() {

    // initialize global singletons
    int i;
    for (i = 0; i < KS_STR_CHAR_MAX; ++i) {

        ks_str tc = &KS_STR_CHARS[i];
        KS_INIT_OBJ(tc, ks_T_str);
        tc->len_c = tc->len_b = i == 0 ? 0 : 1;

        tc->chr[0] = (char)i;
        tc->chr[1] = '\0';
        tc->v_hash = ks_hash_bytes(tc->chr, tc->len_b);

    }

    ks_type_init_c(ks_T_str, "str", ks_T_obj, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c(str_new_, "str.__new__(obj, *args)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(str_free_, "str.__free__(self)")},
        {"__iter__",               (ks_obj)ks_cfunc_new_c(str_iter_, "str.__iter__(self)")},

        {"__bytes__",              (ks_obj)ks_cfunc_new_c(str_bytes_, "str.__bytes__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(str_repr_, "str.__repr__(self)")},
        {"__len__",                (ks_obj)ks_cfunc_new_c(str_len_, "str.__len__(self, mode='chars')")},
        {"__getitem__",            (ks_obj)ks_cfunc_new_c(str_getitem_, "str.__getitem__(self, idx)")},

        {"__add__",                (ks_obj)ks_cfunc_new_c(str_add_, "str.__add__(L, R)")},

        {"__cmp__",                (ks_obj)ks_cfunc_new_c(str_cmp_, "str.__cmp__(L, R)")},
        {"__lt__",                 (ks_obj)ks_cfunc_new_c(str_lt_, "str.__lt__(L, R)")},
        {"__gt__",                 (ks_obj)ks_cfunc_new_c(str_gt_, "str.__gt__(L, R)")},
        {"__le__",                 (ks_obj)ks_cfunc_new_c(str_le_, "str.__le__(L, R)")},
        {"__ge__",                 (ks_obj)ks_cfunc_new_c(str_ge_, "str.__ge__(L, R)")},
        {"__eq__",                 (ks_obj)ks_cfunc_new_c(str_eq_, "str.__eq__(L, R)")},
        {"__ne__",                 (ks_obj)ks_cfunc_new_c(str_ne_, "str.__ne__(L, R)")},

        {"unidata",                (ks_obj)ks_cfunc_new_c(str_unidata_, "str.unidata(self, key='all')")},

        {"startswith",             (ks_obj)ks_cfunc_new_c(str_startswith_, "str.startswith(self, val)")},
        {"endswith",             (ks_obj)ks_cfunc_new_c(str_endswith_, "str.endswith(self, val)")},

        {"substr",                 (ks_obj)ks_cfunc_new_c(str_substr_, "str.substr(self, start, len=none)")},
        {"join",                   (ks_obj)ks_cfunc_new_c(str_join_, "str.join(self, objs)")},


    ));

    ks_type_init_c(ks_T_str_iter, "str_iter", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(str_iter_free_, "str_iter.__free__(self)")},
        {"__next__",               (ks_obj)ks_cfunc_new_c(str_iter_next_, "str_iter.__next__(self)")},
    ));


}
