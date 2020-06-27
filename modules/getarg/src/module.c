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

}* getarg_Parser;


// declare the array type as residing in this file
KS_TYPE_DECLFWD(getarg_type_Parser);




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
            ks_obj new_str = ks_fmt_c("%S", authors_list->elems[i]);
            if (!new_str) {
                KS_DECREF(authors_list);
                return NULL;
            }

            // replace object with its 'str' in the list
            KS_DECREF(authors_list->elems[i]);
            authors_list->elems[i] = new_str;
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



// Parser.add_arg_single(self, name, desc, opt_strs, flags)
// Parser.add_arg_single(self, name, desc, opt_strs, type, flags, default=none)
static KS_TFUNC(Parser, parse) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    getarg_Parser self;
    ks_obj vargs = NULL;
    if (!ks_parse_params(n_args, args, "self%* ?args%iter", &self, getarg_type_Parser, &vargs));

    // get global arguments
    if (!vargs) vargs = ks_dict_get_c(ks_globals, "__argv__");
    if (!vargs) return NULL;

    ks_list args_list = ks_list_from_iterable(vargs);
    if (!args_list) return NULL;

    int len = args_list->len;

    int i;
    for (i = 0; i < len; ++i) {
        ks_str carg = args_list->elems[i];
        if (carg->type != ks_type_str) {
            ks_throw_fmt(ks_type_ArgError, "Given argument to parse() was not a 'str'! (got '%T' instead", carg);
            KS_DECREF(args_list);
            return NULL;
        }

        // now, attempt to parse the current argument


    }

    return KSO_NONE;

}




// Parser.parse(self, args=__argv__[1:]) - return an object with the relevant attributes for the values given,
//   or throw an error if there was a problem
static KS_TFUNC(Parser, parse) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    getarg_Parser self;
    ks_obj vargs = NULL;
    if (!ks_parse_params(n_args, args, "self%* ?args%iter", &self, getarg_type_Parser, &vargs));

    // get global arguments
    if (!vargs) vargs = ks_dict_get_c(ks_globals, "__argv__");
    if (!vargs) return NULL;

    ks_list args_list = ks_list_from_iterable(vargs);
    if (!args_list) return NULL;

    int len = args_list->len;

    int i;
    for (i = 0; i < len; ++i) {
        ks_str carg = args_list->elems[i];
        if (carg->type != ks_type_str) {
            ks_throw_fmt(ks_type_ArgError, "Given argument to parse() was not a 'str'! (got '%T' instead", carg);
            KS_DECREF(args_list);
            return NULL;
        }

        // now, attempt to parse the current argument


    }

    return KSO_NONE;
}




// now, export them all
static ks_module get_module() {
    
    ks_module mod = ks_module_new(MODULE_NAME);

    KS_INIT_TYPE_OBJ(getarg_type_Parser, "Parser");

    ks_type_set_cn(getarg_type_Parser, (ks_dict_ent_c[]){
        {"__new__",        (ks_obj)ks_cfunc_new2(Parser_new_, "Parser.__new__(program_name, version, desc, authors)")},
        {"__free__",        (ks_obj)ks_cfunc_new2(Parser_free_, "Parser.__free__(self)")},

        /*
        {"__getitem__",    (ks_obj)ks_cfunc_new2(array_getitem_, "nx.array.__getitem__(self, *idxs)")},
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
