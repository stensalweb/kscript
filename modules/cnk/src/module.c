/* cnk/src/module.c - the kscript Nuklear bindings
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "cnk"

// include this since this is a module.
#include "ks-module.h"

/* GFX LIBS */

// include OpenGL/GLFW libraries
#include "./ext/gl3w_gl3.h"
#include <GLFW/glfw3.h>

// versions of OpenGL to try to init
static int _try_GLvers[][2] = {
    {  3,  3 },
    { -1, -1 }
};

// the OpenGL version being used
static int _GLvers[2] = { -1, -1 };

// Nuklear options for maximum vertex buffer size
//   & maximum element buffer size
static int  _NK_max_verbuf = 512 * 1024,
            _NK_max_elebuf = 128 * 1024;


// Nuklear options
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_KEYSTATE_BASED_INPUT

// actually define the implementation in this file
#define NK_IMPLEMENTATION

// use GLFW with OpenGL 3 implementation
#define NK_GLFW_GL3_IMPLEMENTATION

// include Nuklear headers
#include "./ext/nuklear.h"
#include "./ext/nuklear_glfw_gl3.h"


/* LIB FUNCTIONS */

// error callback to be used by GLFW
static void _GLFW_errorcb(int er, const char* d) {
    ks_warn("GLFW: %s [code: %d]", er, d);
}



/* WRAPPER TYPES */

// cNk_Context - class wrapping the nk_context object, as well as a window
typedef struct {
    KS_OBJ_BASE

    // the GLFW window the context is bound to
    GLFWwindow* window;

    // the main Nuklear context
    struct nk_context* ctx;

    // font atlas for fonts used by the context
    struct nk_font_atlas* atlas;

    // the RGB components of the background
    float background_color[3];

}* cNk_Context; 


// declare the context
KS_TYPE_DECLFWD(cNk_type_Context);


// cNk_iter_Context - iterable wrapper
typedef struct {
    KS_OBJ_BASE

    // the context object
    cNk_Context context;

    // current frame count, statistics
    int frame_n;

}* cNk_iter_Context;

// declare an iterable type to wrap it
KS_TYPE_DECLFWD(cNk_type_iter_Context);


/* Context class */

/* Context.__new__(width, height, title) -> Context
 *
 * Construct a new Nuklear context object
 * 
 */
static KS_TFUNC(Context, new) {
    KS_REQ_N_ARGS(n_args, 3);
    ks_int width = (ks_int)args[0], height = (ks_int)args[1];
    KS_REQ_TYPE(width, ks_type_int, "width");
    KS_REQ_TYPE(height, ks_type_int, "height");
    ks_str title = (ks_str)args[2];
    KS_REQ_TYPE(title, ks_type_str, "title");

    cNk_Context self = KS_ALLOC_OBJ(cNk_Context);
    KS_INIT_OBJ(self, cNk_type_Context);

    // construct a new GLFW window with the given parameters
    self->window = glfwCreateWindow(width->val, height->val, title->chr, NULL, NULL);
    glfwMakeContextCurrent(self->window);

    // create a context from the GLFW window
    self->ctx = nk_glfw3_init(self->window, NK_GLFW3_INSTALL_CALLBACKS);

    /* load font assets */
    nk_glfw3_font_stash_begin(&self->atlas);

    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/

    nk_glfw3_font_stash_end();


    /* load cursor assets */

    // nk_style_load_all_cursors(ctx, atlas->cursors);


    self->background_color[0] = self->background_color[1] = self->background_color[2] = 0.1f;



    return (ks_obj)self;
}

/* Context.__free__(self) -> none
 *
 * Free a Nuklear context object
 * 
 */
static KS_TFUNC(Context, free) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    // clean up context
    nk_free(self->ctx);

    // clean up font atlas
    nk_font_atlas_cleanup(self->atlas);

    // destroy the window the context was created around
    glfwDestroyWindow(self->window);

    return KSO_NONE;
}

/* Context.__getattr__(self, attr)
 *
 * Get an attribute
 * 
 */
static KS_TFUNC(Context, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");

    // attempt to get one of the attributes
    if (attr->len == 4 && strncmp(attr->chr, "size", 4) == 0) {
        // return (width, height) tuple of the window size
        int ww, wh;
        glfwGetWindowSize(self->window, &ww, &wh);
        return (ks_obj)ks_tuple_new_n(2, (ks_obj[]){ (ks_obj)ks_int_new(ww), (ks_obj)ks_int_new(wh) });
    } else {

        // now, try getting a member function
        ks_obj ret = ks_type_get_mf(self->type, attr, (ks_obj)self);
        if (!ret) {
            KS_ERR_ATTR(self, attr);
        }

        return ret;
    }
}

