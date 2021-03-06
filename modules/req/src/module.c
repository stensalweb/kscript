/* req/src/module.c - the kscript's requests library
 *
 * 
 * @author: Cade Brown <brown.cade@gmail.com>
 */

// always begin by defining the module information
#define MODULE_NAME "req"

// include this since this is a module.
#include "ks-module.h"

#include <curl/curl.h>

// A req.Result object, representing a result from a web request
typedef struct {
    KS_OBJ_BASE

    // the URL that the request was made from
    ks_str url;

    // the HTTP status code returned by the request
    int status_code;

    // the string text that was returned by the request
    ks_str text;

}* req_Result;


// global curl variable
static CURL* curl_lib = NULL;

// declare the array type as residing in this file
KS_TYPE_DECLFWD(req_type_Result);

// helper function to make a result object
static req_Result req_make_Result(ks_str url, int status_code, ks_str text) {
    // create a new result
    req_Result self = KS_ALLOC_OBJ(req_Result);
    KS_INIT_OBJ(self, req_type_Result);

    self->url = (ks_str)KS_NEWREF(url);
    self->status_code = status_code;
    self->text = (ks_str)KS_NEWREF(text);

    return self;
}

/* Result.__free__(self) 
 *
 * Free a Result object
 * 
 */
static KS_TFUNC(Result, free) {
    KS_REQ_N_ARGS(n_args, 1);
    req_Result self = (req_Result)args[0];
    KS_REQ_TYPE(self, req_type_Result, "self");

    KS_DECREF(self->url);
    KS_DECREF(self->text);

    KS_UNINIT_OBJ(self);
    KS_FREE_OBJ(self);

    return KSO_NONE;
}

/* Result.__str__(self) 
 *
 * Convert to a string
 * 
 */
static KS_TFUNC(Result, str) {
    KS_REQ_N_ARGS(n_args, 1);
    req_Result self = (req_Result)args[0];
    KS_REQ_TYPE(self, req_type_Result, "self");

    return (ks_obj)ks_fmt_c("Result[%i]", self->status_code);
}

/* Result.__getattr__(self, attr) 
 *
 * Get an attribute
 * 
 */
static KS_TFUNC(Result, getattr) {
    KS_REQ_N_ARGS(n_args, 2);
    req_Result self = (req_Result)args[0];
    KS_REQ_TYPE(self, req_type_Result, "self");
    ks_str attr = (ks_str)args[1];
    KS_REQ_TYPE(attr, ks_type_str, "attr");

    // attempt to get one of the attributes
    if (attr->len == 3 && strncmp(attr->chr, "url", 3) == 0) {
        return KS_NEWREF(self->url);
    } else if (attr->len == 11 && strncmp(attr->chr, "status_code", 11) == 0) {
        return (ks_obj)ks_int_new(self->status_code);
    } else if (attr->len == 4 && strncmp(attr->chr, "text", 4) == 0) {
        return KS_NEWREF(self->text);
    } else {

        // now, try getting a member function
        ks_obj ret = ks_type_get_mf(self->type, attr, (ks_obj)self);
        if (!ret) {
            KS_ERR_ATTR(self, attr);
        }

        return ret;
    }
}

// custom callback for curl's writing from a stream
static size_t my_curlwrite(void* ptr, size_t size, size_t nmemb, void* _strb) {
    ks_str_b* strb = (ks_str_b*)_strb;

    // add to the string (which will require a lock, since it uses the memory manager
    ks_GIL_lock();
    ks_str_b_add(strb, size * nmemb, (char*)ptr);
    ks_GIL_unlock();

    return size * nmemb;
}


// custom callback for curl's writing from a stream, for use as a stream writer
// i.e. in the download() function
static size_t my_curlwrite_stream(void* ptr, size_t size, size_t nmemb, void* _ios) {
    ks_iostream ios = (ks_iostream)_ios;

    // add to the string (which will require a lock, since it uses the memory manager
    ks_GIL_lock();
    int res = ks_iostream_writestr_c(ios, (char*)ptr, size * nmemb);
    int amt_to_ret = size * nmemb;
    // mark error if something went wrong
    if (!res) {
        amt_to_ret = 0;
    }
    ks_GIL_unlock();

    return amt_to_ret;
}






// build a URL form key,value pairs in 'data'...
// i.e. key=value&key1=value1&...
// NOTE: You must use `ks_free` on the result
static char* my_buildurl(ks_dict data) {
    ks_str_b SB;
    ks_str_b_init(&SB);

    // whether not a valid entry has been processed
    bool hasHadOne = false;
    int i;
    for (i = 0; i < data->n_entries; ++i) {
        if (data->entries[i].key != NULL) {
            // valid entry, so add it
            if (hasHadOne) {
                ks_str_b_add_c(&SB, "&");
            }

            // TODO: convert to string, then run `curl_easy_escape` on the result
            //   per each key & value
            ks_str_b_add_str(&SB, data->entries[i].key);
            ks_str_b_add_c(&SB, "=");
            ks_str_b_add_str(&SB, data->entries[i].val);

            hasHadOne = true;
        }
    }

    ks_str resstr = ks_str_b_get(&SB);
    ks_str_b_free(&SB);

    //char* res_escaped = curl_easy_escape(curl_lib, resstr->chr, resstr->len);
    char* res_charp = ks_malloc(resstr->len + 1);
    memcpy(res_charp, resstr->chr, resstr->len + 1);
    KS_DECREF(resstr);

    return res_charp;
}




