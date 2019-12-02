
# design document

This document describes the design & implementation of various features of kscript, such as the bytecode, VM, symbol resolution, standard interfaces, etc


## Bytecode/VM

Code (by default) in kscript is executed as bytecode on a virtual machine, the `ks_bc` & `ks_ctx` types.

Bytecodes are essentially operations on the stack. In the future, I may consider making the virtual machine a stack + register machine (registers would be completely optional, only for performance for caching essentially).


### Stack v. Registers

Essentially, stack machines are (obviously) very easy to generate code for, and they don't have side effects, like register allocation. With registers, however, we can avoid the repeated look ups if we recognize what we re use. Take the following example:

```
std.print(A + " is good")
std.print(A + " is bad")
```

Direct translation to bytecode might yield:

```
# line 1
load "A"
const "is good"
load "add"
call 2
load "std"
attr "print"
call 1


# line 2
load "A"
const "is bad"
load "add"
call 2
load "std"
attr "print"
call 1
```

We're having to look up `A` and `std.print` twice each, which means this requires 14 total instructions. Now, if we were to use a register extension, we could have the following:

```
# load A and std.print
loadreg $0 "A"
loadreg $1 "std"
attrreg $1 "print"
loadreg $2 "add"

# line 1
pushreg $0
const "is good"
callreg $2 2
callreg $1 1

# line 2
pushreg $0
const "is good"
callreg $2 2
callreg $1 1

```

Although about the same number of instructions were made (we now have 12 generated), but the `pushreg` instruction is very fast compared to a `load`, since the `load` might have to traverse scopes, globals, modules, etc to find it, whereas it is basically cached in the `$0,$1,$2` registers.

Of course, all bytecode should be backwards compatible as much as I can make it, but this does support some interesting opportunities.

It would also make a lot of sense for dedicated typed-registers for fast floating point calculations, for instance.


### Text Representation

I've yet to formalize the bytecode, so I'm thinking it should also be writable on its own. Here's my examples of writing bytecode directly. It will be much like assembly, 1 instruction per line (some are pseudo instructions, like `jmp`, but will ultimately compile down to a single instruction):

```
# main.ksbc - bytecode for calculating primes
# 
# n = 30
# sieve = [true] * n
#
# for i = 2, i * i < n, i++ {
#   if sieve[i] {
#     for j = i * i, j < n, j += i {
#       sieve[j] = false
#     }
#   }
# }
# for i = 2, i < n, i++ {
#   if sieve[i] {
#     print(i)
#   }
# }
#


# n = 30
const 30
store "n"

# sieve = [true] * n
const true 
load "list"
call 1
load "n"
load "mul"
call 2
store "sieve"

# i = 2
const 2
store "i"

# sieve loop
loop:
    # if sieve[i]:
    load "i"
    load "sieve"
    attr "__get__"
    call 1
    beqf ~if.after

    if:
        # inner loop
        load "i"
        load "i"
        load "mul"
        call 2
        store "j"
        
        loop_inner:
            # sieve[j] = false
            load "j"
            const false
            load "sieve"
            attr "__set__"
            call 2

            # j += i
            load "j"
            load "i"
            load "add"
            call 2
            store "j"

            # j < n -> loop, else continue
            load "j"
            load "n"
            load "__lt__"
            call 2
            beqt ~loop_inner

    if.after:

    # i * i < n -> loop, else continue on
    load "i"
    load "i"
    load "mul"
    call 2
    load "n"
    load "__lt__"
    call 2
    beqt ~loop

const 2
store "i"

# print loop
loop_print:

    # if sieve[i]:
    load "i"
    load "sieve"
    attr "__get__"
    call 1
    beqf ~if2.after

    if2:
        load "i"
        load "print"
        call 1

    if2.after:

    # i++
    load "i"
    const 1
    load "add"
    call 2
    store "i"

    # i < n
    load "i"
    load "n"
    load "__lt__"
    call 2
    beqt ~loop_print

# done!
load "exit"
call 0

```

Obviously, there are some ways we could make this more efficient, but this is a direct, no-optimization compilation of the source code


