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
    const char* bidi_class;


    // (12,13,14) cases for upper, lower, and titles
    ks_unich case_upper, case_lower, case_title;


}};


// Get information about a given character, may return NULL if error
const struct ks_unich_info* ks_unich_info_get(ks_unich chr);


#endif /* KS_UNICODE_H__ */

"""


unicode_c += f"""/* ks_unicode.c - actual source code generated for unicode
 *
 *
 * Generated at {times}
 */

#include <ks.h>

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


    def tocase(v):
        if v.startswith('0x'):
            return v
        elif v:
            return "0x" + v
        else:
            return "0x0"

    # generate
    unicode_c += f"    [0x{fields[0]}] = (struct ks_unich_info) {{ .name = \"{fields[1]}\", .cat_gen = \"{fields[2]}\", .can_com_class = \"{fields[2]}\", .bidi_class = \"{fields[3]}\", .case_upper = {tocase(fields[12])}, .case_lower = {tocase(fields[13])}, .case_title = {tocase(fields[14])},    }},\n"

    idx += 1


unicode_c += f"""

}};


// get unicode information
const struct ks_unich_info* ks_unich_info_get(int32_t chr) {{
    return &unich_infos[chr];
}}


"""



# write files
with open('include/ks-unicode.h', 'w') as fp:
    fp.write(unicode_h)

with open('src/unicode.c', 'w') as fp:
    fp.write(unicode_c)


