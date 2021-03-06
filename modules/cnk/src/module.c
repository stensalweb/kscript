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

// define which backend to use
//#define CNK_USE_GLFW
#define CNK_USE_XLIB


// define per-implementation code
#if defined(CNK_USE_XLIB)

// misc options
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

// implement using XLIB
#define NK_XLIB_GL3_IMPLEMENTATION
#define NK_XLIB_LOAD_OPENGL_EXTENSIONS

// actually place code in this file
#define NK_IMPLEMENTATION

// include Nuklear headers
#include "./ext/nuklear.h"
#include "./ext/nuklear_xlib_gl3.h"

#elif defined(CNK_USE_GLFW)

// include OpenGL/GLFW libraries
#include "./ext/gl3w_gl3.h"
#include <GLFW/glfw3.h>

// misc options
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_KEYSTATE_BASED_INPUT

// use GLFW with OpenGL 3 implementation
#define NK_GLFW_GL3_IMPLEMENTATION

// actually define the implementation in this file
#define NK_IMPLEMENTATION

// include Nuklear headers
#include "./ext/nuklear.h"
#include "./ext/nuklear_glfw_gl3.h"


#else
#error CNK_USE_* must be defined!
#endif



/* generic OpenGL configuration */



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





/* WRAPPER TYPES */

