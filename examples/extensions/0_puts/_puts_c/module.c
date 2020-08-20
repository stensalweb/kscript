/* _puts_c/module.c - module definition for `puts_c`
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 * 
 */

// always begin by defining the module information
#define MODULE_NAME "_puts_c"

// include this since this is a module.
#include "ks-module.h"


// _puts_c.puts(obj) - write `obj` out to the standard output
static KS_TFUNC(_puts_c, puts) {
    ks_str obj;
    KS_GETARGS("obj:*", &obj, ks_T_str)

    if (KS_STR_ISUTF8(obj)) {
        // call 'puts' function
        puts(obj->chr);
    } else {
        // not in UTF8
        return ks_throw(ks_T_InternalError, "`str` object was not in UTF8!");
    }

    // always return `none`
    return KSO_NONE;
}


// export as module
static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME, "C implementation to display a string by printing to the standard output");

    ks_dict_set_c(mod->attr, KS_KEYVALS(
        {"puts",                   (ks_obj)ks_cfunc_new_c(_puts_c_puts_, "_puts_c.puts(obj)", "")},
    ));

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
