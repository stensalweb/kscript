/* types/iostream.c - implementation of the input/output stream class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_iostream);


// return IOS flags from a given mode, or 0 if there was an error
static uint32_t mode_to_flags(char* mode) {

    /**/ if (strcmp(mode, "r") == 0) return KS_IOS_READ;
    else if (strcmp(mode, "rb") == 0) return KS_IOS_READ | KS_IOS_BINARY;
    else if (strcmp(mode, "w") == 0) return KS_IOS_WRITE;
    else if (strcmp(mode, "wb") == 0) return KS_IOS_WRITE | KS_IOS_BINARY;
    else if (strcmp(mode, "rw") == 0) return KS_IOS_READ | KS_IOS_WRITE;

    ks_throw_fmt(ks_type_ArgError, "Invalid mode for iostream: '%s'", mode);
    return 0;
}

// Create a blank iostream
ks_iostream ks_iostream_new() {
    ks_iostream self = KS_ALLOC_OBJ(ks_iostream);
    KS_INIT_OBJ(self, ks_type_iostream);

    // initialize type-specific things
    self->ios_flags = KS_IOS_NONE;
    self->fp = NULL;

    return self;
}

// Create a wrapper around an external file stream
ks_iostream ks_iostream_new_extern(FILE* fp, char* mode) {
    uint32_t ios_flags = mode_to_flags(mode);
    if (!ios_flags) return NULL;

    ios_flags |= KS_IOS_OPEN;
    
    ks_iostream self = KS_ALLOC_OBJ(ks_iostream);
    KS_INIT_OBJ(self, ks_type_iostream);

    // initialize type-specific things
    self->ios_flags = ios_flags;
    self->fp = fp;

    return self;
}

// open an 'iostream' with a given target, and mode
// returns success
bool ks_iostream_open(ks_iostream self, char* fname, char* mode) {
    if (self->ios_flags & KS_IOS_OPEN) {
        ks_throw_fmt(ks_type_IOError, "Attempting to open iostream, but it was already open!");
        return false;
    }

    // reset
    self->ios_flags = mode_to_flags(mode);
    if (!self->ios_flags) return NULL;

    self->fp = fopen(fname, mode);

    if (!self->fp) {
        self->ios_flags = KS_IOS_NONE;
        ks_throw_fmt(ks_type_IOError, "Failed to open '%s': %s", fname, strerror(errno));
        return false;
    }

    self->ios_flags |= KS_IOS_OPEN;

    // success
    return true;
}

// Attempt to change the mode
bool ks_iostream_change(ks_iostream self, char* mode) {
    uint32_t flags = mode_to_flags(mode);
    if (!flags) return false;

    if (!self->fp || !(self->ios_flags & KS_IOS_OPEN)) {
        ks_throw_fmt(ks_type_IOError, "Failing to reopen %R; it isn't currently open", self);
        return false;
    }

    FILE* new_fp = freopen(NULL, mode, self->fp);

    if (!new_fp) {
        ks_throw_fmt(ks_type_IOError, "Failed to reopen %R: %s", self, strerror(errno));
        return false;
    } else {
        self->fp = new_fp;
        self->ios_flags = flags | KS_IOS_OPEN;
    }

    return true;
}

// Return an entire line
ks_str ks_iostream_getline(ks_iostream self, char* delims) {
    int c;

    if (!(self->ios_flags & KS_IOS_OPEN)) return ks_throw_fmt(ks_type_IOError, "Attempted to read line from iostream that was not open!");
    if (!(self->ios_flags & KS_IOS_READ)) return ks_throw_fmt(ks_type_IOError, "Attempted to read line from iostream that was not open for reading!");

    // release GIL
    ks_GIL_unlock();

    ks_str_b SB;
    ks_str_b_init(&SB);

    // keep reading characters until newline is hit
    while ((c = fgetc(self->fp)) != EOF && strchr(delims, c) == NULL) {
        // append character
        ks_str_b_add(&SB, 1, (char*)&c);
    }

    // claim back
    ks_GIL_lock();
    
    ks_str res = ks_str_b_get(&SB);
    ks_str_b_free(&SB);
    return res;
}


