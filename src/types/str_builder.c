/* str_builder.c - string builder API
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"



// describes a single instance of a formatter
typedef struct {

    // whether or not it is left aligned output
    bool isLeft;

    // has a '+' specifier, which tells us to always output a sign
    bool hasPlus;

    // has a ' ' specifier, which means we should add a space to positive values,
    //   and a negative sign to negative values
    bool hasSpace;

    // has a '0' flag, which tells to zero pad to the left if width is higher than the actual 
    bool hasZero;

    // the minimum width to output
    int width;

    // the base to print in
    int base;


} ksfmt_t;

#define KSFMT_DFT ((ksfmt_t) { .isLeft = false, .hasPlus = false, .hasSpace = false, .hasZero = false, .width = 0, .base = 10 })

// construct a new string builder
ks_str_builder ks_str_builder_new() {
    ks_str_builder self = KS_ALLOC_OBJ(ks_str_builder);
    KS_INIT_OBJ(self, ks_T_str_builder);


    self->len = 0;
    self->data = NULL;

    return self;

}

// Add raw bytes to the string builder
// NOTE: Returns success
bool ks_str_builder_add(ks_str_builder self, void* data, ks_size_t len) {

    int idx = self->len;
    self->len += len;

    self->data = ks_realloc(self->data, self->len);

    memcpy((void*)((intptr_t)self->data + idx), data, len);

    return true;
}


// Add str(obj) to the string builder
bool ks_str_builder_add_str(ks_str_builder self, ks_obj obj) {

    if (obj->type == ks_T_str) {
        return ks_str_builder_add(self, ((ks_str)obj)->chr, ((ks_str)obj)->len_b);
    } else if (obj->type->__str__ != NULL) {
        ks_str obj_str = (ks_str)ks_obj_call(obj->type->__str__, 1, &obj);
        if (!obj_str) return false;
        bool rst = ks_str_builder_add_str(self, (ks_obj)obj_str);
        KS_DECREF(obj_str);
        return rst;
    } else {
        return ks_str_builder_add_fmt(self, "%O", obj);
    }
}


// return true if 'c' is a valid part of the field specifier, which means
//   it is not alpha character
static bool ks_is_field(char c) {
    return c && !isalpha(c) && c != '%';
}

// digits used for base formatting
static const char bfmt_digs[] = "0123456789abcdef";

// internal buffer reversing on a buffer, reverses '0' through 'n-1' (inclusive)
static void bfmt_rev(char* buf, int n) {
    int i;
    for (i = 0; 2 * i < n; ++i) {
        char tmp = buf[i];
        buf[i] = buf[n - i - 1];
        buf[n - i - 1] = tmp;
    }
}


static bool add_i64(ks_str_builder sb, ksfmt_t ksfmt, int64_t val) {
    char buf[256];

    // buffer pointer
    int bp = 0;

    bool is_neg = val < 0;
    if (is_neg) val = -val;

    // ensure the base is representable
    assert (ksfmt.base <= sizeof(bfmt_digs) - 1);

    // add digits, which will be in reverse order
    do {
        buf[bp++] = bfmt_digs[val % ksfmt.base];
        val /= ksfmt.base;
    } while (bp < sizeof(buf) - 1 && val > 0);

    /*if (arg.width > 0) while (bp < n - 1 && bp < arg.width) {
        buf[bp++] = arg.zero_pad ? '0' : ' ';
    }*/


    // add negative sign to the end, which will be reversed
    if (is_neg) buf[bp++] = '-';
    else {
        if (ksfmt.hasPlus) buf[bp++] = '+';
        else if (ksfmt.hasSpace) buf[bp++] = ' ';
    }
    // reverse the buffer, since digits come out in opposite order
    bfmt_rev(buf, bp);

    ks_str_builder_add(sb, buf, bp);

    // the next character would be here
    return true;
}


