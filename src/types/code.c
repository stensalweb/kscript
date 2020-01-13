/* types/code.c - represents a bytecode object */

#include "ks_common.h"


// create a new empty piece of code
ks_code ks_code_new_empty(ks_list v_const) {
    ks_code self = (ks_code)ks_malloc(sizeof(*self));
    *self = (struct ks_code) {
        KSO_BASE_INIT(ks_T_code)
        .v_const = v_const,
        .bc_n = 0, .bc = NULL,
        .meta_ast_n = 0, .meta_ast = NULL
    };

    // record our reference
    KSO_INCREF(v_const);
    return self;
}

// set the meta-ast
void ks_code_add_meta(ks_code self, ks_ast ast) {

    int idx = self->meta_ast_n++;
    self->meta_ast = ks_realloc(self->meta_ast, sizeof(*self->meta_ast) * self->meta_ast_n);
    self->meta_ast[idx].ast = ast;
    self->meta_ast[idx].bc_n = self->bc_n;

}

// link in another code object, appending it to the end
void ks_code_linkin(ks_code self, ks_code other) {
    ksbc inst;

    // the constant value, looked up
    kso v_c;

    // call like DECODE(ksbc_), or DECODE(ksbc_i32) to read and pass an instruction of that
    //   kind at the program counter, incrementing the program counter
    #define DECODE(_inst_type) { inst = (ksbc)(*(_inst_type*)(&other->bc[i])); i += sizeof(_inst_type); }

    // get the constant
    #define GET_CONST(_idx) (other->v_const->items[_idx])

    int i;
    for (i = 0; i < other->bc_n; ) {
        switch (other->bc[i])
        {
        case KSBC_NOOP:
            DECODE(ksbc_);
            ksc_noop(self);
            break;
        
        case KSBC_CONST:
            DECODE(ksbc_i32);
            v_c = GET_CONST(inst.i32.i32);
            ksc_const(self, v_c);
            break;
        
        case KSBC_POPU:
            DECODE(ksbc_);
            ksc_popu(self);
            break;
        
        case KSBC_RET:
            DECODE(ksbc_);
            ksc_ret(self);
            break;
        
        case KSBC_RET_NONE:
            DECODE(ksbc_);
            ksc_ret_none(self);
            break;

        case KSBC_LOAD:
            DECODE(ksbc_i32);
            v_c = GET_CONST(inst.i32.i32);
            ksc_loado(self, v_c);
            break;

        case KSBC_STORE:
            DECODE(ksbc_i32);
            v_c = GET_CONST(inst.i32.i32);
            ksc_storeo(self, v_c);
            break;


        case KSBC_CALL:
            DECODE(ksbc_i32);
            ksc_call(self, inst.i32.i32);
            break;
        case KSBC_ADD:
            DECODE(ksbc_);
            ksc_add(self);
            break;
        case KSBC_SUB:
            DECODE(ksbc_);
            ksc_sub(self);
            break;
        case KSBC_MUL:
            DECODE(ksbc_);
            ksc_mul(self);
            break;
        case KSBC_DIV:
            DECODE(ksbc_);
            ksc_div(self);
            break;


        default:
            kse_fmt("While linking (code @ %p) into (code @ %p), unknown instruction was encountered: %i (at pos=%i)", other, self, (int)other->bc[i], i);
            return;
            break;
        }

    }

}

/* code generation helpers */

void ksc_addbytes(ks_code code, int size, uint8_t* new_bytes) {
    int start = code->bc_n;
    code->bc = ks_realloc(code->bc, code->bc_n += size);
    memcpy(&code->bc[start], new_bytes, size);
}

// add a constant to the `v_const` list, returning the index
int ksc_refconst(ks_code code, kso val) {
    int i;
    for (i = 0; i < code->v_const->len; ++i) {
        // try and find a match, and just return that index instead of adding it
        if (kso_eq(code->v_const->items[i], val)) {
            return i;
        }
    }

    // return the index it was added at
    return ks_list_push(code->v_const, val);
}