// cNk_Context - class wrapping the nk_context object, as well as a window
typedef struct {
    KS_OBJ_BASE

    #if defined(CNK_USE_XLIB)

    // X11-specific variables
    struct {
        Display* display;

        Window window;

        // OpenGL context
        GLXContext glCTX;


        XVisualInfo *vis;
        Colormap cmap;
        XSetWindowAttributes swa;
        XWindowAttributes attr;
        GLXFBConfig fbc;
        Atom wm_delete_window;
        int width, height;

    } x;

    #elif defined(CNK_USE_GLFW)

    // the GLFW window the context is bound to
    GLFWwindow* window;

    #endif

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



/* helper functions */

#if defined(CNK_USE_XLIB)

// X error call back function
static int _X_errorcb(Display* display, XErrorEvent* ev) {
    char tmp[256];
    XGetErrorText(display, ev->error_code, tmp, sizeof(tmp) - 1);
    ks_warn("ks", "[X11]: %s\n", tmp);
    return 0;
}

// return whether or not an extensions string has a given extension in it
static bool _X_hasext(const char *string, const char *ext) {
    const char *start, *where, *term;
    where = strchr(ext, ' ');
    if (where || *ext == '\0')
        return false;

    for (start = string;;) {
        where = strstr((const char*)start, ext);
        if (!where) break;
        term = where + strlen(ext);
        if (where == start || *(where - 1) == ' ') {
            if (*term == ' ' || *term == '\0')
                return true;
        }
        start = term;
    }

    return false;
}

#elif defined(CNK_USE_GLFW)

// error callback to be used by GLFW
static void _GLFW_errorcb(int er, const char* d) {
    ks_warn("ks", "GLFW: %s [code: %i]", d, er);
}

// key function call back from GLFW
static void _GLFW_keycb(GLFWwindow *win, unsigned int codepoint) {
    cNk_Context ctx = (cNk_Context)glfwGetWindowUserPointer(win);
    nk_input_unicode(ctx->ctx, codepoint);
}

#endif


/* Context class */

/* Context.__new__(width, height, title) -> Context
 *
 * Construct a new Nuklear context object
 * 
 */
static KS_TFUNC(Context, new) {
    KS_REQ_N_ARGS(n_args, 3);
    int64_t width, height;
    ks_str title;
    if (!ks_parse_params(n_args, args, "width%i64 height%i64 title%s", &width, &height, &title)) return NULL;

    cNk_Context self = KS_ALLOC_OBJ(cNk_Context);
    KS_INIT_OBJ(self, cNk_type_Context);


    self->background_color[0] = self->background_color[1] = self->background_color[2] = 0.1f;


    #if defined(CNK_USE_XLIB)

    if (!(self->x.display = XOpenDisplay(NULL))) {
        return ks_throw(ks_type_InternalError, "Failed to open X display");
    }

    int glx_major, glx_minor;

    if (!glXQueryVersion(self->x.display, &glx_major, &glx_minor)) {
        return ks_throw(ks_type_InternalError, "Failed to query OpenGL version");
    }

    ks_debug("ks", "[X11]: Queried OpenGL version %i.%i\n", glx_major, glx_minor);

    /* find and pick matching framebuffer visual */
    int fb_count;
    static GLint attr[] = {
        GLX_X_RENDERABLE,   True,
        GLX_DRAWABLE_TYPE,  GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,    GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
        GLX_RED_SIZE,       8,
        GLX_GREEN_SIZE,     8,
        GLX_BLUE_SIZE,      8,
        GLX_ALPHA_SIZE,     8,
        GLX_DEPTH_SIZE,     24,
        GLX_STENCIL_SIZE,   8,
        GLX_DOUBLEBUFFER,   True,
        None
    };

    // Get framebuffer configuration
    GLXFBConfig *fbc;
    fbc = glXChooseFBConfig(self->x.display, DefaultScreen(self->x.display), attr, &fb_count);
    if (!fbc) {
        return ks_throw(ks_type_InternalError, "Failed to retrieve framebuffer config from X");
    }

    // pick the configuration with the most samples/pixel
    int i;
    int fb_best = -1, best_num_samples = -1;
    for (i = 0; i < fb_count; ++i) {
        XVisualInfo *vi = glXGetVisualFromFBConfig(self->x.display, fbc[i]);
        if (vi) {
            int sample_buffer, samples;
            glXGetFBConfigAttrib(self->x.display, fbc[i], GLX_SAMPLE_BUFFERS, &sample_buffer);
            glXGetFBConfigAttrib(self->x.display, fbc[i], GLX_SAMPLES, &samples);
            if ((fb_best < 0) || (sample_buffer && samples > best_num_samples))
                fb_best = i, best_num_samples = samples;
        }
    }
    self->x.fbc = fbc[fb_best];
    XFree(fbc);

    // get visual
    self->x.vis = glXGetVisualFromFBConfig(self->x.display, self->x.fbc);

    // now, create a colormap, etc in order to create a window
    self->x.cmap = XCreateColormap(self->x.display, RootWindow(self->x.display, self->x.vis->screen), self->x.vis->visual, AllocNone);
    self->x.swa.colormap =  self->x.cmap;
    self->x.swa.background_pixmap = None;
    self->x.swa.border_pixel = 0;
    self->x.swa.event_mask =
        ExposureMask | KeyPressMask | KeyReleaseMask |
        ButtonPress | ButtonReleaseMask| ButtonMotionMask |
        Button1MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask|
        PointerMotionMask| StructureNotifyMask;
    
    // create window
    self->x.window = XCreateWindow(self->x.display, RootWindow(self->x.display, self->x.vis->screen), 0, 0,
        width, height, 0, self->x.vis->depth, InputOutput,
        self->x.vis->visual, CWBorderPixel|CWColormap|CWEventMask, &self->x.swa
    );

    if (!self->x.window) {
        return ks_throw(ks_type_InternalError, "Failed to create window in X");
    }
    
    XFree(self->x.vis);
    // set title
    XStoreName(self->x.display, self->x.window, title->chr);
    XMapWindow(self->x.display, self->x.window);
    self->x.wm_delete_window = XInternAtom(self->x.display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(self->x.display, self->x.window, &self->x.wm_delete_window, 1);

    /* OpenGL context configuration */


    // error handler
    int (*old_handler)(Display*, XErrorEvent*) = XSetErrorHandler(_X_errorcb);

    // Create context FP
    typedef GLXContext (*glxCreateContext)(Display*, GLXFBConfig, GLXContext, Bool, const int*);

    // load extensions for OpenGL
    const char *extensions_str = glXQueryExtensionsString(self->x.display, DefaultScreen(self->x.display));

    // Attempt to get the context
    glxCreateContext create_context = (glxCreateContext)glXGetProcAddressARB((const GLubyte*)"glXCreateContextAttribsARB");

    // whether or not we had an error
    bool hadErr = false;

    if (!_X_hasext(extensions_str, "GLX_ARB_create_context") || !create_context) {
        ks_info("ks", "[X11]: glxCreateContextAttribARB() was not found, so using old style GLX context");
        self->x.glCTX = glXCreateNewContext(self->x.display, self->x.fbc, GLX_RGBA_TYPE, 0, True);
    } else {
        GLint attr[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB, 3,
            GLX_CONTEXT_MINOR_VERSION_ARB, 0,
            None
        };
        self->x.glCTX = create_context(self->x.display, self->x.fbc, 0, True, attr);
        XSync(self->x.display, False);
        if (hadErr || !self->x.glCTX) {
            /* Could not create GL 3.0 context. Fallback to old 2.x context.
                * If a version below 3.0 is requested, implementations will
                * return the newest context version compatible with OpenGL
                * version less than version 3.0.*/
            attr[1] = 1; attr[3] = 0;
            hadErr = false;
            ks_info("ks", "[X11]: Failed to create OpenGL 3.0 context, using old style GLX context");
            self->x.glCTX = create_context(self->x.display, self->x.fbc, 0, True, attr);
        }
    }


    XSync(self->x.display, False);
    XSetErrorHandler(old_handler);
    if (hadErr || !self->x.glCTX) {
        return ks_throw(ks_type_InternalError, "Failed to create an OpenGL context in X");
    }

    // make it the current context
    glXMakeCurrent(self->x.display, self->x.window, self->x.glCTX);

    // now, finally, initialize Nuklear
    self->ctx = nk_x11_init(self->x.display, self->x.window);

    /* load font assets */
    nk_x11_font_stash_begin(&self->atlas);

    /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas, "../../../extra_font/DroidSans.ttf", 14, 0);*/
    /*struct nk_font *roboto = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Roboto-Regular.ttf", 14, 0);*/
    /*struct nk_font *future = nk_font_atlas_add_from_file(atlas, "../../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
    /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyClean.ttf", 12, 0);*/
    /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas, "../../../extra_font/ProggyTiny.ttf", 10, 0);*/
    /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas, "../../../extra_font/Cousine-Regular.ttf", 13, 0);*/

    nk_x11_font_stash_end();


    #elif defined(CNK_USE_GLFW)

    // construct a new GLFW window with the given parameters
    self->window = glfwCreateWindow(width, height, title->chr, NULL, NULL);

    glfwMakeContextCurrent(self->window);
    glfwSetCharCallback(self->window, _GLFW_keycb);

    // set user data to the context object
    glfwSetWindowUserPointer(self->window, self);

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

    #endif


    /* load cursor assets */

    // nk_style_load_all_cursors(ctx, atlas->cursors);


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


    #if defined(CNK_USE_XLIB)

    #elif defined(CNK_USE_GLFW)

    // destroy the window the context was created around
    glfwDestroyWindow(self->window);

    #endif

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
        
        #if defined(CNK_USE_XLIB)
        XGetWindowAttributes(self->x.display, self->x.window, &self->x.attr);
        ww = self->x.attr.width;
        ww = self->x.attr.height;
        #elif defined(CNK_USE_GLFW)

        glfwGetWindowSize(self->window, &ww, &wh);
        #endif

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
            return ks_throw(ks_type_ArgError, "Attribute 'size' must be an iterable of size 2, containing the integers '(width, height)'");
        }


        // window width & height
        int64_t sizes[2];

        int i;
        for (i = 0; i < 2; ++i) {
            if (!ks_num_get_int64(val_list->elems[i], &sizes[i])) {
                KS_DECREF(val_list);
                return ks_throw(ks_type_ArgError, "Attribute 'size' must be an iterable of size 2, containing the integers '(width, height)'");
            }
        }

        KS_DECREF(val_list);

        // set the size
                
        #if defined(CNK_USE_XLIB)

        #elif defined(CNK_USE_GLFW)
        glfwSetWindowSize(self->window, sizes[0], sizes[1]);
        #endif
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

    bool rst = true;

    // update GLFW things
    #if defined(CNK_USE_XLIB)

    // TODO: seperate events maybe?
    XEvent evt;
    nk_input_begin(self->ctx);
    while (XPending(self->x.display)) {
        XNextEvent(self->x.display, &evt);
        if (evt.type == ClientMessage) rst = false;
        if (XFilterEvent(&evt, self->x.window)) continue;
        nk_x11_handle_event(&evt);
    }

    nk_input_end(self->ctx);

    #elif defined(CNK_USE_GLFW)

    glfwPollEvents();
    nk_glfw3_new_frame();
    if (glfwWindowShouldClose(self->window)) rst = false;
    
    #endif

    // return whether it should keep going
    return KSO_BOOL(rst);
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

    bool rst = true;

    // query the actual size
    int dw, dh;
    #if defined(CNK_USE_XLIB)
    XGetWindowAttributes(self->x.display, self->x.window, &self->x.attr);
    dw = self->x.attr.width;
    dh = self->x.attr.height;
    #elif defined(CNK_USE_GLFW)
    glfwGetWindowSize(self->window, &dw, &dh);
    #endif


    /* shared OpenGL code */

    // render the entire window
    glViewport(0, 0, dw, dh);

    // set background color
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(self->background_color[0], self->background_color[1], self->background_color[2], self->background_color[3]);


    #if defined(CNK_USE_XLIB)

    nk_x11_render(NK_ANTI_ALIASING_ON, _NK_max_verbuf, _NK_max_elebuf);
    glXSwapBuffers(self->x.display, self->x.window);


    #elif defined(CNK_USE_GLFW)
    // use the Nuklear GLFW3 render helper (with our configuration of max memory)
    nk_glfw3_render(NK_ANTI_ALIASING_ON, _NK_max_verbuf, _NK_max_elebuf);

    // swap buffers
    glfwSwapBuffers(self->window);

    if (glfwWindowShouldClose(self->window)) rst = false;
    #endif

    // return whether it should keep going
    return KSO_BOOL(rst);
}


