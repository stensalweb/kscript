/* types/str.c - kscript's string implementation
 *
 * This file includes most elementary string operations
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_str);

// string characters
struct ks_str KS_STR_CHARS[256];

// create a kscript string from a C-style string with a length (not including NUL-terminator)
ks_str ks_str_new_l(char* chr, int len) {
    assert (len >= 0 && "ks_str creation encountered negative length!");
    if (len == 0) {
        // return empty string
        return (ks_str)KS_NEWREF(&KS_STR_CHARS[0]);    
    } else if (len == 1) {
        return (ks_str)KS_NEWREF(&KS_STR_CHARS[*chr]);    
    }

    ks_str self = (ks_str)ks_malloc(sizeof(struct ks_str) + len);
    KS_INIT_OBJ(self, ks_type_str);

    // set the length
    self->len = len;

    // copy in all the data
    memcpy(self->chr, chr, len);

    self->chr[len] = '\0';

    // calculate the hash for the string when it gets created
    self->v_hash = ks_hash_bytes(self->len, self->chr);

    return self;
}

// create a kscript string from a C-style string
ks_str ks_str_new(char* chr) {
    return ks_str_new_l(chr, chr == NULL ? 0 : strlen(chr));
}

// Escape the string 'A', i.e. replace '\' -> '\\', and newlines to '\n'
ks_str ks_str_escape(ks_str A) {

    // generate a string representation
    ks_str_b SB;
    ks_str_b_init(&SB);

    int i;
    for (i = 0; i < A->len; ++i) {
        char c = A->chr[i];
        /**/ if (c == '\\') ks_str_b_add(&SB, 2, "\\\\");
        else if (c == '\n') ks_str_b_add(&SB, 2, "\\n");
        else if (c == '\t') ks_str_b_add(&SB, 2, "\\t");
        else {
            // just add character
            ks_str_b_add(&SB, 1, &c);
        }
    }

    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);
    return ret;
}

// Undo the string escaping, i.e. replaces '\n' with a newline
ks_str ks_str_unescape(ks_str A) {
    // generate a string representation
    ks_str_b SB;
    ks_str_b_init(&SB);

    int i;
    for (i = 0; i < A->len; ++i) {
        char c = A->chr[i];
        /**/ if (c == '\\') {
            // interpret escape code
            i++;
            c = A->chr[i];
            /**/ if (c == '\\') ks_str_b_add(&SB, 1, "\\");
            else if (c == 'n') ks_str_b_add(&SB, 1, "\n");
            else if (c == 't') ks_str_b_add(&SB, 1, "\t");
            else {
                // unknown escape code
            }
        }
        else {
            // just add character
            ks_str_b_add(&SB, 1, &c);
        }
    }

    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);
    return ret;
}

int ks_str_cmp(ks_str A, ks_str B) {
    if (A->len != B->len) return A->len - B->len;
    else return memcmp(A->chr, B->chr, A->len);
}

// str.__new__(obj) -> convert any object to a string
static KS_TFUNC(str, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_obj obj = args[0];

    if (obj->type == ks_type_str) {
        return KS_NEWREF(obj);
    } else if (obj->type->__str__ != NULL) {
        return ks_call(obj->type->__str__, n_args, args);
    } else {
        // do a default format
        return (ks_obj)ks_fmt_c("<'%T' obj @ %p>", obj, obj);
        //return ks_throw_fmt(ks_type_Error, "'%T' object is not convertable to string!", obj);
    }
};

// str.__free__(self) -> free a string object
static KS_TFUNC(str, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_type_str, "self");
    if (self >= &KS_STR_CHARS[0] || self <= &KS_STR_CHARS[255]) {
        // global singleton
        self->refcnt = 0xFFFF;
        return KSO_NONE;
    }

    // nothing else is needed because the string is allocated with enough bytes for all the characters    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// str.__len__(self) -> get the length
static KS_TFUNC(str, len) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_type_str, "self");

    return (ks_obj)ks_int_new(self->len);
};

// str.__repr__(self) -> get the string representation
static KS_TFUNC(str, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_type_str, "self");

    return (ks_obj)ks_fmt_c("'%S'", self);
};

// str.__add__(L, R) -> return the sum of 2 objects, by summing them
static KS_TFUNC(str, add) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    return (ks_obj)ks_fmt_c("%S%S", L, R);
}

// string comparison function
// TODO: optimize this?
static int my_strcmp(ks_str L, ks_str R) {
    return strcmp(L->chr, R->chr);
}


// str.__cmp__(L, R) -> cmp 2 strings
static KS_TFUNC(str, cmp) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_str && R->type == ks_type_str) {
        int res = my_strcmp((ks_str)L, (ks_str)R);
        return (ks_obj)ks_int_new(res > 0 ? 1 : (res < 0 ? -1 : 0));
    }

    KS_ERR_BOP_UNDEF("<=>", L, R);
};

