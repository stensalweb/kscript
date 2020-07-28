/* stack_frame.c - implementation of the 'stack_frame' type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"


// create a stack frame
ks_stack_frame ks_stack_frame_new(ks_obj func) {
    ks_stack_frame self = KS_ALLOC_OBJ(ks_stack_frame);
    KS_INIT_OBJ(self, ks_T_stack_frame);

    self->func = KS_NEWREF(func);

    self->code = NULL;
    if (func->type == ks_T_code) {
        self->code = (ks_code)func;
    } else if (func->type == ks_T_kfunc) {
        self->code = ((ks_kfunc)func)->code;
    }

    self->locals = NULL;
    self->pc = NULL;

    return self;
}



// stack_frame.__str__(self) - convert to a string
static KS_TFUNC(stack_frame, str) {
    ks_stack_frame self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_stack_frame)) return NULL;

    if (self->func->type == ks_T_code) {
        // attempt to search for it

        ks_code code_obj = (ks_code)self->func;

        // get current offset into btecode
        int offset = (int)(self->pc - code_obj->bc);

        int fi = -1, i;
        for (i = 0; i < code_obj->meta_n; ++i) {
            if (offset <= code_obj->meta[i].bc_n) {
                fi = i;
                break;
            }
        }
        if (fi >= 0) {
            // set information
            //ks_list_popu(call_stk);
            ks_str o_str = ks_tok_expstr_2(code_obj->parser, code_obj->meta[fi].tok);
            ks_str new_str = ks_fmt_c("%S (line %i, col %i): %S", code_obj->name_hr, code_obj->meta[fi].tok.line+1, code_obj->meta[fi].tok.col+1, o_str);
            KS_DECREF(o_str);

            return (ks_obj)new_str;
        }
    } else if (self->func->type == ks_T_kfunc) {
        // attempt to search for it

        ks_code code_obj = ((ks_kfunc)self->func)->code;

        // get current offset into btecode
        int offset = (int)(self->pc - code_obj->bc);

        int fi = -1, i;
        for (i = 0; i < code_obj->meta_n; ++i) {
            if (offset <= code_obj->meta[i].bc_n) {
                fi = i;
                break;
            }
        }
        if (fi >= 0) {
            // set information
            //ks_list_popu(call_stk);
            ks_str o_str = ks_tok_expstr_2(code_obj->parser, code_obj->meta[fi].tok);
            ks_str new_str = ks_fmt_c("%S (line %i, col %i): %S", code_obj->name_hr, code_obj->meta[fi].tok.line+1, code_obj->meta[fi].tok.col+1, o_str);
            KS_DECREF(o_str);

            return (ks_obj)new_str;
        }

    } else if (self->func->type == ks_T_cfunc) {
        return (ks_obj)ks_fmt_c("%S [cfunc]", ((ks_cfunc)self->func)->sig_hr);
    }

    return (ks_obj)ks_fmt_c("<'stack_frame' type(func): %T obj @ %p>", self->func, self);
};
// stack_frame.__free__(self) - free obj
static KS_TFUNC(stack_frame, free) {
    ks_stack_frame self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_stack_frame)) return NULL;

    KS_DECREF(self->func);

    if (self->code != NULL) KS_DECREF(self->code);
    if (self->locals != NULL) KS_DECREF(self->locals);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_stack_frame);

void ks_init_T_stack_frame() {
    ks_type_init_c(ks_T_stack_frame, "stack_frame", ks_T_obj, KS_KEYVALS(
        {"__str__",               (ks_obj)ks_cfunc_new_c(stack_frame_str_, "stack_frame.__str__(self)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(stack_frame_free_, "stack_frame.__free__(self)")},
    ));

}




