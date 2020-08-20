#!/usr/bin/env python3
""" tools/gen_gz.py - generate C code to compute Gamma and Zeta functions for real and complex values

Example:
$ ./tools/gen_gz.py --lgamma > src/my_gz.c

Originally developed for kscript's `m` library to supplement the C99 math library; it is free to use this script to generate
  code for any non-commercial uses. For any commercial uses, please contact the @author below

References:
  [0]: http://numbers.computation.free.fr/Constants/Miscellaneous/zetaevaluations.pdf
  [1]: https://en.wikipedia.org/wiki/Lanczos_approximation
  [2]: https://mrob.com/pub/ries/lanczos-gamma.html
  [3]: https://web.viu.ca/pughg/phdThesis/phdThesis.pdf
  [4]: https://my.fit.edu/~gabdo/gamma.txt

@author: Cade Brown <brown.cade@gmail.com>

"""

# std library
import sys
import math
import argparse

# for cached functions (may help performance)
from functools import lru_cache

# gmpy2: multiprecision
import gmpy2
from gmpy2 import mpfr, const_pi, exp, sqrt, log, log10

# add commandline arguments
parser = argparse.ArgumentParser(formatter_class=lambda prog: argparse.HelpFormatter(prog, max_help_position=40))

parser.add_argument('--prec', help='Internal precision to use in all computations (in bits)', default=1024, type=int)
parser.add_argument('--prefix', help='Prefix to the C style functions to generate (include the "_"!)', default="my_")
parser.add_argument('--lgamma', help='Whether or not to include an implementation of the `lgamma` function', action='store_true')

args = parser.parse_args()

# set precision to the requested one
gmpy2.get_context().precision = args.prec

# pi & e, but to full precision within gmpy2
pi = const_pi()
e = exp(1)

# factorial, product of all integers <= x
@lru_cache
def factorial(x):
    return math.factorial(x)

# double factorial, if n is even, product of all even numbers <= n, otherwise the product of all odd numbers <= n
@lru_cache
def double_factorial(n):
     if n <= 1:
         return 1
     else:
         return n * double_factorial(n - 2)

# n choose k, i.e. 
@lru_cache
def choose(n, k):
    if k < 0:
        return 0
    return math.comb(n, k)

# return the amount of digits that are accurate in a given error bound
# example: digits_accurate(.001) returns 3, since it is down to 3 places
def digits_accurate(x):
    return float(max([-log10(x), 0]))


# -- GAMMA --

