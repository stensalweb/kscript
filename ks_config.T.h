/* ks_config.h - set the kscript build configuration information */

#pragma once
#ifndef KS_CONFIG_H__
#define KS_CONFIG_H__

/* logging/printing */

// uncomment this line to disable `ks_trace` calls
// this makes the build faster in many important areas, but disallows trace debugging
//#define KS_C_NO_TRACE

// uncomment this line to disable `ks_debug` calls
// this makes the build faster in some areas, but disallows most debugging.
//#define KS_C_NO_DEBUG


#endif /* KS_CONFIG_H__ */
