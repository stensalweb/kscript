#!/bin/bash
# Run this to build all dependencies locally, in ./deps
#




# -*- CONFIG

# where to store the results
DEPDIR=$PWD/deps/src
PREFIXDIR=$PWD/deps/prefix

# how many jobs to invoke 'make' with
JOBS=16


CFLAGS=-fPIC

# tar files for various dependencies
TAR_GMP="./deps/gmp-6.2.0.tar.xz"
TAR_GLFW="./deps/glfw-3.3.2.tar.xz"


# -*- PREP

rm -rf $DEPDIR $PREFIXDIR

mkdir -p $DEPDIR
mkdir -p $PREFIXDIR


# -*- UNTAR ALL

tar xf $TAR_GMP -C $DEPDIR || { echo "Untarring 'GMP' failed"; exit 1; }
DIR_GMP=`echo $DEPDIR/gmp-*`

tar xf $TAR_GLFW -C $DEPDIR || { echo "Untarring 'GLFW' failed"; exit 1; }
DIR_GLFW=`echo $DEPDIR/glfw-*`


# -*- 1. Build GMP

cd $DIR_GMP

# extra flags to consider in some builds:
# --enable-fat --disable-assembly --enable-assert
./configure --prefix=$PREFIXDIR --enable-shared --enable-static CFLAGS=$CFLAGS &&
    make -j$JOBS &&
    make install || { echo "Compiling 'GMP' failed"; exit 1; }



# -*- 2. Build GLFW

cd $DIR_GLFW

cmake . -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=$PREFIXDIR -DBUILD_SHARED_LIBS=OFF \
    -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF && \
    make -j$JOBS && \
    make install || { echo "Compiling 'GLFW' failed"; exit 1; }




# -*- PRINT INFO

echo "Build 'GMP': $DIR_GMP"
echo "Build 'GLFW': $DIR_GLFW"




