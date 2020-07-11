#!/usr/bin/env ks
""" tests/basicmath.ks - testing basic math in kscript (i.e. not the 'm' module, just primitives and such)


@author: Cade Brown <brown.cade@gmail.com>
"""

# some basic properties
assert 0 == 0
assert 1 == 1
assert 1 != 0
assert 0 != 1
assert 1 > 0 && 0 < 1
assert 1 > -1 && -1 < 0

assert 1 == true


# edge cases around bit limits that should be made sure to
#   parse and output them back
edge_str_cases = [
    "4294967295",
    "-2147483648",
    "2147483647",
    "-2147483649",
    "2147483648",
]

for edge_str in edge_str_cases {
    assert edge_str == str(int(edge_str))
}


# some base 2 checks
assert 2147483647 + 1 == 2 ** 31
assert 9223372036854775807 + 1 == 2 ** 63
assert -9223372036854775808 == - (2 ** 63)
assert -9223372036854775809 == - (2 ** 63) - 1

# check some triangular number formulas
for j in range(5, 100, 7), assert sum(range(j)) == j * (j - 1) / 2

hadErr = false

try {
    x = 1 / 0
    assert false && "Division by 0 was not thrown!"
} catch e {
    # ensure it was a math error that was thrown
    assert typeof(e) == MathError
    hadErr = true
}

assert hadErr


assert int(true) == 1
assert int(false) != int(true)

assert !!1
assert !0
