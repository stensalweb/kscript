/* modules/mm/mm.h - the kscript multi-media (mm) library
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef MM_H__
#define MM_H__

// include the main kscript API
#include <ks.h>

// we use the NumeriX library for decoding/encoding data from images & audio
#include <nx.h>



/* I/O */

// Read a file as a binary object, and return the whole thing
// NOTE: Returns a new reference
KS_API ks_blob mm_read_file(char* fname);









#endif /* MM_H__ */
