/* funcs.c - implementation of standard/builtin functions
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


ks_cfunc
    ks_F_print = NULL,
    ks_F_getattr = NULL,
    ks_F_setattr = NULL,
    ks_F_getitem = NULL,
    ks_F_setitem = NULL,

    ks_F_iter = NULL,
    ks_F_next = NULL,

    ks_F_recurse = NULL,

    ks_F_truthy = NULL,

    ks_F_repr = NULL,
    ks_F_hash = NULL,
    ks_F_id = NULL,
    ks_F_len = NULL,
    ks_F_typeof = NULL,

    ks_F_import = NULL,

    ks_F_issub = NULL,

    ks_F_any = NULL,
    ks_F_all = NULL,

    ks_F_sort = NULL,
    ks_F_map = NULL,
    ks_F_sum = NULL,
    ks_F_filter = NULL,

    ks_F_chr = NULL,
    ks_F_ord = NULL,

    ks_F_add = NULL,
    ks_F_sub = NULL,
    ks_F_mul = NULL,
    ks_F_div = NULL,
    ks_F_mod = NULL,
    ks_F_pow = NULL,

    ks_F_binand = NULL,
    ks_F_binor = NULL,
    ks_F_binxor = NULL,

    ks_F_lshift = NULL,
    ks_F_rshift = NULL,

    ks_F_cmp = NULL,

    ks_F_lt = NULL,
    ks_F_gt = NULL,
    ks_F_le = NULL,
    ks_F_ge = NULL,
    ks_F_eq = NULL,
    ks_F_ne = NULL,

    ks_F_pos = NULL,
    ks_F_neg = NULL,
    ks_F_abs = NULL,
    ks_F_sqig = NULL,

    ks_F_eval = NULL,
    ks_F_exec_file = NULL,
    ks_F_exec_expr = NULL,
    ks_F_exec_interactive = NULL

;


/* Misc. Kscript Functions */

// number of bytes written to the stdout
static int64_t _print_numbytes = 0;

// print (*args) -> print arguments
static KS_FUNC(print) {
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("*args", &n_extra, &extra)

    ks_str_builder sb = ks_str_builder_new();

    int i;
    for (i = 0; i < n_extra; ++i) {
        if (i > 0) ks_str_builder_add(sb, " ", 1);
        if (!ks_str_builder_add_str(sb, extra[i])) {
            KS_DECREF(sb);
            return NULL;
        }
    }

    ks_str toprint = ks_str_builder_get(sb);
    KS_DECREF(sb);

    _print_numbytes += toprint->len_b + 1;

    // print out to console
    printf("%s\n", toprint->chr);
    KS_DECREF(toprint);

    return KSO_NONE;
}


// __recurse__ (*args) - recursively call the current function
static KS_FUNC(recurse) {
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("*args", &n_extra, &extra)

    ks_thread th = ks_thread_get();

    // since the top frame should be the __recurse__ function, we need the one under that
    ks_stack_frame targ = (ks_stack_frame)th->frames->elems[th->frames->len - 2];

    return ks_obj_call(targ->func, n_extra, extra);
}



/* Iterators */

// iter(obj) -> turn into iterable
static KS_FUNC(iter) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)

    if (obj->type->__next__ != NULL) {
        // already is iterable; just return it
        return KS_NEWREF(obj);
    } else if (obj->type->__iter__ != NULL) {
        // create an iterable and return it
        return ks_obj_call(obj->type->__iter__, 1, &obj);
    } else {
        KS_THROW_ITER_ERR(obj);
    }
}

// next(obj) -> return next object for an iterable
static KS_FUNC(next) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)

    if (obj->type->__next__ != NULL) {
        // create an iterable and return it
        return ks_obj_call(obj->type->__next__, 1, &obj);
    } else {
        KS_THROW_ITER_ERR(obj);
    }
}

// hash(obj) - calculate hash
static KS_FUNC(hash) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)

    ks_hash_t res;
    if (!ks_obj_hash(obj, &res)) return NULL;


    if ((int64_t)res < 0) {
        // overflow; correct for this
        char tmp[256];
        snprintf(tmp, sizeof(tmp) - 1, "%llx", (long long unsigned int)res);

        return (ks_obj)ks_int_new_s(tmp, 16);

    }

    return (ks_obj)ks_int_new(res);
}

// id(obj) - return unique identifier
static KS_FUNC(id) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)
    
    return (ks_obj)ks_int_new((intptr_t)obj);
}

// len(obj, *args) -> calculate 'length' of object
static KS_FUNC(len) {
    ks_obj obj;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("obj *args", &obj, &n_extra, &extra)

    if (obj->type->__len__ != NULL) {
        return ks_obj_call(obj->type->__len__, n_args, args);
    }
    
    KS_THROW_METH_ERR(obj, "__len__");
}

