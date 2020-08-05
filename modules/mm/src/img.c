/* src/img.c - image-related OI functionality
 *
 *
 * Useful links: 
 *  - https://ffmpeg.org/doxygen/3.4/lavfutils_8c_source.html
 *  - pix_fmt conversion: https://ffmpeg.org/doxygen/0.6/imgconvert_8c.html
 * 
 * NOTE:
 * 
 *   * Sometimes I allow channel strides to be negative to indicate a it has not been computed,
 *       but for grayscale images, the stride could be 0 to repeat, or -1 (to convert from RGB <-> BGR for example)
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../mm-impl.h"


/* INTERNAL HELPER ROUTINES */

// implementation of comparison ignoring case
static int my_strcasecmp(const char *as, const char *bs) {
    char a = *as, b = *bs;
    while (a && b) {
        if (a > b || a < b) return b - a;

        // advance once
        a = *++as, b = *++bs;
    }

    return b - a;
}

// Custom callback for `codec_context->get_format` to pick one of our preferred formats if the codec supports it
static enum AVPixelFormat my_libav_get_format(struct AVCodecContext* s, const enum AVPixelFormat* fmt) {

    // here are a list of formats we prefer (-1 terminated)
    static const enum AVPixelFormat best_formats[] = {
        AV_PIX_FMT_RGBA,
        AV_PIX_FMT_RGB24,
        AV_PIX_FMT_RGB0,
        -1,
    };
    
    // current one that is supported
    // NOTE: This one is on the outside, since the codecs normally list them in their preferred order;
    //   if they prefer it and we support it, we should choose the one they support the most first
    const enum AVPixelFormat* cur = &fmt[0];

    while (*cur > 0) {
        // the one we are looking for & can handle
        const enum AVPixelFormat* target = &best_formats[0];

        // now, see if any of our formats is the one we are looping over
        while (*target > 0) {

            // if they match, we try to request this format
            if (*cur == *target) return *cur;

            target++;
        }

        cur++;
    }

    // return default if none of our favorites were present
    return avcodec_default_get_format(s, fmt);
}

// Filtering bad pixel formats; this returns a better version of pix_fmt (that is still the same memory wise),
//   but does not cause
// See: https://stackoverflow.com/questions/23067722/swscaler-warning-deprecated-pixel-format-used
static enum AVPixelFormat my_filter_pix_fmt(enum AVPixelFormat pix_fmt) {

    // pixel-format-case, for turning 'old' into 'new'
    #define _PFC(_old, _new) else if (pix_fmt == _old) return _new;

    if (false) {}

    // YUV-JPEG formats are the same as normal YUV, but for some reason codecs don't like them at all
    _PFC(AV_PIX_FMT_YUVJ411P, AV_PIX_FMT_YUV411P)
    _PFC(AV_PIX_FMT_YUVJ420P, AV_PIX_FMT_YUV420P)
    _PFC(AV_PIX_FMT_YUVJ422P, AV_PIX_FMT_YUV422P)
    _PFC(AV_PIX_FMT_YUVJ444P, AV_PIX_FMT_YUV444P)
    _PFC(AV_PIX_FMT_YUVJ440P, AV_PIX_FMT_YUV440P)

    #undef _PFC

    // otherwise, return what was given, as it's probably just fine
    return pix_fmt;
}


