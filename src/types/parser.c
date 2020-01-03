/* types/parser.c - represents a language parser */

#include "ks_common.h"



TFUNC(parser, free) {
    #define SIG "parser.__free__(self)"
    REQ_N_ARGS(1);
    ks_parser self = (ks_parser)args[0];
    REQ_TYPE("self", self, ks_T_parser);

    ks_free(self->toks);

    KSO_DECREF(self->src_name);
    KSO_DECREF(self->src);

    ks_free(self);

    return KSO_NONE;
    #undef SIG
}



/* exporting functionality */

struct ks_type T_parser, *ks_T_parser = &T_parser;

void ks_init__parser() {

    /* first create the type */
    T_parser = (struct ks_type) {
        KS_TYPE_INIT("parser")

        .f_free = (kso)ks_cfunc_newref(parser_free_)

    };

}




