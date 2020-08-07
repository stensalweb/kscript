/* m/src/module.c - the kscript's standard math library
 *
 * In general, all `math.h` functions are exported equivalently to kscript
 * In some cases, functions are provided as multiple arguments ommitted with default, for example:
 *   m.log(x) is a natural logarithm, wheras m.log(x, 10) is log base 10
 * 
 * Most functions cast down to floats if neccessary; and handle complex floats specifically
 * 
 * 
 * Also, I have added the following functions for float & complex types:
 *   * gamma -> Gamma function (https://en.wikipedia.org/wiki/Gamma_function)
 *   * zeta -> Zeta function (https://en.wikipedia.org/wiki/Riemann_zeta_function)
 * 
 * 
 * See the comments by the Zeta/Gamma as well as the file `compute_table.py` for information on how
 *   these functions were implemented. In my testing, they are accurate to at least 14 digits in double precision,
 *   which is not bad, and is close to optimal, outside of using mpfr to do it
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


// always begin by defining the module information
#define MODULE_NAME "m"

// include this since this is a module.
#include "ks-module.h"

// include the C-style math functions
#include <math.h>
#include <complex.h>

/* Constant Defines */

// PI, 3.141...
#ifndef _KS_M_PI
#define _KS_M_PI KS_M_PI
#endif

// E, euler's number, 2.71828...
#ifndef _KS_M_E
#define _KS_M_E KS_M_E
#endif

// PHI, the golden ratio, 1.618
#ifndef _KS_M_PHI
#define _KS_M_PHI 1.618033988749894848204586834365638117720309179805762862135448622705260462818
#endif


// DEG2RAD, mulitplier to convert degrees to radians
#ifndef _KS_M_DEG2RAD
#define _KS_M_DEG2RAD (_KS_M_PI / 180.0)
#endif

// RAD2DEG, mulitplier to convert radians to degrees
// NOTE: `_KS_M_DEG2RAD * _KS_M_RAD2DEG` should be approximately equal to 1.0
#ifndef _KS_M_RAD2DEG
#define _KS_M_RAD2DEG (180.0 / _KS_M_PI)
#endif



/* Utility Functions */

// throw an argument error that some math domain error happened
static void* arg_error(char* argname, char* expr) {
    return ks_throw(ks_T_MathError, "Invalid argument '%s', requirement '%s' failed", argname, expr);
}


/* MT: Math Templates, to reduce code duplication */

// function taking a single float argument (named _par0)
#define _MT_F(_name, _par0, _func_f, ...) static KS_TFUNC(m, _name) {           \
    ks_obj __arg0;                                                              \
    KS_GETARGS(#_par0, &__arg0);                                                \
    { __VA_ARGS__; }                                                            \
    double c_arg0;                                                              \
    if (!ks_num_get_double(__arg0, &c_arg0)) return NULL;                       \
    return (ks_obj)ks_float_new(_func_f(c_arg0));                               \
}

// function taking two floating point arguments (named _par0 and _par1)
#define _MT_F_F(_name, _par0, _par1, _func_f, ...) static KS_TFUNC(m, _name) {  \
    ks_obj __arg0, __arg1;                                                      \
    KS_GETARGS(#_par0 " " #_par1, &__arg0, &__arg1);                            \
    { __VA_ARGS__; }                                                            \
    double c_arg0, c_arg1;                                                      \
    if (!ks_num_get_double(__arg0, &c_arg0)                                     \
    || !ks_num_get_double(__arg1, &c_arg1)) return NULL;                        \
    return (ks_obj)ks_float_new(_func_f(c_arg0, c_arg1));                       \
}


// function taking a single argument (named _par0), and can be either a float or complex
// Use `_func_f` and `_func_c` for float and complex arguments, respectively
// You can add `special cases` by just using the varargs
#define _MT_FC(_name, _par0, _func_f, _func_c, ...) static KS_TFUNC(m, _name) { \
    ks_obj __arg0;                                                              \
    KS_GETARGS(#_par0, &__arg0);                                                \
    { __VA_ARGS__; }                                                            \
    if (__arg0->type == ks_T_complex) {                                         \
        double complex c_arg0 = ((ks_complex)__arg0)->val;                      \
        return (ks_obj)ks_complex_new(_func_c(c_arg0));                         \
    } else {                                                                    \
        double c_arg0;                                                          \
        if (!ks_num_get_double(__arg0, &c_arg0)) return NULL;                   \
        return (ks_obj)ks_float_new(_func_f(c_arg0));                           \
    }                                                                           \
}

