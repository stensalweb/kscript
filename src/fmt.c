/* fmt.c - implementation of string formatting functions, using string builder functionality
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"
#include <ctype.h>


// return true if 'c' is a valid part of the field specifier, which means
//   it is not alpha character
static bool ks_is_field(char c) {
    return c && !isalpha(c);
}



// arguments to the internal buffer formatter
typedef struct {

    // the total width of the field, in characters. Default is 0, which is interpreted differently
    //   by different formatting strings
    ks_size_t width;

    // the base for numbers (default: 10)
    int base;

    // true if we should always add the sign, even for positive numbers (default: false)
    bool do_sign;


    // true if empty values for ints/floats should be zero padded (if not, they are blank) (default: false)
    bool zero_pad;

} bfmt_arg;

#define BFMT_ARG_DEFAULT ((bfmt_arg){ .width = 0, .base = 10, .do_sign = false, .zero_pad = false })

// internal buffer reversing on a buffer, reverses '0' through 'n-1' (inclusive)
static void bfmt_rev(char* buf, int n) {
    int i;
    for (i = 0; 2 * i < n; ++i) {
        char tmp = buf[i];
        buf[i] = buf[n - i - 1];
        buf[n - i - 1] = tmp;
    }
}

// digits used for base formatting
static const char bfmt_digs[] = "0123456789abcdef";

// internal buffer formatting on a buffer (of max size 'n'), with an integer value, and argument specifiers
// returns the number of characters written
static int bfmt_i64(char* buf, int n, int64_t val, bfmt_arg arg) {
    // ensure positive values, but extract the negative sign
    bool is_neg = val < 0;
    if (is_neg) val = -val;

    // buffer pointer, start at 0
    int bp = 0;

    // ensure the base is representable
    assert (arg.base <= sizeof(bfmt_digs) - 1);

    // add digits, which will be in reverse order
    do {
        buf[bp++] = bfmt_digs[val % arg.base];
        val /= arg.base;
    } while (bp < n - 1 && val > 0);

    if (arg.width > 0) while (bp < n - 1 && bp < arg.width) {
        buf[bp++] = arg.zero_pad ? '0' : ' ';
    }


    // add negative sign to the end, which will be reversed
    if (is_neg) buf[bp++] = '-';
    else if (arg.do_sign) buf[bp++] = '+';

    // reverse the buffer, since digits come out in opposite order
    bfmt_rev(buf, bp);

    // the next character would be here
    return bp;
}

// internal buffer formatting on a buffer (of max size 'n'), with a floating point value
// returns the number of characters written
static int bfmt_f64(char* buf, int n, double val, bfmt_arg arg) {

    // make sure we are working with a positive argument
    bool is_neg = val < 0;
    if (is_neg) val = -val;

    int bp = 0;

    // whole number part
    int64_t whole = (int64_t)val;
    
    // format an integer
    bp += bfmt_i64(buf + bp, n - bp, whole, arg);

    buf[bp++] = '.';


    // the fractional part
    int64_t frac = (uint64_t)(10000 * (val - whole));
    bfmt_arg frac_arg = (bfmt_arg){ .width = 4, .base = arg.base, .do_sign = false };

    // add on fractional part
    bp += bfmt_i64(buf + bp, n - bp, frac, frac_arg);

    // add the sign back
    if (is_neg) buf[bp++] = '-';
    else if (arg.do_sign) buf[bp++] = '+';

    return bp;
}


// Add a formatted string (formmated by ks_vfmt), and then appended to the string buffer
void ks_str_b_add_fmt(ks_str_b* self, char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    ks_str ret = ks_fmt_vc(fmt, ap);
    va_end(ap);

    ks_str_b_add(self, ret->len, ret->chr);

    KS_DECREF(ret);

}

// Perform a var-args style C formatting, like vprintf
ks_str ks_fmt_vc(const char* fmt, va_list ap) {
    ks_str_b SB;
    ks_str_b_init(&SB);

    // current position in the formatting string
    char* p = (char*)fmt;

    // the field in between '%' and the actual format code
    char field[256];

    // temporary field for fixed size formats
    char tmp[256];

    // temporary variable for looping
    int j;

    // formatting arguments
    bfmt_arg barg;

    while (*p) {

        // starting position
        char* s_p = p;

        // parse until we find a formatting character
        while (*p && *p != '%') p++;

        // now, add all those bytes to the string builder
        ks_str_b_add(&SB, (int)(p - s_p), s_p);

        // current character
        char c = *p;

        // ensure we have not hit the end
        if (!c) break;

        // this should be the only way to get here, if we have started parsing a format specifier
        assert(c == '%');
        p++;

        // now, parse out the field, which will be all digits

        // field position
        int f_p = 0;

        // copy the entire field over to the 'field' array, max size of 255
        while (*p && f_p < 255 && ks_is_field(*p)) {
            field[f_p++] = *p++;
        }
        
        // always NUL-terminate it
        field[f_p] = '\0';

        // reset formatting args
        barg = BFMT_ARG_DEFAULT;

        // now, try and parse the field
        if (strncmp(p, "c", 1) == 0) {
            // %c - print a 'char' in C

            // add single character
            char v_char = (char)va_arg(ap, int);

            ks_str_b_add(&SB, 1, &v_char);

            p += 1;

        } else if (strncmp(p, "i", 1) == 0) {
            // %i - print a 32 bit 'int' in C

            // get an integer from the varargs
            int v_int = va_arg(ap, int);

            // parse out argument specifiers
            barg.do_sign = strchr(field, '+');

            for (j = 0; j < f_p; ++j) {
                // attempt to search for a digit
                if (isdigit(field[j])) {
                    // if so, scan out the width
                    if (field[j] == '0') barg.zero_pad = true;
                    int x = atoll(field);
                    barg.width = x;
                    break;
                }
            }

            int amt = bfmt_i64(tmp, 255, v_int, barg);
            
            // add to the string builder
            ks_str_b_add(&SB, amt, tmp);

            // advance past the specifier
            p += 1;
        } else if (strncmp(p, "l", 1) == 0) {
            // %l - print a 64 bit 'int' in C (aka long)

            // get the value from varargs
            int64_t v_long = va_arg(ap, int64_t);

            // parse out argument specifiers
            barg.do_sign = strchr(field, '+');

            for (j = 0; j < f_p; ++j) {
                // attempt to search for a digit
                if (isdigit(field[j])) {
                    // if so, scan out the width
                    if (field[j] == '0') barg.zero_pad = true;
                    int x = atoll(field);
                    barg.width = x;
                    break;
                }
            }

            int amt = bfmt_i64(tmp, 255, v_long, barg);

            // add to the string builder
            ks_str_b_add(&SB, amt, tmp);

            // advance past the specifier
            p += 1;
        } else if (strncmp(p, "x", 1) == 0) {
            // %x - print a 32 bit integer as a hex value
            // get an integer from the varargs
            int v_int = va_arg(ap, int);

            barg.base = 16;

            // parse out argument specifiers
            barg.do_sign = strchr(field, '+');

            for (j = 0; j < f_p; ++j) {
                // attempt to search for a digit
                if (isdigit(field[j])) {
                    // if so, scan out the width
                    if (field[j] == '0') barg.zero_pad = true;
                    int x = atoll(field);
                    barg.width = x;
                    break;
                }
            }

            int amt = bfmt_i64(tmp, 255, v_int, barg);
            
            // add to the string builder
            ks_str_b_add(&SB, amt, tmp);

            // advance past the specifier
            p += 1;
        } else if (strncmp(p, "p", 1) == 0) {
            // %p - print a device pointer address in hex
            uintptr_t v_ptr = va_arg(ap, uintptr_t);

            // HEX (i.e. base '16') for it
            barg.base = 16;

            // add pointer to the buffer
            int amt = bfmt_i64(tmp, 255, v_ptr, barg);

            // add hex prefix
            ks_str_b_add(&SB, 2, "0x");

            // add to the string builder
            ks_str_b_add(&SB, amt, tmp);

            // advance past the specifier
            p += 1;

        } else if (strncmp(p, "f", 1) == 0 || strncmp(p, "lf", 2) == 0) {
            // %f, %lf : print a floating point number
            // NOTE: by the C standard, floats are upcasted to doubles, so this is fine
            // print out a double
            double v_lf = va_arg(ap, double);

            int amt = bfmt_f64(tmp, 255, v_lf, barg);

            // add to the string builder
            ks_str_b_add(&SB, amt, tmp);

            // advance past the specifier
            p += strncmp(p, "lf", 2) == 0 ? 2 : 1;
        } else if (strncmp(p, "s", 1) == 0) {
            // %s : print a C-style NUL-terminated string

            char* v_chr = va_arg(ap, char*);

            int sz = v_chr == NULL ? 0 : strlen(v_chr);

            // add to the string builder
            ks_str_b_add(&SB, sz, v_chr);

            // advance past the specifier
            p += 1;


        } else if (strncmp(p, "T", 1) == 0) {
            // %T : print the type of an object
        
            ks_obj v_obj = va_arg(ap, ks_obj);

            // add to the string builder
            ks_str_b_add(&SB, v_obj->type->__name__->len, v_obj->type->__name__->chr);

            // advance past the specifier
            p += 1;

        } else if (strncmp(p, "S", 1) == 0) {
            // %S : convert a kscript object to a string and add it
        
            ks_obj v_obj = va_arg(ap, ks_obj);

            // add to the string builder
            ks_str_b_add_str(&SB, v_obj);

            // advance past the specifier
            p += 1;
        } else if (strncmp(p, "R", 1) == 0) {
            // %R : convert a kscript object to a repr and add it
        
            ks_obj v_obj = va_arg(ap, ks_obj);

            // add to the string builder
            ks_str_b_add_repr(&SB, v_obj);

            // advance past the specifier
            p += 1;
        } else {
            fprintf(stderr, "Unknown format specifier: '%%%c' (whole format string was: %s)\n", *p, fmt);
            assert(false && "Unknown format specifier in 'ks_fmt_vc'");
        }

    }


    // actually generate out the string
    ks_str ret = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    return ret;
}

// Perform a var-args style C formatting, like printf
ks_str ks_fmt_c(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    ks_str ret = ks_fmt_vc(fmt, ap);
    va_end(ap);
    return ret;
}

