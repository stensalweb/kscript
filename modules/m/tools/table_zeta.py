#!/usr/local/env python3
""" tools/table_zeta.py - generates tables for helping to compute the Riemann Zeta function



References:
  * http://numbers.computation.free.fr/Constants/Miscellaneous/zetaevaluations.pdf


@author: Cade Brown <brown.cade@gmail.com>

"""

# std library
import math
from functools import lru_cache
import argparse

# import installed packages
try:
    import numpy as np

    import gmpy2
    from gmpy2 import mpfr, const_pi, exp, sqrt, gamma
except:
    raise Exception("Dependencies have not been installed! Please run `pip3 install numpy gmpy2`")


parser = argparse.ArgumentParser(formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=40))

parser.add_argument('--prec', help='Internal precision to use in all computations', default=256, type=int)

args = parser.parse_args()

# set precision to the requested one
gmpy2.get_context().precision = args.prec