// close an iostream
bool ks_iostream_close(ks_iostream self) {
    if (!(self->ios_flags & KS_IOS_OPEN)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to close an iostream that was not open!");
        return false;
    }

    fclose(self->fp);
    self->fp = NULL;
    self->ios_flags = KS_IOS_NONE;

    return true;
}

// read a string of a given size from the iostream
ks_str ks_iostream_readstr_n(ks_iostream self, ks_ssize_t sz) {

    if (!(self->ios_flags & KS_IOS_OPEN)) return ks_throw_fmt(ks_type_IOError, "Attempted to read string from iostream that was not open!");
    if (!(self->ios_flags & KS_IOS_READ)) return ks_throw_fmt(ks_type_IOError, "Attempted to read string from iostream that was not open for reading!");

    // allocate temporary buffer
    char* tmp = ks_malloc(sz + 1);

    // allow other things access
    ks_GIL_unlock();

    size_t actual_bytes = fread(tmp, 1, sz, self->fp);
    if (actual_bytes != sz) {
        // discrepancy
        //ks_warn("Problem reading string!");
    }

    tmp[actual_bytes] = '\0';

    // acquire back
    ks_GIL_lock();

    ks_str res = ks_str_new(tmp);
    ks_free(tmp);

    return res;

}

// write a C-style string into an iostream, return success
bool ks_iostream_writestr_c(ks_iostream self, char* data, int len) {
    if (!(self->ios_flags & KS_IOS_OPEN)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to write string to iostream that was not open!");
        return false;
    }
    if (!(self->ios_flags & KS_IOS_WRITE)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to write string to iostream that was not open for writing!");
        return false;
    }

    // release GIL
    ks_GIL_unlock();

    size_t actual_bytes = fwrite(data, 1, len, self->fp);
    if (actual_bytes != len) {
        // discrepancy
    }

    // acquire back
    ks_GIL_lock();

    return true;

}
// write a string into an iostream, return success
bool ks_iostream_writestr(ks_iostream self, ks_str data) {
    return ks_iostream_writestr_c(self, data->chr, data->len);
}



// Return the current position in the IOstream, or -1 if there was an error (and throw an error in that case)
ks_ssize_t ks_iostream_tell(ks_iostream self) {
    if (!(self->ios_flags & KS_IOS_OPEN)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to get 'tell()' of an iostream that was not open!");
        return -1;
    }

    return (ks_ssize_t)ftell(self->fp);
}

// Seek to a given position in the file, as an absolute offset
// NOTE: Returns -1 if there was an error
ks_ssize_t ks_iostream_seek(ks_iostream self, ks_ssize_t pos) {
    if (!(self->ios_flags & KS_IOS_OPEN)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to 'seek()' of an iostream that was not open!");
        return -1;
    }

    return (ks_ssize_t)fseek(self->fp, pos, SEEK_SET);
}

// Get the size of the I/O stream, in bytes
// NOTE: Returns -1 if there was an error
ks_ssize_t ks_iostream_size(ks_iostream self) {
    if (!(self->ios_flags & KS_IOS_OPEN)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to get 'size()' of an iostream that was not open!");
        return -1;
    }

    if (!(self->ios_flags & KS_IOS_READ)) {
        ks_throw_fmt(ks_type_IOError, "Attempted to get 'size()' of an iostream that was not open for reading!");
        return -1;
    }

    ks_ssize_t curpos = ftell(self->fp);
    fseek(self->fp, 0, SEEK_END);
    ks_ssize_t endpos = ftell(self->fp);
    fseek(self->fp, curpos, SEEK_SET);

    return endpos;
}


/* member functions */

