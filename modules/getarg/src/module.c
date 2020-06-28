/* getarg/src/module.c - the kscript argument parsing library
 *
 * Example usage:
 * 
 * import getarg
 * 
 * p = getarg.Parser("my_program_name", "v1.0.0", "This does XYZ and other stuff", ["Firstname Lastname <email@address>"])
 * 
 * p.add_arg_single("name", "Your name", ["-n", "--name"])
 * p.add_arg_multi("aliases", "A list of aliases you go by", ["-a", "--aliases"], getarg.AllowMany)
 * p.add_arg_multi("money", "How much money do you have?", ["-m, "--money", "--balance"], int, getarg.AllowOnce)
 *
 * args = p.parse()
 *
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "getarg"

// include this since this is a module.
#include "ks-module.h"




// getarg.Parser - an argument parser object
typedef struct {
    KS_OBJ_BASE

    // strings describing the program name, version, and description
    ks_str program_name, version, desc;

    // a list of author strings (ALL elements are strings), in the order they were given
    ks_list authors;

    // this dictionary mapping, containing entries:
    // target: [option_string, option_string, ...]
    // where 'target' is the name of the argument, and option_string are option strings which indiciate the value
    // for example, having an argument 'name' which can be given via '--name' or '-n' would contain:
    // 'name': ['--name', '-n']
    ks_dict arg_map;
    
    // number of argument configurations
    int n_args;

    // list of argument configurations
    struct getarg_arg {

        // what kind of argument is it?
        enum getarg_arg_type {
            // none/error
            ARG_TYPE_NONE     = 0,

            // an 'argument' that doesn't take any value, like `-v` for verbosity
            ARG_TYPE_FLAG     = 0x01,

            // an argument that takes a single value after it, like `-n 5` for a variable
            ARG_TYPE_SINGLE   = 0x02,

            // an argument that takes a number of values, like `-a 2 3` or `-a 2 3 4 5`
            ARG_TYPE_MULTI    = 0x04,

        } argtype;


        // the name of the argument, can't contain '-', or anything not valid for an identifier
        ks_str name;

        // description of what the argument does
        ks_str desc;

        // list of strings that indicate the argument
        ks_list opt_strs;

        // the default object to return if none is given
        // NOTE: this does not need to be type checked at all
        ks_obj defa;

        // this can be a type, or function, or anything that takes a single argument (what was read in)
        //   and either throws an error if there's a problem, or returns the processed argument
        ks_obj validator;

        // only used for ARG_TYPE_MULTI, the range of acceptable arguments. -1 for max means no maximum
        int na_min, na_max;

    }* args;


}* getarg_Parser;


// declare the array type as residing in this file
KS_TYPE_DECLFWD(getarg_type_Parser);


// implementation to add the argument
// returns a pointer to the structure or NULL if there was a problem
// NOTE: the pointer is not valid after another call to my_addarg, since there might have been a
//   reallocation of memory
static struct getarg_arg* my_addarg(getarg_Parser self, int argtype, ks_str name, ks_str desc, ks_list opt_strs, ks_obj validator, ks_obj defa) {

    // add a new element
    int idx = self->n_args++;
    self->args = ks_realloc(self->args, sizeof(*self->args) * self->n_args);

    // get the element of the argument
    struct getarg_arg* argp = &self->args[idx];


    // set variables
    argp->argtype = argtype;
    argp->name = (ks_str)KS_NEWREF(name);
    argp->desc = (ks_str)KS_NEWREF(desc);
    argp->opt_strs = (ks_list)KS_NEWREF(opt_strs);


    // set up validator
    argp->validator = KS_NEWREF(validator);


    // allow 'NULL' for the default
    argp->defa = defa ? KS_NEWREF(defa) : NULL;


    // default to 1 or more arguments (only used in ARG_TYPE_MULTI)
    argp->na_min = 1;
    argp->na_max = -1;

    // return pointer to structure
    return argp;

}

// Parser.__new__(program_name, version, desc, authors)
// Construct a new Parser object
static KS_TFUNC(Parser, new) {
    KS_REQ_N_ARGS(n_args, 4);
    ks_str program_name, version, desc;
    ks_obj authors;
    if (!ks_parse_params(n_args, args, "program_name%s version%s desc%s authors%iter", &program_name, &version, &desc, &authors)) return NULL;

    // convert to a list
    ks_list authors_list = ks_list_from_iterable(authors);
    if (!authors_list) return NULL;

    int i;
    // convert all author lists to strings
    for (i = 0; i < authors_list->len; ++i) {
        if (authors_list->elems[i]->type != ks_type_str) {
            // ensure it is a string
            ks_str new_str = ks_fmt_c("%S", authors_list->elems[i]);
            if (!new_str) {
                KS_DECREF(authors_list);
                return NULL;
            }

            // replace object with its 'str' in the list
            KS_DECREF(authors_list->elems[i]);
            authors_list->elems[i] = (ks_obj)new_str;
        }
    }

    // allocate a getarg_Parser
    getarg_Parser self = KS_ALLOC_OBJ(getarg_Parser);
    KS_INIT_OBJ(self, getarg_type_Parser);

    // store attributes
    self->program_name = (ks_str)KS_NEWREF(program_name);
    self->version = (ks_str)KS_NEWREF(version);
    self->desc = (ks_str)KS_NEWREF(desc);

    // reuse the newly created reference
    self->authors = authors_list;

    // no arguments yet
    self->n_args = 0;
    self->args = NULL;


    // return the parser object
    return (ks_obj)self;
}


// Parser.__free__(self)
// free the parser object
static KS_TFUNC(Parser, free) {
    KS_REQ_N_ARGS(n_args, 1);
    getarg_Parser self;
    if (!ks_parse_params(n_args, args, "self%*", &self, getarg_type_Parser)) return NULL;

    // decref all attributes
    KS_DECREF(self->authors);
    KS_DECREF(self->desc);
    KS_DECREF(self->version);
    KS_DECREF(self->program_name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// Parser.add_arg_single(self, name, desc, opt_strs, type=str, default=none, flags=none)
static KS_TFUNC(Parser, add_arg_single) {
    KS_REQ_N_ARGS_RANGE(n_args, 4, 7);
    getarg_Parser self;
    ks_str name, desc;
    ks_obj opt_strs, type = (ks_obj)ks_type_str, defa = NULL;
    int64_t flags = 0;
    if (!ks_parse_params(n_args, args, "self%* name%s desc%s opt_strs%iter ?type%any ?default%any ?flags%i64", &self, getarg_type_Parser, &name, &desc, &opt_strs, &type, &defa, &flags)) return NULL;

    ks_list opt_strs_list = ks_list_from_iterable(opt_strs);
    if (!opt_strs_list) return NULL;


    int i;
    // ensure all were strings
    for (i = 0; i < opt_strs_list->len; ++i) {
        ks_str copt = (ks_str)opt_strs_list->elems[i];
        if (copt->type != ks_type_str) {
            ks_throw_fmt(ks_type_ArgError, "'opt_strs' contained a non-string!");
            KS_DECREF(opt_strs_list);
            return NULL;
        }
    }

    // internally append it to the structure
    struct getarg_arg* argp = my_addarg(self, ARG_TYPE_SINGLE, name, desc, opt_strs_list, type, defa);
    if (!argp) {
        KS_DECREF(opt_strs_list);
        return NULL;
    }

    return KSO_NONE;
}


// Parser.add_arg_multi(self, name, desc, opt_strs, num=(1,none), type=str, default=none, flags=none)
static KS_TFUNC(Parser, add_arg_multi) {
    KS_REQ_N_ARGS_RANGE(n_args, 4, 8);
    getarg_Parser self;
    ks_str name, desc;
    ks_obj opt_strs, num = NULL, type = (ks_obj)ks_type_str, defa = NULL;
    int64_t flags = 0;
    if (!ks_parse_params(n_args, args, "self%* name%s desc%s opt_strs%iter ?num%any ?type%any ?default%any ?flags%i64", &self, getarg_type_Parser, &name, &desc, &opt_strs, &num, &type, &defa, &flags)) return NULL;

    ks_list opt_strs_list = ks_list_from_iterable(opt_strs);
    if (!opt_strs_list) return NULL;

    int i;
    // ensure all were strings
    for (i = 0; i < opt_strs_list->len; ++i) {
        ks_str copt = (ks_str)opt_strs_list->elems[i];
        if (copt->type != ks_type_str) {
            ks_throw_fmt(ks_type_ArgError, "'opt_strs' contained a non-string!");
            KS_DECREF(opt_strs_list);
            return NULL;
        }
    }

    // internally append it to the structure
    struct getarg_arg* argp = my_addarg(self, ARG_TYPE_MULTI, name, desc, opt_strs_list, type, defa);
    KS_DECREF(opt_strs_list);
    if (!argp) {
        return NULL;
    }


    // now, figure out min/max
    if (!num) {
        argp->na_min = 1;
        argp->na_max = 0xFFFFFFF;
    } else if (ks_is_iterable(num)) {
        ks_list num_list = ks_list_from_iterable(num);
        if (!num_list) return NULL;

        if (num_list->len != 2) {
            KS_DECREF(num_list);
            return ks_throw_fmt(ks_type_ArgError, "if 'num' is iterable, it must contain 2 int elements (min, max)");
        }


        int64_t nmn, nmx;
        if (
            !ks_num_get_int64(num_list->elems[0], &nmn) ||
            !ks_num_get_int64(num_list->elems[1], &nmx)
        ) {
            KS_DECREF(num_list);
            return NULL;
        }

        KS_DECREF(num_list);


        argp->na_min = nmn;
        argp->na_max = nmx;

    } else {

        int64_t nitems;
        if (!ks_num_get_int64(num, &nitems)) {
            return NULL;
        }

        argp->na_min = argp->na_max = nitems;
    }



    return KSO_NONE;
}

// Parser.parse(self, user_args=__argv__[1:]) - return an object with the relevant attributes for the values given,
//   or throw an error if there was a problem
static KS_TFUNC(Parser, parse) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    getarg_Parser self;
    ks_obj user_argst = NULL;
    if (!ks_parse_params(n_args, args, "self%* ?user_args%iter", &self, getarg_type_Parser, &user_argst));

    if (!user_argst) {
        // default: take the default arguments given to main, minus the first
        ks_list main_argv = (ks_list)ks_dict_get_c(ks_globals, "__argv__");
        if (!main_argv) return NULL;

        if (main_argv->type != ks_type_list) {
            return ks_throw_fmt(ks_type_TypeError, "Internal type error; __argv__ was not a list! (something went bad here)");
        }
        
        // get the arguments
        user_argst = (ks_obj)ks_list_new(main_argv->len - 1, main_argv->elems + 1);

    } else {
        // add another reference
        user_argst = KS_NEWREF(user_argst);
    }

    // ensure it is a list
    ks_list user_args = ks_list_from_iterable(user_argst);
    KS_DECREF(user_argst);
    if (!user_args) return NULL;

    // result dictionary
    ks_dict res = ks_dict_new(0, NULL);

    // positional argument index
    int posi_i = 0;

    ks_list posi_args = ks_list_new(0, NULL);

    int i;
    // loop through provided arguments
    for (i = 0; i < user_args->len; ++i) {

        // get current argument
        ks_str user_arg = (ks_str)user_args->elems[i];
        if (user_arg->type != ks_type_str) {
            ks_throw_fmt(ks_type_ArgError, "Given argument to parse() was not a 'str'! (got '%T' instead", user_arg);
            KS_DECREF(user_args);
            KS_DECREF(res);
            KS_DECREF(posi_args);
            return NULL;
        }
        int j;

        // C-style user argument
        char* cu_argc = user_arg->chr;
        int cu_argl = user_arg->len;

        if (cu_argl == 2 && strncmp(cu_argc, "--", 2) == 0) {
            // argument break, only parse positional arguments
            ++i;
            break;
        } else if (cu_argl == 6 && strncmp(cu_argc, "--help", 6) == 0) {
            // special case: print help/usage and then exit

            ks_printf("%S v%S\n", self->program_name, self->version);
            ks_printf("  %S\n\n", self->desc);

            int help_width = 28;

            ks_str tmp;
            for (j = 0; j < self->n_args; ++j) {
                tmp = ks_fmt_c("  %*I", ",", self->args[j].opt_strs);
                tmp = ks_fmt_c("%S%*c%S", tmp, help_width - tmp->len, ' ', self->args[j].desc);

                // add number of arguments required
                if (self->args[j].argtype == ARG_TYPE_MULTI) {
                    if (self->args[j].na_min == self->args[j].na_max) {
                        tmp = ks_fmt_c("%S (%i)", tmp, (int)self->args[j].na_min);
                    } else if (self->args[j].na_min > self->args[j].na_max) {
                        tmp = ks_fmt_c("%S (%i+)", tmp, (int)self->args[j].na_min);
                    } else {
                        tmp = ks_fmt_c("%S (%i-%i)", tmp, (int)self->args[j].na_min, (int)self->args[j].na_max);
                    }

                }

                // add type information if available
                if (self->args[j].validator->type == ks_type_type) {
                    tmp = ks_fmt_c("%S (%S)", tmp, self->args[j].validator);
                }

                ks_printf("%S\n", tmp);

                KS_DECREF(tmp);


            }

            if (self->authors->len > 0) {
                ks_printf("\nAuthors:\n");

                for (j = 0; j < self->authors->len; ++j) {
                    ks_printf("  %S\n", self->authors->elems[j]);
                }
            }

            exit(0);
        }

        // keep track of whether the argument has been matched or not
        bool isFound = false;

        // iterate through list of valid argument structures
        for (j = 0; j < self->n_args && !isFound; ++j) {
            struct getarg_arg* arg_st = &self->args[j];

            int k;
            // iterate through all matches indicating this argument name
            for (k = 0; k < arg_st->opt_strs->len; ++k) {

                // get the key to search for
                ks_str this_arg_key = (ks_str)arg_st->opt_strs->elems[k];

                if (this_arg_key->len <= cu_argl && strncmp(this_arg_key->chr, cu_argc, this_arg_key->len) == 0) {
                    // it does start with the given argument key

                    bool hasValue = false, isShort = false;

                    if (this_arg_key->len == cu_argl || (hasValue = cu_argc[this_arg_key->len] == '=') || (isShort = (this_arg_key->len >= 2 && this_arg_key->chr[0] == '-' && this_arg_key->chr[1] != '-'))) {
                        // it is either a 1-1 match, or it also contains an '=VALUE' after it

                        // check if it already has contained it
                        if (ks_dict_has(res, arg_st->name->v_hash, (ks_obj)arg_st->name)) {
                            KS_DECREF(user_args);
                            KS_DECREF(res);
                            KS_DECREF(posi_args);
                            return ks_throw_fmt(ks_type_ArgError, "Missing duplicate argument '%S'", this_arg_key);
                        }

                        if (arg_st->argtype == ARG_TYPE_FLAG) {
                            // TODO: handle singular flags

                        } else if (arg_st->argtype == ARG_TYPE_SINGLE) {
                            // requires a single value

                            ks_str arg_value = NULL;
                            if (hasValue) {
                                // parse out sub string as the value part of it
                                arg_value = ks_str_new_l(&cu_argc[this_arg_key->len+1], cu_argl - this_arg_key->len - 1);
                            } else if (isShort) {
                                // start immediately after the arg key
                                arg_value = ks_str_new_l(&cu_argc[this_arg_key->len], cu_argl - this_arg_key->len);

                            } else {
                                // capture the next string
                                i++;
                                if (j >= self->n_args) {
                                    // out-of-bounds; expected another argument
                                    KS_DECREF(user_args);
                                    KS_DECREF(res);
                                    KS_DECREF(posi_args);

                                    return ks_throw_fmt(ks_type_ArgError, "Missing argument after '%S'", this_arg_key);
                                }
                                // parse the argument value from the next arg available from the input
                                arg_value = (ks_str)KS_NEWREF(user_args->elems[i]);
                            }

                            // call the validator on it
                            ks_obj created_obj = ks_call(arg_st->validator, 1, (ks_obj*)&arg_value);
                            if (!created_obj) {
                                KS_DECREF(user_args);
                                KS_DECREF(arg_value);
                                KS_DECREF(res);
                                KS_DECREF(posi_args);
                                return NULL;
                            }

                            int stat = ks_dict_set(res, arg_st->name->v_hash, (ks_obj)arg_st->name, (ks_obj)arg_value);
                            KS_DECREF(arg_value);
                        } else if (arg_st->argtype == ARG_TYPE_MULTI) {
                            // requires as many values as it can between min and max
                            // if they are the same, then values beginning with '-' are accepted
                            int num_consumed = 0;

                            // TODO; handle if it has a value
                            if (hasValue) {
                                // out-of-bounds; expected another argument
                                KS_DECREF(user_args);
                                KS_DECREF(res);
                                KS_DECREF(posi_args);

                                return ks_throw_fmt(ks_type_ArgError, "Used assign option (-opt=VALUE) on '%S', which is unsupported due to it being a multi-arg", this_arg_key);
                            }
                            // TODO; handle if it has a value
                            if (isShort) {
                                // out-of-bounds; expected another argument
                                KS_DECREF(user_args);
                                KS_DECREF(res);
                                KS_DECREF(posi_args);

                                return ks_throw_fmt(ks_type_ArgError, "Used short option (-optVALUE) on '%S', which is unsupported due to it being a multi-arg", this_arg_key);
                            }

                            // advance past it
                            i++;

                            // collection of arguments
                            ks_list collec = ks_list_new(0, NULL);

                            while (i < user_args->len && (num_consumed < arg_st->na_max || arg_st->na_max <= 0)) {
                                ks_str cur_m_arg = (ks_str)user_args->elems[i];

                                if (cur_m_arg->type != ks_type_str) {
                                    // out-of-bounds; expected another argument
                                    KS_DECREF(user_args);
                                    KS_DECREF(res);
                                    KS_DECREF(posi_args);
                                    KS_DECREF(collec);

                                    return ks_throw_fmt(ks_type_ArgError, "Encountered non-string argument in user_args ('%T' was found)", cur_m_arg);
                                }


                                if (cur_m_arg->chr[0] == '-' && num_consumed >= arg_st->na_min) {
                                    // reached an argument starting with '-' that is after the minimum number of arguments,
                                    //   so it must be considered another argument

                                    // rewind off
                                    i--;
                                    break;
                                }

                                // call the validator on it
                                ks_obj created_obj = ks_call(arg_st->validator, 1, (ks_obj*)&cur_m_arg);
                                if (!created_obj) {
                                    KS_DECREF(user_args);
                                    KS_DECREF(res);
                                    KS_DECREF(posi_args);
                                    KS_DECREF(collec);

                                    return NULL;
                                }

                                // append it to the list
                                ks_list_push(collec, created_obj);
                                KS_DECREF(created_obj);

                                // move past these
                                num_consumed++;
                                i++;

                            }

                            if (num_consumed < arg_st->na_min) {
                                // out-of-bounds; expected another argument
                                KS_DECREF(user_args);
                                KS_DECREF(res);
                                KS_DECREF(posi_args);
                                KS_DECREF(collec);

                                return ks_throw_fmt(ks_type_ArgError, "Missing arguments for '%S', expected at least %i", this_arg_key, arg_st->na_min);
                            }

                            // otherwise, done and we set it

                            int stat = ks_dict_set(res, arg_st->name->v_hash, (ks_obj)arg_st->name, (ks_obj)collec);
                            KS_DECREF(collec);
                        }

                        isFound = true;
                        break;
                    }
                }
            }
        }

        // done parsing this one
        if (!isFound) {
            // if it starts with '-' or '--' we have a problem
            if (user_arg->chr[0] == '-') {
                KS_DECREF(user_args);
                KS_DECREF(res);
                KS_DECREF(posi_args);

                return ks_throw_fmt(ks_type_ArgError, "Unknown argument '%S'", user_arg);
            }

            // positional arguments time
            ks_list_push(posi_args, (ks_obj)user_arg);
        }
    }

    // positional arguments only
    for (; i < user_args->len; ++i, posi_i++) {
        // get current argument
        ks_str user_arg = (ks_str)user_args->elems[i];
        if (user_arg->type != ks_type_str) {
            ks_throw_fmt(ks_type_ArgError, "Given argument to parse() was not a 'str'! (got '%T' instead", user_arg);
            KS_DECREF(user_args);
            KS_DECREF(res);
            KS_DECREF(posi_args);
            return NULL;
        }

        // add positional argument
        ks_list_push(posi_args, (ks_obj)user_arg);

    }

    KS_DECREF(user_args);

    // now, fill in defaults
    // iterate through list of valid argument structures
    for (i = 0; i < self->n_args; ++i) {
        struct getarg_arg* arg_st = &self->args[i];
        if (!ks_dict_has(res, arg_st->name->v_hash, (ks_obj)arg_st->name)) {
            if (!arg_st->defa) {
                KS_DECREF(res);
                KS_DECREF(posi_args);
                return ks_throw_fmt(ks_type_ArgError, "Missing argument for '%S' (%*I)", arg_st->name, ",", arg_st->opt_strs);
            }

            // doesn't contain it, so set default
            // NOTE: this ignores the validator, which is useful in many cases
            ks_dict_set(res, arg_st->name->v_hash, (ks_obj)arg_st->name, arg_st->defa);
        }
    }

    // add positional arguments    
    ks_dict_set_cn(res, (ks_dict_ent_c[]){
        {"positional", (ks_obj)posi_args},

        {NULL, NULL}        
    });

    ks_namespace res_n = ks_namespace_new(res);
    KS_DECREF(res);

    return (ks_obj)res_n;
}




// now, export them all
static ks_module get_module() {
    
    ks_module mod = ks_module_new(MODULE_NAME);

    KS_INIT_TYPE_OBJ(getarg_type_Parser, "Parser");

    ks_type_set_cn(getarg_type_Parser, (ks_dict_ent_c[]){
        {"__new__",         (ks_obj)ks_cfunc_new2(Parser_new_, "Parser.__new__(program_name, version, desc, authors)")},
        {"__free__",        (ks_obj)ks_cfunc_new2(Parser_free_, "Parser.__free__(self)")},

        {"add_arg_single",  (ks_obj)ks_cfunc_new2(Parser_add_arg_single_, "Parser.add_arg_single(self, name, desc, opt_strs, type=str, defa=None, flags=0)")},
        {"add_arg_multi",  (ks_obj)ks_cfunc_new2(Parser_add_arg_multi_, "Parser.add_arg_multi(self, name, desc, opt_strs, num=(1, none), type=str, defa=None, flags=0)")},

        {"parse",           (ks_obj)ks_cfunc_new2(Parser_parse_, "Parser.parse(self, args=none)")},

        /*
        {"__setitem__",    (ks_obj)ks_cfunc_new2(array_setitem_, "nx.array.__setitem__(self, *idxs)")},

        {"shape",          (ks_obj)ks_cfunc_new2(array_shape_, "nx.array.shape(self)")},*/

        {NULL, NULL},
    });

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){

        {"Parser",        (ks_obj)getarg_type_Parser},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
