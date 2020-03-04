/* ks_impl.h - internal implementation header, only included by internal kscript files
 *
 * Don't include this! Just include `ks.h` for the officially supported API
 * 
 * This file may define things that conflict with other libraries naming schemes
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#ifndef KS_IMPL_H__
#define KS_IMPL_H__

// always include the main header
#include "ks.h"

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
void ks_type_str_init();
void ks_type_tuple_init();
void ks_type_list_init();
void ks_type_dict_init();
void ks_type_Error_init();

void ks_type_vm_init();
void ks_type_code_init();
void ks_type_ast_init();
void ks_type_parser_init();
void ks_type_cfunc_init();
void ks_type_pfunc_init();


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



#endif /* KS_IMPL_H__ */
