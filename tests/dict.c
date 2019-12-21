/* tests/dict.c - testing dictionary functionality */

#include "kscript_test.h"



int main(int argc, char** argv) {

    ks_dict d = KS_DICT_EMPTY;


    ks_dict_free(&d);

    return 0;
}

