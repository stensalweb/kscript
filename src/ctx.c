// ctx.c - implementation of the global context type for kscript
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


ks_ctx ks_ctx_new() {
    ks_ctx ret = (ks_ctx)malloc(sizeof(struct ks_ctx));
    ret->call_stk_n = 0;
    ret->call_stk_scopes = NULL;
    ret->call_stk_names = NULL;
    ret->types_n = 0;
    ret->type_names = NULL;
    ret->types = NULL;
    ret->stk = KS_STK_EMPTY;
    return ret;
}

int ks_ctx_new_type(ks_ctx ctx, ks_str name, ks_obj type) {
    int idx = ctx->types_n++;
    ctx->type_names = realloc(ctx->type_names, sizeof(ks_str) * ctx->types_n);
    ctx->types = realloc(ctx->types, sizeof(ks_obj) * ctx->types_n);
    ctx->type_names[idx] = ks_str_dup(name);
    ctx->types[idx] = type;
    return idx;
}

int ks_ctx_push(ks_ctx ctx, ks_str name, ks_scope scope) {
    int idx = ctx->call_stk_n++;
    ctx->call_stk_scopes = realloc(ctx->call_stk_scopes, sizeof(ks_scope) * ctx->call_stk_n);
    ctx->call_stk_names = realloc(ctx->call_stk_names, sizeof(ks_str) * ctx->call_stk_n);
    ctx->call_stk_scopes[idx] = scope;
    ctx->call_stk_names[idx] = ks_str_dup(name);
    return idx;
}

ks_scope ks_ctx_pop(ks_ctx ctx) {
    int idx = --ctx->call_stk_n;
    ks_str_free(&ctx->call_stk_names[idx]);
    return ctx->call_stk_scopes[idx];
}

ks_obj ks_ctx_resolve(ks_ctx ctx, ks_str name) {
    int i;
    // check all the scopes
    for (i = ctx->call_stk_n - 1; i >= 0; --i) {
        ks_obj found = ks_dict_get(&ctx->call_stk_scopes[i]->locals, name);
        if (found != NULL) return found;
    }
    return NULL;
}

