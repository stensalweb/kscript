/* kso.c - implementation of the builtin object types */

#include "ks.h"

// have the types
static struct ks_type 
    T_none,
    T_bool,
    T_int,
    T_str,
    T_tuple,
    T_list,
    T_dict,
    T_type,
    T_code,
    T_kfunc,
    T_cfunc,
    T_ast,
    T_parser,
    T_vm
;

// construct the `none` global value
static struct ks_none 
    V_none = { KSO_BASE_INIT_R(&T_none, KSOF_NONE, 1) }
;

// construct the 2 booleans
static struct ks_bool
    V_true = { KSO_BASE_INIT_R(&T_bool, KSOF_NONE, 1) .v_bool = true },
    V_false = { KSO_BASE_INIT_R(&T_bool, KSOF_NONE, 1) .v_bool = false }
;



// export them all with the same names as declared in the header

ks_type 
    ks_T_none = &T_none,
    ks_T_bool = &T_bool,
    ks_T_int = &T_int,
    ks_T_str = &T_str,
    ks_T_tuple = &T_tuple,
    ks_T_list = &T_list,
    ks_T_dict = &T_dict,
    ks_T_type = &T_type,
    ks_T_code = &T_code,
    ks_T_kfunc = &T_kfunc,
    ks_T_cfunc = &T_cfunc,
    ks_T_ast = &T_ast,
    ks_T_parser = &T_parser,
    ks_T_vm = &T_vm
;

ks_none ks_V_none = &V_none;
ks_bool ks_V_true = &V_true, ks_V_false = &V_false;



// the stopping point of literals
#define _INT_CONST_MAX 256

// table of -_INT_CONSTMAX<=x<_INT_CONST_MAX
static struct ks_int int_const[2 * _INT_CONST_MAX];

ks_int ks_int_new(int64_t v_int) {
    if (v_int >= -_INT_CONST_MAX && v_int < _INT_CONST_MAX) {
        return &int_const[v_int + _INT_CONST_MAX];
    } else {
        // construct one from the value

        ks_int self = (ks_int)ks_malloc(sizeof(*self));
        *self = (struct ks_int) {
            KSO_BASE_INIT(ks_T_int, KSOF_NONE)
            .v_int = v_int
        };
        return self;
    }
}

// number of chars to hold
#define _STR_CHR_MAX 256

// list of the single character constants (+NULL)
static struct ks_str str_const_chr[_STR_CHR_MAX];

// returns a good hash function for some data
inline uint64_t str_hash(int len, const char* chr) {
    uint64_t ret = 7;
    int i;
    for (i = 0; i < len; ++i) {
        ret = ret * 31 + ((unsigned char*)chr)[i];
    }
    return ret;
}


// create a new string from a character array
ks_str ks_str_new(int len, const char* chr) {
    // handle constants
    if (len == 0) return &str_const_chr[0];
    if (len == 1) return &str_const_chr[*chr];

    ks_str self = (ks_str)ks_malloc(sizeof(*self) + len);
    *self = (struct ks_str) {
        KSO_BASE_INIT(ks_T_str, KSOF_NONE)
        .v_hash = str_hash(len, chr),
        .len = len,
    };

    memcpy(self->chr, chr, len);
    self->chr[len] = '\0';

    return self;
}

// creates a new string from a C-string, finding out its length
ks_str ks_str_new_r(const char* chr) {
    if (chr == NULL || *chr == (char)0) {
        return &str_const_chr[0];
    } else {
        return ks_str_new((int)strlen(chr), chr);;
    }
}

ks_str ks_str_new_cfmt(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    ks_str ret = ks_str_new_vcfmt(fmt, ap);
    va_end(ap);
    return ret;
}


// create a tuple from objects
ks_tuple ks_tuple_new(int len, kso* items) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + len * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple, KSOF_NONE)
        .len = len
    };

    int i;
    for (i = 0; i < len; ++i) {
        self->items[i] = items[i];
        // record this list's reference to it
        KSO_INCREF(items[i]);
    }
    return self;
}

// return a new reference to a new tuple
ks_tuple ks_tuple_newref(int len, kso* items) {
    ks_tuple self = ks_tuple_new(len, items);
    KSO_INCREF(self);
    return self;
}

// create a new empty tuple
ks_tuple ks_tuple_new_empty() {
    return ks_tuple_new(0, NULL);
}

// create a tuple containing a single object, o0
ks_tuple ks_tuple_new_1(kso o0) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + 1 * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple, KSOF_NONE)
        .len = 1
    };

    self->items[0] = o0;
    KSO_INCREF(o0);

    return self;
}
// create a tuple containing two objects, (o0, o1)
ks_tuple ks_tuple_new_2(kso o0, kso o1) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + 2 * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple, KSOF_NONE)
        .len = 2
    };

    self->items[0] = o0;
    KSO_INCREF(o0);
    self->items[1] = o1;
    KSO_INCREF(o1);

    return self;
}
// create a tuple containing 3 objects, (o0, o1, o2)
ks_tuple ks_tuple_new_3(kso o0, kso o1, kso o2) {
    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + 3 * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple, KSOF_NONE)
        .len = 3
    };

    self->items[0] = o0;
    KSO_INCREF(o0);
    self->items[1] = o1;
    KSO_INCREF(o1);
    self->items[2] = o2;
    KSO_INCREF(o2);

    return self;
}

