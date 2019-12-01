// scope.c - implementation of the scope functionality
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"


ks_scope ks_scope_new(ks_scope parent) {
    ks_scope ret = (ks_scope)malloc(sizeof(struct ks_scope));
    ret->locals = KS_DICT_EMPTY;
    ret->parent = parent;
    return ret;
}

void ks_scope_free(ks_scope scope) {
    ks_dict_free(&scope->locals);
    free(scope);
}

