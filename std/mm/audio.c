/* audio.c - implementation of the mm.Audio class

Includes reading/writing/transforming of audio data

*/

#include "ksm_mm.h"

/* include an mp3 loading library, for loading into float buffers 
//#define MINIMP3_ONLY_MP3
//#define MINIMP3_ONLY_SIMD
//#define MINIMP3_NO_SIMD
//#define MINIMP3_NONSTANDARD_BUT_LOGICAL
#define MINIMP3_FLOAT_OUTPUT
#define MINIMP3_IMPLEMENTATION
#include "./minimp3-ex.h"


#include <sox.h>
*/

// create a new audio object, with given parameters.
// if data==NULL, then it is initialized to 0.0f for all the data
ks_mm_Audio ks_mm_Audio_new(int samples, int channels, int hz, double* data) {
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
            self->buf[i] = 0.0;
        }
    } else {
        memcpy(self->buf, data, sizeof(*self->buf) * samples * channels);
    }

    // return our constructed integer
    return self;
}

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096

#define BUFFER_SIZE 20480

#define die printf


// function that writes a number of samples to a given output, returns 0 on success, otherwise on Error
// specifically, it treats the codec's sample format as the type that `data` is, converts `num` of those into `output`,
// updating every `stride`th value (so stride=2 means that output[0], output[2], output[stride*n] is written)
// `data` is assumed to be a pointer directly to the data, so for planar data, this function should be called on each channel,
// and `stride` should be the number of channels, so that output is skipped
static inline int mm_avc_cvt(const AVCodecContext* codec_ctx, void* _data, int num, double* output, int stride) {
    // get sampling format
    const enum AVSampleFormat sfmt = codec_ctx->sample_fmt;
    // shared variables
    int i;

    /* handle PCM samples */
    if (sfmt == AV_SAMPLE_FMT_U8 || sfmt == AV_SAMPLE_FMT_U8P) {
        uint8_t* data = (uint8_t*)_data;
        for (i = 0; i < num; ++i) {
            uint8_t tmp = data[i];
            // convert to unsigned
            tmp -= 127;
            output[stride * i] = (double)tmp / 0xFF;
        }
    } else if (sfmt == AV_SAMPLE_FMT_S16 || sfmt == AV_SAMPLE_FMT_S16P) {
        int16_t* data = (int16_t*)_data;
        for (i = 0; i < num; ++i) {
            output[stride * i] = (double)data[i] / 0x7FFF;
        }
    } else if (sfmt == AV_SAMPLE_FMT_S32 || sfmt == AV_SAMPLE_FMT_S32P) {
        int32_t* data = (int32_t*)_data;
        for (i = 0; i < num; ++i) {
            output[stride * i] = (double)data[i] / 0x7FFFFFFF;
        }
    } else if (sfmt == AV_SAMPLE_FMT_S64 || sfmt == AV_SAMPLE_FMT_S64P) {
        int64_t* data = (int64_t*)_data;
        for (i = 0; i < num; ++i) {
            output[stride * i] = (double)data[i] / 0x7FFFFFFFFFFFFFFF;
        }
    } else if (sfmt == AV_SAMPLE_FMT_FLT || sfmt == AV_SAMPLE_FMT_FLT) {
        float* data = (float*)_data;
        for (i = 0; i < num; ++i) {
            output[stride * i] = (double)data[i];
        }
    } else if (sfmt == AV_SAMPLE_FMT_FLT || sfmt == AV_SAMPLE_FMT_FLT) {
        double* data = (double*)_data;
        for (i = 0; i < num; ++i) {
            output[stride * i] = (double)data[i];
        }
    } else {
        // invalid/unsupported sample format
        return 1;
    }

    return 0;

}

