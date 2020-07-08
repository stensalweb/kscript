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


#ifdef KS_HAVE_LIBAV



// internal decoding routine
static bool my_decode(AVCodecContext *codec_ctx, AVFrame *frame, AVPacket *packet) {

    int avst;

    if ((avst = avcodec_send_packet(codec_ctx, packet)) < 0) {
        ks_throw_fmt(ks_type_InternalError, "`avcodec_send_packet()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
        return false;
    }

    while (avst >= 0) {
        avst = avcodec_receive_frame(codec_ctx, frame);
        if (avst == AVERROR(EAGAIN) || avst == AVERROR_EOF) {
            return true;
        } else if (avst < 0) {
            ks_throw_fmt(ks_type_InternalError, "`avcodec_recieve_frame()` failed (code: %i, reason: %s)", avst, av_err2str(avst));
            return false;
        }

        //printf("saving frame %3d\n", codec_ctx->frame_number);
        //fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
        //snprintf(buf, sizeof(buf), filename, dec_ctx->frame_number);
        //pgm_save(frame->data[0], frame->linesize[0],
        //         frame->width, frame->height, buf);
    }
}

#endif



