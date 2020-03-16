/* ks_module.h - a specialized header for C-extension headers
 *
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#ifndef MODULE_NAME
#error 'MODULE_NAME' is not defined!
#endif

#pragma once
#ifndef KS_MODULE_H__
#define KS_MODULE_H__

#ifdef __cplusplus
extern "C" {
#endif

// always include the main header
#include "ks.h"


#define MODULE_INIT(__func) struct ks_module_cext_init __C_module_init__ = (struct ks_module_cext_init) { \
    .init_func = __func \
};


// the C extgn
extern struct ks_module_cext_init __C_module_init__;


#ifdef __cplusplus
}
#endif

#endif /* KS_MODULE_H__ */