/* Context.begin(self, title, x, y, w, h, flags=cnk.NK_WINDOW_NONE) -> bool
 *
 * Construct a window with a given title, at a given position. Return true if it is visible
 * 
 */
static KS_TFUNC(Context, begin) {
    KS_REQ_N_ARGS_RANGE(n_args, 6, 7);
    cNk_Context self;
    ks_str title;
    double x, y, w, h;
    int64_t flags = 0;
    if (!ks_parse_params(n_args, args, "self%* title%s x%f y%f w%f h%f ?flags%i64", &self, cNk_type_Context, &title, &x, &y, &w, &h, &flags)) return NULL;

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
    cNk_Context self;
    if (!ks_parse_params(n_args, args, "self%*", &self, cNk_type_Context)) return NULL;

    nk_end(self->ctx);
   
    return KSO_NONE;
}


/***  LAYOUT FUNCTIONS  ***/


/* Context.layout_set_min_row_height(self, height) -> none
 *
 * Set the minimum row height
 */
static KS_TFUNC(Context, layout_set_min_row_height) {
    KS_REQ_N_ARGS(n_args, 2);
    cNk_Context self;
    double height;
    if (!ks_parse_params(n_args, args, "self%* height%f", &self, cNk_type_Context, &height)) return NULL;

    // call internal library function
    nk_layout_set_min_row_height(self->ctx, height);

    return KSO_NONE;
}

