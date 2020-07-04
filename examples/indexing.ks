#!/usr/bin/env ks
""" indexing.ks - simple examples in kscript showing how to index iterables & collections

"""

x = {
    "name": "Cade",
    "mysvals": [1, 2, 3]
}

print (x)


# create fibonacci numbers
fibs = [1, 1]
while fibs[-1] < 120, fibs.push(fibs[-1] + fibs[-2])

print (fibs)

print (fibs[slice(2, 5)])