// add a constant reference to the `v_int` value
int ksc_refconst_int(ks_code code, int64_t v_int) {
    ks_int myint = ks_int_new(v_int);
    int ret = ksc_refconst(code, (kso)myint);
    KSO_DECREF(myint);
    return ret;
}

// add a constant reference to the `v_const` list, returning the index
int ksc_refconst_str(ks_code code, const char* cstr, int len) {
    ks_str mystr = ks_str_new_l(cstr, len);
    int ret = ksc_refconst(code, (kso)mystr);
    KSO_DECREF(mystr);
    return ret;
}


// helper macro to add an instruction only
#define KSC_(_bc) { ksc_addbytes(code, sizeof(ksbc_), (uint8_t*)&(ksbc_){ .op = _bc }); }

#define KSC_I32(_bc, _i32) { ksc_addbytes(code, sizeof(ksbc_i32), (uint8_t*)&(ksbc_i32){ .op = _bc, .i32 = _i32 }); }

/* bytecode generation */

/* bytecode generation helpers */

// noop; do nothing
void ksc_noop     (ks_code code) KSC_(KSBC_NOOP)
// popu; pop unused value
void ksc_popu     (ks_code code) KSC_(KSBC_POPU)
// dup value 
void ksc_dup      (ks_code code) KSC_(KSBC_DUP)


// load "v_name"; loads a value
void ksc_load     (ks_code code, const char* v_name) KSC_I32(KSBC_LOAD, ksc_refconst_str(code, v_name, strlen(v_name)))
// load_a "v_attr"; replaces the top item with its attribute `v_attr`
void ksc_load_a   (ks_code code, const char* v_attr) KSC_I32(KSBC_LOAD_A, ksc_refconst_str(code, v_attr, strlen(v_attr)))

// store "v_name"; takes top value and stores it as a variable `v_name`
void ksc_store    (ks_code code, const char* v_name) KSC_I32(KSBC_STORE, ksc_refconst_str(code, v_name, strlen(v_name)))
// store "v_attr"; takes top two values (obj, value), and sets `obj.v_attr = value`, 
//   then takes off obj, value, and just pops on value
void ksc_store_a  (ks_code code, const char* v_attr) KSC_I32(KSBC_STORE_A, ksc_refconst_str(code, v_attr, strlen(v_attr)))

/* different versions for specific argument types */

// length encoded
void ksc_loadl    (ks_code code, const char* v_name, int len) KSC_I32(KSBC_LOAD, ksc_refconst_str(code, v_name, len))
void ksc_load_al  (ks_code code, const char* v_attr, int len) KSC_I32(KSBC_LOAD_A, ksc_refconst_str(code, v_attr, len))
void ksc_storel   (ks_code code, const char* v_name, int len) KSC_I32(KSBC_STORE, ksc_refconst_str(code, v_name, len))
void ksc_store_al (ks_code code, const char* v_attr, int len) KSC_I32(KSBC_STORE_A, ksc_refconst_str(code, v_attr, len))

// object format
void ksc_loado    (ks_code code, kso o_name) KSC_I32(KSBC_LOAD, ksc_refconst(code, o_name))
void ksc_load_ao  (ks_code code, kso o_attr) KSC_I32(KSBC_LOAD_A, ksc_refconst(code, o_attr))
void ksc_storeo   (ks_code code, kso o_name) KSC_I32(KSBC_STORE, ksc_refconst(code, o_name))
void ksc_store_ao (ks_code code, kso o_attr) KSC_I32(KSBC_STORE_A, ksc_refconst(code, o_attr))