/* Context.layout_reset_min_row_height(self) -> none
 *
 * Reset the minimum row height to its default calculated value (font_height + text_padding + padding)
 */
static KS_TFUNC(Context, layout_reset_min_row_height) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self;
    if (!ks_parse_params(n_args, args, "self%*", &self, cNk_type_Context)) return NULL;

    // call internal library function
    nk_layout_reset_min_row_height(self->ctx);

    return KSO_NONE;
}

/* Context.layout_widget_bounds(self) -> (x, y, w, h)
 *
 * Return the bounding rectangle (as a tuple) of the next row
 */
static KS_TFUNC(Context, layout_widget_bounds) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self;
    if (!ks_parse_params(n_args, args, "self%*", &self, cNk_type_Context)) return NULL;

    // call internal library function
    struct nk_rect res = nk_layout_widget_bounds(self->ctx);

    // construct tuple
    return (ks_obj)ks_build_tuple("%f %f %f %f", res.x, res.y, res.w, res.h);
}


/* Context.layout_ratio_from_pixel(self, pixel_width) -> ratio
 *
 * Return the window aspect ratio from a given pixel width
 */
static KS_TFUNC(Context, layout_ratio_from_pixel) {
    KS_REQ_N_ARGS(n_args, 2);
    cNk_Context self;
    double pixel_width;
    if (!ks_parse_params(n_args, args, "self%* pixel_width%f", &self, cNk_type_Context, &pixel_width)) return NULL;

    // call internal library function
    double res = nk_layout_ratio_from_pixel(self->ctx, pixel_width);

    return (ks_obj)ks_float_new(res);
}


