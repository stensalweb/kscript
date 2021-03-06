/* types/logger.c - implementation of the logger type
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


/* constants */

// printed names for the levels (including colors)
static const char* _level_strs[] = {
    COL_LGRY "TRACE",
    COL_LGRY "DEBUG",
    COL_LBLU "INFO ",
    COL_WARN "WARN ",
    COL_FAIL COL_BOLD "ERROR",
};




// internal logger
static void my_log(int level, const char* file, int line, ks_logger lgr, const char* fmt, ...) {

    if (level >= lgr->level) {
        //ks_mutex_lock(logmut);

        // now, convert arguments using the C string formatter I wrote for kscript
        va_list args;
        va_start(args, fmt);
        ks_str gen_str = ks_fmt_vc(fmt, args);
        va_end(args);


        // print message preceder
        fprintf(stderr, COL_RESET "[" COL_MGA "%s" COL_RESET "] [%s" COL_RESET "] ", lgr->name->chr, _level_strs[level]);

        // print out file information
        if (line > 0 && file != NULL) {
            fprintf(stderr, "[" COL_LBLU "@" "%s" COL_RESET ":" COL_LCYN "%i" COL_RESET "]: ", file, line);
        }


        // finish it off
        fprintf(stderr, "%s\n", gen_str->chr);
        KS_DECREF(gen_str);

        // flush the output
        fflush(stderr);

        // release mutex
        //ks_mutex_unlock(logmut);
    }

    KS_DECREF(lgr);
}



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
            KS_INIT_OBJ(got, ks_T_logger);

            got->name = (ks_str)KS_NEWREF(key);
            got->level = KS_LOG_WARN;

            ks_dict_set_h(ks_all_loggers, (ks_obj)key, key->v_hash, (ks_obj)got);

            KS_DECREF(key);
            return got;

        } else {
            ks_catch_ignore();
            ks_throw(ks_T_Error, "No logger named '%S'", key);
            KS_DECREF(key);
            return NULL;
        }

    } else if (got->type != ks_T_logger) {
        ks_throw(ks_T_InternalError, "ks_all_loggers contained a non-logger object: '%S' (for key: '%S')", got, key);
        KS_DECREF(got);
        KS_DECREF(key);
        return NULL;
    }

    return got;

}


// logger.__new__(name) -> get a logger by a given name (creates if it doesn't eixst)
static KS_TFUNC(logger, new) {
    ks_str name;
    KS_GETARGS("name:*", &name, ks_T_str)

    return (ks_obj)ks_logger_get(name->chr, true);
}


// logger.__free__(self) -> free resources held by a logger
static KS_TFUNC(logger, free) {
    ks_logger self;
    KS_GETARGS("self:*", &self, ks_T_logger)

    KS_DECREF(self->name);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}




// get level from string
static int level_from_str(ks_str lvl) {

    if (ks_str_eq_c(lvl, "ERROR", 5) || ks_str_eq_c(lvl, "error", 5)) {
        return KS_LOG_ERROR;
    } else if (ks_str_eq_c(lvl, "WARN", 4) || ks_str_eq_c(lvl, "warn", 4)) {
        return KS_LOG_WARN;
    } else if (ks_str_eq_c(lvl, "INFO", 4) || ks_str_eq_c(lvl, "info", 4)) {
        return KS_LOG_INFO;
    } else if (ks_str_eq_c(lvl, "DEBUG", 5) || ks_str_eq_c(lvl, "debug", 5)) {
        return KS_LOG_DEBUG;
    } else if (ks_str_eq_c(lvl, "TRACE", 5) || ks_str_eq_c(lvl, "trace", 5)) {
        return KS_LOG_TRACE;
    } else {
        ks_throw(ks_T_Error, "Unknown logging level: %R", lvl);
        return -1;
    }

}

// logger.set(self, level='WARN')
static KS_TFUNC(logger, set) {
    ks_logger self;
    ks_obj level = NULL;
    KS_GETARGS("self:* ?level", &self, ks_T_logger, &level);

    if (!level) self->level = KS_LOG_WARN;

    if (level->type == ks_T_str) {
        // set string
        int lvl = level_from_str((ks_str)level);
        if (lvl < 0) return NULL;

        self->level = lvl;

        return KSO_NONE;
    }


    return ks_throw(ks_T_Error, "Unknown logging level: %R", level);
}


// logger.error(self, *objs) - print message
static KS_TFUNC(logger, error) {
    ks_logger self;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("self:* *extra", &self, ks_T_logger, &n_extra, &extra);

    ks_str fname = NULL;
    int line = -1;
    ks_thread_getloc(ks_thread_get(), &fname, &line);

    my_log(KS_LOG_ERROR, fname ? fname->chr : NULL, line, self, "%A", n_extra, extra);

    return KSO_NONE;
}

