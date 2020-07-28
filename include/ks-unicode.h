/* ks_unicode.h - GENERATED file detailing unicode information
 *
 * For information on file formats/etc: https://www.unicode.org/reports/tr44/
 *
 * Generated at 1595894161.2032266
 */

#pragma once
#ifndef KS_UNICODE_H__
#define KS_UNICODE_H__


// main kscript header file
#include <ks.h>


// ks_unich_info - structure describing information about a single character
struct ks_unich_info {

    // (1) Human readable name, which may or may not be in `<>`
    const char* name;

    // (2) General category, i.e. "Lu", "Ll", etc
    // TODO: should this be encoded in a more 'memory efficient' way?
    const char* cat_gen;

    // (3) Canonical Combining Class
    const char* can_com_class;

    // (4) Bidirectional Class, used for bidirectionals
    const char* bidi_class;


    // (12,13,14) cases for upper, lower, and titles
    ks_unich case_upper, case_lower, case_title;


};


// Get information about a given character, may return NULL if error
const struct ks_unich_info* ks_unich_info_get(ks_unich chr);


#endif /* KS_UNICODE_H__ */

