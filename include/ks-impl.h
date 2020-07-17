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

void ks_init_T_obj();
void ks_init_T_none();
void ks_init_T_type();

void ks_init_T_bool();
void ks_init_T_int();
void ks_init_T_float();

void ks_init_T_str();
void ks_init_T_str_builder();

void ks_init_T_list();
void ks_init_T_tuple();
void ks_init_T_dict();

void ks_init_T_cfunc();
void ks_init_T_Error();
void ks_init_T_thread();
void ks_init_T_logger();

// extra initializations
void ks_init_funcs();



#ifdef __cplusplus
}
#endif

#endif /* KS_IMPL_H__ */

