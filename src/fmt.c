/* fmt.c - implementation of string formatting rules and functions */

#include "ks.h"

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

    // print out the value in hex
    FE_HEX  = 1<<3,

    // end of the flags
    FE__END

};

// format arguments
struct fmta {
    uint32_t flags;

    // width, if applicable. Usually 0
    int width;

};

#define FMTA_NONE ((struct fmta){.flags = FE_NONE, .width = -1})

// returns the base to print in
#define FMTA_BASE(_fmta) (((_fmta).flags & FE_HEX)?16:10)

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


/* helper macros to format basic data types */


// appends '_val' as an integer to `_buf` (with buffer pointer _bp)
#define FMT_INT(_buf, _bp, _args, _val) { \
    struct fmta args = (struct fmta)(_args); \
    int64_t v = (int64_t)(_val); \
    if (v < 0) { _buf[(_bp)++] = '-'; v = -v; } \
    else if (args.flags & FE_SIGN) _buf[(_bp)++] = '+'; \
    int sbp = _bp; \
    if (args.flags & FE_STAR) { \
        int try = 0; \
        do { \
            if (try > 0 && v == 0) { \
                if (args.flags & FE_ZERO) _buf[(_bp)++] = '0'; \
                else _buf[(_bp)++] = ' '; \
            } else { \
                _buf[(_bp)++] = base_str_digs[v % FMTA_BASE(args)]; \
                v /= FMTA_BASE(args); \
            } \
        } while (++try < args.width || v > 0); \
    } else { \
        do { \
            _buf[(_bp)++] = base_str_digs[v % FMTA_BASE(args)]; \
            v /= FMTA_BASE(args); \
        } while (v > 0); \
    } \
    FMT_REV(_buf, sbp, _bp); \
}

// appends `_val` as a base 10 floating point value to `_buf` (with buffer pointer _bp)
#define FMT_FLOAT(_buf, _bp, _args, _val) { \
    struct fmta args = (struct fmta)(_args); \
    double v = (double)(_val); \
    if (v < 0) { _buf[(_bp)++] = '-'; v = -v; } \
    else if (args.flags & FE_SIGN) _buf[(_bp)++] = '+'; \
    int sbp = _bp; \
    uint64_t v_int = (uint64_t)(v); \
    FMT_INT(_buf, _bp, FMTA_NONE, v_int); \
    FMT_CHAR(_buf, _bp, '.'); \
    uint64_t v_frac = (uint64_t)(10000 * (v - v_int)); \
    struct fmta frac_args = { .flags = FE_ZERO | FE_STAR, .width = 4 }; \
    FMT_INT(_buf, _bp, frac_args, v_frac); \
}


// reverse a buffer, between start and stop points
#define FMT_REV(_buf, _bp_start, _bp_stop) { \
    int j; \
    for (j = _bp_start; 2 * (j - _bp_start) < _bp_stop - _bp_start; ++j) { \
        char tc = _buf[j]; \
        _buf[j] = _buf[_bp_stop - j - 1 + _bp_start]; \
        _buf[_bp_stop - j - 1 + _bp_start] = tc; \
    } \
}

// formats and appends a single character to the buffer
#define FMT_CHAR(_buf, _bp, _c) { \
    _buf[(_bp)++] = (char)(_c); \
}


