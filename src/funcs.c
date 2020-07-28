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

    ks_F_repr = NULL,
    ks_F_hash = NULL,
    ks_F_len = NULL,
    ks_F_typeof = NULL,

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

    ks_F_cmp = NULL,

    ks_F_lt = NULL,
    ks_F_gt = NULL,
    ks_F_le = NULL,
    ks_F_ge = NULL,
    ks_F_eq = NULL,
    ks_F_ne = NULL,

    ks_F_pos = NULL,
    ks_F_neg = NULL,
    ks_F_sqig = NULL,

    ks_F_exec_file = NULL,
    ks_F_exec_expr = NULL,
    ks_F_exec_interactive = NULL

;


/* Misc. Kscript Functions */

// print (*args) -> print arguments
static KS_FUNC(print) {
    int n_extra;
    ks_obj* extra;
    if (!ks_getargs(n_args, args, "*args", &n_extra, &extra)) return false;

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

    // print out to console
    printf("%s\n", toprint->chr);
    KS_DECREF(toprint);

    return KSO_NONE;
}



/* Iterators */

// iter(obj) -> turn into iterable
static KS_FUNC(iter) {
    ks_obj obj;
    if (!ks_getargs(n_args, args, "obj", &obj)) return NULL;

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
    if (!ks_getargs(n_args, args, "obj", &obj)) return NULL;

    if (obj->type->__next__ != NULL) {
        // create an iterable and return it
        return ks_obj_call(obj->type->__next__, 1, &obj);
    } else {
        KS_THROW_ITER_ERR(obj);
    }
}


// len(obj, *args) -> calculate 'length' of object
static KS_FUNC(len) {
    ks_obj obj;
    int n_extra;
    ks_obj* extra;
    if (!ks_getargs(n_args, args, "obj *args", &obj, &n_extra, &extra)) return NULL;

    if (obj->type->__len__ != NULL) {
        return ks_obj_call(obj->type->__len__, n_args, args);
    }
    
    KS_THROW_METH_ERR(obj, "__len__");
}

// repr(obj) -> calculate represnetation
static KS_FUNC(repr) {
    ks_obj obj;
    if (!ks_getargs(n_args, args, "obj", &obj)) return NULL;

    if (obj->type->__repr__ != NULL) {
        return ks_obj_call(obj->type->__repr__, 1, &obj);
    }
    
    KS_THROW_METH_ERR(obj, "__repr__");
}


// typeof(obj) -> get type of object
static KS_FUNC(typeof) {
    ks_obj obj;
    if (!ks_getargs(n_args, args, "obj", &obj)) return NULL;

    return KS_NEWREF(obj->type);
}


/* Get/Set Attr/Item */

// getattr(obj, key) -> set attribute
static KS_FUNC(getattr) {
    ks_obj obj, key;
    if (!ks_getargs(n_args, args, "obj key", &obj, &key)) return NULL;

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
                ks_memberfunc ret = ks_memberfunc_new(type_attr, obj);
                KS_DECREF(type_attr);

                if (ret != NULL) {
                    // created member function
                    return (ks_obj)ret;
                }
            }
        }
    }

    KS_THROW_METH_ERR(obj, "__getattr__");
}

// setattr(obj, key, val) -> set subscript of item
static KS_FUNC(setattr) {
    ks_obj obj, key, val;
    if (!ks_getargs(n_args, args, "obj key val", &obj, &key, &val)) return NULL;

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
    if (!ks_getargs(n_args, args, "obj *args", &obj, &n_extra, &extra)) return NULL;

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
    if (!ks_getargs(n_args, args, "obj *args", &obj, &n_extra, &extra)) return NULL;

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
    if (!ks_getargs(n_args, args, "ord:i64", &ord)) return NULL;

    ks_unich to_unich = ord;
    if (to_unich != ord) {
        return ks_throw(ks_T_ArgError, "chr() given invalid value! Expected between 0 and 11141111, but got '%z'", (ks_size_t)ord);
    }

    return (ks_obj)ks_str_chr(to_unich);
}

// ord(chr) - convert ordinal to (single character) string
static KS_FUNC(ord) {
    ks_str chr;
    if (!ks_getargs(n_args, args, "chr:*", &chr, ks_T_str)) return NULL;
    ks_unich r = ks_str_ord(chr);


    return r >= 0 ? (ks_obj)ks_int_new(r) : NULL;
}


/* Operators */


