/* m/src/module.c - the kscript's standard math library
 *
 * In general, all `math.h` functions are exported equivalently to kscript
 * In some cases, functions are provided as multiple arguments ommitted with default, for example:
 *   m.log(x) is a natural logarithm, wheras m.log(x, 10) is log base 10
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

#ifdef M_PI
#undef M_PI
#endif
#define M_PI 3.141592653589793238462643383279502884

enum {

    TY_NONE = 0,

    // cast to an integer
    TY_INT,

    // cast to a float
    TY_FLOAT,

    // cast to a complex
    TY_COMPLEX

};

// convert to a numeric of at least 'minTY', see TY_* macros
static ks_obj cvtNum(ks_obj obj, int minTY) {
    // fails conversion if not a numeric
    if (obj->type != ks_type_int && obj->type != ks_type_float && obj->type != ks_type_complex) {
        return NULL;
    }
    if (minTY == TY_INT) {
        // keep everything as it is
        return KS_NEWREF(obj);
    } else if (minTY == TY_FLOAT) {
        if (obj->type == ks_type_int) {
            double out;
            if (!ks_num_get_double(obj, &out)) return NULL;
            return (ks_obj)ks_float_new(out);
        }
        return KS_NEWREF(obj);
    } else if (minTY == TY_COMPLEX) {
        if (obj->type == ks_type_int) {
            double complex out;
            if (!ks_num_get_double_complex(obj, &out)) return NULL;
            return (ks_obj)ks_complex_new(out);
        } else if (obj->type == ks_type_float) {
            return (ks_obj)ks_complex_new(((ks_float)obj)->val);
        }

        return KS_NEWREF(obj);

    } else {
        assert(false && "cvtNum() given invalid 'minTY'!");
        return NULL;
    }
}

// throw a type conversion error, _types should be a string representing the requested types
#define _TY_ERR(_a, _types) { \
    return ks_throw(ks_type_TypeError, "Could not convert '%T' object to a valid type: %s", _a, _types); \
}


// throw an argument error that some math domain error happened
static void* arg_error(char* argname, char* expr) {
    return ks_throw(ks_type_MathError, "Invalid argument '%s', requirement '%s' failed!", argname, expr);
}

/* now, define our function that runs a given command */


// 1 argument, float, complex
#define T_M_F_1fc(_name, _cfunc_f, _cfunc_c, ...) static KS_TFUNC(m, _name) { \
    KS_REQ_N_ARGS(n_args, 1); \
    ks_obj a0 = cvtNum(args[0], TY_FLOAT); \
    if (!a0) _TY_ERR(args[0], "float, complex"); \
    { __VA_ARGS__; }; \
    /**/ if (a0->type == ks_type_float) { \
        ks_obj ret = (ks_obj)ks_float_new(_cfunc_f(((ks_float)a0)->val)); \
        KS_DECREF(a0); \
        return ret; \
    } else if (a0->type == ks_type_complex) { \
        ks_obj ret = (ks_obj)ks_complex_new(_cfunc_c(((ks_complex)a0)->val)); \
        KS_DECREF(a0); \
        return ret; \
    } \
    KS_DECREF(a0); \
    _TY_ERR(args[0], "float, complex"); \
}

// 1 argument, float
#define T_M_F_1f(_name, _cfunc_f, ...) static KS_TFUNC(m, _name) { \
    KS_REQ_N_ARGS(n_args, 1); \
    ks_obj a0 = cvtNum(args[0], TY_FLOAT); \
    if (!a0) _TY_ERR(args[0], "float"); \
    if (a0->type == ks_type_complex) { \
        KS_DECREF(a0); \
        _TY_ERR(args[0], "float"); \
    } \
    { __VA_ARGS__; }; \
    ks_obj ret = (ks_obj)ks_float_new(_cfunc_f(((ks_float)a0)->val)); \
    KS_DECREF(a0); \
    return ret; \
}


// template for a function taking exactly 2 arguments, only floats
#define T_M_F_2f(_name, _cfunc_f, ...) static KS_TFUNC(m, _name) { \
    KS_REQ_N_ARGS(n_args, 2); \
    if (args[0]->type == ks_type_complex) _TY_ERR(args[0], "float"); \
    if (args[1]->type == ks_type_complex) _TY_ERR(args[1], "float"); \
    ks_obj a0 = cvtNum(args[0], TY_FLOAT); \
    if (!a0) _TY_ERR(args[0], "float"); \
    ks_obj a1 = cvtNum(args[1], TY_FLOAT); \
    if (!a1) { KS_DECREF(a0); _TY_ERR(args[1], "float"); } \
    { __VA_ARGS__; }; \
    ks_obj ret = (ks_obj)ks_float_new(_cfunc_f(((ks_float)a0)->val, ((ks_float)a1)->val)); \
    KS_DECREF(a0); KS_DECREF(a1); \
    return ret; \
}

