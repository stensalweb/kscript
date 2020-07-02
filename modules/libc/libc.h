/* modules/libc/libc.h - C library header
 *
 * 
 * 'void' is considered 'none'
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef LIBC_H__
#define LIBC_H__

// include the main kscript API
#include <ks.h>



/* STANDARD TYPES */


typedef struct {
    KS_OBJ_BASE

    int val;

}* libc_int;

typedef struct {
    KS_OBJ_BASE

    long val;

}* libc_long;

typedef struct {
    KS_OBJ_BASE

    long long val;

}* libc_long_long;

typedef struct {
    KS_OBJ_BASE

    size_t val;

}* libc_size_t;

typedef struct {
    KS_OBJ_BASE

    // address of the pointer
    void* val;

    // NOTE: to get the type that the pointer is of, use `typeof(ptr_obj).of`

}* libc_pointer;


// declaring the types
extern ks_type libc_type_int, libc_type_long, libc_type_long_long, libc_type_size_t, libc_type_pointer;


// Create a libc_int
// NOTE: Returns a new reference
KS_API libc_int libc_make_int(int val);

// Create a pointer type
// NOTE: Returns a new reference
KS_API ks_type libc_make_pointer_type(ks_type of);

// Create a pointer
// NOTE: Returns a new reference
KS_API libc_pointer libc_make_pointer(ks_type of, void* addr);


// Return sizeof(of)
// Or, a negative value indicates error
KS_API ks_ssize_t libc_get_size(ks_type of);

KS_API void libc_init_types();


#endif /* LIBC_H__ */

