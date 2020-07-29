#!/usr/bin/env ks
""" indexing.ks - simple examples in kscript showing how to index iterables & collections


Slices are very similar to python's slices

"""

# calculate first N fibonacci numbers
fibs = [1, 1]

while len(fibs) < 25, fibs.push(fibs[-1] + fibs[-2])
#while len(fibs) < 25, fibs.push(sum(fibs[-2:]))
assert len(fibs) == 25


print (fibs)

# every-other-one
print (fibs[::2])

# every-other-one, starting with fibs[1]
print (fibs[1::2])

# you can check that all the elements are there
#assert sort(fibs[::2] + fibs[1::2]) == fibs

# reverse the sequence
print (fibs[::-1])