// template for a function taking exactly 2 arguments, floats or complex
#define T_M_F_2fc(_name, _cfunc_f, _cfunc_c, ...) static KS_TFUNC(m, _name) { \
    KS_REQ_N_ARGS(n_args, 2); \
    ks_obj a0 = cvtNum(args[0], TY_FLOAT); \
    if (!a0) _TY_ERR(args[0], "float, complex"); \
    ks_obj a1 = cvtNum(args[1], TY_FLOAT); \
    if (!a1) { KS_DECREF(a0); _TY_ERR(args[1], "float, complex"); } \
    { __VA_ARGS__; }; \
    if (a0->type == ks_type_float && a1->type == ks_type_float) { \
        ks_obj ret = (ks_obj)ks_float_new(_cfunc_f(((ks_float)a0)->val, ((ks_float)a1)->val)); \
        KS_DECREF(a0); KS_DECREF(a1); \
        return ret; \
    } else { \
        double complex a0c, a1c; \
        if (a0->type == ks_type_complex) { \
            a0c = ((ks_complex)a0)->val; \
            if (a1->type == ks_type_complex) { \
                a1c = ((ks_complex)a1)->val; \
            } else { \
                a1c = ((ks_float)a1)->val; \
            } \
        } else { \
            a0c = ((ks_float)a0)->val; \
            a1c = ((ks_complex)a1)->val; \
        } \
        ks_obj ret = (ks_obj)ks_complex_new(_cfunc_c(a0c, a1c)); \
        KS_DECREF(a0); KS_DECREF(a1); \
        return ret; \
    } \
}



/* simple functions */


// floor(val) -> floor a value
T_M_F_1f(floor, floor);
// ceil(val) -> ceil a value
T_M_F_1f(ceil, ceil);
// round(val) -> round a value
T_M_F_1f(round, round);
// round(val) -> round a value
T_M_F_1f(trunc, trunc);
// round(val) -> round a value
T_M_F_1fc(abs, fabs, cabs, if (a0->type == ks_type_complex) { ks_obj ret = (ks_obj)ks_float_new(cabs(((ks_complex)a0)->val)); KS_DECREF(a0); return ret; });



/* misc */

// my cube root for complex numbers
static double complex my_ccbrt(double complex val) {
    return cpow(val, 1.0 / 3.0);
}

// sqrt(val) -> return the square root
T_M_F_1fc(sqrt, sqrt, csqrt, if (a0->type == ks_type_float && ((ks_float)a0)->val < 0.0) return arg_error("val", "val >= 0");)
// cbrt(val) -> return the cube root
T_M_F_1fc(cbrt, cbrt, my_ccbrt)
// exp(val) -> return the exponential function
T_M_F_1fc(exp, exp, cexp)


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
T_M_F_1f(sign, my_sign);
// deg(rad) -> turn radians into degrees
T_M_F_1f(deg, my_deg);
// rad(deg) -> turn degrees into radians
T_M_F_1f(rad, my_rad);


// return x**y
T_M_F_2fc(pow, pow, cpow, 
    if (
        a0->type == ks_type_float && a1->type == ks_type_float && 
        ((ks_float)a0)->val < 0 && floor(((ks_float)a1)->val) != ((ks_float)a1)->val
    ) { 
        return ks_throw(ks_type_MathError, "Cannot raise negative float to fractional float power (cast one or both to 'complex')"); 
    });

// atan2(y, x) -> return the phase given by the coordinate (x, y)
T_M_F_2f(atan2, atan2);
// hypot(x, y) -> return the hypoteneuse of a right triangle with sides 'x' and 'y'
T_M_F_2f(hypot, hypot);


/* trig functions */

// sin(rad) -> return the sin of a given radian measure
T_M_F_1fc(sin, sin, csin);
// cos(rad) -> return the cos of a given radian measure
T_M_F_1fc(cos, cos, ccos)
// tan(rad) -> return the tan of a given radian measure
T_M_F_1fc(tan, tan, ctan)

