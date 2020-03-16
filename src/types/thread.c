/* types/thread.c - implementation of 'ks_thread', for executing concurrently
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// forward declare it
KS_TYPE_DECLFWD(ks_type_stack_frame);
KS_TYPE_DECLFWD(ks_type_mutex);
KS_TYPE_DECLFWD(ks_type_thread);

/** MISC THREAD TYPES **/

// stack frame: used in thread's implementation

// create a stack frame
ks_stack_frame ks_stack_frame_new(ks_obj func) {
    ks_stack_frame self = KS_ALLOC_OBJ(ks_stack_frame);
    KS_INIT_OBJ(self, ks_type_stack_frame);

    self->func = KS_NEWREF(func);

    self->kfunc = (ks_kfunc)(func->type == ks_type_kfunc ? func : NULL);

    self->locals = NULL;
    self->pc = NULL;

    return self;
}


/* member functions */

// stack_frame.__free__(self) -> free a stack frame object
static KS_TFUNC(stack_frame, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_stack_frame self = (ks_stack_frame)args[0];
    KS_REQ_TYPE(self, ks_type_stack_frame, "self");

    KS_DECREF(self->func);

    // free local variables, if allocated
    if (self->locals != NULL) KS_DECREF(self->locals);


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
            ks_str o_str = ks_tok_expstr_2(code_obj->meta[fi].tok);
            ks_str new_str = ks_fmt_c("%S (line %i, col %i): %S", code_obj->name_hr, code_obj->meta[fi].tok.line+1, code_obj->meta[fi].tok.col+1, o_str);
            KS_DECREF(o_str);

            return (ks_obj)new_str;
        }
    } else if (self->func->type == ks_type_kfunc) {
        // attempt to search for it

        ks_code code_obj = ((ks_kfunc)self->func)->code;

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
            ks_str o_str = ks_tok_expstr_2(code_obj->meta[fi].tok);
            ks_str new_str = ks_fmt_c("%S (line %i, col %i): %S", code_obj->name_hr, code_obj->meta[fi].tok.line+1, code_obj->meta[fi].tok.col+1, o_str);
            KS_DECREF(o_str);

            return (ks_obj)new_str;
        }

    } else if (self->func->type == ks_type_cfunc) {
        return (ks_obj)ks_fmt_c("%S [cfunc]", ((ks_cfunc)self->func)->name_hr);

    }

    return (ks_obj)ks_fmt_c("<'stack_frame' type(func): %T obj @ %p>", self->func, self);
};


/* MUTEX */

// Construct a new, unlocked, mutex
// NOTE: This returns a new reference
ks_mutex ks_mutex_new() {
    ks_mutex self = KS_ALLOC_OBJ(ks_mutex);
    KS_INIT_OBJ(self, ks_type_mutex);

    // create pthread's mutex
    self->_mut = ks_malloc(sizeof(*self->_mut));
    pthread_mutex_init(self->_mut, NULL);

    return self;
}

// Lock a mutex
void ks_mutex_lock(ks_mutex self) {
    pthread_mutex_lock(self->_mut);
}

// Unlock a mutex
void ks_mutex_unlock(ks_mutex self) {
    pthread_mutex_unlock(self->_mut);
}


// acquire the GIL lock
void ks_lockGIL() {
    ks_thread th = ks_thread_cur();
    assert(th != NULL && "Not in a thread!");
    if (!th->hasGIL) ks_mutex_lock(ks_GIL);
    th->hasGIL = true;
}

// end GIL usage
void ks_unlockGIL() {
    ks_thread th = ks_thread_cur();
    assert(th != NULL && "Not in a thread!");
    if (th->hasGIL) ks_mutex_unlock(ks_GIL);
    th->hasGIL = false;

}



/* member functions */



static KS_TFUNC(mutex, lock) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mutex self = (ks_mutex)args[0];
    KS_REQ_TYPE(self, ks_type_mutex, "self");

    // lock itself
    ks_mutex_lock(self);

    // always return true (success)
    return KSO_TRUE;

}

static KS_TFUNC(mutex, unlock) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mutex self = (ks_mutex)args[0];
    KS_REQ_TYPE(self, ks_type_mutex, "self");

    // unlock itself
    ks_mutex_unlock(self);

    // always return true (success)
    return KSO_TRUE;

}

static KS_TFUNC(mutex, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_mutex self = (ks_mutex)args[0];
    KS_REQ_TYPE(self, ks_type_mutex, "self");

    // free mutex resource
    pthread_mutex_destroy(self->_mut);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;

}



/* ACTUAL THREAD TYPE */

// the main thread, i.e. global thread
static ks_thread main_thread = NULL;

// thread-local variable for the current thread
pthread_key_t this_thread_key;

// initialize a thread
static void* thread_init(void* _self) {
    ks_thread self = (ks_thread)_self;

    // set the global variable
    pthread_setspecific(this_thread_key, (void*)self);

    ks_debug("thread <%p> initializing!", self);

    // execute
    ks_lockGIL();
    ks_obj ret = ks_call(self->target, self->args->len, self->args->elems);

    if (!ret) {
        // handle exception
        ks_errend();
    }

    // store the result
    self->result = ret;

    ks_unlockGIL();

    return NULL;
}

