#!/usr/local/env python3
""" compute_table.py - compute lookup tables for Chebyshev approximations (as well as Lanczos approximations)

For required modules: run `pip3 install -r compute_table_requirements.txt`

On some platforms, gmpy2 might require something like `sudo apt install libgmp-dev libmpfr-dev libmpc-dev`


Essentially, this file generates arrays of float constants that can be used 

Right now, I only have implemented the 'Gamma' and the 'Zeta' function using these approximations, so that is what
  I have written


 -- GAMMA FUNCTION --

The Gamma function can be tricky to compute



 -- ZETA FUNCTION --

The Zeta function can be approximated with transforming to a Eta function, and also by
  using Chebyshev error approximation polynomials.

You can read the paper that I implemented this from here: http://numbers.computation.free.fr/Constants/Miscellaneous/zetaevaluations.pdf



"""

import gmpy2
import numpy as np
from gmpy2 import mpfr, const_pi, exp, sqrt, gamma
import math
from functools import lru_cache


# set to a lot of bits
gmpy2.get_context().precision = 160

pi = const_pi()

def factorial(x):
    return gmpy2.gamma(x + 1)


def double_factorial(n):
     if n <= 0:
         return 1
     else:
         return n * double_factorial(n - 2)



# print out a zeta table of a given size
def do_Zeta(N):

    # calculate the error
    def err(s):
        # error only depends on imaginary component 
        t = abs(complex(s).imag)
        # calculate error term
        et = (3 / (3 + sqrt(8)) ** N) * ((1 + 2 * t) * exp(t * pi / 2)) / (1 - 2 ** (1 - t))
        return abs(et)

    # compute term
    def d(k):
        # compute sum from Proposition #1
        res = 0

        for j in range(k, N+1):
            res += factorial(N + j - 1) * 4 ** j / (factorial(N - j) * factorial(2 * j))

        return N * res

    d0 = d(0)

    print (" --- Rieman Zeta Function Table (N=%i) ---" % (N, ))

    # calculate them all
    for k in range(1, N+1):
        d_k = d(k) / d0
        print ("%-60s," % (d_k,))

    print ("   - Correct Digits: -")

    # real component does not change accuracy
    cr = 0
    for ci in range(0, 100, 5):
        c = cr + ci * 1j
        # number of correct digits
        digs = -math.log(float(err(c)), 10)
        if digs >= 13:
            print ("at %s, %.2f digits" % (c, digs))


    print ("")

# create a gamma function table
def do_Gamma(N, g):

    # not working currently

    # partial fractions of coefficient terms
    partial_fracs = [
        [0.5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, 2, -6, 0, 0, 0, 0, 0, 0, 0, 0],
        [1, -3, 24, -30, 0, 0, 0, 0, 0, 0, 0],
        [1, 4, -60, 180, -140, 0, 0, 0, 0, 0, 0],
        [1, -5, 120, -630, 1120, -630, 0, 0, 0, 0, 0],
        [1, 6, -210, 1680, -5040, 6300, -2772, 0, 0, 0, 0],
        [1, -7, 336, -3780, 16800, -34650, 33264, -12012, 0, 0, 0],
        [1, 8, -504, 7560, -46200, 138600, -216216, 168168, -51480, 0, 0],
        [1, -9, 720, -13860, 110880, -450450, 1009008, -1261260, 823680, -218790, 0],
        [1, 10, -990, 23760, -240240, 1261260, -3783780, 6726720, -7001280, 3938220, -923780]
    ]

    # max size
    assert N <= len(partial_fracs)

    # compute Chebyshev polynomial coefficient in a matrix
    @lru_cache()
    def C(i, j):
        # recursively calculate
        if i > j and j >= 2:
            return 2 * C(i - 1, j - 1) - C(i - 2, j)
        elif i >= 3 and j == i:
            return 2 * C(i - 1, j - 1)
        elif i >= 3 and j == 1:
            return -C(i - 2, 1)
        else:
            return 1

    # compute term
    def p(k):
        # compute sums
        res = 0
        for a in range(0, k+1):
            Cij = C(2 * k + 1, 2 * a + 1)
            res += Cij * factorial(a - 0.5) * (a + g + 0.5) ** (-(a + 0.5)) * exp(a + g + 0.5)

        return (sqrt(2.0) / pi) * res

    # generate final coefficient
    def c(idx):
        res = 0
        for i in range(len(partial_fracs[0])):
            res += partial_fracs[i][idx] * p(i)
        return res

    print (" --- Gamma Function Table (N=%i,g=%f) ---" % (N, g))

    # calculate them all
    for i in range(N):
        c_i = c(i)
        print ("%.60s," % (c_i,))

    print ("")

# do some basic zeta tables, for various accuracies required

do_Zeta(18)
do_Zeta(29)

do_Zeta(65)
do_Zeta(112)

#do_Gamma(7, 5)
#do_Gamma(15, 4.7421875)

