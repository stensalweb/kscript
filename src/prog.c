/* prog.c - implementation of program building, management, etc */


#include "kscript.h"

// register string in constant table, return index into constant table
int ksb_strr(ks_prog* prog, ks_str str) {
    int i;
    for (i = 0; i < prog->str_tbl_n; ++i) {
        if (ks_str_eq(prog->str_tbl[i]->_str, str)) return i;
    }
    // else, it doesn't exist, so add it
    i = prog->str_tbl_n++;
    prog->str_tbl = ks_realloc(prog->str_tbl, sizeof(ks_str) * prog->str_tbl_n);
    prog->str_tbl[i] = (kso_str)kso_new_str(str);
    KSO_INCREF(prog->str_tbl[i]);
    return i;
}

// add byte, return index
int ksb_byte(ks_prog* prog, uint8_t byte) {
    int idx = prog->bc_n++;
    prog->bc = ks_realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    return idx;
}

// add some bytes
int ksb_bytes(ks_prog* prog, void* bytes, int n) {
    int idx = prog->bc_n;
    prog->bc_n += n;
    prog->bc = ks_realloc(prog->bc, prog->bc_n);
    //prog->bc[idx] = byte;
    memcpy(prog->bc + idx, bytes, n);
    return idx;
}

// add a structure
#define ksb_struct(_type, ...) { struct _type to_add = (struct _type){ __VA_ARGS__ }; return ksb_bytes(prog, &to_add, sizeof(struct _type)); }
// instruction/opcode only
#define ksb_ionly(_code) { return ksb_byte(prog, _code); }

/* add instructions */

int ksb_noop(ks_prog* prog) ksb_ionly(KS_BC_NOOP);
int ksb_bool(ks_prog* prog, ks_bool val) {
    if (val) ksb_ionly(KS_BC_BOOLT)
    else ksb_ionly(KS_BC_BOOLF)
}

int ksb_int64(ks_prog* prog, int64_t val) ksb_struct(ks_bc_int64, .op = KS_BC_INT64, .val = val )
int ksb_int32(ks_prog* prog, int32_t val) ksb_struct(ks_bc_int32, .op = KS_BC_INT32, .val = val )
int ksb_int16(ks_prog* prog, int16_t val) ksb_struct(ks_bc_int16, .op = KS_BC_INT16, .val = val )
int ksb_float(ks_prog* prog, ks_float val) ksb_struct(ks_bc_float, .op = KS_BC_FLOAT, .val = val )
int ksb_str(ks_prog* prog, ks_str val) ksb_struct(ks_bc_str, .op = KS_BC_STR, .val_idx = ksb_strr(prog, val) )

int ksb_load(ks_prog* prog, ks_str name) ksb_struct(ks_bc_load, .op = KS_BC_LOAD, .name_idx = ksb_strr(prog, name))
int ksb_store(ks_prog* prog, ks_str name) ksb_struct(ks_bc_store, .op = KS_BC_STORE, .name_idx = ksb_strr(prog, name))

int ksb_add(ks_prog* prog) ksb_ionly(KS_BC_ADD);
int ksb_sub(ks_prog* prog) ksb_ionly(KS_BC_SUB);
int ksb_mul(ks_prog* prog) ksb_ionly(KS_BC_MUL);
int ksb_div(ks_prog* prog) ksb_ionly(KS_BC_DIV);
int ksb_mod(ks_prog* prog) ksb_ionly(KS_BC_MOD);
int ksb_pow(ks_prog* prog) ksb_ionly(KS_BC_POW);
int ksb_lt(ks_prog* prog) ksb_ionly(KS_BC_LT);
int ksb_gt(ks_prog* prog) ksb_ionly(KS_BC_GT);
int ksb_eq(ks_prog* prog) ksb_ionly(KS_BC_EQ);