// const val; pushes a constant
void ksc_const    (ks_code code, kso val) KSC_I32(KSBC_CONST, ksc_refconst(code, val))
// const true; pushes a true value
void ksc_const_true(ks_code code) KSC_(KSBC_CONST_TRUE)
// const false; pushes a false value
void ksc_const_false(ks_code code) KSC_(KSBC_CONST_FALSE)
// const none; pushes a none value
void ksc_const_none(ks_code code) KSC_(KSBC_CONST_NONE)
// const `v_int`; pushes a literal integer
void ksc_int      (ks_code code, int64_t v_int) KSC_I32(KSBC_CONST, ksc_refconst_int(code, v_int))

/* different constant loaders for C-strings */

void ksc_cstr      (ks_code code, const char* v_cstr) KSC_I32(KSBC_CONST, ksc_refconst_str(code, v_cstr, strlen(v_cstr)))
void ksc_cstrl     (ks_code code, const char* v_cstr, int len) KSC_I32(KSBC_CONST, ksc_refconst_str(code, v_cstr, len))

// call n_items; pops on a function call on the last n_items (includes the function)
void ksc_call      (ks_code code, int n_items) KSC_I32(KSBC_CALL, n_items)
void ksc_getitem   (ks_code code, int n_items) KSC_I32(KSBC_GETITEM, n_items)
void ksc_setitem   (ks_code code, int n_items) KSC_I32(KSBC_SETITEM, n_items)
// tuple n_items; creates a tuple from the last n_items
void ksc_tuple     (ks_code code, int n_items) KSC_I32(KSBC_TUPLE, n_items)
// list n_items; creates a list from the last n_items
void ksc_list      (ks_code code, int n_items) KSC_I32(KSBC_LIST, n_items)

/* binary operators */
void ksc_add       (ks_code code) KSC_(KSBC_ADD)
void ksc_sub       (ks_code code) KSC_(KSBC_SUB)
void ksc_mul       (ks_code code) KSC_(KSBC_MUL)
void ksc_div       (ks_code code) KSC_(KSBC_DIV)
void ksc_mod       (ks_code code) KSC_(KSBC_MOD)
void ksc_pow       (ks_code code) KSC_(KSBC_POW)
void ksc_lt        (ks_code code) KSC_(KSBC_LT)
void ksc_le        (ks_code code) KSC_(KSBC_LE)
void ksc_gt        (ks_code code) KSC_(KSBC_GT)
void ksc_ge        (ks_code code) KSC_(KSBC_GE)
void ksc_eq        (ks_code code) KSC_(KSBC_EQ)
void ksc_ne        (ks_code code) KSC_(KSBC_NE)

/* branching/conditionals */
void ksc_jmp       (ks_code code, int relamt) KSC_I32(KSBC_JMP, relamt)
void ksc_jmpt      (ks_code code, int relamt) KSC_I32(KSBC_JMPT, relamt)
void ksc_jmpf      (ks_code code, int relamt) KSC_I32(KSBC_JMPF, relamt)

/* returning/control flow */
void ksc_ret       (ks_code code) KSC_(KSBC_RET)
void ksc_ret_none  (ks_code code) KSC_(KSBC_RET_NONE)

/* exception handling */
void ksc_exc_add   (ks_code code, int abspos) KSC_I32(KSBC_EXC_ADD, abspos)
void ksc_exc_rem   (ks_code code) KSC_(KSBC_EXC_REM)

// called when the object should be freed

TFUNC(code, free) {

    // get the arguments
    ks_code self = (ks_code)args[0];


    // deref the constant pool
    KSO_DECREF(self->v_const);

    // free the bytecode
    ks_free(self->bc);

    // free meta
    ks_free(self->meta_ast);

    ks_free(self);

    return KSO_NONE;
}

/* exporting functionality */

struct ks_type T_code, *ks_T_code = &T_code;

void ks_init__code() {

    /* create the type */
    T_code = KS_TYPE_INIT();
    
    ks_type_setname_c(ks_T_code, "code");

    // add cfuncs
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }
    
    ADDCF(ks_T_code, "__free__", "code.__free__(self)", code_free_);
}