# generates a table for the Gamma function, used for approximation
# returns (coefs, errbound())
@lru_cache
def get_gamma_table(n, g):

    # we need to generate the array of coefficients `a` such that:
    # Gamma(x) = (x + g + 0.5)^(x+0.5) / (e^(x+g-0.5)) * L_g(x)
    # L_g(x) = a[0] + sum(a[k] / (z + k) for k in range(1, N))

    # essentially, we construct some matrices from number-theoretic functions
    #   and we can generate the coefficients of the partial fraction terms `1 / (z + k)`
    # This greatly simplifies from the native `z(z-1).../((z+1)(z+2)...)` form, which
    #   would require a lot more operations to implement internally (see definition of Ag(z) on wikipedia)

    # calculate an element for the 'B' matrix (n x n)
    def getB(i, j):
        if i == 0:
            return 1
        elif i > 0 and j >= i:
            return (-1) ** (j - i) * choose(i + j - 1, j - i)
        else:
            return 0
    
    # calculate an element for the 'C' matrix (n x n)
    def getC(i, j):
        if i == j and i == 0:
            return mpfr(0.5)
        elif j > i:
            return 0
        else:
            # this is the closed form instead of calculating a sum via the 'S' symbol mentioned in some places
            return int((-1) ** (i - j) * 4 ** j * i * factorial(i + j - 1) / (factorial(i - j) * factorial(2 * j)))

    # calculate an element for the 'Dc' matrix (n x n)
    @lru_cache
    def getDc(i, j):
        if i != j:
            # it's a diagonal matrix, so return 0 for all non-diagonal elements
            return 0
        else:
            # otherwise, compute via the formula given
            return 2 * double_factorial(2 * i - 1)

    # calculate an element for the 'Dr' matrix (n x n)
    @lru_cache
    def getDr(i, j):
        # it's diagonal, so filter out non-diagonal efforts
        if i != j:
            return 0
        elif i == 0:
            return 1
        else:
            # guaranteed to be a integer, so cast it (so no precision is lost)
            return -int(factorial(2 * i) / (2 * factorial(i) * factorial(i - 1)))

    # generate matrices from their generator functions as 2D lists
    # NOTE: this obviously isn't very efficient, but it allows arbitrary precision elements,
    #   which numpy does not
    # these matrices are size <100, so it won't be that bad anyway
    B  = [[getB (i, j) for j in range(n)] for i in range(n)]
    C  = [[getC (i, j) for j in range(n)] for i in range(n)]
    Dc = [[getDc(i, j) for j in range(n)] for i in range(n)]
    Dr = [[getDr(i, j) for j in range(n)] for i in range(n)]

    # the `f` vector, defined as `F` but without the double rising factorial (which Dc has)
    # I left this in here instead of combining here to be more accurate to 
    #   the method given in 4
    f_gn = [sqrt(2) * (e / (2 * (i + g) + 1)) ** (i + 0.5) for i in range(n)]

    # multiply matrices X*Y*...
    def matmul(X, Y, *args):
        if args:
            return matmul(matmul(X, Y), *args)
        else:
            # nonrecursive
            assert len(X[0]) == len(Y)
            M, N, K = len(X), len(Y[0]), len(Y)

            # GEMM kernel (very inefficient; but doesn't matter due to AP floats & small matrix sizes)
            return [[sum(X[i][k] * Y[k][j] for k in range(K)) for j in range(N)] for i in range(M)]

    # normalization factor; we multiply everything by this so it is `pretty close` to 1.0
    W = exp(g) / sqrt(2 * pi) 

    # get the resulting coefficients
    # NOTE: we should get a column vector back, so return the 0th element of each row to get the coefficients
    a = list(map(lambda x: W * x[0], matmul(Dr, B, C, Dc, [[f_gn[i]] for i in range(n)])))

    # compute 'p' coefficients (only needed for the error bound function)
    p = [sum([getC(2 * j, 2 * j) * f_gn[j] * Dc[j][j] for j in range(i)]) for i in range(n)]

    # error bound; does not depend on 'x'
    def errbound():
        # given: err <= |pi/2*W*( sqrt(pi) - u*a )|
        # compute dot product `u * a`
        dot_ua = (a[0] + sum([2 * a[i] / mpfr(2 * i - 1) for i in range(1, n)])) / W
        # compute full formulat
        return abs(pi / 2 * W) * abs(sqrt(pi) - dot_ua)

    # return them
    return a, errbound

# -- ZETA --

# generates a zeta table which can be used for calculation
# returns (coefs, errbound(x)), which are the coefficients for the table as well as an error bound function
@lru_cache
def get_zeta_table(n):

    # the error bound of the approximation technique for a given input `x`
    def errbound(x):
        # absolute value of the imaginary component
        t = abs(complex(x).imag)

        # calculate error term
        et = (3 / (3 + sqrt(8)) ** n) * ((1 + 2 * t) * exp(t * pi / 2)) / (1 - 2 ** (1 - x))
        return abs(et)

    # compute `d_k`, from Proposition #1 in the paper
    def d(k):
        res = 0
        for j in range(k, n+1):
            num, den = factorial(n + j - 1) * 4 ** j, (factorial(n - j) * factorial(2 * j))
            res += mpfr(num) / den

        return n * res

    # d(0) is the value by which we normalize; this is more efficient
    #   and reduces loss of precision while doing arithmetic in C
    d_norm = d(0)

    # list of 'd's to sum later (normalized to d(0))
    d = [d(k) / d_norm for k in range(1, n + 1)]

    return d, errbound


# -- FUNCTION IMPLEMENTATIONS --
# NOTE: these are just examples; the C code we generate will be faster

# compute the Gamma function (given 'a' from `get_gamma_table`, and 'g' is what is was generated as)
def my_gamma(x, a, g):
    n = len(a)
    x = mpfr(x)

    # calculate sum of partial fractions
    ret = ret[0]
    for i in range(1, n):
        ret += x[i] / (x + i)
    
    # temporary variable
    tmp = x + g + 0.5
    return sqrt(2 * pi) * tmp ** (x + 0.5) * exp(-tmp) * ret

# compute the Zeta function (given 'd' from `get_zeta_table`)
def my_zeta(x, d):
    # the length should be the list of them
    n = len(d)
    x = mpfr(x)

    # calculate sum
    ret = 0
    for i in range(n):
        ret += ((-1) ** i) * d[i] * (i + 1) ** (-x)

    # return result
    return ret / (1 - 2 ** (1 - x))



# number of digits for accurate computation
goal_digits = 17.0


# -*- C Code Generation -*-

