/* ks_bytecode.h - definition of bytecode instructions/format


*/

#pragma once
#ifndef KS_BYTECODE_H__
#define KS_BYTECODE_H__

#include <stdint.h>

/* bytecode enumerations, detailing different instructions
This is also the main reference for the functionality of the bytecode, outside of the
  interprereter source code
Scheme: SIZE[TYPE description], SIZE in bytes of the section, TYPE being what it can be casted to
  and a short description of what that parameter does
*/
enum {

    /* NOOP - does nothing, changes nothing, just skips over the instruction
    1[KSBC_NOOP]
    */
    KSBC_NOOP = 0,

    /* CONST - used to push a constant/literal (which is stored in the `v_const` of the code)
        which comes from the 4 byte `int` value stored in the instruction itself
    1[KSBC_CONST] 4[int v_const_idx, index into the `v_const` list for which constant it is]
    */
    KSBC_CONST,

    /* CONST_TRUE - push on the constant 'true' (boolean), without using the extra 4 bytes 
    1[KSBC_CONST_TRUE]
    */
    KSBC_CONST_TRUE,
    /* CONST_FALSE - push on the constant 'false' (boolean), without using the extra 4 bytes 
    1[KSBC_CONST_FALSE]
    */
    KSBC_CONST_FALSE,
    /* CONST_NONE - push on the constant 'none' (none-type), without using the extra 4 bytes 
    1[KSBC_CONST_NONE]
    */
    KSBC_CONST_NONE,

    /* POPU - pops off a value from the stack, which is unused (i.e. will remove the stack's ref)
        and possibly free the object, if the refcnt reaches 0
    1[KSBC_POP]
    */
    KSBC_POPU,

    /* LOAD - reads an index into the `v_const` list, and then treats that as a key to the current VM state,
        looking up a value by that key. This is the generic lookup function that will search locals/globals
    1[KSBC_LOAD] 4[int v_const_idx, the name of the object to load]
    */
    KSBC_LOAD,
    /* LOAD_A - pop off the top object, and replace it with its attr(v_const[idx]), where idx is encoded
    1[KSBC_LOAD_A] 4[int v_const_idx, the name of the attribute to look up */
    KSBC_LOAD_A,


    /* STORE - reads an index into the `v_const` list, and then treats that as a key to the current VM state,
        looking up a value by that key, and setting its entry to the top of the stack. This is the generic
        setting function to locals. NOTE: The top object is not popped off, so it should be followed by a
        `KSBC_POPU` if its not used subsequently
    1[KSBC_STORE] 4[int v_const_idx, the name of the object to load]
    */
    KSBC_STORE,
    /* STORE_A - pops off a value, then an object, and sets the objects attr(v_const[idx])=value, leaving the
        value on the stack
    1[KSBC_STORE_A] 4[int v_const_idx, the name of the attribute to set]
    */
    KSBC_STORE_A,


    /* CALL - pops off `n_items` items, and performs a functor-like call, with the first such object
        as the functor. So, `A B C call(3)` yields `A(B, C)`. To call with no arguments, `n_items` should
        still be at least 1. 
    1[KSBC_CALL] 4[int n_items, number of items (including the functor) that the call should have]
    */
    KSBC_CALL,
    
    /* GETITEM - pops off `n_items` items, and performs a functor-like subscript, with the first such object
        as the functor. So, `A B C getitem(3)` yields `A[B, C]`. To call with no arguments, `n_items` should
        still be at least 1. 
    1[KSBC_GETITEM] 4[int n_items, number of items (including the functor) that the call should have]
    */
    KSBC_GETITEM,
    
    /* TUPLE - pops off `n_items` items, and turns them into a tuple, pushing on the result
    1[KSBC_TUPLE] 4[int n_items, number of items in the tuple]
    */
    KSBC_TUPLE,

    /* LIST - pops off `n_items` items, and turns them into a list, pushing on the result
    1[KSBC_LIST] 4[int n_items, number of items in the list]
    */
    KSBC_LIST,

    
    /** binary operators **/