// create tuple from varargs, NULL argument signifies the last
ks_tuple _ks_tuple_new_va(kso first, ...) {
    if (first == NULL) return ks_tuple_new(0, NULL);

    int num = 1;
    va_list ap, ap1;

    // first, count how many we will have
    va_start(ap, first);
    while ((va_arg(ap, kso)) != NULL) num++;
    va_end(ap);

    ks_tuple self = (ks_tuple)ks_malloc(sizeof(*self) + num * sizeof(kso));
    *self = (struct ks_tuple) {
        KSO_BASE_INIT(ks_T_tuple, KSOF_NONE)
        .len = num
    };

    va_start(ap, first);

    self->items[0] = first;
    KSO_INCREF(first);

    int i;
    for (i = 0; i < num-1; ++i) {
        self->items[i+1] = va_arg(ap, kso);
        KSO_INCREF(self->items[i]);
    }
    va_end(ap);

    return self;
}

KS_CFUNC_TDECL(tuple, str) {
    // get the arguments
    ks_tuple self = (ks_tuple)args[0];

    if (self->len == 0) {
        // just return early
        return (kso)ks_str_new_r("(,)");
    } else if (self->len == 1) {
        // return simple
        ks_str arg_str = kso_torepr(self->items[0]);
        ks_str ret = ks_str_new_cfmt("(%*s,)", arg_str->len, arg_str->chr);
        KSO_CHKREF(arg_str);
        return (kso)ret;
    }

    // now, generate a string
    ks_str gen = ks_str_new_r("(");

    // append all the items
    int i;
    for (i = 0; i < self->len; ++i) {
        ks_str i_str = kso_torepr(self->items[i]);
        ks_str new_gen_str = ks_str_new_cfmt(i == 0 ? "%*s%*s" : ((i == self->len - 1) ? "%*s, %*s)" : "%*s, %*s"), 
            gen->len, gen->chr,
            i_str->len, i_str->chr
        );

        KSO_CHKREF(gen);
        gen = new_gen_str;

        KSO_CHKREF(i_str);
    }
    return (kso)gen;
}

KS_CFUNC_TDECL(tuple, repr) {
    // get the arguments
    ks_tuple self = (ks_tuple)args[0];

    if (self->len == 0) {
        // just return early
        return (kso)ks_str_new_r("(,)");
    } else if (self->len == 1) {
        // return simple
        ks_str arg_str = kso_torepr(self->items[0]);
        ks_str ret = ks_str_new_cfmt("(%*s,)", arg_str->len, arg_str->chr);
        KSO_CHKREF(arg_str);
        return (kso)ret;
    }

    // now, generate a string
    ks_str gen = ks_str_new_r("(");

    // append all the items
    int i;
    for (i = 0; i < self->len; ++i) {
        ks_str i_str = kso_torepr(self->items[i]);
        ks_str new_gen_str = ks_str_new_cfmt(i == 0 ? "%*s%*s" : ((i == self->len - 1) ? "%*s, %*s)" : "%*s, %*s"), 
            gen->len, gen->chr,
            i_str->len, i_str->chr
        );

        KSO_CHKREF(gen);
        gen = new_gen_str;

        KSO_CHKREF(i_str);
    }
    return (kso)gen;
}

KS_CFUNC_TDECL(tuple, free) {

    // get the arguments
    ks_tuple self = (ks_tuple)args[0];

    // remove references from the items
    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    ks_free(self);

    return KSO_NONE;
}



// create a list from objects
ks_list ks_list_new(int len, kso* items) {
    ks_list self = (ks_list)ks_malloc(sizeof(*self));
    *self = (struct ks_list) {
        KSO_BASE_INIT(ks_T_list, KSOF_NONE)
        .len = len,
        .items = (kso*)ks_malloc(sizeof(kso) * len)
    };

    int i;
    for (i = 0; i < len; ++i) {
        self->items[i] = items[i];
        // record this list's reference to it
        KSO_INCREF(items[i]);
    }
    return self;
}

// create a new empty list
ks_list ks_list_new_empty() {
    return ks_list_new(0, NULL);
}

// push an object onto the list, returning the index
// NOTE: This adds a reference to the object
int ks_list_push(ks_list self, kso obj) {
    int idx = self->len++;
    self->items = ks_realloc(self->items, sizeof(kso) * self->len);
    self->items[idx] = obj;
    KSO_INCREF(obj);
    return idx;
}

// pops an object off of the list, transfering the reference to the caller
// NOTE: Call KSO_DECREF when the object reference is dead
kso ks_list_pop(ks_list self) {
    return self->items[--self->len];
}

