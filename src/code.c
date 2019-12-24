/* code.c - bytecode object */

#include "kscript.h"

kso_code kso_code_new_empty(kso_list v_const) {
    // construct
    kso_code ret = (kso_code)ks_malloc(sizeof(*ret));
    ret->type = kso_T_code;
    ret->flags = KSOF_NONE;
    ret->refcnt = 0;
    if (v_const != NULL) KSO_INCREF(v_const);
    ret->v_const = v_const;

    return ret;
}


/* appending instructions */

// add to constants list, return index
int ksc_add_const(kso_code code, kso obj) {
    int i;
    for (i = 0; i < code->v_const->v_list.len; ++i) {
        kso citem = code->v_const->v_list.items[i];
        if (citem->type == obj->type) {
            // check for strings
            if (obj->type == kso_T_str && ks_str_eq(KSO_CAST(kso_str, citem)->v_str, KSO_CAST(kso_str, obj)->v_str)) {
                return i;
            }
        }
    }

    return ks_list_push(&code->v_const->v_list, obj);
}

// append arbitrary data
int ksc_data(kso_code code, int inst_n, ks_bc* inst) {
    int res = code->bc_n;
    code->bc = ks_realloc(code->bc, code->bc_n += inst_n);
    memcpy(code->bc + res, inst, inst_n);
    return res;
}

// appends a struct
#define APPEND_STRUCT(...) { ksc_data(code, sizeof(__VA_ARGS__), (ks_bc*)&(__VA_ARGS__)); }

// append an instruction-only bc
#define APPEND_INST(_icode) APPEND_STRUCT((struct ks_bc){ .op = (_icode) })

void ksc_noop(kso_code code) APPEND_INST(KS_BC_NOOP);
void ksc_discard(kso_code code) APPEND_INST(KS_BC_DISCARD);
void ksc_boolt(kso_code code) APPEND_INST(KS_BC_BOOLT);
void ksc_boolf(kso_code code) APPEND_INST(KS_BC_BOOLF);
void ksc_int(kso_code code, ks_int v_int) APPEND_STRUCT((struct ks_bc_int64){ .op = KS_BC_INT64, .val = v_int })
void ksc_str(kso_code code, ks_str v_str) APPEND_STRUCT((struct ks_bc_str){ .op = KS_BC_STR, .val_idx = ksc_add_const(code, (kso)kso_str_new(v_str)) })
void ksc_func_lit(kso_code code, kso_kfunc v_kfunc) APPEND_STRUCT((struct ks_bc_func_lit){ .op = KS_BC_FUNC_LIT, .val_idx = ksc_add_const(code, (kso)v_kfunc) })

void ksc_load(kso_code code, ks_str name) APPEND_STRUCT((struct ks_bc_load){ .op = KS_BC_LOAD, .name_idx = ksc_add_const(code, (kso)kso_str_new(name)) })
void ksc_store(kso_code code, ks_str name) APPEND_STRUCT((struct ks_bc_store){ .op = KS_BC_STORE, .name_idx = ksc_add_const(code, (kso)kso_str_new(name)) })

void ksc_jmpt(kso_code code, int relamt) APPEND_INST(KS_BC_JMPT);
void ksc_jmpf(kso_code code, int relamt) APPEND_INST(KS_BC_JMPF);


void ksc_call(kso_code code, int n_args) APPEND_STRUCT((struct ks_bc_call){ .op = KS_BC_CALL, .n_args = n_args })

void ksc_ret(kso_code code) APPEND_INST(KS_BC_RET);
void ksc_retnone(kso_code code) APPEND_INST(KS_BC_RETNONE);

















