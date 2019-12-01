// bytecode.c - implementation of the byte code construction
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

ks_bc_inst ks_bc_new_load(ks_str name) {
    return (ks_bc_inst){ .type = KS_BC_LOAD, ._load = ks_str_dup(name) };
}

ks_bc_inst ks_bc_new_store(ks_str name) {
    return (ks_bc_inst){ .type = KS_BC_STORE, ._store = ks_str_dup(name) };
}
ks_bc_inst ks_bc_new_attr(ks_str attr) {
    return (ks_bc_inst){ .type = KS_BC_ATTR, ._attr = ks_str_dup(attr) };
}
ks_bc_inst ks_bc_new_ret() {
    return (ks_bc_inst){ .type = KS_BC_RET };
}

ks_bc_inst ks_bc_new_ret_none() {
    return (ks_bc_inst){ .type = KS_BC_RET_NONE };
}

ks_bc_inst ks_bc_new_int(ks_int val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_INT, ._int = val };
}

ks_bc_inst ks_bc_new_float(ks_float val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_FLOAT, ._float = val };
}

ks_bc_inst ks_bc_new_str(ks_str val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_STR, ._str = ks_str_dup(val) };
}

ks_bc_inst ks_bc_new_jmpi(int relamt) {
    return (ks_bc_inst){ .type = KS_BC_JMPI, ._jmpi = relamt };
}

ks_bc_inst ks_bc_new_jmpc(int relamt) {
    return (ks_bc_inst){ .type = KS_BC_JMPC, ._jmpc = relamt };
}

ks_bc_inst ks_bc_new_call(int args_n) {
    return (ks_bc_inst){ .type = KS_BC_CALL, ._call.args_n = args_n };
}

ks_bc_inst ks_bc_new_throw() {
    return (ks_bc_inst){ .type = KS_BC_THROW };
}

ks_bc_inst ks_bc_new_exit() {
    return (ks_bc_inst){ .type = KS_BC_EXIT };
}

void ks_bc_inst_free(ks_bc_inst* inst) {
    // just free things that require it
    if (inst->type == KS_BC_LOAD) {
        ks_str_free(&inst->_load);
    } else if (inst->type == KS_BC_STORE) {
        ks_str_free(&inst->_store);
    } else if (inst->type == KS_BC_CONST_STR) {
        ks_str_free(&inst->_str);
    }
}

int ks_bc_add(ks_bc* bc, ks_bc_inst inst) {
    int idx = bc->len++;
    bc->inst = realloc(bc->inst, bc->len * sizeof(ks_bc_inst));
    bc->inst[idx] = inst;
    return idx;
}



