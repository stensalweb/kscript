#!/usr/bin/env ks
""" tests/strings.ks - testing out string functionalities

@author: Cade Brown <brown.cade@gmail.com>
"""

# basic equality
assert "a" == "a"
assert "abc" == "abc"

# case sensitivity
assert "a" != "A" 
assert "abc" != "ABC"
assert "abc" != "abC"

# length of various strings
assert len("a") == 1
assert len("abc") == 3

assert len("") == 0



# unicode
assert len("𝄞") == 1 && len("𝄞", 'bytes') == 4

assert "𝄞"[0] == "𝄞" && "𝄞" == chr(119070) && ord("𝄞") == 119070 && "𝄞" == chr(0x1D11E)

# escape sequences
assert "𝄞" == "\U0001D11E" && "𝄞" == "\N{MUSICAL SYMBOL G CLEF}"


# string interpolation
x, y = 2, 3
assert "2 ** 3 == 8" == $"{x} ** {y} == {x ** y}"
assert str(x) == $"{x}"
