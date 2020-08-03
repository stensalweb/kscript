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


// mm_media_type - details which kind of media a given object is
enum mm_media_type {

    // none (autodetect), or error
    MM_MEDIA_TYPE_NONE    = 0x0,

    // audio data
    MM_MEDIA_TYPE_AUDIO   = 0x1,

    // video/picture data
    MM_MEDIA_TYPE_VIDEO   = 0x2,

};


// mm_flags - generic flags
enum mm_flags {

    // none/no specific behavior
    MM_FLAGS_NONE         = 0x0,

    // Specifies/requests a grayscale image
    MM_FLAGS_GREY         = 0x1,

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

// Read, then decode an image file to an array, then return the array
// By default, a tensor of (h, w, d) is returned, with `d` being the number of channels
//   normally 3 or 4, and ordered RGB[A].
// By default, the tensor contains elements of `fp32` (i.e. floats) between [0, 1] for all channels
// `flags` controls the behavior (pass '0' or MM_FLAGS_NONE for the default behavior)
// MM_FLAGS_GREY: Returns a greyscale image of size (h, w)
// NOTE: Returns a new reference, or NULL and throws an error
KS_API nx_array mm_read_image(char* fname, enum mm_flags flags);

// Interperet 'img' as an image with shape (h, w[, d]), and encode it to a file
// It is assumped to be in a few different formats, based on size:
//  If img.rank==3, then it is interpretered as (h, w, d), with 'd' being the number of color channels,
//    which should be either 3 or 4 (in which case it is interpreted as RGB or RGBA)
//  If img.rank==2, then it is interpreretd as (h, w) with the last channel being greyscale
// As far as type conversions, we have:
//   if img.dtype->kind == CINT, then it is assumed to be 8 bit unsigned fixed point. Modulo is performed
//   if img.dtype->kind == CFLOAT or CCOMPLEX, then the value is scaled up (i.e. *255) before conversion. Modulo is performed
//     (complex values are casted to their real components only)
//   Otherwise, an error is thrown, as the type must be numeric
// TODO: Should saturating the input be supported, or modulo kept as the standard?
// NOTE: Returns whether successful, or false and throws an error
KS_API bool mm_write_image(char* fname, nxar_t img, enum mm_flags flags);



#endif /* MM_H__ */
