/* code.c - implementation of the bytecode object
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// create a kscript code given a constant array
ks_code ks_code_new(ks_list v_const, ks_parser parser) {
    ks_code self = KS_ALLOC_OBJ(ks_code);
    KS_INIT_OBJ(self, ks_T_code);
    
    if (v_const == NULL) {
        // create a new constants array
        v_const = ks_list_new(0, NULL);
        self->v_const = v_const;
    } else {
        // keep a reference to the constant array
        self->v_const = v_const;
        KS_INCREF(v_const);
    }

    self->name_hr = ks_fmt_c("<code @ %p>", self);

    self->parser = parser;
    if (parser) KS_INCREF(parser);

    // start with an empty bytecode
    self->bc_n = 0;
    self->bc = NULL;

    // and no meta
    self->meta_n = 0;
    self->meta = NULL;

    return self;
}

// add a meta token (and hold a reference to the parser)
void ks_code_add_meta(ks_code self, struct ks_tok tok) {
    int idx = self->meta_n++;
    self->meta = ks_realloc(self->meta, sizeof(*self->meta) * self->meta_n);
    
    self->meta[idx] = (struct ks_code_meta) {
        .bc_n = self->bc_n,
        .tok = tok
    };

}

// add bytes to the code
void ks_code_add(ks_code self, int len, ksb* data) {
    // start index
    int idx = self->bc_n;

    // expand list
    self->bc_n += len;
    self->bc = ks_realloc(self->bc, sizeof(*self->bc) * self->bc_n);

    // write new data
    memcpy(&self->bc[idx], data, len);
}

// add a constant to the v_const list
int ks_code_add_const(ks_code self, ks_obj val) {

    int i;
    for (i = 0; i < self->v_const->len; ++i) {
        // check if it already exists, if so just return that index
        if (ks_obj_eq(val, self->v_const->elems[i])) {
            return i;
        }
    }
    
    // else, add it and return the last index
    ks_list_push(self->v_const, val);
    return self->v_const->len - 1;
}

/* ADDING BYTECODE */

// macro to just add a byte
#define KSCA_B(_b) { ksb r = _b; ks_code_add(self, 1, (ksb*)&r); }
// macro to add a byte, then a 4 byte signed integer
#define KSCA_B_I32(_b, _i) { ksb r = _b; int32_t v = _i; ks_code_add(self, 1, (ksb*)&r); ks_code_add(self, sizeof(int32_t), (ksb*)&v); }

void ksca_noop   (ks_code self) KSCA_B(KSB_NOOP)

void ksca_push   (ks_code self, ks_obj val) KSCA_B_I32(KSB_PUSH, ks_code_add_const(self, val));
void ksca_dup    (ks_code self) KSCA_B(KSB_DUP)
void ksca_popu   (ks_code self) KSCA_B(KSB_POPU)

void ksca_getitem   (ks_code self, int n_items) KSCA_B_I32(KSB_GETITEM, n_items)
void ksca_setitem   (ks_code self, int n_items) KSCA_B_I32(KSB_SETITEM, n_items)

void ksca_list      (ks_code self, int n_items) KSCA_B_I32(KSB_LIST, n_items)
void ksca_tuple     (ks_code self, int n_items) KSCA_B_I32(KSB_TUPLE, n_items)
void ksca_dict      (ks_code self, int n_items) KSCA_B_I32(KSB_DICT, n_items)
void ksca_slice     (ks_code self) KSCA_B(KSB_SLICE)

void ksca_call   (ks_code self, int n_items) KSCA_B_I32(KSB_CALL, n_items)
void ksca_vcall     (ks_code self) KSCA_B(KSB_VCALL);

void ksca_list_add_objs (ks_code self, int n_items) KSCA_B_I32(KSB_LIST_ADD_OBJS, n_items)
void ksca_list_add_iter (ks_code self) KSCA_B(KSB_LIST_ADD_ITER)
void ksca_buildstr  (ks_code self, int n_items) KSCA_B_I32(KSB_BUILDSTR, n_items)

void ksca_ret    (ks_code self) KSCA_B(KSB_RET)
void ksca_throw  (ks_code self) KSCA_B(KSB_THROW)
void ksca_assert (ks_code self) KSCA_B(KSB_ASSERT)
void ksca_jmp    (ks_code self, int relamt) KSCA_B_I32(KSB_JMP, relamt)
void ksca_jmpt   (ks_code self, int relamt) KSCA_B_I32(KSB_JMPT, relamt)
void ksca_jmpf   (ks_code self, int relamt) KSCA_B_I32(KSB_JMPF, relamt)

void ksca_try_start (ks_code self, int relamt) KSCA_B_I32(KSB_TRY_START, relamt)
void ksca_try_end   (ks_code self, int relamt) KSCA_B_I32(KSB_TRY_END, relamt)