// repr(obj) -> calculate represnetation
static KS_FUNC(repr) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)

    if (obj->type->__repr__ != NULL) {
        return ks_obj_call(obj->type->__repr__, 1, &obj);
    }
    
    KS_THROW_METH_ERR(obj, "__repr__");
}


// typeof(obj) -> get type of object
static KS_FUNC(typeof) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)

    return KS_NEWREF(obj->type);
}


// truthy(obj) - calculate a boolean from an object
static KS_FUNC(truthy) {
    ks_obj obj;
    KS_GETARGS("obj", &obj)

    // check for error
    int tru = ks_obj_truthy(obj);
    if (tru < 0) return NULL;

    // return object
    return KSO_BOOL(tru);

}


/* Get/Set Attr/Item */

// getattr(obj, key) -> set attribute
static KS_FUNC(getattr) {
    ks_obj obj, key;
    KS_GETARGS("obj key", &obj, &key)

    if (obj->type->__getattr__ != NULL) {
        // it has a getattr
        // use the arguments, since they should be in the correct order
        return ks_obj_call(obj->type->__getattr__, n_args, args);
    } else if (key->type == ks_T_str) {
        // attempt to find a method

        ks_obj type_attr = ks_type_get(obj->type, (ks_str)key);
        if (type_attr != NULL) {
            // was valid attribute
            if (ks_obj_is_callable(type_attr)) {
                // create a partial function (i.e. a member function) from the callable attribute
                ks_pfunc ret = ks_pfunc_new(type_attr, obj);
                KS_DECREF(type_attr);

                if (ret != NULL) {
                    // created member function
                    return (ks_obj)ret;
                }
            }
        }
    }

    KS_THROW_ATTR_ERR(obj, key);
    //KS_THROW_METH_ERR(obj, "__getattr__");
}

// setattr(obj, key, val) -> set subscript of item
static KS_FUNC(setattr) {
    ks_obj obj, key, val;
    KS_GETARGS("obj key val", &obj, &key, &val)

    if (obj->type->__setattr__ != NULL) {
        // it has a setattr
        // use the arguments, since they should be in the correct order
        return ks_obj_call(obj->type->__setattr__, n_args, args);
    }

    KS_THROW_METH_ERR(obj, "__setattr__");
}



// getitem(obj, *args) -> return subscript of item
static KS_FUNC(getitem) {
    ks_obj obj;
    int n_extra;
    ks_obj* extra = NULL;
    KS_GETARGS("obj *args", &obj, &n_extra, &extra)

    if (obj->type->__getitem__ != NULL) {

        // it has a getitem
        // use the arguments, since they should be in the correct order
        ks_obj r =  ks_obj_call(obj->type->__getitem__, n_args, args);

        return r;
    }

    KS_THROW_METH_ERR(obj, "__getitem__");
}

// setitem(obj, *args) -> set subscript of item
static KS_FUNC(setitem) {
    ks_obj obj;
    int n_extra;
    ks_obj* extra = NULL;
    KS_GETARGS("obj *args", &obj, &n_extra, &extra)

    if (obj->type->__setitem__ != NULL) {
        // it has a getitem
        // use the arguments, since they should be in the correct order
        return ks_obj_call(obj->type->__setitem__, n_args, args);
    }

    KS_THROW_METH_ERR(obj, "__setitem__");
}


// chr(ord) - convert ordinal to (single character) string
static KS_FUNC(chr) {
    int64_t ord;
    KS_GETARGS("ord:i64", &ord)

    ks_unich to_unich = ord;
    if (to_unich != ord) {
        return ks_throw(ks_T_ArgError, "chr() given invalid value! Expected between 0 and 11141111, but got '%z'", (ks_size_t)ord);
    }

    return (ks_obj)ks_str_chr(to_unich);
}

// ord(chr) - convert ordinal to (single character) string
static KS_FUNC(ord) {
    ks_str chr;
    KS_GETARGS("chr:*", &chr, ks_T_str)
    ks_unich r = ks_str_ord(chr);

    return r >= 0 ? (ks_obj)ks_int_new(r) : NULL;
}


/* Misc. */

// __import__(name) - import a given module name
static KS_FUNC(import) {
    ks_str name;
    KS_GETARGS("name:*", &name, ks_T_str);

    return (ks_obj)ks_module_import(name->chr);
}


/* Iterables */