// read in from a given file, overwriting the data
int ks_mm_Audio_read(ks_mm_Audio self, char* fname) {
    int errstat = 0;
    #define READ_ERR(...) { kse_fmt(__VA_ARGS__); errstat = 1; goto audio_read_end; }
    #define logging ks_info

    // convert everything to doubles
    enum AVSampleFormat smp_fmt = AV_SAMPLE_FMT_DBL;

    // now, configure the codec for reading it
    AVCodec* codec = NULL;
    AVCodecContext* codec_ctx = NULL;
    AVCodecParameters* codec_par = NULL;

    // allocate a frame & packet for reading in chunks of the file
    AVFrame*  frm = av_frame_alloc();
    AVPacket* pkt = av_packet_alloc();

    // create a format context
    AVFormatContext* fmt_ctx = avformat_alloc_context();

    if (!fmt_ctx || !frm || !pkt) READ_ERR("Internal av* alloc failure");

    if (avformat_open_input(&fmt_ctx, fname, NULL, NULL) != 0) READ_ERR("Couldn't open file '%s'", fname);

    //logging("format %s", fmt_ctx->iformat->name);

    if (avformat_find_stream_info(fmt_ctx, NULL) < 0) READ_ERR("Couldn't get stream info for file '%s'", fname);

    // audio stream index
    int A_idx = -1;

    // loop though all the streams and print its main information
    int i;
    for (i = 0; i < fmt_ctx->nb_streams; i++) {
        AVCodecParameters* i_codec_p = fmt_ctx->streams[i]->codecpar;

        // finds the registered decoder for a codec ID
        // https://ffmpeg.org/doxygen/trunk/group__lavc__decoding.html#ga19a0ca553277f019dd5b0fec6e1f9dca
        AVCodec* i_codec = avcodec_find_decoder(i_codec_p->codec_id);

        if (!i_codec) READ_ERR("Unsupported codec!");

        // when the stream is a video we store its index, codec parameters and codec
        if (i_codec_p->codec_type == AVMEDIA_TYPE_AUDIO) {
            // we found the first audio stream, so set our variables
            A_idx = i;
            codec = i_codec;
            codec_par = i_codec_p;
            break;
        }

        // print its name, id and bitrate
        //logging("\tCodec %s ID %i bit_rate %lld", pLocalCodec->name, pLocalCodec->id, pCodecParameters->bit_rate);
    }

    // ensure a stream was found
    if (A_idx < 0) READ_ERR("There was no audio!");

    // and it had a valid codec
    if (!codec) READ_ERR("No codec!");

    // create a specific instance of the codec for our file
    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) READ_ERR("Failed to alloc codec_ctx");

    // set variables
    codec_ctx->sample_fmt = AV_SAMPLE_FMT_S16;
    codec_ctx->channels = codec_par->channels;
    codec_ctx->pkt_timebase = fmt_ctx->streams[A_idx]->time_base;

    if (avcodec_parameters_to_context(codec_ctx, codec_par) < 0) READ_ERR("Failed to copy codec params to context");

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) READ_ERR("Failed top open codec");


    // reset the audio buffer object to the current codec
    self->samples = 0;
    self->channels = codec_par->channels;
    self->hz = codec_par->sample_rate;

    // now, keep reading frames while in the video
    while (av_read_frame(fmt_ctx, pkt) >= 0) {
        if (pkt->stream_index == A_idx) {

            int got_frame;
            // keep going
            if (avcodec_decode_audio4(codec_ctx, frm, &got_frame, pkt) < 0) break;
            if (!got_frame) continue;

            int start_out = self->samples;
            self->samples += frm->nb_samples;
            self->buf = ks_realloc(self->buf, sizeof(*self->buf) * self->samples * self->channels);

            // handle the samples
            if (av_sample_fmt_is_planar(codec_ctx->sample_fmt)) {
                int erc = 0;

                // convert each channel into the appropriate stride self
                for (i = 0; i < self->channels; ++i) {
                    erc |= mm_avc_cvt(codec_ctx, frm->extended_data[i], frm->nb_samples, &self->buf[start_out + i], self->channels);
                }

                if (erc) READ_ERR("Invalid/unsupported sample format in file '%s'", fname)

            } else {


                // since they packed together, just output them all contiguously
                int erc = mm_avc_cvt(codec_ctx, frm->extended_data[0], frm->nb_samples * self->channels, &self->buf[start_out], 1);
                if (erc) READ_ERR("Invalid/unsupported sample format in file '%s'", fname)

            }

        }


        // deref the packet
        av_packet_unref(pkt);
    }

    audio_read_end:

    // free our temporary buffers
    if (frm) av_frame_free(&frm);
    if (pkt) av_packet_free(&pkt);

    // free our format context
    if (fmt_ctx) {
        avformat_close_input(&fmt_ctx);
        avformat_free_context(fmt_ctx);
    }

    // and remove our decoder
    if (codec_ctx) avcodec_free_context(&codec_ctx);

    // done!
    return errstat;
}

// write to a given file
void ks_mm_Audio_write(ks_mm_Audio self, char* fname) {

    #define WRITE_ERR(...) { kse_fmt(__VA_ARGS__); return; }

    const AVCodec* codec = avcodec_find_encoder(AV_CODEC_ID_WAVPACK);
    if (!codec) WRITE_ERR("Couldn't find .wav encoder");

    AVCodecContext* codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) WRITE_ERR("Couldn't alloc codec_ctx");

    codec_ctx->sample_fmt = AV_SAMPLE_FMT_DBL;
    int supp = 0;

    // search through the sample formats that are supported
    const enum AVSampleFormat* p = codec->sample_fmts;
    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == codec_ctx->sample_fmt) supp = 1;
        p++;
    }

    if (!supp) WRITE_ERR("Encoder couldn't handle doubles");




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
        if (ks_mm_Audio_read(self, fname->chr) != 0) {
            KSO_DECREF(self);
            return NULL;
        }
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
    ks_mm_Audio_write(self, fname->chr);
/*
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
    */

    return KSO_NEWREF(self);
}


