/* fmt.c - implementation of string formatting rules and functions, as well as the string building utilities

Essentially, this includes `strB`, the string builder utility structure (to avoid the overhead of
immutable string concatenation, which is O(n^2), whereas strB appending is roughly O(n)).

It also includes arbitrary formatting, which is styled like printf. It uses a `%` as a formatting key,
lowercase letters are C literals (i.e. expect a machine-type), and upper case letters expect a `kso` of some type.

The one exception is `%o`, which does except a `kso`. But, this is intentional; it is completely non-recursive,
non-function-calling, so it never has the risk of resulting in an exception. This is useful for lower level tracing machines.

Here are the format strings:

C-arg:
  %i, int : prints out a signed integer value
  %l, int64_t : formats an int64_t as a signed 64 bit integer value
  %p, void* : formats a pointer of any type as a hex address, i.e. `0xabd9032`
  %s, char* : formats a NUL-terminated C-string
  %*s, int, char* : formats a C-string, given a length before the full value
  %o, kso : formats an object in a generic way, with type name and address

C-arg object formatters:

  %S, kso : formats the kscript object as if `str()` had been called on it (i.e. tostring)
  %R, kso : formats the kscript object as if `repr()` had been called on it
  %T, kso : formats the type name of the kscript object


 -*- kfmt methods -*-

These methods are meant to be called from kscript. These are more dynamic in nature, and take arrays rather than C-style varargs

But, they work similarly:

(in kscript) `"%s %s".format()`



*/

#include "ks.h"

#include <ctype.h>


/* internal format enums */

enum {

    // no special format enums
    FE_NONE = 0,

    // always include sign, i.e. %+i
    // print 2 as `+2`
    FE_SIGN = 1<<0,

    // some variable-sized input should also be read in,
    // i.e. %*s
    FE_STAR = 1<<1,

    // do zero padding, i.e. %0i
    FE_ZERO = 1<<2,

    // end of the flags
    FE__END

};

// format arguments
struct fmta {
    uint32_t flags;

    // width, if applicable. Usually 0
    int width;

    // the integer-base, if applicable, usually 10
    int base;

};

#define FMTA_NONE ((struct fmta){.flags = FE_NONE, .width = -1, .base = 10})

// first 100 base 10-strings
static const char b10_f100[100][2] = {
    "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
    "10", "11", "12", "13", "14", "15", "16", "17", "18", "19",
    "20", "21", "22", "23", "24", "25", "26", "27", "28", "29",
    "30", "31", "32", "33", "34", "35", "36", "37", "38", "39",
    "40", "41", "42", "43", "44", "45", "46", "47", "48", "49",
    "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
    "60", "61", "62", "63", "64", "65", "66", "67", "68", "69",
    "70", "71", "72", "73", "74", "75", "76", "77", "78", "79",
    "80", "81", "82", "83", "84", "85", "86", "87", "88", "89",
    "90", "91", "92", "93", "94", "95", "96", "97", "98", "99"
};

// base strings, for bases >= 10
static const char base_str_digs[] = "0123456789abcdefghijk";

/* helper macros/functions to format basic data types into buffers */

// reverse the string from [start, stop)
static void buffmt_rev(char* buf, int start, int stop) {
    int i;
    for (i = start; 2 * (i - start) < stop - start; ++i) {
        char tc = buf[i];
        buf[i] = buf[stop - i - 1 + start];
        buf[stop - i - 1 + start] = tc;
    }
}

static void buffmt_charp(char* buf, int* bufp, char* str, int len) {
    memcpy(buf+*bufp, str, len);
    (*bufp) += len;
}

// formats an integer given some arguments, updating `buf` and `bufp`
static void buffmt_int(char* buf, int* bufp, int64_t val, struct fmta args) {
    if (val < 0) { buf[(*bufp)++] = '-'; val = -val; }
    else if (args.flags & FE_SIGN) buf[(*bufp)++] = '+';

    int start_buf = *bufp;//, try = args.width < 0 ? -10000 : 0;

    do {
        buf[(*bufp)++] = base_str_digs[val % args.base];
        val /= args.base;
    } while (val > 0);


    buffmt_rev(buf, start_buf, *bufp);
}