// iostream.__new__(fname=None, mode=None) -> open a new IOstream
static KS_TFUNC(iostream, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 2);
    
    // new iostream
    ks_iostream self = ks_iostream_new();

    char* my_mode = "r";

    if (n_args == 2) {
        ks_str mode = (ks_str)args[1];
        KS_REQ_TYPE(mode, ks_type_str, "mode");

        my_mode = mode->chr;
    }

    if (n_args == 0) {
        // return blank iostream

    } else {
        ks_str fname = (ks_str)args[0];
        KS_REQ_TYPE(fname, ks_type_str, "fname");
        if (!ks_iostream_open(self, fname->chr, my_mode)) {
            KS_DECREF(self);
            return NULL;
        }
    }

    if (!self) return ks_throw_fmt(ks_type_IOError, "ERR CREATING IOS");

    return (ks_obj)self;
};


// iostream.__free__(self) -> free a stream
static KS_TFUNC(iostream, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");
    
    // only thing is the attribute dictionary
    if (self->fp) fclose(self->fp);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// iostream.__bool__(self) -> convert to a boolean
static KS_TFUNC(iostream, bool) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    // if it's not open, or NULL fp
    if (!(self->ios_flags & KS_IOS_OPEN) || !self->fp) return KSO_FALSE;

    // if its at the end of the file
    if (feof(self->fp)) return KSO_FALSE;

    if (self->ios_flags & KS_IOS_READ) {
        ks_ssize_t sz = ks_iostream_size(self);
        
        if (sz < 0) {
            // handle indefinite streams
            if (ks_thread_get()->exc) return NULL;
            else {
                return KSO_TRUE;
            }
        }


        return KSO_BOOL(ftell(self->fp) < sz);
    } else {
        return KSO_TRUE;
    }
}



// iostream.getline(self, delim='\n') -> return the next line
static KS_TFUNC(iostream, getline) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    char* delim = "\n";

    if (n_args == 2) {
        ks_str delim_o = (ks_str)args[1];
        KS_REQ_TYPE(delim_o, ks_type_str, "delim");

        delim = delim_o->chr;
    }

    return (ks_obj)ks_iostream_getline(self, delim);    
}

// iostream.read(self, nbytes=none) -> read 'n' bytes (or, by default, the entire file)
static KS_TFUNC(iostream, read) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    if (!(self->ios_flags & KS_IOS_OPEN)) return ks_throw_fmt(ks_type_IOError, "Attempted to read string from iostream that was not open!");
    if (!(self->ios_flags & KS_IOS_READ)) return ks_throw_fmt(ks_type_IOError, "Attempted to read string from iostream that was not open for reading!");

    ks_ssize_t nbytes = -1;

    if (n_args > 1) {
        // read in the number of bytes
        if (args[1] != KSO_NONE) {
            int64_t nb;
            if (!ks_parse_params(1, args+1, "nbytes%i64", &nb)) return NULL;
            nbytes = nb;
        }
    }

    // calculate default of entire file
    if (nbytes <= 0) {
        nbytes = ks_iostream_size(self);
        if (nbytes < 0) {
            
            if (ks_thread_get()->exc) return NULL;
            else {
                // read line
                return (ks_obj)ks_iostream_getline(self, "\n");
            }
        }

    }

    return (ks_obj)ks_iostream_readstr_n(self, nbytes);
};


// iostream.write(self, data) -> write an object
static KS_TFUNC(iostream, write) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");
    ks_str data = (ks_str)args[1];
    KS_REQ_TYPE(data, ks_type_str, "str");


    if (!ks_iostream_writestr(self, data)) {
        return NULL;
    }

    return KSO_NONE;
};


// iostream.change(self, mode) -> attempt to change the mode
static KS_TFUNC(iostream, change) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");
    ks_str mode = (ks_str)args[1];
    KS_REQ_TYPE(mode, ks_type_str, "mode");

    if (!ks_iostream_change(self, mode->chr)) {
        return NULL;
    }

    return KS_NEWREF(self);
}