void ksca_closure   (ks_code self) KSCA_B(KSB_ADD_CLOSURE)
void ksca_new_func  (ks_code self) KSCA_B(KSB_NEW_FUNC)

void ksca_make_iter (ks_code self) KSCA_B(KSB_MAKE_ITER)
void ksca_iter_next (ks_code self, int relamt) KSCA_B_I32(KSB_ITER_NEXT, relamt)

void ksca_load      (ks_code self, ks_str name) KSCA_B_I32(KSB_LOAD, ks_code_add_const(self, (ks_obj)name))
void ksca_load_attr (ks_code self, ks_str name) KSCA_B_I32(KSB_LOAD_ATTR, ks_code_add_const(self, (ks_obj)name))
void ksca_store     (ks_code self, ks_str name) KSCA_B_I32(KSB_STORE, ks_code_add_const(self, (ks_obj)name))
void ksca_store_attr(ks_code self, ks_str name) KSCA_B_I32(KSB_STORE_ATTR, ks_code_add_const(self, (ks_obj)name))


void ksca_bop       (ks_code self, int ksb_bop_type) KSCA_B(ksb_bop_type)
void ksca_uop       (ks_code self, int ksb_uop_type) KSCA_B(ksb_uop_type)

void ksca_truthy    (ks_code self) KSCA_B(KSB_TRUTHY)



/* C-style funcs */
void ksca_load_c(ks_code self, char* name) {
    ks_str obj = ks_str_new(name);
    ksca_load(self, obj);
    KS_DECREF(obj);
}

void ksca_load_attr_c(ks_code self, char* name) {
    ks_str obj = ks_str_new(name);
    ksca_load_attr(self, obj);
    KS_DECREF(obj);
}

void ksca_store_c(ks_code self, char* name) {
    ks_str obj = ks_str_new(name);
    ksca_store(self, obj);
    KS_DECREF(obj);
}

void ksca_store_attr_c(ks_code self, char* name) {
    ks_str obj = ks_str_new(name);
    ksca_store_attr(self, obj);
    KS_DECREF(obj);
}



