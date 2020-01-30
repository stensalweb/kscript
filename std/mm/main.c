/* main.c - main module file */

// module header
#include "ksm_mm.h"

// since this is a ks_module, include the boilerplate
#define MODULE_NAME "mm"
#include "ks_module.h"

// declare the mm.Audio type
ks_type ks_T_mm_Audio = NULL; 

// initialization code for the module
MODULE_INIT() {

    // initalize libav
    //av_register_all();

    // create the module
    ks_module mod = ks_module_new_c(MODULE_NAME);

    // helper macro to add a C function to a type
    #define ADDCF(_type, _name, _sig, _fn) { \
        kso _f = (kso)ks_cfunc_new(_fn, _sig); \
        ks_type_setattr_c(_type, _name, _f); \
        KSO_DECREF(_f); \
    }

    // create a list of all supported formats
    ks_list all_codecs = ks_list_new_empty();
    ks_list audio_codecs = ks_list_new_empty();
    ks_list video_codecs = ks_list_new_empty();

    /*
    // iterate through codecs
    const AVCodec* codec = NULL;
    // iterator
    void* i = 0;
    while (codec = av_codec_iterate(&i)) {
        ks_str c_codec = ks_str_new(codec->long_name);

        // add them
        ks_list_push(all_codecs, (kso)c_codec);

        if (codec->type == AVMEDIA_TYPE_AUDIO) ks_list_push(audio_codecs, (kso)c_codec);
        if (codec->type == AVMEDIA_TYPE_VIDEO) ks_list_push(video_codecs, (kso)c_codec);

        KSO_DECREF(c_codec);
    }
    */
    MODULE_ADD_VAL(mod, "codecs", all_codecs);
    MODULE_ADD_VAL(mod, "codecs_audio", audio_codecs);
    MODULE_ADD_VAL(mod, "codecs_video", video_codecs);

    // create our type
    ks_T_mm_Audio = ks_type_new("mm.Audio");

    // populate with member functions    
    ADDCF(ks_T_mm_Audio, "__new__",     "mm.Audio.__new__(self)",     mm_Audio_new_);
    ADDCF(ks_T_mm_Audio, "__str__",     "mm.Audio.__str__(self)",     mm_Audio_str_);
    ADDCF(ks_T_mm_Audio, "__repr__",    "mm.Audio.__repr__(self)",    mm_Audio_repr_);
    ADDCF(ks_T_mm_Audio, "__getattr__", "mm.Audio.__getattr__(self)", mm_Audio_getattr_);
    ADDCF(ks_T_mm_Audio, "__getitem__", "mm.Audio.__getitem__(self, idx)", mm_Audio_getitem_);
    ADDCF(ks_T_mm_Audio, "__setitem__", "mm.Audio.__setitem__(self, idx, val)", mm_Audio_setitem_);
    ADDCF(ks_T_mm_Audio, "__free__",    "mm.Audio.__free__(self)",    mm_Audio_free_);
    ADDCF(ks_T_mm_Audio, "write",       "mm.Audio.write(self, fname)",    mm_Audio_write_);


    /* add types to module */

    MODULE_ADD_TYPE(mod, "Audio", ks_T_mm_Audio);

    // return our module
    return (kso)mod;
}