// asin(val) -> return the inverse sin of a given result
T_M_F_1fc(asin, asin, casin, if (a0->type == ks_type_float && fabs(((ks_float)a0)->val) > 1) return arg_error("val", "|val| <= 1");)
// acos(val) -> return the inverse cos of a given result
T_M_F_1fc(acos, acos, cacos, if (a0->type == ks_type_float && fabs(((ks_float)a0)->val) > 1) return arg_error("val", "|val| <= 1");)
// atan(val) -> return the inverse tan of a given result
T_M_F_1fc(atan, atan, catan)


/* hyperbolic trig functions */

// sinh(rad) -> return the sin of a given radian measure
T_M_F_1fc(sinh, sinh, csinh)
// cosh(rad) -> return the cos of a given radian measure
T_M_F_1fc(cosh, cosh, ccosh)
// tanh(rad) -> return the tan of a given radian measure
T_M_F_1fc(tanh, tanh, ctanh)

// asinh(val) -> return the inverse sin of a given value
T_M_F_1fc(asinh, asinh, casinh)
// acosh(val) -> return the inverse cos of a given value
T_M_F_1fc(acosh, acosh, cacosh, if (a0->type == ks_type_float && ((ks_float)a0)->val < 1) return arg_error("val", "val >= 1");)
// atanh(val) -> return the inverse tan of a given value
T_M_F_1fc(atanh, atanh, catanh, if (a0->type == ks_type_float && fabs(((ks_float)a0)->val) > 1) return arg_error("val", "|val| <= 1");)

// logarithm
// Use an optional 'base' argument which defaults to 'E' (natural logarithm)
static KS_TFUNC(m, log) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_obj val = cvtNum(args[0], TY_FLOAT);
    if (!val) _TY_ERR(args[0], "float, complex");

    if (n_args == 2) {

        // logarithm of a given base
        ks_obj base = cvtNum(args[1], TY_FLOAT);
        if (!base) {
            KS_DECREF(val);
            _TY_ERR(args[1], "float, complex");
        }
        ks_obj ret = NULL;

        if (base->type == ks_type_complex || val->type == ks_type_complex) {
            // 1 or the other is complex
            double complex base_c, val_c;
            if (base->type == ks_type_complex) {
                base_c = ((ks_complex)base)->val;
                if (val->type == ks_type_complex) {
                    val_c = ((ks_complex)val)->val;
                } else {
                    val_c = ((ks_float)val)->val;
                }
            } else if (base->type == ks_type_float) {
                base_c = ((ks_float)base)->val;
                // other must be complex
                val_c = ((ks_complex)val)->val;
            }

            ret = (ks_obj)ks_complex_new(clog(val_c) / clog(base_c));

        } else {
            // both floats
            ret = (ks_obj)ks_float_new(log(((ks_float)val)->val) / log(((ks_float)base)->val));

        }

        KS_DECREF(base);
        KS_DECREF(val);
        return ret;
    } else {
        // natural logarithm
        if (val->type == ks_type_float) {
            ks_obj ret = (ks_obj)ks_float_new(log(((ks_float)val)->val));
            KS_DECREF(val);
            return ret;
        } else if (val->type == ks_type_complex) {
            ks_obj ret = (ks_obj)ks_complex_new(clog(((ks_complex)val)->val));
            KS_DECREF(val);
            return ret;
        } else {
            assert (false && "Unknown type ! shouldn't get here");
        }
    }
}



/* my additional functions */

/* 
 * Gamma Function : compute (x-1)! for all values, analycritcally continued
 * See: https://en.wikipedia.org/wiki/Gamma_function
 * 
 * 
 * Use Lanczos approximation [0] to compute it very fast and accurately
 * 
 *   [0]: https://en.wikipedia.org/wiki/Lanczos_approximation
 *   [1]: https://mrob.com/pub/ries/lanczos-gamma.html
 * 
 * 
 * TODO: Actually generate arbitrary coefficients for gamma
 * https://web.viu.ca/pughg/phdThesis/phdThesis.pdf
 * 
 */

// constants used in computation
#define SQRT_2PI 2.506628274631000502415765284811045253006986
#define LOG_SQRT_2PI 0.91893853320467274178032973640561763986139747363778341


// Level 0 of approximation, using a series of length 7
// TODO: add this to compute_table.py, right now I pull these from [1] in the Lanczos approximation
#define GAMMA_C0_G 5
#define GAMMA_C0_N 7

static const double gamma_C0[] = {
    1.000000000190015,
    76.18009172947146,
    -86.50532032941677,
    24.01409824083091,
    -1.231739572450155,
    0.1208650973866179e-2,
    -0.5395239384953e-5
};