// pops off an object from the list, with it not being used
// NOTE: This decrements the reference originally added with the push function
void ks_list_popu(ks_list self) {
    kso obj = self->items[--self->len];
    KSO_DECREF(obj);
}

// clears the list, setting it back to empty
void ks_list_clear(ks_list self) {

    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    self->len = 0;
}

KS_CFUNC_TDECL(list, str) {
    // get the arguments
    ks_list self = (ks_list)args[0];

    if (self->len == 0) {
        // just return early
        return (kso)ks_str_new_r("[]");
    } else if (self->len == 1) {
        // return simple
        ks_str arg_str = kso_torepr(self->items[0]);
        ks_str ret = ks_str_new_cfmt("[%*s]", arg_str->len, arg_str->chr);
        KSO_CHKREF(arg_str);
        return (kso)ret;
    }

    // now, generate a string
    ks_str gen = ks_str_new_r("[");

    // append all the items
    int i;
    for (i = 0; i < self->len; ++i) {
        ks_str i_str = kso_torepr(self->items[i]);
        ks_str new_gen_str = ks_str_new_cfmt(i == 0 ? "%*s%*s" : ((i == self->len - 1) ? "%*s, %*s]" : "%*s, %*s"), 
            gen->len, gen->chr,
            i_str->len, i_str->chr
        );

        KSO_CHKREF(gen);
        gen = new_gen_str;

        KSO_CHKREF(i_str);
    }
    return (kso)gen;
}

KS_CFUNC_TDECL(list, repr) {
    // get the arguments
    ks_list self = (ks_list)args[0];

    if (self->len == 0) {
        // just return early
        return (kso)ks_str_new_r("[]");
    } else if (self->len == 1) {
        // return simple
        ks_str arg_str = kso_torepr(self->items[0]);
        ks_str ret = ks_str_new_cfmt("[%*s]", arg_str->len, arg_str->chr);
        KSO_CHKREF(arg_str);
        return (kso)ret;
    }

    // now, generate a string
    ks_str gen = ks_str_new_r("[");

    // append all the items
    int i;
    for (i = 0; i < self->len; ++i) {
        ks_str i_str = kso_torepr(self->items[i]);
        ks_str new_gen_str = ks_str_new_cfmt(i == 0 ? "%*s%*s" : ((i == self->len - 1) ? "%*s, %*s]" : "%*s, %*s"), 
            gen->len, gen->chr,
            i_str->len, i_str->chr
        );

        KSO_CHKREF(gen);
        gen = new_gen_str;

        KSO_CHKREF(i_str);
    }
    return (kso)gen;
}

KS_CFUNC_TDECL(list, free) {

    // get the arguments
    ks_list self = (ks_list)args[0];

    // remove references from the items
    int i;
    for (i = 0; i < self->len; ++i) {
        KSO_DECREF(self->items[i]);
    }

    ks_free(self->items);

    ks_free(self);

    return KSO_NONE;
}


// create a new C-function wrapper
ks_cfunc ks_cfunc_new(ks_cfunc_sig v_cfunc) {
    ks_cfunc self = (ks_cfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_cfunc) {
        KSO_BASE_INIT(ks_T_cfunc, KSOF_NONE)
        .v_cfunc = v_cfunc
    };
    return self;
}

// create a new C-function wrapper with a reference
ks_cfunc ks_cfunc_newref(ks_cfunc_sig v_cfunc) {
    ks_cfunc self = (ks_cfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_cfunc) {
        KSO_BASE_INIT_R(ks_T_cfunc, KSOF_NONE, 1)
        .v_cfunc = v_cfunc
    };
    return self;
}

// create a new kfunc
ks_kfunc ks_kfunc_new(ks_list params, ks_code code) {
    ks_kfunc self = (ks_kfunc)ks_malloc(sizeof(*self));
    *self = (struct ks_kfunc) {
        KSO_BASE_INIT(ks_T_kfunc, KSOF_NONE)
        .code = code,
        .params = params
    };

    KSO_INCREF(params);
    KSO_INCREF(code);
    return self;
}


KS_CFUNC_TDECL(kfunc, free) {

    // get the arguments
    ks_kfunc self = (ks_kfunc)args[0];

    KSO_DECREF(self->params);
    KSO_DECREF(self->code);

    ks_free(self);

    return KSO_NONE;
}


// create a new empty piece of code
ks_code ks_code_new_empty(ks_list v_const) {
    ks_code self = (ks_code)ks_malloc(sizeof(*self));
    *self = (struct ks_code) {
        KSO_BASE_INIT(ks_T_code, KSOF_NONE)
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

        default:
            kse_fmt("While linking (code @ %p) into (code @ %p), unknown instruction was encountered: %i (at pos=%i)", other, self, (int)other->bc[i], i);
            return;
            break;
        }

    }

}


// called when the object should be freed

