/* ks_impl.h - internal implementation header, only included by internal kscript files
 *
 * Don't include this! Just include `ks.h` for the officially supported API
 * 
 * This file may define things that conflict with other libraries naming schemes
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef KS_IMPL_H__
#define KS_IMPL_H__

#ifdef __cplusplus
extern "C" {
#endif


// for error printing
#include <errno.h>
#include <unistd.h>

// always include the main header
#include "ks.h"

// other standard defines
#include <math.h>
#include <errno.h>
#include <ctype.h>
#include <stddef.h>
#include <dlfcn.h>


// formatting colors
#define BOLD   "\033[1m"
#define RESET  "\033[0m"
#define WHITE  "\033[37m"
#define RED    "\033[31m"
#define YELLOW "\033[33m"

/* INTERNAL TYPE INITIALIZATION FUNCTIONS */
void ks_type_type_init();
void ks_type_none_init();
void ks_type_bool_init();
void ks_type_int_init();
void ks_type_float_init();
void ks_type_complex_init();
void ks_type_str_init();
void ks_type_tuple_init();
void ks_type_list_init();
void ks_type_dict_init();
void ks_type_Error_init();
void ks_type_kfunc_init();

void ks_type_code_init();
void ks_type_ast_init();
void ks_type_parser_init();
void ks_type_cfunc_init();
void ks_type_pfunc_init();
void ks_type_thread_init();
void ks_type_module_init();

void ks_util_init();

void ks_mem_init();

// init the logging system
void ks_log_init();

void ks_init_funcs();

// internal function to hash a length of bytes, using djb hash
static inline ks_hash_t ks_hash_bytes(int len, uint8_t* data) {

    // hold our result
    ks_hash_t res = 5381;

    int i;
    // do iterations of DJB: 
    for (i = 0; i < len; ++i) {
        res = (33 * res) + data[i];
    }

    // return out result, making sure it is never 0
    return res == 0 ? 1 : res;
}

#ifdef __cplusplus
}
#endif

#endif /* KS_IMPL_H__ */

