
#ifndef KS_STD_H_
#define KS_STD_H_

#define MODULE_NAME std
#include "../src/kscript-module.h"


/* list of types we'll be registering */
extern REGISTER_FUNC(None);
extern REGISTER_FUNC(int);
extern REGISTER_FUNC(bool);
extern REGISTER_FUNC(float);
extern REGISTER_FUNC(str);

// some extras
extern REGISTER_FUNC(list);


#endif