// str.__eq__(L, R) -> check whether objects are equal
static KS_TFUNC(str, eq) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_obj L = args[0], R = args[1];

    if (L->type == ks_type_str && R->type == ks_type_str) {
        ks_str sL = (ks_str)L, sR = (ks_str)R;
        bool res = sL->v_hash == sR->v_hash && sL->len == sR->len && strncmp(sL->chr, sR->chr, sL->len) == 0;
        return KSO_BOOL(res);
    }

    KS_ERR_BOP_UNDEF("==", L, R);
}


// search a string for a given character
static bool my_strnchr(char* coll, int n, char c) {
    int i;
    for (i = 0; i < n; ++i) {
        if (coll[i] == c) return true;
    }
    return false;
}

// str.split(self, delim=' \t\n') -> split a string on a given delimeter
static KS_TFUNC(str, split) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_type_str, "self");

    int n_delims = 0;
    char* delims = NULL;

    if (n_args == 2) {
        ks_str delim_o = (ks_str)args[1];
        KS_REQ_TYPE(delim_o, ks_type_str, "delim");

        // take from this one
        n_delims = delim_o->len;
        delims = delim_o->chr;
    } else {
        // default
        n_delims = 3;
        delims = " \t\n";
    }

    // the list of tokens
    ks_list ret = ks_list_new(0, NULL);

    ks_str tmp = NULL;

    int last_pos = 0;
    int i;
    for (i = 0; i < self->len; ++i) {
        if (my_strnchr(delims, n_delims, self->chr[i])) {
            // found a character, so add the last pos until the current to it
            tmp = ks_str_new_l(self->chr + last_pos, i - last_pos);
            ks_list_push(ret, (ks_obj)tmp);
            KS_DECREF(tmp);

            // update it for next time
            last_pos = i + 1;
        } else {
            // do nothing
        }
    }

    // push the last match (which may be empty)
    tmp = ks_str_new_l(self->chr + last_pos, i - last_pos);
    ks_list_push(ret, (ks_obj)tmp);

    KS_DECREF(tmp);

    // just append their string conversions
    return (ks_obj)ret;
};

// str.find(self, target) -> return the index at which 'target' appears in 'self', or '-1' if it is not contained in anything
static KS_TFUNC(str, find) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_str self = (ks_str)args[0];
    KS_REQ_TYPE(self, ks_type_str, "self");
    ks_str target = (ks_str)args[1];
    KS_REQ_TYPE(target, ks_type_str, "target");

    if (target->len == 0 || self->len == 0) return (ks_obj)ks_int_new(-1);

    int i;
    for (i = 0; i + target->len <= self->len; ++i) {
        if (strncmp(self->chr + i, target->chr, target->len) == 0) {
            return (ks_obj)ks_int_new(i);
        }
    }

    // just append their string conversions
    return (ks_obj)ks_int_new(-1);
};





// initialize string type
void ks_type_str_init() {
    KS_INIT_TYPE_OBJ(ks_type_str, "str");

    int i;
    // initialize single character string singletons
    for (i = 0; i < 256; ++i) {

        KS_INIT_OBJ(&KS_STR_CHARS[i], ks_type_str);

        // set the length
        KS_STR_CHARS[i].len = i == 0 ? 0 : 1;

        // copy in all the data
        memcpy(KS_STR_CHARS[i].chr, &i, KS_STR_CHARS[i].len);

        KS_STR_CHARS[i].chr[KS_STR_CHARS[i].len] = '\0';

        // calculate the hash for the string when it gets created
        KS_STR_CHARS[i].v_hash = ks_hash_bytes(KS_STR_CHARS[i].len, KS_STR_CHARS[i].chr);

    }

    // set properties
    ks_type_set_cn(ks_type_str, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(str_new_, "str.__new__(obj)")},
        {"__free__", (ks_obj)ks_cfunc_new2(str_free_, "str.__free__(self)")},

        {"__len__", (ks_obj)ks_cfunc_new2(str_len_, "str.__len__(self)")},

        {"__repr__", (ks_obj)ks_cfunc_new2(str_repr_, "str.__repr__(self)")},

        {"__add__", (ks_obj)ks_cfunc_new2(str_add_, "str.__add__(L, R)")},

        {"__cmp__", (ks_obj)ks_cfunc_new2(str_cmp_, "str.__cmp__(L, R)")},
        {"__eq__", (ks_obj)ks_cfunc_new2(str_eq_, "str.__eq__(L, R)")},

        {"split", (ks_obj)ks_cfunc_new2(str_split_, "str.split(self, delim=' \\t\\n')")},
        {"find", (ks_obj)ks_cfunc_new2(str_find_, "str.find(self, target)")},

        {NULL, NULL},
    });
}

