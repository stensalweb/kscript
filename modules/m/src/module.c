/* m/src/module.c - the kscript's standard math library
 *
 * In general, all `math.h` functions are exported equivalently to kscript
 * In some cases, functions are provided as multiple arguments ommitted with default, for example:
 *   m.log(x) is a natural logarithm, wheras m.log(x, 10) is log base 10
 * 
 * 
 * Functions:
 *   sin(rad): compute 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "m"

// include this since this is a module.
#include "ks-module.h"

// include the C-style math functions
#include <math.h>

#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.141592653589793238462643383279502884


// attempt to convert an object to a float, throwing an error if
static ks_float cvtFloat(ks_obj obj) {

    // try a couple of common results
    if (obj->type == ks_type_float) {
        return (ks_float)KS_NEWREF(obj);
    } else if (obj->type == ks_type_int) {
        return (ks_float)ks_float_new(((ks_int)obj)->val);
    }

    // could not convert float
    KS_ERR_CONV(obj, ks_type_float);
}


// throw an argument error
static void* arg_error(char* argname, char* expr) {
    return ks_throw_fmt(ks_type_MathError, "Invalid argument '%s', requirement '%s' failed!", argname, expr);
}

/* now, define our function that runs a given command */

// template for a math function taking 1 argument
// Extra arguments are ran after parsing, but before the cfunction is called
#define T_MATH_FUNC_1(_name, _cfunc, ...) static KS_TFUNC(m, _name) { \
    KS_REQ_N_ARGS(n_args, 1); \
    ks_float a0 = cvtFloat(args[0]); \
    if (!a0) return NULL; \
    { __VA_ARGS__; }; \
    double ret = _cfunc(a0->val); \
    KS_DECREF(a0); \
    return (ks_obj)ks_float_new(ret); \
}

// template for a math function taking 2 arguments
// Extra arguments are ran after parsing, but before the cfunction is called
#define T_MATH_FUNC_2(_name, _cfunc, ...) static KS_TFUNC(m, _name) { \
    KS_REQ_N_ARGS(n_args, 2); \
    ks_float a0 = cvtFloat(args[0]); \
    if (!a0) return NULL; \
    ks_float a1 = cvtFloat(args[1]); \
    if (!a1) return NULL; \
    { __VA_ARGS__; }; \
    double ret = _cfunc(a0->val, a1->val); \
    KS_DECREF(a0); \
    KS_DECREF(a1); \
    return (ks_obj)ks_float_new(ret); \
}


/* misc */

// sqrt(val) -> return the square root
T_MATH_FUNC_1(sqrt, sqrt, if (a0->val < 0.0) return arg_error("val", "val >= 0");)
// cbrt(val) -> return the cube root
T_MATH_FUNC_1(cbrt, cbrt)
// exp(val) -> return the exponential function
T_MATH_FUNC_1(exp, exp)



// calculate sign of a double
static double my_sign(double x) {
    
    // check for NaNs
    if (x != x) return x;

    if (x == 0) return 0;
    else if (x > 0) return +1;
    else return -1;
}

// convert degrees to radians
static double my_rad(double degrees) {
    return degrees * 0.01745329251994329576923690768488612713442871888541;
}

// convert radians to degrees
static double my_deg(double radians) {
    return radians * 57.2957795130823208767981548141051703324054724665643;
}


// sign(x) -> return +1 if the sign is positive, 0 if it is zero, or -1 otherwise
T_MATH_FUNC_1(sign, my_sign);



// deg(rad) -> turn radians into degrees
T_MATH_FUNC_1(deg, my_deg);

// rad(deg) -> turn degrees into radians
T_MATH_FUNC_1(rad, my_rad);



// return x**y
T_MATH_FUNC_2(pow, pow);
// atan2(y, x) -> return the phase given by the coordinate (x, y)
T_MATH_FUNC_2(atan2, atan2);

// hypot(x, y) -> return the hypoteneuse of a right triangle with sides 'x' and 'y'
T_MATH_FUNC_2(hypot, hypot);


/* analysis/stats */