// Level 1 of approximation, using a series of length 6
// TODO: add this to compute_table.py
#define GAMMA_C1_G 4.7421875
#define GAMMA_C1_N 15

static const double gamma_C1[] = {
    0.99999999999999709182,
   	57.156235665862923517,
   	-59.597960355475491248,
   	14.136097974741747174,
    -0.49191381609762019978,
    0.33994649984811888699e-4,
    0.46523628927048575665e-4,
    -0.98374475304879564677e-4,
    0.15808870322491248884e-3,
	-0.21026444172410488319e-3,
 	0.21743961811521264320e-3,
 	-.16431810653676389022e-3,
 	0.84418223983852743293e-4,
    -0.26190838401581408670e-4,
	0.36899182659531622704e-5
};


// compute Gamma(z), for all complex numbers
static complex double my_cgamma(complex double z) {
    if (creal(z) < 0.5) {
        // use reflection formula, so our approximation converges
        return M_PI / (csin(M_PI * z) * my_cgamma(1 - z));
    } else {
        // shift off
        z -= 1;

        // sum a seriies approximation
        complex double sum = gamma_C1[0];

        int i;
        for (i = 1; i < GAMMA_C1_N; ++i) {
            sum += gamma_C1[i] / (z + i);
        }

        double complex t = z + (GAMMA_C1_G) + 0.5;
        return SQRT_2PI * cpow(t, z + 0.5) * cexp(-t) * sum;
    }
}

// compute ln(Gamma(z)), for all complex numbers
static complex double my_clgamma(complex double z) {
    if (creal(z) < 0.5) {
        // use reflection formula, so our approximation converges
        return M_PI / (csin(M_PI * z) * my_cgamma(1 - z));
    } else {
        // shift off
        z -= 1;

        // sum a seriies approximation
        complex double sum = gamma_C1[0];

        int i;
        for (i = 1; i < GAMMA_C1_N; ++i) {
            // sum approximation terms
            sum += gamma_C1[i] / (z + i);
        }

        double complex t = z + (GAMMA_C1_G) + 0.5;
        return (LOG_SQRT_2PI + clog(sum)) - t + clog(t) * (z + 0.5);
    }
}

// gamma(x) -> return the Gamma(x) function
T_M_F_1fc(gamma, tgamma, my_cgamma, if (a0->type == ks_type_float && ((ks_float)a0)->val <= 0 && floor(((ks_float)a0)->val) == ((ks_float)a0)->val) { return arg_error("x", "!(x <= 0 && is_int(x))"); });
// lgamma(x) -> return log(|Gamma(x)|) function
T_M_F_1fc(lgamma, lgamma, my_clgamma);




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

// Level 0: accurate to 13.3 digits for all real numbers
#define ZETA_C0_N 18

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C0[] = {
    0.99999999999996680250428630369534923790383303322221        ,
    0.99999999997845482528181109828165539958763856134628        ,
    0.99999999766233194432864731540728547421070042269762        ,
    0.99999989884108902366032591276750199146133984034571        ,
    0.9999976753631233086230943533723736296007267374311         ,
    0.99996723842030374278010234031906094279766737308791        ,
    0.99969146127172646438450804022995508358206889010209        ,
    0.99794588283677577915525181109451579887674222856525        ,
    0.98994531500991847185449409422375241064399502985365        ,
    0.96275384265720082612642865126429514344773004076832        ,
    0.8932009186391967428430612550627360284088627528992         ,
    0.7583103993315524601116820624294092598486352855157         ,
    0.55988449049494529058653730804850104232888038784346        ,
    0.34008963762978042588176158011887963215315188580675        ,
    0.15983459956575633048895608895966630634766025715231        ,
    0.053753473716583529522155616001600533000060586036066       ,
    0.011406572671954064620086072361082341220172007646909       ,
    0.0011406572671954064620086072361082341220172007646914      ,
};