// iostream.size(self) -> return the size of a file, in bytes
static KS_TFUNC(iostream, size) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    ks_size_t ret = ks_iostream_size(self);
    if (ret < 0) return NULL;

    return (ks_obj)ks_int_new(ret);
}

// iostream.tell(self) -> return current position
static KS_TFUNC(iostream, tell) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    ks_size_t ret = ks_iostream_tell(self);
    if (ret < 0) return NULL;

    return (ks_obj)ks_int_new(ret);
}

// iostream.seek(self, pos=0) -> go to a position in the file
static KS_TFUNC(iostream, seek) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    ks_ssize_t posn = 0;
    if (n_args > 1) {
        int64_t nb;
        if (!ks_parse_params(1, args+1, "pos%i64", &nb)) return NULL;
        posn = nb;
    }

    ks_size_t ret = ks_iostream_seek(self, posn);
    if (ret < 0) return NULL;

    return KS_NEWREF(self);
}

// iostream.open(self, fname, mode='r') -> open the file
static KS_TFUNC(iostream, open) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");
    ks_str fname = (ks_str)args[1];
    KS_REQ_TYPE(fname, ks_type_str, "fname");


    // get the mode
    char* my_mode = "r";

    if (n_args == 3) {
        ks_str mode = (ks_str)args[1];
        KS_REQ_TYPE(mode, ks_type_str, "mode");

        my_mode = mode->chr;
    }

    if (!ks_iostream_open(self, fname->chr, my_mode)) {
        KS_DECREF(self);
        return NULL;
    }

    if (!self) return ks_throw_fmt(ks_type_IOError, "ERR CREATING IOS");

    return KSO_NONE;
}

// iostream.flush(self) -> flush the object
static KS_TFUNC(iostream, flush) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    if (!(self->ios_flags & KS_IOS_OPEN)) return ks_throw_fmt(ks_type_IOError, "Attempted to flush iostream that was not open!");

    fflush(self->fp);

    return KSO_NONE;
}

// iostream.close(self) -> close a 
static KS_TFUNC(iostream, close) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    if (!ks_iostream_close(self)) return NULL;

    return KSO_NONE;
}

// initialize cfunc type
void ks_type_iostream_init() {
    KS_INIT_TYPE_OBJ(ks_type_iostream, "iostream");

    ks_type_set_cn(ks_type_iostream, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(iostream_new_, "iostream.__new__(fname=none, mode='r')")},
        {"__free__", (ks_obj)ks_cfunc_new2(iostream_free_, "iostream.__free__(self)")},
        {"__bool__", (ks_obj)ks_cfunc_new2(iostream_bool_, "iostream.__bool__(self)")},

        {"getline", (ks_obj)ks_cfunc_new2(iostream_getline_, "iostream.getline(self, delim='\\n')")},

        {"write", (ks_obj)ks_cfunc_new2(iostream_write_, "iostream.write(self, data)")},
        {"read", (ks_obj)ks_cfunc_new2(iostream_read_, "iostream.read(self, nbytes=none)")},

        {"change", (ks_obj)ks_cfunc_new2(iostream_change_, "iostream.change(self, mode)")},

        {"size", (ks_obj)ks_cfunc_new2(iostream_size_, "iostream.size(self)")},
        {"tell", (ks_obj)ks_cfunc_new2(iostream_tell_, "iostream.tell(self)")},
        {"seek", (ks_obj)ks_cfunc_new2(iostream_seek_, "iostream.seek(self, pos=0)")},
        {"open", (ks_obj)ks_cfunc_new2(iostream_open_, "iostream.open(self, fname, mode='r')")},
        {"flush", (ks_obj)ks_cfunc_new2(iostream_flush_, "iostream.flush(self)")},
        {"close", (ks_obj)ks_cfunc_new2(iostream_close_, "iostream.close(self)")},

        {NULL, NULL}   
    });
}