# prefix and upper prefix
prefix = args.prefix
uprefix = args.prefix.upper()

# return a string representing a float value in C
def cfloat(val):
    return "{0:> 50.44fl}l".format(val)

#newline, but we have to escape it for fstring
_n = "\n"

print(f"""/* gz.c - implementations of the Gamma and Zeta functions
 *
 * NOTE: DO NOT EDIT THIS FILE, it was generated via the `gen_gz.py` script, available here: https://github.com/ChemicalDevelopment/kscript/blob/master/modules/m/tools/gen_gz.py
 *
 * Generation Arguments:
 * $ {' '.join(sys.argv)}
 *
 *
 * To embed this in your own, compile it like so:
 * $ cc -std=c99 -Ofast -fno-math-errno my_gz.c -o my_gz.o
 * Then,
 * $ cc other_objs.o... my_gz.o -lm -o total.o
 *
 * The flags `-Ofast` and `-fno-math-errno` are not required; they are just mainly included to generate more efficient code
 *
 *
 * You can embed all this data & use these methods in any non-commercial project for free; for commercial projects,
 *   contact the @author below.
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// std includes
#include <complex.h>
#include <math.h>


/* Constants */

// PI, 3.1415... constant
#define {uprefix}PI             {cfloat(pi)}

// log(PI), a constant that should be precomputed
#define {uprefix}LOG_PI         {cfloat(log(pi))}

// sqrt(2 * PI), a constant that should be precomputed
#define {uprefix}SQRT_2PI       {cfloat(sqrt(2 * pi))}

// log(sqrt(2 * PI)), a constant that should be precomputed
#define {uprefix}LOG_SQRT_2PI   {cfloat(log(sqrt(2 * pi)))}


/* Forward Declarations */

// gamma(x) - compute the Gamma function evaluated at 'x'
double {prefix}gamma(double x);

// cgamma(x) - compute the Gamma function evaluated at 'x'
double complex {prefix}cgamma(double complex x);


// zeta(x) - compute the Riemann Zeta function evaluated at 'x'
double {prefix}zeta(double x);

// czeta(x) - compute the Riemann Zeta function evaluated at 'x'
double complex {prefix}czeta(double complex x);

""")

if args.lgamma:

    print(f"""

// lgamma(x) - compute the Gamma function evaluated at 'x'
double {prefix}lgamma(double x);

// clgamma(x) - compute the Gamma function evaluated at 'x'
double complex {prefix}lcgamma(double complex x);

""")


# -- Gamma Generation --

# magic constants from our references
# they say these are the best; we could search but this has already been found; just default to this
gamma_n = 15
gamma_g = 4.7421875

gamma_a, gamma_errbound = get_gamma_table(gamma_n, gamma_g)


print(f"""

/* -- GAMMA -- */

// NOTE: correct to {digits_accurate(gamma_errbound())} digits

double {prefix}gamma(double x) {{
    if (x <= 0) {{

        int ix = (int)x;
        //if (x == ix) return INFINITY;
        if (x == ix) return nan("");

        // use reflection formula, since it won't converge otherwise
        // Gamma(x) = pi / (sin(pi * x) * Gamma(1 - x))
        return {uprefix}PI / (sin({uprefix}PI * x) * {prefix}gamma(1 - x));
    }} else {{
        // shift off by 1 to make indexing cleaner
        x -= 1.0;

        // constant 
        static const double g = {gamma_g};

        // length (n) used
        static const int a_n = {gamma_n};

        // array of data
        static const long double a[] = {{
{_n.join([' ' * (3 * 4) + cfloat(gamma_a[i]) + "," for i in range(gamma_n)])}
        }};

        // keep track of sum
        long double sum = a[0];

        // loop varz
        int i;

        for (i = 1; i < a_n; ++i) {{
            sum += a[i] / (x + i);
        }}

        // temporary variable
        double tmp = x + g + 0.5;
        return {uprefix}SQRT_2PI * pow(tmp, x + 0.5) * exp(-tmp) * sum;
    }}
}}


double complex {prefix}cgamma(double complex x) {{
    double x_re = creal(x), x_im = cimag(x);

    // short circuit for real only
    if (x_im == 0.0) return {prefix}gamma(x_re);

    if (x_re < 0) {{
        // use reflection formula, since it won't converge otherwise
        // Gamma(x) = pi / (sin(pi * x) * Gamma(1 - x))
        return {uprefix}PI / (csin({uprefix}PI * x) * {prefix}cgamma(1 - x));
    }} else {{
        // shift off by 1 to make indexing cleaner
        x -= 1.0;

        // constant 
        static const double g = {gamma_g};

        // length (n) used
        static const int a_n = {gamma_n};

        // array of data
        static const long double a[] = {{
{_n.join([' ' * (3 * 4) + cfloat(gamma_a[i]) + "," for i in range(gamma_n)])}
        }};

        // keep track of sum
        long double complex sum = a[0];

        // loop var
        int i;

        for (i = 1; i < a_n; ++i) {{
            sum += a[i] / (x + i);
        }}

        // temporary variable
        double complex tmp = x + g + 0.5;
        return {uprefix}SQRT_2PI * pow(tmp, x + 0.5) * exp(-tmp) * sum;
    }}
}}

""")