// create a new thread
ks_thread ks_thread_new(char* name, ks_obj func, int n_args, ks_obj* args) {
    ks_thread self = KS_ALLOC_OBJ(ks_thread);
    KS_INIT_OBJ(self, ks_type_thread);

    ks_thread parent = ks_thread_cur();

    // add it to the parent thread
    if (parent) {
        ks_list_push(parent->sub_threads, (ks_obj)self);
    }

    // initialize variables
    self->hasGIL = false;

    // start off with NULL
    self->result = NULL;

    if (name) {
        self->name = ks_str_new(name);
    } else {
        self->name = ks_fmt_c("__thread__%p", self);
    }

    self->stk = ks_list_new(0, NULL);
    self->stack_frames = ks_list_new(0, NULL);

    self->target = KS_NEWREF(func);
    self->args = ks_tuple_new(n_args, args);

    self->sub_threads = ks_list_new(0, NULL);

    self->exc = NULL;
    self->exc_info = NULL;

    // set the pthread
    self->_pth_active = false;

    return self;
}

// start executing the thread
void ks_thread_start(ks_thread self) {
    if (self->_pth_active) return;
    
    // begin the pthread
    self->_pth_active = true;

    // start the pthread up
    pthread_create(&self->_pth, NULL, thread_init, self);
}

// join the thread back
void ks_thread_join(ks_thread self) {
    if (!self->_pth_active) return;

    // unlock ourselves so the other one can join
    bool inThread = ks_thread_cur() != NULL;
    if (inThread) ks_unlockGIL();


    pthread_join(self->_pth, NULL);
    self->_pth_active = false;


    int i;
    for (i = 0; i < self->sub_threads->len; ++i) {
        ks_thread_join((ks_thread)self->sub_threads->elems[i]);
    }

    // acquire back the lock
    if (inThread) ks_lockGIL();
}

// return the current thread
// NOTE: Does *NOT* return a new reference to the thread
ks_thread ks_thread_cur() {
    // attempt to return it
    ks_thread this_thread = (ks_thread)pthread_getspecific(this_thread_key);
    return this_thread;
}

/* member functions */

// thread.__new__(name, target, args=(,)) -> construct a thread from a target
static KS_TFUNC(thread, new) {
    KS_REQ_N_ARGS_MIN(n_args, 2);
    ks_str name = (ks_str)args[0];
    KS_REQ_TYPE(name, ks_type_str, "name");
    ks_obj target = (ks_obj)args[1];
    KS_REQ_CALLABLE(target, "target");

    // result thread
    ks_thread ret = NULL;

    if (n_args > 2) {
        ks_tuple p_args = (ks_tuple)args[2];
        KS_REQ_TYPE(p_args, ks_type_tuple, "args");
        // for some reason, it seems we need an extra reference here
        KS_INCREF(p_args);
        ret = ks_thread_new(name->chr, target, p_args->len, p_args->elems);
    } else {
        ret = ks_thread_new(name->chr, target, 0, NULL);
    }

    return (ks_obj)ret;

};

// thread.start(self) -> start executing the thread
static KS_TFUNC(thread, start) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_thread self = (ks_thread)args[0];
    KS_REQ_TYPE(self, ks_type_thread, "self");

    ks_thread_start(self);

    return KSO_NONE;

};

// thread.join(self) -> join the thread
static KS_TFUNC(thread, join) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_thread self = (ks_thread)args[0];
    KS_REQ_TYPE(self, ks_type_thread, "self");

    ks_thread_join(self);

    return KSO_NONE;
};

// thread.result(self) -> join the thread, and then get its result
static KS_TFUNC(thread, result) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_thread self = (ks_thread)args[0];
    KS_REQ_TYPE(self, ks_type_thread, "self");

    ks_thread_join(self);

    return KS_NEWREF(self->result);
};


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
    ks_thread_join(self);

    ks_debug("thread <%p> done!", self);

    KS_DECREF(self->target);
    KS_DECREF(self->args);

    // free result if it exists
    if (self->result) KS_DECREF(self->result);

    // dereference vars
    KS_DECREF(self->sub_threads);

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
    KS_INIT_TYPE_OBJ(ks_type_stack_frame, "stack_frame");
    KS_INIT_TYPE_OBJ(ks_type_mutex, "mutex");
    KS_INIT_TYPE_OBJ(ks_type_thread, "thread");

    ks_type_set_cn(ks_type_stack_frame, (ks_dict_ent_c[]){
        {"__str__", (ks_obj)ks_cfunc_new(stack_frame_str_)},
        {"__free__", (ks_obj)ks_cfunc_new(stack_frame_free_)},
        {NULL, NULL}   
    });

    ks_type_set_cn(ks_type_mutex, (ks_dict_ent_c[]){
        {"lock", (ks_obj)ks_cfunc_new(mutex_lock_)},
        {"unlock", (ks_obj)ks_cfunc_new(mutex_unlock_)},

        {"__free__", (ks_obj)ks_cfunc_new(mutex_free_)},
        {NULL, NULL}   
    });

    ks_type_set_cn(ks_type_thread, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new(thread_new_)},

        {"start", (ks_obj)ks_cfunc_new(thread_start_)},
        {"join", (ks_obj)ks_cfunc_new(thread_join_)},
        {"result", (ks_obj)ks_cfunc_new(thread_result_)},

        {"__str__", (ks_obj)ks_cfunc_new(thread_str_)},
        {"__repr__", (ks_obj)ks_cfunc_new(thread_str_)},
        {"__this__", (ks_obj)ks_cfunc_new(thread_this_)},
        {"__free__", (ks_obj)ks_cfunc_new(thread_free_)},
        
        {NULL, NULL}   
    });

    // create the global variable
    pthread_key_create(&this_thread_key, NULL);

}