/* req.GET(url, data=none) -> req.Result
 *
 * Perform a GET request for a given URL, and return a Result object describing it
 *
 */
static KS_TFUNC(req, GET) {
    KS_REQ_N_ARGS(n_args, 1);
    ks_str url = (ks_str)args[0];
    KS_REQ_TYPE(url, ks_type_str, "url");

    // set the URL and allow redirects
    curl_easy_setopt(curl_lib, CURLOPT_URL, url->chr);
    curl_easy_setopt(curl_lib, CURLOPT_FOLLOWLOCATION, 1L);

    // set CURL up to write the result into a given string builder
    ks_str_b* strb = ks_malloc(sizeof(strb));
    ks_str_b_init(strb);

    curl_easy_setopt(curl_lib, CURLOPT_WRITEFUNCTION, my_curlwrite);
    curl_easy_setopt(curl_lib, CURLOPT_WRITEDATA, strb);

    // unlock the GIL while the web request is coming through
    ks_GIL_unlock();

    // actually perform the action
    CURLcode result_code = curl_easy_perform(curl_lib);

    // acquire lock to continue processing
    ks_GIL_lock();

    // handle an error if one occured
    if (result_code  != CURLE_OK) {
        ks_str_b_free(strb);
        ks_free(strb);
        return ks_throw(ks_type_IOError, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result_code));
    }

    // get result text, free the temp resources
    ks_str res_text = ks_str_b_get(strb);
    ks_str_b_free(strb);
    ks_free(strb);

    // query the HTTP code
    long http_code = 0;
    curl_easy_getinfo (curl_lib, CURLINFO_RESPONSE_CODE, &http_code);

    // construct the result we want to return
    req_Result res = req_make_Result(url, http_code, res_text);
    KS_DECREF(res_text);

    return (ks_obj)res;
}

/* req.POST(url, data=none) -> req.Result
 *
 * Perform a POST request for a given URL, and return a Result object describing it
 *
 */
static KS_TFUNC(req, POST) {
    KS_REQ_N_ARGS_RANGE(n_args, 1, 2);
    ks_str url = (ks_str)args[0];
    KS_REQ_TYPE(url, ks_type_str, "url");

    ks_dict data = (ks_dict)(n_args >= 2 ? args[1] : NULL);
    if (data) KS_REQ_TYPE(data, ks_type_dict, "data");

    // set the URL and allow redirects
    curl_easy_setopt(curl_lib, CURLOPT_URL, url->chr);
    curl_easy_setopt(curl_lib, CURLOPT_FOLLOWLOCATION, 1L);

    // set CURL up to write the result into a given string builder
    ks_str_b* strb = ks_malloc(sizeof(strb));
    ks_str_b_init(strb);

    curl_easy_setopt(curl_lib, CURLOPT_WRITEFUNCTION, my_curlwrite);
    curl_easy_setopt(curl_lib, CURLOPT_WRITEDATA, strb);

    // generate the POST string
    char* postfields_str = NULL;

    if (data) {
        postfields_str = my_buildurl(data);
        ks_printf("::%s\n", postfields_str);
    }

    curl_easy_setopt(curl_lib, CURLOPT_POSTFIELDS, postfields_str);

    // unlock the GIL while the web request is coming through
    ks_GIL_unlock();

    // actually perform the action
    CURLcode result_code = curl_easy_perform(curl_lib);

    // acquire lock to continue processing
    ks_GIL_lock();

    // handle an error if one occured
    if (result_code  != CURLE_OK) {
        ks_str_b_free(strb);
        ks_free(strb);
        return ks_throw(ks_type_IOError, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result_code));
    }

    // get result text, free the temp resources
    if (postfields_str) ks_free(postfields_str);
    ks_str res_text = ks_str_b_get(strb);
    ks_str_b_free(strb);
    ks_free(strb);

    // query the HTTP code
    long http_code = 0;
    curl_easy_getinfo (curl_lib, CURLINFO_RESPONSE_CODE, &http_code);

    // construct the result we want to return
    req_Result res = req_make_Result(url, http_code, res_text);
    KS_DECREF(res_text);

    return (ks_obj)res;
}


/* req.download(url, dest, data=none) -> req.Result
 *
 * Perform a GET request for a given URL, download the result into 'fname', which is a filename
 * 
 * Returns the normal results... Except .text gives the file name to which it was downloaded to
 *
 */
