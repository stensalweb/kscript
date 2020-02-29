/* err.c - error handling/global error stack

Essentially, errosr can be aded via `kse_addo, kse_add, kse_fmt`, etc

Then, other code can poll it via `kse_N()` to check how many errors, and 

*/

#include "ks.h"

// current error stack
static ks_list err_stk = NULL;

void* kse_addo(kso errmsg) {
    ks_list_push(err_stk, (kso)errmsg);
    return NULL;
}

// return NULL, always so it can be returned as an error code
void* kse_add(const char* errmsg) {

    // add it to the error stack
    kso new_str = (kso)ks_str_new(errmsg);
    ks_list_push(err_stk, new_str);
    KSO_DECREF(new_str);

    return NULL;
}

// adds a format string as an error
void* kse_fmt(const char* fmt, ...) {

    va_list ap;
    va_start(ap, fmt);
    ks_str filled = ks_str_new_vcfmt(fmt, ap);
    va_end(ap);

    // push it onto the error stack
    ks_list_push(err_stk, (kso)filled);

    KSO_DECREF(filled);

    return NULL;
}


// raise an error given a token, and a format, which will add additional information 
// including a source helper for the token
void* kse_tok(ks_tok tok, const char* fmt, ...) {

    // first, just compute the error string
    va_list ap;
    va_start(ap, fmt);
    ks_str errstr = ks_str_new_vcfmt(fmt, ap);
    va_end(ap);

    if (tok.v_parser != NULL && tok.len > 0) {
        // we have a valid token
        int i = tok.offset;
        int lineno = tok.line;
        char c;

        char* src = tok.v_parser->src->chr;

        // rewind to the start of the line
        while (i >= 0) {
            c = src[i];

            if (c == '\n') {
                i++;
                break;
            }
            i--;
        }


        if (i < 0) i++;
        if (src[i] == '\n') i++;

        // line start i
        int lsi = i;

        while (c = src[i]) {
            if (c == '\n' || c == '\0') break;
            i++;
        }

        // line length
        int ll = i - lsi;
        ks_str new_err_str = ks_str_new_cfmt("%S\n%*s\n%*c^%*c\n@ Line %i, Col %i, in '%*s'", 
            errstr, 
            ll, src + lsi,
            tok.col, ' ',
            tok.len - 1, '~',
            tok.line + 1, tok.col + 1, 
            tok.v_parser->src_name->len, tok.v_parser->src_name->chr
        );

        KSO_DECREF(errstr);
        errstr = new_err_str;

    }

    kse_addo((kso)errstr);
    KSO_DECREF(errstr);

    return NULL;
}



// number of errors
int kse_N() {
    return err_stk->len;
}

// pop off an error
kso kse_pop() {
    return err_stk->len > 0 ? ks_list_pop(err_stk) : NULL;
}


// dump out all errors, return if there were any
bool kse_dumpall() {
    int i;
    for (i = 0; i < err_stk->len; ++i) {
        kso erri = err_stk->items[i];

        ks_error("%S", erri);

    }
    
    ks_list_clear(err_stk);

    return i != 0;
}


// clears the error stack
bool kse_clear() {
    bool res = err_stk->len != 0;

    ks_list_clear(err_stk);

    return res;
}

// INTERNAL METHOD; DO NOT CALL
void kse_init() {
    err_stk = ks_list_new_empty();
}