// internal encoding routine for a frame of data
static bool my_encode(AVCodecContext* codec_ctx, AVFrame* frame, AVPacket* packet, FILE* fp) {

    int avst;

    // send frame to the encoder
    if ((avst = avcodec_send_frame(codec_ctx, frame)) < 0) {
        ks_throw(ks_type_InternalError, "`avcodec_send_frame()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        return false;
    }

    while (avst >= 0) {
        avst = avcodec_receive_packet(codec_ctx, packet);
        if (avst == AVERROR(EAGAIN) || avst == AVERROR_EOF)
            return true;
        else if (avst < 0) {
            ks_throw(ks_type_InternalError, "`avcodec_receive_packet()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
            return false;
        }

        fwrite(packet->data, 1, packet->size, fp);
        av_packet_unref(packet);
    }

    return true;
}


/* MAIN FUNCS */

// Read image from file, returning tensor
// READ: mm.h for more information
nx_array mm_read_image(char* fname, enum mm_flags flags) {

    #ifdef KS_HAVE_LIBAV

    /* libav* structures */
    
    // format specification
    AVInputFormat* iformat = NULL;
    AVFormatContext* format_ctx = NULL;

    // decoder stuff
    AVStream* stream = NULL;
    AVCodec* codec = NULL;
    AVCodecContext* codec_ctx = NULL;
    AVCodecParameters* codec_par = NULL;

    // options
    AVDictionary* opt = NULL;

    // data buffers
    AVFrame* frame = NULL;
    AVPacket* packet = NULL;

    // temporary data
    void* tmp_data = NULL;

    // avst = libAV STatus
    // got_frame = whether a frame was grabbed by the decoder
    int avst, got_frame = 0;

    // stream index
    int st_i;

    // return result
    nx_array res = NULL;

    // find image 2D input format
    if (!(iformat = av_find_input_format("image2"))) {
        ks_throw(ks_type_InternalError, "Failed to find input format 'image2'");
        goto end_read_image;
    }

    // open the input format
    if ((avst = avformat_open_input(&format_ctx, fname, iformat, NULL)) < 0) {
        ks_throw(ks_type_IOError, "Failed to open image file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // attempt to find stream information about the file
    if ((avst = avformat_find_stream_info(format_ctx, NULL)) < 0) {
        ks_throw(ks_type_IOError, "Failed to find stream info for file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // and locate the best video stream
    if ((st_i = av_find_best_stream(format_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &codec, 0)) < 0) {
        ks_throw(ks_type_IOError, "Failed to find a video stream file '%s': %s (code: %i)", fname, av_err2str(st_i), st_i);
        goto end_read_image;
    } else if (!codec) {
        ks_throw(ks_type_IOError, "Failed to find a valid codec for stream #%i in file '%s': %s (code: %i)", fname, st_i, av_err2str(st_i), st_i);
        goto end_read_image;
    }

    // now, get the stream
    stream = format_ctx->streams[st_i];

    // and the codec parameters
    codec_par = stream->codecpar;

    // allocate structures
	if (!(codec_ctx = avcodec_alloc_context3(NULL))) {
        ks_throw(ks_type_InternalError, "`avcodec_alloc_context()` failed");
        goto end_read_image;
    }

    // now, transfer to context
    if ((avst = avcodec_parameters_to_context(codec_ctx, codec_par)) < 0) {
        ks_throw(ks_type_InternalError, "`avcodec_parameters_to_context()` failed: %s (code %i)", av_err2str(avst), avst);
        goto end_read_image;
    }

    // allocate data buffres
    if (!(frame = av_frame_alloc())) {
        ks_throw(ks_type_InternalError, "`av_frame_alloc()` failed");
        goto end_read_image;
    }

    if (!(packet = av_packet_alloc())) {
        ks_throw(ks_type_InternalError, "Failed to `av_packet_alloc()`");
        goto end_read_image;
    }

    // set some options
    av_dict_set(&opt, "thread_type", "slice", 0);

    // set the format negotiator to one of our preferred formats (if possible)
    codec_ctx->get_format = my_libav_get_format;

    // open it
    if ((avst = avcodec_open2(codec_ctx, codec, &opt)) < 0) {
        ks_throw(ks_type_IOError, "Failed to find codec for file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // read a frame (still encoded) into packet
    if ((avst = av_read_frame(format_ctx, packet)) < 0) {
        ks_throw(ks_type_IOError, "Failed to find read frame from file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // now, send the packet to the decoder
    if ((avst = avcodec_send_packet(codec_ctx, packet)) < 0) {
        ks_throw(ks_type_IOError, "Failed to send packet to decoder in file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // and receive it as a decoded frame
    if ((avst = avcodec_receive_frame(codec_ctx, frame)) < 0) {
        ks_throw(ks_type_IOError, "Failed to receive frame to decoder in file '%s': %s (code: %i)", fname, av_err2str(avst), avst);
        goto end_read_image;
    }

    // calculate shape of the tensor (chn < 0 indicates it has not been found yet)
    int h = frame->height, w = frame->width, out_chn = -1, in_chn = -1;

    // channel stride (i.e. padding per channel)
    int in_chn_stride = -1;

    // input data type
    nx_dtype indt = NULL;

    // the pixel format the frame is in
    enum AVPixelFormat pix_fmt = frame->format;

    // filter it to replace deprecated pixel formats
    pix_fmt = my_filter_pix_fmt(pix_fmt);


    // calculate return data type

    // return data type
    nx_dtype rdt = NULL;

    rdt = nx_dtype_fp32;



    // formats that are already amenable (i.e. non-planar, byte-width, RGB[A] formats)
    if (pix_fmt == AV_PIX_FMT_RGBA || pix_fmt == AV_PIX_FMT_RGB24 || pix_fmt == AV_PIX_FMT_RGB0) {

        // RGB[A] formats are unsigned 8 bit values

        // calculate number of channels
        /**/ if (pix_fmt == AV_PIX_FMT_RGBA)  { in_chn = 4; in_chn_stride = 4; }
        else if (pix_fmt == AV_PIX_FMT_RGB0)  { in_chn = 3; in_chn_stride = 4; }
        else if (pix_fmt == AV_PIX_FMT_RGB24) { in_chn = 3; in_chn_stride = 3; }


        // input data type
        indt = nx_dtype_uint8;

        // default to the same number of channels
        if (out_chn <= 0) out_chn = in_chn;

        // assure we calculated it
        assert(in_chn > 0 && in_chn_stride > 0 && out_chn >= 0 && "layout was not calculated!");


        // construct new array
        if (!(res = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = rdt,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, out_chn },
            .stride = NULL
        }))) {
            goto end_read_image;
        }

        // calculate a scaling factor        
        double scale_fac = 1.0;

        if (indt->kind == NX_DTYPE_KIND_CINT && (rdt->kind == NX_DTYPE_KIND_CFLOAT || rdt->kind == NX_DTYPE_KIND_CCOMPLEX)) {
            // converting from int -> float requires to divide by fixed point amount
            scale_fac = 1.0 / ((1ULL << (8 * indt->size)) - 1);
        }

        // now, set res = input * scale
        if (!nx_T_mul(
            (nxar_t) {
                .data = (void*)frame->data[0],
                .dtype = indt,
                .rank = 3,
                .dim = (nx_size_t[]){ h, w, in_chn },
                .stride = (nx_size_t[]){ frame->linesize[0], in_chn_stride * indt->size, indt->size },
            },
            (nxar_t) {
                .data = (void*)&scale_fac,
                .dtype = nx_dtype_fp64,
                .rank = 1,
                .dim = (nx_size_t[]){ 1 },
                .stride = (nx_size_t[]){ 0 },
            },
            NXAR_ARRAY(res)
        )) {
            KS_DECREF(res);
            res = NULL;
            goto end_read_image;
        }

    } else {
        // not a preferred pixel format, so we need to use sws
        //   to change the dimensions

        // first, try to convert to 8-bit greyscale
        enum AVPixelFormat new_fmt = AV_PIX_FMT_GRAY8;
        new_fmt = AV_PIX_FMT_RGBA;

        // convert to RGB if we would lose chroma
        /*if (av_get_pix_fmt_loss(new_fmt, pix_fmt, true) & FF_LOSS_CHROMA) {
            new_fmt = AV_PIX_FMT_RGB24;
        }

        // ensure alpha is included if the format would lose it
        if (av_get_pix_fmt_loss(new_fmt, pix_fmt, true) & FF_LOSS_ALPHA) {
            new_fmt = AV_PIX_FMT_RGBA;
        }*/

        // calculate data layout
        /**/ if (new_fmt == AV_PIX_FMT_GRAY8)   { in_chn = 1; in_chn_stride = 1; } 
        else if (new_fmt == AV_PIX_FMT_RGB24)   { in_chn = 3; in_chn_stride = 3; } 
        else if (new_fmt == AV_PIX_FMT_RGBA)    { in_chn = 4; in_chn_stride = 4; }


        // default to the same number of channels
        if (out_chn <= 0) out_chn = in_chn;

        // all the formats are uint8_t's
        indt = nx_dtype_uint8;

        // assume it was calculated
        assert(in_chn > 0 && in_chn_stride > 0 && out_chn >= 0 && "layout was not calculated!");

        // static context used for converting types (which will cache itself,
        //   making reading a lot of files with the same format efficient)
        static struct SwsContext *sws_context = NULL;


        // get the context with caching
        sws_context = sws_getCachedContext(sws_context,
            w, h, pix_fmt,
            w, h, new_fmt,
            0, NULL, NULL, NULL
        );


        // compute line size (with padding)
        nx_size_t linesize = in_chn_stride * w;
        linesize += (16 - linesize) % 16;

        // allocate an array of temporary data
        tmp_data = ks_malloc(linesize * h);


        // now, convert the data into the 'tmp_data'
        sws_scale(sws_context, (const uint8_t* const*)frame->data, frame->linesize, 0, h, (uint8_t* const[]){ tmp_data, NULL, NULL, NULL }, (int[]){ linesize, 0, 0, 0 });

        // construct new array
        res = nx_array_new((nxar_t){
            .data = NULL,
            .dtype = rdt,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, out_chn },
            .stride = NULL
        });

        if (!res) {
            goto end_read_image;
        }

        // calculate a scaling factor        
        double scale_fac = 1.0;

        if (indt->kind == NX_DTYPE_KIND_CINT && (rdt->kind == NX_DTYPE_KIND_CFLOAT || rdt->kind == NX_DTYPE_KIND_CCOMPLEX)) {

            // converting from int -> float requires to divide by fixed point amount
            scale_fac = 1.0 / ((1ULL << (8 * indt->size)) - 1);
        }


        // now, set res = input * scale
        if (!nx_T_mul(
            (nxar_t) {
                .data = (void*)tmp_data,
                .dtype = indt,
                .rank = 3,
                .dim = (nx_size_t[]){ h, w, in_chn },
                .stride = (nx_size_t[]){ linesize, in_chn_stride * indt->size, indt->size },
            },
            (nxar_t) {
                .data = (void*)&scale_fac,
                .dtype = nx_dtype_fp64,
                .rank = 1,
                .dim = (nx_size_t[]){ 1 },
                .stride = (nx_size_t[]){ 0 },
            },
            NXAR_ARRAY(res)
        )) {
            KS_DECREF(res);
            res = NULL;
            goto end_read_image;
        }
    }

 end_read_image:
    // free libav* structures
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&format_ctx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    av_dict_free(&opt);

    // free temp data
    ks_free(tmp_data);

    return res;

    #else

    return ks_throw(ks_type_InternalError, "`mm` was compiled without libav, so you can't read images");

    #endif
}



// Encode an image to a given filename
// READ: mm.h for more information
bool mm_write_image(char* fname, nxar_t img, enum mm_flags flags) {

    #ifdef KS_HAVE_LIBAV

    // useful: 
    //   https://lists.libav.org/pipermail/libav-user/2009-April/002763.html
    //   https://libav.org/documentation/doxygen/master/encode__video_8c_source.html
    // check everything
    if (img.rank != 3) {
        ks_throw(ks_type_ToDoError, "Only rank-3 tensors can be converted to images right now: (h, w, d)");
        return false;
    }

    // find file extension
    char* _ext = strrchr(fname, '.');

    if (!_ext) {
        ks_throw(ks_type_ArgError, "Can't determine file output format for file name '%s'", fname);
        return false;
    }


    // find the codecid
    enum AVCodecID codec_id = -1;

    // helper macro to test if the extension is a given name
    #define _EQE(_name) (my_strcasecmp(_ext, _name) == 0)

    // here's my main list:
    // https://blog.filestack.com/thoughts-and-knowledge/complete-image-file-extension-list/
    // I wonder what's the best way to test this...
    if (false) {}

    else if (_EQE(".jpg") || _EQE(".jpeg") || _EQE(".jpe") || _EQE(".jif") || _EQE(".jfif") || _EQE(".jfi")) codec_id = AV_CODEC_ID_MJPEG;
    else if (_EQE(".jp2") || _EQE(".j2k") || _EQE(".jpf") || _EQE(".jpx") || _EQE(".jpm") || _EQE(".mj2")) codec_id = AV_CODEC_ID_JPEG2000;
    else if (_EQE(".png")) codec_id = AV_CODEC_ID_PNG;
    else if (_EQE(".gif")) codec_id = AV_CODEC_ID_GIF;
    else if (_EQE(".tga") || _EQE(".icb") || _EQE(".vda") || _EQE(".vst")) codec_id = AV_CODEC_ID_TARGA;
    else if (_EQE(".webp")) codec_id = AV_CODEC_ID_WEBP;
    else if (_EQE(".tiff") || _EQE(".tif")) codec_id = AV_CODEC_ID_TIFF;
    else if (_EQE(".psd")) codec_id = AV_CODEC_ID_PSD;
    else if (_EQE(".raw") || _EQE(".arw") || _EQE(".cr2") || _EQE(".nrw") || _EQE(".k25")) codec_id = AV_CODEC_ID_RAWVIDEO;
    else if (_EQE(".bmp") || _EQE(".dib")) codec_id = AV_CODEC_ID_BMP;
    else if (_EQE(".ppm")) codec_id = AV_CODEC_ID_PPM;
    else if (_EQE(".pgm")) codec_id = AV_CODEC_ID_PGM;
    else if (_EQE(".pbm")) codec_id = AV_CODEC_ID_PBM;

    // see: https://trac.ffmpeg.org/ticket/6521
    //else if (_EQE(".heif") || _EQE(".heic")) codec_id = AV_CODEC_ID_ 

    // not easy to support; not straightforward    
    //else if (_EQE(".ico")) codec_id = AV_CODEC_ID_;
    
    
    //else if (_EQE(".")) codec_id = AV_CODEC_ID_;

    #undef _EQE

    if (codec_id < 0) {
        ks_throw(ks_type_ArgError, "Can't determine file output format for file name '%s' (unknown extension: '%s')", fname, _ext);
        return false;
    }

    
    /* libav structures */

    // codec & current context
    AVCodec* codec = NULL;
    AVCodecContext* codec_ctx = NULL;

    // data buffers
    AVFrame* frame = NULL;
    AVPacket* packet = NULL;

    // height, width, & depth
    int h = img.dim[0], w = img.dim[1], d = img.dim[2];

    // requested pixel format
    // TODO: allow others as well
    enum AVPixelFormat req_pix_fmt = AV_PIX_FMT_RGBA;

    // magic word for the end of file
    uint8_t endcode[] = { 0, 0, 1, 0xb7 };


    FILE* fp = NULL;

    // libav status
    int avst;

    // quality of encoder
    int quality = 90;

    // return status
    bool rst = false;

	if (!(codec = avcodec_find_encoder(codec_id))) {
        ks_throw(ks_type_InternalError, "`avcodec_find_encoder()` failed");
        goto end_write_image;
    }

	if (!(codec_ctx = avcodec_alloc_context3(NULL))) {
        ks_throw(ks_type_InternalError, "`avcodec_alloc_context()` failed");
        goto end_write_image;
    }

    if (!(frame = av_frame_alloc())) {
        ks_throw(ks_type_InternalError, "Failed to `av_frame_alloc()`");
        goto end_write_image;
    }

    if (!(packet = av_packet_alloc())) {
        ks_throw(ks_type_InternalError, "Failed to `av_packet_alloc()`");
        goto end_write_image;
    }

    // set codec parameters

	codec_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
	codec_ctx->codec_id = codec_id;
	
	codec_ctx->time_base = (AVRational){ 1, 24 };
	codec_ctx->framerate = (AVRational){ 24, 1 };

    codec_ctx->bit_rate = 400000;
	
    codec_ctx->width = w;
	codec_ctx->height = h;
	codec_ctx->pix_fmt = req_pix_fmt;

    // initialize VBR settings
	codec_ctx->qmin = codec_ctx->qmax = quality;
    codec_ctx->mb_lmin = codec_ctx->qmin * FF_QP2LAMBDA;
	codec_ctx->mb_lmax = codec_ctx->qmax * FF_QP2LAMBDA;
	codec_ctx->flags = AV_CODEC_FLAG_QSCALE;
	codec_ctx->global_quality = codec_ctx->qmin * FF_QP2LAMBDA;

    // open encoder
	if (avst = avcodec_open2(codec_ctx, codec, NULL)) {
        ks_throw(ks_type_InternalError, "`avcodec_open()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        goto end_write_image;
    }

	//
	//	Set the timestamp and quality parameters
	//
	frame->pts = 1;
	frame->quality = codec_ctx->global_quality;


    fp = fopen(fname, "wb");
    if (!fp) {
        ks_throw(ks_type_IOError, "Could not open file '%s': ", fname, strerror(errno));
        goto end_write_image;
    }

    // copy to frame
    frame->format = codec_ctx->pix_fmt;
    frame->width = w;
    frame->height = h;

    if ((avst = av_frame_get_buffer(frame, 64)) < 0) {
        ks_throw(ks_type_InternalError, "`avcodec_frame_get_buffer()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        goto end_write_image;
    }

    if ((avst = av_frame_make_writable(frame)) < 0) {
        ks_throw(ks_type_InternalError, "`aav_frame_make_writable()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        goto end_write_image;
    }

    // now, get the pixel format
    enum AVPixelFormat frame_pix_fmt = frame->format;

    // number of channels in the frame
    int frame_chn = -1, frame_chn_stride = -1;

    // number of channels in 
    int ar_chn = d;

    nx_dtype frame_dtype = NULL;

    // whether or not to add alpha component
    bool frame_has_alpha = false;

    // write to 'frame'
    if (frame_pix_fmt == AV_PIX_FMT_RGBA || frame_pix_fmt == AV_PIX_FMT_RGB24 || frame_pix_fmt == AV_PIX_FMT_RGB0) {
        // we can do it without using swscale

        /**/ if (false) {}        
        else if (frame_pix_fmt == AV_PIX_FMT_RGBA)  { frame_chn = 4; frame_chn_stride = 4; frame_has_alpha = true; }
        else if (frame_pix_fmt == AV_PIX_FMT_RGB24) { frame_chn = 3; frame_chn_stride = 3; }
        else if (frame_pix_fmt == AV_PIX_FMT_RGB0)  { frame_chn = 3; frame_chn_stride = 4; }

        // all these formats are uint8
        frame_dtype = nx_dtype_uint8;


        assert (frame_chn > 0 && frame_chn_stride > 0 && frame_dtype && "layout was not computed correctly!");

        // create a nxar describing the frame data
        nxar_t frame_ar = (nxar_t){
            .data = (void*)frame->data[0],
            .dtype = frame_dtype,
            .rank = 3,
            .dim = (nx_size_t[]){ h, w, frame_chn },
            .stride = (nx_size_t[]){ frame->linesize[0], frame_chn_stride * frame_dtype->size, frame_dtype->size },
        };

        // scaling factor;
        // for converting from float->int, since
        //   integers are fixed point
        double scale_fac = 1.0;

        if (frame_ar.dtype->kind == NX_DTYPE_KIND_CINT && (img.dtype->kind == NX_DTYPE_KIND_CFLOAT || img.dtype->kind == NX_DTYPE_KIND_CCOMPLEX)) {
            scale_fac = (1ULL << (frame_ar.dtype->size * 8)) - 1;
        }

        // minimum channels
        int min_chn = frame_chn < d ? frame_chn : d;


        // set all shared channels at once
        if (!nx_T_mul(
            (nxar_t) {
                .data = img.data,
                .dtype = img.dtype,
                .rank = 3,
                .dim = (nx_size_t[]){ h, w, min_chn },
                .stride = (nx_size_t[]){ img.stride[0], img.stride[1], img.stride[2] },
            },
            (nxar_t) {
                .data = (void*)&scale_fac,
                .dtype = nx_dtype_fp64,
                .rank = 1,
                .dim = (nx_size_t[]){ 1 },
                .stride = (nx_size_t[]){ 0 },
            },
            (nxar_t) {
                .data = frame_ar.data,
                .dtype = frame_ar.dtype,
                .rank = 3,
                .dim = (nx_size_t[]){ h, w, min_chn },
                .stride = (nx_size_t[]) { frame_ar.stride[0], frame_ar.stride[1], frame_ar.stride[2] },
            }
        )) {
            goto end_write_image;
        }

        //printf("%i,%i\n", min_chn, frame_chn);
        // convert the rest over here
        while (min_chn < frame_chn) {

            // calculate fill value
            uint8_t vu8 = (min_chn == (frame_chn - 1) && frame_has_alpha) ? 255 : 0;

            // fill value up
            if (!nx_T_cast(
                (nxar_t) {
                    .data = (void*)&vu8,
                    .dtype = nx_dtype_uint8,
                    .rank = 1,
                    .dim = (nx_size_t[]){ 1 },
                    .stride = (nx_size_t[]){ 0 },
                },
                (nxar_t) {
                    .data = (void*)((intptr_t)frame_ar.data + frame_ar.stride[2] * min_chn),
                    .dtype = frame_ar.dtype,
                    .rank = 2,
                    .dim = (nx_size_t[]){ h, w },
                    .stride = (nx_size_t[]) { frame_ar.stride[0], frame_ar.stride[1] },
                }
            )) {
                goto end_write_image;
            }


            min_chn++;
        }

        frame->pts = 1;

        /* encode the image */
        if (!my_encode(codec_ctx, frame, packet, fp)) goto end_write_image;
        if (!my_encode(codec_ctx, NULL, packet, fp)) goto end_write_image;

        fwrite(endcode, 1, sizeof(endcode), fp);

    } else {
        // we need to convert what we have to it
        ks_throw(ks_type_ToDoError, "Need to implement swscale for other formats");

        goto end_write_image;
    }



    rst = true;
end_write_image:
    // cleanup
    if (codec_ctx) avcodec_close(codec_ctx);
    av_frame_free(&frame);
    av_packet_free(&packet);
    if (fp) fclose(fp);

    return rst;

    #else

    return ks_throw(ks_type_InternalError, "`mm` was compiled without libav, so you can't write images");

    #endif

}