/* Context.__setattr__(self, attr, val)
 *
 * Set an attribute
 * 
 */
static KS_TFUNC(Context, setattr) {
    KS_REQ_N_ARGS(n_args, 3);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");
    ks_obj val = args[2];

    // attempt to get one of the attributes
    if (attr->len == 4 && strncmp(attr->chr, "size", 4) == 0) {
        KS_REQ_ITERABLE(val, "val");
        ks_list val_list = ks_list_from_iterable(val);
        if (val_list->len != 2) {
            KS_DECREF(val_list);
            return ks_throw_fmt(ks_type_ArgError, "Attribute 'size' must be an iterable of size 2, containing the integers '(width, height)'");
        }


        // window width & height
        int64_t sizes[2];

        int i;
        for (i = 0; i < 2; ++i) {
            if (!ks_num_getint64(val_list->elems[i], &sizes[i])) {
                KS_DECREF(val_list);
                return ks_throw_fmt(ks_type_ArgError, "Attribute 'size' must be an iterable of size 2, containing the integers '(width, height)'");
            }
        }

        KS_DECREF(val_list);

        // set the size
        glfwSetWindowSize(self->window, sizes[0], sizes[1]);
        return KSO_NONE;
    } else {
        KS_ERR_ATTR(self, attr);
    }
}



/* Context.frame_start(self) -> bool
 *
 * Start rendering a frame, return whether it should keep going
 * 
 */
static KS_TFUNC(Context, frame_start) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    // update GLFW things
    glfwPollEvents();
    nk_glfw3_new_frame();

    // return whether it should keep going
    return KSO_BOOL(!glfwWindowShouldClose(self->window));
}


/* Context.frame_end(self) -> bool
 *
 * End rendering a frame, and return whether it should keep going
 * 
 */
static KS_TFUNC(Context, frame_end) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    // query the actual size
    int dw, dh;
    glfwGetWindowSize(self->window, &dw, &dh);

    // render the entire window
    glViewport(0, 0, dw, dh);

    // set background color
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // use the Nuklear GLFW3 render helper (with our configuration of max memory)
    nk_glfw3_render(NK_ANTI_ALIASING_ON, _NK_max_verbuf, _NK_max_elebuf);

    // swap buffers
    glfwSwapBuffers(self->window);

    // return whether it should keep going
    return KSO_BOOL(!glfwWindowShouldClose(self->window));
}




/* Context.begin(self, title, x, y, w, h, flags=cnk.NK_WINDOW_NONE) -> bool
 *
 * Construct a window with a given title, at a given position. Return true if it is visible
 * 
 */
static KS_TFUNC(Context, begin) {
    KS_REQ_N_ARGS_RANGE(n_args, 6, 7);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    ks_str title = (ks_str)args[1];
    KS_REQ_TYPE(title, ks_type_str, "title");

    // parse float parameters
    double x, y, w, h;

    // and flags parameters
    int64_t flags = 0;

    if (
        !ks_num_getdouble(args[2], &x) 
     || !ks_num_getdouble(args[3], &y) 
     || !ks_num_getdouble(args[4], &w) 
     || !ks_num_getdouble(args[5], &h)
     || (n_args >= 7 && !ks_num_getint64(args[6], &flags))
    ) {
         return NULL;
    }

    // NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE

    // begin the window process, with given parameters
    int res = nk_begin(self->ctx, title->chr, nk_rect(x, y, w, h), flags);

    return KSO_BOOL(res);
}

/* Context.end(self) -> none
 *
 * Finish & draw the context
 * 
 */
static KS_TFUNC(Context, end) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    nk_end(self->ctx);
   
    return KSO_NONE;
}


/***  LAYOUT FUNCTIONS  ***/



/* Context.layout_row_static(self, height, item_width, cols=1) -> bool
 *
 * Create a button, returning true if pressed
 * 
 */
static KS_TFUNC(Context, layout_row_static) {
    KS_REQ_N_ARGS_RANGE(n_args, 3, 4);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    // declare parameters & convert arguments
    double height;
    int64_t item_width, cols = 1;

    if (
        !ks_num_getdouble(args[1], &height) ||
        !ks_num_getint64(args[2], &item_width) ||
        (n_args < 4 || !ks_num_getint64(args[3], &cols))
    ) {
        return NULL;
    }

    // call internal library function
    nk_layout_row_static(self->ctx, height, item_width, cols);

    return KSO_NONE;
}