// Level 1 of approximation, using a series of length '29'
// accurate to 13 .3 digits for imag(z) < 11
#define ZETA_C1_N 29

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C1[] = {
    0.99999999999999999999987409579181870725745186299241        ,
    0.99999999999999999978810321763088431429148541571048        ,
    0.99999999999999994049225733256925428382088017692034        ,
    0.99999999999999332307585655969134288330133552789077        ,
    0.99999999999960005947261062866117965242553924272514        ,
    0.99999999998518039402025982422186118697967545330829        ,
    0.99999999962862139374394902354053185959104356955615        ,
    0.99999999332026985039383485764008991348447947239428        ,
    0.99999991005002947817232786775425622487783338986548        ,
    0.99999906428562726619702157832559248569660455162507        ,
    0.99999229817040957039457126289628257224677384570722        ,
    0.99994888958706487264898092754460598465954827786623        ,
    0.99972241002178818875894439527498900594358879348072        ,
    0.99875098690180142770444923840161648800498103584056        ,
    0.99529703803073738839957756951851420200104234200667        ,
    0.98505429310275437528857882731345225040315518098346        ,
    0.95961263634615140723867872570733062869227416811906        ,
    0.90655249658639120649289776513841387753054371348208        ,
    0.81357091834071618804314827233193118978046367868978        ,
    0.676809962571117370010373058161372016048482517985          ,
    0.50848878623930344012388048687452995607065955096426        ,
    0.33606221536281112170357102360508199219093846279627        ,
    0.19024693343765905538618458532225073309603690832022        ,
    0.089655521558858499491871622101051197836365691029614       ,
    0.034009208604628404741826153085068476203356081464497       ,
    0.009933660959124771911194235878153502762135597326164       ,
    0.0020900436266530001292689053854029231794755300955678      ,
    0.00028124298394462508521903881474765325475308272582778     ,
    1.8144708641588715175421859015977629338908562956638e-05     ,

};


// Level 2 of approximation, using a series of length '65'
// accurate to 13.1 digits for all imag(z) <= 50
#define ZETA_C2_N 65

// coefficients (see `compute_table.py` for how to compute)
static const double zeta_C2[] = {
    1.0                                                         ,
    0.99999999999999999999999999999999999999999999970715        ,
    0.99999999999999999999999999999999999999999958699191        ,
    0.99999999999999999999999999999999999999976731098323        ,
    0.99999999999999999999999999999999999992981905015767        ,
    0.9999999999999999999999999999999999868448310582755         ,
    0.99999999999999999999999999999999832148272300070387        ,
    0.99999999999999999999999999999984499836468666737008        ,
    0.9999999999999999999999999999891737093493578673601         ,
    0.9999999999999999999999999994087392832999050981708         ,
    0.99999999999999999999999997408957863541640746110968        ,
    0.99999999999999999999999906983384121100577756607691        ,
    0.99999999999999999999997217805451954592443633988545        ,
    0.9999999999999999999992968220458320677893376070317         ,
    0.99999999999999999998480346770066587035039337182508        ,
    0.99999999999999999971632657272744927503312291827876        ,
    0.99999999999999999538605407315943967314166399009306        ,
    0.99999999999999993411385603114364605493417562656588        ,
    0.99999999999999916850315287757487246361774959849666        ,
    0.99999999999999067164012868960393141176745675212727        ,
    0.99999999999990648733570442724568468266609378194508        ,
    0.99999999999915850832426934357485137531948899461149        ,
    0.99999999999317467623278867420818491654665069593941        ,
    0.9999999999499176407656791397140801392148862991648         ,
    0.99999999966644600408759963707186159840247280540861        ,
    0.99999999797764926573933497561642412015839633403193        ,
    0.99999998880771222493427799033803057313173676095475        ,
    0.99999994332328656760772698998440555790096831251938        ,
    0.9999997368121799468108149156515834108272455907922         ,
    0.99999887703635673790316283122418957164916950432653        ,
    0.99999558948676833231729316602384973913096670590869        ,
    0.99998402829969117888205610966094128527266495579193        ,
    0.99994659207486992013938373667628533944578309826946        ,
    0.99983485934232647096894619269131220882401263120243        ,
    0.99952722646326039845051586965630278158092906252763        ,
    0.99874534215199433090677867595489993917179989295078        ,
    0.99690993297061858549894488792343786778886757000114        ,
    0.99292924915555041941264709810918210541892401285992        ,
    0.98495112075145590548810921199724213536379503096807        ,
    0.97017444736365248009740667368401865436258710646411        ,
    0.94488576075566484575787524102645138054786164579524        ,
    0.90490816223355250601606854983336851769712130598428        ,
    0.84655875853862778295057444633640764386333850364317        ,
    0.76798262584740494110970933623735390350495848911193        ,
    0.67043984043761106847966988921783891547386605727685        ,
    0.55894149546731485152578210183822852466079710523189        ,
    0.44174304399352045243426125509425200302259753786475        ,
    0.32864694051228852029790002618414150925231024087204        ,
    0.22864617532888344346153851851625433581331936774184        ,
    0.14781239880150235673624903556653362881718998954571        ,
    0.088240355009202113428399331768436428994587975069223       ,
    0.04834061688389909665261181903237831612563496595566        ,
    0.024144509820608544283724992787658982556113051541729       ,
    0.010918174207575325899869875907796230944525331209774       ,
    0.0044354842705538522994756753187046226779942782537378      ,
    0.0016045114173224564886613271715416818203164973298162      ,
    0.00051147170565010289375231244290734172082700662559877     ,
    0.00014186442541063912574861207356009863175883231201611     ,
    3.3697497220619144467769026938536636277411583363209e-05     ,
    6.7145259989453203672892751954785809074265776063063e-06     ,
    1.091184097285565428701898361631187855530542793106e-06      ,
    1.3885008061103708928525819873872866099456616168543e-07     ,
    1.2971154724868119165670828663250183668016929523099e-08     ,
    7.9087008673976929505170790166102814765749905862593e-10     ,
    2.3608062290739381941842026915254571571865643541076e-11     ,
};