/* Context.layout_row_dynamic(self, height, cols=1) -> none
 *
 * Create a dynamic row, given a height and a number of columns
 */
static KS_TFUNC(Context, layout_row_dynamic) {
    KS_REQ_N_ARGS_RANGE(n_args, 2, 3);
    cNk_Context self;
    double height;
    int64_t cols = 1;
    if (!ks_parse_params(n_args, args, "self%* height%f ?cols%i64", &self, cNk_type_Context, &height, &cols)) return NULL;

    // call internal library function
    nk_layout_row_dynamic(self->ctx, height, cols);

    return KSO_NONE;
}


/* Context.layout_row_static(self, height, item_width, cols=1) -> none
 *
 * Create a row with a static item width
 */
static KS_TFUNC(Context, layout_row_static) {
    KS_REQ_N_ARGS_RANGE(n_args, 3, 4);
    cNk_Context self;
    double height;
    int64_t item_width, cols = 1;
    if (!ks_parse_params(n_args, args, "self%* height%f item_width%i64 ?cols%i64", &self, cNk_type_Context, &height, &item_width, &cols)) return NULL;

    // call internal library function
    nk_layout_row_static(self->ctx, height, item_width, cols);

    return KSO_NONE;
}

/* Context.layout_row_begin(self, fmt, row_height, cols=1) -> none
 *
 * Create a layout row with a given format & row height & coumns
 */
static KS_TFUNC(Context, layout_row_begin) {
    KS_REQ_N_ARGS_RANGE(n_args, 3, 4);
    cNk_Context self;
    int64_t fmt, cols = 1;
    double row_height;
    if (!ks_parse_params(n_args, args, "self%* fmt%i64 row_height%f ?cols%i64", &self, cNk_type_Context, &fmt, &row_height, &cols)) return NULL;

    // call internal library function
    nk_layout_row_begin(self->ctx, fmt, row_height, cols);

    return KSO_NONE;
}

/* Context.layout_row_push(self, value) -> none
 *
 * Set either the window ratio, or fixed width depending on `fmt` in `layout_row_begin()` call
 */
static KS_TFUNC(Context, layout_row_push) {
    KS_REQ_N_ARGS(n_args, 2);
    cNk_Context self;
    double value;
    if (!ks_parse_params(n_args, args, "self%* value%f", &self, cNk_type_Context, &value)) return NULL;

    // call internal library function
    nk_layout_row_push(self->ctx, value);

    return KSO_NONE;
}

/* Context.layout_row_end(self) -> none
 *
 * End the layout row started with `layout_row_begin()`
 */
static KS_TFUNC(Context, layout_row_end) {
    KS_REQ_N_ARGS(n_args, 1);
    cNk_Context self;
    if (!ks_parse_params(n_args, args, "self%*", &self, cNk_type_Context)) return NULL;

    // call internal library function
    nk_layout_row_end(self->ctx);

    return KSO_NONE;
}


/* Context.layout_row(self, fmt, height, col_sizes) -> none
 *
 * Specifies row columns sizes or ratio (depending on enums) from the tuple `ratio`
 *   'fmt': either NK_DYNAMIC or NK_STATIC
 * 
 */
