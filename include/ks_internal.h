/* ks_internal.h - internal, non-exported functions & data
 *
 * This file is not included with `ks.h`, as it includes internal initialization methods that must
 * be called at initialization time
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef KS_INTERNAL_H__
#define KS_INTERNAL_H__

// only include if not already included
#ifndef KS_H__
#include "ks.h"
#endif

/* internal methods */

// INTERNAL METHOD, DO NOT CALL
void kso_init();
// INTERNAL METHOD, DO NOT CALl
void ksf_init();
// INTERNAL METHOD; DO NOT CALL
void kse_init();
// INTERNAL METHOD; DO NOT CALL
void ks_init__EXEC();

// INTERNAL METHOD; DO NOT CALL
void ks_init__type();
void ks_init__module();
// INTERNAL METHOD; DO NOT CALL
void ks_init__none();
// INTERNAL METHOD; DO NOT CALL
void ks_init__bool();
// INTERNAL METHOD; DO NOT CALL
void ks_init__int();
// INTERNAL METHOD; DO NOT CALL
void ks_init__float();
// INTERNAL METHOD; DO NOT CALL
void ks_init__str();
// INTERNAL METHOD; DO NOT CALL
void ks_init__tuple();
// INTERNAL METHOD; DO NOT CALL
void ks_init__list();
// INTERNAL METHOD; DO NOT CALL
void ks_init__dict();
// INTERNAL METHOD; DO NOT CALL
void ks_init__cfunc();
// INTERNAL METHOD; DO NOT CALL
void ks_init__code();
// INTERNAL METHOD; DO NOT CALL
void ks_init__kfunc();
// INTERNAL METHOD; DO NOT CALL
void ks_init__parser();
// INTERNAL METHOD; DO NOT CALL
void ks_init__ast();
// INTERNAL METHOD; DO NOT CALL
void ks_init__pfunc();
// INTERNAL METHOD; DO NOT CALL
void ks_init__kobj();

// INTERNAL METHOD; DO NOT CALL
void ks_init__list_iter();
// INTERNAL METHOD; DO NOT CALL
void ks_init__dict_iter();

#endif /* KS_INTERNAL_H__ */
