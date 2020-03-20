# `modules/m` - math module

This is the general purpose math library (similar to `math.h` and `-lm` in C), covers standard functions & algorithms

This is always built with kscript, and requires no outside libraries.


## Building

`m` doesn't require any special dependencies, so it should always be enabled, and built as part of the normal build process (set `KSM_STD='m ...' make)


## Exported Types

There are no new types exported by `m`


## Exported Functions

`m` includes most of the functions found in the C `math.h` library, as well as some custom implementations, namely of the [Gamma Function](https://en.wikipedia.org/wiki/Gamma_function), and the [Zeta Function](https://en.wikipedia.org/wiki/Riemann_zeta_function). These new implementations have tables generated (see `./compute_table.py` for the source) and approximations which are fairly accurate (in my tests, always well within `2^-40`, or 12 digits for most ranges).

Most functions operate on `int`, `float`, or `complex` arguments, up-casting as neccessary. Some functions (`rad`, `deg`, `round`, etc) only accept floats/ints. A `MathError` is raised if an argument is out of range, or takes on an invalid value.

In most cases, the result type is the same as the input type (notable exception: `abs` always returns a float). So, some functions (such as `sqrt`) will throw errors, even if a type conversion could solve the problem:

For example, `sqrt(-1)` will raise an error, but `sqrt(complex(-1))` will not, since there are no bounds checks of `sqrt()` of a complex number, where there are on a float.

