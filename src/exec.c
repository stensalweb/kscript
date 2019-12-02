// exec.c - the execution of kscript's bytecode format
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

// uncomment this to not do any safety checks, assume input is properly created
#define DO_SAFETY_CHECKS



// have some static programs for manual error handling, at a global level
static struct ks_bc_inst kse_e0[] = {
    { .type = KS_BC_LOAD, ._name = KS_STR_CONST("print") },
    { .type = KS_BC_CALL, ._args_n = 1 },
    { .type = KS_BC_CONST_STR, ._str = KS_STR_CONST("^ exception was thrown, causing program to exit") },
    { .type = KS_BC_LOAD, ._name = KS_STR_CONST("print") },
    { .type = KS_BC_CALL, ._args_n = 1 },
    { .type = KS_BC_LOAD, ._name = KS_STR_CONST("__stacktrace") },
    { .type = KS_BC_CALL, ._args_n = 0 },

    { .type = KS_BC_CONST_INT, ._int = 1 },
    { .type = KS_BC_ERET }
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
    ks_bc_inst ci;

    // temporary objects (found for looking up symbols, popped is for recently popped values, etc)
    ks_obj found, popped, result;

    // number of arguments for a function call
    int args_n;


    // return addresses, for jumping between functions
    ks_bc_inst* rstk[4096];
    int rstk_ptr = -1;

    // exception handler addresses, for handling any exception that arises
    ks_bc_inst* estk[4096];
    // correct lengths of the stacks
    int estk_lens[4096];
    // how deep is the cirent exception handler (every time rstk is pushed to, this should go up)
    // this is so rstk can be rewinded should an exception occi
    int estk_depth[4096];
    // cirent elem
    int estk_ptr = -1;

    // initialize with the global exception handler
    /*estk_ptr++;
    estk[estk_ptr] = kse_e0;
    estk_lens[estk_ptr] = ctx->stk.len;
    estk_depth[estk_ptr] = 0;*/

    // reset the exception
    ctx->cexc = NULL;

    // number of instructions executed
    int count = 0;