// template for defining a binary operator function
#define T_KS_FUNC_BOP(_name, _str, _fname, _spec)             \
static KS_FUNC(_name) {                                       \
    ks_obj L, R;                                              \
    if (!ks_getargs(n_args, args, "L R", &L, &R))             \
        { return NULL; }                                      \
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
T_KS_FUNC_BOP(pow, "**", __pow__, {})

T_KS_FUNC_BOP(binand, "&", __binand__, {})
T_KS_FUNC_BOP(binor, "|", __binor__, {})
T_KS_FUNC_BOP(binxor, "^", __binxor__, {})

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
    if (!ks_getargs(n_args, args, "V", &V))                 \
        { return NULL; }                                    \
    if (V->type->_fname != NULL)                            \
        return ks_obj_call(V->type->_fname, 1, &V);         \
    KS_THROW_UOP_ERR(_str, V); return NULL;                 \
}


T_KS_FUNC_UOP(pos, "+", __pos__)
T_KS_FUNC_UOP(neg, "-", __neg__)
T_KS_FUNC_UOP(sqig, "~", __sqig__)




/* Builtin Utilities */

// exec_file(fname) - run and execute the given file
static KS_FUNC(exec_file) {
    ks_str fname;
    if (!ks_getargs(n_args, args, "fname:*", &fname, ks_T_str)) return NULL;

    // 1. Read the entire file
    ks_str src_code = ks_readfile(fname->chr, "r");
    if (!src_code) return NULL;

    ks_debug("ks", "read file '%S', got: " COL_DIM "'%S'" COL_RESET "", fname, src_code);

    // 2. Parse it
    ks_parser parser = ks_parser_new(src_code, fname);
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


// exec_expr(expr) - run and execute the given expression
static KS_FUNC(exec_expr) {
    ks_str expr;
    if (!ks_getargs(n_args, args, "expr:*", &expr, ks_T_str)) return NULL;

    // get a source name for it
    ks_str src_name = ks_fmt_c("-e %R", expr);

    // Parse it
    ks_parser parser = ks_parser_new(expr, src_name);
    KS_DECREF(src_name);
    if (!parser) {
        return NULL;
    }

    //ks_debug("ks", "got parser: %O", parser);

    // 3. Parse out the entire file into an AST (which will do syntax validation as well)

    ks_ast ast = ks_parser_file(parser);
    if (!ast) {
        KS_DECREF(parser);
        return NULL;
    }

    // 4. Compile to bytecode
    ks_code bcode = ks_compile(parser, ast);
    if (!bcode) {
        KS_DECREF(ast);
        KS_DECREF(parser);
        return NULL;
    }

    ks_debug("ks", "expr '%S' compiled to: '%S'", expr, bcode);

    ks_obj result = ks_obj_call((ks_obj)bcode, 0, NULL);

    KS_DECREF(ast);
    KS_DECREF(parser);

    if (result) KS_DECREF(result)
    else return NULL;

    return KSO_NONE;
}





// export

void ks_init_funcs() {

    ks_F_print = ks_cfunc_new_c(print_, "print(*args)");

    ks_F_iter = ks_cfunc_new_c(iter_, "iter(obj)");
    ks_F_next = ks_cfunc_new_c(next_, "next(obj)");

    ks_F_typeof = ks_cfunc_new_c(typeof_, "typeof(obj)");
    ks_F_len = ks_cfunc_new_c(len_, "len(obj)");
    ks_F_repr = ks_cfunc_new_c(repr_, "repr(obj)");

    ks_F_chr = ks_cfunc_new_c(chr_, "chr(ord)");
    ks_F_ord = ks_cfunc_new_c(ord_, "ord(chr)");

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

    ks_F_cmp = ks_cfunc_new_c(cmp_, "__cmp__(L, R)");

    ks_F_lt = ks_cfunc_new_c(lt_, "__lt__(L, R)");
    ks_F_gt = ks_cfunc_new_c(gt_, "__gt__(L, R)");
    ks_F_le = ks_cfunc_new_c(le_, "__le__(L, R)");
    ks_F_ge = ks_cfunc_new_c(ge_, "__ge__(L, R)");
    ks_F_eq = ks_cfunc_new_c(eq_, "__eq__(L, R)");
    ks_F_ne = ks_cfunc_new_c(ne_, "__ne__(L, R)");

    ks_F_pos = ks_cfunc_new_c(pos_, "__pos__(V)");
    ks_F_neg = ks_cfunc_new_c(neg_, "__neg__(V)");
    ks_F_sqig = ks_cfunc_new_c(sqig_, "__sqig__(V)");

    ks_F_exec_file = ks_cfunc_new_c(exec_file_, "exec_file(fname)");
    ks_F_exec_expr = ks_cfunc_new_c(exec_expr_, "exec_expr(expr)");

}