// create a new string from C-style printf arguments
ks_str ks_str_new_vcfmt(const char* fmt, va_list ap) {

    // the current result
    ks_str self = (ks_str)ks_malloc(sizeof(*self) + 0);
    *self = (struct ks_str) {
        KSO_BASE_INIT(ks_T_str, KSOF_NONE)
        .len = 0,
    };

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

    // digits for bases > 10
    static const char base_digs[] = "0123456789abcdefghijklmnopqrstuv";

    // macro to determine if a character is alpha (part of the alphabet)
    #define ISALPHA(_c) (((_c) >= 'a' && (_c) <= 'z') || ((_c) >= 'A' && (_c) <= 'Z'))

    // append `_num` characters from `_buf` to the result
    #define VCFMT_APPEND(_buf, _num) { \
        int __len = (int)(_num); \
        self = ks_realloc(self, sizeof(*self) + self->len + _num); \
        memcpy(&(self->chr[self->len]), (_buf), __len); \
        self->len += __len;  \
    }

    // current pointer to the format
    int i, j;
    for (i = 0; fmt[i] != '\0'; ) {

        if (fmt[i] == '%') {
            // we have hit a format specifier, time to parse
            // skip the `%`
            i++;

            // reset the formatting flags
            F_args = FMTA_NONE;

            // start parsing out the formatting arguments
            F_fld_len = 0;
            // parse until we get to an alpha character, which means we have parsed the formatting field
            while (fmt[i] && !ISALPHA(fmt[i]) && F_fld_len < 100) {
                char ca = fmt[i++];
                F_fld[F_fld_len++] = ca;
                /**/ if (ca == '*') F_args.flags |= FE_STAR;
                else if (ca == '+') F_args.flags |= FE_SIGN;
                else if (ca == '0') F_args.flags |= FE_ZERO;
                else {
                    // maybe emit a warning?
                }
            }

            // NUL-terminate it
            F_fld[F_fld_len] = '\0';

            // get the specifier, then skip it
            char spec = fmt[i++];

            /* valid specifiers: 
                i: int
                l: long
                p: void*
                f: float/double
                c: char
                s: char* 
                o: kso, vague (never allocates another string, just uses `<type obj @ addr>` format)
                V: kso, value (will turn into string)
            */

            if (spec == 'i') {
                // i: int, print out an integer value, base 10, with sign
                // set the width required
                if (F_args.flags & FE_STAR) F_args.width = va_arg(ap, int);

                // now, read in the argument
                int d_val = va_arg(ap, int);

                // we are outputting the base-10 representation into tmp, reversed
                tmp_p = 0;
                FMT_INT(tmp, tmp_p, F_args, d_val);
                VCFMT_APPEND(tmp, tmp_p);

            } else if (spec == 'l') {
                // l: long, 64 bit signed integer output
                // set the width required
                if (F_args.flags & FE_STAR) F_args.width = va_arg(ap, int);

                // now, read in the argument
                int64_t d_val = va_arg(ap, int64_t);

                // we are outputting the base-10 representation into tmp, reversed
                tmp_p = 0;
                FMT_INT(tmp, tmp_p, F_args, d_val);
                VCFMT_APPEND(tmp, tmp_p);

            } else if (spec == 'p') {
                // `p` for pointer, void*, print as a hex address
                F_args.flags |= FE_HEX;

                uintptr_t addr = va_arg(ap, uintptr_t);

                tmp_p = 0;
                FMT_INT(tmp,tmp_p, F_args, addr);

                VCFMT_APPEND("0x", 2);
                VCFMT_APPEND(tmp, tmp_p);
                
            } else if (spec == 'f') {
                // f: float, print out floating point value to some digits
                // %f -> float, base 10, to 4 digits (which will be 0-filled)
                // %+f -> float, with sign always prepended

                // NOTE: as per the C spec, calling with float or double is always promoted to double:
                //   https://stackoverflow.com/questions/11270588/variadic-function-va-arg-doesnt-work-with-float
                double f_val = va_arg(ap, double);

                tmp_p = 0;
                FMT_FLOAT(tmp, tmp_p, F_args, f_val);
                VCFMT_APPEND(tmp, tmp_p);

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
                    VCFMT_APPEND(&c_val, 1);
                }

            } else if (spec == 's') {
                // s for string, print a C-style NUL-terminated string

                // argument length
                int n_chars = -1;

                // read in length
                if (F_args.flags & FE_STAR) n_chars = va_arg(ap, int);

                const char* s_val = va_arg(ap, const char*);

                // we calculate how many characters we need
                if (n_chars < 0) n_chars = strlen(s_val);

                VCFMT_APPEND(s_val, n_chars);

            } else if (spec == 'o') {
                // 'o' for object, print out just the generic object format,
                // and will never recurse or call another function
                kso o_val = va_arg(ap, kso);

                VCFMT_APPEND("<'", 2);
                VCFMT_APPEND(o_val->type->name->chr, o_val->type->name->len);

                VCFMT_APPEND("' obj @ 0x", 10);

                tmp_p = 0;
                struct fmta addr_fmt = FMTA_NONE;
                addr_fmt.flags |= FE_HEX;
                FMT_INT(tmp, tmp_p, addr_fmt, o_val);
                VCFMT_APPEND(tmp, tmp_p);

                VCFMT_APPEND(">", 1);

            } else if (spec == 'V') {
                // 'V' for value, print the kscript object as its tostring
                kso o_val = va_arg(ap, kso);

                ks_str to_str = kso_tostr(o_val);

                // now, print out the string
                VCFMT_APPEND(to_str->chr, to_str->len);

                KSO_CHKREF(to_str);

            } else if (spec == 'R') {
                // 'R' for representation, print the kscript object as its representation
                kso o_val = va_arg(ap, kso);

                ks_str to_str = kso_torepr(o_val);

                // now, print out the string
                VCFMT_APPEND(to_str->chr, to_str->len);

                KSO_CHKREF(to_str);

            } else {
                // take an argument off just in case, this may prevent an error
                va_arg(ap, void*);

                // add a literal ! to signify an error
                tmp_p = 0;
                FMT_CHAR(tmp, tmp_p, '!');
                VCFMT_APPEND(tmp, tmp_p);

                fprintf(stderr, "Unsupported format code %c used\n", spec);
            }

        } else {
            int s_i = i;
            // else, go through, scan for all the literal values, until we hit the end or format specifier `%`
            for (; fmt[i] != '\0' && fmt[i] != '%'; ++i) ;

            // append them here
            VCFMT_APPEND(&fmt[s_i], (i - s_i));

        }
    }

    // NUL-terminate it
    self->chr[self->len] = '\0';

    // calculate hash
    self->v_hash = ks_hash_bytes(self->chr, self->len);

    // TODO: try and intern the string
    return self;
}


