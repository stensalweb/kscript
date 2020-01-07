/* cexec.c - an example module demonstrating how to write a C-extension

This extension is named `cexec`, and runs the a command as a bash/cmd script, returning the full output

FUNCTIONS:

  cexec.run(cmd): runs `cmd` as a shell command with `popen`, returns a tuple with: (exit_code, result_str),
                    capturing the exit code integer, as well as the string of the stdout

*/

#define MODULE_NAME "cexec"

// since this is a module
#include "ks_module.h"

// for helper macros
#include "ks_common.h"

// for our popen() usage
#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>

/* now, define our function */

MFUNC(cexec, run) {
    #define SIG "cexec.run(cmd)"
    REQ_N_ARGS(1);
    ks_str cmd = (ks_str)args[0];
    REQ_TYPE("cmd", cmd, ks_T_str);

    // file pointer to command output
    FILE* fp = NULL;

    // open a new process
    fp = popen(cmd->chr, "r");
    if (fp == NULL) return kse_fmt(SIG ": Error running comand '%S'", cmd);

    // create a string builder to add the buffers to
    ks_strB result = ks_strB_create();

    // keep reading valid buffers until the output is finished
    char buf[4096];
    while (fgets(buf, sizeof(buf) - 1, fp) != NULL) {
        ks_strB_add(&result, buf, strlen(buf));
    }

    // finish the string builder
    ks_str ret = ks_strB_finish(&result);

    // check for error code
    ks_int status = ks_int_new(pclose(fp));

    // return our tuple. Since we created the string and int, we don't want to record another reference to them
    return (kso)ks_tuple_new_norefs((kso[]){ (kso)status, (kso)ret }, 2);
    #undef SIG
}


MODULE_INIT() {
    // create our new module
    ks_module mod = ks_module_new_c("cexec");

    // our our function
    MODULE_ADD_CFUNC(mod, "run", cexec_run_);
    
    // return our module
    return (kso)mod;
}

// finalize everything
MODULE_END();

