/* ks_unicode.h - GENERATED file detailing unicode information
 *
 * For information on file formats/etc: https://www.unicode.org/reports/tr44/
 *
 * Generated at 1595951019.3883367
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
    int bidi_class;

    // (12,13,14) cases for upper, lower, and titles
    ks_unich case_upper, case_lower, case_title;

};

// category strings (used so pointer comparison can be used)
// see: https://www.unicode.org/reports/tr44/#General_Category_Values
KS_API extern const char
    ks_unicat_Lu[],
    ks_unicat_Ll[],
    ks_unicat_Lt[],
    ks_unicat_LC[],
    ks_unicat_Lm[],
    ks_unicat_Lo[],
    ks_unicat_L [],
    ks_unicat_Mn[],
    ks_unicat_Mc[],
    ks_unicat_Me[],
    ks_unicat_M [],
    ks_unicat_Nd[],
    ks_unicat_Nl[],
    ks_unicat_No[],
    ks_unicat_N [],
    ks_unicat_Pc[],
    ks_unicat_Pd[],
    ks_unicat_Ps[],
    ks_unicat_Pe[],
    ks_unicat_Pi[],
    ks_unicat_Pf[],
    ks_unicat_Po[],
    ks_unicat_P [],
    ks_unicat_Sm[],
    ks_unicat_Sc[],
    ks_unicat_Sk[],
    ks_unicat_So[],
    ks_unicat_S [],
    ks_unicat_Zs[],
    ks_unicat_Zl[],
    ks_unicat_Zp[],
    ks_unicat_Z [],
    ks_unicat_Cc[],
    ks_unicat_Cf[],
    ks_unicat_Cs[],
    ks_unicat_Co[],
    ks_unicat_Cn[],
    ks_unicat_C [];

// Get information about a given character, may return NULL if error
KS_API const struct ks_unich_info* ks_unich_info_get(ks_unich chr);


/* Specific Checks (i.e. replacing 'ctype' library) */

// Return whether or not a given unicode character is alpha
// This is according to whether or not 'chr' has a general category that begins with 'L', meaning it is a letter
KS_API bool ks_uni_isalpha(ks_unich chr);



#endif /* KS_UNICODE_H__ */

