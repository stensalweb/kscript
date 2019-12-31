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
    T_ast,
    T_parser,
    T_vm,
    T_cfunc
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
    ks_T_ast = &T_ast,
    ks_T_parser = &T_parser,
    ks_T_vm = &T_vm,
    ks_T_cfunc = &T_cfunc
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

// create a new string from C-style printf arguments
ks_str ks_str_new_vcfmt(const char* fmt, va_list ap) {
    ks_str self = (ks_str)ks_malloc(sizeof(*self) + 0);
    *self = (struct ks_str) {
        KSO_BASE_INIT(ks_T_str, KSOF_NONE)
        .len = 0,
    };

    // the fields for the format argument:
    // i.e. '%*d' maps to `*` as a fmt_field
    static char fmt_fld[256];

    // temporary chars for constructing integers/etc
    static char tmp[256];
    static int tmp_i;

    // digits for bases > 10
    static const char base_digs[] = "0123456789abcdefghijklmnopqrstuv";

    // macro to determine if a character is alpha (part of the alphabet)
    #define ISALPHA(_c) (((_c) >= 'a' && (_c) <= 'z') || ((_c) >= 'A' && (_c) <= 'Z'))

    // append characters to the string
    #define VCFMT_ADD(_len, _ptr) { \
        int __len = (_len); \
        self = ks_realloc(self, sizeof(*self) + self->len + __len); \
        memcpy(&(self->chr[self->len]), (_ptr), __len); \
        self->len += __len;  \
    }

    // append to the tmp buffer
    #define VCFMT_TMP_ADD(_len, _ptr) { \
        memcpy(&tmp[tmp_i], _ptr, _len); \
        tmp_i += _len; \
    }


    // reverse the temp buffer, starting at _start, and for _len number
    #define VCFMT_TMP_REV(_start, _stop) { \
        for (j = _start; 2 * (j - _start) < _stop - _start; ++j) { \
            char tc = tmp[j]; \
            tmp[j] = tmp[_stop - j - 1 + _start]; \
            tmp[_stop - j - 1 + _start] = tc; \
        } \
    }

    // number of digits for printing out the digits of a float/double
    #define VCFMT_F_DIGITS 5

    // current pointer to the format
    int i, j;
    for (i = 0; fmt[i] != '\0'; ) {

        if (fmt[i] == '%') {
            // we have hit a format specifier, time to parse
            // skip the `%`
            i++;

            // length of the field
            int fld_len = 0;

            // parse until we get to an alpha characer
            while (fmt[i] && !ISALPHA(fmt[i])) {
                fmt_fld[fld_len++] = fmt[i];
                i++;
            }
            fmt_fld[fld_len] = '\0';

            // get the specifier, then skip it
            char spec = fmt[i++];

            /* valid specifiers: 
                i: int
                f: float/double
                c: char
                s: char* 
                p: void*
                o: kso
            */

            if (spec == 'i') {
                // i: int, print out an integer value, base 10, with sign
                // %i -> integer, base 10
                // %+i -> integer, with sign always prepended
                // first, pull out the argument given
                int d_val = va_arg(ap, int);

                // extract if there was + in format field
                bool f_sgn = strchr(fmt_fld, '+') != NULL;

                // extract sign info
                bool is_neg = d_val >= 0 ? false : (d_val = -d_val, true);

                // we are outputting the base-10 representation into tmp, reversed
                // NOTE: This will output at least one digit, so it will never be empty
                tmp_i = 0;
                do {
                    // extract a digit at a time
                    tmp[tmp_i++] = (d_val % 10) + '0';
                    d_val /= 10;
                } while (d_val > 0);

                // add sign if negative, or the + field specifier was given
                if (is_neg) tmp[tmp_i++] = '-';
                else if (f_sgn) tmp[tmp_i++] = '+';

                // reverse the whole string
                VCFMT_TMP_REV(0, tmp_i)

                // add the string to the output
                VCFMT_ADD(tmp_i, tmp);

            } else if (spec == 'f') {
                // f: float, print out floating point value to some digits
                // %f -> float, base 10, to 4 digits (which will be 0-filled)
                // %+f -> float, with sign always prepended

                // NOTE: as per the C spec, calling with float or double is always promoted to double:
                //   https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
                double f_val = va_arg(ap, double);

                // number of digits to print
                int n_digs = VCFMT_F_DIGITS;

                // the digit base
                uint64_t dig_base = (uint64_t)(pow(10, n_digs));


                // extract if there was + in format field
                bool f_sgn = strchr(fmt_fld, '+') != NULL;

                // extract sign info
                bool is_neg = f_val >= 0 ? false : (f_val = -f_val, true);


                // extract whether or not to print in scientific notation
                bool do_sci = false;

                // tell whether or not the scientific notation is negative or positive (small or large value)
                bool sci_neg = false;

                // number of exponentials to put at the end
                int sci_exp = 0;


                if (f_val > 1e6) {
                    // do scientific notation
                    while (f_val >= 10.0) {
                        sci_exp++;
                        f_val /= 100;
                    }
                    do_sci = true;
                    sci_neg = false;


                } else if (f_val < 1e-6) {
                    // do scientific notation
                    while (f_val < 1.0) {
                        sci_exp++;
                        f_val *= 10;
                    }
                    do_sci = true;
                    sci_neg = true;
                }
                
                // output into the temporary buffer
                tmp_i = 0;


                // holds a digit/integer value
                uint64_t d_val;

                if (do_sci) {
                    // first, put out 'e+N', where N==sci_exp
                    // but, it should be backwards
                    d_val = sci_exp;
                    do {
                        tmp[tmp_i++] = (d_val % 10) + '0';
                        d_val /= 10;
                    } while (d_val > 0);

                    // now, output sign
                    tmp[tmp_i++] = sci_neg ? '-' : '+';

                    // now, output the literal 'e'
                    tmp[tmp_i++] = 'e';

                }

                // print in normal notation

                // first, output the fractional part
                d_val = (uint64_t)((f_val - (uint64_t)f_val) * dig_base);

                do {
                    tmp[tmp_i++] = (d_val % 10) + '0';
                    d_val /= 10;
                    n_digs--;
                } while (n_digs > 0);

                // add seperator
                tmp[tmp_i++] = '.';

                // now, output the whole number part
                d_val = (uint64_t)(f_val);
                do {
                    tmp[tmp_i++] = (d_val % 10) + '0';
                    d_val /= 10;
                } while (d_val > 0);

                // add sign if negative, or the + field specifier was given
                if (is_neg) tmp[tmp_i++] = '-';
                else if (f_sgn) tmp[tmp_i++] = '+';

                // reverse the whole string
                VCFMT_TMP_REV(0, tmp_i)

                // add to the output
                VCFMT_ADD(tmp_i, tmp);

            } else if (spec == 'c') {

 
                // how many times should the character be printed?
                int n_times = 1;

                if (strcmp(fmt_fld, "*") == 0) {
                    // we read in an integer to determine how many times to print
                    n_times = va_arg(ap, int);

                }

                // get the char
                int c_val = va_arg(ap, int);

                int ii;
                for (ii = 0; ii < n_times; ++ii) {
                    VCFMT_ADD(1, &c_val);
                }

            } else if (spec == 's') {
                // s for string, print a C-style NUL-terminated string
                int s_val_len = -1;

                // allow %*s to be given an integer value
                if (strcmp(fmt_fld, "*") == 0) {
                    // we read in an integer length of the string
                    s_val_len = va_arg(ap, int);
                }

                const char* s_val = va_arg(ap, const char*);

                if (s_val_len < 0) {
                    // we calculate it, as it as not given
                    s_val_len= strlen(s_val);
                }

                VCFMT_ADD(s_val_len, s_val);

            } else if (spec == 'o') {
                // 'o' for object, print the object's tostring
                kso o_val = va_arg(ap, kso);

                // get the type
                ks_type o_T = o_val->type;

                tmp_i = 0;

                tmp[tmp_i++] = '<';
                tmp[tmp_i++] = '\'';
                VCFMT_TMP_ADD(o_T->name->len, o_T->name->chr);
                tmp[tmp_i++] = '\'';

                VCFMT_TMP_ADD(sizeof(" obj @ 0x") - 1, " obj @ 0x");

                int addr_st = tmp_i;
                uintptr_t p_val = (uintptr_t)o_val;
                // output hex, backwards, into tmp
                do {
                    // extract a digit at a time
                    tmp[tmp_i++] = base_digs[p_val % 16];
                    p_val /= 16;
                } while (p_val > 0);

                // reverse the address
                VCFMT_TMP_REV(addr_st, tmp_i);

                tmp[tmp_i++] = '>';

                // add the type name
                VCFMT_ADD(tmp_i, tmp);


            } else if (spec == 'p') {
                // `p` for pointer, void*, print as a hex address
                
                uintptr_t p_val = (uintptr_t)(va_arg(ap, void*));

                tmp_i = 0;

                // output hex, backwards, into tmp
                do {
                    // extract a digit at a time
                    tmp[tmp_i++] = base_digs[p_val % 16];
                    p_val /= 16;
                } while (p_val > 0);

                tmp[tmp_i++] = 'x';
                tmp[tmp_i++] = '0';

                // now, reverse the string
                VCFMT_TMP_REV(0, tmp_i)


                // add the string to the output
                VCFMT_ADD(tmp_i, tmp);

            } else {
                // take an argument off just in case, this may prevent an error
                va_arg(ap, double);

                // add a literal ! to signify an error
                VCFMT_ADD(1, "!");
            }

        } else {
            int s_i = i;
            // else, go through, scan for all the literal values, until we hit the end or format specifier `%`
            for (; fmt[i] != '\0' && fmt[i] != '%'; ++i) ;

            // append them here
            VCFMT_ADD((i - s_i), &fmt[s_i]);
        }
    }

    self->chr[self->len] = '\0';
    self->v_hash = str_hash(self->len, self->chr);

    return self;
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