// formats a double-precision floating point to a buffer
static void buffmt_double(char* buf, int* bufp, double val, struct fmta args) {
    if (val < 0) { buf[(*bufp)++] = '-'; val = -val; }
    else if (args.flags & FE_SIGN) buf[(*bufp)++] = '+';

    // output whole number
    buffmt_int(buf, bufp, (int64_t)val, FMTA_NONE);
    
    buffmt_charp(buf, bufp, ".", 1);

    // output fractional part
    uint64_t v_frac = (uint64_t)(10000 * (val - (int64_t)val));
    struct fmta frac_args = { .flags = FE_ZERO | FE_STAR, .width = 4 };
    buffmt_int(buf, bufp, v_frac, frac_args);

}


/* string building */

// create a new empty string builder
ks_strB ks_strB_create() {
    ks_strB ret;
    ret.cur = (ks_str)ks_malloc(sizeof(*ret.cur));
    *ret.cur = (struct ks_str) {
        KSO_BASE_INIT(ks_T_str)
        .len = 0,
    };
    return ret;
}

// append a string to the string builder
void ks_strB_add(ks_strB* strb, char* val, int len) {
    strb->cur = ks_realloc(strb->cur, sizeof(*strb->cur) + strb->cur->len + len);
    memcpy(&strb->cur->chr[strb->cur->len], val, len);
    strb->cur->len += len;
}

// appends repr(obj) to the builder
void ks_strB_add_repr(ks_strB* strb, kso obj) {
    if (obj->type == ks_T_str) {
        ks_strB_add(strb, "'", 1);
        ks_strB_add(strb, ((ks_str)obj)->chr, ((ks_str)obj)->len);
        ks_strB_add(strb, "'", 1);
    } else if (obj->type == ks_T_int) {
        char tmp[100];
        int tmp_p = 0;
        buffmt_int(tmp, &tmp_p, ((ks_int)obj)->v_int, FMTA_NONE);
        ks_strB_add(strb, tmp, tmp_p);
    } else {
        ks_str reprS = kso_torepr(obj);
        ks_strB_add(strb, reprS->chr, reprS->len);
        KSO_DECREF(reprS);
    }
}
// appends str(obj) to the builder
void ks_strB_add_tostr(ks_strB* strb, kso obj) {
    if (obj->type == ks_T_str) {
        ks_strB_add(strb, ((ks_str)obj)->chr, ((ks_str)obj)->len);
    } else if (obj->type == ks_T_int) {
        char tmp[100];
        int tmp_p = 0;

        buffmt_int(tmp, &tmp_p, ((ks_int)obj)->v_int, FMTA_NONE);
        ks_strB_add(strb, tmp, tmp_p);
    } else {
        ks_str reprS = kso_tostr(obj);
        ks_strB_add(strb, reprS->chr, reprS->len);
        KSO_DECREF(reprS);
    }
}

// finish building the string, and return the new string
ks_str ks_strB_finish(ks_strB* strb) {
    strb->cur->chr[strb->cur->len] = '\0';
    return strb->cur;
}


/* printf-style formatting */

