/* src/stream.c - implementation of the stream type
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../mm-impl.h"



KS_TYPE_DECLFWD(mm_type_Stream);

mm_Stream mm_Stream_new() {
    // create a new result
    mm_Stream self = KS_ALLOC_OBJ(mm_Stream);
    KS_INIT_OBJ(self, mm_type_Stream);

    self->source_url = NULL;

    // allocate a context
    self->format_ctx = avformat_alloc_context();

    // allocate members
    self->frame = av_frame_alloc();

    // initialize packet
    av_init_packet(&self->packet);

    // empty sub-streams
    self->n_subs = 0;
    self->subs = NULL;

    return self;
}


// open stream
bool mm_Stream_open(mm_Stream self, char* url) {
    if (self->source_url) {
        ks_throw_fmt(ks_type_IOError, "Attempted to open '%s' on a stream that is already open!", url);
        return false;
    }

    self->source_url = ks_str_new(url);

    // attempt to open the file
    if (avformat_open_input(&self->format_ctx, url, NULL, NULL) != 0) {
        ks_throw_fmt(ks_type_IOError, "Could not open file '%s'", url);
        return false;
    }

    self->n_subs = self->format_ctx->nb_streams;

    self->subs = ks_malloc(sizeof(*self->subs) * self->n_subs);

    int i;
    for (i = 0; i < self->format_ctx->nb_streams; ++i) {
        self->subs[i].stream = self->format_ctx->streams[i];
        self->subs[i].codec_ctx = NULL;


        // TODO: maybe add lazy opening
        self->subs[i].isOpen = true;

        self->subs[i].codec_ctx = self->subs[i].stream->codec;

        // attempt to open it
        if (avcodec_open2(self->subs[i].codec_ctx, avcodec_find_decoder(self->subs[i].codec_ctx->codec_id), NULL) < 0) {
            ks_throw_fmt(ks_type_IOError, "Could not open decoder for audio stream (#%i) in file '%s'", i, url);
            return false;
        }

        //printf("OPEN STREAM: '%s'\n", self->subs[i].codec_ctx->codec->name);
    }

    // success
    return true;
}

// Stream.__new__(url) -> create a new Stream
static KS_TFUNC(stream, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str url = (ks_str)args[0];
    KS_REQ_TYPE(url, ks_type_str, "url");

    // create a new stream
    mm_Stream self = mm_Stream_new();

    // attempt to open a stream
    if (!mm_Stream_open(self, url->chr)) {
        KS_DECREF(self);
        return NULL;
    }

    return (ks_obj)self;
}

// Stream.__free__(self) -> free a mm stream
static KS_TFUNC(stream, free) {
    KS_REQ_N_ARGS(n_args, 1);
    mm_Stream self = (mm_Stream)args[0];
    KS_REQ_TYPE(self, mm_type_Stream, "self");

    if (self->source_url != NULL) KS_DECREF(self->source_url);

    ks_free(self->subs);

    // destroy temporary variables & resources
    av_free_packet(&self->packet);
    av_frame_free(&self->frame);
    avformat_free_context(self->format_ctx);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;

}


static ks_namespace my_getinfo(int i, struct mm_Stream_sub* sub) {
    ks_namespace this_info = ks_namespace_new(NULL);

    // determine media type
    enum mm_media_type medtyp;
    if (sub->codec_ctx->codec->type == AVMEDIA_TYPE_AUDIO) {
        medtyp = MM_MEDIA_TYPE_AUDIO;
    } else if (sub->codec_ctx->codec->type == AVMEDIA_TYPE_VIDEO) {
        medtyp = MM_MEDIA_TYPE_VIDEO;
    } else {
        medtyp = MM_MEDIA_TYPE_NONE;
    }

    
    // create namespace with everything
    ks_dict_set_cn(this_info->attr, (ks_dict_ent_c[]){

        {"idx",         (ks_obj)ks_int_new(i)},

        {"codec",       (ks_obj)ks_str_new(sub->codec_ctx->codec->name)},

        {"media_type",  (ks_obj)ks_Enum_get_i(mm_Enum_MediaType, medtyp)},
        
        {"size",        (medtyp == MM_MEDIA_TYPE_VIDEO ? (ks_obj)ks_build_tuple("%i %i", (int)sub->codec_ctx->width, (int)sub->codec_ctx->height) : KSO_NONE)},

        {NULL, NULL}
    });

    return this_info;
}


// Stream.get_info(self, idx=none) -> return stream informatio about either a specific stream, or all of them
static KS_TFUNC(stream, get_info) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    mm_Stream self;
    int64_t idx = -1;
    if (!ks_parse_params(n_args, args, "self%* ?idx%i64", &self, mm_type_Stream, &idx)) return NULL;


    if (idx < 0) {
        // do all streams

        // create a new list
        ks_list ret = ks_list_new(0, NULL);


        int i;
        for (i = 0; i < self->n_subs; ++i) {

            ks_namespace this_info = my_getinfo(i, &self->subs[i]);
            if (!this_info) {
                return NULL;
            }
                    
            ks_list_push(ret, (ks_obj)this_info);
            KS_DECREF(this_info);
        }

        return (ks_obj)ret;

    } else {
        if (idx >= self->n_subs) {
            return ks_throw_fmt(ks_type_ArgError, "Index #%i is out of range for a mm.Stream with %i substreams", idx, self->n_subs);
        }
        return (ks_obj)my_getinfo(idx, &self->subs[idx]);

        //return ks_throw_fmt(ks_type_ToDoError, "Need to support specific stream infos");
    }

}


// initialize the type
void mm_init_type_Stream() {
    
    KS_INIT_TYPE_OBJ(mm_type_Stream, "mm.Stream");

    ks_type_set_cn(mm_type_Stream, (ks_dict_ent_c[]){

        {"__new__",     (ks_obj)ks_cfunc_new2(stream_new_, "mm.Stream.__new__(url)")},
        {"__free__",     (ks_obj)ks_cfunc_new2(stream_free_, "mm.Stream.__free__(self)")},


        {"get_info",     (ks_obj)ks_cfunc_new2(stream_get_info_, "mm.Stream.get_info(self, idx=-1)")},


        {NULL, NULL},
    });


}
