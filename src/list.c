/* list.c - implementation of a reference list in C */

#include "kscript.h"

// pushes on, incrementing the reference count
int ks_list_push(ks_list* list, kso obj) {
    int idx = list->len++;
    if (list->len > list->max_len) {
        list->max_len = (uint32_t)(list->len * 1.25 + 5);
        list->items = ks_realloc(list->items, sizeof(kso) * list->max_len);
    }
    KSO_INCREF(obj);
    list->items[idx] = obj;
    return idx;
}

// pops off, transfers reference to callee
kso ks_list_pop(ks_list* list) {
    int idx = --list->len;
    kso obj = list->items[idx];
    return obj;
}

// pops off, eliminates reference
void ks_list_popu(ks_list* list) {
    int idx = --list->len;
    kso obj = list->items[idx];
    KSO_DECREF(obj);
}

// ks_frees 'list's resources
void ks_list_free(ks_list* list) {
    int i;
    for (i = 0; i < list->len; ++i) {
        KSO_DECREF(list->items[i]);
    }
    ks_free(list->items);
    *list = KS_LIST_EMPTY;
}


