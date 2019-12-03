// bytecode.c - implementation of the byte code construction
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


ks_bc ks_bc_new() {
    ks_bc ret = malloc(sizeof(struct ks_bc));
    ret->inst_n = 0;
    ret->inst = NULL;
    ret->const_str_n = 0;
    ret->const_str = NULL;
    return ret;
}

// returns index of string
int ks_bc_const_str_idx(ks_bc bc, ks_str str) {
    int i;
    for (i = 0; i < bc->const_str_n; ++i) {

        if (ks_str_eq(str, bc->const_str[i])) {
            return i;
        }

    }
    return -1;
}

// returns index of string added
int ks_bc_const_str_add(ks_bc bc, ks_str str) {
    int idx = ks_bc_const_str_idx(bc, str);

    if (idx < 0) {
        // add it
        idx = bc->const_str_n++;
        bc->const_str = realloc(bc->const_str, sizeof(ks_str) * bc->const_str_n);
        bc->const_str[idx] = ks_str_dup(str);
    }
    return idx;
}

// just add a number of bytes
int ks_bc_append_bytes(ks_bc bc, void* data, int n) {
    int idx = bc->inst_n;
    // add this many bytes, grow the memory
    bc->inst_n += n;
    bc->inst = realloc(bc->inst, bc->inst_n);
    // set that memory
    memcpy(bc->inst + idx, data, n);
    return idx;
}

// adds a literal uint8, returns index
int ks_bc_append_uint8(ks_bc bc, uint8_t val) {
    return ks_bc_append_bytes(bc, &val, 1);
}

// adds a literal 16 bit integer
int ks_bc_append_int16(ks_bc bc, int16_t val) {
    return ks_bc_append_bytes(bc, &val, sizeof(val));
}

// adds a literal integer value to the instruction set, returns index
int ks_bc_append_int(ks_bc bc, int val) {
    return ks_bc_append_bytes(bc, &val, sizeof(val));
}

// adds a full 8 byte integer
int ks_bc_append_ks_int(ks_bc bc, ks_int val) {
    return ks_bc_append_bytes(bc, &val, sizeof(val));
}

// adds a full 8 byte float
int ks_bc_append_ks_float(ks_bc bc, ks_float val) {
    return ks_bc_append_bytes(bc, &val, sizeof(val));
}


/* build bytecode from instructions */


int ks_bc_noop(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_NOOP);
}

int ks_bc_load(ks_bc bc, ks_str name) {
    int res = ks_bc_append_uint8(bc, KS_BC_LOAD);
    ks_bc_append_int(bc, ks_bc_const_str_add(bc, name));
    return res;
}

int ks_bc_store(ks_bc bc, ks_str name) {
    int res = ks_bc_append_uint8(bc, KS_BC_STORE);
    ks_bc_append_int(bc, ks_bc_const_str_add(bc, name));
    return res;
}

int ks_bc_attr(ks_bc bc, ks_str name) {
    int res = ks_bc_append_uint8(bc, KS_BC_ATTR);
    ks_bc_append_int(bc, ks_bc_const_str_add(bc, name));
    return res;
}

int ks_bc_const_none(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_CONST_NONE);
}
int ks_bc_const_bool_true(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_CONST_BOOL_TRUE);
}
int ks_bc_const_bool_false(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_CONST_BOOL_FALSE);
}
int ks_bc_const_int(ks_bc bc, ks_int val) {
    int res = ks_bc_append_uint8(bc, KS_BC_CONST_INT);
    ks_bc_append_ks_int(bc, val);
    return res;
}
int ks_bc_const_float(ks_bc bc, ks_float val) {
    int res = ks_bc_append_uint8(bc, KS_BC_CONST_FLOAT);
    ks_bc_append_ks_float(bc, val);
    return res;
}
int ks_bc_const_str(ks_bc bc, ks_str val) {
    int res = ks_bc_append_uint8(bc, KS_BC_CONST_STR);
    ks_bc_append_int(bc, ks_bc_const_str_add(bc, val));
    return res;
}

int ks_bc_vcall(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_VCALL);
}

int ks_bc_call(ks_bc bc, int args_n) {
    int res = ks_bc_append_uint8(bc, KS_BC_CALL);
    ks_bc_append_int(bc, args_n);
    return res;
}

int ks_bc_jmp(ks_bc bc, int relamt) {
    int res = ks_bc_append_uint8(bc, KS_BC_JMP);
    ks_bc_append_int(bc, relamt);
    return res;
}
int ks_bc_beqt(ks_bc bc, int relamt) {
    int res = ks_bc_append_uint8(bc, KS_BC_BEQT);
    ks_bc_append_int(bc, relamt);
    return res;
}
int ks_bc_beqf(ks_bc bc, int relamt) {
    int res = ks_bc_append_uint8(bc, KS_BC_BEQF);
    ks_bc_append_int(bc, relamt);
    return res;
}

int ks_bc_ret(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_RET);
}
int ks_bc_throw(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_THROW);
}
int ks_bc_eret(ks_bc bc) {
    return ks_bc_append_uint8(bc, KS_BC_ERET);
}


void ks_bc_free(ks_bc bc) {
    
    free(bc->inst);

    int i;
    for (i = 0; i < bc->const_str_n; ++i) {
        ks_str_free(&bc->const_str[i]);
    }

    free(bc->const_str);

    free(bc);
}


/*

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
*/