if args.lgamma:
    print(f"""

double {prefix}lgamma(double x) {{
    if (x <= 0) {{

        int ix = (int)x;
        //if (x == ix) return INFINITY;
        if (x == ix) return nan("");
        
        // use reflection formula, since it won't converge otherwise
        // log(Gamma(x)) = log(pi / (sin(pi * x) * Gamma(1 - x)))
        // log(Gamma(x)) = log(pi) - log(sin(pi * x)) - log(Gamma(1 - x))
        return {uprefix}LOG_PI - log(sin({uprefix}PI * x)) - {prefix}lgamma(1 - x);
    }} else {{
        // shift off by 1 to make indexing cleaner
        x -= 1.0;

        // constant 
        static const double g = {gamma_g};

        // length (n) used
        static const int a_n = {gamma_n};

        // array of data
        static const long double a[] = {{
{_n.join([' ' * (3 * 4) + cfloat(gamma_a[i]) + "," for i in range(gamma_n)])}
        }};

        // keep track of sum
        long double sum = a[0];

        // loop var
        int i;

        for (i = 1; i < a_n; ++i) {{
            sum += a[i] / (x + i);
        }}

        // temporary variable
        double tmp = x + g + 0.5;
        // log(sqrt(2pi) * pow(tmp, (x + 0.5)) * exp(-tmp) * sum)
        // = log(sqrt(2pi)) + log(tmp) * (x + 0.5) - tmp + log(sum)
        return {uprefix}LOG_SQRT_2PI + log(sum) + log(tmp) * (x + 0.5) - tmp;
    }}
}}


double complex {prefix}clgamma(double complex x) {{
    double x_re = creal(x), x_im = cimag(x);

    // short circuit for real only
    if (x_im == 0.0) return {prefix}gamma(x_re);

    if (x_re < 0) {{
        // use reflection formula, since it won't converge otherwise
        // log(Gamma(x)) = log(pi / (sin(pi * x) * Gamma(1 - x)))
        // log(Gamma(x)) = log(pi) - log(sin(pi * x)) - log(Gamma(1 - x))

        return {uprefix}LOG_PI - clog(csin({uprefix}PI * x)) - {prefix}clgamma(1 - x);
    }} else {{
        // shift off by 1 to make indexing cleaner
        x -= 1.0;

        // constant 
        static const double g = {gamma_g};

        // length (n) used
        static const int a_n = {gamma_n};

        // array of data
        static const long double a[] = {{
{_n.join([' ' * (3 * 4) + cfloat(gamma_a[i]) + "," for i in range(gamma_n)])}
        }};

        // keep track of sum
        long double complex sum = a[0];

        // loop var
        int i;

        for (i = 1; i < a_n; ++i) {{
            sum += a[i] / (x + i);
        }}

        // temporary variable
        double complex tmp = x + g + 0.5;
        // log(sqrt(2pi) * pow(tmp, (x + 0.5)) * exp(-tmp) * sum)
        // = log(sqrt(2pi)) + log(tmp) * (x + 0.5) - tmp + log(sum)
        return {uprefix}LOG_SQRT_2PI + clog(sum) + clog(tmp) * (x + 0.5) - tmp;
    }}
}}


""")



# -- Zeta Generation --

# current table length
n = 4

# get table bounds
d, errbound = get_zeta_table(n)

val_r, val_i = 2.0, 0.0

while digits_accurate(errbound(val_r + val_i * 1j)) < goal_digits:
    n += 4
    d, errbound = get_zeta_table(n)


# the 'd's computed to be sufficient for real variables
real_d = d

