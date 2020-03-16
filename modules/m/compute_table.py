#!/usr/local/env python3

"""

compute_table.py


"""

from math import factorial


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



N = 100
print (" --- Rieman Zeta Function Table (N=%i) ---" % (N, ))

for d_k in calc_zeta_table(N):
    print (str(d_k) + ", ")



