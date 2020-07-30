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

    ks_F_truthy = NULL,

    ks_F_repr = NULL,
    ks_F_hash = NULL,
    ks_F_len = NULL,
    ks_F_typeof = NULL,

    ks_F_import = NULL,

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

    // print out to console
    printf("%s\n", toprint->chr);
    KS_DECREF(toprint);

    return KSO_NONE;
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
                ks_memberfunc ret = ks_memberfunc_new(type_attr, obj);
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
    KS_GETARGS("V", &V)                                     \
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
    KS_GETARGS("fname:*", &fname, ks_T_str)

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
    KS_GETARGS("expr:*", &expr, ks_T_str)

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

    ks_F_truthy = ks_cfunc_new_c(truthy_, "truthy(obj)");

    ks_F_typeof = ks_cfunc_new_c(typeof_, "typeof(obj)");
    ks_F_len = ks_cfunc_new_c(len_, "len(obj)");
    ks_F_repr = ks_cfunc_new_c(repr_, "repr(obj)");

    ks_F_chr = ks_cfunc_new_c(chr_, "chr(ord)");
    ks_F_ord = ks_cfunc_new_c(ord_, "ord(chr)");

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