// gamma(x) -> return the Gamma(x) function
T_MATH_FUNC_1(gamma, tgamma, if (a0->val <= 0 && floor(a0->val) == a0->val) { return arg_error("x", "!(x <= 0 && is_int(x))"); });
// lgamma(x) -> return log(|Gamma(x)|) function
T_MATH_FUNC_1(lgamma, lgamma);


/* trig functions */

// sin(rad) -> return the sin of a given radian measure
T_MATH_FUNC_1(sin, sin)
// cos(rad) -> return the cos of a given radian measure
T_MATH_FUNC_1(cos, cos)
// tan(rad) -> return the tan of a given radian measure
T_MATH_FUNC_1(tan, tan)

// asin(val) -> return the inverse sin of a given result
T_MATH_FUNC_1(asin, asin, if (fabs(a0->val) > 1) return arg_error("val", "|val| <= 1");)
// acos(val) -> return the inverse cos of a given result
T_MATH_FUNC_1(acos, acos, if (fabs(a0->val) > 1) return arg_error("val", "|val| <= 1");)
// atan(val) -> return the inverse tan of a given result
T_MATH_FUNC_1(atan, atan)


/* hyperbolic trig functions */

// sinh(rad) -> return the sin of a given radian measure
T_MATH_FUNC_1(sinh, sinh)
// cosh(rad) -> return the cos of a given radian measure
T_MATH_FUNC_1(cosh, cosh)
// tanh(rad) -> return the tan of a given radian measure
T_MATH_FUNC_1(tanh, tanh)

// asinh(val) -> return the inverse sin of a given value
T_MATH_FUNC_1(asinh, asinh)
// acosh(val) -> return the inverse cos of a given value
T_MATH_FUNC_1(acosh, acosh, if (a0->val < 1) return arg_error("val", "val >= 1");)
// atanh(val) -> return the inverse tan of a given value
T_MATH_FUNC_1(atanh, atanh, if (fabs(a0->val) > 1) return arg_error("val", "|val| <= 1");)


// logarithm
// Use an optional 'base' argument which defaults to 'E' (natural logarithm)
static KS_TFUNC(m, log) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_float val = cvtFloat(args[0]);
    if (!val) return NULL;

    double ret = 0.0;

    if (n_args == 2) {

        // logarithm of a given base
        ks_float base = cvtFloat(args[1]);
        if (!base) {
            KS_DECREF(val);
            return NULL;
        }
        ret = log(val->val) / log(base->val);

        KS_DECREF(base);
    } else {
        // natural logarithm
        ret = log(val->val);
    }
    KS_DECREF(val);

    return (ks_obj)ks_float_new(ret);
}



/* my additional functions */

/* Riemann Zeta Function : compute sum(i**-n for all n)
 *
 * We first use a transform via the Dirichlet function [3], and then apply a Chebyshev approximation
 *   to that function using methods in [3], and finally transform back.
 * 
 * For negative inputs, we use the functional equation satisfied with the gamma function
 * 
 * Resources:
 *   [0] https://en.wikipedia.org/wiki/Riemann_zeta_function
 *   [1] https://en.wikipedia.org/wiki/Dedekind_eta_function
 *   [2] https://www.ams.org/journals/mcom/1971-25-115/S0025-5718-1971-0295535-9/S0025-5718-1971-0295535-9.pdf
 *   [3] https://en.wikipedia.org/wiki/Dirichlet_eta_function
 *   [4] http://numbers.computation.free.fr/Constants/Miscellaneous/zetaevaluations.pdf
 *
 */


// Level 0 of approximation, using a series of length '10'
#define ZETA_C0_N 10

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C0[] = {
    0.9999999557904302, 
    0.9999911138764688, 
    0.9996993307157437, 
    0.9959645062584614, 
    0.9716881472861271, 
    0.8810564071227454, 
    0.6750751794786959, 
    0.38534334279256033, 
    0.13907128160934507, 
    0.023178546934890847,
};


