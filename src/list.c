/* list.c - implementation of a list in C */

#include "kscript.h"

// pushes an object reference to the list, returns the index
int ks_list_push(ks_list* list, kso obj) {
    int idx = list->len++;
    if (list->len > list->max_len) {
        list->max_len = (uint32_t)(list->len * 1.25 + 5);
        list->items = ks_realloc(list->items, sizeof(kso) * list->max_len);
    }
    list->items[idx] = obj;
    return idx;
}
//#define ks_list_pop(_list) ((_list)->items[--(_list)->len])

kso ks_list_pop(ks_list* list) {
    int idx = --list->len;
    kso obj = list->items[idx];
    return obj;
}

// ks_frees 'list's resources
void ks_list_free(ks_list* list) {
    ks_free(list->items);
    *list = KS_LIST_EMPTY;
}


