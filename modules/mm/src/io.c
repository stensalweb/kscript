/* src/io.c - Input/Output related functions
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../mm-impl.h"


// read whole file as a blob
ks_blob mm_read_file(char* fname) {
    ks_iostream ios = ks_iostream_new();

    // attempt to open the file
    if (!ks_iostream_open(ios, fname, "rb")) {
        KS_DECREF(ios);
        return NULL;
    }

    ks_ssize_t sz_ios = ks_iostream_size(ios);
    if (sz_ios < 0) {
        KS_DECREF(ios);
        return NULL;
    }


    // read data
    ks_blob res = ks_iostream_readblob_n(ios, sz_ios);

    if (!res) {
        KS_DECREF(ios);
        return NULL;
    }

    return res;
}




