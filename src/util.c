/*
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// read a file's entire source and return as a string
// return 'NULL' and throw an exception if there was an error
ks_str ks_readfile(char* fname) {

    FILE* fp = fopen(fname, "r");
    if (!fp) {
        return ks_throw_fmt(ks_type_Error, "Failed to open file '%s': %s", fname, strerror(errno));
    }

    fseek(fp, 0, SEEK_END);
    int len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char* csrc = ks_malloc(len + 1);
    if (len != fread(csrc, 1, len, fp)) {
        ks_warn("Reading %i bytes failed for file %s", len, fname);
    }
    csrc[len] = '\0';

    ks_str src = ks_str_new(csrc);
    ks_free(csrc);
    
    return src;
}

// implementation of GNU's getline
// Adapted from: https://gist.github.com/jstaursky/84cf1ddf91716d31558d6f0b5afc3feb
int ks_getline(char** lineptr, size_t* n, FILE* fp) {
    char    *buffer_tmp, *position;
    size_t   block,      offset;
    int      c;

    if (!fp || !lineptr || !n) {
        return -1;

    } else if (*lineptr == NULL || *n <= 1) {
        // Minimum length for strings is 2 bytes
        *n = 128;
        if (!(*lineptr = ks_malloc(*n))) {
            return -1;
        }
    }

    block = *n;
    position = *lineptr;

    // keep reading characters until newline is hit
    while ((c = fgetc(fp)) != EOF && (*position++ = c) != '\n') {
        // Keep track of our offset
        offset = position - *lineptr;

        if( offset >= *n ) {
            buffer_tmp = ks_realloc(*lineptr, *n += block);
            
            /* Do not free. Return *lineptr. */
            if (!buffer_tmp) {
                return -1;
            }

            *lineptr = buffer_tmp;
            position = *lineptr + offset;
        }
    }
    // NUL-terminate
    *position = '\0';
    return (position - *lineptr - 1);
}
