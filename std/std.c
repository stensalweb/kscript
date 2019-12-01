// std.c - the main file for the standard library
//
// This contains the methods to actually initialize everything
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//


#include "ks_std.h"



REGISTER_FUNC(main) {
    int status;

    // register types
    if ((status = REGISTER_FUNC_NAME(None)(ctx, module)) != 0) return status;
    if ((status = REGISTER_FUNC_NAME(int)(ctx, module)) != 0) return status;
    if ((status = REGISTER_FUNC_NAME(bool)(ctx, module)) != 0) return status;
    if ((status = REGISTER_FUNC_NAME(float)(ctx, module)) != 0) return status;
    if ((status = REGISTER_FUNC_NAME(str)(ctx, module)) != 0) return status;

    if ((status = REGISTER_FUNC_NAME(list)(ctx, module)) != 0) return status;

    // register functions
    if ((status = REGISTER_FUNC_NAME(builtins0)(ctx, module)) != 0) return status;

    return 0;
}


// define the global loading point for the module
struct ks_module_loader module_loader = KS_MODULE_LOADER(REGISTER_FUNC_NAME(main));