// create a new string from C-style printf arguments
ks_str ks_str_new_vcfmt(const char* fmt, va_list ap) {

    // empty string
    if (fmt == NULL || *fmt == '\0') return ks_str_new("");

    // string builder, for building up the value
    ks_strB ksb = ks_strB_create();

    // current formatting flags
    struct fmta F_args = FMTA_NONE;

    // the formatting fields, as a C str. These are the arguments between `%` and the specifier.
    // EXAMPLE: %*i -> `*` as the fmt_args
    // %.*s -> `.*` as the fmt_args
    char F_fld[256];
    int F_fld_len = 0;

    // a temporary buffer, mainly for constructing integers
    char tmp[256];
    // the current position in the temporary buffer
    int tmp_p;

    // current pointer to the format
    int i, j;
    for (i = 0; fmt[i] != '\0'; ) {

        if (fmt[i] == '%') {
            // we have hit a format specifier, time to parse
            // skip the `%`
            i++;

            // add a literal '%', don't format
            if (fmt[i] == '%') { ks_strB_add(&ksb, "%", 1); i++; continue; }

            // reset the formatting flags
            F_args = FMTA_NONE;

            // start parsing out the formatting arguments
            F_fld_len = 0;
            // parse until we get to an alpha character, which means we have parsed the formatting field
            while (fmt[i] && !isalpha(fmt[i]) && F_fld_len < 100) {
                char ca = fmt[i++];
                F_fld[F_fld_len++] = ca;
                /**/ if (ca == '*') F_args.flags |= FE_STAR;
                else if (ca == '+') F_args.flags |= FE_SIGN;
                else if (ca == '0') F_args.flags |= FE_ZERO;
                else {
                    // maybe emit a warning? unrecocognized field flag
                }
            }

            // NUL-terminate it
            F_fld[F_fld_len] = '\0';

            // get the specifier, then skip it
            char spec = fmt[i++];

            // reset the temporary buffer pointer
            tmp_p = 0;


            /* valid specifiers: 
                i: int
                l: long
                p: void*
                f: float/double
                c: char
                s: char* 
                o: kso, vague (never allocates another string, just uses `<type obj @ addr>` format)

                S: kso, tostring (will turn into string)
                R: kso, repr (turns into its representation)
                T: kso, type (prints out the type name as a string)
            */

            if (spec == 'i') {
                // i: int, print out an integer value, base 10, with sign
                // set the width required
                if (F_args.flags & FE_STAR) F_args.width = va_arg(ap, int);

                // now, read in the argument
                int d_val = va_arg(ap, int);

                // fill a buffer with the contents, then output buffer
                buffmt_int(tmp, &tmp_p, (int64_t)d_val, F_args);
                ks_strB_add(&ksb, tmp, tmp_p);

            } else if (spec == 'l') {
                // l: long, 64 bit signed integer output
                // set the width required
                if (F_args.flags & FE_STAR) F_args.width = va_arg(ap, int);

                // now, read in the argument
                int64_t d_val = va_arg(ap, int64_t);

                // fill a buffer with the contents, then output buffer
                buffmt_int(tmp, &tmp_p, d_val, F_args);
                ks_strB_add(&ksb, tmp, tmp_p);

            } else if (spec == 'p') {
                // `p` for pointer, void*, print as a hex address
                F_args.base = 16;

                uintptr_t addr = va_arg(ap, uintptr_t);

                ks_strB_add(&ksb, "0x", 2);

                // print out the address as a hex number
                buffmt_int(tmp, &tmp_p, addr, F_args);
                ks_strB_add(&ksb, tmp, tmp_p);
                
            } else if (spec == 'f') {
                // f: float, print out floating point value to some digits
                // %f -> float, base 10, to 4 digits (which will be 0-filled)
                // %+f -> float, with sign always prepended

                // NOTE: as per the C spec, calling with float or double is always promoted to double:
                //   https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
                double f_val = va_arg(ap, double);

                // output the basic value
                buffmt_double(tmp, &tmp_p, f_val, F_args);
                ks_strB_add(&ksb, tmp, tmp_p);

            } else if (spec == 'c') {
                // c for character, print a single character
 
                // how many times should the character be printed?
                int n_chars = 1;

                // if a variable number of characters should be printed, read it in
                if (F_args.flags & FE_STAR) n_chars = va_arg(ap, int);

                // get the char
                char c_val = (char)(va_arg(ap, int));

                int i;
                for (i = 0; i < n_chars; ++i) {
                    ks_strB_add(&ksb, &c_val, 1);
                }

            } else if (spec == 's') {
                // s for string, print a C-style NUL-terminated string

                // argument length
                int n_chars = -1;

                // read in length, %*s -> len, str
                if (F_args.flags & FE_STAR) n_chars = va_arg(ap, int);

                const char* s_val = va_arg(ap, const char*);

                // we calculate how many characters we need
                if (n_chars < 0) n_chars = strlen(s_val);

                ks_strB_add(&ksb, (char*)s_val, n_chars);

            } else if (spec == 'o') {
                // 'o' for object, print out just the generic object format,
                // and will never recurse or call another function
                kso o_val = va_arg(ap, kso);

                ks_strB_add(&ksb, "<'", 2);
                ks_strB_add(&ksb, o_val->type->name->chr, o_val->type->name->len);

                ks_strB_add(&ksb, "' obj @ 0x", 10);

                struct fmta addr_fmt = FMTA_NONE;
                addr_fmt.base = 16;
                buffmt_int(tmp, &tmp_p, (intptr_t)o_val, addr_fmt);
                ks_strB_add(&ksb, tmp, tmp_p);

                ks_strB_add(&ksb, ">", 1);


            } else if (spec == 'S') {
                // 'S' for string, print the kscript object as its tostring
                kso o_val = va_arg(ap, kso);

                ks_strB_add_tostr(&ksb, o_val);

            } else if (spec == 'R') {
                // 'R' for representation, print the kscript object as its representation
                kso o_val = va_arg(ap, kso);

                ks_strB_add_repr(&ksb, o_val);
            } else if (spec == 'T') {
                // 'T' for type, print the name of the type
                kso o_val = va_arg(ap, kso);

                ks_strB_add(&ksb, o_val->type->name->chr, o_val->type->name->len);



            } else {
                // take an argument off just in case, this may prevent an error
                va_arg(ap, void*);

                // add a literal ! to signify an error
                ks_strB_add(&ksb, "!", 1);

                fprintf(stderr, "Unsupported format code %c used\n", spec);
            }

        } else {
            int s_i = i;
            // else, go through, scan for all the literal values, until we hit the end or format specifier `%`
            for (; fmt[i] != '\0' && fmt[i] != '%'; ++i) ;

            // append them here

            ks_strB_add(&ksb, (char*)(fmt+s_i), i - s_i);

        }
    }

    // TODO: try and intern the string
    return ks_strB_finish(&ksb);
}


