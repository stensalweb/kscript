/* modules/mm/mm.h - the kscript multi-media (mm) library
 *
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#pragma once
#ifndef MM_H__
#define MM_H__

// include the main kscript API
#include <ks.h>


#ifdef KS_HAVE_MODULE_NX

// we use the NumeriX library for decoding/encoding data from images & audio
#include <nx.h>

#else

#error Building 'mm' library without 'nx' library is impossible

#endif


#ifdef KS_HAVE_LIBAV

// ffmpeg/libav headers 
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#include <libswscale/swscale.h>

#else

#error Building 'mm' library without libav is impossible (run `./configure --with-libav`)

#endif


/* TYPE */


// detailing which types of media
enum mm_media_type {

    MM_MEDIA_TYPE_NONE    = 0x0,
    MM_MEDIA_TYPE_AUDIO   = 0x1,
    MM_MEDIA_TYPE_VIDEO   = 0x2,
};

// mm.Stream - an audio/video stream that can be opened by any URL
typedef struct {
    KS_OBJ_BASE

    // what is the source URL of the stream?
    // can be file name, URL online, etc
    ks_str source_url;

    /* libav vars */


    #ifdef KS_HAVE_LIBAV

    // A handle to the current format
    AVFormatContext* format_ctx;

    // single frame
    AVFrame* frame;

    // single packet variable
    AVPacket packet;

    #endif


    // individual streams
    int n_subs;

    struct mm_Stream_sub {

        // whether or not the stream's codec has been opened
        bool isOpen;

        #ifdef KS_HAVE_LIBAV

        // stream
        AVStream* stream;

        // Codec context for decoding/encoding values
        // NOTE: Only valid if !=NULL && isOpen
        AVCodecContext* codec_ctx;

        #endif

    }* subs;


}* mm_Stream;


// forward decl types
extern ks_type mm_type_Stream;

// enu mtypes
extern ks_type mm_Enum_MediaType;


// Create a new Stream object
// NOTE: Returns a new reference
// NOTE: Use mm_Stream_open() to actually read things
KS_API mm_Stream mm_Stream_new();


// Open a stream, returning success
KS_API bool mm_Stream_open(mm_Stream self, char* url);



/* I/O */

// Read a file as a binary object, and return the whole thing
// NOTE: Returns a new reference
KS_API ks_blob mm_read_file(char* fname);

// Read a file as an image array
// TODO: Allow format conversion specifications (i.e. return as uint8's, convert to black&white, alpha, etc)
// NOTE: Returns a new reference
KS_API nx_array mm_read_image(char* fname);

KS_API bool mm_write_image(char* fname, nxar_t img);



#endif /* MM_H__ */