// sum(objs, initial=none) - compute the sum of objs
static KS_FUNC(sum) {
    ks_obj objs, initial = KSO_NONE; 
    KS_GETARGS("obj ?initial", &objs, &initial);

    // result, if `none` was given, use the first element of the iterator
    // (or 0)
    ks_obj res = initial == KSO_NONE ? NULL : KS_NEWREF(initial);

    // iterate through the entire iterable
    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob = NULL;
    while (ob = ks_citer_next(&cit)) {
        if (!res) {
            // if !res, that implies that `initial==none` and this is
            //   the first run; so just use the first as the initial
            //   value
            res = ob;
        } else {
            // essentially, lfold reduce on the `+` operator
            ks_obj new_res = ks_F_add->func(2, (ks_obj[]){ res, ob });
            if (!new_res) cit.threwErr = true;

            // swap out variables
            KS_DECREF(res);
            KS_DECREF(ob);
            res = new_res;
        }
    }

    ks_citer_done(&cit);

    // if there was an error, exit out
    if (cit.threwErr) {
        if (res) KS_DECREF(res);
        return NULL;
    }

    // otherwise, return the result
    return res ? res : (ks_obj)ks_int_new(0);
}


// map(func, objs) - apply `func` to `objs`
// TODO: should this be an async object, similar to python, or always produce a list?
static KS_FUNC(map) {
    ks_obj func, objs;
    KS_GETARGS("func objs", &func, &objs);

    // create result list
    ks_list res = ks_list_new(0, NULL);

    // iterate through the entire iterable
    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob = NULL;
    while (ob = ks_citer_next(&cit)) {
        // apply the function to this object
        ks_obj new_ob = ks_obj_call(func, 1, &ob);
        KS_DECREF(ob);
        // handle an error
        if (!new_ob) {
            cit.threwErr = true;
            break;
        }

        // add it to the result
        ks_list_push(res, new_ob);
        KS_DECREF(new_ob);
    }

    ks_citer_done(&cit);

    // if there was an error, exit out
    if (cit.threwErr) {
        KS_DECREF(res);
        return NULL;
    }

    // otherwise, return the result
    return (ks_obj)res;
}


// filter(func, objs) - filter `objs` based on `func`, i.e. if `func(obj)` is truthy,
//   add `obj` to the output
// TODO: should this be an async object, similar to python, or always produce a list?
static KS_FUNC(filter) {
    ks_obj func, objs;
    KS_GETARGS("func objs", &func, &objs);

    // create result list
    ks_list res = ks_list_new(0, NULL);

    // iterate through the entire iterable
    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob = NULL;
    while (ob = ks_citer_next(&cit)) {
        // apply the function to this object
        ks_obj new_ob = ks_obj_call(func, 1, &ob);
        // handle an error
        if (!new_ob) {
            cit.threwErr = true;
            KS_DECREF(ob);
            break;
        }

        // determine truthiness
        int tru = ks_obj_truthy(new_ob);
        KS_DECREF(new_ob);

        // error in determining truthiness
        if (tru < 0) {
            cit.threwErr = true;
            KS_DECREF(ob);
            break;
        }

        // add original object
        if (tru) ks_list_push(res, ob);
        KS_DECREF(ob);
    }

    ks_citer_done(&cit);

    // if there was an error, exit out
    if (cit.threwErr) {
        KS_DECREF(res);
        return NULL;
    }

    // otherwise, return the result
    return (ks_obj)res;
}



// issub(a, b) - return whether `a` is a sub type of `b`
static KS_FUNC(issub) {
    ks_type a, b;
    KS_GETARGS("a:* b:*", &a, ks_T_type, &b, ks_T_type);
    return KSO_BOOL(ks_type_issub(a, b));
}


// any(objs) - compute whether any of objs are true
static KS_FUNC(any) {
    ks_obj objs;
    KS_GETARGS("obj", &objs);

    bool hadAny = false;

    // iterate through the entire iterable
    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob = NULL;
    while (ob = ks_citer_next(&cit)) {
        // convert to truthiness
        int tru = ks_obj_truthy(ob);
        KS_DECREF(ob);

        if (tru < 0) {
            // there was an error
            cit.threwErr = true;
            break;
        } else if (tru) {
            // there was a truthy value
            hadAny = true;
            break;
        }
    }

    ks_citer_done(&cit);
    return cit.threwErr ? NULL : KSO_BOOL(hadAny);
}



// all(objs) - compute whether all of objs are true
static KS_FUNC(all) {
    ks_obj objs;
    KS_GETARGS("obj", &objs);

    bool hadAll = true;

    // iterate through the entire iterable
    struct ks_citer cit = ks_citer_make(objs);
    ks_obj ob = NULL;
    while (ob = ks_citer_next(&cit)) {
        int tru = ks_obj_truthy(ob);
        KS_DECREF(ob);
        if (tru < 0) {
            // there was an error
            cit.threwErr = true;
            break;
        } else if (!tru) {
            // had one non-truthy
            cit.done = true;
            hadAll = false;
            break;
        }
    }

    ks_citer_done(&cit);
    return cit.threwErr ? NULL : KSO_BOOL(hadAll);
}




/* Operators */