    // interp loop
    // TODO: Maybe rewrite using jump tables? We'll see what the performance may be
    while (true) {
        
        // do next instruction
        do_next: ;

        // increment counter
        count++;

        // read the next instruction, and increment program counter
        ci = *pc++;

        switch (ci.type) {
            case KS_BC_NOOP:
                goto do_next;

            /* load/stores */

            case KS_BC_LOAD:
                // try and resolve the symbol
                found = ks_ctx_resolve(ctx, ci._name);

                if (found == NULL) {
                    // notfound! @@@ EXCEPTION IS THROWN
                    ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Unknown variable: %s", ci._name._));
                    goto do_exception;
                } else {
                    // found, just push it on the stack
                    ks_stk_push(&ctx->stk, found);
                    goto do_next;
                }
            case KS_BC_STORE:
                // pop off the value, then set it as a local
                // TODO: detect when it is higher up
                popped = ks_stk_pop(&ctx->stk);
                ks_dict_set(&ctx->call_stk_scopes[ctx->call_stk_n - 1]->locals, ci._name, found);
                goto do_next;
            case KS_BC_ATTR:
                // pop off an object, and get its attr
                popped = ks_stk_pop(&ctx->stk);
                // find attr
                found = ks_dict_get(&found->_dict, ci._name);
                if (found == NULL) {
                    // notfound! @@@ EXCEPTION IS THROWN
                    ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Unknown attr: %s", ci._name._));
                    goto do_exception;
                } else {
                    // it was found, push it to the stack
                    ctx->stk.vals[ctx->stk.len++] = found;
                    goto do_next;
                }

            /* constants */
            
            case KS_BC_CONST_NONE:
                ks_stk_push(&ctx->stk, ks_obj_new_none());
                goto do_next;
            case KS_BC_CONST_INT:
                ks_stk_push(&ctx->stk, ks_obj_new_int(ci._int));
                goto do_next;
            case KS_BC_CONST_BOOL:
                ks_stk_push(&ctx->stk, ks_obj_new_bool(ci._bool));
                goto do_next;
            case KS_BC_CONST_FLOAT:
                ks_stk_push(&ctx->stk, ks_obj_new_float(ci._float));
                goto do_next;
            case KS_BC_CONST_STR:
                ks_stk_push(&ctx->stk, ks_obj_new_str(ci._str));
                goto do_next;

            /* calling */

            case KS_BC_VCALL:
                // first, pop off the number of arguments
                popped = ks_stk_pop(&ctx->stk);

                // must be an int!
                if (popped->type != KS_TYPE_INT) {
                    // @@@ EXCEPTION IS THROWN
                    ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Argument on top of stack of 'vcall' instruction was not int! (was of type '%s')", ctx->type_names[popped->type]));
                    goto do_exception;
                }

                // set it
                ci._args_n = (int)popped->_int;

                // now, flow through and do the normal call method
            case KS_BC_CALL:
                // pop off the function
                popped = ks_stk_pop(&ctx->stk);

                // ensure there are enough arguments
                if (ctx->stk.len < ci._args_n) {
                    // @@@ EXCEPTION IS THROWN
                    ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Not enough arguments for function, expected %d, but stack only had %d", ci._args_n, ctx->stk.len));
                    goto do_exception;
                }

                if (popped->type == KS_TYPE_CFUNC) {
                    // execute a C-style function

                    // create a named scope, add to the call stack
                    ks_str read_name = ks_str_fmt("%s [cfunc] @ %p", "__cfunc", popped->_cfunc);
                    ks_scope new_scope = ks_scope_new(ctx->call_stk_scopes[ctx->call_stk_n - 1]);
                    ks_ctx_push(ctx, read_name, new_scope);
                    ks_str_free(&read_name);

                    // execute and get the result from the C function
                    result = popped->_cfunc(ctx, ci._args_n, &(ctx->stk.vals[ctx->stk.len - ci._args_n]));
                    ctx->stk.len -= ci._args_n;

                    // check for any exceptions raised by the C function
                    if (ctx->cexc != NULL) {
                        ks_stk_push(&ctx->stk, ctx->cexc);
                        ctx->cexc = NULL;
                        goto do_exception;
                    } else {
                        // exit the scope:
                        ks_scope_free(ks_ctx_pop(ctx));
                        // pop the result back on the stack, continue executing
                        ks_stk_push(&ctx->stk, result);
                        goto do_next;
                    }

                } else if (popped->type == KS_TYPE_KFUNC) {
                    // execute a kscript function (which will still be in the interpreter)

                    // make sure the given number of arguments is the same the function expects
                    if (popped->_kfunc.params_n != ci._args_n) {
                        // @@@ EXCEPTION IS THROWN
                        ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Wrong number of arguments (expected %d, but got %d)", popped->_kfunc.params_n, ci._args_n));
                        goto do_exception;
                    }

                    // create a named scope, add to the call stack
                    ks_str read_name = ks_str_fmt("%s [kfunc] @ %p", "__kfunc", popped->_kfunc.inst);
                    ks_scope new_scope = ks_scope_new(ctx->call_stk_scopes[ctx->call_stk_n - 1]);
                    int sidx = ks_ctx_push(ctx, read_name, new_scope);
                    ks_str_free(&read_name);

                    // now, add stack variables as local vars
                    int i;
                    for (i = 0; i < popped->_kfunc.params_n; ++i) {
                        ks_dict_set(&ctx->call_stk_scopes[sidx]->locals, popped->_kfunc.param_names[i], ctx->stk.vals[ctx->stk.len - popped->_kfunc.params_n + i]);
                    }

                    // pop them all off in one go
                    ctx->stk.len -= ci._args_n;

                    // now, basically jump and link, so we will resume executing directly after the current instruction
                    rstk[++rstk_ptr] = pc;
                    pc = popped->_kfunc.inst;

                    // since we are stepping one deeper into the call stack,
                    //   we increment the depth for the exception handler, so it can rewind successfull and not mess up stacks
                    estk_depth[estk_ptr]++;

                    goto do_next;

                } else {
                    //ks_error("Unknown type to use call() on: %d", func->type);
                    ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Unknown type to use call() on: %s", ctx->type_names[popped->type]));
                    goto do_exception;
                }
            
            /* returning */

            case KS_BC_RET:
                // pop off one level of scope (since we're returning from the function)
                ks_scope_free(ks_ctx_pop(ctx));

                // ending the program, since there is nowhere to return to
                if (ctx->call_stk_n == 0) return;

                // resume at the last place we called the function at
                pc = rstk[rstk_ptr--];

                // now, take one back off our depth, if we've gone back up past the exception handler (depth==0), then
                //   we need to remove the exception handler, since its no longer relevant
                // TODO: Need a better way of (in general) managing exceptions, like:
                // try {
                //   y = 1 / 0   
                // } catch (e) {
                //    print (e)
                // }
                // 1 / 0
                //
                // the later 1 / 0 will still be caught, since the exception handler isn't removed until the function returns

                if (--estk_depth[estk_ptr] < 0) {
                    estk_ptr--;
                }

                goto do_next;

            /* conditions */

            case KS_BC_JMP:
                pc += ci._relamt;
                goto do_next;
            case KS_BC_BEQT:
                popped = ks_stk_pop(&ctx->stk);
                if (popped->type == KS_TYPE_BOOL && popped->_bool) {
                    pc += ci._relamt;
                }
                goto do_next;
            case KS_BC_BEQF:
                popped = ks_stk_pop(&ctx->stk);
                if (popped->type == KS_TYPE_BOOL && !popped->_bool) {
                    pc += ci._relamt;
                }
                goto do_next;


            case KS_BC_THROW:
                // the last item should be the exception, so handle the exception
                goto do_exception;

            case KS_BC_ERET:
                goto do_exception;

            default:
                ks_stk_push(&ctx->stk, ks_obj_new_exception_fmt("Bytecode internal format error! (got bc.type=%d)", ci.type));
                goto do_exception;
        }

        // exception handler
        do_exception: ;

        // pop of the exception
        popped = ks_stk_pop(&ctx->stk);

        // an exception occied; handle it
        // now, start executing at the exception handler
        int right_len = estk_lens[estk_ptr];
        int back_depth = estk_depth[estk_ptr];
        pc = estk[estk_ptr--];

        // we need to go back `back_depth` scopes        
        int i;
        for (i = 0; i < back_depth; ++i) {
            ks_scope_free(ks_ctx_pop(ctx));
        }

        // rewind
        ctx->stk.len = right_len;

        // push our exception back on
        ks_stk_push(&ctx->stk, popped);

        // now, execute at the exception handler
        goto do_next;

    }
}



