#!/usr/bin/env ks
""" tests/literals.ks - testing out syntax literals for values

@author: Cade Brown <brown.cade@gmail.com>
"""


# ensure zeros work
assert 0 == 0
assert 0x0 == 0
assert 0b0 == 0
assert 0o0 == 0

# check various conversion
assert 0x10 == 16
assert 0b10000 == 16
assert 0o20 == 16

# roman numerals
assert 0rI == 1
assert 0rV == 5
assert 0rL == 50
assert 0rC == 100
assert 0rD == 500
assert 0rM == 1000

assert 0rII == 2
assert 0rIII == 3
assert 0rIV == 4
assert 0rXVI == 16

# misc. powers of 2
assert 0xFF == 255
assert 0xFFFF == 65535
assert 0xFFFFFFFF == 4294967295
assert 0xFFFFFFFFFFFFFFFF == 18446744073709551615
