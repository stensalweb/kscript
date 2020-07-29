/* citer.c - implementation of C-style iterable routines
 *
 * 
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */


#include "ks-impl.h"


// Create a new C iterator for object
// NOTE: Make sure to use `ks_citer_done(&result)` after you are done with the object
struct ks_citer ks_citer_make(ks_obj obj) {
    struct ks_citer cit;
    cit.obj = obj;
    cit.done = false;
    cit.threwErr = false;

    if (obj->type->__next__ != NULL) {
        // already has .next
        KS_INCREF(obj);
        cit.iter_obj = obj;
    } else {
        // we need to get an iterable
        cit.iter_obj = ks_F_iter->func(1, &obj);
        // check for errors
        if (!cit.iter_obj) {

            cit.done = true;
            cit.threwErr = true;
        }
    }

    return cit;
}

// Do the next C-iterator
// NOTE: NULL just indicates end-of-iterator, not neccessarily an error
ks_obj ks_citer_next(struct ks_citer* cit) {
    // done
    if (cit->done || cit->threwErr) return NULL;

    ks_obj next_obj = ks_F_next->func(1, &cit->iter_obj);
    if (!next_obj) {
        // no matter what, we're done now
        cit->done = true;

        // get current thread
        ks_thread cth = ks_thread_get();

        // check exception status for 'OutOfIterError'
        if (cth->exc && cth->exc->type == ks_T_OutOfIterError) {
            // ignore; this just signals end of input
            ks_catch_ignore();
        } else {

            // an unrelated error was thrown
            cit->threwErr = true;
        }
    }

    return next_obj;
}

// Declare we are done with the C iterator; free all resources and turn `cit` INVALID for further use
void ks_citer_done(struct ks_citer* cit) {
    if (cit->iter_obj) KS_DECREF(cit->iter_obj);
    cit->done = true;
}



