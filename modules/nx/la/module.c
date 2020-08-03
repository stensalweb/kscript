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

    ks_dict_set_cn(submod->attr, (ks_dict_ent_c[]){


        {NULL, NULL}
    });


    ks_dict_set_cn(nxmod->attr, (ks_dict_ent_c[]){

        {SUBMOD,        (ks_obj)submod},

        {NULL, NULL}
    });

}



