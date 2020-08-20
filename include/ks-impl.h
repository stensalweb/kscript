/* ks_impl.h - internal implementation header, only included by internal kscript files
 *
 * Don't include this! Just include `ks.h` for the officially supported API
 * 
 * This file may define things that conflict with other libraries naming schemes
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef KS_IMPL_H__
#define KS_IMPL_H__

#ifdef __cplusplus
extern "C" {
#endif

// always include the main header
#include <ks.h>

// other standard defines
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <dlfcn.h>


/* type initialization functions */

void ks_init_T_object();
void ks_init_T_none();
void ks_init_T_type();

void ks_init_T_bool();
void ks_init_T_int();
void ks_init_T_float();
void ks_init_T_complex();
void ks_init_T_Enum();

void ks_init_T_str();
void ks_init_T_bytes();
void ks_init_T_str_builder();

void ks_init_T_list();
void ks_init_T_tuple();
void ks_init_T_slice();
void ks_init_T_range();
void ks_init_T_dict();
void ks_init_T_set();
void ks_init_T_namespace();

void ks_init_T_func();
void ks_init_T_cfunc();
void ks_init_T_pfunc();
void ks_init_T_Error();
void ks_init_T_thread();
void ks_init_T_logger();
void ks_init_T_module();
void ks_init_T_ios();

void ks_init_T_parser();
void ks_init_T_ast();
void ks_init_T_code();
void ks_init_T_kfunc();
void ks_init_T_stack_frame();

// extra initializations
void ks_init_funcs();


/* numeric type code simplification */


#define _KST_BOPF(_type, _name) static KS_TFUNC(_type, _name) {  \
    ks_obj L, R;                                                 \
    KS_GETARGS("L R", &L, &R)                                    \
    return ks_num_##_name(L, R);                                 \
}


#define _KST_UOPF(_type, _name) static KS_TFUNC(_type, _name) {  \
    ks_obj V;                                                    \
    KS_GETARGS("V", &V)                                          \
    return ks_num_##_name(V);                                    \
}

// generate opfuncs for numerical types
#define KST_NUM_OPFS(_type) \
_KST_BOPF(_type, add) \
_KST_BOPF(_type, sub) \
_KST_BOPF(_type, mul) \
_KST_BOPF(_type, div) \
_KST_BOPF(_type, mod) \
_KST_BOPF(_type, pow) \
_KST_BOPF(_type, binor) \
_KST_BOPF(_type, binand) \
_KST_BOPF(_type, binxor) \
_KST_BOPF(_type, lshift) \
_KST_BOPF(_type, rshift) \
static KS_TFUNC(_type, cmp) { \
    ks_obj L, R;                                                 \
    KS_GETARGS("L R", &L, &R)   \
    int res; \
    if (!ks_num_cmp(args[0], args[1], &res)) return NULL;        \
    return (ks_obj)ks_int_new(res); \
} \
_KST_BOPF(_type, lt) \
_KST_BOPF(_type, gt) \
_KST_BOPF(_type, le) \
_KST_BOPF(_type, ge) \
static KS_TFUNC(_type, eq) { \
    ks_obj L, R;                                                 \
    KS_GETARGS("L R", &L, &R)                                    \
    bool res;                                                    \
    if (!ks_num_eq(L, R, &res)) return NULL;                     \
    return KSO_BOOL(res); \
} \
static KS_TFUNC(_type, ne) { \
    ks_obj L, R;                                                 \
    KS_GETARGS("L R", &L, &R)   \
    bool res; \
    if (!ks_num_eq(L, R, &res)) return NULL;                    \
    return KSO_BOOL(!res); \
} \
/**/ \
static KS_TFUNC(_type, pos) { \
    ks_obj V;                                              \
    KS_GETARGS("V", &V)   \
    return KS_NEWREF(V); \
} \
_KST_UOPF(_type, sqig) \
_KST_UOPF(_type, neg) \
_KST_UOPF(_type, abs) \



// key-values for numerical operator functions
#define KST_NUM_OPKVS(_type) \
    {"__add__",          (ks_obj)ks_cfunc_new_c_old(_type##_add##_, #_type ".__add__(L, R)")}, \
    {"__sub__",          (ks_obj)ks_cfunc_new_c_old(_type##_sub##_, #_type ".__sub__(L, R)")}, \
    {"__mul__",          (ks_obj)ks_cfunc_new_c_old(_type##_mul##_, #_type ".__mul__(L, R)")}, \
    {"__div__",          (ks_obj)ks_cfunc_new_c_old(_type##_div##_, #_type ".__div__(L, R)")}, \
    {"__mod__",          (ks_obj)ks_cfunc_new_c_old(_type##_mod##_, #_type ".__mod__(L, R)")}, \
    {"__pow__",          (ks_obj)ks_cfunc_new_c_old(_type##_pow##_, #_type ".__pow__(L, R)")}, \
    {"__binand__",       (ks_obj)ks_cfunc_new_c_old(_type##_binand##_, #_type ".__binand__(L, R)")}, \
    {"__binor__",        (ks_obj)ks_cfunc_new_c_old(_type##_binor##_, #_type ".__binor__(L, R)")}, \
    {"__binxor__",       (ks_obj)ks_cfunc_new_c_old(_type##_binxor##_, #_type ".__binxor__(L, R)")}, \
    {"__lshift__",       (ks_obj)ks_cfunc_new_c_old(_type##_lshift##_, #_type ".__lshift__(L, R)")}, \
    {"__rshift__",       (ks_obj)ks_cfunc_new_c_old(_type##_rshift##_, #_type ".__rshift__(L, R)")}, \
    {"__cmp__",          (ks_obj)ks_cfunc_new_c_old(_type##_cmp##_, #_type ".__cmp__(L, R)")}, \
    {"__lt__",          (ks_obj)ks_cfunc_new_c_old(_type##_lt##_, #_type ".__lt__(L, R)")}, \
    {"__gt__",          (ks_obj)ks_cfunc_new_c_old(_type##_gt##_, #_type ".__gt__(L, R)")}, \
    {"__le__",          (ks_obj)ks_cfunc_new_c_old(_type##_le##_, #_type ".__le__(L, R)")}, \
    {"__ge__",          (ks_obj)ks_cfunc_new_c_old(_type##_ge##_, #_type ".__ge__(L, R)")}, \
    {"__eq__",          (ks_obj)ks_cfunc_new_c_old(_type##_eq##_, #_type ".__eq__(L, R)")}, \
    {"__ne__",          (ks_obj)ks_cfunc_new_c_old(_type##_ne##_, #_type ".__ne__(L, R)")}, \
    {"__pos__",          (ks_obj)ks_cfunc_new_c_old(_type##_pos##_, #_type ".__pos__(V)")}, \
    {"__neg__",          (ks_obj)ks_cfunc_new_c_old(_type##_neg##_, #_type ".__neg__(V)")}, \
    {"__sqig__",         (ks_obj)ks_cfunc_new_c_old(_type##_sqig##_, #_type ".__sqig__(V)")}, \
    {"__abs__",          (ks_obj)ks_cfunc_new_c_old(_type##_abs##_, #_type ".__abs__(V)")}, \


#ifdef __cplusplus
}
#endif

#endif /* KS_IMPL_H__ */