// template for defining a binary operator function
#define T_KS_FUNC_BOP(_name, _str, _fname, _spec)             \
static KS_FUNC(_name) {                                       \
    ks_obj L, R;                                              \
    KS_GETARGS("L R", &L, &R)                                 \
    { _spec; }                                                \
    if (L->type->_fname != NULL) {                            \
        ks_obj ret = NULL;                                    \
        ret = ks_obj_call(L->type->_fname,                    \
            2, (ks_obj[]){ L, R });                           \
        if (ret != NULL) return ret;                          \
        ks_thread cth = ks_thread_get();                      \
        if (cth->exc && cth->exc->type == ks_T_OpError)       \
            { ks_catch_ignore(); }                            \
        else return NULL;                                     \
    }                                                         \
    if (R->type->_fname != NULL) {                            \
        return ks_obj_call(R->type->_fname,                   \
            2, (ks_obj[]){ L, R });                           \
    }                                                         \
    KS_THROW_BOP_ERR(_str, L, R);                             \
}

T_KS_FUNC_BOP(add, "+", __add__, {})
T_KS_FUNC_BOP(sub, "-", __sub__, {})
T_KS_FUNC_BOP(mul, "*", __mul__, {})
T_KS_FUNC_BOP(div, "/", __div__, {})
T_KS_FUNC_BOP(mod, "%", __mod__, {})
T_KS_FUNC_BOP(pow, "**", __pow__, {  })

T_KS_FUNC_BOP(binand, "&", __binand__, {})
T_KS_FUNC_BOP(binor, "|", __binor__, {})
T_KS_FUNC_BOP(binxor, "^", __binxor__, {})

T_KS_FUNC_BOP(lshift, "<<", __lshift__, { })
T_KS_FUNC_BOP(rshift, ">>", __rshift__, {})

T_KS_FUNC_BOP(cmp, "<=>", __cmp__, {})

T_KS_FUNC_BOP(lt, "<", __lt__, {})
T_KS_FUNC_BOP(gt, ">", __gt__, {})
T_KS_FUNC_BOP(le, "<=", __le__, {})
T_KS_FUNC_BOP(ge, ">=", __ge__, {})
T_KS_FUNC_BOP(eq, "==", __eq__, { if (L == R && (L->type->flags & KS_TYPE_FLAGS_EQSS)) return KSO_TRUE; })
T_KS_FUNC_BOP(ne, "!=", __ne__, { if (L == R && (L->type->flags & KS_TYPE_FLAGS_EQSS)) return KSO_FALSE; })


// template for defining a unary operator function
#define T_KS_FUNC_UOP(_name, _str, _fname)                  \
static KS_FUNC(_name) {                                     \
    ks_obj V;                                               \
    KS_GETARGS("V", &V)                                     \
    if (V->type->_fname != NULL)                            \
        return ks_obj_call(V->type->_fname, 1, &V);         \
    KS_THROW_UOP_ERR(_str, V); return NULL;                 \
}

T_KS_FUNC_UOP(pos, "+", __pos__)
T_KS_FUNC_UOP(neg, "-", __neg__)
T_KS_FUNC_UOP(sqig, "~", __sqig__)

// abs(V) - return absolute value of an object
static KS_FUNC(abs) {
    ks_obj V;
    KS_GETARGS("V", &V)

    if (V->type->__abs__ != NULL) return ks_obj_call(V->type->__abs__, 1, &V); 

    KS_THROW_METH_ERR(V, "__abs__");

}

// interpreter variables
ks_dict ks_inter_vars = NULL;

/* Readline-specific methods */

#ifdef KS_HAVE_READLINE