/* Context.button(self, label) -> bool
 *
 * Create a button, returning true if pressed
 * 
 */
static KS_TFUNC(Context, button) {
    KS_REQ_N_ARGS(n_args, 2);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");
    ks_str label = (ks_str)args[1];
    KS_REQ_TYPE(label, ks_type_str, "label");

    int res = nk_button_label(self->ctx, label->chr);

    return KSO_BOOL(res);
}



/* iter_Context class */


/* iter_Context.__free__(self) -> none
 *
 * Free a Nuklear context iterator object
 * 
 */
static KS_TFUNC(iter_Context, free) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_iter_Context self = (cNk_iter_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_iter_Context, "self");

    KS_DECREF(self->context);

    return KSO_NONE;
}

/* iter_Context.__next__(self) -> none
 *
 * Continue the next frame
 * 
 */
static KS_TFUNC(iter_Context, next) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_iter_Context self = (cNk_iter_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_iter_Context, "self");

    // query the actual size
    int dw, dh;
    glfwGetWindowSize(self->context->window, &dw, &dh);

    // render the entire window
    glViewport(0, 0, dw, dh);

    // set background color
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // use the Nuklear GLFW3 render helper (with our configuration of max memory)
    nk_glfw3_render(NK_ANTI_ALIASING_ON, _NK_max_verbuf, _NK_max_elebuf);

    // swap buffers
    glfwSwapBuffers(self->context->window);

    // stop iterating
    if (glfwWindowShouldClose(self->context->window)) {
        return ks_throw_fmt(ks_type_OutOfIterError, "");
    }

    // update GLFW things for the next frame
    glfwPollEvents();
    nk_glfw3_new_frame();

    // return the frame count
    return (ks_obj)ks_int_new(self->frame_n++);
}




/* misc. functions */

/* Context.__iter__(self) -> none
 *
 * Return an iterator for a Context object
 * 
 */
static KS_TFUNC(Context, iter) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self = (cNk_Context)args[0];
    KS_REQ_TYPE(self, cNk_type_Context, "self");

    // construct an iterator
    cNk_iter_Context res = KS_ALLOC_OBJ(cNk_iter_Context);
    KS_INIT_OBJ(res, cNk_type_iter_Context);

    // initialize everything
    res->frame_n = 0;
    res->context = (cNk_Context)KS_NEWREF(self);

    return (ks_obj)res;
}


