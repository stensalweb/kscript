#!/usr/local/env python3

"""

compute_table.py


"""

from math import factorial, sqrt, pi, gamma, exp


# calculate the 'd' series from the paper's Proposition #1, which can
#   be used in the other function
def calc_zeta_table(N):

    # compute term
    def d(k):
        # compute sum from Proposition #1
        res = 0

        for j in range(k, N+1):
            res += factorial(N + j - 1) * 4 ** j / (factorial(N - j) * factorial(2 * j))

        return N * res

    d0 = d(0)

    # calculate them all
    for k in range(1, N+1):
        yield d(k) / d0


# return factorial of a float
def ffact(x):
    return gamma(x + 1)


# calculate the 'p' series from here: https://en.wikipedia.org/wiki/Lanczos_approximation
#   be used in the other function
def calc_gamma_table(k, g):

    # compute the matrix coefficients for chebyshev polynomials
    def C(n, m):
        if n == 1 and m == 1: return 1
        elif n == 2 and m == 2: return 1
        elif m == 1: return -C(n - 2, 1)
        elif n == m: return 2 * C(n - 1, m - 1)
        else:
            return 2 * C(n-1, m-1) - C(n - 2, m)

    # compute term
    def p(k):
        # compute sum from Proposition #1
        res = 0

        for l in range(0, k+1):
            res += C(2 * k + 1, 2 * l + 1) * ffact(l - .5) * (l + g + 0.5) ** (-(l + 0.5)) * exp(l + g + 0.5)

        return sqrt(2) / pi * res

    def c(i):
        return p(i)


    # calculate them all
    for i in range(k):
        yield c(i)


N = 6

print (" --- Rieman Zeta Function Table (N=%i) ---" % (N, ))

for d_k in calc_zeta_table(N):
    print (str(d_k) + ", ")


"""

k = 9
g = 1.0

print (" --- Gamma Function Table (k=%i,g=%f) ---" % (k, g))

for p_k in calc_gamma_table(k, g):
    print (str(p_k) + ", ")
"""