// attempt to tab complete a given match via readline
// The programming of this section is a bit awkward; but essentially, we iterate through
//   dictionaries & attributes and suggest them
// NOTE: see here for more information: http://web.mit.edu/gnu/doc/html/rlman_2.html#SEC36
static char* match_gen(const char* text, int state) {

    // which dictionaries may we start matching from?
    static ks_dict match_roots[2];

    // match index, and internal index
    static int match_i, i;

    // string length
    static int slen = 0;

    // text.split('.'); elements of that
    static ks_list text_split = NULL;

    // the root dictionary for this match
    static ks_dict match_root_dict = NULL;

    // prefix of the output
    static ks_str text_prefix = NULL;

    // extra bytes for additional characters for completion
    #define RLM_BUF 16

    if (!state) {
        // initialize for the first time
        match_roots[0] = ks_inter_vars;
        match_roots[1] = ks_globals;

        // return length of string
        slen = strlen(text);
        
        text_split = ks_list_new(0, NULL);

        int j, lj = 0;
        for (j = 0; j < slen; ++j) {
            if (text[j] == '.') {
                // add substring to the text_split var
                ks_str tmp = ks_str_utf8(text + lj, j - lj);

                ks_list_push(text_split, (ks_obj)tmp);

                KS_DECREF(tmp);
                lj = j + 1;
            }
        }


        text_prefix = ks_str_utf8(text, lj);

        ks_str tmp = ks_str_utf8(text + lj, j - lj);
        ks_list_push(text_split, (ks_obj)tmp);
        KS_DECREF(tmp);

        //ks_printf("split: %S", text_split);

        // set at beginning
        match_i = 0;
        i = 0;

        match_root_dict = NULL;


    }


    // don't append a space
    rl_completion_append_character = '\0';

    // while in a valid match root
    while (match_i < sizeof(match_roots) / sizeof(*match_roots)) {

        if (match_root_dict == NULL) {
            // find the next to last
            // the match dictionary
            ks_dict this_dict = match_roots[match_i];

            // traverse down to the end dictionary
            int dep = 0;

            // go up until the last one, which is where partial matches will be found
            while (dep < text_split->len - 1) {

                // attempt to go down one
                ks_obj this_val = ks_dict_get(this_dict, text_split->elems[dep]);
                if (!this_val) {
                    ks_catch_ignore();
                    break;
                }

                if (this_val->type == ks_T_module) {
                    this_dict = ((ks_module)this_val)->attr;
                } else if (this_val->type == ks_T_namespace) {
                    this_dict = ((ks_namespace)this_val)->attr;
                } else if (this_val->type == ks_T_type) {
                    this_dict = ((ks_type)this_val)->attr;
                } else {
                    // use type attributes
                    this_dict = this_val->type->attr;
                }

                KS_DECREF(this_val);

                // go down another dict
                dep++;
            }

            if (dep != text_split->len - 1) {
                // skip ahead, didn't match all the way to the end
                match_i++;
                match_root_dict = NULL;
                i = 0;
                continue;
            } else {
                // set to the top most dictionary
                match_root_dict = this_dict;
                i = 0;
            }
        }


        while (i < match_root_dict->n_entries) {

            // current key
            ks_str this_key = (ks_str)match_root_dict->entries[i].key;
            ks_obj this_val = match_root_dict->entries[i].val;

            // consume this entry
            i++;

            if (this_key && this_key->type == ks_T_str) {

                // is a string; good
                if (text_split->len == 0 || strncmp(this_key->chr, ((ks_str)text_split->elems[text_split->len - 1])->chr, ((ks_str)text_split->elems[text_split->len - 1])->len_b) == 0) {

                    // NOTE: need to use malloc(), since readline frees it
                    char* new_match = malloc(text_prefix->len_b + this_key->len_b + RLM_BUF + slen + 1);
                    //snprintf(new_match, this_key->len_b + RLM_BUF, "%*s%s", (int)this_key->len_b, this_key->chr, ks_obj_is_callable(this_val) ? "(" : "");
                    snprintf(new_match, this_key->len_b + RLM_BUF, "%*s%*s%s", (int)text_prefix->len_b, text_prefix->chr, (int)this_key->len_b, this_key->chr, ks_obj_is_callable(this_val) ? "(" : "");
                    return new_match;
                }
            }

        }


        // to next match
        match_root_dict = NULL;
        match_i++;
        i = 0;
    }

    KS_DECREF(text_split);
    KS_DECREF(text_prefix);

    // no match found
    return NULL;
}

// full completion function to return a list of matches
static char** match_completion(const char *text, int start, int end) {
    rl_attempted_completion_over = 1;
    return rl_completion_matches(text, match_gen);
}

// kscript's 'tab' functionality, to complete from a menu
int my_tab_func (int count, int key) {
    //printf("COUNT: %i\n", count);
    rl_menu_complete(count, key);
    return count + 1;
}

// Handle a SIGINT; this causes Ctrl+C to print a warning message
static void handle_sigint(int signum) {
    // do nothing
    #define MSG_SIGINT_RESET "\nUse 'CTRL-D' or 'exit()' to quit the process\n"
    fwrite(MSG_SIGINT_RESET, 1, sizeof(MSG_SIGINT_RESET) - 1, stdout);
    rl_on_new_line(); // Regenerate the prompt on a newline
    rl_replace_line("", 0); // Clear the previous text
    rl_redisplay();

    // set it for next time
    signal(SIGINT, handle_sigint);
}

// ensure readline is enabled & initializaed
static void ensure_readline() {
    static bool isInit = false;
    if (isInit) return;

    // ensured

    // TODO: initialize it
    // set the tab completion to our own
    rl_attempted_completion_function = match_completion;

    // disable their signal handling
    rl_catch_signals = 0;
    rl_clear_signals();
   
    signal(SIGINT, handle_sigint);

    //rl_bind_key('\t', my_tab_func);

    isInit = true;
}

#endif /* KS_HAVE_READLINE */



/* Builtin Execution functions*/