// now, export them all
static ks_module get_module() {
    
    /* import & initialize required graphics libraries */

    /* OpenGL */

    // Initialize OpenGL extension wrangler
    gl3wInit();

    // attempt to find a supported Major/Minor version of OpenGL
    int i;
    for (i = 0; _try_GLvers[i][0] > 0; ++i) {
        int major = _try_GLvers[i][0], minor = _try_GLvers[i][1];
        // see if this version is supported
        ks_debug("Trying OpenGL %i.%i", major, minor);
        if (gl3wIsSupported(major, minor) == 0) {
            // set the version to the correct one
            _GLvers[0] = major, _GLvers[1] = minor;
            break;
        }
    }

    // ensure a valid OpenGL has been found
    if (_GLvers[0] < 0) return ks_throw_fmt(ks_type_Error, "Failed to initialize OpenGL");

    /* GLFW */

    glfwSetErrorCallback(_GLFW_errorcb);

    // initialize GLFW
    if (!glfwInit()) {
        return ks_throw_fmt(ks_type_Error, "Failed to initialize GLFW");
    }

    // set the OpenGL version to be used
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _GLvers[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _GLvers[1]);

    // set to core profile only (for maximum compatibility)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    /* kscript code */

    // create the module
    ks_module mod = ks_module_new(MODULE_NAME);

    // create a context wrapper
    KS_INIT_TYPE_OBJ(cNk_type_Context, "cnk.Context");
    KS_INIT_TYPE_OBJ(cNk_type_iter_Context, "cnk.iter_Context");


    // create Context type
    ks_type_set_cn(cNk_type_Context, (ks_dict_ent_c[]){
        {"__new__",       (ks_obj)ks_cfunc_new2(Context_new_, "cnk.Context.__new__()")},
        {"__free__",      (ks_obj)ks_cfunc_new2(Context_free_, "cnk.Context.__free__(self)")},

        {"__getattr__",   (ks_obj)ks_cfunc_new2(Context_getattr_, "cnk.Context.__getattr__(self, attr)")},
        {"__setattr__",   (ks_obj)ks_cfunc_new2(Context_setattr_, "cnk.Context.__setattr__(self, attr, val)")},

        {"__iter__",      (ks_obj)ks_cfunc_new2(Context_iter_, "cnk.Context.__iter__(self)")},

        {"frame_start",   (ks_obj)ks_cfunc_new2(Context_frame_start_, "cnk.Context.frame_start(self)")},
        {"frame_end",     (ks_obj)ks_cfunc_new2(Context_frame_end_, "cnk.Context.frame_end(self)")},

        /* direct C API bindings */

        {"begin",                  (ks_obj)ks_cfunc_new2(Context_begin_, "cnk.Context.begin(self, title, x, y, w, h, flags=cnk.NK_WINDOW_NONE)")},
        {"end",                    (ks_obj)ks_cfunc_new2(Context_end_, "cnk.Context.end(self)")},

        {"layout_row_static",      (ks_obj)ks_cfunc_new2(Context_layout_row_static_, "cnk.Context.layout_row_static(self, height, item_width, cols=1)")},

        {"button",                 (ks_obj)ks_cfunc_new2(Context_button_, "cnk.Context.button(self, label)")},


        {NULL, NULL},
    });

    // create Context type
    ks_type_set_cn(cNk_type_iter_Context, (ks_dict_ent_c[]){
        {"__free__",      (ks_obj)ks_cfunc_new2(iter_Context_free_, "cnk.iter_Context.__free__(self)")},

        {"__next__",         (ks_obj)ks_cfunc_new2(iter_Context_next_, "cnk.iter_Context.__next__(self)")},

        {NULL, NULL},
    });


    // populate the module
    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        {"Context",       (ks_obj)cNk_type_Context},

        {NULL, NULL}
    });


    /* enums */
    ks_type E_Heading = ks_Enum_create_c("Heading", (struct ks_enum_entry_c[]){
        KS_EEF(NK_UP),
        KS_EEF(NK_RIGHT),
        KS_EEF(NK_DOWN),
        KS_EEF(NK_LEFT),
        {NULL, -1}
    });
    ks_module_add_enum_members(mod, E_Heading);

    ks_type E_Symbol = ks_Enum_create_c("Symbol", (struct ks_enum_entry_c[]){
        KS_EEF(NK_SYMBOL_NONE),
        KS_EEF(NK_SYMBOL_X),
        KS_EEF(NK_SYMBOL_UNDERSCORE),
        KS_EEF(NK_SYMBOL_CIRCLE_SOLID),
        KS_EEF(NK_SYMBOL_CIRCLE_OUTLINE),
        KS_EEF(NK_SYMBOL_RECT_SOLID),
        KS_EEF(NK_SYMBOL_RECT_OUTLINE),
        KS_EEF(NK_SYMBOL_TRIANGLE_UP),
        KS_EEF(NK_SYMBOL_TRIANGLE_DOWN),
        KS_EEF(NK_SYMBOL_TRIANGLE_LEFT),
        KS_EEF(NK_SYMBOL_TRIANGLE_RIGHT),
        KS_EEF(NK_SYMBOL_PLUS),
        KS_EEF(NK_SYMBOL_MINUS),
        KS_EEF(NK_SYMBOL_MAX),
        {NULL, -1}
    });
    ks_module_add_enum_members(mod, E_Symbol);

    ks_type E_Window = ks_Enum_create_c("Window", (struct ks_enum_entry_c[]){
        {"NK_WINDOW_NONE", 0},
        KS_EEF(NK_WINDOW_BORDER),
        KS_EEF(NK_WINDOW_MOVABLE),
        KS_EEF(NK_WINDOW_SCALABLE),
        KS_EEF(NK_WINDOW_CLOSABLE),
        KS_EEF(NK_WINDOW_MINIMIZABLE),
        KS_EEF(NK_WINDOW_NO_SCROLLBAR),
        KS_EEF(NK_WINDOW_TITLE),
        KS_EEF(NK_WINDOW_SCROLL_AUTO_HIDE),
        KS_EEF(NK_WINDOW_BACKGROUND),
        KS_EEF(NK_WINDOW_SCALE_LEFT),
        KS_EEF(NK_WINDOW_NO_INPUT),
        {NULL, -1}
    });
    ks_module_add_enum_members(mod, E_Window);

    ks_type E_Key = ks_Enum_create_c("Key", (struct ks_enum_entry_c[]){
        KS_EEF(NK_KEY_NONE),
        KS_EEF(NK_KEY_SHIFT),
        KS_EEF(NK_KEY_CTRL),
        KS_EEF(NK_KEY_DEL),
        KS_EEF(NK_KEY_ENTER),
        KS_EEF(NK_KEY_TAB),
        KS_EEF(NK_KEY_BACKSPACE),
        KS_EEF(NK_KEY_COPY),
        KS_EEF(NK_KEY_CUT),
        KS_EEF(NK_KEY_PASTE),
        KS_EEF(NK_KEY_UP),
        KS_EEF(NK_KEY_DOWN),
        KS_EEF(NK_KEY_LEFT),
        KS_EEF(NK_KEY_RIGHT),

        KS_EEF(NK_KEY_TEXT_INSERT_MODE),
        KS_EEF(NK_KEY_TEXT_REPLACE_MODE),
        KS_EEF(NK_KEY_TEXT_RESET_MODE),
        KS_EEF(NK_KEY_TEXT_LINE_START),
        KS_EEF(NK_KEY_TEXT_LINE_END),
        KS_EEF(NK_KEY_TEXT_START),
        KS_EEF(NK_KEY_TEXT_END),
        KS_EEF(NK_KEY_TEXT_UNDO),
        KS_EEF(NK_KEY_TEXT_REDO),
        KS_EEF(NK_KEY_TEXT_SELECT_ALL),
        KS_EEF(NK_KEY_TEXT_WORD_LEFT),
        KS_EEF(NK_KEY_TEXT_WORD_RIGHT),

        KS_EEF(NK_KEY_SCROLL_START),
        KS_EEF(NK_KEY_SCROLL_END),
        KS_EEF(NK_KEY_SCROLL_DOWN),
        KS_EEF(NK_KEY_SCROLL_UP),
        {NULL, -1}
    });
    ks_module_add_enum_members(mod, E_Key);

    ks_type E_Button = ks_Enum_create_c("Button", (struct ks_enum_entry_c[]){
        KS_EEF(NK_BUTTON_LEFT),
        KS_EEF(NK_BUTTON_MIDDLE),
        KS_EEF(NK_BUTTON_RIGHT),
        KS_EEF(NK_BUTTON_DOUBLE),
        KS_EEF(NK_BUTTON_MAX),

        {NULL, -1}

    });
    ks_module_add_enum_members(mod, E_Button);



