/* mm/src/module.c - the kscript's multi-media library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "mm"

// include this since this is a module.
#include "ks-module.h"

// this library's header
#include "../mm-impl.h"



ks_type mm_Enum_MediaType = NULL;


// mm.read_file(fname) -> read an entire file (by default, as a blob)
static KS_TFUNC(mm, read_file) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str fname;
    if (!ks_parse_params(n_args, args, "fname%s", &fname)) return NULL;

    return (ks_obj)mm_read_file(fname->chr);
}

static bool my_setfrom_pi(enum AVSampleFormat smp_fmt, void** samples, int len, int chn, double* output) {

    // loop vars
    int i, c;

    switch (smp_fmt)
    {
    case AV_SAMPLE_FMT_FLTP:
        for (c = 0; c < chn; ++c) for (i = 0; i < len; ++i) output[i * chn + c] = ((float**)samples)[c][i];
        break;
    


    default:
        ks_throw_fmt(ks_type_ToDoError, "Haven't handled AvSampleFormat==%i", (int)smp_fmt);
        return false;
        break;
    }

    return true;
}

 
bool decode_audio_file(const char* fname, double** data, int* size, int* channels, int* hz) {

    // all objects that need to be freed
    AVFormatContext* format_ctx = avformat_alloc_context();

    // packet holds data
    AVPacket packet;
    av_init_packet(&packet);

    // current frame
    AVFrame* frame = NULL;

    // return status
    bool rstat = false;


    // attempt to open the file
    if (avformat_open_input(&format_ctx, fname, NULL, NULL) != 0) {
        ks_throw_fmt(ks_type_IOError, "Could not open file '%s'", fname);
        goto cleanup;
    }
        
    // this causes annoying error messages, and is only used for 'swr' (i.e. resampling)
    /*
    if (avformat_find_stream_info(format, NULL) < 0) {
        fprintf(stderr, "Could not retrieve stream info from file '%s'\n", fname);
        return -1;
    }
    */
    
    // find the best audio stream
    //AVCodec *decoder = NULL;
    //int A_st_i = av_find_best_stream(format_ctx, AVMEDIA_TYPE_AUDIO, -1, -1, &decoder, 0);

    int A_st_i = -1;

    int i;
    for (i = 0; i < format_ctx->nb_streams; ++i) {
        if (format_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_AUDIO) {
            A_st_i = i;
            break;
        }
    }

    if (A_st_i < 0) {
        ks_throw_fmt(ks_type_IOError, "Could not find audio stream in file '%s'", fname);
        goto cleanup;
    }

    // get the audio stream
    AVStream* A_stream = format_ctx->streams[A_st_i];
 
    // find & open codec
    AVCodecContext* A_codec = A_stream->codec;
    if (avcodec_open2(A_codec, avcodec_find_decoder(A_codec->codec_id), NULL) < 0) {
        ks_throw_fmt(ks_type_IOError, "Could not open decoder for audio stream (#%i) in file '%s'", A_st_i, fname);
        goto cleanup;
    }

    // prepare to read data
    frame = av_frame_alloc();
	if (!frame) {
        ks_throw_fmt(ks_type_InternalError, "Failed to call `av_frame_alloc()`");
        goto cleanup;
	}
 
    // set data
    *data = NULL;
    *size = 0;
    *channels = -1;
    *hz = -1;

    // iterate through frames
    int got_frame = 0;

    while (av_read_frame(format_ctx, &packet) >= 0) {

        // decode audio from a frame
        if (avcodec_decode_audio4(A_codec, frame, &got_frame, &packet) < 0) {
            // done reading
            break;
        } else if (!got_frame) {
            // frame not completed
            continue;
        } else {
            // handle data
            
            if (*channels < 0) *channels = A_codec->channels;
            if (*hz < 0) *hz = A_codec->sample_rate;

            // reallocate for more
            *data = realloc(*data, (*size + frame->nb_samples) * (*channels) * sizeof(double));

            // attempt to have internally setting it
            if (!my_setfrom_pi(A_codec->sample_fmt, (void**)frame->data, frame->nb_samples, A_codec->channels, *data + *size)) goto cleanup;

            // extra sample count
            *size += frame->nb_samples;
        }
    }

    // success
    rstat = true;

    cleanup:

    // free temporary resources
    av_free_packet(&packet);
    if (frame) av_frame_free(&frame);
    if (format_ctx) avformat_free_context(format_ctx);

    return rstat;
 
}

// mm.read_audio(fname) -> read an entire file, decoding any audio contained in it
static KS_TFUNC(mm, read_audio) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str fname;
    if (!ks_parse_params(n_args, args, "fname%s", &fname)) return NULL;

    // decode data
    int size = 0, channels = 0, hz = 0;
    double* data = NULL;
    if (!decode_audio_file(fname->chr, &data, &size, &channels, &hz)) {
        return NULL;
    }
 
    nx_array res = nx_array_new(NX_DTYPE_FP64, 2, (nx_size_t[]){ channels, size }, NULL);
    if (!res) {
        free(data);
        return NULL;
    }

    // convert from (len, channels) to (channels, len) (i.e. transpose)
    double* resdata = res->data;

    int i, j;
    for (i = 0; i < size; ++i) {
        for (j = 0; j < channels; ++j) {
            resdata[j * size + i] = data[i * channels + j];
        }
    }

    free(data);
    ks_tuple res_tuple = ks_tuple_new_n(2, (ks_obj[]){ (ks_obj)res, (ks_obj)ks_int_new(hz) });

    return (ks_obj)res_tuple;
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


    /* import libav */

    ks_module mod = ks_module_new(MODULE_NAME);

    mm_init_type_Stream();

    mm_Enum_MediaType = ks_Enum_create_c("mm.MediaType", (ks_enum_entry_c[]){

        {"NONE",      MM_MEDIA_TYPE_NONE},
        {"Audio",     MM_MEDIA_TYPE_AUDIO},
        {"Video",     MM_MEDIA_TYPE_VIDEO},

        {NULL, -1}
    });


    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]) {
        {"MediaType",        (ks_obj)mm_Enum_MediaType},

        {"read_file",        (ks_obj)ks_cfunc_new2(mm_read_file_,  "mm.read_file(fname)")},
        {"read_audio",       (ks_obj)ks_cfunc_new2(mm_read_audio_, "mm.read_audio(fname)")},

        {"Stream",           (ks_obj)mm_type_Stream},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
