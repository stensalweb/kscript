/* types/blob.c - implementation of the blob type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"

// forward declare it
KS_TYPE_DECLFWD(ks_type_blob);

// create new blob
ks_blob ks_blob_new(ks_size_t len, uint8_t* data) {
    ks_blob self = (ks_blob)ks_malloc(sizeof(*self) + len);
    KS_INIT_OBJ(self, ks_type_blob);

    // set the length
    self->len = len;

    // copy in the data
    memcpy(self->data, data, len);

    // calculate the hash for the string when it gets created
    self->v_hash = ks_hash_bytes(self->len, self->data);

    return self;
}


// blob.__new__(obj, *args) -> convert object to blob
static KS_TFUNC(blob, new) {
    KS_REQ_N_ARGS_MIN(n_args, 1);
    ks_obj obj = args[0];

    if (obj->type == ks_type_blob) {
        return KS_NEWREF(obj);
    } else if (obj->type == ks_type_str) {
        return (ks_obj)ks_blob_new(((ks_str)obj)->len, ((ks_str)obj)->chr);
    } else if (obj->type->__blob__ != NULL) {
        return ks_call(obj->type->__blob__, n_args, args);
    } else {
        KS_ERR_CONV(obj, ks_type_blob);
    }
};

// blob.__free__(self) -> free a blob object
static KS_TFUNC(blob, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_blob self = (ks_blob)args[0];
    KS_REQ_TYPE(self, ks_type_blob, "self");

    // nothing else is needed because the string is allocated with enough bytes for all the characters    
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// blob.__len__(self) -> get the length
static KS_TFUNC(blob, len) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_blob self = (ks_blob)args[0];
    KS_REQ_TYPE(self, ks_type_blob, "self");

    return (ks_obj)ks_int_new(self->len);
};

// blob.__str__(self, encoding='utf-8') -> get the string
static KS_TFUNC(blob, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_blob self = (ks_blob)args[0];
    KS_REQ_TYPE(self, ks_type_blob, "self");

    return (ks_obj)ks_str_new_l(self->data, self->len);
};

// blob.__repr__(self) -> get the string representation
static KS_TFUNC(blob, repr) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_blob self = (ks_blob)args[0];
    KS_REQ_TYPE(self, ks_type_blob, "self");

    return (ks_obj)ks_fmt_c("blob[%z]", self->len);
};



// initialize string type
void ks_type_blob_init() {
    KS_INIT_TYPE_OBJ(ks_type_blob, "blob");


    // set properties
    ks_type_set_cn(ks_type_blob, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(blob_new_, "blob.__new__(obj, *args)")},
        {"__free__", (ks_obj)ks_cfunc_new2(blob_free_, "blob.__free__(self)")},

        {"__len__", (ks_obj)ks_cfunc_new2(blob_len_, "blob.__len__(self)")},

        {"__str__", (ks_obj)ks_cfunc_new2(blob_str_, "blob.__str__(self, encoding='utf-8')")},
        {"__repr__", (ks_obj)ks_cfunc_new2(blob_repr_, "blob.__repr__(self)")},


        {NULL, NULL},
    });


}

