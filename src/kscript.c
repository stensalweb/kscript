// kscript.c - the main commandline interface to the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


// return (string) representation of a single argument
ks_obj ks_std_repr(int args_n, ks_obj* args) {
    if (args_n != 1) {
        ks_error("repr takes %d args, was given %d", 1, args_n);
        return ks_obj_new_none();
    }

    // get the only argument
    ks_obj A = args[0];
    // the result we'll return
    ks_obj ret = ks_obj_new_str(KS_STR_EMPTY);

    // check type
    if (A->type == KS_TYPE_NONE) {
        ks_str_copy(&ret->_str, KS_STR_CONST("NONE"));
    } else if (A->type == KS_TYPE_INT) {
        char tmp[100];
        sprintf(tmp, "%ld", A->_int);
        ks_str_copy_cp(&ret->_str, tmp, strlen(tmp));
    } else if (A->type == KS_TYPE_FLOAT) {
        char tmp[100];
        sprintf(tmp, "%lf", A->_float);
        ks_str_copy_cp(&ret->_str, tmp, strlen(tmp));
    } else if (A->type == KS_TYPE_STR) {
        ks_str_append_c(&ret->_str, '"');
        ks_str_append(&ret->_str, A->_str);
        ks_str_append_c(&ret->_str, '"');

    } else if (A->type >= KS_TYPE_CUSTOM) {
        ks_str_append_c(&ret->_str, '{');

        // add all entries of dictionary
        int i;
        for (i = 0; i < A->_dict.len; ++i) {
            if (i != 0) { 
                ks_str_append(&ret->_str, KS_STR_CONST(", "));
            }

            ks_str_append(&ret->_str, A->_dict.keys[i]);
            ks_str_append(&ret->_str, KS_STR_CONST(": "));

            // get repr of subobject
            ks_obj subrepr = ks_std_repr(1, &A->_dict.vals[i]);
            ks_str_append(&ret->_str, subrepr->_str);
            ks_obj_free(subrepr);

        }

        ks_str_append_c(&ret->_str, '}');
    } else {
        ks_error("repr given unknown type (id: %d)", A->type);
    }

    return ret;
}

// print all arguments as string representations, joined by spaces
ks_obj ks_std_print(int args_n, ks_obj* args) {

    int i;
    for (i = 0; i < args_n; ++i) {
        ks_obj repr = ks_std_repr(1, &args[i]);

        if (i != 0) printf(" ");
        if (repr->type == KS_TYPE_STR) {
            printf("%s", repr->_str._);
        } else {
            ks_error("Internal error; `repr` gave a non-string");
            ks_obj_free(repr);
            return ks_obj_new_none();
        }

        ks_obj_free(repr);
    }

    // end with a newline
    printf("\n");

    return ks_obj_new_none();
}


int main(int argc, char** argv) {

    ks_obj sconst = ks_obj_new_str(KS_STR_CONST("ANSWER OF ALL IS"));
    ks_obj num = ks_obj_new_int(42);
    ks_obj dict = ks_obj_new_custom();

    ks_dict_set(&dict->_dict, KS_STR_CONST("key1"), num);
    ks_dict_set(&dict->_dict, KS_STR_CONST("key2"), sconst);

    ks_obj print = ks_obj_new_cfunc(ks_std_print);
    
    //print->_cfunc(2, (ks_obj[]){ sconst, num });
    print->_cfunc(1, (ks_obj[]){ dict });

    //ks_std_print(2, (ks_obj[]){ sconst, num });

    // always free when you're done!
    ks_obj_free(sconst);
    ks_obj_free(num);
    ks_obj_free(dict);

    return 0;
}


