# `modules/cnk` - C bindings for Nuklear

This library is dedicated to binding the [Nuklear](https://github.com/Immediate-Mode-UI/Nuklear) library, in a straightforward manner

## Building

This requires the `GLFW3` libraries, which should be automatically detected at `./configure`. To cause an error if this is not supported, run with `./configure --with-glfw3`

## Exported Types

`cnk.Context` -> a wrapper around `struct nk_context*` and an atlas map, and a `GLFWWindow*`. In general, this is an entire application, and contains all the neccessary data


## Exported Functions

There are a lot of functions exported, and it would take a long time to list them all here. Most of the functions from the [Nuklear API docs](https://immediate-mode-ui.github.io/Nuklear/doc/nuklear.html#api) are wrapped as written.

The changes made are:

  * `nk_rect` parameters are split up into 4 respective parameters indicating the `x, y, w, h` components (which may be integers or floats)
  * Functions indicating success (i.e. by returning an integer) return a boolean indicating success in kscript
  * Functions accepting flags as a final argument have a default of 0 (i.e. none of the flags set)


Except, `nk_rect` arguments are 4 seperate arguments in most cases. So, for example:

```
// Nuklear C API funcs
void nk_layout_row_static(struct nk_context *ctx, float height, int item_width, int cols);
int nk_begin(struct nk_context *ctx, const char *title, struct nk_rect bounds, nk_flags flags);
```

becomes

```
# cnk, kscript binding functions
Context.layout_row_static(height, item_width) -> none
Context.begin(title, bounds_x, bounds_y, bounds_w, bounds_h, flags=0) -> bool
```