// logger.warn(self, *objs) - print message
static KS_TFUNC(logger, warn) {
    ks_logger self;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("self:* *extra", &self, ks_T_logger, &n_extra, &extra);

    ks_str fname = NULL;
    int line = -1;
    ks_thread_getloc(ks_thread_get(), &fname, &line);

    my_log(KS_LOG_WARN, fname ? fname->chr : NULL, line, self, "%A", n_extra, extra);

    return KSO_NONE;
}

// logger.info(self, *objs) - print message
static KS_TFUNC(logger, info) {
    ks_logger self;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("self:* *extra", &self, ks_T_logger, &n_extra, &extra);

    ks_str fname = NULL;
    int line = -1;
    ks_thread_getloc(ks_thread_get(), &fname, &line);

    my_log(KS_LOG_INFO, fname ? fname->chr : NULL, line, self, "%A", n_extra, extra);

    return KSO_NONE;
}

// logger.debug(self, *objs) - print message
static KS_TFUNC(logger, debug) {
    ks_logger self;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("self:* *extra", &self, ks_T_logger, &n_extra, &extra);

    ks_str fname = NULL;
    int line = -1;
    ks_thread_getloc(ks_thread_get(), &fname, &line);

    my_log(KS_LOG_DEBUG, fname ? fname->chr : NULL, line, self, "%A", n_extra, extra);

    return KSO_NONE;
}

// logger.trace(self, *objs) - print message
static KS_TFUNC(logger, trace) {
    ks_logger self;
    int n_extra;
    ks_obj* extra;
    KS_GETARGS("self:* *extra", &self, ks_T_logger, &n_extra, &extra);

    ks_str fname = NULL;
    int line = -1;
    ks_thread_getloc(ks_thread_get(), &fname, &line);

    my_log(KS_LOG_TRACE, fname ? fname->chr : NULL, line, self, "%A", n_extra, extra);

    return KSO_NONE;
}





// mutex for loggers
//static ks_mutex logmut = NULL;


// forward declare it
KS_TYPE_DECLFWD(ks_T_logger);

// initialize the type
void ks_init_T_logger() {

    ks_all_loggers = ks_dict_new(0, NULL);

    ks_type_init_c(ks_T_logger, "logger", ks_T_object, KS_KEYVALS(
        {"__new__",                (ks_obj)ks_cfunc_new_c_old(logger_new_, "logger.__new__(name)")},
        {"__free__",               (ks_obj)ks_cfunc_new_c_old(logger_free_, "logger.__free__(self)")},

        {"set",                    (ks_obj)ks_cfunc_new_c_old(logger_set_, "logger.set(self, level='WARN')")},

        {"error",                  (ks_obj)ks_cfunc_new_c_old(logger_error_, "logger.error(self, *objs)")},
        {"warn",                   (ks_obj)ks_cfunc_new_c_old(logger_warn_, "logger.warn(self, *objs)")},
        {"info",                   (ks_obj)ks_cfunc_new_c_old(logger_info_, "logger.info(self, *objs)")},
        {"debug",                  (ks_obj)ks_cfunc_new_c_old(logger_debug_, "logger.debug(self, *objs)")},
        {"trace",                  (ks_obj)ks_cfunc_new_c_old(logger_trace_, "logger.trace(self, *objs)")},

    ));

}



/* easy-C-API */


void ks_log_c(int level, const char* file, int line, const char* logname, const char* fmt, ...) {
    ks_logger lgr = ks_logger_get(logname, true);

    if (level >= lgr->level) {
        //ks_mutex_lock(logmut);

        // now, convert arguments using the C string formatter I wrote for kscript
        va_list args;
        va_start(args, fmt);
        ks_str gen_str = ks_fmt_vc(fmt, args);
        va_end(args);


        // print message preceder
        fprintf(stderr, COL_RESET "[" COL_MGA "%s" COL_RESET "] [%s" COL_RESET "] ", lgr->name->chr, _level_strs[level]);

        // print out file information
        if (line > 0 && file != NULL) {
            fprintf(stderr, "[" COL_LBLU "@" "%s" COL_RESET ":" COL_LCYN "%i" COL_RESET "]: ", file, line);
        }


        // finish it off
        fprintf(stderr, "%s\n", gen_str->chr);
        KS_DECREF(gen_str);

        // flush the output
        fflush(stderr);

        // release mutex
        //ks_mutex_unlock(logmut);
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