// handle exception
static void interactive_handle_exc() {
    // handle error
    ks_list exc_info = NULL;
    ks_obj exc = ks_catch(&exc_info);
    assert(exc != NULL && "NULL returned with no exception!");

    // error, so rethrow it and return
    ks_printf(COL_RED COL_BOLD "%T" COL_RESET ": %S\n", exc, exc);

    // print in reverse order
    ks_printf("Call Stack:\n");
    
    int i;
    for (i = 0; i < exc_info->len; i++) {
        ks_printf("%*c#%i: In %S\n", 2, ' ', i, exc_info->elems[i]);
    }
    
    ks_printf("In thread %R @ %p\n", ks_thread_get()->name, ks_thread_get());

    KS_DECREF(exc);
    KS_DECREF(exc_info);
}

// runs a single expression from the interactive 
static void run_interactive_expr(ks_str expr, ks_str src_name) {

    ks_str tmp = ks_str_new("<anon expr>");
    // 1. construct a parser
    ks_parser p = ks_parser_new(expr, src_name, tmp);
    KS_DECREF(tmp);
    if (!p) {
        interactive_handle_exc();
        return;
    }

    // 2. parse out the entire module (which will also syntax validate)
    ks_ast prog = ks_parser_file(p);
    if (!prog) {
        KS_DECREF(p);
        interactive_handle_exc();
        return;
    }

    // whether ot not to print the result
    bool doPrint = false;
    // if this is true, we should only print if nothing else has printed in that time,
    //   to avoid double printing
    bool doPrintIffy = false;
    int64_t start_nbytes = _print_numbytes;

    // get whether its an actual terminal screen
    bool isTTY = isatty(STDIN_FILENO);

    // now, check if it is a binary operator or a special case,
    //   in which case 
    if (isTTY) if ((prog->kind >= KS_AST_BOP__FIRST && prog->kind <= KS_AST_BOP__LAST) ||
        (prog->kind >= KS_AST_UOP__FIRST && prog->kind <= KS_AST_UOP__LAST) ||
        prog->kind == KS_AST_ATTR || prog->kind == KS_AST_VAR || prog->kind == KS_AST_CONST ||
        prog->kind == KS_AST_SUBSCRIPT || prog->kind == KS_AST_CALL || prog->kind == KS_AST_LIST || prog->kind == KS_AST_TUPLE || prog->kind == KS_AST_DICT) {
            doPrint = true;
            doPrintIffy = prog->kind == KS_AST_CALL;
            ks_ast new_prog = ks_ast_new_ret(prog);
            KS_DECREF(prog);
            prog = new_prog;
        }

    // 3. generate a bytecode object reprsenting the module
    ks_code myc = ks_compile(p, prog);
    if (!myc) {
        KS_DECREF(p);    
        interactive_handle_exc();
        return;
    }

    // 4. (optional) attempt to set some metadata for the code, if asked
    if (myc->meta_n > 0 && myc->parser != NULL) {
        if (myc->name_hr) KS_DECREF(myc->name_hr);
        myc->name_hr = ks_fmt_c("%S", myc->parser->src_name);
    }

    // debug the code it is going to run
    ks_debug("ks", "CODE: %S", myc);

    // now, call the code object with no arguments, and return the result
    // If there is an error, it will return NULL, and the thread will call ks_errend(),
    //   which will print out a stack trace and terminate the program for us
    ks_obj ret = ks_obj_call2((ks_obj)myc, 0, NULL, ks_inter_vars);
    if (!ret) {
        interactive_handle_exc();

    } else {
        if (doPrint) {

            if (doPrintIffy && _print_numbytes != start_nbytes) {
                // do nothing
            } else {
                ks_printf("%S\n", ret);
                ks_catch_ignore();
            }
        }
        // discard error
        KS_DECREF(ret);
    }

    KS_DECREF(prog);
    KS_DECREF(myc);

}


// define the prompt
#ifndef KS_PROMPT
#define KS_PROMPT " %> "
#endif


