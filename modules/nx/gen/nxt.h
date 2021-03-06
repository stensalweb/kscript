/* gen/nxt.h - GENERATED FILE FOR TEMPLATES 
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
#define _NXT_CASE_1A(_LOOP, _dtype_0, NXT_DTYPE_0, NXT_TYPE_0) else if (_dtype_0 == NXT_DTYPE_0) { _LOOP(NXT_DTYPE_0, NXT_TYPE_0) }
// internal 2argument case
#define _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1) else if (_dtype_0 == NXT_DTYPE_0 && _dtype_1 == NXT_DTYPE_1) { _LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1) }
// internal 3argument case
#define _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1, NXT_DTYPE_2, NXT_TYPE_2) else if (_dtype_0 == NXT_DTYPE_0 && _dtype_1 == NXT_DTYPE_1 && _dtype_2 == NXT_DTYPE_2) { _LOOP(NXT_DTYPE_0, NXT_TYPE_0, NXT_DTYPE_1, NXT_TYPE_1, NXT_DTYPE_2, NXT_TYPE_2) }







/* Generate loops for a function containing 1 arguments (A)
 *
 *  
 *
 */
#define NXT_COMBO_1A(_LOOP, _dtype_0) if (false) {} \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_sint8, int8_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_sint16, int16_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_sint32, int32_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_sint64, int64_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_fp32, float) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_fp64, double) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_1A(_LOOP, _dtype_0, nx_dtype_cplx_fp64, double complex) \



/* Generate loops for a function containing 2 arguments (A, B)
 *
 *  
 *
 */
#define NXT_COMBO_2A(_LOOP, _dtype_0, _dtype_1) if (false) {} \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_2A(_LOOP, _dtype_0, _dtype_1, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \



/* Generate loops for a function containing 3 arguments (A, B, C)
 *
 *  
 *
 */
#define NXT_COMBO_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2) if (false) {} \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex, nx_dtype_cplx_fp64, double complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint8, int8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint8, uint8_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint16, int16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint16, uint16_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint32, int32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint32, uint32_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_sint64, int64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_uint64, uint64_t) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_fp32, float) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_fp64, double) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp32, float complex) \
    _NXT_CASE_3A(_LOOP, _dtype_0, _dtype_1, _dtype_2, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex, nx_dtype_cplx_fp64, double complex) \


#endif /* NXT_H__ */


