#!/bin/sh
# generate a kscript config from the argv's passed in (should be 'KS_OPTS')
# echo it out to stdout, so that you can call this script and redirect to a file

# options
OPTS="$@"

echo "//$OPTS"

echo '/* ks_config.h - set the kscript build configuration information */

#pragma once
#ifndef KS_CONFIG_H__
#define KS_CONFIG_H__

#ifdef __cplusplus
extern "C" {
#endif

// OPTIONS:
'

# TODO: autodetect?
for i in $OPTS; do
    # process "$i"
    # split on ':'
    set -- `echo $i | tr ':' ' '`
    
    # ensure this is the newest definition of the given argument, and add it to the generated header
    echo "
#ifdef $1
#undef $1
#endif
#define $1 $2"

done


echo '

#ifdef __cplusplus
}
#endif

#endif /* KS_CONFIG_H__ */'

