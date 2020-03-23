/* types/iostream.c - implementation of the input/output stream class
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_iostream);

// Create a blank iostream
ks_iostream ks_iostream_new() {
    ks_iostream self = KS_ALLOC_OBJ(ks_iostream);
    KS_INIT_OBJ(self, ks_type_iostream);

    // initialize type-specific things
    self->ios_flags = KS_IOS_NONE;
    self->fp = NULL;

    return self;
}


// open an 'iostream' with a given target, and mode
// returns success
bool ks_iostream_open(ks_iostream self, char* fname, char* mode) {

    if (self->ios_flags & KS_IOS_OPEN) {
        ks_throw_fmt(ks_type_ArgError, "Attempting to open iostream, but it was already open!");
        return false;
    }

    // reset
    self->ios_flags = KS_IOS_NONE;

    bool isRead = strchr(mode, 'r'), isWrite = strchr(mode, 'w');

    // current temporary position
    int tp = 0;
    // temporary buffer to generate the appropriate C-mode
    char tmp[256];


    // now, populate it
    if (isRead && isWrite) {
        tmp[tp++] = 'r';
        tmp[tp++] = '+';
        self->ios_flags |= KS_IOS_READ | KS_IOS_WRITE;
    } else if (isRead && !isWrite) {
        tmp[tp++] = 'r';
        self->ios_flags |= KS_IOS_READ;
    } else if (!isRead && isWrite) {
        tmp[tp++] = 'w';
        self->ios_flags |= KS_IOS_WRITE;
    } else {
        // neither were given, default to reading
        tmp[tp++] = 'r';
        self->ios_flags |= KS_IOS_READ;
    }

    // NUL-terminate
    tmp[tp] = '\0';

    self->fp = fopen(fname, tmp);

    if (!self->fp) {
        self->ios_flags = KS_IOS_NONE;
        ks_throw_fmt(ks_type_IOError, "Failed to open '%s': %s", fname, strerror(errno));
        return false;
    }

    self->ios_flags |= KS_IOS_OPEN;

    // success
    return true;
}

// read a string of a given size from the iostream
ks_str ks_iostream_readstr_n(ks_iostream self, ks_ssize_t sz) {
    if (!(self->ios_flags & KS_IOS_OPEN)) return ks_throw_fmt(ks_type_IOError, "Attempted to read string from iostream that was not open!");
    if (!(self->ios_flags & KS_IOS_READ)) return ks_throw_fmt(ks_type_IOError, "Attempted to read string from iostream that was not open for reading!");


    // allocate temporary buffer
    char* tmp = ks_malloc(sz + 1);

    size_t actual_bytes = fread(tmp, 1, sz, self->fp);
    if (actual_bytes != sz) {
        // discrepancy
    }

    tmp[actual_bytes] = '\0';

    ks_str res = ks_str_new(tmp);
    ks_free(tmp);

    return res;

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
        ks_throw_fmt(ks_type_IOError, "Attempted to get 'size()' of an iostream that was not open!");
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


    ks_ssize_t curpos = ftell(self->fp);
    fseek(self->fp, 0, SEEK_END);
    ks_ssize_t endpos = ftell(self->fp);
    fseek(self->fp, curpos, SEEK_SET);

    return endpos;
}




/* member functions */

// iostream.__new__(fname=None, mode=None) -> open a new IOstream
static KS_TFUNC(iostream, new) {
    KS_REQ_N_ARGS_RANGE(n_args, 0, 1);
    
    // new iostream
    ks_iostream self = ks_iostream_new();

    if (n_args == 0) {
        // return blank iostream

    } else if (n_args == 1) {
        ks_str fname = (ks_str)args[0];
        KS_REQ_TYPE(fname, ks_type_str, "fname");
        if (!ks_iostream_open(self, fname->chr, "r")) {
            KS_DECREF(self);
            return NULL;
        }
    }

    if (!self) return ks_throw_fmt(ks_type_IOError, "ERR CREATING IOS");

    return self;
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

// iostream.read(self, nbytes=none) -> read 'n' bytes (or, by default, the entire file)
static KS_TFUNC(iostream, read) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");


    ks_ssize_t nbytes = -1;

    if (n_args > 1) {
        // read in the number of bytes
        if (args[1] != KSO_NONE) {
            // allow integer
            ks_int nb = (ks_int)args[1];
            KS_REQ_TYPE(nb, ks_type_int, "nbytes");
            nbytes = nb->val;
        }
    }


    // calculate default of entire file
    if (nbytes <= 0) {
        nbytes = ks_iostream_size(self);
        if (nbytes < 0) return NULL;
    }

    return (ks_obj)ks_iostream_readstr_n(self, nbytes);
};


// iostream.size(self) -> return the size of a file, in bytes
static KS_TFUNC(iostream, size) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    ks_size_t ret = ks_iostream_size(self);
    if (ret < 0) return NULL;

    return (ks_obj)ks_int_new(ret);
}


// iostream.seek(self, pos=0) -> go to a position in the file
static KS_TFUNC(iostream, seek) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_iostream self = (ks_iostream)args[0];
    KS_REQ_TYPE(self, ks_type_iostream, "self");

    ks_ssize_t posn = 0;
    if (n_args > 0) {
        ks_int pos = (ks_int)args[1];
        KS_REQ_TYPE(pos, ks_type_int, "pos");
        posn = pos->val;
    }

    ks_size_t ret = ks_iostream_seek(self, posn);
    if (ret < 0) return NULL;

    return KS_NEWREF(self);
}


// initialize cfunc type
void ks_type_iostream_init() {
    KS_INIT_TYPE_OBJ(ks_type_iostream, "iostream");

    ks_type_set_cn(ks_type_iostream, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(iostream_new_, "iostream.__new__(fname=none, mode=none)")},
        {"__free__", (ks_obj)ks_cfunc_new2(iostream_free_, "iostream.__free__(self)")},

        {"read", (ks_obj)ks_cfunc_new2(iostream_read_, "iostream.read(self, nbytes=none)")},
        {"size", (ks_obj)ks_cfunc_new2(iostream_size_, "iostream.size(self)")},
        {"seek", (ks_obj)ks_cfunc_new2(iostream_seek_, "iostream.seek(self, pos=0)")},

        {NULL, NULL}   
    });

}