// add format
// based roughly on: https://en.wikipedia.org/wiki/Printf_format_string
bool ks_str_builder_add_vfmt(ks_str_builder self, const char* fmt, va_list ap) {

    // maximum field size
    #define MAX_FIELD 256

    // field temporary
    char field[MAX_FIELD];

    // return status
    bool rst = false;

    // keep looping while we have more formatting strings to go
    while (*fmt) {

        // starting place
        const char* sp = fmt;

        // advance the formatter to the next '%' (or the end)
        while (*fmt && fmt[0] != '%') fmt++;

        // add those bytes to the builder
        ks_str_builder_add(self, (void*)sp, fmt - sp);

        // break out if we hit the end
        if (!*fmt) break;

        // skip '%'
        fmt++;

        // if we have '%%', just do a single one and keep on going
        if (*fmt == '%') {
            ks_str_builder_add(self, "%", 1);
            fmt++;
            continue;
        }


        // current pos into 'field'
        int i = 0;

        // otherwise, it is '%<field><c>', so we need to parse the field
        while (ks_is_field(*fmt) && i < MAX_FIELD - 1) {
            field[i++] = *fmt++;
        }
        field[i] = '\0';

        // now, get the single character formatter
        char c = *fmt++;


        ksfmt_t ksfmt = KSFMT_DFT;
        ksfmt.hasZero = strchr(field, '0') != NULL;
        ksfmt.hasPlus = strchr(field, '+') != NULL;
        ksfmt.hasSpace = strchr(field, ' ') != NULL;
        ksfmt.isLeft = strchr(field, '-') != NULL;

        // read width
        if (strchr(field, '*') != NULL) {
            ksfmt.width = va_arg(ap, int);
        } else {
            ksfmt.width = 0;
        }


        if (c == 'i') {
            // %i -> base 10 integer, from C 'int'
            int val = va_arg(ap, int);

            add_i64(self, ksfmt, val);

        } else if (c == 'l') {
            // %i -> base 10 integer, from C 'int'
            int64_t val = va_arg(ap, int64_t);

            add_i64(self, ksfmt, val);

        } else if (c == 'p') {
            // %i -> base 10 integer, from C 'int'
            void* val = va_arg(ap, void*);

            ksfmt.base = 16;

            ks_str_builder_add(self, "0x", 2);
            add_i64(self, ksfmt, (int64_t)val);

        } else if (c == 's') {
            // %i -> base 10 integer, from C 'int'
            char* val = va_arg(ap, char*);

            if (ksfmt.width < 1) ksfmt.width = strlen(val);

            ks_str_builder_add(self, val, ksfmt.width);


        } else if (c == 'O') {
            // %O -> add a simplified object format
            ks_obj val = va_arg(ap, ks_obj);

            // attempt to add it
            if (!ks_str_builder_add_fmt(self, "<'%T' obj @ %p>", val, val)) {
                goto vfmt_end;
            }


        } else if (c == 'T') {
            // %T -> add just the type name of an object
            ks_obj val = va_arg(ap, ks_obj);

            ks_str_builder_add(self, val->type->__name__->chr, val->type->__name__->len_b);


        } else if (c == 'S') {
            // %S -> add str(obj) to the result
            ks_obj val = va_arg(ap, ks_obj);

            // attempt to add it
            if (!ks_str_builder_add_str(self, val)) {
                goto vfmt_end;
            }



        } else {
            printf("UNKNOWN FMT SPECIFIER\n");
            goto vfmt_end;

        }


    }

    rst = true;
    vfmt_end: ;

    return rst;

}

// add format
bool ks_str_builder_add_fmt(ks_str_builder self, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    bool rst = ks_str_builder_add_vfmt(self, fmt, ap);
    va_end(ap);
    return rst;
}


// return current string
ks_str ks_str_builder_get(ks_str_builder self) {
    return ks_str_new_c(self->data, self->len);
}

// str_builder.__free__(self) -> free obj
static KS_TFUNC(str_builder, free) {
    ks_str_builder self;
    if (!ks_getargs(n_args, args, "self:*", &self, ks_T_str_builder)) return false;

    ks_free(self->data);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


/* export */

KS_TYPE_DECLFWD(ks_T_str_builder);

void ks_init_T_str_builder() {

    ks_type_init_c(ks_T_str_builder, "str_builder", ks_T_obj, KS_KEYVALS(
        {"__free__",               (ks_obj)ks_cfunc_new_c(str_builder_free_, "str_builder.__free__(self)")},
    ));

}
