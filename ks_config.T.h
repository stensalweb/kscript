/* ks_config.h - set the kscript build configuration information */

#pragma once
#ifndef KS_CONFIG_H__
#define KS_CONFIG_H__

/* logging/printing */

// uncomment this line to disable `ks_trace` calls
// this makes the build faster in many important areas, but 
//#define KS_C_NO_TRACE

// if defined, `ks_debug` calls become nothing, so debugging is not available, but makes more efficient code
//#define KS_C_NO_DEBUG



#endif /* KS_CONFIG_H__ */
