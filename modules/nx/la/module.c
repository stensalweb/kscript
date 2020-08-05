/* la/module.c - `nx.la` module
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

#include "../nx-impl.h"

#define SUBMOD "la"


// add the LA module to `nxmod`
void nx_mod_add_la(ks_module nxmod) {


    ks_module submod = ks_module_new("nx." SUBMOD);

    ks_dict_set_c(submod->attr, KS_KEYVALS(

    ));


    ks_dict_set_c(nxmod->attr, KS_KEYVALS(

        {SUBMOD,        (ks_obj)submod},

    ));

}



