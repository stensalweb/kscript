#!/usr/bin/env python3
""" unicodedata.py - parses UnicodeData, and generates the C file (that should typically go in `include/ks_unicode.h`)

Check here for information: https://www.unicode.org/reports/tr44/

Usage:

./tools/unicodedata.py > include/ks_unicode.h

"""

import time
import requests

# root of the unicode folder
UCD_ROOT = "https://www.unicode.org/Public/13.0.0"


# get source code
UnicodeData_src = requests.get(f"{UCD_ROOT}/ucd/UnicodeData.txt").text

# C source files
unicode_h = ""
unicode_c = ""


# timestamp
times = time.time()



# print header info
unicode_h += f"""/* ks_unicode.h - GENERATED file detailing unicode information
 *
 * For information on file formats/etc: https://www.unicode.org/reports/tr44/
 *
 * Generated at {times}
 */

#pragma once
#ifndef KS_UNICODE_H__
#define KS_UNICODE_H__


// main kscript header file
#include <ks.h>

// ks_unich_info - structure describing information about a single character
struct ks_unich_info {{

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

}};

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

"""


unicode_c += f"""/* ks_unicode.c - actual source code generated for unicode
 *
 *
 * Generated at {times}
 */

#include <ks.h>


// category strings (used so pointer comparison can be used)
const char
    ks_unicat_Lu[] = "Lu",
    ks_unicat_Ll[] = "Ll",
    ks_unicat_Lt[] = "Lt",
    ks_unicat_LC[] = "LC",
    ks_unicat_Lm[] = "Lm",
    ks_unicat_Lo[] = "Lo",
    ks_unicat_L [] = "L",
    ks_unicat_Mn[] = "Mn",
    ks_unicat_Mc[] = "Mc",
    ks_unicat_Me[] = "Me",
    ks_unicat_M [] = "M",
    ks_unicat_Nd[] = "Nd",
    ks_unicat_Nl[] = "Nl",
    ks_unicat_No[] = "No",
    ks_unicat_N [] = "N",
    ks_unicat_Pc[] = "Pc",
    ks_unicat_Pd[] = "Pd",
    ks_unicat_Ps[] = "Ps",
    ks_unicat_Pe[] = "Pe",
    ks_unicat_Pi[] = "Pi",
    ks_unicat_Pf[] = "Pf",
    ks_unicat_Po[] = "Po",
    ks_unicat_P [] = "P",
    ks_unicat_Sm[] = "Sm",
    ks_unicat_Sc[] = "Sc",
    ks_unicat_Sk[] = "Sk",
    ks_unicat_So[] = "So",
    ks_unicat_S [] = "S",
    ks_unicat_Zs[] = "Zs",
    ks_unicat_Zl[] = "Zl",
    ks_unicat_Zp[] = "Zp",
    ks_unicat_Z [] = "Z",
    ks_unicat_Cc[] = "Cc",
    ks_unicat_Cf[] = "Cf",
    ks_unicat_Cs[] = "Cs",
    ks_unicat_Co[] = "Co",
    ks_unicat_Cn[] = "Cn",
    ks_unicat_C [] = "C";


// all data from 'UnicodeData.txt'
static struct ks_unich_info unich_infos[] = {{

"""




# max codepoint
idx = 0

# TODO: are others neccessary?
for line in UnicodeData_src.splitlines():
    # check here: https://www.unicode.org/reports/tr44/#UnicodeData.txt

    fields = line.split(';')
    assert len(fields) == 15 and "not enough fields!"


    def tocase(i, v):
        if v:
            return "0x" + v
        else:
            return "0x" + i


    # generate
    unicode_c += f"    [0x{fields[0]}] = (struct ks_unich_info) {{ .name = \"{fields[1]}\", .cat_gen = ks_unicat_{fields[2]}, .can_com_class = ks_unicat_{fields[2]}, .bidi_class = {fields[3]}, .case_upper = {tocase(fields[0], fields[12])}, .case_lower = {tocase(fields[0], fields[13])}, .case_title = {tocase(fields[0], fields[14])},    }},\n"

    idx += 1


unicode_c += f"""

}};


// macro to get requested information
#define _UINFO(_ch) ks_unich_info_get(_ch)

// get unicode information
const struct ks_unich_info* ks_unich_info_get(int32_t chr) {{
    return &unich_infos[chr];
}}


/* Misc. Text functions */

// whether it is a letter
bool ks_uni_isalpha(ks_unich chr) {{
    return _UINFO(chr)->cat_gen[0] == 'L';
}}


"""



# write files
with open('include/ks-unicode.h', 'w') as fp:
    fp.write(unicode_h)

with open('src/unicode.c', 'w') as fp:
    fp.write(unicode_c)


