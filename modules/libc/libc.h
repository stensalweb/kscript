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


// foreign function interface (FFI)
#ifdef KS_HAVE_FFI
#include <ffi.h>
#else
#warn Building 'libc' without FFI support
#endif



/* STANDARD TYPES */




typedef struct {
    KS_OBJ_BASE

    int8_t val;

}* libc_char;

typedef struct {
    KS_OBJ_BASE

    int16_t val;

}* libc_short;

typedef struct {
    KS_OBJ_BASE

    int32_t val;

}* libc_int;

typedef struct {
    KS_OBJ_BASE

    int64_t val;

}* libc_long;


// unsigned versions
typedef struct {
    KS_OBJ_BASE

    uint8_t val;

}* libc_uchar;

typedef struct {
    KS_OBJ_BASE

    uint16_t val;

}* libc_ushort;

typedef struct {
    KS_OBJ_BASE

    uint32_t val;

}* libc_uint;

typedef struct {
    KS_OBJ_BASE

    uint64_t val;

}* libc_ulong;


/* special types */



// void is special; like none
typedef struct {
    KS_OBJ_BASE

}* libc_void;



/* other types */

typedef struct {
    KS_OBJ_BASE

    // address of the pointer
    void* val;

    // NOTE: to get the type that the pointer is of, use `typeof(ptr_obj).of`

}* libc_pointer;


// function pointer meta structure
struct libc_fp_meta {
// foreign function interface
#ifdef KS_HAVE_FFI


    // main handle for the function
    ffi_cif _ffi_cif;

    // number of arguments it takes + return value,
    // so 1+n_args
    int _ffi_n;

    // _ffi_types[0] gives the result type,
    // _ffi_types[1:] gives the types for the arguments
    ffi_type** _ffi_types;

#endif
};


// function type
typedef struct {
    KS_OBJ_BASE

    // function
    void (*val)();

    // metadata (this is not managed by the instance, it is just a reference to the type)
    // so, this should not be freed
    struct libc_fp_meta* fp_meta;


}* libc_function;


// declaring the types
extern ks_type libc_type_void;
extern ks_type libc_type_char, libc_type_short, libc_type_int, libc_type_long;
extern ks_type libc_type_uchar, libc_type_ushort, libc_type_uint, libc_type_ulong;
extern ks_type libc_type_function, libc_type_pointer;


// common pointer types
extern ks_type libc_type_void_p;
extern ks_type libc_type_char_p, libc_type_short_p, libc_type_int_p, libc_type_long_p;
extern ks_type libc_type_uchar_p, libc_type_ushort_p, libc_type_uint_p, libc_type_ulong_p;

/* template types: */




// Create a pointer type
// NOTE: Returns a new reference
KS_API ks_type libc_make_pointer_type(ks_type of);

// Create a new function type
// n_args is the number of paramaters + returns (so +1)
// For example, int myfunc(float, char*) would have `n_args==3`, and argtypes:
//   (int, float, char_p)
// NOTE: Returns a new reference
KS_API ks_type libc_make_function_type(int n_args, ks_type* argtypes);





/* construct values */

// Create a libc_char
// NOTE: Returns a new reference
KS_API libc_char libc_make_char(int8_t val);

// Create a libc_short
// NOTE: Returns a new reference
KS_API libc_short libc_make_short(int16_t val);

// Create a libc_int
// NOTE: Returns a new reference
KS_API libc_int libc_make_int(int32_t val);

// Create a libc_long
// NOTE: Returns a new reference
KS_API libc_long libc_make_long(int64_t val);

// Create a libc_uchar
// NOTE: Returns a new reference
KS_API libc_uchar libc_make_uchar(uint8_t val);

// Create a libc_ushort
// NOTE: Returns a new reference
KS_API libc_ushort libc_make_ushort(uint16_t val);

// Create a libc_uint
// NOTE: Returns a new reference
KS_API libc_uint libc_make_uint(uint32_t val);

// Create a libc_ulong
// NOTE: Returns a new reference
KS_API libc_ulong libc_make_ulong(uint64_t val);



/* construct templated types */

// Creates a new libc_pointer, from a given pointer type, and C-style address
// NOTE: Returns a new reference
KS_API libc_pointer libc_make_pointer(ks_type ptr_type, void* val);

// Create a function pointer
// NOTE: Returns a new reference
KS_API libc_function libc_make_function(ks_type func_type, void (*val)());



// Return sizeof(of)
// Or, a negative value indicates error
KS_API ks_ssize_t libc_get_size(ks_type of);

KS_API void libc_init_types();


#endif /* LIBC_H__ */

