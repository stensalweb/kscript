/* audio.c - implementation of the mm.Audio class */

#include "ksm_mm.h"


ks_mm_audio ks_mm_audio_new(int samples, int channels, int hz, float* data) {
    // create a new one
    ks_mm_audio self = ks_malloc(sizeof(*self));
    *self = (struct ks_mm_audio) {
        KSO_BASE_INIT(ks_T_mm_Audio)
        .channels = 2,
        .samples = 0,
        .hz = 44100,
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


TFUNC(mm_Audio, free) {
    ks_mm_audio self = (ks_mm_audio)args[0];

    ks_free(self->buf);

    ks_free(self);

    return KSO_NONE;
}


