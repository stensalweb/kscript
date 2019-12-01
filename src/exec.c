// exec.c - the execution of kscript's bytecode format
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"



// have some static programs for manual error handling, at a global level
static struct ks_bc_inst kse_e0[] = {
    { .type = KS_BC_LOAD, ._load = KS_STR_CONST("print") },
    { .type = KS_BC_CALL, ._call.args_n = 1 },
    { .type = KS_BC_CONST_STR, ._str = KS_STR_CONST("^ exception was thrown, causing program to exit") },
    { .type = KS_BC_LOAD, ._load = KS_STR_CONST("print") },
    { .type = KS_BC_CALL, ._call.args_n = 1 },
    { .type = KS_BC_LOAD, ._load = KS_STR_CONST("__stacktrace") },
    { .type = KS_BC_CALL, ._call.args_n = 0 },

    { .type = KS_BC_CONST_INT, ._int = 1 },
    { .type = KS_BC_EXIT }
};


ks_obj ks_exec_kfunc(ks_ctx ctx, ks_kfunc kfunc, int args_n, ks_obj* args) {
    if (args_n != kfunc.params_n) {
        ks_error("Not the right number of arguments!");
        return NULL;
    }

    int i;
    for (i = 0; i < args_n; ++i) {
        ks_stk_push(&ctx->stk, args[i]);
    }

    // now actually execute it
    ks_exec(ctx, kfunc.inst);

    return NULL;
}


