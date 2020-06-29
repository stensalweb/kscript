#!/usr/bin/env ks
""" calc.ks - direct port of the calculator demo for Nuklear (https://github.com/Immediate-Mode-UI/Nuklear/blob/master/demo/calculator.c)




@author: Cade Brown <brown.cade@gmail.com>
"""

# import the C bindings for Nuklear (called `cnk`)
# these are typically included in the standard library, unless explicitly disabled.
# So, you shouldn't have to install anything extra
import cnk

# import the getarg library, to parse options
import getarg


# -*- PARSE ARGUMENTS

p = getarg.Parser("calc", "0.0.1", "Calculator application using Nuklear bindings", ["Cade Brown <brown.cade@gmail.com>"])

p.add_arg_multi ("size", "Window size of the application", ["-s", "--size"], 2, int, [1200, 900])

args = p.parse()


# -*- DO GUI APPLICATION

# create a context, with a given (w, h, title)
# this will be the state of the application, and will handle everything
ctx = cnk.Context(args.size[0], args.size[1], "Basic Application Window")

# calculator buttons
calc_buttons = [
    "789+",
    "456-",
    "123*",
    "C0=/"
]

# operators
ops = "+-*/"

input_str = "0"

# values
vals = [0.0, 0.0]
cur_op = ""

# perfor operation
func do_op(op, a, b) {
    if op == "" {
        # just take last value
        ret b
    } else if op == "+" {
        ret a + b
    } else if op == "-" {
        ret a - b
    } else if op == "*" {
        ret a * b
    } else if op == "/" {
        ret a / b
    } else {
        throw Error("Undefined OP!")
    }
}


# loop continuously
for frame in ctx {

    # 'begin' starts a new panel, with (x, y, w, h) size, and (optional) extra flags from the `cnk.NK_WINDOW_*` enum
    # the body of the 'if' block is executed only if the window is active & visible
    if ctx.begin("Calc (4 Function)", 50, 50, 360, 320, cnk.NK_WINDOW_MOVABLE | cnk.NK_WINDOW_MINIMIZABLE | cnk.NK_WINDOW_TITLE | cnk.NK_WINDOW_NO_SCROLLBAR) {

        # create a row
        ctx.layout_row_dynamic(40, 1)

        # edit the input string
        input_str = ctx.edit_string(cnk.NK_EDIT_SIMPLE, input_str, 64)

        # replace leading 0s
        while len(input_str) > 1 && input_str[0] == '0' && !(len(input_str) > 2 && input_str[1] == '.'), input_str = input_str.substr(1)

        # create a row
        ctx.layout_row_dynamic(56, 4)

        # do buttons (i, j)
        for row in calc_buttons {
            for bt in row {
                if ctx.button_label(bt) {
                    # remove 0s at the beginning
                    if bt.isdigit() {
                        input_str = input_str + bt
                    } else if ops.contains(bt) {

                        if cur_op != "" {
                            vals[1] = do_op(cur_op, vals[1], float(input_str))
                            vals[0] = 0.0
                        } else {
                            # cycle the values
                            vals[0] = vals[1]; vals[1] = float(input_str)
                        }

                        # reset the input string
                        input_str = "0"

                        cur_op = bt
                    } else if bt == "=" {
                        # cycle the values
                        vals[0] = vals[1]; vals[1] = float(input_str)
                        # reset the input string
                        input_str = "0"

                        # calculate result
                        out = do_op(cur_op, vals[0], vals[1])

                        # reset cur op
                        cur_op = ""

                        # reset internal values
                        vals[0] = vals[1] = 0.0

                        input_str = str(out)
                    } else if bt == "C" {
                        vals[0] = vals[1] = 0.0
                        input_str = "0"
                        cur_op = ""
                    }
                }
            }
        }
    }

    # end the current panel
    ctx.end()
}