int ksb_call(ks_prog* prog, uint32_t n_args) ksb_struct(ks_bc_call, .op = KS_BC_CALL, .n_args = n_args)
int ksb_get (ks_prog* prog, uint32_t n_args) ksb_struct(ks_bc_get, .op = KS_BC_GET, .n_args = n_args)
int ksb_set (ks_prog* prog, uint32_t n_args) ksb_struct(ks_bc_set, .op = KS_BC_SET, .n_args = n_args)
int ksb_create_list(ks_prog* prog, uint32_t n_items) ksb_struct(ks_bc_create_list, .op = KS_BC_CREATE_LIST, .n_items = n_items)

int ksb_jmp (ks_prog* prog, int32_t relamt) ksb_struct(ks_bc_jmp, .op = KS_BC_JMP, .relamt = relamt)
int ksb_jmpt(ks_prog* prog, int32_t relamt) ksb_struct(ks_bc_jmp, .op = KS_BC_JMPT, .relamt = relamt)
int ksb_jmpf(ks_prog* prog, int32_t relamt) ksb_struct(ks_bc_jmp, .op = KS_BC_JMPF, .relamt = relamt)

int ksb_discard(ks_prog* prog) ksb_ionly(KS_BC_DISCARD);
int ksb_ret(ks_prog* prog) ksb_ionly(KS_BC_RET);
int ksb_retnone(ks_prog* prog) ksb_ionly(KS_BC_RETNONE);


// dumps to a string
int ks_prog_tostr(ks_prog* prog, ks_str* to) {


    #define DECODE(_type) (*(struct _type*)&(prog->bc[i]))
    #define PASS(_type) i += sizeof(struct _type);

    struct ks_bc_load i_load;
    struct ks_bc_store i_store;
    struct ks_bc_int64 i_int64;
    struct ks_bc_int32 i_int32;
    struct ks_bc_int16 i_int16;
    struct ks_bc_float i_float;
    struct ks_bc_create_list i_create_list;
    struct ks_bc_str i_str;
    struct ks_bc_call i_call;
    struct ks_bc_get i_get;
    struct ks_bc_set i_set;
    struct ks_bc_jmp i_jmp;    

    ks_str_cfmt(to, "");

    int i;
    for (i = 0; i < prog->bc_n; ) {
        ks_bc op = prog->bc[i];
        switch (op) {
        case KS_BC_NOOP:
            PASS(ks_bc);
            ks_str_cfmt(to, "%snoop", to->_);
            break;
        case KS_BC_LOAD:
            i_load = DECODE(ks_bc_load);
            PASS(ks_bc_load);
            ks_str_cfmt(to, "%sload \"%s\"", to->_, prog->str_tbl[i_load.name_idx]->_str._);
            break;
        case KS_BC_STORE:
            i_store = DECODE(ks_bc_store);
            PASS(ks_bc_store);
            ks_str_cfmt(to, "%sstore \"%s\"", to->_, prog->str_tbl[i_store.name_idx]->_str._);
            break;
        case KS_BC_BOOLT:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sconst true", to->_);
            break;
        case KS_BC_BOOLF:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sconst false", to->_);
            break;
        case KS_BC_INT64:
            i_int64 = DECODE(ks_bc_int64);
            PASS(ks_bc_int64);
            ks_str_cfmt(to, "%sconst %ld", to->_, i_int64.val);
            break;
        case KS_BC_INT32:
            i_int32 = DECODE(ks_bc_int32);
            PASS(ks_bc_int32);
            ks_str_cfmt(to, "%sconst %d", to->_, i_int32.val);
            break;
        case KS_BC_INT16:
            i_int16 = DECODE(ks_bc_int16);
            PASS(ks_bc_int16);
            ks_str_cfmt(to, "%sconst %d", to->_, (int)i_int16.val);
            break;
        case KS_BC_STR:
            i_str = DECODE(ks_bc_str);
            PASS(ks_bc_str);
            ks_str_cfmt(to, "%sconst \"%s\"", to->_, prog->str_tbl[i_str.val_idx]->_str._);
            break;
        case KS_BC_ADD:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop +", to->_);
            break;
        case KS_BC_SUB:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop -", to->_);
            break;
        case KS_BC_MUL:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop *", to->_);
            break;
        case KS_BC_DIV:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop /", to->_);
            break;
        case KS_BC_MOD:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop %", to->_);
            break;
        case KS_BC_POW:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop ^", to->_);
            break;
        case KS_BC_LT:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop <", to->_);
            break;
        case KS_BC_GT:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop >", to->_);
            break;
        case KS_BC_EQ:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sop ==", to->_);
            break;
        case KS_BC_CALL:
            i_call = DECODE(ks_bc_call);
            PASS(ks_bc_call);
            ks_str_cfmt(to, "%scall %d", to->_, (int)i_call.n_args);
            break;
        case KS_BC_GET:
            i_get = DECODE(ks_bc_get);
            PASS(ks_bc_get);
            ks_str_cfmt(to, "%sget %d", to->_, (int)i_get.n_args);
            break;
        case KS_BC_SET:
            i_set = DECODE(ks_bc_set);
            PASS(ks_bc_set);
            ks_str_cfmt(to, "%sset %d", to->_, (int)i_set.n_args);
            break;
        case KS_BC_CREATE_LIST:
            i_create_list = DECODE(ks_bc_create_list);
            PASS(ks_bc_create_list);
            ks_str_cfmt(to, "%screate_list %d", to->_, (int)i_create_list.n_items);
            break;

        case KS_BC_RETNONE:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sretnone", to->_);
            break;
        case KS_BC_DISCARD:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sdiscard", to->_);
            break;

        case KS_BC_JMP:
            i_jmp = DECODE(ks_bc_jmp);
            PASS(ks_bc_jmp);
            ks_str_cfmt(to, "%sjmp %+d", to->_, (int)i_jmp.relamt);
            break;
        case KS_BC_JMPT:
            i_jmp = DECODE(ks_bc_jmp);
            PASS(ks_bc_jmp);
            ks_str_cfmt(to, "%sjmpt %+d", to->_, (int)i_jmp.relamt);
            break;
        case KS_BC_JMPF:
            i_jmp = DECODE(ks_bc_jmp);
            PASS(ks_bc_jmp);
            ks_str_cfmt(to, "%sjmpf %+d", to->_, (int)i_jmp.relamt);
            break;
        default:
            PASS(ks_bc);
            ks_str_append(to, KS_STR_CONST("<err>"));
            break;
        }
        
        ks_str_append(to, KS_STR_CONST("\n"));

    }

    return 0;

}




