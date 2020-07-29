#!/usr/bin/env ks
""" tests/dict.ks - testing some basic dictionary functionality

@author: Cade Brown <brown.cade@gmail.com>
"""


# small example dictionary
x = {
    "first": "Cade",
    "last": "Brown",
}

assert x['first'] == 'Cade'
assert len(x) == 2

assert x.keys()[0] == 'first'
assert x['last'] < x['first']

#assert sort(x.keys()) == x.keys()
#assert sort(x.vals()) != x.vals()
#assert x.keys() != x.vals()

