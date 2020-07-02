#!/usr/bin/env ks
""" clibs.ks - example showing how to import C libraries and find symbols


"""

# in the standard library, which implements C library functions
import libc

# load a library handle (void* in C)
handle = libc.dlopen("./lib/libks.so", libc.RTLD_LOCAL | libc.RTLD_LAZY)
if !handle, throw Error(libc.dlerror())

# now, attempt to load a symbol
# NOTE: you can't just cast function symbol addresses to function types on some architectures
#   In the future, I plan to add `libffi` to help handle this issue
sym = libc.dlsym(handle, "strcpy")
if !sym, throw Error(libc.dlerror())

# print the address:
print (sym)