int ks_prog_lbl_add(ks_prog* prog, ks_str name, int idx) {
    int i;
    for (i = 0; i < prog->lbl_n; ++i) {
        if (ks_str_eq(name, prog->lbls[i].name)) {
            prog->lbls[i].idx = idx;
            return i;
        }
    }

    i = prog->lbl_n++;
    prog->lbls = ks_realloc(prog->lbls, sizeof(*prog->lbls) * prog->lbl_n);
    prog->lbls[i].name = ks_str_dup(name);
    prog->lbls[i].idx = idx;

    return i;
}


int ks_prog_lbl_i(ks_prog* prog, ks_str name) {
    int i;
    for (i = 0; i < prog->lbl_n; ++i) {
        if (ks_str_eq(name, prog->lbls[i].name)) {
            return prog->lbls[i].idx;
        }
    }

    return -1;
}

void ks_prog_free(ks_prog* prog) {
    ks_free(prog->bc);
    int i;
    for (i = 0; i < prog->str_tbl_n; ++i) {
        KSO_DECREF(prog->str_tbl[i]);
        //ks_str_free(&prog->str_tbl[i]);
    }
    for (i = 0; i < prog->lbl_n; ++i) {
        ks_str_free(&prog->lbls[i].name);
    }
    ks_free(prog->lbls);
    ks_free(prog->str_tbl);
    *prog = KS_PROG_EMPTY;
}
