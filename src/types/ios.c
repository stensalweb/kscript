/* types/iostream.c - implementation of the input/output stream class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// Create a blank 'iostream', with no target
// NOTE: Use `ks_ios_open()` to actually get a target
// NOTE: Returns a new reference
ks_ios ks_ios_new() {
    ks_ios self = KS_ALLOC_OBJ(ks_ios);
    KS_INIT_OBJ(self, ks_T_ios);

    // set universal members
    self->mode = ks_str_new("");
    self->name = ks_str_new("");
    self->isOpen = false;
    self->isExtern = false;


    // specifics
    self->_fp = NULL;

    return self;

}

// Open 'self' to the given file and mode
// NOTE: Returns success, or false and throws an error
bool ks_ios_open(ks_ios self, ks_str fname, ks_str mode) {
    if (self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to open ios which was already open");
        return false;
    }

    // TODO: may need to encode `fname` to utf16 for windows

    self->_fp = fopen(fname->chr, mode->chr);
    if (!self->_fp) {
        // error
        ks_throw(ks_T_IOError, "Failed to open '%S': %s", fname, strerror(errno));
        return false;
    }

    self->isOpen = true;
    self->isExtern = false;


    // swap out references
    KS_INCREF(fname);
    KS_INCREF(mode);
    KS_DECREF(self->name);
    KS_DECREF(self->mode);
    self->name = fname;
    self->mode = mode;

    // success
    return true;
}

// Open 'self' as an extern 
// NOTE: `fname` is not used for opening any files! It is only used as a decoration
//   if it is NULL, then one is generated
// NOTE: `mode` should simply describe the mode _fp was opened in
// NOTE: Returns success, or false and throws an error
bool ks_ios_extern_FILE(ks_ios self, ks_str fname, ks_str mode, FILE* _fp) {
    if (self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to open ios which was already open");
        return false;
    }

    self->_fp = _fp;

    self->isOpen = true;
    self->isExtern = true;

    // swap out references
    KS_INCREF(fname);
    KS_INCREF(mode);
    KS_DECREF(self->name);
    KS_DECREF(self->mode);
    self->name = fname;
    self->mode = mode;

    // success
    return true;
}

// Close 'self' (or, if already closed, do nothing)
// NOTE: will never throw an error; all errors are ignored
void ks_ios_close(ks_ios self) {

    // short circuit
    if (!self->isOpen) return;

    // actually close if not extern
    if (!self->isExtern) {

        ks_trace("ks", "Closing file %S (_fp: %p)", self, self->_fp);
        fclose(self->_fp);
    }

    self->isOpen = false;
    self->isExtern = false;
    self->_fp = NULL;
    
    ks_str empt = ks_str_new("");
    KS_INCREF(empt);

    KS_DECREF(self->name);
    KS_DECREF(self->mode);
    self->name = self->mode = empt;

}

// Return the current position (in bytes) in 'self', or -1 and throw an error
ks_ssize_t ks_ios_tell(ks_ios self) {
    if (!self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to get 'tell()' of an ios that was not open");
        return -1;
    }

    ks_ssize_t ret = ftell(self->_fp);
    if (ret < 0) ks_throw(ks_T_IOError, "Failed to get 'tell()': %s", strerror(errno));
    return ret;
}

// Return the size (in bytes) of the entire stream, or -1 and throw an error
ks_ssize_t ks_ios_size(ks_ios self) {
    if (!self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to get 'size()' of an ios that was not open");
        return -1;
    }

    ks_ssize_t pos = ftell(self->_fp);
    if (pos < 0) {
        ks_throw(ks_T_IOError, "Failed to get 'size()': %s", strerror(errno));
        return -1;
    } else {
        fseek(self->_fp, 0, SEEK_END);
        ks_ssize_t endpos = ftell(self->_fp);
        if (endpos < 0) {
            ks_throw(ks_T_IOError, "Failed to get 'size()': %s", strerror(errno));
            return -1;
        }

        fseek(self->_fp, pos, SEEK_SET);

        return endpos;
    }
}


// Attempt to seek `self` to given position, in mode `seekmode` (see KS_IOS_SEEK_* enum definitions)
// NOTE: everything is in bytes here
bool ks_ios_seek(ks_ios self, ks_ssize_t pos_b, int seekmode) {
    if (!self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to get seek in file that wasn't open");
        return -1;
    }

    // convert to C seek mode
    int _seek = -1;
    if (seekmode == KS_IOS_SEEK_SET) {
        _seek = SEEK_SET;
    } else if (seekmode == KS_IOS_SEEK_CUR) {
        _seek = SEEK_CUR;
    } else if (seekmode == KS_IOS_SEEK_END) {
        _seek = SEEK_END;
    } else {
        ks_throw(ks_T_ArgError, "Unknown seekmode %i given", seekmode);
        return false;
    }

    // now, seek to the given position


    ks_ssize_t ret = fseek(self->_fp, pos_b, _seek);
    if (ret < 0) {
        ks_throw(ks_T_ArgError, "Failed to seek in file: %s", strerror(errno));

    }

    return ret;
}

// Read a given number of bytes from the iostream and put them into `dest`
// NOTE: Returns number of bytes actually read (accounting for truncation), or -1 and throw an error
ks_ssize_t ks_ios_readb(ks_ios self, ks_ssize_t len_b, void* dest) {
    if (!self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to read from file that wasn't open");
        return -1;
    }

    ks_ssize_t byt = fread(dest, 1, len_b, self->_fp);
    if (byt < 0) {
        // handle discrepency
    } else if (byt != len_b) {
        // handle discrepancy
    }

    return byt;
}

// Read a given number of characters from the iostream and put them into `dest` (encoded as utf8)
// NOTE: Returns the number of bytes read into 'dest'; note: `dest` may be reallocated via `ks_realloc()`
// Example:
// void* dest = NULL;
// ks_ios_reads(self, 4, &dest);
// ks_free(dest);
ks_ssize_t ks_ios_reads(ks_ios self, ks_ssize_t len_c, void** dest, ks_ssize_t* len_c_actual) {
    if (!self->isOpen) {
        ks_throw(ks_T_IOError, "Attempted to read from file that wasn't open");
        return *len_c_actual = -1;
    }
    
    // number of bytes read
    ks_ssize_t len_b = *len_c_actual = 0;

    // holds single utf8 sequence
    uint8_t tmp[4];

    // whethe or not to keep going
    bool keepGoing = true;

    ks_size_t cur_dest_size = 0;

    while (*len_c_actual < len_c && keepGoing) {
        // read an entire character from the stream
        // TODO: allow other than utf8

        int i;
        for (i = 0; i < 4; ++i) {
            ks_ssize_t read = fread(tmp + i, 1, 1, self->_fp);
            if (read != 1) {
                // end of sequence
                keepGoing = false;
                break;
            }

            if (tmp[i] & 0xC0 == 0x80) {
                // we are done
                break;
            }
        }

        // exit early
        if (!keepGoing) break;

        // append to destination
        *dest = ks_realloc(*dest, cur_dest_size + i + 1);
        memcpy((void*)(((intptr_t)*dest) + cur_dest_size), tmp, i);
        cur_dest_size += i;

        // inc counters
        *len_c_actual++;
        len_b += i;
    }

    // NUL-terminate it
    if (*dest) ((char*)*dest)[cur_dest_size] = '\0';

    return len_b;
}

// ios.__new__(typ, fname, mode="r")
static KS_TFUNC(ios, new) {
    ks_type typ;
    ks_str fname;
    ks_str mode;
    KS_GETARGS("typ:* fname:* mode:*", &typ, ks_T_type, &fname, ks_T_str, &mode, ks_T_str)

    ks_ios self = ks_ios_new();
    if (!ks_ios_open(self, fname, mode)) {
        KS_DECREF(self);
        return NULL;
    }

    return (ks_obj)self;
}

// ios.__str__(self)
static KS_TFUNC(ios, str) {
    ks_ios self;
    KS_GETARGS("self:*", &self, ks_T_ios)

    return (ks_obj)ks_fmt_c("<ios name=%R, mode=%R>", self->name, self->mode);
}


// ios.__free__(self) - free obj
static KS_TFUNC(ios, free) {
    ks_ios self;
    KS_GETARGS("self:*", &self, ks_T_ios)

    // close the stream
    ks_ios_close(self);

    KS_DECREF(self->mode);
    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}


// ios.close(self) - close the stream
static KS_TFUNC(ios, close) {
    ks_ios self;
    KS_GETARGS("self:*", &self, ks_T_ios)

    ks_ios_close(self);

    return KSO_NONE;
}


// ios.reads(self, numc=none) - read a given number of characters (defaulting to the entire file)
static KS_TFUNC(ios, reads) {
    ks_ios self;
    ks_obj numc = KSO_NONE;
    KS_GETARGS("self:* ?numc", &self, ks_T_ios, &numc)

    ks_ssize_t len_c = KS_SSIZE_MAX;

    if (numc == KSO_NONE) {

    } else {
        // extra number of characters
        int64_t v64;
        if (!ks_num_get_int64(numc, &v64)) return NULL;
        len_c = v64;
    }

    void* dest = NULL;
    ks_ssize_t len_c_actual = 0;
    ks_ssize_t len_b = ks_ios_reads(self, len_c, &dest, &len_c_actual);
    if (len_b < 0) {
        ks_free(dest);
        return NULL;
    } else {
        ks_str ret = ks_str_utf8((const char*)dest, len_b);
        ks_free(dest);
        return (ks_obj)ret;
    }
}







/* export */

KS_TYPE_DECLFWD(ks_T_ios);

void ks_init_T_ios() {
    
    ks_type_init_c(ks_T_ios, "ios", ks_T_obj, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c(ios_new_, "ios.__new__(fname, mode='r'")},
        {"__free__",               (ks_obj)ks_cfunc_new_c(ios_free_, "ios.__free__(self)")},
        {"__str__",                (ks_obj)ks_cfunc_new_c(ios_str_, "ios.__str__(self)")},
        {"__repr__",               (ks_obj)ks_cfunc_new_c(ios_str_, "ios.__repr__(self)")},

        {"reads",                  (ks_obj)ks_cfunc_new_c(ios_reads_, "ios.reads(self, numc=none)")},
        {"close",                  (ks_obj)ks_cfunc_new_c(ios_close_, "ios.close(self)")},
        
    ));

    ks_T_ios->flags |= KS_TYPE_FLAGS_EQSS;
}