// code.__str__(self) -> generate a string for the bytecode
static KS_TFUNC(code, str) {
    ks_code self;
    KS_GETARGS("self:*", &self, ks_T_code)


    
    ks_str_builder sb = ks_str_builder_new();

    // first, dump out the constant list:
    ks_str_builder_add_fmt(sb, "\n# -*- code @ %p\n", self);
    ks_str_builder_add_fmt(sb, "# v_const (vc): %S\n", self->v_const);

    // now, iterate through all the instructions

    int i = 0;

    while (i < self->bc_n) {
        ks_str_builder_add_fmt(sb, "%0*i ", 4, i);

        ksb op = self->bc[i++];
        bool haderr = false;

        // just always read it if there is no possible buffer overrun, otherwise 0
        int val = (i + 4 >= self->bc_n) ? 0 : *(int *)(self->bc + i);

        switch (op)
        {
        case KSB_NOOP:
            ks_str_builder_add_fmt(sb, "noop");
            break;
        
        case KSB_PUSH:
            i += 4;
            ks_str_builder_add_fmt(sb, "push %R  # idx: %i", self->v_const->elems[val], val); // i peed.com
            break;

        case KSB_DUP:
            ks_str_builder_add_fmt(sb, "dup");
            break;

        case KSB_POPU:
            ks_str_builder_add_fmt(sb, "popu");
            break;

        case KSB_LIST:
            i += 4;
            ks_str_builder_add_fmt(sb, "list %i", val);
            break;

        case KSB_LIST_ADD_ITER:
            ks_str_builder_add_fmt(sb, "list_add_iter");
            break;

        case KSB_LIST_ADD_OBJS:
            i += 4;
            ks_str_builder_add_fmt(sb, "list_add_objs %i", val);
            break;

        case KSB_SLICE:
            ks_str_builder_add_fmt(sb, "slice");
            break;
            
        case KSB_TUPLE:
            i += 4;
            ks_str_builder_add_fmt(sb, "tuple %i", val);
            break;

        case KSB_DICT:
            i += 4;
            ks_str_builder_add_fmt(sb, "dict %i", val);
            break;


        case KSB_GETITEM:
            i += 4;
            ks_str_builder_add_fmt(sb, "getitem %i", val);
            break;

        case KSB_SETITEM:
            i += 4;
            ks_str_builder_add_fmt(sb, "setitem %i", val);
            break;

        case KSB_CALL:
            i += 4;
            ks_str_builder_add_fmt(sb, "call %i", val);
            break;
        case KSB_VCALL:
            ks_str_builder_add_fmt(sb, "vcall");
            break;

        case KSB_RET:
            ks_str_builder_add_fmt(sb, "ret");
            break;

        case KSB_THROW:
            ks_str_builder_add_fmt(sb, "throw");
            break;


        case KSB_MAKE_ITER:
            ks_str_builder_add_fmt(sb, "make_iter");
            break;

        case KSB_ITER_NEXT:
            i += 4;
            ks_str_builder_add_fmt(sb, "iter_next %+i  # to %i", val, i + val);
            break;


        case KSB_JMP:
            i += 4;
            ks_str_builder_add_fmt(sb, "jmp %+i  # to %i", val, i + val);
            break;
        
        case KSB_JMPT:
            i += 4;
            ks_str_builder_add_fmt(sb, "jmpt %+i  # to %i", val, i + val);
            break;
                
        case KSB_JMPF:
            i += 4;
            ks_str_builder_add_fmt(sb, "jmpf %+i  # to %i", val, i + val);
            break;
        case KSB_TRY_START:
            i += 4;
            ks_str_builder_add_fmt(sb, "try_start %+i  # to %i", val, i + val);
            break;
        case KSB_TRY_END:
            i += 4;
            ks_str_builder_add_fmt(sb, "try_end %+i  # to %i", val, i + val);
            break;
        
        case KSB_ASSERT:
            ks_str_builder_add_fmt(sb, "assert");
            break;

        case KSB_NEW_FUNC:
            ks_str_builder_add_fmt(sb, "new_func");
            break;

        case KSB_ADD_CLOSURE:
            ks_str_builder_add_fmt(sb, "add_closure");
            break;


        case KSB_LOAD:
            i += 4;
            ks_str_builder_add_fmt(sb, "load %R  # idx: %i", self->v_const->elems[val], val);
            break;

        case KSB_STORE:
            i += 4;
            ks_str_builder_add_fmt(sb, "store %R  # idx: %i", self->v_const->elems[val], val);
            break;

        case KSB_LOAD_ATTR:
            i += 4;
            ks_str_builder_add_fmt(sb, "load_attr %R  # idx: %i", self->v_const->elems[val], val);
            break;

        case KSB_STORE_ATTR:
            i += 4;
            ks_str_builder_add_fmt(sb, "load_attr %R  # idx: %i", self->v_const->elems[val], val);
            break;

        #define OP_CASE(_op, _str) case _op: ks_str_builder_add_fmt(sb, "bop " _str); break;

        OP_CASE(KSB_BOP_ADD, "+")
        OP_CASE(KSB_BOP_SUB, "-")
        OP_CASE(KSB_BOP_MUL, "*")
        OP_CASE(KSB_BOP_DIV, "/")
        OP_CASE(KSB_BOP_MOD, "%%")
        OP_CASE(KSB_BOP_POW, "**")

        OP_CASE(KSB_BOP_LT, "<")
        OP_CASE(KSB_BOP_LE, "<=")
        OP_CASE(KSB_BOP_GT, ">")
        OP_CASE(KSB_BOP_GE, ">=")
        OP_CASE(KSB_BOP_EQ, "==")
        OP_CASE(KSB_BOP_NE, "!=")

        OP_CASE(KSB_BOP_BINOR, "|")
        OP_CASE(KSB_BOP_BINAND, "&")
        OP_CASE(KSB_BOP_BINXOR, "^")
        
        #define UOP_CASE(_op, _str) case _op: ks_str_builder_add_fmt(sb, "uop " _str); break;

        OP_CASE(KSB_UOP_POS, "+")
        OP_CASE(KSB_UOP_NOT, "!")
        OP_CASE(KSB_UOP_NEG, "-")
        OP_CASE(KSB_UOP_SQIG, "~")

        case KSB_TRUTHY:
            ks_str_builder_add_fmt(sb, "truthy");
            break;


        default:
            ks_str_builder_add_fmt(sb, "<err:%i>", (int)op);
            haderr = true;
            break;
        }

        ks_str_builder_add(sb, "\n", 1);

        if (haderr) break;
    }


    ks_str ret = ks_str_builder_get(sb);
    KS_DECREF(sb);

    return (ks_obj)ret;
}


// code.__free__(self) -> free a bytecode object
static KS_TFUNC(code, free) {
    ks_code self;
    KS_GETARGS("self:*", &self, ks_T_code)

    // free member variables
    KS_DECREF(self->v_const);
    ks_free(self->bc);

    if (self->parser) KS_DECREF(self->parser);

    ks_free(self->meta);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// code.__bytes__(self) - return the bytes
static KS_TFUNC(code, bytes) {
    ks_code self;
    KS_GETARGS("self:*", &self, ks_T_code)

    return (ks_obj)ks_bytes_new(self->bc, self->bc_n);
}




/* export */

KS_TYPE_DECLFWD(ks_T_code);

void ks_init_T_code() {
    ks_type_init_c(ks_T_code, "code", ks_T_object, KS_KEYVALS(
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(code_str_, "code.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(code_free_, "code.__free__(self)")},

        {"__bytes__",              (ks_obj)ks_cfunc_new_c_old(code_bytes_, "code.__bytes__(self)")},
    ));
}