KS_CFUNC_TDECL(code, free) {

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


/* code generation helpers */

void ksc_addbytes(ks_code code, int size, uint8_t* new_bytes) {
    int start = code->bc_n;
    code->bc = ks_realloc(code->bc, code->bc_n += size);
    memcpy(&code->bc[start], new_bytes, size);
}

// add a constant to the `v_const` list, returning the index
int ksc_addconst(ks_code code, kso val) {
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


// helper macro to add an instruction only
#define KSC_(_bc) { ksc_addbytes(code, sizeof(ksbc_), (uint8_t*)&(ksbc_){ .op = _bc }); }

#define KSC_I32(_bc, _i32) { ksc_addbytes(code, sizeof(ksbc_i32), (uint8_t*)&(ksbc_i32){ .op = _bc, .i32 = _i32 }); }

void ksc_noop(ks_code code) { KSC_(KSBC_NOOP); }
void ksc_popu(ks_code code) { KSC_(KSBC_POPU); }
void ksc_const(ks_code code, kso val) {
    int idx = ksc_addconst(code, val);
    KSC_I32(KSBC_CONST, idx); 
}
void ksc_const_true(ks_code code) { KSC_(KSBC_CONST_TRUE); }
void ksc_const_false(ks_code code) { KSC_(KSBC_CONST_FALSE); }
void ksc_const_none(ks_code code) { KSC_(KSBC_CONST_NONE); }
void ksc_int(ks_code code, int64_t v_int) { 
    ks_int iobj = ks_int_new(v_int); 
    int idx = ksc_addconst(code, (kso)iobj); 
    KSC_I32(KSBC_CONST, idx); 
    KSO_CHKREF(iobj);
}
void ksc_cstr(ks_code code, const char* v_cstr) { 
    ks_str sobj = ks_str_new_r(v_cstr);
    int idx = ksc_addconst(code, (kso)sobj);
    KSC_I32(KSBC_CONST, idx);
    KSO_CHKREF(sobj);
}
void ksc_cstrl(ks_code code, int len, const char* v_cstr) { 
    ks_str sobj = ks_str_new(len, v_cstr);
    int idx = ksc_addconst(code, (kso)sobj);
    KSC_I32(KSBC_CONST, idx);
    KSO_CHKREF(sobj);
}
void ksc_load(ks_code code, const char* v_name) { 
    ks_str sobj = ks_str_new_r(v_name);
    int idx = ksc_addconst(code, (kso)sobj);
    KSC_I32(KSBC_LOAD, idx);
    KSO_CHKREF(sobj);
}
void ksc_loadl(ks_code code, int len, const char* v_name) { 
    ks_str sobj = ks_str_new(len, v_name);
    int idx = ksc_addconst(code, (kso)sobj);
    KSC_I32(KSBC_LOAD, idx);
    KSO_CHKREF(sobj);
}
void ksc_loado(ks_code code, kso obj) { 
    int idx = ksc_addconst(code, obj);
    KSC_I32(KSBC_LOAD, idx);
}
void ksc_store(ks_code code, const char* v_name) { 
    ks_str sobj = ks_str_new_r(v_name);
    int idx = ksc_addconst(code, (kso)sobj);
    KSC_I32(KSBC_STORE, idx);
    KSO_CHKREF(sobj);
}
void ksc_storeo(ks_code code, kso obj) { 
    int idx = ksc_addconst(code, obj);
    KSC_I32(KSBC_STORE, idx);
}
void ksc_call(ks_code code, int n_items) { KSC_I32(KSBC_CALL, n_items); }
void ksc_tuple(ks_code code, int n_items) { KSC_I32(KSBC_TUPLE, n_items); }
void ksc_list(ks_code code, int n_items) { KSC_I32(KSBC_LIST, n_items); }

void ksc_add(ks_code code) { KSC_(KSBC_ADD); }
void ksc_sub(ks_code code) { KSC_(KSBC_SUB); }
void ksc_mul(ks_code code) { KSC_(KSBC_MUL); }
void ksc_div(ks_code code) { KSC_(KSBC_DIV); }
void ksc_lt(ks_code code) { KSC_(KSBC_LT); }
void ksc_gt(ks_code code) { KSC_(KSBC_GT); }
void ksc_eq(ks_code code) { KSC_(KSBC_EQ); }
void ksc_jmp(ks_code code, int relamt) { KSC_I32(KSBC_JMP, relamt); }
void ksc_jmpt(ks_code code, int relamt) { KSC_I32(KSBC_JMPT, relamt); }
void ksc_jmpf(ks_code code, int relamt) { KSC_I32(KSBC_JMPF, relamt); }

void ksc_ret(ks_code code) { KSC_(KSBC_RET); }
void ksc_ret_none(ks_code code) { KSC_(KSBC_RET_NONE); }



#define _AST_TOK_INIT .tok = ks_tok_new(KS_TOK_NONE, NULL, 0, 0, 0, 0), .tok_expr = ks_tok_new(KS_TOK_NONE, NULL, 0, 0, 0, 0),

// create a new AST representing a constant int
ks_ast ks_ast_new_int(int64_t v_int) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        .atype = KS_AST_INT,
        _AST_TOK_INIT
        .v_int = ks_int_new(v_int)
    };
    KSO_INCREF(self->v_int);
    return self;
}

