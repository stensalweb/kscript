/* cexec.c - an example module demonstrating how to write a C-extension */

#define MODULE_NAME "cexec"

// since this is a module
#include "ks_module.h"

// for helper macros
#include "ks_common.h"

MFUNC(cexec, run) {
    #define SIG "cexec.run(cmd)"
    REQ_N_ARGS(1);
    ks_str cmd = (ks_str)args[0];
    REQ_TYPE("cmd", cmd, ks_T_str);

    KSO_INCREF(cmd);

    return (kso)cmd;
    #undef SIG
}

MODULE_INIT_FUNC() {

    ks_str name = ks_str_new("cexec");
    ks_module mod = ks_module_new(name);
    KSO_DECREF(name);

    return (kso)mod;
}

// finalize everything
MODULE_END();

