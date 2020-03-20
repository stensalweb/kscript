/* cexec/cexec.c - an example module demonstrating how to write a C-extension

This extension is named `cexec`, and runs the a command as a bash/cmd script, returning the full output.

EXAMPLE:

    > import cexec
    > print (cexec.run("ls"))
    (0, "bin
    examples
    include
    ks_config.T.h
    lib
    Makefile
    README.md
    src
    ")

As you can see, you get the exit code of the process (0, since there was no error), and the string output
  from `stdout`, in a tuple. So, you can take `result[0]` to get the int error code, or `result[1]` to get
  the string output

TYPES:

  N/A

FUNCTIONS:

  cexec.run(cmd): runs `cmd` as a shell command with `popen`, returns a tuple with: (exit_code, result_str),
                    capturing the exit code integer, as well as the string of the stdout

*/

// always begin by defining the module information
#define MODULE_NAME "cexec"

// include this since this is a module.
// NOTE: this also includes the `REQ_*` macros which are useful for error generation
#include "ks-module.h"

// to ensure we have the `popen()` C function
#include <stdio.h>

/* now, define our function that runs a given command */

static KS_TFUNC(cexec, run) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str cmd = (ks_str)args[0];
    KS_REQ_TYPE(cmd, ks_type_str, "cmd");

    // file pointer to command output
    FILE* fp = NULL;

    // open a new process
    fp = popen(cmd->chr, "r");
    //if (fp == NULL) return kse_fmt(SIG ": Error running comand '%S'", cmd);
    if (fp == NULL) return ks_throw_fmt(ks_type_Error, "cmd '%S' failed to open!", cmd);

    // create a string builder to add the buffers to
    ks_str_b SB;
    ks_str_b_init(&SB);

    // unlock the GIL, so other threads may go while it is running
    ks_GIL_unlock();

    // keep reading valid buffers until the output is finished
    char buf[4096];
    while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
        ks_str_b_add(&SB, strlen(buf), buf);
    }


    // acquire it back before calling any more functions
    ks_GIL_lock();


    // finish the string builder
    ks_str ret = ks_str_b_get(&SB);

    // check for error code
    ks_int status = ks_int_new(WEXITSTATUS(pclose(fp)));

    // return our tuple. Since we created the string and int, we don't want to record another reference to them

    return (ks_obj)ks_tuple_new_n(2, (ks_obj[]){ (ks_obj)status, (ks_obj)ret });
}

static ks_module get_module() {
    ks_module mod = ks_module_new("cexec");

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* functions */
        {"run", (ks_obj)ks_cfunc_new2(cexec_run_, "cexec.run(cmd)")},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