// create a new AST representing a constant string
ks_ast ks_ast_new_str(const char* v_str) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        .atype = KS_AST_STR,
        _AST_TOK_INIT
        .v_str = ks_str_new_r(v_str),
    };
    KSO_INCREF(self->v_str);
    return self;
}


ks_ast ks_ast_new_stro(ks_str v_str) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        .atype = KS_AST_STR,
        _AST_TOK_INIT
        .v_str = v_str
    };
    KSO_INCREF(self->v_str);
    return self;
}


// create a new AST representing 'true'
ks_ast ks_ast_new_true() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_TRUE
    };
    return self;
}

// create a new AST representing 'false'
ks_ast ks_ast_new_false() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_FALSE
    };
    return self;
}

// create a new AST representing 'none'
ks_ast ks_ast_new_none() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_NONE
    };
    return self;
}


// create a new AST representing a variable reference
ks_ast ks_ast_new_var(const char* var_name) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_VAR,
        .v_var = ks_str_new_r(var_name),
    };
    KSO_INCREF(self->v_var);
    return self;
}
// create a new AST representing a variable reference
ks_ast ks_ast_new_varl(int len, const char* var_name) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_VAR,
        .v_var = ks_str_new(len, var_name),
    };
    KSO_INCREF(self->v_var);
    return self;
}

// new AST representing a tuple
ks_ast ks_ast_new_tuple(int n_items, ks_ast* items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_TUPLE,
        .v_list = ks_list_new(n_items, (kso*)items),
    };
    KSO_INCREF(self->v_list);
    return self;
}

// new AST representing a list
ks_ast ks_ast_new_list(int n_items, ks_ast* items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_LIST,
        .v_list = ks_list_new(n_items, (kso*)items),
    };
    KSO_INCREF(self->v_list);
    return self;
}

// create a new AST representing a functor call, with `items[0]` being the function
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_call(int n_items, ks_ast* items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_CALL,
        .v_call = ks_list_new(n_items, (kso*)items),
    };
    KSO_INCREF(self->v_call);
    return self;
}

ks_ast ks_ast_new_bop(int bop_type, ks_ast L, ks_ast R) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = bop_type,
        .v_bop = { L, R }
    };
    KSO_INCREF(L);
    KSO_INCREF(R);
    return self;
}

// create a new if block AST
ks_ast ks_ast_new_if(ks_ast cond, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_IF,
        .v_if = {cond, body}
    };
    KSO_INCREF(cond);
    KSO_INCREF(body);

    return self;
}

// create a new while block AST
ks_ast ks_ast_new_while(ks_ast cond, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_WHILE,
        .v_while = {cond, body}
    };
    KSO_INCREF(cond);
    KSO_INCREF(body);

    return self;
}


// create a new empty block AST
ks_ast ks_ast_new_block_empty() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_BLOCK,
        .v_block = ks_list_new_empty()
    };
    KSO_INCREF(self->v_block);
    return self;
}

ks_ast ks_ast_new_block(int n_items, ks_ast* items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_BLOCK,
        .v_block = ks_list_new(n_items, (kso*)items)
    };
    KSO_INCREF(self->v_block);
    return self;
}

// createa a new AST representing a function literal
ks_ast ks_ast_new_func(ks_list params, ks_ast body) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_FUNC,
        .v_func = {params, body}
    };
    KSO_INCREF(params);
    KSO_INCREF(body);
    return self;
}
// create a new AST representing a return statement
ks_ast ks_ast_new_ret(ks_ast val) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_RET,
        .v_ret = val
    };
    KSO_INCREF(val);
    return self;
}
// createa a new AST representing a block of code
ks_ast ks_ast_new_code(ks_code code) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        _AST_TOK_INIT
        .atype = KS_AST_CODE,
        .v_code = code
    };
    KSO_INCREF(code);
    return self;
}


KS_CFUNC_TDECL(ast, free) {
    ks_ast self = (ks_ast)args[0];


    switch (self->atype) {
    case KS_AST_INT:
        KSO_DECREF(self->v_int);
        break;
    case KS_AST_STR:
        KSO_DECREF(self->v_str);
        break;
    case KS_AST_TRUE:
    case KS_AST_FALSE:
    case KS_AST_NONE:
        // do nothing, they don't hold refs
        break;

    case KS_AST_VAR:
        KSO_DECREF(self->v_var);
        break;

    case KS_AST_TUPLE:
    case KS_AST_LIST:
        // both use this
        KSO_DECREF(self->v_list);
        break;

    case KS_AST_CALL:
        KSO_DECREF(self->v_call);
        break;

    case KS_AST_CODE:
        KSO_DECREF(self->v_code);
        break;
    case KS_AST_FUNC:
        KSO_DECREF(self->v_func.params);
        KSO_DECREF(self->v_func.body);
        break;

    case KS_AST_RET:
        KSO_DECREF(self->v_ret);
        break;


    case KS_AST_BLOCK:
        KSO_DECREF(self->v_block);
        break;

    case KS_AST_IF:
        KSO_DECREF(self->v_if.cond);
        KSO_DECREF(self->v_if.body);
        break;

    case KS_AST_WHILE:
        KSO_DECREF(self->v_while.cond);
        KSO_DECREF(self->v_while.body);
        break;


    // handle all binary operators
    case KS_AST_BOP_ADD:
    case KS_AST_BOP_SUB:
    case KS_AST_BOP_MUL:
    case KS_AST_BOP_DIV:
    case KS_AST_BOP_LT:
    case KS_AST_BOP_GT:
    case KS_AST_BOP_EQ:
    case KS_AST_BOP_ASSIGN:
        KSO_DECREF(self->v_bop.L);
        KSO_DECREF(self->v_bop.R);
        break;

    default:
        ks_warn("ast obj @ %p was of unknown type %d", self, self->atype);
        break;
    }

    ks_free(self);

    return KSO_NONE;
}


