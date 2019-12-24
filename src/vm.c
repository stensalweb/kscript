/* vm.c - implementing the kscript virtual machine */

#include "kscript.h"

kso_vm kso_vm_new_empty() {
    kso_vm ret = (kso_vm)ks_malloc(sizeof(*ret));
    ret->type = kso_T_vm;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    ret->stk = KS_LIST_EMPTY;
    ret->globals = KS_DICT_EMPTY;
    ret->call_stk_n = 0;
    return ret;

}

