/* prog.c - implementation of program building, management, etc */


#include "kscript.h"

// register string in constant table, return index into constant table

int ksb_sta(ks_prog* prog, ks_str str) {
    int i;
    for (i = 0; i < prog->str_tbl_n; ++i) {
        if (ks_str_eq(prog->str_tbl[i], str)) return i;
    }
    // else, it doesn't exist, so add it
    i = prog->str_tbl_n++;
    prog->str_tbl = realloc(prog->str_tbl, sizeof(ks_str) * prog->str_tbl_n);
    prog->str_tbl[i] = ks_str_dup(str);
    return i;
}

// add byte, return index
int ksb_ab(ks_prog* prog, uint8_t byte) {
    int idx = prog->bc_n++;
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    return idx;
}

// adds two bytes
int ksb_abb(ks_prog* prog, uint8_t byte, uint8_t ob) {
    int idx = prog->bc_n;
    prog->bc_n += 2;
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    prog->bc[idx+1] = ob;
    return idx;
}

// add byte, 2 byte integer, return index
int ksb_abi2(ks_prog* prog, uint8_t byte, int16_t i2) {
    int idx = prog->bc_n;
    prog->bc_n += 1 + sizeof(i2);
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    ((int16_t *)(prog->bc + idx + 1))[0] = i2;
    return idx;
}

// add byte, 4 byte integer, return index
int ksb_abi4(ks_prog* prog, uint8_t byte, int32_t i4) {
    int idx = prog->bc_n;
    prog->bc_n += 1 + sizeof(i4);
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    ((int32_t *)(prog->bc + idx + 1))[0] = i4;
    return idx;
}

// add byte, 8 byte integer, return index
int ksb_abi8(ks_prog* prog, uint8_t byte, int64_t i8) {
    int idx = prog->bc_n;
    prog->bc_n += 1 + sizeof(i8);
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    ((int64_t *)(prog->bc + idx + 1))[0] = i8;
    return idx;
}

// add byte, 8 byte float, return index
int ksb_abf(ks_prog* prog, uint8_t byte, double f) {
    int idx = prog->bc_n;
    prog->bc_n += 1 + sizeof(f);
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    ((double *)(prog->bc + idx + 1))[0] = f;
    return idx;
}

// add byte, register string, then add str_tbl_idx, then return index 
int ksb_abs(ks_prog* prog, uint8_t byte, ks_str str) {
    return ksb_abi4(prog, byte, ksb_sta(prog, str));
}


/* add instructions */
int ksb_noop(ks_prog* prog) { return ksb_ab(prog, KS_BC_NOOP); }
int ksb_bool(ks_prog* prog, ks_bool val) { return ksb_abb(prog, KS_BC_BOOL, val); }
int ksb_int(ks_prog* prog, ks_int val) { return ksb_abi8(prog, KS_BC_INT, val); }
int ksb_float(ks_prog* prog, ks_float val) { return ksb_abf(prog, KS_BC_FLOAT, val); }
int ksb_load(ks_prog* prog, ks_str name) { return ksb_abs(prog, KS_BC_LOAD, name); }
int ksb_store(ks_prog* prog, ks_str name) { return ksb_abs(prog, KS_BC_STORE, name); }
int ksb_call(ks_prog* prog, int16_t n_args) { return ksb_abi2(prog, KS_BC_CALL, n_args); }
int ksb_typeof(ks_prog* prog) { return ksb_ab(prog, KS_BC_TYPEOF); }
int ksb_ret(ks_prog* prog) { return ksb_ab(prog, KS_BC_RET); }
int ksb_retnone(ks_prog* prog) { return ksb_ab(prog, KS_BC_RETNONE); }



void ks_prog_free(ks_prog* prog) {
    free(prog->bc);
    int i;
    for (i = 0; i < prog->str_tbl_n; ++i) {
        ks_str_free(&prog->str_tbl[i]);
    }
    free(prog->str_tbl);
    *prog = KS_PROG_EMPTY;
}