KS_CFUNC_TDECL(parser, free) {

    // get the arguments
    ks_parser self = (ks_parser)args[0];

    ks_free(self->toks);

    KSO_DECREF(self->src_name);
    KSO_DECREF(self->src);

    ks_free(self);

    return KSO_NONE;
}

/*
Dictionary implementation:

*/


// starting length for the dictionary
#define _DICT_MIN_LEN 8

// the maximum load value (as a percentage used)
// once the load factor exceeds this, the dictionary is resized
#define _DICT_LOAD_MAX 25

// generates the new size for the dictionary
#define _DICT_NEW_SIZE(_dict) (2 * (_dict)->n_buckets + (_dict)->n_items)

// return a new empty dictionary
ks_dict ks_dict_new_empty() {
    ks_dict self = (ks_dict)ks_malloc(sizeof(*self));
    *self = (struct ks_dict) {
        KSO_BASE_INIT(ks_T_dict, KSOF_NONE)
        .n_items = 0,
        .n_buckets = _DICT_MIN_LEN,
        .buckets = ks_malloc(sizeof(*self->buckets) * _DICT_MIN_LEN)
    };

    int i;
    for (i = 0; i < _DICT_MIN_LEN; ++i) {
        self->buckets[i] = (struct ks_dict_entry){ .key = NULL, .hash = 0, .val = NULL };
    }
    return self;
}


/* dictionary helpers */

// maps a hash to a bucket index
static int32_t dict_buck(ks_dict self, uint64_t hash) {
    return hash % self->n_buckets;
}

// gets the next bucket, given a try index
// this is the probing function
static int32_t dict_buck_next(int32_t cur_buck, int try) {
    // linear probing
    return cur_buck + 1;
}

// check whether it fully matches
static bool dict_entry_matches(struct ks_dict_entry entry, kso key, uint64_t hash) {
    // TODO: Also add a literal `x==y` using their object types and everything
    return entry.hash == hash && kso_eq(entry.key, key);
}

/* prime number finding, for optimal hash-table sizes */

static bool isprime(int x) {
    // true if prime
    if (x < 2) return false;
    if (x == 2 || x == 3 || x == 5) return true;
    if (x % 2 == 0 || x % 3 == 0 || x % 5 == 0) return false;

    // sqrt(x)

    // now check all odd numbers  from 7 to sqrt(x)
    int i;
    for (i = 7; i * i <= x; i += 2) {
        if (x % i == 0) return false;
    }

    return true;
}
// returns the next prime after x (not including x)
static int nextprime(int x) {
    // round up to next odd number
    int p;
    if (x % 2 == 0) p = x + 1;
    else p = x + 2;

    do {
        if (isprime(p)) return p;

        p += 2;
    } while (true);
    
    // just return it anyway
    return p;
}


void ks_dict_resize(ks_dict self, int new_size) {
    if (self->n_buckets >= new_size) return;

    // always round up to a prime number
    new_size = nextprime(new_size);
    //ks_trace("dict resize %d -> %d", self->n_buckets, new_size);

    // get the old entries
    int old_n_buckets = self->n_buckets;
    struct ks_dict_entry* old_buckets = self->buckets;

    // allocate the new buckets
    self->n_buckets = new_size;
    self->buckets = ks_malloc(sizeof(*self->buckets) * self->n_buckets);

    // initialize them to empty
    int i;
    for (i = 0; i < self->n_buckets; ++i) {
        self->buckets[i] = (struct ks_dict_entry){ .key = NULL, .hash = 0, .val = NULL };
    }

    // go through all the buckets, and merge them over
    for (i = 0; i < old_n_buckets; ++i) {
        struct ks_dict_entry* old_entry = &old_buckets[i];

        if (old_entry->val != NULL) {
            // we have a valid bucket that needs to be copied
            ks_dict_set(self, old_entry->key, old_entry->hash, old_entry->val);
            KSO_DECREF(old_entry->key);
            KSO_DECREF(old_entry->val);
        }
    }

    // free the old buckets
    ks_free(old_buckets);
}