// Level 1 of approximation, using a series of length '20'
#define ZETA_C1_N 20

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C1[] = {
    0.9999999999999991, 
    0.9999999999992174, 
    0.9999999998952387, 
    0.9999999944051655, 
    0.9999998410752649, 
    0.9999972242449596, 
    0.999967487536949, 
    0.9997295938728632, 
    0.9983379159379615, 
    0.9922254481454523, 
    0.9717004247158687, 
    0.9183886755481191, 
    0.8106062261437557, 
    0.640807413543651, 
    0.4332755314768562, 
    0.23862493864179354, 
    0.10127068966543885, 
    0.030757278426240712, 
    0.00590969541814232, 
    0.0005372450380129382, 
};


// Level 2 of approximation, using a series of length '50'
#define ZETA_C2_N 50

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C2[] = {
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    0.9999999999999992, 
    0.9999999999999855, 
    0.9999999999997845, 
    0.9999999999973079, 
    0.9999999999710746, 
    0.999999999730425, 
    0.9999999978052276, 
    0.9999999842921771, 
    0.9999999006381277, 
    0.9999994418278422, 
    0.999997203728888, 
    0.9999874611374572, 
    0.9999495073899707, 
    0.999816871155403, 
    0.9994002309671448, 
    0.998221949891754, 
    0.995218197716334, 
    0.9883095677128683, 
    0.9739656970704641, 
    0.9470770175272452, 
    0.9015752858835113, 
    0.8321038920345964, 
    0.7364957080523137, 
    0.6180557189399335, 
    0.48622686149311023, 
    0.35470746145813403, 
    0.2374550677949638, 
    0.14439369429808976, 
    0.07894393711347507, 
    0.038389941997185324, 
    0.016409456839304695, 
    0.006081277066324643, 
    0.0019217474833351403, 
    0.0005069858540111238, 
    0.00010852115541499642, 
    1.8090800979659343e-05, 
    2.2018711680970302e-06, 
    1.7394196846341903e-07, 
    6.690075710131501e-09, 
};

// Level 3 of approximation, using a series of length '100'
#define ZETA_C3_N 100

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C3[] = {
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    1.0, 
    0.9999999999999998, 
    0.9999999999999988, 
    0.999999999999993, 
    0.9999999999999574, 
    0.9999999999997549, 
    0.9999999999986675, 
    0.9999999999931668, 
    0.9999999999669134, 
    0.9999999998485969, 
    0.9999999993447323, 
    0.9999999973156857, 
    0.9999999895843686, 
    0.9999999616943196, 
    0.9999998663910856, 
    0.9999995577634031, 
    0.9999986101828937, 
    0.99999585077547, 
    0.9999882268563036, 
    0.9999682361593627, 
    0.9999184773841107, 
    0.9998008823336072, 
    0.9995369745223901, 
    0.9989744956476213, 
    0.9978358764921295, 
    0.9956467191066903, 
    0.991649207343195, 
    0.9847167929585939, 
    0.9733009445660133, 
    0.9554530810219558, 
    0.9289667461218057, 
    0.8916679217329002, 
    0.841841247930927, 
    0.7787233606145009, 
    0.702940642313668, 
    0.6167441134687979, 
    0.5239211711012182, 
    0.42934580279142776, 
    0.33824260709440934, 
    0.2553417113973208, 
    0.18414662336623058, 
    0.12650249303018543, 
    0.08255110400610076, 
    0.05103414156866193, 
    0.029809070587220453, 
    0.016406284492997324, 
    0.008484732145798469, 
    0.004111204279850167, 
    0.00186063382780824, 
    0.0007839096509715324, 
    0.00030633780692548184, 
    0.00011058820596877149, 
    3.671289757017618e-05, 
    1.1150303314652678e-05, 
    3.0798271282659686e-06, 
    7.682681520089493e-07, 
    1.7165348886328286e-07, 
    3.400940411108005e-08, 
    5.901756013071422e-09, 
    8.830582532874918e-10, 
    1.1160977525038991e-10, 
    1.1585711932630346e-11, 
    9.48336088954996e-13, 
    5.738980815359954e-14, 
    2.282612763371867e-15, 
    4.475711300729151e-17, 
};

