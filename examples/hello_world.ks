#!/usr/bin/env ks
""" hello_world.ks

The simplest example, how to print 'Hello World' out to the console

This can be ran via the interpreter, in a number of ways:

$ ./bin/ks examples/hello_world.ks
(if installed locally)

$ ./examples/hello_world.ks
(if installed to somewhere on the $PATH)

$ cat ./examples/hello_world.ks | ks
(via indirection)

"""

print ("Hello World")
