/* module.c - implementation of the kscript module system
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// cache of modules
static ks_dict mod_cache = NULL;


// create a kscript function
ks_module ks_module_new(const char* mname, const char* doc) {
    ks_module self = KS_ALLOC_OBJ(ks_module);
    KS_INIT_OBJ(self, ks_T_module);

    // initialize type-specific things
    self->attr = ks_dict_new_c(KS_KEYVALS(
        {"__name__",               (ks_obj)ks_str_new(mname)},
        {"__doc__",                (ks_obj)ks_str_new(doc)},
    ));

    self->parent = NULL;
    self->children = ks_list_new(0, NULL);

    return self;
}

// set a module's parent
void ks_module_parent(ks_module self, ks_module par) {
    if (par != NULL) KS_INCREF(par);
    if (self->parent != NULL) KS_DECREF(self->parent);
    self->parent = par;
}

// attempt to load a single file, without any extra paths
// Do not raise error, just return NULL if not successful
static ks_module attempt_load(ks_str mod_key, const char* mname, char* cname) {

    char* ext = strrchr(cname, '.');

    if (ext != NULL && strcmp(&ext[1], KS_SHARED_END) == 0) {
        // load the library as a C API

        // attempt to load linux shared library
        // now, load it via dlopen

        void* handle = dlopen(cname, RTLD_LAZY | RTLD_GLOBAL);    
        if (handle == NULL) {
            char* dlmsg = dlerror();
            ks_debug("ks", "[import] file '%s' failed: dlerror(): %s", cname, dlmsg);

            if (dlmsg) {
                int sl = strlen(dlmsg);
                static const char spec_msg[] = "undefined symbol";
                int i;
                for (i = 0; i < sl - sizeof(spec_msg); ++i) {
                    if (strncmp(dlmsg + i, spec_msg, sizeof(spec_msg) - 1) == 0) {
                        ks_throw(ks_T_ImportError, "Failed to import module '%s': %*s (while loading '%s')", mname, sl - i, dlmsg + i, cname);
                        break;
                    }
                }
            }

            return NULL;
        }

        //ks_printf("MODKEY: %S, %p\n", mod_key, handle);

        struct ks_module_cinit* mod_cinit = (struct ks_module_cinit*)dlsym(handle, "__ks_module_cinit__");
        if (mod_cinit != NULL) {

            // call the function, and return its result
            ks_module mod = mod_cinit->load_func();
            if (!mod) {
                ks_debug("ks", "[import] file '%s' failed: Exception was thrown by '__ks_module_cinit__->load_func()'", cname);
                return NULL;
            }


            ks_dict_set_h(mod_cache, (ks_obj)mod_key, mod_key->v_hash, (ks_obj)mod);
            ks_debug("ks", "[import] file '%s' succeeded!", cname);

            return mod;
        } else {
            ks_debug("ks", "[import] file '%s' failed: No '__ks_module_cinit__' symbol!", cname);
            return NULL;
        }

    } else if (ext != NULL && strcmp(&ext[1], "ks") == 0) {
        // attempt to load a file

        // attempt to open a file
        FILE* fp = fopen(cname, "r");
        if (fp == NULL) {
            ks_debug("ks", "[import] file '%s' failed: %s", cname, strerror(errno));
            return NULL;
        }

        fclose(fp);

        // now, extract the name by finding the last slash (or, start of the string if there
        //   was none)
        char* slash = cname + strlen(cname) - 1;

        while (slash > cname && *slash != '/' && *slash != '\\') {
            slash--;
        }

        char* name_start = slash == cname ? slash : slash + 1;

        // get the name as a C-string
        ks_str name_str = ks_str_utf8(name_start, ext - name_start);
        ks_str doc_str = ks_str_utf8("", 0);


        // construct module
        ks_module mod = ks_module_new(name_str->chr, doc_str->chr);

        KS_DECREF(name_str);
        KS_DECREF(doc_str);

        // now, we need to execute the file by compiling it and using `mod`'s attribute as the local dictionary

        // 1. Read the entire file
        ks_str src_code = ks_readfile(cname, "r");
        if (!src_code) return NULL;
        ks_debug("ks", "[import] read file '%s', got: " COL_DIM "'%S'" COL_RESET "", cname, src_code);

        ks_str cname_obj = ks_str_new(cname);
        // 2. Parse it
        ks_parser parser = ks_parser_new(src_code, cname_obj, cname_obj);
        KS_DECREF(cname_obj);
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

        ks_debug("ks", "[import] compiled to: '%S'", bcode);

        ks_obj result = ks_obj_call2((ks_obj)bcode, 0, NULL, mod->attr);

        KS_DECREF(src_code);
        KS_DECREF(parser);

        if (result) { 
            KS_DECREF(result);
            ks_dict_set_h(mod_cache, (ks_obj)mod_key, mod_key->v_hash, (ks_obj)mod);
            return mod;
        } else {
            KS_DECREF(mod);
            return NULL;
        }
    }

    // not found
    return NULL;
}


// attempt a module with a given name
ks_module ks_module_import(const char* mname) {
    //__C_module_init__
    ks_module mod = NULL;

    ks_str mod_key = ks_str_new(mname);
    ks_debug("ks", "[import] trying to import '%s'...", mname);
    
    // check the cache for quick return
    mod = (ks_module)ks_dict_get_h(mod_cache, (ks_obj)mod_key, mod_key->v_hash);
    if (mod != NULL) {
        KS_DECREF(mod_key);
        return mod;
    }

    int i;
    for (i = 0; i < ks_paths->len; ++i) {
        // current path we are trying
        ks_str ctry = ks_fmt_c("%S/%s/libksm_%s.%s", ks_paths->elems[i], mname, mname, KS_SHARED_END);
        mod = attempt_load(mod_key, mname, ctry->chr);
        KS_DECREF(ctry);

        if (mod != NULL) goto finish;
        if (ks_thread_get()->exc) goto finish;

        ctry = ks_fmt_c("%S/%s.ks", ks_paths->elems[i], mname);

        mod = attempt_load(mod_key, mname, ctry->chr);
        KS_DECREF(ctry);

        if (mod != NULL) goto finish;
        if (ks_thread_get()->exc) goto finish;

    }

    finish:;
    KS_DECREF(mod_key);

    if (mod == NULL) {
        // not found, throw error
        if (ks_thread_get()->exc) return NULL;
        else return (ks_module)ks_throw(ks_T_ImportError, "Failed to import module '%s': No such module!", mname);
    } else {
        // add it to the dictionary, and return
        return mod;
    }

}


// module.__free__(self) - free obj
static KS_TFUNC(module, free) {
    ks_module self;
    KS_GETARGS("self:*", &self, ks_T_module)

    KS_DECREF(self->attr);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// module.__str__(self) - to string
static KS_TFUNC(module, str) {
    ks_module self;
    KS_GETARGS("self:*", &self, ks_T_module)
    
    ks_str name = (ks_str)ks_dict_get_c(self->attr, "__name__");
    ks_str ret = ks_fmt_c("<'%T' : %S>", self, name);
    KS_DECREF(name);

    return (ks_obj)ret;
}

// module.__getattr__(self, attr) -> get attribute
static KS_TFUNC(module, getattr) {
    ks_module self;
    ks_str attr;
    KS_GETARGS("self:* attr:*", &self, ks_T_module, &attr, ks_T_str)

    // special case
    if (*attr->chr == '_' && strncmp(attr->chr, "__dict__", 8) == 0) {
        return KS_NEWREF(self->attr);
    }

    ks_obj ret = ks_dict_get_h(self->attr, (ks_obj)attr, attr->v_hash);

    if (!ret) {
        KS_THROW_ATTR_ERR(self, attr);
    }

    return ret;
}

// module.__setattr__(self, attr, val) -> set attribute
static KS_TFUNC(module, setattr) {
    ks_module self;
    ks_str attr;
    ks_obj val;
    KS_GETARGS("self:* attr:* val", &self, ks_T_module, &attr, ks_T_str, &val)

    ks_dict_set_h(self->attr, (ks_obj)attr, attr->v_hash, val);

    return KS_NEWREF(val);
}




/* export */

KS_TYPE_DECLFWD(ks_T_module);

void ks_init_T_module() {
    ks_type_init_c(ks_T_module, "module", ks_T_object, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(module_free_, "module.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c_old(module_str_, "module.__str__(self)")},
        {"__getattr__",            (ks_obj)ks_cfunc_new_c_old(module_getattr_, "module.__getattr__(self, attr)")},
        {"__setattr__",            (ks_obj)ks_cfunc_new_c_old(module_setattr_, "module.__setattr__(self, attr, val)")},
    ));

    mod_cache = ks_dict_new(0, NULL);
}
