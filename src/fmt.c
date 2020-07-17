/* fmt.c - implementation of formatting routines
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "ks-impl.h"

// ks_fmt_* - formatting routines for strings
// NOTE: Returns a new reference, or NULL if an error was thrown
ks_str ks_fmt_vc(const char* fmt, va_list ap) {
    ks_str_builder sb = ks_str_builder_new();
    bool rst = ks_str_builder_add_vfmt(sb, fmt, ap);

    ks_str res = ks_str_builder_get(sb);
    KS_DECREF(sb);

    return res;
}

ks_str ks_fmt_c(const char* fmt, ...) {

    va_list ap;
    va_start(ap, fmt);
    ks_str res = ks_fmt_vc(fmt, ap);
    va_end(ap);

    return res;
}


// ks_*printf(...) - prints out, similar to C-style ones, but using the `ks_fmt_*` methods
void ks_vfprintf(FILE* fp, const char* fmt, va_list ap) {
    ks_str fmt_str = ks_fmt_vc(fmt, ap);
    fprintf(fp, "%s", fmt_str->chr);
    KS_DECREF(fmt_str);
}

void ks_printf(const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    ks_vfprintf(stdout, fmt, ap);
    va_end(ap);

}
