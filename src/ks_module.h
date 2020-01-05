/* ks_module.h - header to be included when defining a kscript module extension in C */

#pragma once
#ifndef KS_MODULE_H__
#define KS_MODULE_H__


#ifndef MODULE_NAME
#error Need to define MODULE_NAME
#endif

#include "ks.h"

// generate the header for the module initialization function
#define MODULE_INIT_FUNC() MFUNC(_this, init)

// this must be included at the end of your file
#define MODULE_END() ks_module_init_t _module_init = {.f_init = _this_init_};


#endif

