#!/usr/bin/env ks
""" indexing.ks - simple examples in kscript showing how to index iterables & collections

"""

x = {
    "name": "Cade",
    "mysvals": [1, 2, 3]
}

#print (x)


# create fibonacci numbers
fibs = [1, 1]
while fibs[-1] < 120, fibs.push(fibs[-1] + fibs[-2])

#print (fibs, len(fibs))

#print (fibs[-2:2:-2])



import nx


mat = nx.array([
    [1, 2, 3],
    [4, 5, 6],
    [7, 8, 9],
])

print (mat)

print (mat[1, 1])

topleft = mat[:2, :2]

print (topleft)


