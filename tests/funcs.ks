#!/usr/bin/env ks
""" tests/funcs.ks - testing basic functional procedures

@author: Cade Brown <brown.cade@gmail.com>
"""

# basic squaring function
func sqr(x) {
    ret x ** 2
}

assert 4 == sqr(2)
assert 1 == sqr(-1)

# overloaded function, with default parameters
func mypow(x, y=1) {
    ret x ** y
} 


assert 4 == mypow(2, 2)
assert 2 == mypow(2)
assert 0 == mypow(0)
assert 1 == mypow(0, 0)