// function taking a two arguments (named _par0 & _par1), and can be either a float or complex
// Use `_func_f` and `_func_c` for float and complex arguments, respectively
// You can add `special cases` by just using the varargs
#define _MT_FC_FC(_name, _par0, _par1, _func_f, _func_c, ...) static KS_TFUNC(m, _name) { \
    ks_obj __arg0, __arg1;                                                      \
    KS_GETARGS(#_par0 " " #_par1, &__arg0, &__arg1);                            \
    { __VA_ARGS__; }                                                            \
    if (__arg0->type == ks_T_complex || __arg1->type == ks_T_complex) {         \
        double complex c_arg0, c_arg1;                                          \
        if (!ks_num_get_double_complex(__arg0, &c_arg0)                         \
        || !ks_num_get_double_complex(__arg1, &c_arg1)) return NULL;            \
        return (ks_obj)ks_complex_new(_func_c(c_arg0, c_arg1));                 \
    } else {                                                                    \
        double c_arg0, c_arg1;                                                  \
        if (!ks_num_get_double(__arg0, &c_arg0)                                 \
        || !ks_num_get_double(__arg1, &c_arg1)) return NULL;                    \
        return (ks_obj)ks_float_new(_func_f(c_arg0, c_arg1));                   \
    }                                                                           \
}


// m.floor(x) - compute integer by rounding towards -inf
_MT_F(floor, "x", floor, { })

// m.ceil(x) - compute integer by rounding towards -inf
_MT_F(ceil, "x", ceil, { })

// m.round(x) - compute nearest integer
_MT_F(round, "x", round, { })

// m.abs(x) - compute nearest integer
_MT_F(abs, "x", fabs, { if (__arg0->type == ks_T_complex) { return (ks_obj)ks_float_new(cabs(((ks_complex)__arg0)->val)); } })


// custom implementation to convert degrees to radians
static double my_rad(double degrees) {
    return _KS_M_DEG2RAD * degrees;
}

// custom implementation to convert radians to degrees
static double my_deg(double radians) {
    return _KS_M_RAD2DEG * radians;
}

// m.rad(degrees) - convert degrees to radians
_MT_F(rad, "degrees", my_rad, { })

// m.deg(radians) - convert radians to degrees
_MT_F(deg, "radians", my_deg, { })


// m.sin(x) - calculate the sin of x (in radians), or the extension to the complex plane
_MT_FC(sin, "x", sin, csin, { })

// m.cos(x) - calculate the cosine of x (in radians), or the extension to the complex plane
_MT_FC(cos, "x", cos, ccos, { })

// m.tan(x) - calculate the tangent of x (in radians), or the extension to the complex plane
_MT_FC(tan, "x", tan, ctan, { })


// m.asin(x) - calculate the inverse sin of x (in radians), or the extension to the complex plane
_MT_FC(asin, "x", asin, casin, { })

// m.acos(x) - calculate the inverse cosine of x (in radians), or the extension to the complex plane
_MT_FC(acos, "x", acos, cacos, { })

// m.atan(x) - calculate the inverse tangent of x (in radians), or the extension to the complex plane
_MT_FC(atan, "x", atan, catan, { })


// m.atan2(y, x) - calculate the arctangent of the given values; but correct for quadrants,
_MT_F_F(atan2, "y", "x", atan2, { })


// m.sinh(x) - calculate the hyperbolic sin of x, or the extension to the complex plane
_MT_FC(sinh, "x", sinh, csinh, { })

// m.cosh(x) - calculate the hyperbolic cosine of x, or the extension to the complex plane
_MT_FC(cosh, "x", cosh, ccosh, { })

// m.tanh(x) - calculate the hyperbolic tangent of x, or the extension to the complex plane
_MT_FC(tanh, "x", tanh, ctanh, { })


// m.asinh(x) - calculate the inverse hyperbolic sine of x, or the extension to the complex plane
_MT_FC(asinh, "x", asinh, casinh, { })

// m.acosh(x) - calculate the inverse hyperbolic cosine of x, or the extension to the complex plane
_MT_FC(acosh, "x", acosh, cacosh, { })

