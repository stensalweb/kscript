/* src/io.c - Input/Output related functions
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../mm-impl.h"





static bool my_setfrom_pix_fmt(enum AVPixelFormat pix_fmt, void* pix, int w, int h, int linesz, double** output, int* dep) {

    // loop vars
    int i, j, c;

    switch (pix_fmt)
    {

    case AV_PIX_FMT_RGB24:
        *dep = 3;
        *output = ks_malloc(sizeof(*output) * w * h * *dep);
        
        for (i = 0; i < h; ++i) for (j = 0; j < w; ++j) for (c = 0; c < *dep; ++c) {
            (*output)[*dep * (i * w + j) + c] = ((uint8_t*)pix)[i * linesz + *dep * j + c];
        }


        break;

    case AV_PIX_FMT_RGBA:
        *dep = 4;
        *output = ks_malloc(sizeof(*output) * w * h * *dep);

        for (i = 0; i < h; ++i) for (j = 0; j < w; ++j) for (c = 0; c < *dep; ++c) {
            (*output)[*dep * (i * w + j) + c] = ((uint8_t*)pix)[i * linesz + *dep * j + c];
        }

        break;

    default:
        ks_throw_fmt(ks_type_ToDoError, "Haven't handled AVPixelFormat==%i", (int)pix_fmt);
        return false;
        break;
    }

    return true;
}







// read whole file as a blob
ks_blob mm_read_file(char* fname) {
    ks_iostream ios = ks_iostream_new();

    // attempt to open the file
    if (!ks_iostream_open(ios, fname, "rb")) {
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

    return res;
}


// read an image from a file
nx_array mm_read_image(char* fname) {

    // libav* structures
    AVInputFormat* inp_format = NULL;
    AVFormatContext* format_ctx = NULL;
    AVCodec* codec=NULL;
    AVCodecContext* codec_ctx=NULL;
    AVFrame* frame = NULL;
    AVPacket packet;

    // whether we got a frame or not
    int got_frame = 0;

    // allocated data
    double* data = NULL;
    
    // result array
    nx_array res = NULL;


    av_init_packet(&packet);

    inp_format = av_find_input_format("image2");
    if (avformat_open_input(&format_ctx, fname, inp_format, NULL) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to open image file '%s'", fname);
        goto end_read_image;
    }

    codec_ctx = format_ctx->streams[0]->codec;
    codec = avcodec_find_decoder(codec_ctx->codec_id);
    if (!codec) {
        ks_throw_fmt(ks_type_IOError, "Failed to find codec for image file '%s'", fname);
        goto end_read_image;
    }

    if (avcodec_open2(codec_ctx, codec, NULL) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to open codec for image file '%s'", fname);
        goto end_read_image;
    }

    if (!(frame = av_frame_alloc())) {
        ks_throw_fmt(ks_type_InternalError, "Failed to `av_frame_alloc()`");
        goto end_read_image;
    }

    if (av_read_frame(format_ctx, &packet) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to read frame from image file '%s'", fname);
        goto end_read_image;
    }

    if (avcodec_decode_video2(codec_ctx, frame, &got_frame, &packet) < 0 || !got_frame) {
        ks_throw_fmt(ks_type_IOError, "Failed to decode image file '%s'", fname);
        goto end_read_image;
    }


    // now, actually read image data
    int w = frame->width, h = frame->height;
    int pix_fmt = frame->format;
    int dep = 0;


    // actually collect data from the array
    if (!my_setfrom_pix_fmt(codec_ctx->pix_fmt, frame->data[0], w, h, frame->linesize[0], &data, &dep)) goto end_read_image;


    nxar_t nxar = (nxar_t){
        .data = data,
        .dtype = NX_DTYPE_FP64,
        .N = 3,
        .dim = (nx_size_t[]){ h, w, dep },
        .stride = (nx_size_t[]){ dep * w, dep, 1 }
    };

    printf("TEST\n");
    // construct NumeriX array
    res = nx_array_new(nxar);



end_read_image:
    // cleanup
    av_free_packet(&packet);
    if (codec_ctx) avcodec_close(codec_ctx);
    if (format_ctx) avformat_close_input(&format_ctx);
    if (frame) av_freep(&frame);
    if (data) ks_free(data);

    return res;
}



