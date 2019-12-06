/* vm.c - implementing the kscript virtual machine */

#include "kscript.h"

void ks_vm_free(ks_vm* vm) {
    ks_list_free(&vm->stk);
    ks_dict_free(&vm->dict);

    *vm = KS_VM_EMPTY;
}

