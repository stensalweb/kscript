/* list.c - implementation of a list in C */

#include "kscript.h"

// pushes an object reference to the list, returns the index
int ks_list_push(ks_list* list, kso obj) {
    int idx = list->len++;
    if (list->len > list->max_len) {
        list->max_len = (uint32_t)(list->len * 1.25 + 5);
        list->items = realloc(list->items, sizeof(kso) * list->max_len);
    }
    list->items[idx] = obj;
    return idx;
}
// frees 'list's resources
void ks_list_free(ks_list* list) {
    free(list->items);
    *list = KS_LIST_EMPTY;
}