// m.atanh(x) - calculate the inverse hyperbolic tangent of x, or the extension to the complex plane
_MT_FC(atanh, "x", atanh, catanh, { })


// m.sqrt(x) - compute the square root of x
_MT_FC(sqrt, "x", sqrt, csqrt, {
    if (__arg0->type != ks_T_complex) {
        double x;
        if (!ks_num_get_double(__arg0, &x)) {
            ks_catch_ignore();
        } else {
            if (x < 0.0) return arg_error("x", "x >= 0");
        }
    }
})


// custom implementation of cube root for complex values
static double complex my_ccbrt(double complex val) {
    return cpow(val, 1.0 / 3.0);
}

// m.cbrt(x) - compute the cube root of x
_MT_FC(cbrt, "x", cbrt, my_ccbrt, { })

// m.hypot(x, y) - return the hypotenuse, i.e. sqrt(x ** 2 + y ** 2), or the norm of the vector `(x, y)`
_MT_F_F(hypot, "x", "y", hypot, { })


// m.exp(x) - compute the exponential function of x, i.e. `e**x`, where e is Euler's number (2.7...)
_MT_FC(exp, "x", exp, cexp, { })

// m.pow(x, y) - compute x ** y
_MT_FC_FC(pow, "x", "y", pow, cpow, { })


// m.log(x, base=m.E) - compute logarithm; by default base 'E'
static KS_TFUNC(m, log) {
    ks_obj x;
    ks_obj base = NULL;
    KS_GETARGS("x ?base", &x, &base);

    if (x->type == ks_T_complex || (base != NULL && base->type == ks_T_complex)) {
        double complex c_x;
        if (!ks_num_get_double_complex(x, &c_x)) return NULL;
        if (c_x == 0.0) return arg_error("x", "x != 0.0");
        if (base != NULL) {
            double complex c_base;
            if(!ks_num_get_double_complex(base, &c_base)) return NULL;
            if (c_base == 0.0) return arg_error("base", "base != 0.0");
            if (c_base == 1.0) return arg_error("base", "base != 1.0");
            return (ks_obj)ks_complex_new(clog(c_x) / clog(c_base));
        } else {
            // default to base E
            return (ks_obj)ks_complex_new(clog(c_x));
        }
    } else {
        double c_x;
        if (!ks_num_get_double(x, &c_x)) return NULL;
        if (c_x <= 0.0) return arg_error("x", "x > 0.0");
        if (base != NULL) {
            double c_base;
            if(!ks_num_get_double(base, &c_base)) return NULL;
            if (c_base <= 0.0) return arg_error("base", "base > 0.0");
            if (c_base == 1.0) return arg_error("base", "base != 1.0");
            return (ks_obj)ks_float_new(log(c_x) / log(c_base));
        } else {
            // default to base E
            return (ks_obj)ks_float_new(log(c_x));
        }
    }
}


// m.erf(x) - return the error function, defined as: 2/PI * integral[0, x]{ e**(-t**2) dt }
_MT_F(erf, "x", erf, { })

// m.erfc(x) - complimentary error function, defined as: 1 - erf(x)
_MT_F(erfc, "x", erfc, { })


// m.frexp(x) - decompose 'x' into a pair (m, e), where x == m *  2 ** e
static KS_TFUNC(m, frexp) {
    ks_obj x;
    KS_GETARGS("x", &x);
    double c_x;
    if (!ks_num_get_double(x, &c_x)) return NULL;

    // mantissa and exponent
    double m;
    int e;
    m = frexp(c_x, &e);

    // return as a tuple
    return (ks_obj)ks_tuple_new_n(2, (ks_obj[]){
        (ks_obj)ks_float_new(m),
        (ks_obj)ks_int_new(e),
    });

}


/* Custom Functions Implemented */


// generated in `my_gz.c`
double my_zeta(double x);
double complex my_czeta(double complex x);
double my_gamma(double x);
double complex my_cgamma(double complex x);
double my_lgamma(double x);
double complex my_clgamma(double complex x);


// m.zeta(x) - compute the Riemann Zeta evaluated at 'x'
_MT_FC(zeta, "x", my_zeta, my_czeta, { });

// m.gamma(x) - compute the Gammaevaluated at 'x'
_MT_FC(gamma, "x", my_gamma, my_cgamma, { });

