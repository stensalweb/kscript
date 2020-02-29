/* types/str.c - kscript's basic string implementation
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_str);

// create a kscript string from a C-style string with a length (not including NUL-terminator)
ks_str ks_new_str_l(int len, char* chr) {
    ks_str self = (ks_str)ks_malloc(sizeof(struct ks_str) + len);
    KS_INIT_OBJ(self, ks_type_str);

    // set the length
    self->len = len;

    // copy in all the data
    memcpy(self->chr, chr, len);
    self->chr[len] = '\0';

    // calculate the hash for the string when it gets created
    self->v_hash = ks_hash_bytes(self->len, self->chr);

    return self;
}

// create a kscript string from a C-style string
ks_str ks_new_str(char* chr) {
    return ks_new_str_l(chr == NULL ? 0 : strlen(chr), chr);
}


// free a kscript string
void ks_free_str(ks_int self) {
    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);
}

int ks_str_cmp(ks_str A, ks_str B) {
    if (A->len != B->len) return A->len - B->len;
    else return memcmp(A->chr, B->chr, A->len);
}


static KS_TFUNC(str, mine) {

    printf("TEST\n");

    return KSO_NONE;
};


// initialize string type
void ks_type_str_init() {
    KS_INIT_TYPE_OBJ(ks_type_str);


    // set properties
    ks_dict_set_cn(ks_type_str->attr, (ks_dict_ent_c[]){
        {"mine", (ks_obj)ks_new_cfunc(str_mine_)},
        {NULL, NULL}
    });

}