static KS_TFUNC(Context, layout_row) {
    KS_REQ_N_ARGS(n_args, 4);
    cNk_Context self;
    int64_t fmt;
    double height;
    ks_obj col_sizes;
    if (!ks_parse_params(n_args, args, "self%* fmt%i64 height%f col_sizes%iter", &self, cNk_type_Context, &fmt, &height, &col_sizes)) return NULL;

    ks_list col_sizes_l = ks_list_from_iterable(col_sizes);
    if (!col_sizes_l) return NULL;


    // convert to C floats
    float* ratio_arr = ks_malloc(sizeof(*ratio_arr) * col_sizes_l->len);
    int i;
    for (i = 0; i < col_sizes_l->len; ++i) {

        double cf;
        if (!ks_num_get_double(col_sizes_l->elems[i], &cf)) {
            KS_DECREF(col_sizes_l);
            ks_free(ratio_arr);
            return NULL;
        }

        ratio_arr[i] = cf;
    }

    // call internal library function
    nk_layout_row(self->ctx, fmt, height, col_sizes_l->len, ratio_arr);

    // free tmp resources
    ks_free(ratio_arr);
    KS_DECREF(col_sizes_l);

    return KSO_NONE;
}





/* Context.edit_string(self, edit_type, cur_str, max_len) -> new_str
 *
 * Edits a string (returns new string)
 * 
 */
static KS_TFUNC(Context, edit_string) {
    KS_REQ_N_ARGS(n_args, 4);
    cNk_Context self;
    int64_t edit_type, max_len;
    ks_str cur_str;
    if (!ks_parse_params(n_args, args, "self%* edit_type%i64 cur_str%s max_len%i64", &self, cNk_type_Context, &edit_type, &cur_str, &max_len)) return NULL;

    // create a modifiable buffer
    char* strbuf = ks_malloc(max_len + 1);
    int strbuf_len = cur_str->len + 1;
    memcpy(strbuf, cur_str->chr, cur_str->len);
    strbuf[cur_str->len] = '\0';

    // call internal library function
    int64_t new_flags = nk_edit_string_zero_terminated(self->ctx, edit_type, strbuf, max_len, nk_filter_ascii);

    // construct output string
    ks_str res = ks_str_new(strbuf);

    // free tmp resources
    ks_free(strbuf);

    return (ks_obj)res;
}




/* Context.button_label(self, label) -> bool
 *
 * Create a button, returning true if pressed
 * 
 */
