/* opt/propconst.c - AST optimization pass for propogation of constants 

Let 'I' denote an integer constant

Many operations can be 'folded' or 'propogated' through an AST. Due to kscript's nature
as a dynamic language, however, this is very difficult in general. Things like assignments,
'level playing field operations' like (a+b+c) can not always be reordered, for example:

`obj+2+3` seems to be the same as `obj+(2+3)`, although the literal parsing is `(obj+2)+3`

This is not always true, however, since the type of `obj` is unknown, and thus the `__add__` method
may not return an integer, and may not be communative/associative in nature.

So, this folding/propogator will always be conservative in what it can replace.

Now, as for what it can do:

  * `I+I` -> `I`, sums of literal integers are replaced with their sums (same is true for all binary 
        operations on 2 constant integers)
  * `~I` -> `I`, computes the result of unary operators on integers


PLANNED:

These are not implemented yet (as their speed boost will probably be neglible anyway)

  * loop unrolling, `for i = $start, i < $n, $i = $i + $inc { BODY }`, will be transformed into a number
        of BODY's with `i` substituted in as a variable


*/

#include "ks_common.h"

ks_ast ks_ast_opt_propconst(ks_ast self, void* data) {

    ks_ast ret = NULL;

    switch (self->atype) {

    // handle all binary operators
    case KS_AST_BOP_ADD:
    case KS_AST_BOP_SUB:
    case KS_AST_BOP_MUL:
    case KS_AST_BOP_DIV:
    case KS_AST_BOP_MOD:
    case KS_AST_BOP_POW:
        {
            ks_ast L = self->v_bop.L, R = self->v_bop.R;
            if (L->atype == KS_AST_INT && R->atype == KS_AST_INT) {
                // reduce to an op on ints
                int64_t iL = ((ks_int)L->v_val)->v_int, iR = ((ks_int)R->v_val)->v_int;

                /*  */ if (self->atype == KS_AST_BOP_ADD) {
                    ret = ks_ast_new_int(iL + iR);
                } else if (self->atype == KS_AST_BOP_SUB) {
                    ret = ks_ast_new_int(iL - iR);
                } else if (self->atype == KS_AST_BOP_MUL) {
                    ret = ks_ast_new_int(iL * iR);
                } else if (self->atype == KS_AST_BOP_DIV) {
                    ret = ks_ast_new_int(iL / iR);
                } else if (self->atype == KS_AST_BOP_MOD) {
                    int64_t res = iL % iR;
                    if (res < 0) res += iR;
                    ret = ks_ast_new_int(res);
                } else if (self->atype == KS_AST_BOP_POW) {
                    if (iR == 0) {
                        ret = ks_ast_new_int(1);
                    } else if (iR < 0) {
                        ret = ks_ast_new_int(0);
                    } else if (iR == 1) {
                        ret = ks_ast_new_int(iL);
                    } else {
                        // compute power
                        // the sign to be applied
                        bool neg = (iL < 0 && iR & 1 == 1);
                        if (iL < 0) iL = -iL;

                        // now, use repeated squaring to speed up the computation
                        int64_t Ri = 1;
                        while (iR != 0) {
                            if (iR & 1) Ri *= iL;
                            iR >>= 1;
                            iL *= iL;
                        }
                        ret = ks_ast_new_int(neg ? -Ri : Ri);

                    }
                }
            }
        }
        break;

    // handle all unary operators
    case KS_AST_UOP_NEG:
    case KS_AST_UOP_SQIG:
        {
            ks_ast v = self->v_uop;
            if (v->atype == KS_AST_INT) {
                // reduce to an integer
                if (self->atype == KS_AST_UOP_NEG) {
                    ret = ks_ast_new_int(-((ks_int)v->v_val)->v_int);
                } else if (self->atype == KS_AST_UOP_SQIG) {
                    ret = ks_ast_new_int(~((ks_int)v->v_val)->v_int);
                }
            }
        }
        break;
    
    }

    if (ret == NULL) {
        // nothing was replaced
        return self;
    } else {
        // there was a new generated value, so replace it and delete the old reference
        KSO_DECREF(self);
        return ret;
    }

}