int ks_dict_set(ks_dict self, kso key, uint64_t hash, kso val) {

    // make sure it is large enough
    if (self->n_buckets * _DICT_LOAD_MAX <= self->n_items * 100) {
        ks_dict_resize(self, _DICT_NEW_SIZE(self));
    }

    struct ks_dict_entry* entry = NULL;
    int b_idx = dict_buck(self, hash), tries = 0;

    // first, search through filled buckets (those)
    while ((entry = &self->buckets[b_idx])->val != NULL && tries++ < self->n_buckets) {

        if (dict_entry_matches(*entry, key, hash)) {
            // we've found it, just replace the value
            KSO_INCREF(val);
            KSO_DECREF(entry->val);
            entry->val = val;
            return b_idx;
        }

        // update the bucket index, try again
        b_idx = dict_buck_next(b_idx, tries);
        // wrap back around
        while (b_idx > self->n_buckets) b_idx -= self->n_buckets;
    }

    // if we've gotten to here, it means we found an empty bucket, so just replace it
    KSO_INCREF(key);
    KSO_INCREF(val);
    
    *entry = (struct ks_dict_entry) {
        .key = key,
        .hash = hash,
        .val = val
    };

    return b_idx;
}


kso ks_dict_get(ks_dict self, kso key, uint64_t hash) {
    if (self->n_buckets == 0) return NULL;

    int b_idx = dict_buck(self, hash), tries = 0;
    struct ks_dict_entry* entry = NULL;

    // search through non-empty buckets
    while ((entry = &self->buckets[b_idx])->val != NULL && tries++ < self->n_buckets) {
        if (dict_entry_matches(*entry, key, hash)) {
            // we've found a match, just return it
            return entry->val;
        }

        // update the bucket index, try again
        b_idx = dict_buck_next(b_idx, tries);
        // wrap back around
        while (b_idx > self->n_buckets) b_idx -= self->n_buckets;
    }

    // not found, return NULL
    return NULL;

}

KS_CFUNC_TDECL(dict, free) {

    // get the arguments
    ks_dict self = (ks_dict)args[0];

    int i;

    for (i = 0; i < self->n_buckets; ++i) {
        struct ks_dict_entry* entry = &self->buckets[i];
        if (entry->val != NULL) {
            KSO_DECREF(entry->key);
            KSO_DECREF(entry->val);
        }
    }

    // free the indices/buckets
    ks_free(self->buckets);

    ks_free(self);

    return KSO_NONE;
}


// return a new empty virtual machine
ks_vm ks_vm_new_empty() {
    ks_vm self = (ks_vm)ks_malloc(sizeof(*self));
    *self = (struct ks_vm) {
        KSO_BASE_INIT(ks_T_vm, KSOF_NONE)
        .stk = ks_list_new_empty(),
        .globals = ks_dict_new_empty(),
        .n_scopes = 0,
        .scopes = NULL
    };

    KSO_INCREF(self->globals);
    KSO_INCREF(self->stk);

    return self;
}

KS_CFUNC_TDECL(vm, free) {
    // get the arguments
    ks_vm self = (ks_vm)args[0];

    // free our dense items list
    ks_free(self->scopes);
    
    KSO_DECREF(self->globals);
    KSO_DECREF(self->stk);

    ks_free(self);

    return KSO_NONE;
}




/* generic object interface functionality */

uint64_t kso_hash(kso obj) {
    if (obj->type == ks_T_str) {
        return ((ks_str)obj)->v_hash;
    } else {
        return (uint64_t)obj;
    }
}


// call an object as a callable, with a list of arguments
kso kso_call(kso func, int n_args, kso* args) {
    if (func->type == ks_T_cfunc) {
        return ((ks_cfunc)func)->v_cfunc(n_args, args);
    } else {
        return NULL;
    }
}


// try to convert A to a boolean, return 0 if it would be false, 1 if it would be true,
//   and -1 if we couldn't decide
int kso_bool(kso A) {
    if (A == KSO_TRUE) return 1;
    if (A == KSO_FALSE) return 0;
    if (A == KSO_NONE) return 0;

    if (A->type == ks_T_int) return ((ks_int)A)->v_int == 0 ? 0 : 1;

    // containers are determined by their length
    if (A->type == ks_T_str) return ((ks_str)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_tuple) return ((ks_tuple)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_list) return ((ks_list)A)->len == 0 ? 0 : 1;
    if (A->type == ks_T_dict) return ((ks_dict)A)->n_items == 0 ? 0 : 1;

    // else, we couldn't decide

    return -1;

}