// called when the object should be freed

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



// create a new empty piece of code
ks_code ks_code_new_empty(ks_list v_const) {
    ks_code self = (ks_code)ks_malloc(sizeof(*self));
    *self = (struct ks_code) {
        KSO_BASE_INIT(ks_T_code, KSOF_NONE)
        .v_const = v_const
    };

    // record our reference
    KSO_INCREF(v_const);
    return self;
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

void ksc_add(ks_code code) { KSC_(KSBC_ADD); }
void ksc_sub(ks_code code) { KSC_(KSBC_SUB); }
void ksc_mul(ks_code code) { KSC_(KSBC_MUL); }
void ksc_div(ks_code code) { KSC_(KSBC_DIV); }
void ksc_jmp(ks_code code, int relamt) { KSC_I32(KSBC_JMP, relamt); }
void ksc_jmpt(ks_code code, int relamt) { KSC_I32(KSBC_JMPT, relamt); }
void ksc_jmpf(ks_code code, int relamt) { KSC_I32(KSBC_JMPF, relamt); }


// create a new AST representing a constant int
ks_ast ks_ast_new_int(int64_t v_int) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        .atype = KS_AST_INT,
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
        .atype = KS_AST_TRUE
    };
    return self;
}

// create a new AST representing 'false'
ks_ast ks_ast_new_false() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        .atype = KS_AST_FALSE
    };
    return self;
}