/*
    win = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Demo", NULL, NULL);
    glfwMakeContextCurrent(win);
    glfwGetWindowSize(win, &width, &height);



    bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
    while (!glfwWindowShouldClose(win))
    {
        glfwPollEvents();
        nk_glfw3_new_frame();

        if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
            NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE))
        {
            enum {EASY, HARD};
            static int op = EASY;
            static int property = 20;
            nk_layout_row_static(ctx, 30, 80, 1);
            if (nk_button_label(ctx, "button"))
                fprintf(stdout, "button pressed\n");

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_option_label(ctx, "easy", op == EASY)) op = EASY;
            if (nk_option_label(ctx, "hard", op == HARD)) op = HARD;

            nk_layout_row_dynamic(ctx, 25, 1);
            nk_property_int(ctx, "Compression:", 0, &property, 100, 10, 1);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "background:", NK_TEXT_LEFT);
            nk_layout_row_dynamic(ctx, 25, 1);
            if (nk_combo_begin_color(ctx, nk_rgb_cf(bg), nk_vec2(nk_widget_width(ctx),400))) {
                nk_layout_row_dynamic(ctx, 120, 1);
                bg = nk_color_picker(ctx, bg, NK_RGBA);
                nk_layout_row_dynamic(ctx, 25, 1);
                bg.r = nk_propertyf(ctx, "#R:", 0, bg.r, 1.0f, 0.01f,0.005f);
                bg.g = nk_propertyf(ctx, "#G:", 0, bg.g, 1.0f, 0.01f,0.005f);
                bg.b = nk_propertyf(ctx, "#B:", 0, bg.b, 1.0f, 0.01f,0.005f);
                bg.a = nk_propertyf(ctx, "#A:", 0, bg.a, 1.0f, 0.01f,0.005f);
                nk_combo_end(ctx);
            }
        }
        nk_end(ctx);

        #ifdef INCLUDE_CALCULATOR
          calculator(ctx);
        #endif
        #ifdef INCLUDE_OVERVIEW
          overview(ctx);
        #endif
        #ifdef INCLUDE_NODE_EDITOR
          node_editor(ctx);
        #endif

        glfwGetWindowSize(win, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg.r, bg.g, bg.b, bg.a);
        nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);
        glfwSwapBuffers(win);
    }
    nk_glfw3_shutdown();
    glfwTerminate();
    */


    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