# now, generate zeta functions
print(f"""

/* -- ZETA -- */

double {prefix}zeta(double x) {{
    if (x < 0) {{
        // check for negative even integers (which are exactly 0)
        int ix = (int)x;
        if (ix == x && ix % 2 == 0) return 0.0;

        // use reflection formula with the functional equation
        // Zeta(x) = 2 * (2*PI)^(x-1) * sin(x * PI/2) * Gamma(1-x) * Zeta(1 - x)
        return 2 * pow(2 *{uprefix}PI, x - 1) * sin(x * {uprefix}PI / 2) * {prefix}gamma(1 - x) * {prefix}zeta(1 - x);

    }} else {{
        // for x >= 0, summation with the coefficients will work fine

        // the 'n' used in computation
        static const int n = {len(d)};

        // d_k for k == 0 through n-1 (0-indexing)
        // NOTE: we bake in the alternating sign here, instead of at runtime (cheaper this way)
        static const long double d[] = {{
{_n.join([' ' * (3 * 4) + cfloat((-1) ** i * d[i]) + "," for i in range(len(d))])}
        }};

        // use `long double` will prevent some rounding errors
        long double sum = 0.0;

        int i;
        // compute elementwise sum
        for (i = 0; i < n; ++i) {{
            sum += d[i] * pow(i + 1, -x);
        }}

        // divide by the normalization factor in Proposition 1 (without d0, since everything has been normalized by that)
        sum /= 1 - pow(2, 1 - x);

        return (double)sum;
    }}
}}

""")


# dictiorary where `k, v` indicates:
# when imag(x) <= k, `v` may be used as the list of `d_k` for sufficient accuracy
imag_d_map = { }
# current table length
n = 4

# get table bounds
d, errbound = get_zeta_table(n)

# find tables for imag(x) <= 2 ** p
for p in range(0, 9):

    val_r = 2.0
    val_i = 2 ** p

    while digits_accurate(errbound(val_r + val_i * 1j)) < goal_digits:
        n += 4
        d, errbound = get_zeta_table(n)

    imag_d_map[val_i] = d


# ensure they were sorted (this will come in useful during code generation)
assert list(imag_d_map.keys()) == sorted(imag_d_map.keys()) and "Keys were not in sorted order"

#print (imag_d_map)
#print (my_zeta(2, get_zeta_table(24)[0]), pi ** 2 / 6)

# now, generate `my_zeta()` function
print(f"""

double complex {prefix}czeta(double complex x) {{
    // get real and imaginary components
    double x_re = creal(x), x_im = cimag(x);

    // use the real-only version of the function if it is a real argument
    if (x_im == 0.0) return {prefix}zeta(x_re);

    if (x_re < 0) {{

        // use reflection formula with the functional equation
        // Zeta(x) = 2 * (2*PI)^(x-1) * sin(x * PI/2) * Gamma(1-x) * Zeta(1 - x)
        return 2 * cpow(2 * {uprefix}PI, x - 1) * csin(x * {uprefix}PI / 2) * {prefix}cgamma(1 - x) * {prefix}czeta(1 - x);

    }} else {{
        // when the real component is >= 0, summation with the coefficients will work fine

        /* Generated Tables */
""")



# print out the tables
for i, imag_thresh in enumerate(imag_d_map):

    # get the coefficients
    d = imag_d_map[imag_thresh]

    print (f"""
        // the 'n' used in computation
        // NOTE: this table is useful for abs(imag(x)) <= {imag_thresh}
        static const int n_{i} = {len(d)};

        // d_k for k == 0 through n-1 (0-indexing)
        // NOTE: we bake in the alternating sign here, instead of at runtime (cheaper this way)
        static const long double d_{i}[] = {{
{_n.join([' ' * (3 * 4) + cfloat((-1) ** j * d[j]) + "," for j in range(len(d))])}
        }};
""")


# now, discriminate based on size
print (f"""
        // absolute value of the imaginary portion (used for discriminating amongst tables)
        double x_im_abs = fabs(x_im);

        // the sum of all elements in the series
        long double complex sum = 0.0;

        // tmp vars
        int i;

        if (0) {{ 
            // just used for easier code generation
""")


max_imag_thresh = max(imag_d_map.keys())

# iterate through maximum imaginary thresholds
for i, imag_thresh in enumerate(imag_d_map):

    if imag_thresh == max_imag_thresh:
        start =  "        } else {"
    else:
        start = f"        }} else if (x_im_abs <= {imag_thresh}) {{"


    print(f"""{start}
            for (i = 0; i < n_{i}; ++i) {{
                sum += d_{i}[i] * cpow(i + 1, -x);
            }}""")




# now, print end of the function
print (f"""
        }}

        // transform by the normalization factor
        sum /= 1 - cpow(2, 1 - x);

        return sum;
    }}
}}
""")