// create a new AST representing 'none'
ks_ast ks_ast_new_none() {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
        .atype = KS_AST_NONE
    };
    return self;
}



// create a new AST representing a variable reference
ks_ast ks_ast_new_var(const char* var_name) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
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
        .atype = KS_AST_VAR,
        .v_var = ks_str_new(len, var_name),
    };
    KSO_INCREF(self->v_var);
    return self;
}


// create a new AST representing a functor call, with `items[0]` being the function
// so, `n_items` should be `n_args+1`, since it includes function, then arguments
ks_ast ks_ast_new_call(int n_items, ks_ast* items) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
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
        .atype = KS_AST_IF,
        .v_if = {cond, body}
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
        .atype = KS_AST_BLOCK,
        .v_block = ks_list_new(n_items, (kso*)items)
    };
    KSO_INCREF(self->v_block);
    return self;
}

// createa a new AST representing a block of code
ks_ast ks_ast_new_code(ks_code code) {
    ks_ast self = (ks_ast)ks_malloc(sizeof(*self));
    *self = (struct ks_ast) {
        KSO_BASE_INIT(ks_T_ast, KSOF_NONE)
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
    case KS_AST_CALL:
        KSO_DECREF(self->v_call);
        break;

    case KS_AST_CODE:
        KSO_DECREF(self->v_code);
        break;

    case KS_AST_BLOCK:
        KSO_DECREF(self->v_block);
        break;

    case KS_AST_IF:
        KSO_DECREF(self->v_if.cond);
        KSO_DECREF(self->v_if.body);
        break;


    // handle all binary operators
    case KS_AST_BOP_ADD:
    case KS_AST_BOP_SUB:
    case KS_AST_BOP_MUL:
    case KS_AST_BOP_DIV:
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


ITEMS:
[(k, h, v), (k, h, v), ...]
Dense array of items that have been added to the dictionary, in the order they were added

BUCKETS:
[-1, -1, ... 5, ...]
These hold indexes into the `items` array, _BUCK_EMPTY means that bucket is empty (unoccupied), while >= 0 means
  that is the index into the items array
*/


// starting length for the dictionary
#define _DICT_MIN_LEN 8

// represents an unused bucket
#define _BUCK_EMPTY (-1)

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
        .items = NULL,
        .n_buckets = _DICT_MIN_LEN,
        .buckets = (int32_t*)ks_malloc(sizeof(int32_t) * _DICT_MIN_LEN)
    };

    int i;
    for (i = 0; i < _DICT_MIN_LEN; ++i) {
        self->buckets[i] = _BUCK_EMPTY;
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

    // save the old buckets
    int old_n_buckets = self->n_buckets;
    int32_t* old_buckets = self->buckets;

    // allocate the new buckets
    self->n_buckets = nextprime(new_size);
    self->buckets = ks_malloc(sizeof(*self->buckets) * self->n_buckets);

    int i;
    for (i = 0; i < self->n_buckets; ++i) {
        self->buckets[i] = _BUCK_EMPTY;
    }

    for (i = 0; i < self->n_items; ++i) {
        // now, set all the existing entries by rehashing them
        struct ks_dict_entry entry = self->items[i];
        int b_idx = dict_buck(self, entry.hash);
        int tries = 0;

        // keep going while there's not an empty bucket
        while (self->buckets[b_idx] != _BUCK_EMPTY && ++tries < self->n_buckets) {
            // search for the next dictionary
            b_idx = dict_buck_next(b_idx, tries);

            // make sure it wraps around
            while (b_idx > self->n_buckets) b_idx -= self->n_buckets;
        }

        if (self->buckets[b_idx] == _BUCK_EMPTY) {
            // we found an open bucket
            self->buckets[b_idx] = i;
        } else {
            // some error, which shouldn't happen
            ks_error("INTERNAL DICT RESIZE ERROR");
        }
    }

    // free the old buckets
    ks_free(old_buckets);
}

int ks_dict_set(ks_dict self, kso key, uint64_t hash, kso val) {

    if (self->n_buckets * _DICT_LOAD_MAX <= self->n_items * 100) {
        ks_dict_resize(self, _DICT_NEW_SIZE(self));
    }

    int b_idx = dict_buck(self, hash), i_idx = -1;
    int tries = 0;

    // keep going while there's not an empty bucket
    while ((i_idx = self->buckets[b_idx]) != _BUCK_EMPTY && ++tries < self->n_buckets) {

        if (dict_entry_matches(self->items[i_idx], key, hash)) {
            // we located it, so just update the value and return
            kso old_val = self->items[i_idx].val;
            self->items[i_idx].val = val;

            // record reference changes; none is neccessary to the key, since it was already here
            KSO_INCREF(val);
            KSO_DECREF(old_val);

            return i_idx;

        } else {
            // else, keep searching

            // search for the next dictionary
            b_idx = dict_buck_next(b_idx, tries);

            // make sure it wraps around
            while (b_idx > self->n_buckets) b_idx -= self->n_buckets;
        }
    }

    if (self->buckets[b_idx] == _BUCK_EMPTY) {
        // we found an empty bucket

        // so now, we need to append the item to the end of our items array
        i_idx = self->n_items++;
        self->buckets[b_idx] = i_idx;

        // reallocate the items array
        self->items = ks_realloc(self->items, sizeof(*self->items) * self->n_items);
        self->items[i_idx] = (struct ks_dict_entry) {
            .key = key,
            .hash = hash,
            .val = val
        };

        // record references, we took one to the key, and one to the value
        KSO_INCREF(key);
        KSO_INCREF(val);

        return i_idx;
    } else {
        // otherwise, still collided all the way
        ks_warn("COLLISION");
        return -1;
    }
}


kso ks_dict_get(ks_dict self, kso key, uint64_t hash) {
    int b_idx = dict_buck(self, hash);
    int tries = 0;

    while (self->buckets[b_idx] == _BUCK_EMPTY && ++tries < self->n_buckets) {
        // find a bucket by triangular probing
        b_idx = dict_buck_next(b_idx, tries);

        // make sure it wraps around
        if (b_idx > self->n_buckets) b_idx -= self->n_buckets;
    }

    if (self->buckets[b_idx] == _BUCK_EMPTY) {
        // we still didn't find anything
        return NULL;
    } else {
        // we found a bucket, now make sure it matches
        int i_idx = self->buckets[b_idx];
        return self->items[i_idx].val;
    }
}

KS_CFUNC_TDECL(dict, free) {

    // get the arguments
    ks_dict self = (ks_dict)args[0];

    int i;

    for (i = 0; i < self->n_items; ++i) {
        KSO_DECREF(self->items[i].key);
        KSO_DECREF(self->items[i].val);
    }

    // free our dense items list
    ks_free(self->items);

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

    // otherwise, free it
    ks_trace("freeing '%s' obj @ %p", obj->type->name->chr, (void*)obj);

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
        .f_free = (kso)ks_cfunc_newref(tuple_free),
    };

    *ks_T_list = (struct ks_type) {
        KS_TYPE_INIT
        .name = ks_str_new_r("list"),
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