// just starts executing given a starting program counter
void ks_exec(ks_ctx ctx, ks_bc_inst* bc) {

    // program counter
    ks_bc_inst* pc = bc;

    // current instruction
    ks_bc_inst cur;

    // found object
    ks_obj found;

    int count = 0;

    // return addresses, for jumping between functions
    ks_bc_inst* rstk[4096];
    int rstk_ptr = -1;

    // exception handler addresses, for handling any exception that arises
    ks_bc_inst* estk[4096];
    // correct lengths of the stacks
    int estk_lens[4096];
    // how deep is the current exception handler (every time rstk is pushed to, this should go up)
    // this is so rstk can be rewinded should an exception occur
    int estk_depth[4096];
    // current elem
    int estk_ptr = -1;

    // initialize with the global exception handler
    estk_ptr++;
    estk[estk_ptr] = kse_e0;
    estk_lens[estk_ptr] = ctx->stk.len;
    estk_depth[estk_ptr] = 0;

    // reset the exception
    ctx->cexc = NULL;

    // interp loop
    // TODO: Maybe rewrite using jump tables? We'll see what the performance may be
    while (true) {
        
        count++;
        cur = *pc++;

        if (cur.type == KS_BC_NOOP) {
            continue;
        } else if (cur.type == KS_BC_LOAD) {
            // resolve symbol
            found = ks_ctx_resolve(ctx, cur._load);

            if (found == NULL) {
                ks_error("Unknown local: %s", cur._load._);
                return;
            }
            ks_stk_push(&ctx->stk, found);
            continue;
        } else if (cur.type == KS_BC_STORE) {
            found = ks_stk_pop(&ctx->stk);
            ks_dict_set(&ctx->call_stk_scopes[ctx->call_stk_n - 1]->locals, cur._store, found);
            continue;
        } else if (cur.type == KS_BC_RET || cur.type == KS_BC_RET_NONE) {


            // since there should be nothing on the stack, just make it `none`
            if (cur.type == KS_BC_RET_NONE) {
                ks_stk_push(&ctx->stk, ks_obj_new_none());
            }

            // pop off one level of scope
            ks_scope_free(ks_ctx_pop(ctx));

            // we have just left the main function
            if (ctx->call_stk_n == 0) return;

            // return the program counter back to where it was before executing the function
            pc = rstk[rstk_ptr--];

            // now, take one back off our depth, if we've gone back up past the exception handler, then
            //   we need to remove the exception handler
            if (--estk_depth[estk_ptr] < 0) {
                estk_ptr--;
            }
            continue;

        } else if (cur.type == KS_BC_CONST_INT) {
            ks_stk_push(&ctx->stk, ks_obj_new_int(cur._int));
            continue;

        } else if (cur.type == KS_BC_CONST_FLOAT) {
            ks_stk_push(&ctx->stk, ks_obj_new_float(cur._float));
            continue;

        } else if (cur.type == KS_BC_CONST_STR) {
            ks_stk_push(&ctx->stk, ks_obj_new_str(cur._str));
            continue;

        } else if (cur.type == KS_BC_JMPI) {
            pc += cur._jmpi;
            continue;

        } else if (cur.type == KS_BC_JMPC) {
            // pop off condition from the stack
            found = ks_stk_pop(&ctx->stk);
            // check conditionals
            if ((found->type == KS_TYPE_INT && found->_int != 0)) {
                pc += cur._jmpc;
            }
            continue;

        } else if (cur.type == KS_BC_CALL) {

            ks_obj func = ks_stk_pop(&ctx->stk);

            if (func->type == KS_TYPE_CFUNC) {

                // since the stack should have the last few values, we should just call with the stack, instead of allocating
                //   a new arguments array
                if (ctx->stk.len < cur._call.args_n) {
                    ks_error("Not enough arguments on stack!");
                    return;
                }


                // produce a readable name
                ks_str read_name = ks_str_fmt("%s [cfunc] @ %p", "__cfunc", func->_cfunc);


                ks_scope new_scope = ks_scope_new(ctx->call_stk_scopes[ctx->call_stk_n - 1]);
                // push it on
                ks_ctx_push(ctx, read_name, new_scope);


                ks_str_free(&read_name);

                // run function
                ks_obj result = func->_cfunc(ctx, cur._call.args_n, &(ctx->stk.vals[ctx->stk.len - cur._call.args_n]));
                ctx->stk.len -= cur._call.args_n;

                if (ctx->cexc != NULL) {
                    // an exception occured; handle it
                    // now, start executing at the exception handler
                    int right_len = estk_lens[estk_ptr];
                    pc = estk[estk_ptr--];
                    
                    // rewind
                    ctx->stk.len = right_len;

                    // pop our exception back on
                    ks_stk_push(&ctx->stk, ctx->cexc);

                    // reset the exception
                    ctx->cexc = NULL;

                    // now the exception handler just has to take off the top object

                } else {

                    // pop the result back on the stack
                    ks_stk_push(&ctx->stk, result);
                }


                // leave the scope
                //ks_scope_free(ks_ctx_pop(ctx));
                //ks_scope old_scope = ks_ctx_pop(ctx);
                ks_scope_free(ks_ctx_pop(ctx));

                continue;

            } else if (func->type == KS_TYPE_KFUNC) {
                // continue executing in the interpreter loop


                // every time we enter into another function, we should pop on a new scope, setting
                //   the last one as the parent to it
                ks_scope new_scope = ks_scope_new(ctx->call_stk_scopes[ctx->call_stk_n - 1]);

                ks_str read_name = ks_str_fmt("%s [kfunc] @ %p", "__kfunc", func->_kfunc.inst);

                int sidx = ks_ctx_push(ctx, read_name, new_scope);

                ks_str_free(&read_name);

                // just double check this
                if (func->_kfunc.params_n != cur._call.args_n) {

                    // throw an exception
                    found = ks_obj_new_exception_fmt("Wrong number of arguments (expected %d, but got %d)", func->_kfunc.params_n, cur._call.args_n);

                    // now, start executing at the exception handler
                    int right_len = estk_lens[estk_ptr];
                    pc = estk[estk_ptr--];
                    
                    // rewind
                    ctx->stk.len = right_len;

                    // pop our exception back on
                    ks_stk_push(&ctx->stk, found);

                    // now the exception handler just has to take off the top object
                    continue;
                }

                // set all the parameters in the new local scope by name
                int i;
                for (i = 0; i < cur._call.args_n; ++i) {
                    ks_dict_set(&ctx->call_stk_scopes[sidx]->locals, func->_kfunc.param_names[i], ctx->stk.vals[ctx->stk.len - cur._call.args_n + i]);
                }

                // take them all off the stack, consume them
                ctx->stk.len -= cur._call.args_n;

                // store our current position into the return stack
                rstk[++rstk_ptr] = pc;
                // but now, start executing at the function's instructions
                pc = func->_kfunc.inst;

                // since we are stepping one deeper into the call stack,
                //   we increment the depth for the exception handler
                estk_depth[estk_ptr]++;

                continue;

            } else {
                ks_error("Unknown type to use call() on: %d", func->type);
                return;
            }


        } else if (cur.type == KS_BC_THROW) {
            // we are throwing this as the exception
            found = ks_stk_pop(&ctx->stk);

            // now, start executing at the exception handler
            int right_len = estk_lens[estk_ptr];
            pc = estk[estk_ptr--];
            
            // rewind
            ctx->stk.len = right_len;

            // pop our exception back on
            ks_stk_push(&ctx->stk, found);

            // now the exception handler just has to take off the top object
            continue;

        } else if (cur.type == KS_BC_ERET) {
            // returning from an exception, so just do nothing
            continue;

        } else if (cur.type == KS_BC_EXIT) {
            // exit out with the given error code
            found = ks_stk_pop(&ctx->stk);
            exit(found->_int);
            continue;

        } else {
            ks_error("Unknown inst type! (%d)", cur.type);
            return;
        }
    }
}



