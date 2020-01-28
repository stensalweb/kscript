/* audio.c - implementation of the mm.Audio class

Includes reading/writing/transforming of audio data

*/

#include "ksm_mm.h"

/* include an mp3 loading library, for loading into float buffers */
//#define MINIMP3_ONLY_MP3
//#define MINIMP3_ONLY_SIMD
//#define MINIMP3_NO_SIMD
//#define MINIMP3_NONSTANDARD_BUT_LOGICAL
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "./minimp3-ex.h"


#include <sox.h>


// create a new audio object, with given parameters.
// if data==NULL, then it is initialized to 0.0f for all the data
ks_mm_Audio ks_mm_Audio_new(int samples, int channels, int hz, float* data) {
    // create a new one
    ks_mm_Audio self = ks_malloc(sizeof(*self));
    *self = (struct ks_mm_Audio) {
        KSO_BASE_INIT(ks_T_mm_Audio)
        .channels = channels,
        .samples = samples,
        .hz = hz,
        .buf = ks_malloc(sizeof(*self->buf) * samples * channels)
    };

    if (data == NULL) {
        int i;
        for (i = 0; i < samples * channels; ++i) {
            self->buf[i] = 0.0f;
        }
    } else {
        memcpy(self->buf, data, sizeof(*self->buf) * samples * channels);
    }

    // return our constructed integer
    return self;
}

// read in from a given file, overwriting the data
void ks_mm_Audio_read(ks_mm_Audio self, char* fname) {
    // get the extension
    char* ext = strrchr(fname, '.');

    // open with libsox
    sox_format_t* ft = sox_open_read(fname, NULL, NULL, NULL);

    // temporary buffer
    const int bsize = 4096;
    sox_sample_t buf[bsize];

    if (ft) {
        size_t amt, total = 0;
        while ((amt = sox_read(ft, buf, bsize)) == bsize) {
            // continue reading
            self->buf = ks_realloc(self->buf, sizeof(*self->buf) * (total + amt));
            
            int i;
            for (i = 0; i < amt; ++i) {
                // convert it
                //float csamp = SOX_SAMPLE_TO_FLOAT_32BIT(buf[i], ft->clips);
                float csamp = (float)(2 * (buf[i] - (double)SOX_SAMPLE_MIN)/((double)SOX_SAMPLE_MAX-(double)SOX_SAMPLE_MIN) - 1);
                self->buf[total + i] = csamp;
            }

            total += amt;
        }
        self->channels = ft->signal.channels;
        self->samples = total / ft->signal.channels;
        self->hz = ft->signal.rate;
    } else {
        kse_fmt("Error reading audio file '%s'", fname);
    }

    sox_close(ft);

    return;
    /*

    if (strcmp(ext, ".mp3") == 0) {

        // read in the mp3 using the library

        mp3dec_t mp3d;
        mp3dec_file_info_t info;
        if (mp3dec_load(&mp3d, fname, &info, NULL, NULL)) {
            kse_fmt("Error reading audio file '%s'", fname);
            return;
        }

        self->channels = info.channels;
        self->hz = info.hz;
        self->samples = info.samples / info.channels;
        self->buf = ks_realloc(self->buf, sizeof(*self->buf) * self->channels * self->samples);
        memcpy(self->buf, info.buffer, sizeof(*self->buf) * self->channels * self->samples);

        // we done here
        free(info.buffer);

    } else {
        kse_fmt("Unknown file format '%s'", ext);
        return;
    }
*/
}


TFUNC(mm_Audio, new) {
    //KS_REQ_N_ARGS(n_args, 0);
    ks_mm_Audio self = NULL;

    if (n_args == 0) {
        // construct new empty audio
        self = ks_mm_Audio_new(0, 1, 44100, NULL);
    } else if (n_args == 1) {
        ks_str fname = (ks_str)args[0];
        KS_REQ_TYPE(fname, ks_T_str, "fname");
        // construct a new argument
        self = ks_mm_Audio_new(0, 1, 44100, NULL);
        ks_mm_Audio_read(self, fname->chr);
    } else {
        return kse_fmt("Wrong number of args, expected 0 or 1, not %i", n_args);
    }

    return (kso)self;
}

TFUNC(mm_Audio, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");

    return (kso)ks_str_new_cfmt("<'%T' (%ismp %ihz %ichn) @ %p>", self, self->samples, self->hz, self->channels, self);
}

TFUNC(mm_Audio, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");

    return (kso)ks_str_new_cfmt("<'%T' (%ismp %ihz %ichn) obj @ %p>", self, self->samples, self->hz, self->channels, self);
}


TFUNC(mm_Audio, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_T_str, "attr");

    if (KS_STR_EQ_CONST(attr, "hz")) {
        return (kso)ks_int_new(self->hz);
    } else if (KS_STR_EQ_CONST(attr, "N")) {
        return (kso)ks_int_new(self->samples);
    } else {
        return kse_fmt("AttrError: %R", attr);
    }
}

TFUNC(mm_Audio, getitem) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");
    ks_int idx = (ks_int)args[1];
    KS_REQ_TYPE(idx, ks_T_int, "idx");

    return (kso)ks_float_new((double)self->buf[idx->v_int]);
}


TFUNC(mm_Audio, setitem) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");
    ks_int idx = (ks_int)args[1];
    KS_REQ_TYPE(idx, ks_T_int, "idx");
    ks_float val = (ks_float)args[2];
    KS_REQ_TYPE(val, ks_T_float, "val");

    self->buf[idx->v_int] = val->v_float;

    return KSO_NEWREF(val);

}

TFUNC(mm_Audio, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");

    ks_free(self->buf);

    ks_free(self);
    return KSO_NONE;
}



TFUNC(mm_Audio, write) {
    KS_REQ_N_ARGS(n_args, 2);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");
    ks_str fname = (ks_str)args[1];
    KS_REQ_TYPE(fname, ks_T_str, "fname");

    // output to file


    sox_signalinfo_t siginfo;
    siginfo.channels = self->channels;
    siginfo.length = self->samples * self->channels;
    siginfo.rate = self->hz;
    // open with libsox
    sox_format_t* ft = sox_open_write(fname->chr, &siginfo, NULL, NULL, NULL, NULL);


    if (ft) {
        size_t amt, total = self->samples * self->channels;

        size_t written = 0;

        // temporary buffer
        const int bsize = 4096;
        sox_sample_t buf[bsize];

        while (total > 0) {
            int camt = total;
            if (camt > bsize) camt = bsize;

            int i;
            for (i = 0; i < camt; ++i) {
                int32_t csamp = (int32_t)(((double)SOX_SAMPLE_MAX-(double)SOX_SAMPLE_MIN) * (self->buf[written + i] + 1) / 2.0f + SOX_SAMPLE_MIN);
                buf[i] = csamp;
            }

            sox_write(ft, buf, camt);
            written += camt;
            total -= camt;
        }

        sox_close(ft);

    } else {
        kse_fmt("Error writing audio file '%S'", fname);
    }

    return KSO_NEWREF(self);
}


