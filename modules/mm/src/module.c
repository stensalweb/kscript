/* mm/src/module.c - the kscript's multi-media library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "mm"

// include this since this is a module.
#include "ks-module.h"

// mm.read_file(fname) -> read an entire file (by default, as a blob)
static KS_TFUNC(mm, read_file) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str fname;
    if (!ks_parse_params(n_args, args, "fname%s", &fname)) return NULL;

    ks_iostream ios = ks_iostream_new();

    // attempt to open the file
    if (!ks_iostream_open(ios, fname->chr, "rb")) {
        KS_DECREF(ios);
        return NULL;
    }

    ks_ssize_t sz_ios = ks_iostream_size(ios);
    if (sz_ios < 0) {
        KS_DECREF(ios);
        return NULL;
    }


    // read data
    ks_blob res = ks_iostream_readblob_n(ios, sz_ios);

    if (!res) {
        KS_DECREF(ios);
        return NULL;
    }

    return (ks_obj)res;
}



// now, export them all
static ks_module get_module() {
    
    ks_module mod_nx = ks_module_import("nx");
    if (!mod_nx) {
        ks_catch_ignore();
        return ks_throw_fmt(ks_type_InternalError, "`mm` module requires the `nx` library, but it could not be found!");
    } else {
        KS_DECREF(mod_nx);
    }


    ks_module mod = ks_module_new(MODULE_NAME);

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]) {

        {"read_file",      (ks_obj)ks_cfunc_new2(mm_read_file_, "mm.read_file(fname)")},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
