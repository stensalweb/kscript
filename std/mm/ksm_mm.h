/* ksm_mm.h - header for the multi-media library included with kscript */


#pragma once
#ifndef KSM_MM_H__
#define KSM_MM_H__

// include the main kscript header file
#include <ks_common.h>


/* declare our types in this module */

extern ks_type
    ks_T_mm_Audio  // mm.Audio
;


// mm.Audio class, i.e. an audio buffer
typedef struct ks_mm_Audio {
    KSO_BASE

    // number of channels (1=mono, 2=stereo)
    int channels;

    // number of samples taken
    int samples;

    // sampling rate, in samples/sec. Default is 44100
    int hz;

    // an interleaved buffer. For mono, we have:
    // SSSSSSSS (in order)
    // for stereo:
    // LRLRLRLRLR (interleaved)
    // thus, the size is channels*n_samples*sizeof(*buf)
    float* buf;

}* ks_mm_Audio;

// audio methods

// create a new audio object, with given parameters.
// if data==NULL, then it is initialized to 0
ks_mm_Audio ks_mm_Audio_new(int samples, int channels, int hz, float* data);


TFUNC(mm_Audio, repr);
TFUNC(mm_Audio, call);
TFUNC(mm_Audio, free);

#endif