static KS_TFUNC(req, download) {
    KS_REQ_N_ARGS(n_args, 2);

    ks_str url = (ks_str)args[0];
    KS_REQ_TYPE(url, ks_type_str, "url");
    ks_obj dest_arg = args[1];
    ks_iostream dest = NULL;
    if (ks_type_issub(dest_arg->type, ks_type_iostream)) {
        // do nothing; already a correct type
        dest = (ks_iostream)KS_NEWREF(dest_arg);
    } else if (ks_type_issub(dest_arg->type, ks_type_str)) {
        // open the file in write mode
        dest = ks_iostream_new();

        // try to open and check for errors
        if (!ks_iostream_open(dest, ((ks_str)dest_arg)->chr, "w")) {
            KS_DECREF(dest);
            return NULL;
        }

    } else {
        // throw an error
        KS_REQ_TYPE(dest_arg, ks_type_iostream, "dest");
    }

    // set the URL and allow redirects
    curl_easy_setopt(curl_lib, CURLOPT_URL, url->chr);
    curl_easy_setopt(curl_lib, CURLOPT_FOLLOWLOCATION, 1L);

    // set CURL up to write the result into the given destination stream
    curl_easy_setopt(curl_lib, CURLOPT_WRITEFUNCTION, my_curlwrite_stream);
    curl_easy_setopt(curl_lib, CURLOPT_WRITEDATA, dest);

    // unlock the GIL while the web request is coming through
    ks_GIL_unlock();

    // actually perform the action
    CURLcode result_code = curl_easy_perform(curl_lib);

    // acquire lock to continue processing
    ks_GIL_lock();

    // handle an error if one occured
    if (result_code  != CURLE_OK) {
        KS_DECREF(dest);
        if (!ks_thread_get()->exc) return ks_throw(ks_type_IOError, "curl_easy_perform() failed: %s\n", curl_easy_strerror(result_code));
        else return NULL;
    }

    // query the HTTP code
    long http_code = 0;
    curl_easy_getinfo (curl_lib, CURLINFO_RESPONSE_CODE, &http_code);

    // construct the result we want to return
    req_Result res = NULL;
    if (dest_arg->type == ks_type_str) {
        res = req_make_Result(url, http_code, (ks_str)dest_arg);
    } else {
        ks_str dest_arg_str = ks_str_new("");
        res = req_make_Result(url, http_code, dest_arg_str);
        KS_DECREF(dest_arg_str);
    }
    
    
    KS_DECREF(dest);
    return (ks_obj)res;
}




// now, export them all
static ks_module get_module() {
    
    // initialize the global CURL lib
    curl_global_init(CURL_GLOBAL_ALL);

    // create a handle
    curl_lib = curl_easy_init();

    if (!curl_lib) {
        return ks_throw(ks_type_Error, "Error initializing libCURL!");
    }


    ks_module mod = ks_module_new(MODULE_NAME);

    KS_INIT_TYPE_OBJ(req_type_Result, "req.Result");

    ks_type_set_cn(req_type_Result, (ks_dict_ent_c[]){
        //{"__new__",        (ks_obj)ks_cfunc_new_c_old(array_new_, "nx.array.__new__(elems, dtype=None)")},
        {"__free__",        (ks_obj)ks_cfunc_new_c_old(Result_free_, "req.Result.__free__(self)")},

        {"__repr__",       (ks_obj)ks_cfunc_new_c_old(Result_str_, "req.Result.__repr__(self)")},
        {"__str__",        (ks_obj)ks_cfunc_new_c_old(Result_str_, "req.Result.__str__(self)")},

        {"__getattr__",    (ks_obj)ks_cfunc_new_c_old(Result_getattr_, "req.Result.__getattr__(self, attr)")},

        /*
        {"__getitem__",    (ks_obj)ks_cfunc_new_c_old(array_getitem_, "nx.array.__getitem__(self, *idxs)")},
        {"__setitem__",    (ks_obj)ks_cfunc_new_c_old(array_setitem_, "nx.array.__setitem__(self, *idxs)")},

        {"shape",          (ks_obj)ks_cfunc_new_c_old(array_shape_, "nx.array.shape(self)")},*/

        {NULL, NULL},
    });

    ks_dict_set_cn(mod->attr, (ks_dict_ent_c[]){
        /* constants */
        {"Result",     (ks_obj)req_type_Result},

        {"GET",        (ks_obj)ks_cfunc_new_c_old(req_GET_, "req.GET(url, data=none)")},
        {"POST",       (ks_obj)ks_cfunc_new_c_old(req_POST_, "req.POST(url, data=none)")},
        {"download",   (ks_obj)ks_cfunc_new_c_old(req_download_, "req.download(url, dest, data=none)")},

        {NULL, NULL}
    });

    return mod;
}

// boiler plate code
MODULE_INIT(get_module)