// exec_interactive() execute a single interactive shell
static KS_FUNC(exec_interactive) {
    KS_GETARGS("")

    size_t alloc_size = 0;
    int len = 0;
    char* cur_line = NULL;

    // how many lines have been processed
    int num_lines = 0;

    #ifndef KS_HAVE_READLINE
    ks_warn("ks", "Using interactive interpreter, but not compiled with readline!");
    #endif

    // get whether its an actual terminal screen
    bool isTTY = isatty(STDIN_FILENO);

    // only use readline if it is a TTY and we are compiled with readline support
    #ifdef KS_HAVE_READLINE
    if (isTTY) {
        ensure_readline();

        // yield GIL
        ks_GIL_unlock();

        // now, continue to read lines
        while ((cur_line = readline(KS_PROMPT)) != NULL) {

            ks_GIL_lock();

            num_lines++;
            // only add non-empty
            if (cur_line && *cur_line) add_history(cur_line);
            else continue;

            // now, compile it
            ks_str src_name = ks_fmt_c("<prompt #%i>", (int)num_lines);
            ks_str expr = ks_str_new(cur_line);

            run_interactive_expr(expr, src_name);
            KS_DECREF(expr);
            KS_DECREF(src_name);

            // free it. readline uses 'malloc', so we must use free
            free(cur_line);

            ks_GIL_unlock();
        }
        
        ks_GIL_lock();

    } else {
    // do fallback version
    
    #endif

    // yield GIL
    //ks_GIL_unlock();

    // do readline version
    if (isTTY) printf("%s", KS_PROMPT);

    while ((len = ks_getline(&cur_line, &alloc_size, stdin)) >= 0) {

        // hold GIL
        //ks_GIL_lock();

        num_lines++;

        // remove last newline
        if (cur_line[len] == '\n') cur_line[len] = '\0';

        // short cut out if there is nothing
        if (len == 0) {
            printf("%s", KS_PROMPT);
            continue;
        }

        // now, compile it
        ks_str src_name = ks_fmt_c("<interactive prompt #%i>", (int)num_lines);
        ks_str expr = ks_str_new(cur_line);

        run_interactive_expr(expr, src_name);
        KS_DECREF(expr);
        KS_DECREF(src_name);

        if (isTTY) printf("%s", KS_PROMPT);

       // ks_GIL_unlock();

    }

    //ks_GIL_lock();

    // end of the 'else' clause
    #ifdef KS_HAVE_READLINE
    }
    #endif

    return KSO_NONE;
}


// eval(expr) - evaluate `expr`
static KS_FUNC(eval) {
    ks_str expr;
    KS_GETARGS("expr:*", &expr, ks_T_str)

    ks_str fname = ks_str_new("<anon expr>");

    // 2. Parse it
    ks_parser parser = ks_parser_new(expr, fname, fname);
    KS_DECREF(fname);
    if (!parser) {
        return NULL;
    }

    //ks_debug("ks", "got parser: %O", parser);

    // 3. Parse out the entire file into an AST (which will do syntax validation as well)

    ks_ast prog = ks_parser_expr(parser, KS_PARSE_NONE);
    if (!prog) {
        KS_DECREF(parser);
        return NULL;
    }

    if (parser->toki != parser->tok_n - 1) {
        ks_throw(ks_T_Error, "Expression is too long for eval() (only expressions; not full files are supported)");
        KS_DECREF(parser);
        KS_DECREF(prog);
        return NULL;
    }

    ks_ast new_prog = ks_ast_new_ret(prog);
    KS_DECREF(prog);
    prog = new_prog;


    // 4. Compile to bytecode
    ks_code bcode = ks_compile(parser, prog);
    if (!bcode) {
        KS_DECREF(prog);
        KS_DECREF(parser);
        return NULL;
    }


    ks_debug("ks", "compiled to: '%S'", bcode);


    // current thread
    ks_thread th = ks_thread_get();

    // local variables
    ks_dict locals = NULL;

    // attempt to hoist from just under the top of the stack frames
    if (th->frames->len > 2) locals = ((ks_stack_frame)th->frames->elems[th->frames->len - 2])->locals;



    ks_obj result = ks_obj_call2((ks_obj)bcode, 0, NULL, locals);

    KS_DECREF(parser);

    return result;

}

// exec_expr(expr) - run and execute the given expression
static KS_FUNC(exec_expr) {
    ks_str expr;
    KS_GETARGS("expr:*", &expr, ks_T_str)

    // get a source name for it
    ks_str src_name = ks_fmt_c("-e %R", expr);
    run_interactive_expr(expr, src_name);
    KS_DECREF(src_name);
    return KSO_NONE;
}


// exec_file(fname) - run and execute the given file
static KS_FUNC(exec_file) {
    ks_str fname;
    KS_GETARGS("fname:*", &fname, ks_T_str)

    // 1. Read the entire file
    ks_str src_code = ks_readfile(fname->chr, "r");
    if (!src_code) return NULL;

    ks_debug("ks", "read file '%S', got: " COL_DIM "'%S'" COL_RESET "", fname, src_code);

    // 2. Parse it
    ks_parser parser = ks_parser_new(src_code, fname, fname);
    if (!parser) {
        KS_DECREF(src_code);
        return NULL;
    }

    //ks_debug("ks", "got parser: %O", parser);

    // 3. Parse out the entire file into an AST (which will do syntax validation as well)

    ks_ast expr = ks_parser_file(parser);
    if (!expr) {
        KS_DECREF(src_code);
        KS_DECREF(parser);
        return NULL;
    }

    // 4. Compile to bytecode
    ks_code bcode = ks_compile(parser, expr);
    if (!bcode) {
        KS_DECREF(expr);
        KS_DECREF(src_code);
        KS_DECREF(parser);
        return NULL;
    }

    ks_debug("ks", "compiled to: '%S'", bcode);


    ks_obj result = ks_obj_call((ks_obj)bcode, 0, NULL);

    KS_DECREF(src_code);
    KS_DECREF(parser);


    if (result) KS_DECREF(result)
    else return NULL;

    return KSO_NONE;
}




