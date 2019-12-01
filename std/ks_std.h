
#ifndef KS_STD_H_
#define KS_STD_H_

#define MODULE_NAME std
#include "../src/kscript-module.h"

// so name collisions dont occur
#undef bool
#undef int
#undef float

/* list of types we'll be registering */
extern REGISTER_FUNC(None);
extern REGISTER_FUNC(int);
extern REGISTER_FUNC(bool);
extern REGISTER_FUNC(float);
extern REGISTER_FUNC(str);

/* the main functions */
extern REGISTER_FUNC(builtins0);


// some extras
extern REGISTER_FUNC(list);


#endif