// compute zeta function
double my_zeta(double x) {
    if (x < 0) {
        // use reflection formula with the functional equation
        // Zeta(x) = 2 * (2*PI)^(x-1) * sin(x * PI/2) * Gamma(1-x) * Zeta(1 - x)
        return 2 * pow(2 * M_PI, x - 1) * sin(x * M_PI / 2) * tgamma(1 - x) * my_zeta(1 - x);

    } else {

        // now, have cutoffs for different acuracies

        // the total sum
        double sum = 0;

        // the alterating sign, and the index
        int sign = 1, i;

        // compute term, using d_k which was precomputed
        // term = (-1)^i * d_i / (i+1)^x
        if (x > 50) {
            for (i = 0; i < ZETA_C0_N; ++i, sign *= -1) {
                sum += sign * zeta_C0[i] * pow(i + 1, -x);
            }
        } else if (x > 30) {
            for (i = 0; i < ZETA_C1_N; ++i, sign *= -1) {
                sum += sign * zeta_C1[i] * pow(i + 1, -x);
            }
        } else if (x > 12) {
            for (i = 0; i < ZETA_C2_N; ++i, sign *= -1) {
                sum += sign * zeta_C2[i] * pow(i + 1, -x);
            }
        } else {
            // fall back to most accurate
            for (i = 0; i < ZETA_C3_N; ++i, sign *= -1) {
                sum += sign * zeta_C3[i] * pow(i + 1, -x);
            }
        }

        // transform it back from the Eta function so that it is relevant to the Riemann Zeta function
        sum /= 1 - pow(2, 1 - x);

        return sum;
    }

}

// zeta(x) -> return the Riemann Zeta function of 'x'
T_MATH_FUNC_1(zeta, my_zeta);

static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* functions */
        {"sin", (ks_obj)ks_cfunc_new2(m_sin_, "m.sin(rad)")},
        {"cos", (ks_obj)ks_cfunc_new2(m_cos_, "m.cos(rad)")},
        {"tan", (ks_obj)ks_cfunc_new2(m_tan_, "m.tan(rad)")},

        {"asin", (ks_obj)ks_cfunc_new2(m_asin_, "m.asin(val)")},
        {"acos", (ks_obj)ks_cfunc_new2(m_acos_, "m.acos(val)")},
        {"atan", (ks_obj)ks_cfunc_new2(m_atan_, "m.atan(val)")},

        {"sinh", (ks_obj)ks_cfunc_new2(m_sinh_, "m.sinh(rad)")},
        {"cosh", (ks_obj)ks_cfunc_new2(m_cosh_, "m.cosh(rad)")},
        {"tanh", (ks_obj)ks_cfunc_new2(m_tanh_, "m.tanh(rad)")},

        {"asinh", (ks_obj)ks_cfunc_new2(m_asinh_, "m.asinh(val)")},
        {"acosh", (ks_obj)ks_cfunc_new2(m_acosh_, "m.acosh(val)")},
        {"atanh", (ks_obj)ks_cfunc_new2(m_atanh_, "m.atanh(val)")},

        {"log", (ks_obj)ks_cfunc_new2(m_log_, "m.log(val, base=E)")},

        {"sign", (ks_obj)ks_cfunc_new2(m_sign_, "m.sign(val)")},

        {"rad", (ks_obj)ks_cfunc_new2(m_rad_, "m.rad(degrees)")},
        {"deg", (ks_obj)ks_cfunc_new2(m_deg_, "m.deg(radians)")},

        {"sqrt", (ks_obj)ks_cfunc_new2(m_sqrt_, "m.sqrt(val)")},
        {"cbrt", (ks_obj)ks_cfunc_new2(m_cbrt_, "m.cbrt(val)")},
        {"exp", (ks_obj)ks_cfunc_new2(m_exp_, "m.exp(val)")},
        {"pow", (ks_obj)ks_cfunc_new2(m_pow_, "m.pow(x, y)")},
        {"hypot", (ks_obj)ks_cfunc_new2(m_hypot_, "m.hypot(x, y)")},

        {"gamma", (ks_obj)ks_cfunc_new2(m_gamma_, "m.gamma(x)")},
        {"lgamma", (ks_obj)ks_cfunc_new2(m_lgamma_, "m.lgamma(x)")},


        {"zeta", (ks_obj)ks_cfunc_new2(m_zeta_, "m.zeta(x)")},


        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