// export

void ks_init_funcs() {
    // interpreter variables
    ks_inter_vars = ks_dict_new(0, NULL);

    ks_F_print = ks_cfunc_new_c(print_, "print(*args)");
    ks_F_recurse = ks_cfunc_new_c(recurse_, "__recurse__(*args)");
    ks_F_import = ks_cfunc_new_c(import_, "__import__(modname)");

    ks_F_iter = ks_cfunc_new_c(iter_, "iter(obj)");
    ks_F_next = ks_cfunc_new_c(next_, "next(obj)");

    ks_F_truthy = ks_cfunc_new_c(truthy_, "truthy(obj)");

    ks_F_typeof = ks_cfunc_new_c(typeof_, "typeof(obj)");
    ks_F_hash = ks_cfunc_new_c(hash_, "hash(obj)");
    ks_F_id = ks_cfunc_new_c(id_, "id(obj)");
    ks_F_len = ks_cfunc_new_c(len_, "len(obj)");
    ks_F_repr = ks_cfunc_new_c(repr_, "repr(obj)");

    ks_F_chr = ks_cfunc_new_c(chr_, "chr(ord)");
    ks_F_ord = ks_cfunc_new_c(ord_, "ord(chr)");

    ks_F_issub = ks_cfunc_new_c(issub_, "issub(a, b)");
    
    ks_F_any = ks_cfunc_new_c(any_, "any(objs)");
    ks_F_all = ks_cfunc_new_c(all_, "all(objs)");


    ks_F_sum = ks_cfunc_new_c(sum_, "sum(objs, initial=none)");
    ks_F_map = ks_cfunc_new_c(map_, "map(func, objs)");
    ks_F_filter = ks_cfunc_new_c(filter_, "filter(func, objs)");

    ks_F_getattr = ks_cfunc_new_c(getattr_, "getattr(obj, key)");
    ks_F_setattr = ks_cfunc_new_c(setattr_, "setattr(obj, key, val)");
    ks_F_getitem = ks_cfunc_new_c(getitem_, "getitem(obj, *args)");
    ks_F_setitem = ks_cfunc_new_c(setitem_, "setitem(obj, *args)");

    ks_F_add = ks_cfunc_new_c(add_, "__add__(L, R)");
    ks_F_sub = ks_cfunc_new_c(sub_, "__sub__(L, R)");
    ks_F_mul = ks_cfunc_new_c(mul_, "__mul__(L, R)");
    ks_F_div = ks_cfunc_new_c(div_, "__div__(L, R)");
    ks_F_mod = ks_cfunc_new_c(mod_, "__mod__(L, R)");
    ks_F_pow = ks_cfunc_new_c(pow_, "__pow__(L, R)");

    ks_F_binand = ks_cfunc_new_c(binand_, "__binand__(L, R)");
    ks_F_binor = ks_cfunc_new_c(binor_, "__binor__(L, R)");
    ks_F_binxor = ks_cfunc_new_c(binxor_, "__binxor__(L, R)");

    ks_F_lshift = ks_cfunc_new_c(lshift_, "__lshift__(L, R)");
    ks_F_rshift = ks_cfunc_new_c(rshift_, "__rshift__(L, R)");

    ks_F_cmp = ks_cfunc_new_c(cmp_, "__cmp__(L, R)");

    ks_F_lt = ks_cfunc_new_c(lt_, "__lt__(L, R)");
    ks_F_gt = ks_cfunc_new_c(gt_, "__gt__(L, R)");
    ks_F_le = ks_cfunc_new_c(le_, "__le__(L, R)");
    ks_F_ge = ks_cfunc_new_c(ge_, "__ge__(L, R)");
    ks_F_eq = ks_cfunc_new_c(eq_, "__eq__(L, R)");
    ks_F_ne = ks_cfunc_new_c(ne_, "__ne__(L, R)");

    ks_F_pos = ks_cfunc_new_c(pos_, "__pos__(V)");
    ks_F_neg = ks_cfunc_new_c(neg_, "__neg__(V)");
    ks_F_abs = ks_cfunc_new_c(abs_, "abs(V)");
    ks_F_sqig = ks_cfunc_new_c(sqig_, "__sqig__(V)");

    ks_F_eval = ks_cfunc_new_c(eval_, "eval(expr)");

    ks_F_exec_interactive = ks_cfunc_new_c(exec_interactive_, "exec_interactive(fname)");
    ks_F_exec_expr = ks_cfunc_new_c(exec_expr_, "exec_expr(expr)");
    ks_F_exec_file = ks_cfunc_new_c(exec_file_, "exec_file(fname)");


}

