/* ks_module.h - header for kscript modules
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
#include <ks.h>


// use this at the end of the module with '__func' as the function which can be called to construct
//   and return the module
#define MODULE_INIT(__func) struct ks_module_cinit __ks_module_cinit__ = (struct ks_module_cinit) { \
    .load_func = __func \
};


// the symbol that is expected to be exported by the module
KS_API extern struct ks_module_cinit __ks_module_cinit__;


#ifdef __cplusplus
}
#endif

#endif /* KS_MODULE_H__ */

