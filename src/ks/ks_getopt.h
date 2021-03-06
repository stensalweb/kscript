/*
  Copyright 2005-2014 Rich Felker, et al.
  
  Permission is hereby granted, free of charge, to any person obtaining
  a copy of this software and associated documentation files (the
  "Software"), to deal in the Software without restriction, including
  without limitation the rights to use, copy, modify, merge, publish,
  distribute, sublicense, and/or sell copies of the Software, and to
  permit persons to whom the Software is furnished to do so, subject to
  the following conditions:
  
  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef KS_GETOPT_H
#define KS_GETOPT_H


#ifdef __cplusplus
extern "C" {
#endif

#include "ks-impl.h"

int ks_getopt(int, char * const [], const char *);
extern char *ks_optarg;
extern int ks_optind, ks_opterr, ks_optopt, ks_optreset;

// argument parsing options
struct ks_option {
    const char *name;
    int has_arg;
    int *flag;
    int val;
};

// main functions
int ks_getopt_long(int, char *const *, const char *, const struct ks_option *, int *);
int ks_getopt_long_only(int, char *const *, const char *, const struct ks_option *, int *);

#define no_argument        0
#define required_argument  1
#define optional_argument  2

#ifdef __cplusplus
}
#endif

#endif