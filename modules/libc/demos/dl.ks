#!/usr/bin/env ks
""" dl.ks - example of dynamic symbol look up using the `clib` library


@author: Cade Brown <brown.cade@gmail.com>
"""

# C library bindings
import libc


# open the standard C library
handle = libc.dlopen("libc.so.6", libc.RTLD_LOCAL | libc.RTLD_LAZY)
if !handle, throw Error(libc.dlerror())

# load symbol
sym = libc.dlsym(handle, "puts")
if !sym, throw Error(libc.dlerror())


# create function pointer
puts = libc.function.create(libc.int, (libc.char_p,))(sym)
print (puts)

res = puts("My Example")