// Level 3 of approximation, using a series of length '112'
#define ZETA_C3_N 112

// coefficients (see `compute_table.py` for how to compute)
// accurate to 15 digits for imag(z) <= 100, and this is the last one used
static const double zeta_C3[] = {
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    1.0                                                         ,
    0.99999999999999999999999999999999999999999999999521        ,
    0.99999999999999999999999999999999999999999999979473        ,
    0.99999999999999999999999999999999999999999999097572        ,
    0.99999999999999999999999999999999999999999964788202        ,
    0.99999999999999999999999999999999999999998772011708        ,
    0.99999999999999999999999999999999999999961511509626        ,
    0.99999999999999999999999999999999999998910427798009        ,
    0.9999999999999999999999999999999999997201557333607         ,
    0.99999999999999999999999999999999999345248530223822        ,
    0.99999999999999999999999999999999985993110191582988        ,
    0.99999999999999999999999999999999725097776906920459        ,
    0.99999999999999999999999999999995034884387133328572        ,
    0.99999999999999999999999999999917238305058226288851        ,
    0.99999999999999999999999999998723515467693243594487        ,
    0.99999999999999999999999999981738421012264088741181        ,
    0.99999999999999999999999999757132138060978891849583        ,
    0.99999999999999999999999996991065973402316985339605        ,
    0.99999999999999999999999965206002099155014541783158        ,
    0.99999999999999999999999623794406918428744938266473        ,
    0.99999999999999999999996190194913682941986107465428        ,
    0.99999999999999999999963807726152634724007171513611        ,
    0.99999999999999999999677002433581191234724248164149        ,
    0.99999999999999999997288267942027250940422639994139        ,
    0.99999999999999999978555560613525298106162660134112        ,
    0.9999999999999999984007200993329108134939557625415         ,
    0.99999999999999998873930124396441859001393319539368        ,
    0.99999999999999992506295800207777065640034272574353        ,
    0.99999999999999952820919056856391150387915324105548        ,
    0.99999999999999718726056516047668761103186314805945        ,
    0.99999999999998410656801096011469203422843182904816        ,
    0.99999999999991481319142421482627801239032974063009        ,
    0.99999999999956655851951022835287758443285440949015        ,
    0.99999999999790487939861384265412688082244363249293        ,
    0.99999999999037266285718219958544770458841057096759        ,
    0.99999999995791741464162990628720458881458412450441        ,
    0.99999999982490990650007559884313189253421174209528        ,
    0.99999999930620386426603503818762183179258479240082        ,
    0.99999999738032851776377017836770527691805276820575        ,
    0.99999999056968578296330792477274430658008507078427        ,
    0.99999996762001218542696315538993189874342341124617        ,
    0.999999893905430901997462758519967640978159538677          ,
    0.9999996681367374538103925275659168785368639135172         ,
    0.99999900860192361728067455432706900614198447197055        ,
    0.99999717040819209987625819413063992850768030130192        ,
    0.99999228134011870286842328565513103884147364137621        ,
    0.99997986968074917987626424489503662230230530554801        ,
    0.99994978977791731057891416751845229456180358609281        ,
    0.99988018710739919287355887439135984146596129111559        ,
    0.99972639834968297089601194100578413557800498221087        ,
    0.99940190558457061768305304489817791560034126448307        ,
    0.99874804395554994745214624601515887055053730804465        ,
    0.99748978004349282146303829785234706491340589152616        ,
    0.9951774324030655411131750098808280289899782092341         ,
    0.99111948899486472559723845549943677967337472756409        ,
    0.98431976154475355266034699020786401026948187347897        ,
    0.9734415953075047841160867077074917873978481461755         ,
    0.95682932825775359307576594066466983721689355392505        ,
    0.93261762668932251569124249172871546810117826533642        ,
    0.89894769309581231643756415745027916074328227591506        ,
    0.85428349547176817457043983646867997751342024913226        ,
    0.79778478427593032316697948735047988318345786786998        ,
    0.72965746068513177073398155156295348850429967408219        ,
    0.65138129973741517573075092098660536199745939642213        ,
    0.56572426008991388938726529298618470743290216950391        ,
    0.47649673834148878098140910385730984998115022466927        ,
    0.38806889721250710515088841831025038040278662273638        ,
    0.30474696547468586194826299203908507389416520255798        ,
    0.23015245438873875764161808325854010474242467331551        ,
    0.16675120509772566570081145761373956141082483685091        ,
    0.11563428682845292048762904130045939709086517400401        ,
    0.076576904654868677434382246907520790507093697539875       ,
    0.048323645334687174809573366263424442292884465483138       ,
    0.028996321039916810646060494353809400022370994401187       ,
    0.016508861511802192241712892048571109724214563409889       ,
    0.0088989071240728494700670479471118583522662527695707      ,
    0.0045313318125306084231001035298497703209171478545983      ,
    0.0021745223067940145049747801141114764454614472605774      ,
    0.00098099903250616705035750122110929697396374571242244     ,
    0.00041493393990582789141085804953720836673205465558471     ,
    0.00016407223371873936892853069110356573103203910587577     ,
    6.0458175624423107011018037795635138642659249755355e-05     ,
    2.0687465498561531346998946718998167152887465976289e-05     ,
    6.5475239036822702167328781219095670655031920147595e-06     ,
    1.9082024070892316355309812038695083241165150006899e-06     ,
    5.0948372791335782855425178387896607936680158005111e-07     ,
    1.2388721163401894890227502015695026254786048328503e-07     ,
    2.7245909489825253041516546494686508251285324409144e-08     ,
    5.3748990654221013558450402848261801998146784856049e-09     ,
    9.4165612834204029902884106255754633477587362873795e-10     ,
    1.4471028262948751707609621908952268846873061516993e-10     ,
    1.9203568444817550594953080377948766316190799499324e-11     ,
    2.1558443236949824929360520579200810126311124740808e-12     ,
    1.9906988199182606474612857561588019853290961952906e-13     ,
    1.4516220177445678108060857000626116583079117743192e-14     ,
    7.8373614610464396934698625430018819210538573698521e-16     ,
    2.7850972773045328087766967889719904582298620329288e-17     ,
    4.8861355742184786118889417350385797512804597068872e-19     ,
};