static KS_TFUNC(Context, button_label) {
    KS_REQ_N_ARGS(n_args, 2);
    cNk_Context self;
    ks_str label;
    if (!ks_parse_params(n_args, args, "self%* label%s", &self, cNk_type_Context, &label)) return NULL;

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

    int truthy;
    bool keepGoing;
    ks_obj r;

    if (self->frame_n > 0) {
        r = Context_frame_end_(1, (ks_obj*)&self->context);
        if (!r) return NULL;

        truthy = ks_truthy(r);
        KS_DECREF(r);
        if (truthy < 0) return NULL;

        // tell whether we should keep going
        keepGoing = truthy != 0;
        if (!keepGoing) return ks_throw(ks_type_OutOfIterError, "");
    }


    r = Context_frame_start_(1, (ks_obj*)&self->context);
    if (!r) return NULL;

    truthy = ks_truthy(r);
    KS_DECREF(r);
    if (truthy < 0) return NULL;

    // tell whether we should keep going
    keepGoing = truthy != 0;
    if (!keepGoing) return ks_throw(ks_type_OutOfIterError, "");

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


    #if defined(CNK_USE_XLIB)


    #elif defined(CNK_USE_GLFW)

    ks_debug("ks", "Calling gl3wInit()...");

    int stat;
    // Initialize OpenGL extension wrangler
    if (stat = gl3wInit()) {
        if (stat == GL3W_ERROR_LIBRARY_OPEN) {
            return ks_throw(ks_type_InternalError, "Could not initialize gl3w! (gl3wInit() failed!, reason: GL3W_ERROR_LIBRARY_OPEN)");
        } else {
            //return ks_throw(ks_type_InternalError, "Could not initialize gl3w! (gl3wInit() failed!)");
        }
    }

    // attempt to find a supported Major/Minor version of OpenGL
    int i;
    for (i = 0; _try_GLvers[i][0] > 0; ++i) {
        int major = _try_GLvers[i][0], minor = _try_GLvers[i][1];
        // see if this version is supported
        ks_debug("ks", "Trying OpenGL %i.%i", major, minor);
        if (gl3wIsSupported(major, minor) == 0) {
            // set the version to the correct one
            _GLvers[0] = major, _GLvers[1] = minor;
            break;
        }
    }

    // ensure a valid OpenGL has been found
    if (_GLvers[0] < 0) return ks_throw(ks_type_Error, "Failed to initialize OpenGL");

    /* GLFW */

    glfwSetErrorCallback(_GLFW_errorcb);
    ks_debug("ks", "Calling glfwInit()...");

    // initialize GLFW
    if (!glfwInit()) {
        return ks_throw(ks_type_Error, "Failed to initialize GLFW");
    }
    ks_debug("ks", "Setting GLFW Window Hints......");

    // set the OpenGL version to be used
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _GLvers[0]);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _GLvers[1]);

    // set to core profile only (for maximum compatibility)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    #endif

    /* kscript code */

    // create the module
    ks_module mod = ks_module_new(MODULE_NAME);

    // create a context wrapper
    KS_INIT_TYPE_OBJ(cNk_type_Context, "Context");
    KS_INIT_TYPE_OBJ(cNk_type_iter_Context, "iter_Context");


    // create Context type
    if (!ks_type_set_cn(cNk_type_Context, (ks_dict_ent_c[]){
        {"__new__",       (ks_obj)ks_cfunc_new_c_old(Context_new_, "Context.__new__()")},
        {"__free__",      (ks_obj)ks_cfunc_new_c_old(Context_free_, "Context.__free__(self)")},

        {"__getattr__",   (ks_obj)ks_cfunc_new_c_old(Context_getattr_, "Context.__getattr__(self, attr)")},
        {"__setattr__",   (ks_obj)ks_cfunc_new_c_old(Context_setattr_, "Context.__setattr__(self, attr, val)")},

        {"__iter__",      (ks_obj)ks_cfunc_new_c_old(Context_iter_, "Context.__iter__(self)")},

        {"frame_start",   (ks_obj)ks_cfunc_new_c_old(Context_frame_start_, "Context.frame_start(self)")},
        {"frame_end",     (ks_obj)ks_cfunc_new_c_old(Context_frame_end_, "Context.frame_end(self)")},

        /* direct C API bindings */

        {"begin",                    (ks_obj)ks_cfunc_new_c_old(Context_begin_, "Context.begin(self, title, x, y, w, h, flags=cnk.NK_WINDOW_NONE)")},
        {"end",                      (ks_obj)ks_cfunc_new_c_old(Context_end_, "Context.end(self)")},


        {"layout_set_min_height",    (ks_obj)ks_cfunc_new_c_old(Context_layout_set_min_row_height_,   "Context.layout_set_min_row_height(self, height)")},
        {"layout_reset_min_height",  (ks_obj)ks_cfunc_new_c_old(Context_layout_reset_min_row_height_, "Context.layout_reset_min_row_height(self)")},

        {"layout_widget_bounds",     (ks_obj)ks_cfunc_new_c_old(Context_layout_widget_bounds_,        "Context.layout_widget_bounds(self)")},

        {"layout_ratio_from_pixel",  (ks_obj)ks_cfunc_new_c_old(Context_layout_ratio_from_pixel_,     "Context.layout_ratio_from_pixel(self, pixel_width)")},

        {"layout_row_dynamic",       (ks_obj)ks_cfunc_new_c_old(Context_layout_row_dynamic_,          "Context.layout_row_dynamic(self, height, cols=1)")},
        {"layout_row_static",        (ks_obj)ks_cfunc_new_c_old(Context_layout_row_static_,           "Context.layout_row_static(self, height, item_width, cols=1)")},

        {"edit_string",              (ks_obj)ks_cfunc_new_c_old(Context_edit_string_,        "Context.edit_string(self, edit_type, cur_str, max_len)")},

        {"button_label",                 (ks_obj)ks_cfunc_new_c_old(Context_button_label_, "Context.button_label(self, label)")},


        {NULL, NULL},
    })) {
        KS_DECREF(mod);
        KS_DECREF(cNk_type_Context);
        KS_DECREF(cNk_type_iter_Context);
        return NULL;
    }

    // create Context type
    if (!ks_type_set_cn(cNk_type_iter_Context, (ks_dict_ent_c[]){
        {"__free__",      (ks_obj)ks_cfunc_new_c_old(iter_Context_free_, "iter_Context.__free__(self)")},

        {"__next__",         (ks_obj)ks_cfunc_new_c_old(iter_Context_next_, "iter_Context.__next__(self)")},

        {NULL, NULL},
    })) {
        KS_DECREF(mod);
        KS_DECREF(cNk_type_Context);
        KS_DECREF(cNk_type_iter_Context);
        return NULL;
    }


    // populate the module
    if (!ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        {"Context",       (ks_obj)cNk_type_Context},

        {NULL, NULL}
    })) {
        KS_DECREF(mod);
        KS_DECREF(cNk_type_Context);
        KS_DECREF(cNk_type_iter_Context);
        return NULL;
    }


    /* enums */
    ks_type E_Heading = ks_Enum_create_c("Heading", (ks_enum_entry_c[]){
        KS_EEF(NK_UP),
        KS_EEF(NK_RIGHT),
        KS_EEF(NK_DOWN),
        KS_EEF(NK_LEFT),
        {NULL, -1}
    });
    ks_module_add_enum_members(mod, E_Heading);

    ks_type E_Symbol = ks_Enum_create_c("Symbol", (ks_enum_entry_c[]){
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

    ks_type E_Window = ks_Enum_create_c("Window", (ks_enum_entry_c[]){
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

    ks_type E_Key = ks_Enum_create_c("Key", (ks_enum_entry_c[]){
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

    ks_type E_Button = ks_Enum_create_c("Button", (ks_enum_entry_c[]){
        KS_EEF(NK_BUTTON_LEFT),
        KS_EEF(NK_BUTTON_MIDDLE),
        KS_EEF(NK_BUTTON_RIGHT),
        KS_EEF(NK_BUTTON_DOUBLE),
        KS_EEF(NK_BUTTON_MAX),

        {NULL, -1}

    });
    ks_module_add_enum_members(mod, E_Button);

    ks_type E_Edit  = ks_Enum_create_c("Edit", (ks_enum_entry_c[]){
        KS_EEF(NK_EDIT_DEFAULT),
        KS_EEF(NK_EDIT_READ_ONLY),
        KS_EEF(NK_EDIT_AUTO_SELECT),
        KS_EEF(NK_EDIT_SIG_ENTER),
        KS_EEF(NK_EDIT_ALLOW_TAB),
        KS_EEF(NK_EDIT_NO_CURSOR),
        KS_EEF(NK_EDIT_SELECTABLE),
        KS_EEF(NK_EDIT_CLIPBOARD),
        KS_EEF(NK_EDIT_CTRL_ENTER_NEWLINE),
        KS_EEF(NK_EDIT_NO_HORIZONTAL_SCROLL),
        KS_EEF(NK_EDIT_ALWAYS_INSERT_MODE),
        KS_EEF(NK_EDIT_MULTILINE),
        KS_EEF(NK_EDIT_GOTO_END_ON_ACTIVATE),

        // combinations

        KS_EEF(NK_EDIT_SIMPLE),
        KS_EEF(NK_EDIT_FIELD),
        KS_EEF(NK_EDIT_BOX),
        KS_EEF(NK_EDIT_EDITOR),

        {NULL, -1}
    });
    ks_module_add_enum_members(mod, E_Edit);

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

    ks_debug("ks", "Returning cnk module");

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
