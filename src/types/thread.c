/* types/thread.c - implementation of 'ks_thread', for executing concurrently
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_thread);
KS_TYPE_DECLFWD(ks_type_stack_frame);

/** MISC THREAD TYPES **/

// stack frame: used in thread's implementation

// create a stack frame
ks_stack_frame ks_stack_frame_new(ks_obj func) {
    ks_stack_frame self = KS_ALLOC_OBJ(ks_stack_frame);
    KS_INIT_OBJ(self, ks_type_stack_frame);

    self->func = KS_NEWREF(func);

    return self;
}


/* member functions */

// stack_frame.__free__(self) -> free a stack frame object
static KS_TFUNC(stack_frame, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_stack_frame self = (ks_stack_frame)args[0];
    KS_REQ_TYPE(self, ks_type_stack_frame, "self");

    KS_DECREF(self->func);


    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};

// stack_frame.__str__(self) -> convert to a string
static KS_TFUNC(stack_frame, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_stack_frame self = (ks_stack_frame)args[0];
    KS_REQ_TYPE(self, ks_type_stack_frame, "self");

    if (self->func->type == ks_type_code) {
        // attempt to search for it

        ks_code code_obj = (ks_code)self->func;

        // get current offset into btecode
        int offset = (int)(self->pc - code_obj->bc);

        int fi = -1, i;
        for (i = 0; i < code_obj->meta_n; ++i) {
            if (offset <= code_obj->meta[i].bc_n) {
                fi = i;
                break;
            }
        }
        if (fi >= 0) {
            // set information
            //ks_list_popu(call_stk);
            ks_str o_str = ks_tok_expstr(code_obj->meta[fi].tok);
            ks_str new_str = ks_fmt_c("%S: %S\n", code_obj->name_hr, o_str);
            KS_DECREF(o_str);

            return (ks_obj)new_str;
        }
    }


    return (ks_obj)ks_fmt_c("<'stack_frame' type(func): %T obj @ %p>", self->func, self);
};



/* ACTUAL THREAD TYPE */

// the main thread, i.e. global thread
static ks_thread main_thread = NULL;

// thread-local variable for the current thread
pthread_key_t this_thread_key;

// initialize a thread
static void* thread_init(void* _self) {
    ks_thread self = (ks_thread)_self;
    ks_debug("thread <%p> initializing!", self);

    // set the global variable
    pthread_setspecific(this_thread_key, (void*)self);

    return NULL;
}


// create a new thread
ks_thread ks_thread_new(char* name) {
    ks_thread self = KS_ALLOC_OBJ(ks_thread);
    KS_INIT_OBJ(self, ks_type_thread);

    pthread_mutex_init(&self->_mut, NULL);
    pthread_mutex_lock(&self->_mut);

    if (name) {
        self->name = ks_str_new(name);
    } else {
        self->name = ks_fmt_c("__thread__%p", self);
    }

    self->stk = ks_list_new(0, NULL);
    self->stack_frames = ks_list_new(0, NULL);

    self->exc = NULL;
    self->exc_info = NULL;

    // set the pthread
    pthread_create(&self->_pth, NULL, thread_init, self);

    // done with initialization
    pthread_mutex_unlock(&self->_mut);

    return self;
}

// return the current thread
// NOTE: Does *NOT* return a new reference to the thread
ks_thread ks_thread_cur() {
    // attempt to return it
    ks_thread this_thread = (ks_thread)pthread_getspecific(this_thread_key);
    if (!this_thread) this_thread = main_thread;
    return this_thread;
}

// Lock a thread
// NOTE: Use ks_thread_unlock(self) once the lock is through
void ks_thread_lock(ks_thread self) {
    pthread_mutex_lock(&self->_mut);
}

// Unlock a thread locked with 'ks_thread_lock(self)'
void ks_thread_unlock(ks_thread self) {
    pthread_mutex_unlock(&self->_mut);
}


/* member functions */


// thread.__str__(self) -> string representation
static KS_TFUNC(thread, str) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_thread self = (ks_thread)args[0];
    KS_REQ_TYPE(self, ks_type_thread, "self");

    return (ks_obj)ks_fmt_c("<%T:%R @ %p>", self, self->name, self);

};


// thread.__free__(self) -> free a thread object
static KS_TFUNC(thread, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_thread self = (ks_thread)args[0];
    KS_REQ_TYPE(self, ks_type_thread, "self");


    // ensure that has completed
    pthread_join(self->_pth, NULL);


    KS_DECREF(self->stk);

    ks_free(self->stack_frames);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
};


// thread.this() -> return current thread
static KS_TFUNC(thread, this) {
    KS_REQ_N_ARGS(n_args, 0);

    ks_thread cur = ks_thread_cur();

    if (!cur) {
        return (ks_obj)ks_throw_fmt(ks_type_Error, "No thread!");
    }

    return KS_NEWREF(cur);
};


// initialize thread type
void ks_type_thread_init() {
    KS_INIT_TYPE_OBJ(ks_type_thread, "thread");
    KS_INIT_TYPE_OBJ(ks_type_stack_frame, "stack_frame");

    ks_type_set_cn(ks_type_thread, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(thread_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(thread_str_)},
        {"__this__", (ks_obj)ks_cfunc_new(thread_this_)},
        {"__free__", (ks_obj)ks_cfunc_new(thread_free_)},

        
        {NULL, NULL}   
    });

    ks_type_set_cn(ks_type_stack_frame, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(stack_frame_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(stack_frame_free_)},
        {NULL, NULL}   
    });

    // create the global variable
    pthread_key_create(&this_thread_key, NULL);

    // construct the main thread
    main_thread = ks_thread_new("main");

}