// m.lgamma(x) - compute the logarithm of the gamma function
_MT_FC(lgamma, "x", my_lgamma, my_clgamma, { });




// now, export them all
static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_c(mod->attr, KS_KEYVALS(
        /* functions */
        {"floor",                  (ks_obj)ks_cfunc_new_c(m_floor_, "m.floor(x)")},
        {"ceil",                   (ks_obj)ks_cfunc_new_c(m_ceil_, "m.ceil(x)")},
        {"round",                  (ks_obj)ks_cfunc_new_c(m_round_, "m.round(x)")},
        {"frexp",                    (ks_obj)ks_cfunc_new_c(m_frexp_, "m.frexp(x)")},

        {"abs",                    (ks_obj)ks_cfunc_new_c(m_abs_, "m.abs(x)")},

        {"rad",                    (ks_obj)ks_cfunc_new_c(m_rad_, "m.rad(degrees)")},
        {"deg",                    (ks_obj)ks_cfunc_new_c(m_deg_, "m.deg(radians)")},

        {"sin",                    (ks_obj)ks_cfunc_new_c(m_sin_, "m.sin(x)")},
        {"cos",                    (ks_obj)ks_cfunc_new_c(m_cos_, "m.cos(x)")},
        {"tan",                    (ks_obj)ks_cfunc_new_c(m_tan_, "m.tan(x)")},

        {"asin",                   (ks_obj)ks_cfunc_new_c(m_asin_, "m.asin(x)")},
        {"acos",                   (ks_obj)ks_cfunc_new_c(m_acos_, "m.acos(x)")},
        {"atan",                   (ks_obj)ks_cfunc_new_c(m_atan_, "m.atan(x)")},
        {"atan2",                  (ks_obj)ks_cfunc_new_c(m_atan2_, "m.atan2(y, x)")},

        {"sinh",                   (ks_obj)ks_cfunc_new_c(m_sinh_, "m.sinh(x)")},
        {"cosh",                   (ks_obj)ks_cfunc_new_c(m_cosh_, "m.cosh(x)")},
        {"tanh",                   (ks_obj)ks_cfunc_new_c(m_tanh_, "m.tanh(x)")},

        {"asinh",                  (ks_obj)ks_cfunc_new_c(m_asinh_, "m.asinh(x)")},
        {"acosh",                  (ks_obj)ks_cfunc_new_c(m_acosh_, "m.acosh(x)")},
        {"atanh",                  (ks_obj)ks_cfunc_new_c(m_atanh_, "m.atanh(x)")},

        {"sqrt",                   (ks_obj)ks_cfunc_new_c(m_sqrt_, "m.sqrt(x)")},
        {"cbrt",                   (ks_obj)ks_cfunc_new_c(m_cbrt_, "m.cbrt(x)")},

        {"hypot",                  (ks_obj)ks_cfunc_new_c(m_hypot_, "m.hypot(x, y)")},


        {"exp",                    (ks_obj)ks_cfunc_new_c(m_exp_, "m.exp(x)")},
        {"pow",                    (ks_obj)ks_cfunc_new_c(m_pow_, "m.pow(x, y)")},

        {"log",                    (ks_obj)ks_cfunc_new_c(m_log_, "m.log(x, base=m.E)")},

        {"erf",                    (ks_obj)ks_cfunc_new_c(m_erf_, "m.erf(x)")},
        {"erfc",                   (ks_obj)ks_cfunc_new_c(m_erfc_, "m.erfc(x)")},


        {"zeta",                   (ks_obj)ks_cfunc_new_c(m_zeta_, "m.zeta(x)")},
        {"gamma",                  (ks_obj)ks_cfunc_new_c(m_gamma_, "m.gamma(x)")},

        {"lgamma",                 (ks_obj)ks_cfunc_new_c(m_lgamma_, "m.lgamma(x)")},


/*

        {"sign",                   (ks_obj)ks_cfunc_new_c(m_sign_, "m.sign(val)")},





        
        */

        /* constants */

        {"PI",                     (ks_obj)ks_float_new(_KS_M_PI)},
        {"E",                      (ks_obj)ks_float_new(_KS_M_E)},
        {"PHI",                    (ks_obj)ks_float_new(_KS_M_PHI)},



    ));

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
