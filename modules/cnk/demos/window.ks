#!/usr/bin/env ks
""" basic_app.ks - simple example showing usage of the Nuklear API bindings (https://github.com/Immediate-Mode-UI/Nuklear)


@author: Cade Brown <brown.cade@gmail.com>

"""

# import the C bindings for Nuklear (called `cnk`)
# these are typically included in the standard library, unless explicitly disabled.
# So, you shouldn't have to install anything extra
import cnk
print ("imported cnk")

# create a context, with a given (w, h, title)
# this will be the state of the application, and will handle everything
ctx = cnk.Context(640, 480, "Basic Application Window")

print ("created context")

# You can use the `for' loop, which makes it easy to continually loop every frame, until the 'x' button has been hit,
#   or there has been another action that would normally cause the application to quit
# Alternatively, you can do the more fine-graind syntax shown in the triple quote comment below:
"""
while ctx.frame_begin() {
    # rendering code here
    ctx.frame_end()
}
"""
for frame in ctx {

    print ("frame:", frame)
}

