#!/usr/bin/env ks
""" basic_app.ks - simple example showing usage of the Nuklear API bindings (https://github.com/Immediate-Mode-UI/Nuklear)


@author: Cade Brown <brown.cade@gmail.com>

"""

# import the C bindings for Nuklear (called `cnk`)
# these are typically included in the standard library, unless explicitly disabled.
# So, you shouldn't have to install anything extra
import cnk


# create a context, with a given (w, h, title)
# this will be the state of the application, and will handle everything
ctx = cnk.Context(640, 480, "Basic Application Window")

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

    # 'begin' starts a new panel, with (x, y, w, h) size, and (optional) extra flags from the `cnk.NK_WINDOW_*` enum
    # the body of the 'if' block is executed only if the window is active & visible
    if ctx.begin("Inner Window", 50, 50, 230, 250, cnk.NK_WINDOW_BORDER | cnk.NK_WINDOW_MOVABLE | cnk.NK_WINDOW_SCALABLE | cnk.NK_WINDOW_MINIMIZABLE | cnk.NK_WINDOW_TITLE) {
    
        # create a row given (height, item_width, cols=1)
        ctx.layout_row_static(30, 80, 1)

        # on the current row, create a single button given (label)
        if ctx.button_label("My Button") {
            # if the button was pressed, the body of the if block is ran
            print ("BUTTON WAS PRESSED")
        }
    }

    # end the current panel
    ctx.end()
}

