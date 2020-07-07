#!/usr/bin/env python3
""" nxt_1d.py - generate macros of 1D nxt (NumeriX Template)

To use this, run `python3 tools/gen_nxt_h.py > gen/nxt.h`

@author: Cade Brown <brown.cade@gmail.com

"""

import itertools


# pairs of data type
dtype_pairs = [

    ('nx_dtype_sint8'     , 'int8_t'          ),
    ('nx_dtype_uint8'     , 'uint8_t'         ),
    ('nx_dtype_sint16'    , 'int16_t'         ),
    ('nx_dtype_uint16'    , 'uint16_t'        ),
    ('nx_dtype_sint32'    , 'int32_t'         ),
    ('nx_dtype_uint32'    , 'uint32_t'        ),
    ('nx_dtype_sint64'    , 'int64_t'         ),
    ('nx_dtype_uint64'    , 'uint64_t'        ),

    ('nx_dtype_fp32'      , 'float'           ),
    ('nx_dtype_fp64'      , 'double'          ),

    ('nx_dtype_cplx_fp32' , 'float complex'   ),
    ('nx_dtype_cplx_fp64' , 'double complex'  ),

    # TODO: handle structures
]



print (f"""/* gen/nxt.h - GENERATED FILE FOR TEMPLATES 
 *
 * This defines macros that can be used to quickly generate templates/loops and supporting many data types
 *
 *
 * Macros beginning with `_NXT_` are meant for internal use only, those beginning with `NXT_` are documented
 *
 * `NXT_COMBO` take a `_LOOP` parameter, which should be invoked via `_LOOP(NXT_TYPE_0, ...)`
 *
 *
 */

#ifndef NXT_H__
#define NXT_H__



// internal 1argument case
#define _NXT_CASE_1A(_LOOP, _dtypes, NXT_DTYPE_0, NXT_TYPE_0) else if (_dtypes[0] == NXT_DTYPE_0) {{ _LOOP(NXT_DTYPE_0, NXT_TYPE_0) }}
// internal 2argument case
#define _NXT_CASE_2A(_LOOP, _dtypes, NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1) else if (_dtypes[0] == NXT_DTYPE_0 && _dtypes[1] == NXT_DTYPE_1) {{ _LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1) }}
// internal 3argument case
#define _NXT_CASE_3A(_LOOP, _dtypes, NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1, NXT_DTYPE_2, NXT_TYPE_2) else if (_dtypes[0] == NXT_DTYPE_0 && _dtypes[1] == NXT_DTYPE_1 && _dtypes[2] == NXT_DTYPE_2) {{ _LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1, NXT_DTYPE_2, NXT_TYPE_2) }}



""")


print (f"""


/* Generate loops for a function containing 1 arguments (A)
 *
 *  
 *
 */
#define NXT_COMBO_1A(_LOOP, _dtypes) if (false) {{}} \\""")

for trip in itertools.product(dtype_pairs, repeat=1):
    call_str = ", ".join(", ".join(dtp) for dtp in trip)
    print (f"    _NXT_CASE_1A(_LOOP, _dtypes, {call_str}) \\")


print (f"""


/* Generate loops for a function containing 2 arguments (A, B)
 *
 *  
 *
 */
#define NXT_COMBO_2A(_LOOP, _dtypes) if (false) {{}} \\""")

for trip in itertools.product(dtype_pairs, repeat=2):
    call_str = ", ".join(", ".join(dtp) for dtp in trip)
    print (f"    _NXT_CASE_2A(_LOOP, _dtypes, {call_str}) \\")


print (f"""


/* Generate loops for a function containing 3 arguments (A, B, C)
 *
 *  
 *
 */
#define NXT_COMBO_3A(_LOOP, _dtypes) if (false) {{}} \\""")

for trip in itertools.product(dtype_pairs, repeat=3):
    call_str = ", ".join(", ".join(dtp) for dtp in trip)
    print (f"    _NXT_CASE_3A(_LOOP, _dtypes, {call_str}) \\")





# print ending
print (f"""

#endif /* NXT_H__ */

""")

