/* kso.h - implementation of builtin objects/types */


#pragma once

#ifndef KSO_H__
#define KSO_H__

#include "ks.h"


/* constructing primitives */


// creates a new string from C-style format arguments, in va_list passing style
// NOTE: This is a custom implementation, there may be bugs.
// NOTE: This is implemented in `fmt.c`, rather than `kso.c`
ks_str ks_str_new_vcfmt(const char* fmt, va_list ap);

// creates a new string from C-style format arguments (i.e. only C-types are supported, not arbitrary kscript objects)
ks_str ks_str_new_cfmt(const char* fmt, ...);


// generates the bytecode for a given AST, returns the code object
// NOTE: this is implemented in codegen.c, rather than kso.c
ks_code ks_ast_codegen(ks_ast self, ks_list v_const);



/* GENERIC OBJECT FUNCTIONALITY */

// calls an object as if it was a function with a list of arguments
kso kso_call(kso func, int n_args, kso* args);

// returns the hash of the object
uint64_t kso_hash(kso obj);

// return whether or not the 2 objects are equal
bool kso_eq(kso A, kso B);

// returns the result of turning `A` into a boolean. It returns 0 if A->false, 1 if A->true, and
//   -1 if it could not be determined
// RULES:
//   if A==KSO_TRUE, ret 1
//   if A==KSO_FALSE, ret 0
//   if A==KSO_NONE, ret 0
//   if A is an int, ret 0 if A==0, 1 otherwise
//   if A is a string, ret 0 if len(A)==0, 1 otherwise
//   if A is a list, ret 0 if len(A)==0, 1 otherwise
//   if A is a tuple, ret 0 if len(A)==0, 1 otherwise
//   if A is a dict, ret 0 if it is empty, 1 otherwise
// TODO: also add a lookup function for custom types
// otherwise, return -1, because it could not be determined
int kso_bool(kso A);

// returns the tostring of the object
// if A is a string, just return A,
// if A is an int, none, bool, just return those strings
// if A's type defines a `f_str` method, call that and return the result
// else, return the string:
// <'%TYPE%' obj @ %ADDR%>
// where %TYPE% is the object's type's name
// and %ADDR% is the pointer formatted name (i.e. 0x238748237483)
ks_str kso_tostr(kso A);

// returns the string representation of an object
// if A is a string, return A with quotes around it
// if A is an int, none, bool, just return the same as tostr
// if A's type defines an `f_repr` method, call that and return the result
// else, return the string:
// else, return the string:
// <'%TYPE%' obj @ %ADDR%>
// where %TYPE% is the object's type's name
// and %ADDR% is the pointer formatted name (i.e. 0x238748237483)
ks_str kso_torepr(kso A);


// frees an object, returns true if successful, false otherwise
bool kso_free(kso obj);

#endif

