#!/bin/sh


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
