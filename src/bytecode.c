// bytecode.c - implementation of the byte code construction
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

/* loads/stores */
ks_bc_inst ks_bc_load(ks_str name) {
    return (ks_bc_inst){ .type = KS_BC_LOAD, ._name = ks_str_dup(name) };
}
ks_bc_inst ks_bc_store(ks_str name) {
    return (ks_bc_inst){ .type = KS_BC_STORE, ._name = ks_str_dup(name) };
}
ks_bc_inst ks_bc_attr(ks_str attr) {
    return (ks_bc_inst){ .type = KS_BC_ATTR, ._name = ks_str_dup(attr) };
}


/* constants */
ks_bc_inst ks_bc_none() {
    return (ks_bc_inst){ .type = KS_BC_CONST_NONE };
}
ks_bc_inst ks_bc_int(ks_int val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_INT, ._int = val };
}
ks_bc_inst ks_bc_bool(ks_bool val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_BOOL, ._bool = val };
}
ks_bc_inst ks_bc_float(ks_float val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_FLOAT, ._float = val };
}
ks_bc_inst ks_bc_str(ks_str val) {
    return (ks_bc_inst){ .type = KS_BC_CONST_STR, ._str = ks_str_dup(val) };
}

/* calls */
ks_bc_inst ks_bc_call(int args_n) {
    return (ks_bc_inst){ .type = KS_BC_CALL, ._args_n = args_n };
}
ks_bc_inst ks_bc_vcall() {
    return (ks_bc_inst){ .type = KS_BC_VCALL };
}

/* conditionals */

ks_bc_inst ks_bc_beqt(int relamt) {
    return (ks_bc_inst){ .type = KS_BC_BEQT, ._relamt = relamt };
}
ks_bc_inst ks_bc_beqf(int relamt) {
    return (ks_bc_inst){ .type = KS_BC_BEQF, ._relamt = relamt };
}
ks_bc_inst ks_bc_jmp(int relamt) {
    return (ks_bc_inst){ .type = KS_BC_JMP, ._relamt = relamt };
}

/* return */
ks_bc_inst ks_bc_ret() {
    return (ks_bc_inst){ .type = KS_BC_RET };
}

/* exceptions */
ks_bc_inst ks_bc_throw() {
    return (ks_bc_inst){ .type = KS_BC_THROW };
}
ks_bc_inst ks_bc_eret() {
    return (ks_bc_inst){ .type = KS_BC_ERET };
}


// frees an instruction and its resources
void ks_bc_inst_free(ks_bc_inst* inst) {
    // just free things that require it
    if (inst->type == KS_BC_LOAD || inst->type == KS_BC_STORE || inst->type == KS_BC_ATTR) {
        ks_str_free(&inst->_name);
    } else if (inst->type == KS_BC_CONST_STR) {
        ks_str_free(&inst->_str);
    }
}

int ks_bc_add(ks_bc* bc, ks_bc_inst inst) {
    int idx = bc->inst_len++;
    bc->inst = realloc(bc->inst, bc->inst_len * sizeof(ks_bc_inst));
    bc->inst[idx] = inst;
    return idx;
}

// adds a label to an index into the internal instruction list
void ks_bc_label(ks_bc* bc, ks_str name, int lidx) {
    int idx = bc->labels_len++;
    bc->labels = realloc(bc->labels, sizeof(ks_str) * bc->labels_len);
    bc->labels_idx = realloc(bc->labels_idx, sizeof(int) * bc->labels_len);
    bc->labels[idx] = ks_str_dup(name);
    bc->labels_idx[idx] = lidx;


}




