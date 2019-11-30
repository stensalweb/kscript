// kscript.c - the main commandline interface to the kscript library
//
// @author   : Cade Brown <cade@chemicaldevelopment.us>
// @license  : WTFPL (http://www.wtfpl.net/)
//

#include "kscript.h"

int main(int argc, char** argv) {

    ks_obj sconst = ks_obj_new_str(KS_STR_CONST("I AM OBJECT"));

    if (sconst->type == KS_TYPE_STR) {
        ks_info("%s", sconst->_str._);
    } else {
        ks_error("Type wasn't str!");
    }

    // always free when you're done!
    ks_obj_free(sconst);

    return 0;
}


