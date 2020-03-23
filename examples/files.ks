#!/usr/bin/ks

f = iostream("README.md")


print (f.seek(56).read(10))
