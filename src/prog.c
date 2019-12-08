/* prog.c - implementation of program building, management, etc */


#include "kscript.h"

// register string in constant table, return index into constant table
int ksb_strr(ks_prog* prog, ks_str str) {
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
int ksb_byte(ks_prog* prog, uint8_t byte) {
    int idx = prog->bc_n++;
    prog->bc = realloc(prog->bc, prog->bc_n);
    prog->bc[idx] = byte;
    return idx;
}

// add some bytes
int ksb_bytes(ks_prog* prog, void* bytes, int n) {
    int idx = prog->bc_n;
    prog->bc_n += n;
    prog->bc = realloc(prog->bc, prog->bc_n);
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
int ksb_int(ks_prog* prog, ks_int val) ksb_struct(ks_bc_int, .op = KS_BC_INT, .val = val )
int ksb_float(ks_prog* prog, ks_float val) ksb_struct(ks_bc_float, .op = KS_BC_FLOAT, .val = val )
int ksb_str(ks_prog* prog, ks_str val) ksb_struct(ks_bc_str, .op = KS_BC_STR, .val_idx = ksb_strr(prog, val) )
int ksb_load(ks_prog* prog, ks_str name) ksb_struct(ks_bc_load, .op = KS_BC_LOAD, .name_idx = ksb_strr(prog, name))
int ksb_store(ks_prog* prog, ks_str name) ksb_struct(ks_bc_store, .op = KS_BC_STORE, .name_idx = ksb_strr(prog, name))
int ksb_call(ks_prog* prog, uint32_t n_args) ksb_struct(ks_bc_call, .op = KS_BC_CALL, .n_args = n_args)
int ksb_get(ks_prog* prog, uint32_t n_args) ksb_struct(ks_bc_get, .op = KS_BC_GET, .n_args = n_args)
int ksb_set(ks_prog* prog, uint32_t n_args) ksb_struct(ks_bc_set, .op = KS_BC_SET, .n_args = n_args)
int ksb_new_list(ks_prog* prog, uint32_t n_items) ksb_struct(ks_bc_new_list, .op = KS_BC_NEW_LIST, .n_items = n_items)

int ksb_jmpt(ks_prog* prog, int32_t relamt) ksb_struct(ks_bc_jmpt, .op = KS_BC_JMPT, .relamt = relamt)
int ksb_jmpf(ks_prog* prog, int32_t relamt) ksb_struct(ks_bc_jmpf, .op = KS_BC_JMPF, .relamt = relamt)

int ksb_typeof(ks_prog* prog) ksb_ionly(KS_BC_TYPEOF);
int ksb_ret(ks_prog* prog) ksb_ionly(KS_BC_RET);
int ksb_retnone(ks_prog* prog) ksb_ionly(KS_BC_RETNONE);

// parses source code and add its to the program,
// sets `errmsg` if something went wrong
// returns the error code (0 on success)
int ksb_parse(ks_prog* prog, ks_str src, ks_str* errmsg) {
    int lineno = 0, colno = 0;
    int i;
    ks_str ident = KS_STR_EMPTY;
    #define ADV_ONE { if (c == '\n') { lineno++; colno = 0; } else colno++; c = src._[++i]; }
    #define PARSE_STR { \
        ks_str_copy(&ident, KS_STR_CONST("")); \
        while (c && c != '\"') { \
            if (c == '\\') { \
                if (c == 'n') { \
                    ks_str_append_c(&ident, '\n'); \
                } else { \
                    ks_str_fmt(errmsg, "While parsing str literal, unknown escape code '\\%c', at line %d, col %d", c, lineno, colno); \
                    ks_str_free(&ident); \
                    return 1; \
                } \
            } else { \
                ks_str_append_c(&ident, c); \
            } \
            ADV_ONE; \
        } \
    }

    for (i = 0; i < src.len; ++i) {
        char c = src._[i];
        while (c && c == ' ' || c == '\n' || c == '\t' || c == ';'){
            ADV_ONE;
        }

        // comment
        if (c == '#') {
            while (c && c != '\n') {
                ADV_ONE;
            }
            ADV_ONE;
        }

        if (!c) break;
        
        ks_str_copy(&ident, KS_STR_CONST(""));

        // first, parse an identifier
        while (c && ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')) {
            ks_str_append_c(&ident, c);
            ADV_ONE;
        }

        if (ident.len < 2) {
            ks_str_fmt(errmsg, "Expected an identifier at line %d, col %d", lineno, colno);
            ks_str_free(&ident);
            return 1;
        }

        //if (!c) break;

        // now, check all known instructions
        if (ks_str_eq(ident, KS_STR_CONST("noop"))) {
            ksb_noop(prog);
        } else if (ks_str_eq(ident, KS_STR_CONST("const"))) {
            // check for what kind of constant
            if (c != ' ') {
                ks_str_fmt(errmsg, "Instruction '%s' should have a space after it, at %d, col %d", ident._, lineno, colno);
                ks_str_free(&ident);
                return 1;
            }
            // advance one
            ADV_ONE;
            
            if (c == '\"') {
                // string constant
                ADV_ONE;


                if (c != '\"') {
                    ks_str_fmt(errmsg, "Instruction 'const' (str) should have a an ending '\"' at %d, col %d", lineno, colno);
                    ks_str_free(&ident);
                    return 1;
                }
                ADV_ONE;


            }

        } else if (ks_str_eq(ident, KS_STR_CONST("load"))) {

        }

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
    prog->lbls = realloc(prog->lbls, sizeof(*prog->lbls) * prog->lbl_n);
    prog->lbls[i].name = ks_str_dup(name);
    prog->lbls[i].idx = idx;

    return i;
}


void ks_prog_free(ks_prog* prog) {
    free(prog->bc);
    int i;
    for (i = 0; i < prog->str_tbl_n; ++i) {
        ks_str_free(&prog->str_tbl[i]);
    }
    for (i = 0; i < prog->lbl_n; ++i) {
        ks_str_free(&prog->lbls[i].name);
    }
    free(prog->lbls);
    free(prog->str_tbl);
    *prog = KS_PROG_EMPTY;
}
