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
            } else if (obj->type == kso_T_int && KSO_CAST(kso_int, citem)->v_int == KSO_CAST(kso_int, obj)->v_int) {
                return i;
            }

            if (citem == obj) {
                return i;
            }
        }
    }

    return ks_list_push(&code->v_const->v_list, obj);
}

// add a string to list, return index
int ksc_add_str(kso_code code, ks_str str) {
    int i;
    for (i = 0; i < code->v_const->v_list.len; ++i) {
        kso citem = code->v_const->v_list.items[i];
        // check for strings
        if (citem->type == kso_T_str && ks_str_eq(KSO_CAST(kso_str, citem)->v_str, str)) {
            return i;
        }
    }

    return ks_list_push(&code->v_const->v_list, (kso)kso_str_new(str));
}

// add an int to list, return index
int ksc_add_int(kso_code code, ks_int val) {
    int i;
    for (i = 0; i < code->v_const->v_list.len; ++i) {
        kso citem = code->v_const->v_list.items[i];
        // check for strings
        if (citem->type == kso_T_int && KSO_CAST(kso_int, citem)->v_int == val) {
            return i;
        }
    }

    return ks_list_push(&code->v_const->v_list, (kso)kso_int_new(val));
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
void ksc_const_none(kso_code code) APPEND_STRUCT((struct ks_bc_const){ .op = KS_BC_CONST, .v_idx = ksc_add_const(code, (kso)KSO_NONE) })
void ksc_const_true(kso_code code) APPEND_STRUCT((struct ks_bc_const){ .op = KS_BC_CONST, .v_idx = ksc_add_const(code, (kso)KSO_TRUE) })
void ksc_const_false(kso_code code) APPEND_STRUCT((struct ks_bc_const){ .op = KS_BC_CONST, .v_idx = ksc_add_const(code, (kso)KSO_FALSE) })
void ksc_const_int(kso_code code, ks_int val) APPEND_STRUCT((struct ks_bc_const){ .op = KS_BC_CONST, .v_idx = ksc_add_int(code, val) })
void ksc_const_str(kso_code code, ks_str str) APPEND_STRUCT((struct ks_bc_const){ .op = KS_BC_CONST, .v_idx = ksc_add_str(code, str) })
void ksc_const(kso_code code, kso obj) APPEND_STRUCT((struct ks_bc_const){ .op = KS_BC_CONST, .v_idx = ksc_add_const(code, obj) })

void ksc_load(kso_code code, ks_str name) APPEND_STRUCT((struct ks_bc_nameop){ .op = KS_BC_LOAD, .name_idx = ksc_add_str(code, name) })
void ksc_store(kso_code code, ks_str name) APPEND_STRUCT((struct ks_bc_nameop){ .op = KS_BC_STORE, .name_idx = ksc_add_str(code, name) })

/*void ksc_int(kso_code code, ks_int v_int) APPEND_STRUCT((struct ks_bc_int){ .op = KS_BC_INT, .val = v_int })
void ksc_str(kso_code code, ks_str v_str) APPEND_STRUCT((struct ks_bc_str){ .op = KS_BC_STR, .val_idx = ksc_add_const(code, (kso)kso_str_new(v_str)) })
void ksc_func_lit(kso_code code, kso_kfunc v_kfunc) APPEND_STRUCT((struct ks_bc_func_lit){ .op = KS_BC_FUNC_LIT, .val_idx = ksc_add_const(code, (kso)v_kfunc) })
*/

void ksc_jmpt(kso_code code, int relamt) APPEND_STRUCT((struct ks_bc_jmp){ .op = KS_BC_JMPT, .relamt = relamt })
void ksc_jmpf(kso_code code, int relamt) APPEND_STRUCT((struct ks_bc_jmp){ .op = KS_BC_JMPF, .relamt = relamt })

void ksc_add(kso_code code) APPEND_INST(KS_BC_ADD);

void ksc_call(kso_code code, int n_args) APPEND_STRUCT((struct ks_bc_call){ .op = KS_BC_CALL, .n_args = n_args })

void ksc_ret(kso_code code) APPEND_INST(KS_BC_RET);
void ksc_retnone(kso_code code) APPEND_INST(KS_BC_RETNONE);



/* output to string */


// dumps to a string
void kso_code_tostr(kso_code code, ks_str* to) {

    #define PASS(_type) { i += sizeof(struct _type); }
    #define DECODE(_type) { *((struct _type*)&inst) = (*(struct _type*)&(code->bc[i])); PASS(_type) }
    #define GET_CONST(_idx) ((code->v_const->v_list.items[_idx]))

    ks_BC inst;

    ks_str_cfmt(to, "");

    int i;
    for (i = 0; i < code->bc_n; ) {
        ks_bc op = code->bc[i];
        switch (op) {
        case KS_BC_NOOP:
            DECODE(ks_bc);
            ks_str_cfmt(to, "%snoop", to->_);
            break;
        case KS_BC_LOAD:
            DECODE(ks_bc_nameop);
            //ks_str_cfmt(to, "%sload \"%s\"", to->_, prog->str_tbl[i_load.name_idx]->_str._);
            ks_str_cfmt(to, "%sload \"%s\"", to->_, ((kso_str)GET_CONST(inst.nameop.name_idx))->v_str._);
            break;
        case KS_BC_STORE:
            DECODE(ks_bc_nameop);
            ks_str_cfmt(to, "%sstore \"%s\"", to->_, ((kso_str)GET_CONST(inst.nameop.name_idx))->v_str._);
            break;
        case KS_BC_CONST:
            DECODE(ks_bc_const);
            kso v_const = GET_CONST(inst.v_const.v_idx);
            if (v_const->type == kso_T_none) {
                ks_str_cfmt(to, "%sconst none", to->_);
            } else if (v_const->type == kso_T_bool) {
                ks_str_cfmt(to, "%sconst %s", to->_, ((kso_bool)v_const)->v_bool ? "true" : "false");
            } else if (v_const->type == kso_T_int) {
                ks_str_cfmt(to, "%sconst %lld", to->_, ((kso_int)v_const)->v_int);
            } else if (v_const->type == kso_T_str) {
                ks_str_cfmt(to, "%sconst \"%s\"", to->_, ((kso_str)v_const)->v_str._);
            } else {
                ks_str_cfmt(to, "%sconst <obj @ %p>", to->_, v_const);
            }
            break;

        case KS_BC_CALL:
            DECODE(ks_bc_call);
            ks_str_cfmt(to, "%scall %d", to->_, inst.call.n_args);
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

/*
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
            */

        case KS_BC_RETNONE:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sretnone", to->_);
            break;
        case KS_BC_DISCARD:
            PASS(ks_bc);
            ks_str_cfmt(to, "%sdiscard", to->_);
            break;


        case KS_BC_JMP:
            DECODE(ks_bc_jmp);
            ks_str_cfmt(to, "%sjmp %+d", to->_, inst.jmp.relamt);
            break;
        case KS_BC_JMPT:
            DECODE(ks_bc_jmp);
            ks_str_cfmt(to, "%sjmpt %+d", to->_, inst.jmp.relamt);
            break;
        case KS_BC_JMPF:
            DECODE(ks_bc_jmp);
            ks_str_cfmt(to, "%sjmpf %+d", to->_, inst.jmp.relamt);
            break;
        default:
            PASS(ks_bc);
            ks_str_append(to, KS_STR_CONST("<err>"));
            break;
        }
        
        ks_str_append(to, KS_STR_CONST("\n"));

    }

    return;
}














