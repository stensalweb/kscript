
#include "ks.h"


int main(int argc, char** argv) {

    if (!ks_init()) {
        fprintf(stderr, "Failed to initialize kscript!\n");
        return -1;
    }

    ks_log_level_set(KS_LOG_TRACE);


    ks_str s1 = ks_new_str("asdfA");
    ks_str s2 = ks_new_str("Basdf");
    ks_str s3 = ks_new_str("sdfC");
    ks_str s4 = ks_new_str("Dasdf");
    ks_str s5 = ks_new_str("sdfaE");
    ks_str s6 = ks_new_str("Fdf");
    ks_str s7 = ks_new_str("Gasdf");
    ks_str s8 = ks_new_str("sasH");
    ks_str s9 = ks_new_str("Ics");

    ks_dict dc = ks_new_dict(0, NULL);

    ks_dict_set(dc, 0, (ks_obj)s1, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s2, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s3, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s4, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s5, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s6, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s7, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s8, (ks_obj)s2);
    ks_dict_set(dc, 0, (ks_obj)s9, (ks_obj)s2);

    printf("%i\n", (int)dc->n_entries);

    int i;
    for (i = 0; i < dc->n_entries; ++i) {
        if (dc->entries[i].hash != 0) printf("%s: %s\n", ((ks_str)dc->entries[i].key)->chr, ((ks_str)dc->entries[i].val)->chr);
    }

    printf("%s\n", ks_dict_has(dc, 0, (ks_obj)s1) ? "true" : "false");


    ks_call(ks_dict_get(ks_type_str->attr, 0, ks_new_str("mine")), 0, NULL);

    return 0;
}
