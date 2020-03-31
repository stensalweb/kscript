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



/* sys.system(cmd) -> result
 *
 * Run a command, and return whether it was successfull
 *
 */
static KS_TFUNC(sys, shell) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str cmd = (ks_str)args[0];
    KS_REQ_TYPE(cmd, ks_type_str, "cmd");

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
        return ks_throw_fmt(ks_type_Error, "uname() function failed!");
    }

    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* constants */
        {"platform",     (ks_obj)ks_fmt_c("%s", unameData.sysname)},

        {"uname", (ks_obj)ks_dict_new_cn((ks_dict_ent_c[]){
            {"name",        (ks_obj)ks_str_new(unameData.sysname) },
            {"arch",        (ks_obj)ks_str_new(unameData.machine) },
            {"version",     (ks_obj)ks_str_new(unameData.version) },
            {"release",     (ks_obj)ks_str_new(unameData.release) },
            {"node",        (ks_obj)ks_str_new(unameData.nodename) },
            {NULL, NULL}
        })},

        /* functions */
        {"shell",        (ks_obj)ks_cfunc_new2(sys_shell_, "sys.shell(cmd)")},
        

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
