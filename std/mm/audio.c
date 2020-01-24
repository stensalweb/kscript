/* audio.c - implementation of the mm.Audio class */

#include "ksm_mm.h"


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

TFUNC(mm_Audio, new) {
    KS_REQ_N_ARGS(n_args, 0);
    ks_mm_Audio self = NULL;

    if (n_args == 0) {
        // construct new empty audio
       self = ks_mm_Audio_new(0, 1, 44100, NULL);
    } else if (n_args == 1) {
        // construct a new argument
        
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
    } else {
        return kse_fmt("AttrError: %R", attr);
    }
}




TFUNC(mm_Audio, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mm_Audio self = (ks_mm_Audio)args[0];
    KS_REQ_TYPE(self, ks_T_mm_Audio, "self");

    ks_free(self->buf);

    ks_free(self);
    return KSO_NONE;
}
