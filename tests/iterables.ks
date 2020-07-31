#!/usr/bin/env ks
""" tests/iterables.ks - testing basic iterable functionality

@author: Cade Brown <brown.cade@gmail.com>
"""

x = [1, 2, 3]

# checking length
assert len(x) == 3
assert len(x) == x[-1]

# non-empty
assert x
assert [1,] && (1,)
assert [none,] && (none,)


# empty
assert ![] && !(,)


# count number of tries & errors (they should be equal!)
ct_try = 0
ct_err = 0


ct_try = ct_try + 1
try {
    [1, 2][2]
} catch e {
    ct_err = ct_err + 1
}

ct_try = ct_try + 1
try {
    [1, 2][100]
} catch e {
    ct_err = ct_err + 1
}


assert ct_try == ct_err



