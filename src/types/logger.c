/* types/logger.c - implementation of the logger type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


/* constants */

// printed names for the levels (including colors)
static const char* _level_strs[] = {
    COL_WHT  "T",
    COL_WHT  "D",
    COL_WHT  "I",
    COL_WARN "W",
    COL_FAIL "E",
};


// forward declare it
KS_TYPE_DECLFWD(ks_type_logger);

// logger dictionary
ks_dict ks_all_loggers = NULL;

ks_logger ks_logger_get(const char* logname, bool createIfNeeded) {
    assert (ks_all_loggers != NULL);

    // create a string key
    ks_str key = ks_str_new((char*)logname);

    ks_logger got = (ks_logger)ks_dict_get_h(ks_all_loggers, (ks_obj)key, key->v_hash);
    if (!got) {
        if (createIfNeeded) {
            // create it
            got = KS_ALLOC_OBJ(ks_logger);
            KS_INIT_OBJ(got, ks_type_logger);

            got->name = (ks_str)KS_NEWREF(key);
            got->level = KS_LOG_WARN;

            ks_dict_set_h(ks_all_loggers, (ks_obj)key, key->v_hash, (ks_obj)got);

            KS_DECREF(key);
            return got;

        } else {
            ks_catch_ignore();
            ks_throw_fmt(ks_type_Error, "No logger named '%S'", key);
            KS_DECREF(key);
            return NULL;
        }

    } else if (got->type != ks_type_logger) {
        ks_throw_fmt(ks_type_InternalError, "ks_all_loggers contained a non-logger object: '%S' (for key: '%S')", got, key);
        KS_DECREF(got);
        KS_DECREF(key);
        return NULL;
    }

    return got;

}


// logger.__new__(name) -> get a logger by a given name (creates if it doesn't eixst)
static KS_TFUNC(logger, new) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str name = (ks_str)args[0];
    KS_REQ_TYPE(name, ks_type_str, "name");

    return (ks_obj)ks_logger_get(name->chr, true);
}
// logger.__free__(self) -> free resources held by a logger
static KS_TFUNC(logger, free) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_logger self = (ks_logger)args[0];
    KS_REQ_TYPE(self, ks_type_logger, "self");

    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}






// mutex for loggers
static ks_mutex logmut = NULL;


// initialize the type
void ks_type_logger_init() {

    KS_INIT_TYPE_OBJ(ks_type_logger, "logger");
    logmut = ks_mutex_new();
    ks_all_loggers = ks_dict_new(0, NULL);

    ks_type_set_cn(ks_type_logger, (ks_dict_ent_c[]){
        {"__new__", (ks_obj)ks_cfunc_new2(logger_new_, "logger.__new__(name)")},
        {"__free__", (ks_obj)ks_cfunc_new2(logger_free_, "logger.__free__(self)")},

        {NULL, NULL}   
    });

}



/* easy-C-API */


void ks_log_c(int level, const char* file, int line, const char* logname, const char* fmt, ...) {
    ks_logger lgr = ks_logger_get(logname, true);

    if (level >= lgr->level) {

        ks_mutex_lock(logmut);

        // print message preceder
        fprintf(stderr, "[%s:%s] [@%s:%i]" RESET ": ", _level_strs[level], lgr->name->chr, file, line);

        // now, convert arguments using the C string formatter I wrote for kscript
        va_list args;
        va_start(args, fmt);
        ks_str gen_str = ks_fmt_vc(fmt, args);
        va_end(args);

        // finish it off
        fprintf(stderr, "%s\n", gen_str->chr);
        KS_DECREF(gen_str);

        // flush the output
        fflush(stderr);

        // release mutex
        ks_mutex_unlock(logmut);
    }

    KS_DECREF(lgr);
}

// NOTE: if 'logname' is not created yet, it will be created with 'KS_LOG_WARN' as the default level
int ks_log_c_level(const char* logname) {
    ks_logger lgr = ks_logger_get(logname, true);
    int r = lgr->level;
    KS_DECREF(lgr);
    return r;
}

// NOTE: if 'logname' is not created yet, it will be created with and its level will be initialized
void ks_log_c_set(const char* logname, int level) {
    ks_logger lgr = ks_logger_get(logname, true);

    // set current level (clamp them)
    if (level > KS_LOG_ERROR) level = KS_LOG_ERROR;
    else if (level < KS_LOG_TRACE) level = KS_LOG_TRACE;
    lgr->level = level;

    KS_DECREF(lgr);
}