// compute zeta function
double my_zeta(double x) {
    if (x < 0) {
        // check for negative even integers
        int ix = floor(x);
        if (ix == x && ix % 2 == 0) {
            return 0;
        }

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
        // C0 is accurate for all real numbers
        for (i = 0; i < ZETA_C0_N; ++i, sign *= -1) {
            sum += sign * zeta_C0[i] * pow(i + 1, -x);
        }

        // transform it back from the Eta function so that it is relevant to the Riemann Zeta function
        sum /= 1 - pow(2, 1 - x);

        return sum;
    }
}

// compute zeta function on complex numbers
double complex my_czeta(complex double x) {

    if (x == 1) return INFINITY;

    double cr = creal(x);

    if (cr < 0) {

        // check for negative even integers
        if (cimag(x) == 0) {
            int icr = floor(cr);
            if (icr == cr && icr % 2 == 0) {
                return 0;
            }
        }

        // use reflection formula with the functional equation
        // Zeta(x) = 2 * (2*PI)^(x-1) * sin(x * PI/2) * Gamma(1-x) * Zeta(1 - x)
        return 2 * cpow(2 * M_PI, x - 1) * csin(x * M_PI / 2) * my_cgamma(1 - x) * my_czeta(1 - x);

    } else {

        // now, have cutoffs for different acuracies

        // the total sum
        complex double sum = 0;

        // the alterating sign, and the index
        int sign = 1, i;

        // what do discriminate on. Check comments above, or `compute_table.py` for error bounds 
        double disc = fabs(cimag(x));

        // compute term, using d_k which was precomputed
        // term = (-1)^i * d_i / (i+1)^x
        if (disc < 1) {
            for (i = 0; i < ZETA_C0_N; ++i, sign *= -1) {
                sum += sign * zeta_C0[i] * cpow(i + 1, -x);
            }
        } else if (disc < 11) {
            for (i = 0; i < ZETA_C1_N; ++i, sign *= -1) {
                sum += sign * zeta_C1[i] * cpow(i + 1, -x);
            }
        } else if (disc < 50) {
            for (i = 0; i < ZETA_C2_N; ++i, sign *= -1) {
                sum += sign * zeta_C2[i] * cpow(i + 1, -x);
            }
        } else {
            // fall back to most accurate
            for (i = 0; i < ZETA_C3_N; ++i, sign *= -1) {
                sum += sign * zeta_C3[i] * cpow(i + 1, -x);
            }
        }

        // transform it back from the Eta function so that it is relevant to the Riemann Zeta function
        sum /= 1 - cpow(2, 1 - x);

        return sum;
    }
}


