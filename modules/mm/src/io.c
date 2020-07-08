/* src/io.c - Input/Output related functions
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../mm-impl.h"

#include <errno.h>


static bool my_setfrom_pix_fmt(int pix_fmt, void* pix, int w, int h, int linesz, float** output, int* dep) {

    // loop vars
    int i, j, c;

    switch (pix_fmt)
    {
    #ifdef KS_HAVE_LIBAV

    case AV_PIX_FMT_RGB24:
        *dep = 3;
        *output = ks_malloc(sizeof(*output) * w * h * *dep);
        
        for (i = 0; i < h; ++i) for (j = 0; j < w; ++j) for (c = 0; c < *dep; ++c) {
            (*output)[*dep * (i * w + j) + c] = ((uint8_t*)pix)[i * linesz + *dep * j + c] / 255.0;
        }


        break;

    case AV_PIX_FMT_RGBA:
        *dep = 4;
        *output = ks_malloc(sizeof(*output) * w * h * *dep);

        for (i = 0; i < h; ++i) for (j = 0; j < w; ++j) for (c = 0; c < *dep; ++c) {
            (*output)[*dep * (i * w + j) + c] = ((uint8_t*)pix)[i * linesz + *dep * j + c] / 255.0;
        }

        break;

    #endif

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


    
// internal `get_format` callback that requests the best format type
// `fmt` is terminated by negative one
static enum AVPixelFormat my_libav_get_format(struct AVCodecContext* s, const enum AVPixelFormat* fmt) {

    static const enum AVPixelFormat best_formats[] = {
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_RGB0,
        AV_PIX_FMT_RGB24,
        -1,
    };

    enum AVPixelFormat* target = &best_formats[0];

    // loop through formats we want
    while (*target > 0) {
        enum AVPixelFormat* cur = &fmt[0];

        // loop through available formats
        while (*cur > 0) {
            //printf("pixfmt: %i\n", *cur);
            if (*cur == *target) return *cur;

            cur++;
        }

        target++;
    }


    // return default
    return avcodec_default_get_format(s, fmt);
}


// filter bad pixel formats
// see: https://stackoverflow.com/questions/23067722/swscaler-warning-deprecated-pixel-format-used
static enum AVPixelFormat my_filter_pix_fmt(enum AVPixelFormat pix_fmt) {
    switch (pix_fmt) {
    case AV_PIX_FMT_YUVJ420P:
        pix_fmt = AV_PIX_FMT_YUV420P;
        break;
    case AV_PIX_FMT_YUVJ422P:
        pix_fmt = AV_PIX_FMT_YUV422P;
        break;
    case AV_PIX_FMT_YUVJ444P:
        pix_fmt = AV_PIX_FMT_YUV444P;
        break;
    case AV_PIX_FMT_YUVJ440P:
        pix_fmt = AV_PIX_FMT_YUV440P;
        break;
    }

    return pix_fmt;
}

// read an image from a file
nx_array mm_read_image(char* fname) {
    // https://ffmpeg.org/doxygen/3.4/lavfutils_8c_source.html

    // libav* structures
    #ifdef KS_HAVE_LIBAV

    AVInputFormat* iformat = NULL;
    AVFormatContext* format_ctx = NULL;

    AVCodec* codec = NULL;
    AVCodecContext* codec_ctx = NULL;
    AVCodecParameters* codec_par = NULL;

    // options
    AVDictionary* opt = NULL;

    // temp buffers
    AVFrame* frame = NULL;
    AVPacket* packet = NULL;

    // temporary data
    void* tmp_data = NULL;

    int avst, got_frame = 0;

    // return result
    nx_array res = NULL;


    // find 2D image format
    iformat = av_find_input_format("image2");

    // open the input format
    if ((avst = avformat_open_input(&format_ctx, fname, iformat, NULL)) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to open image file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // attempt to find stream information about the file
    if ((avst = avformat_find_stream_info(format_ctx, NULL)) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to find stream info for file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    codec_par = format_ctx->streams[0]->codecpar;
    codec_par->color_range = AVCOL_RANGE_UNSPECIFIED;

    // find codec parameters & decoder
    if (!(codec = avcodec_find_decoder(codec_par->codec_id))) {
        ks_throw_fmt(ks_type_IOError, "Failed to find decoder for file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }
 
    // allocate structures
	if (!(codec_ctx = avcodec_alloc_context3(NULL))) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_alloc_context()` failed");
        goto end_read_image;
    }

    if (!(frame = av_frame_alloc())) {
        ks_throw_fmt(ks_type_InternalError, "`av_frame_alloc()` failed");
        goto end_read_image;
    }

    if (!(packet = av_packet_alloc())) {
        ks_throw_fmt(ks_type_InternalError, "Failed to `av_packet_alloc()`");
        goto end_read_image;
    }

    // now, transfer to context
    if ((avst = avcodec_parameters_to_context(codec_ctx, codec_par)) < 0) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_parameters_to_context()` failed");
        goto end_read_image;
    }

    // set some options
    av_dict_set(&opt, "thread_type", "slice", 0);


    // set the format negotiator to mine
    //codec_ctx->get_format = my_libav_get_format;

    // open it
    if ((avst = avcodec_open2(codec_ctx, codec, &opt)) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to find codec for file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }
 
    // read single frame
    if ((avst = av_read_frame(format_ctx, packet)) < 0) {
        ks_throw_fmt(ks_type_IOError, "Failed to find read frame from file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    if ((avst = avcodec_decode_video2(codec_ctx, frame, &got_frame, packet)) < 0 || !got_frame) {
        ks_throw_fmt(ks_type_IOError, "Failed to find decode frame from file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }


    // now, actually read image data
    int w = frame->width, h = frame->height, chn = -1;
    enum AVPixelFormat pix_fmt = frame->format;

    // return data type
    nx_dtype rdt = nx_dtype_fp32;

    // input data type
    nx_dtype indt = NULL;

    if (pix_fmt == AV_PIX_FMT_RGBA || pix_fmt == AV_PIX_FMT_RGB24) {
        // already in a good format
        indt = nx_dtype_uint8;
        if (pix_fmt == AV_PIX_FMT_RGBA) {
            chn = 4;
        } else if (pix_fmt == AV_PIX_FMT_RGB24) {
            chn = 3;
        } else {
            chn = -1;
            ks_throw_fmt(ks_type_InternalError, "In read_image, pix_fmt==%i not handled", pix_fmt);
            goto end_read_image;
        }

        // construct NumeriX array
        res = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = rdt,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, chn },
            .stride = NULL
        });


        // cast type over
        if (!nx_T_cast((nxar_t){
            .data = (void*)frame->data[0],
            .dtype = indt,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, chn },
            .stride = (nx_size_t[]){ frame->linesize[0], indt->size * chn, indt->size }
        }, NXAR_ARRAY(res))) {
            KS_DECREF(res);
            res = NULL;
            goto end_read_image;
        }

    } else {

        pix_fmt = my_filter_pix_fmt(pix_fmt);

        // static context used for converting types
        static struct SwsContext *sws_context = NULL;

        // cache the context
        sws_context = sws_getCachedContext(sws_context,
                w, h, pix_fmt,
                w, h, AV_PIX_FMT_RGB24,
                0, NULL, NULL, NULL
        );

        chn = 3;
        indt = nx_dtype_uint8;

        // compute line size
        nx_size_t linesize = chn * w;
        linesize += (16 - linesize) % 16;

        tmp_data = ks_malloc(h * linesize);

        // now, scale the data
        sws_scale(sws_context, frame->data, frame->linesize, 0, h, &tmp_data, (int[]){ linesize });


        // construct NumeriX array
        res = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = rdt,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, chn },
            .stride = NULL
        });

        // cast type over
        if (!nx_T_cast((nxar_t){
            .data = (void*)tmp_data,
            .dtype = indt,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, chn },
            .stride = (nx_size_t[]){ linesize, indt->size * chn, indt->size }
        }, NXAR_ARRAY(res))) {
            KS_DECREF(res);
            res = NULL;
            goto end_read_image;
        }
    }

    // normalize from 0-255 to 0-1.0
    if (rdt->kind == NX_DTYPE_KIND_CFLOAT && indt->kind == NX_DTYPE_KIND_CINT) {

        // normalize by 2**bits-1
        int64_t norm_fac = (1ULL << (8 * indt->size)) - 1;

        double times_fac = 1.0 / norm_fac;

        // cast type over
        if (!nx_T_mul(NXAR_ARRAY(res),
        (nxar_t){
            .data = (void*)&times_fac,
            .dtype = nx_dtype_fp64,
            .rank = 1,
            .dim = (nx_size_t[]){ 1 },
            .stride = (nx_size_t[]){ 0 },
        }, NXAR_ARRAY(res))) {
            KS_DECREF(res);
            res = NULL;
            goto end_read_image;
        }
    }

 end_read_image:
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    av_dict_free(&opt);
    ks_free(tmp_data);

    return res;

    #else

    return ks_throw_fmt(ks_type_InternalError, "`mm` was compiled without libav, so you can't read images");

    #endif
}


#ifdef KS_HAVE_LIBAV

// internal encoding routine
static bool my_encode(AVCodecContext* codec_ctx, AVFrame* frame, AVPacket* packet, FILE* fp) {

    int avst, ret;

    /* send the frame to the encoder */
    if ((avst = avcodec_send_frame(codec_ctx, frame)) < 0) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_send_frame()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        return false;
    }

    while (ret >= 0) {
        ret = avcodec_receive_packet(codec_ctx, packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return true;
        else if (ret < 0) {
            ks_throw_fmt(ks_type_InternalError, "`avcodec_receive_packet()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
            exit(1);
        }

        fwrite(packet->data, 1, packet->size, fp);
        av_packet_unref(packet);
    }

    return true;
}
#endif



// read an image from a file
bool mm_write_image(char* fname, nxar_t img) {

    #ifdef KS_HAVE_LIBAV

    // useful: https://lists.libav.org/pipermail/libav-user/2009-April/002763.html
    // https://libav.org/documentation/doxygen/master/encode__video_8c_source.html
    // check everything
    if (img.rank != 3) {
        ks_throw_fmt(ks_type_ToDoError, "Only rank-3 tensors can be converted to images right now (h, w, d)");
        return false;
    }
    
    // height, width, & depth
    int h = img.dim[0], w = img.dim[1], d = img.dim[2];
    // magic word
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };

    // libav* structures
    AVCodecContext* codec_ctx = NULL;
    AVCodec* codec = NULL;
    AVFrame* frame = NULL;
    AVPacket* packet = NULL;

    // what to encode it as?
    enum AVCodecID codec_id = AV_CODEC_ID_PNG;

    FILE* fp = NULL;

    // libav status
    int avst;

    // quality of encoder
    int quality = 90;

    // TODO: support other ones to output, perhaps better performance
    int imgfmt = AV_PIX_FMT_RGBA;

    // return status
    bool rst = false;

	if (!(codec = avcodec_find_encoder(codec_id))) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_find_encoder()` failed");
        goto end_write_image;
    }

	if (!(codec_ctx = avcodec_alloc_context3(NULL))) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_alloc_context()` failed");
        goto end_write_image;
    }

    if (!(frame = av_frame_alloc())) {
        ks_throw_fmt(ks_type_InternalError, "Failed to `av_frame_alloc()`");
        goto end_write_image;
    }

    if (!(packet = av_packet_alloc())) {
        ks_throw_fmt(ks_type_InternalError, "Failed to `av_packet_alloc()`");
        goto end_write_image;
    }

    // set codec parameters

	codec_ctx->codec_id = AV_CODEC_ID_PNG;
	codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	
	codec_ctx->time_base = (AVRational){ 1, 24 };
	codec_ctx->framerate = (AVRational){ 24, 1 };

    codec_ctx->bit_rate = 400000;
	
    codec_ctx->width = w;
	codec_ctx->height = h;
	codec_ctx->pix_fmt = AV_PIX_FMT_RGBA;

    // initialize VBR settings
	codec_ctx->qmin = codec_ctx->qmax = quality;
    codec_ctx->mb_lmin = codec_ctx->qmin * FF_QP2LAMBDA;
	codec_ctx->mb_lmax = codec_ctx->qmax * FF_QP2LAMBDA;
	codec_ctx->flags = AV_CODEC_FLAG_QSCALE;
	codec_ctx->global_quality = codec_ctx->qmin * FF_QP2LAMBDA;


    // open encoder
	if (avst = avcodec_open2(codec_ctx, codec, NULL)) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_open()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        goto end_write_image;
    }

	//
	//	Set the timestamp and quality parameters
	//
	frame->pts = 1;
	frame->quality = codec_ctx->global_quality;


    fp = fopen(fname, "wb");
    if (!fp) {
        ks_throw_fmt(ks_type_IOError, "Could not open file '%s': ", fname, strerror(errno));
        goto end_write_image;
    }

    // copy to frame
    frame->format = codec_ctx->pix_fmt;
    frame->width = w;
    frame->height = h;

    if ((avst = av_frame_get_buffer(frame, 32)) < 0) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_frame_get_buffer()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        goto end_write_image;
    }

    if ((avst = av_frame_make_writable(frame)) < 0) {
        ks_throw_fmt(ks_type_InternalError, "`aav_frame_make_writable()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        goto end_write_image;
    }

    int x, y;

    int i = 0;

    struct my_rgba {
        uint8_t r, g, b, a;
    };
    
    // data pointer
    intptr_t dptr_img = (intptr_t)frame->data[0];

    // get stride
    int rowstride = frame->linesize[0];

    // nxar, describing image
    nxar_t img_ar = (nxar_t){
        .data = (void*)frame->data[0],
        .dtype = nx_dtype_uint8,
        .rank = 3,
        .dim = (nx_size_t[]){ h, w, 4 },
        .stride = (nx_size_t[]){ rowstride, 4, 1 },
    };

    // value
    uint8_t vu8;
    nx_size_t stride00[] = {0, 0};

    // now, convert every channel
    int chn;
    for (chn = 0; chn < 4; ++chn) {

        nxar_t chn_vals;
        chn_vals.rank = 2;
        chn_vals.dim = (nx_size_t[]){ h, w };
        // now, need to fill in data, dtype, stride

        if (chn < d) {
            // get color channel

            chn_vals.data = (void*)((intptr_t)img.data + chn * img.stride[2]);
            chn_vals.dtype = img.dtype;
            chn_vals.stride = &img.stride[0];

        } else {
            // default to 255, if it is the last one
            vu8 = (chn == (4 - 1)) ? 255 : 0;
            chn_vals.data = &vu8;
            chn_vals.dtype = nx_dtype_uint8;
            chn_vals.stride = stride00;
        }



        // cast values
        if (!nx_T_cast(chn_vals, (nxar_t){
            .data = (void*)((intptr_t)img_ar.data + img_ar.stride[2] * chn),
            .dtype = img_ar.dtype,
            .rank = 2,
            .dim = (nx_size_t[]){ img_ar.dim[0], img_ar.dim[1] },
            .stride = (nx_size_t[]){ img_ar.stride[0], img_ar.stride[1] },
        })) {
            goto end_write_image;
        }
    }

    frame->pts = i;
    /* encode the image */
    if (!my_encode(codec_ctx, frame, packet, fp)) goto end_write_image;
    if (!my_encode(codec_ctx, NULL, packet, fp)) goto end_write_image;

    fwrite(endcode, 1, sizeof(endcode), fp);

    rst = true;
end_write_image:
    // cleanup
    if (codec_ctx) avcodec_close(codec_ctx);
    if (frame) av_frame_free(&frame);
    if (packet) av_packet_free(&packet);
    if (fp) fclose(fp);

    return rst;

    #else

    return ks_throw_fmt(ks_type_InternalError, "`mm` was compiled without libav, so you can't write images");

    #endif

}




