

#ifndef KSCRIPT_MODULE_H_
#define KSCRIPT_MODULE_H_

#ifndef MODULE_NAME
#error Didnt define MODULE_NAME
#else

#include "kscript.h"


// the name for a type's function
#define TYPE_FUNC_NAME(_tname, _fname) _tname##_fname

// declares a type that will be registered by the current file
#define TYPE_DECL(_tname) static int _tname##_typeid = -1; static ks_obj _tname##_type = NULL;

// declares the actual function
#define TYPE_FUNC(_tname, _fname) ks_obj TYPE_FUNC_NAME(_tname, _fname)(ks_ctx ctx, int args_n, ks_obj* args)

// shortcut for registering a type for the context
#define TYPE_REGISTER(_tname, ...) { if ((_tname##_typeid = ks_ctx_new_type(ctx, KS_STR_CONST(#_tname), ks_obj_new_type_dict(ks_dict_fromkvp_cp(__VA_ARGS__)))) < 0) { ks_error("Tried to add type '%s' failed", #_tname); return 1; } _tname##_type = ctx->types[_tname##_typeid]; }

// the name of the function for registering a certain thing
#define REGISTER_FUNC_NAME(_name) _name##_register

// function to register a submodule of the module (just a single file)
#define REGISTER_FUNC(_name) int REGISTER_FUNC_NAME(_name)(ks_ctx ctx)



// this is what the module loading system looks for in a .so file
extern struct ks_module_loader module_loader;


#endif

#endif

