/* types/code.c - implementation of the bytecode 'code' type in kscript
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_code);

// create a kscript code given a constant array
ks_code ks_code_new(ks_list v_const) {

    ks_code self = KS_ALLOC_OBJ(ks_code);
    KS_INIT_OBJ(self, ks_type_code);
    
    if (v_const == NULL) {
        // create a new constants array
        v_const = ks_list_new(0, NULL);
        self->v_const = v_const;
    } else {
        // keep a reference to the constant array
        self->v_const = v_const;
        KS_INCREF(v_const);
    }

    // start with an empty bytecode
    self->bc_n = 0;
    self->bc = NULL;

    return self;
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
        if (ks_eq(val, self->v_const->elems[i])) {
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

void ksca_call   (ks_code self, int n_items) KSCA_B_I32(KSB_CALL, n_items)
void ksca_ret    (ks_code self) KSCA_B(KSB_RET)
void ksca_jmp    (ks_code self, int relamt) KSCA_B_I32(KSB_JMP, relamt)
void ksca_jmpt   (ks_code self, int relamt) KSCA_B_I32(KSB_JMPT, relamt)
void ksca_jmpf   (ks_code self, int relamt) KSCA_B_I32(KSB_JMPF, relamt)

void ksca_load      (ks_code self, ks_str name) KSCA_B_I32(KSB_LOAD, ks_code_add_const(self, (ks_obj)name))
void ksca_load_attr (ks_code self, ks_str name) KSCA_B_I32(KSB_LOAD_ATTR, ks_code_add_const(self, (ks_obj)name))
void ksca_store     (ks_code self, ks_str name) KSCA_B_I32(KSB_STORE, ks_code_add_const(self, (ks_obj)name))
void ksca_store_attr(ks_code self, ks_str name) KSCA_B_I32(KSB_STORE_ATTR, ks_code_add_const(self, (ks_obj)name))


void ksca_bop       (ks_code self, int ksb_bop_type) KSCA_B(ksb_bop_type)


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



// Output the code to a file (encoded), returns true on success
bool ks_code_tofile(ks_code self, char* fname) {
    FILE* fp = fopen(fname, "wb");

    if (!fp) {
        ks_warn("Failed to open file '%s' in 'ks_code_tofile()'", fname);
        return false;
    }

    // internal binary format:

    // first, write 'KSBC', this is a magic word effectively
    fprintf(fp, "KSBC\n");

    // next, output the length of the constants array used 
    fprintf(fp, "vcl:%i\n", (int)self->v_const->len);

    // now, output each line as a constant
    int i;
    for (i = 0; i < self->v_const->len; ++i) {
        ks_obj cur = self->v_const->elems[i];
        if (cur->type == ks_type_none) {
            fprintf(fp, "none:none");
        } else if (cur->type == ks_type_bool) {
            if (cur == KSO_TRUE) {
                fprintf(fp, "bool:true");
            } else {
                fprintf(fp, "bool:false");
            }
        } else if (cur->type == ks_type_int) {
            fprintf(fp, "int:%lli", (long long int)((ks_int)cur)->val);
        } else if (cur->type == ks_type_str) {
            // print it length encoded
            //fprintf(fp, "%lli", (long long int)((ks_int)cur)->val);
            // escaping ensures no newlines will be in the string
            ks_str repr_cur = ks_str_escape((ks_str)cur);
            fprintf(fp, "str:%s", repr_cur->chr);

            KS_DECREF(repr_cur);
        } else {
            // output an error; this is not a constant type
            fprintf(fp, "!err");
        }

        // end the line
        fprintf(fp, "\n");

    }

    // now, actually output the raw bytecode, first with the length and a newline

    fprintf(fp, "bcn:%i\n", (int)self->bc_n);

    // now, the rest of the bytes are binary format, just the bytes of the bytecode
    fwrite(self->bc, 1, self->bc_n, fp);

    fclose(fp);


    // success
    return true;
}



// Read back in a binary file
ks_code ks_code_fromfile(char* fname) {

    FILE* fp = fopen(fname, "rb");

    if (!fp) {
        ks_warn("Failed to open file '%s' in 'ks_code_fromfile()'", fname);
        return NULL;
    }

    char magic[256] = "";

    if (5 != fread(magic, 1, 5, fp)) {
        ks_warn("Failed to parse magic word 'KSBC' from file '%s' in 'ks_code_fromfile()'", fname);
        return NULL;
    }

    if (strncmp(magic, "KSBC\n", 5) != 0) {
        ks_warn("Failed to parse magic word 'KSBC' from file '%s' in 'ks_code_fromfile()'", fname);
        return NULL;
    }


    ks_code self = ks_code_new(NULL);

    // now, start parsing

    int vcl = 0;

    if (fscanf(fp, "vcl:%i\n", &vcl) != 1) {
        ks_warn("Failed to parse v_const length from file '%s' in 'ks_code_fromfile()'", fname);
        KS_DECREF(self);
        return NULL;
    }


    // current buffer size
    size_t cur_bsize = 0;
    // current line length
    int cur_len = 0;
    // the current line
    char* cur_line = NULL;

    // now, read those lines
    int i;
    for (i = 0; i < vcl; ++i) {

        if ((cur_len = ks_getline(&cur_line, &cur_bsize, fp)) > 0) {
            // remove ending newline if there was one
            if (cur_line[cur_len] =='\n') cur_line[cur_len] = '\0';

            // successfull
            //ks_info("Line: '%s'", cur_line);
            if (strncmp(cur_line, "str:", 4) == 0) {
                // parse out string
                ks_str s0 = ks_str_new(cur_line + 4);
                // undo the string formatting
                ks_str s1 = ks_str_unescape(s0);
                KS_DECREF(s0);
                
                // push it onto the constants list
                ks_list_push(self->v_const, (ks_obj)s1);
                KS_DECREF(s1);
            } else if (strncmp(cur_line, "int:", 4) == 0) {
                long long int val = 0;
                if (1 != fscanf(fp, "%lli", &val)) {
                    // error, incorrect format
                    continue;
                }
                ks_int i_val = ks_int_new((int64_t)val);
                ks_list_push(self->v_const, (ks_obj)i_val);
                KS_DECREF(i_val);
            }

        } else {
            ks_warn("Invalid line in file '%s'", fname);
        }
    }

    // free temporary line buffer
    ks_free(cur_line);


    // now, parse the actual bytes
    int bcn = 0;
    if (fscanf(fp, "bcn:%i\n", &bcn) != 1) {
        ks_warn("Failed to parse bcn length from file '%s' in 'ks_code_fromfile()'", fname);
        KS_DECREF(self);
        return NULL;
    }

    // allocate size
    self->bc_n = bcn;
    self->bc = ks_malloc(bcn);

    if (bcn != fread(self->bc, 1, bcn, fp)) {
        ks_warn("Failed to parse bc bytes (length %i) from file '%s' in 'ks_code_fromfile()'", bcn, fname);
        KS_DECREF(self);
        return NULL;
    }


    fclose(fp);

    return self;


}


// code.__str__(self) -> generate a string for the bytecode
static KS_TFUNC(code, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_code self = (ks_code)args[0];
    KS_REQ_TYPE(self, ks_type_code, "self");
    
    ks_str_b SB;
    ks_str_b_init(&SB);

    // first, dump out the constant list:
    ks_str_b_add_fmt(&SB, "\n# -*- code @ %p\n", self);
    ks_str_b_add_fmt(&SB, "# v_const (vc): %S\n", self->v_const);

    // now, iterate through all the instructions

    int i = 0;

    while (i < self->bc_n) {
        ks_str_b_add_fmt(&SB, "%04i ", i);

        ksb op = self->bc[i++];

        // just always read it if there is no possible buffer overrun, otherwise 0
        int val = (i + 4 >= self->bc_n) ? 0 : *(int *)(self->bc + i);

        switch (op)
        {
        case KSB_NOOP:
            ks_str_b_add_fmt(&SB, "noop");
            break;
        
        case KSB_PUSH:
            i += 4;
            ks_str_b_add_fmt(&SB, "push %R  # idx: %i", self->v_const->elems[val], val); // i peed.com
            break;

        case KSB_DUP:
            ks_str_b_add_fmt(&SB, "dup");
            break;

        case KSB_POPU:
            ks_str_b_add_fmt(&SB, "popu");
            break;


        case KSB_CALL:
            i += 4;
            ks_str_b_add_fmt(&SB, "call %i", val);
            break;

        case KSB_RET:
            ks_str_b_add_fmt(&SB, "ret");
            break;
        
        case KSB_JMP:
            i += 4;
            ks_str_b_add_fmt(&SB, "jmp %+i  # to %i", val, i + val);
            break;
        
        case KSB_JMPT:
            i += 4;
            ks_str_b_add_fmt(&SB, "jmpt %+i  # to %i", val, i + val);
            break;
                
        case KSB_JMPF:
            i += 4;
            ks_str_b_add_fmt(&SB, "jmpf %+i  # to %i", val, i + val);
            break;

        case KSB_LOAD:
            i += 4;
            ks_str_b_add_fmt(&SB, "load %R  # idx: %i", self->v_const->elems[val], val);
            break;

        case KSB_STORE:
            i += 4;
            ks_str_b_add_fmt(&SB, "store %R  # idx: %i", self->v_const->elems[val], val);
            break;

        case KSB_LOAD_ATTR:
            i += 4;
            ks_str_b_add_fmt(&SB, "load_attr %R  # idx: %i", self->v_const->elems[val], val);
            break;

        case KSB_STORE_ATTR:
            i += 4;
            ks_str_b_add_fmt(&SB, "load_attr %R  # idx: %i", self->v_const->elems[val], val);
            break;

        #define OP_CASE(_op, _str) case _op: ks_str_b_add_fmt(&SB, "bop " _str); break;

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


        default:
            ks_str_b_add_fmt(&SB, "<err>");
            break;
        }

        ks_str_b_add_c(&SB, "\n");

    }


    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    return (ks_obj)ret;
};


// code.__free__(self) -> free a bytecode object
static KS_TFUNC(code, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_code self = (ks_code)args[0];
    KS_REQ_TYPE(self, ks_type_code, "self");
    
    // free member variables
    KS_DECREF(self->v_const);
    ks_free(self->bc);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// initialize code type
void ks_type_code_init() {
    KS_INIT_TYPE_OBJ(ks_type_code, "code");

    ks_type_set_cn(ks_type_code, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(code_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(code_free_)},
        {NULL, NULL}   
    });
}

