/* gen/nxt.h - GENERATED FILE FOR TEMPLATES 
 *
 * This defines macros that can be used to quickly generate templates/loops and supporting many data types
 *
 *
 *
 */

#ifndef NXT_H__
#define NXT_H__


// internal 1argument case
#define _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NXT_TYPE_ENUM_A, NXT_TYPE_A) \
    else if (_dtypes[0] == NXT_TYPE_ENUM_A) { \
        for (i = 0; i < _len; ++i, _dptr_A +=_sb_A) { \
            _LOOP_NAME(NXT_TYPE_ENUM_A, NXT_TYPE_A) \
        } \
    }

// internal 2argument case
#define _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B) \
    else if (_dtypes[0] == NXT_TYPE_ENUM_A && _dtypes[1] == NXT_TYPE_ENUM_B) { \
        for (i = 0; i < _len; ++i, _dptr_A +=_sb_A, _dptr_B += _sb_B) { \
            _LOOP_NAME(NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B) \
        } \
    }

// internal 3argument case
#define _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B, NXT_TYPE_ENUM_C, NXT_TYPE_C) \
    else if (_dtypes[0] == NXT_TYPE_ENUM_A && _dtypes[1] == NXT_TYPE_ENUM_B && _dtypes[2] == NXT_TYPE_ENUM_C) { \
        for (i = 0; i < _len; ++i, _dptr_A +=_sb_A, _dptr_B += _sb_B, _dptr_C += _sb_C) { \
            _LOOP_NAME(NXT_TYPE_ENUM_A, NXT_TYPE_A, NXT_TYPE_ENUM_B, NXT_TYPE_B, NXT_TYPE_ENUM_C, NXT_TYPE_C) \
        } \
    }




/* Generate loops for a function containing 1 arguments (A)
 *
 *  
 *
 */
#define NXT_GENERATE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME) if (false) {} \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_FP32, float) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_FP64, double) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_1A(_len, _dtypes, _dptr_A, _sb_A, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex) \



/* Generate loops for a function containing 2 arguments (A, B)
 *
 *  
 *
 */
#define NXT_GENERATE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME) if (false) {} \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_2A(_len, _dtypes, _dptr_A, _dptr_B, _sb_A, _sb_B, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \



/* Generate loops for a function containing 3 arguments (A, B, C)
 *
 *  
 *
 */
#define NXT_GENERATE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME) if (false) {} \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex, NX_DTYPE_CPLX_FP64, double complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT8, int8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT8, uint8_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT16, int16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT16, uint16_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT32, int32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT32, uint32_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_SINT64, int64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_UINT64, uint64_t) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP32, float) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_FP64, double) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP32, float complex) \
    _NXT_CASE_3A(_len, _dtypes, _dptr_A, _dptr_B, _dptr_C, _sb_A, _sb_B, _sb_C, _LOOP_NAME, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex, NX_DTYPE_CPLX_FP64, double complex) \


#endif /* NXT_H__ */