// convert A to a string
ks_str kso_tostr(kso A) {

    if (A->type == ks_T_str) return (ks_str)A;
    if (A->type == ks_T_none) return ks_str_new_r("none");
    if (A->type == ks_T_bool) return ks_str_new_r(A == KSO_TRUE ? "true" : "false");
    if (A->type == ks_T_int) {
        ks_int Ai = (ks_int)A;
        if (Ai->v_int >= 0 && Ai->v_int < 10) {
            // return the single character
            return &str_const_chr['0' + Ai->v_int];
        }
    }

    ks_type T_A = A->type;
    if (T_A->f_str != NULL) {
        ks_str ret = (ks_str)kso_call(T_A->f_str, 1, &A);
        if (ret != NULL) {
            // only return if no error occured
            return ret;
        }
    }

    // else, nothing has been returned so do the default
    return ks_str_new_cfmt("<'%*s' obj @ %p>", T_A->name->len, T_A->name->chr, A);
}

// convert A to its representation
ks_str kso_torepr(kso A) {

    if (A->type == ks_T_str) return ks_str_new_cfmt("\"%*s\"", ((ks_str)A)->len, ((ks_str)A)->chr);
    if (A->type == ks_T_none || A->type == ks_T_bool || A->type == ks_T_int) return kso_tostr(A);

    ks_type T_A = A->type;
    if (T_A->f_repr != NULL) {
        ks_str ret = (ks_str)kso_call(T_A->f_repr, 1, &A);
        if (ret != NULL) {
            // only return if no error occured
            return ret;
        }
    }

    // else, nothing has been returned so do the default
    return ks_str_new_cfmt("<'%*s' obj @ %p>", T_A->name->len, T_A->name->chr, A);
}


// return whether or not the 2 objects are equal
bool kso_eq(kso A, kso B) {
    // same pointer should always be equal
    if (A == B) return true;

    if (A->type == B->type) {
        // do basic type checks
        if (A->type == ks_T_int) {
            return ((ks_int)A)->v_int == ((ks_int)B)->v_int;
        } else if (A->type == ks_T_str) {
            ks_str As = (ks_str)A, Bs = (ks_str)B;
            // check their lengths and hashes
            if (As->len != Bs->len || As->v_hash != Bs->v_hash) return false;

            // now, just strcmp
            return memcmp(As->chr, Bs->chr, As->len) == 0;
        }
    }

    // TODO: use their types
    return false;
}

bool kso_free(kso obj) {
    // if it can still be reached, don't free it
    if (obj->refcnt > 0) return false;
    else if (obj->refcnt < 0) ks_warn("refcnt of %o was %i", obj, obj->refcnt);

    // otherwise, free it
    ks_trace("kso_free(%o) (repr was: %R)", obj, obj);

    // check for a type function to free
    if (obj->type->f_free != NULL) {
        if (kso_call(obj->type->f_free, 1, &obj) == NULL) {
            ks_warn("Problem encountered while freeing < obj @ %p >", obj);
        }
    } else {
        // do the default, which is to just free the object
        ks_free(obj);
    }

}

void kso_init() {

    /* first, initialze types */
    *ks_T_type = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("type"),
    };

    *ks_T_none = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("none"),
    };

    *ks_T_bool = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("bool"),
    };

    *ks_T_int = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("int"),
    };

    *ks_T_str = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("str"),
    };

    *ks_T_tuple = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("tuple"),
        .f_str  = (kso)ks_cfunc_newref(tuple_str),
        .f_repr = (kso)ks_cfunc_newref(tuple_repr),
        .f_free = (kso)ks_cfunc_newref(tuple_free),
    };

    *ks_T_list = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("list"),
        .f_str  = (kso)ks_cfunc_newref(list_str),
        .f_repr = (kso)ks_cfunc_newref(list_repr),
        .f_free = (kso)ks_cfunc_newref(list_free),
    };

    *ks_T_dict = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("dict"),
        .f_free = (kso)ks_cfunc_newref(dict_free),
    };

    *ks_T_cfunc = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("cfunc"),
    };

    *ks_T_code = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("code"),
        .f_free = (kso)ks_cfunc_newref(code_free),
    };

    *ks_T_kfunc = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("kfunc"),
        .f_free = (kso)ks_cfunc_newref(kfunc_free),
    };

    *ks_T_ast = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("ast"),
        .f_free = (kso)ks_cfunc_newref(ast_free),
    };

    *ks_T_parser = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("parser"),
        .f_free = (kso)ks_cfunc_newref(parser_free),
    };

    *ks_T_vm = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("vm"),
        .f_free = (kso)ks_cfunc_newref(vm_free),
    };


    // fill up the integer constant tables
    int i;
    for (i = -_INT_CONST_MAX; i < _INT_CONST_MAX; ++i) {
        int_const[i + _INT_CONST_MAX] = (struct ks_int) {
            KSO_BASE_INIT_R(ks_T_int, KSOF_NONE, 1)
            .v_int = i
        };
    }

    // fill up the string constants table

    for (i = 0; i < _STR_CHR_MAX; ++i) {
        str_const_chr[i] = (struct ks_str) {
            KSO_BASE_INIT_R(ks_T_str, KSOF_NONE, 1)
            .len = i == 0 ? 0 : 1
        };
        str_const_chr[i].chr[0] = (char)i;
        str_const_chr[i].chr[1] = (char)0;
    }
}


