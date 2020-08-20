/* sys/src/module.c - the kscript's system library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "sys"

// include this since this is a module.
#include "ks-module.h"

// standard sys modules
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/utsname.h>


// sys.refs(obj) - return references to obj
static KS_TFUNC(sys, refs) {
    ks_obj obj;
    KS_GETARGS("obj", &obj);

    return (ks_obj)ks_int_new(obj->refcnt);
}



// sys.shell(cmd) - result
static KS_TFUNC(sys, shell) {
    ks_str cmd;
    KS_GETARGS("cmd:*", &cmd, ks_T_str)

    // unlock the GIL, so other threads may go while it is running
    ks_GIL_unlock();

    // use the 'system' call here; popen might not always be available
    int res = system(cmd->chr);

    // acquire it back before calling any more functions
    ks_GIL_lock();

    // and now, return a boolean depending on whether the exit status was 0 (success)
    return KSO_BOOL(WEXITSTATUS(res) == 0);
}



// now, export them all
static ks_module get_module() {
    
    struct utsname unameData;

    if (uname(&unameData) != 0) {
        return ks_throw(ks_T_Error, "uname() function failed!");
    }

    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_c(mod->attr, KS_KEYVALS(
        /* constants */
        {"platform",     (ks_obj)ks_fmt_c("%s", unameData.sysname)},

        {"uname", (ks_obj)ks_dict_new_c(KS_KEYVALS(
            {"name",        (ks_obj)ks_str_new(unameData.sysname) },
            {"arch",        (ks_obj)ks_str_new(unameData.machine) },
            {"version",     (ks_obj)ks_str_new(unameData.version) },
            {"release",     (ks_obj)ks_str_new(unameData.release) },
            {"node",        (ks_obj)ks_str_new(unameData.nodename) },
        ))},

        /* functions */
        {"refs",            (ks_obj)ks_cfunc_new_c_old(sys_refs_, "sys.refs(obj)")},
        {"shell",           (ks_obj)ks_cfunc_new_c_old(sys_shell_, "sys.shell(cmd)")},
        
        /* wrappers */
        /*{"stdin",          (ks_obj)ks_iostream_new_extern(stdin, "r")},
        {"stdout",         (ks_obj)ks_iostream_new_extern(stdout, "w")},
        {"stderr",         (ks_obj)ks_iostream_new_extern(stderr, "w")},
        */
    ));

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
