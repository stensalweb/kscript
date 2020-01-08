/* ks_bytecode.h - definition of bytecode instructions/format

## GENERAL BYTECODE FORMAT

In general, the bytecodes should be as short as possible, so that loops and functions always fit on a
single page. This is not hard to do, and I did a test of a 1000000 line long file with a pretty good 
operations/line density. It came out to 18MB of bytecode (using the full 4-byte int extra data, which
was unneccesary; all could have been condensed single byte values)

That means it is approximately 18 bytes/line, thus about 227 lines/page (assuming 4kb page).

Most functions are under this already, and so I'm not worrying about cache performance too much. However,
with single byte extra opcode, that would drop down to, I estimate, around 6 bytes/line, and thus 670
lines/page. That is very good indeed, so there is no need for cramming everything in ridiculously.

The general generators `ksc_*` will just output bytecode and append to a `ks_code` object. They are 
called with 4 byte integers, but internally will revert to single bytes if the opcode is small enough.


So, bytecode operations that take no arguments just take up one byte:

----------
| 1: opc |
----------
size: 1 byte

And most that take an argument <=255 will be reduced to an opcode and an amount:

-------------------
| 1: opc | 1: val |
-------------------
size: 2 bytes

And any with larger arguments will take up 4:

----------------------------------------------
| 1: opc | 4: val                            |
----------------------------------------------
size: 5 bytes

This is, of course, with the exception of the jmp commands, which always take 4 byte signed integers (for 
now). Having variable sized jumps can make it very difficult for the code generator.

### JUMPING BYTECODE

For example, if a jump is added, but then later needs to be resized, it has to re-link all the messed up 
jump targets between the jump and its destination. Eventually, I will probably do this (maybe add an 
optimizing step which can do it after the code generator has ran), but right now its not a huge priority.

In most cases, jumps will add an additional 3 bytes than are really neccessary, just so it is error-prone

*/

#pragma once
#ifndef KS_BYTECODE_H__
#define KS_BYTECODE_H__

// don't need to include `ks.h` here, since it doesn't rely on that

#include <stdint.h>

/* bytecode enumerations, detailing different instructions
This is also the main reference for the functionality of the bytecode, outside of the
  interprereter source code
Scheme: SIZE[TYPE description], SIZE in bytes of the section, TYPE being what it can be casted to
  and a short description of what that parameter does.

Opcodes themselves are always a single byte
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
    
    /* SETITEM - pops off `n_items` items, and performs a functor-like subscript=, with the first such object
        as the functor. So, `A B C D getitem(4)` yields `A[B, C] = D`. To call with no arguments, `n_items` should
        still be at least 1. 
    1[KSBC_SETITEM] 4[int n_items, number of items (including the functor) that the call should have]
    */
    KSBC_SETITEM,
    
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


    /** exception handling **/

    /* EXC_ADD - add an exception handler (useful for try/catch idiom), with an absolute position
    of the place to resume executing on the current code object
    1[KSBC_EXC_ADD] 4[int abs_pos of the exception handler in the local code object] */
    KSBC_EXC_ADD,
    
    /* EXC_REM - remove an exception handler (useful for try/catch idiom), from the top
    1[KSBC_EXC_REM] */
    KSBC_EXC_REM,


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

// instruction + a 1 byte unsigned integer
typedef struct {
    // the first byte, opcode, one of the KSBC_* enum values
    uint8_t op;

    // the unsigned, 8 bit integer representing the value
    uint8_t u8;

} ksbc_u8;


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

    // 1[opcode] 1[uint8_t u8]
    ksbc_u8 u8;

    // 1[opcode] 4[int i32]
    ksbc_i32 i32;

} ksbc;

/* inline function to skip over the current instruction, based on how long it is */
static inline uint8_t* ks_bc_next(uint8_t* bc) {
    // get instruction
    uint8_t ii = *bc;
    // check what argument it takes
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

