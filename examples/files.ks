#!/usr/bin/env ks
""" files.ks

An overview of file I/O in kscript

This can be ran via the interpreter, in a number of ways:

$ ./bin/ks examples/files.ks
(if installed locally)

$ ./examples/files.ks
(if installed to somewhere on the $PATH)


"""

# use the builtin 'open' function to open a file
# by default, opens it for reading, and throws an error if there was a problem
f = open('README.md')

# it gives the builtin 'iostream' type
print ("type:", typeof(f))
assert typeof(f) == iostream && "Type was not iostream!"



# read the buffer (by default, the entire files)
s = f.read()
# it should be the entire file
assert len(s) == f.size() && "the length of the string was different than the size of the file!"

# count number of lines
print ("lines:", len(s.split('\n')))

# any further reads should be empty, since we are at the end of the file:
# the boolean conversion of 'f' should be whether the end has been reached
assert !f && !f.read() && "the file was not empty"

# reset, by seeking to 0
f.seek()

# assert it is truthy, which it should be
assert f && "the file was not valid after a reset!"



# basic iteration through characters
ct = 0
while f {
    s = f.read(1)
    ct = ct + 1
}

# these 2 should be the same, the number of characters in the iteration
#   should be equal to the size of the file
assert ct == f.size()
print ("ct:", ct, "size():", f.size())


import sys


print (sys.stdin.read())



