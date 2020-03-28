#!/usr/bin/ks
"""
"""

f = open('README.md')

print (f.read())

print (f.seek().read(8))

f.open('configure')

print (f.read())