    /* ADD - pops off 2 objects, binary-adds them
    1[KSBC_ADD] */
    KSBC_ADD,
    /* SUB - pops off 2 objects, binary-subtracts them
    1[KSBC_SUB] */
    KSBC_SUB,
    /* MUL - pops off 2 objects, binary-muliplies them
    1[KSBC_MUL] */
    KSBC_MUL,
    /* DIV - pops off 2 objects, binary-divides them
    1[KSBC_DIV] */
    KSBC_DIV,
    /* MOD - pops off 2 objects, binary-modulos them
    1[KSBC_MOD] */
    KSBC_MOD,
    /* POW - pops off 2 objects, binary-exponentiates them
    1[KSBC_POW] */
    KSBC_POW,

    /* LT - pops off 2 objects, binary-less-than compares them
    1[KSBC_LT] */
    KSBC_LT,
    /* LE - pops off 2 objects, binary-less-than-equal-to compares them
    1[KSBC_LE] */
    KSBC_LE,
    /* GT - pops off 2 objects, binary-greater-than compares them
    1[KSBC_DIV] */
    KSBC_GT,
    /* GE - pops off 2 objects, binary-greater-than-equal-to compares them
    1[KSBC_GE] */
    KSBC_GE,
    /* EQ - pops off 2 objects, binary-equality checks them
    1[KSBC_EQ] */
    KSBC_EQ,
    /* NE - pops off 2 objects, binary-not-equality checks them
    1[KSBC_EQ] */
    KSBC_NE,

    /** jumps/branches **/
    
    /* JMP - unconditionally jumps ahead `relamt` bytes in the bytecode
    1[KSBC_JMP] 4[int relamt] */
    KSBC_JMP,

    /* JMPT - pops off a value from the stack, jumps ahead `relamt` bytes in the bytecode
        if the value was exactly `ks_V_true`, otherwise jump ahead 0
    1[KSBC_JMPT] 4[int relamt] */
    KSBC_JMPT,

    /* JMPF - pops off a value from the stack, jumps ahead `relamt` bytes in the bytecode
        if the value was exactly `ks_V_false`, otherwise jump ahead 0
    1[KSBC_JMPF] 4[int relamt] */
    KSBC_JMPF,


    /** return/higher order control flow **/

    /* RET - return the last value on the stack as the function result
    1[KSBC_RET] */
    KSBC_RET,

    /* RET_NONE - return a new none (i.e. does not touch the stack) as the function result
    1[KSBC_RET_NONE] */
    KSBC_RET_NONE,


    // phony enum value to denote the end
    KSBC__END
};


/* structure definitions */

// if the compiler supports it, pack to a single byte, to minimize space
#pragma pack(push, 1)

// instruction only, for 1 byte instructions
typedef struct {
    // the first byte, always the opcode, one of the KSBC_* enum values
    uint8_t op;

} ksbc_;

// instruction + a 4 byte signed integer
typedef struct {
    // the first byte, opcode, one of the KSBC_* enum values
    uint8_t op;

    // the signed, 32 bit integer encoded with the instruction
    int32_t i32;

} ksbc_i32;


// the generic bytecode object which can hold all of them
typedef union {

    // opcode only
    ksbc_ _;

    // 1[opcode] 4[int i32]
    ksbc_i32 i32;

} ksbc;


/* inline function to skip over the current instruction, based on how long it is */
static inline uint8_t* ks_bc_next(uint8_t* bc) {
    // get instruction
    uint8_t ii = *bc;
    // handle operators
    /**/ if (KSBC_JMP <= ii && ii <= KSBC_JMPF) return bc + sizeof(ksbc_i32);
    else if (KSBC_LOAD <= ii && ii <= KSBC_LIST) return bc + sizeof(ksbc_i32);
    else if (KSBC_CONST == ii) return bc + sizeof(ksbc_i32);
    else {
        return bc + sizeof(ksbc_);
    }
}

// stop our single byte alignment
#pragma pack(pop)

#endif