// create a new string from k-script style formatting
ks_str ks_str_new_kfmt(ks_str kfmt, ks_tuple args) {

    // string builder, for building up the value
    ks_strB ksb = ks_strB_create();

    // current formatting flags
    struct fmta F_args = FMTA_NONE;

    // the formatting fields, as a C str. These are the arguments between `%` and the specifier.
    // EXAMPLE: %*i -> `*` as the fmt_args
    // %.*s -> `.*` as the fmt_args
    char F_fld[256];
    int F_fld_len = 0;

    // a temporary buffer, mainly for constructing integers
    char tmp[256];
    // the current position in the temporary buffer
    int tmp_p;

    // current index into arguments
    int args_i = 0;


    // get a C-style string
    char* fmt = kfmt->chr;

    // current pointer to the format
    int i, j;
    for (i = 0; fmt[i] != '\0'; ) {

        if (fmt[i] == '%') {
            // we have hit a format specifier, time to parse
            // skip the `%`
            i++;

            // add a literal '%', don't format
            if (fmt[i] == '%') { ks_strB_add(&ksb, "%", 1); i++; continue; }

            // reset the formatting flags
            F_args = FMTA_NONE;

            // start parsing out the formatting arguments
            F_fld_len = 0;
            // parse until we get to an alpha character, which means we have parsed the formatting field
            while (fmt[i] && !isalpha(fmt[i]) && F_fld_len < 100) {
                char ca = fmt[i++];
                F_fld[F_fld_len++] = ca;
                /**/ if (ca == '*') F_args.flags |= FE_STAR;
                else if (ca == '+') F_args.flags |= FE_SIGN;
                else if (ca == '0') F_args.flags |= FE_ZERO;
                else {
                    // maybe emit a warning? unrecocognized field flag
                }
            }

            // NUL-terminate it
            F_fld[F_fld_len] = '\0';

            // get the specifier, then skip it
            char spec = fmt[i++];

            // reset the temporary buffer pointer
            tmp_p = 0;

            if (args_i >= args->len) {
                KSO_DECREF(ks_strB_finish(&ksb));
                return kse_fmt("Extra format specifier (only had %i arguments, but %i requested)", (int)args->len, args_i);
            }

            kso obj = args->items[args_i++];


            if (spec == 's') {

                ks_strB_add_tostr(&ksb, obj);

            } else {
                // unknown specifier
                KSO_DECREF(ks_strB_finish(&ksb));
                return kse_fmt("Unknown format specifier '%%%c'", spec);
            }

        } else {
            int s_i = i;
            // else, go through, scan for all the literal values, until we hit the end or format specifier `%`
            for (; fmt[i] != '\0' && fmt[i] != '%'; ++i) ;

            // append them here

            ks_strB_add(&ksb, (char*)(fmt+s_i), i - s_i);

        }
    }
    if (args_i != args->len) {
        KSO_DECREF(ks_strB_finish(&ksb));
        return kse_fmt("Not all format specifiers were converted! (only used %i)", args_i);
    }
    return ks_strB_finish(&ksb);
}


