/*
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


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