// zeta(x) -> return the Riemann Zeta function of 'x'
T_M_F_1fc(zeta, my_zeta, my_czeta);


// now, export them all
static ks_module get_module() {
    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* functions */
        {"floor", (ks_obj)ks_cfunc_new_c(m_floor_, "m.floor(val)")},
        {"ceil", (ks_obj)ks_cfunc_new_c(m_ceil_, "m.ceil(val)")},
        {"round", (ks_obj)ks_cfunc_new_c(m_round_, "m.round(val)")},
        {"abs", (ks_obj)ks_cfunc_new_c(m_abs_, "m.abs(val)")},

        {"sin", (ks_obj)ks_cfunc_new_c(m_sin_, "m.sin(rad)")},
        {"cos", (ks_obj)ks_cfunc_new_c(m_cos_, "m.cos(rad)")},
        {"tan", (ks_obj)ks_cfunc_new_c(m_tan_, "m.tan(rad)")},

        {"asin", (ks_obj)ks_cfunc_new_c(m_asin_, "m.asin(val)")},
        {"acos", (ks_obj)ks_cfunc_new_c(m_acos_, "m.acos(val)")},
        {"atan", (ks_obj)ks_cfunc_new_c(m_atan_, "m.atan(val)")},
        {"atan2", (ks_obj)ks_cfunc_new_c(m_atan2_, "m.atan2(y, x)")},

        {"sinh", (ks_obj)ks_cfunc_new_c(m_sinh_, "m.sinh(rad)")},
        {"cosh", (ks_obj)ks_cfunc_new_c(m_cosh_, "m.cosh(rad)")},
        {"tanh", (ks_obj)ks_cfunc_new_c(m_tanh_, "m.tanh(rad)")},

        {"asinh", (ks_obj)ks_cfunc_new_c(m_asinh_, "m.asinh(val)")},
        {"acosh", (ks_obj)ks_cfunc_new_c(m_acosh_, "m.acosh(val)")},
        {"atanh", (ks_obj)ks_cfunc_new_c(m_atanh_, "m.atanh(val)")},

        {"log", (ks_obj)ks_cfunc_new_c(m_log_, "m.log(val, base=E)")},

        {"sign", (ks_obj)ks_cfunc_new_c(m_sign_, "m.sign(val)")},

        {"rad", (ks_obj)ks_cfunc_new_c(m_rad_, "m.rad(degrees)")},
        {"deg", (ks_obj)ks_cfunc_new_c(m_deg_, "m.deg(radians)")},

        {"sqrt", (ks_obj)ks_cfunc_new_c(m_sqrt_, "m.sqrt(val)")},
        {"cbrt", (ks_obj)ks_cfunc_new_c(m_cbrt_, "m.cbrt(val)")},
        {"exp", (ks_obj)ks_cfunc_new_c(m_exp_, "m.exp(val)")},
        {"pow", (ks_obj)ks_cfunc_new_c(m_pow_, "m.pow(x, y)")},
        {"hypot", (ks_obj)ks_cfunc_new_c(m_hypot_, "m.hypot(x, y)")},

        {"gamma", (ks_obj)ks_cfunc_new_c(m_gamma_, "m.gamma(x)")},
        {"lgamma", (ks_obj)ks_cfunc_new_c(m_lgamma_, "m.lgamma(x)")},

        {"zeta", (ks_obj)ks_cfunc_new_c(m_zeta_, "m.zeta(x)")},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
