#!/bin/bash
# Run this to build all dependencies locally, in ./deps
#




# -*- CONFIG

# where to store the results
KSDIR=$PWD
DEPDIR=$PWD/deps/src
PREFIXDIR=$PWD/deps/prefix

# how many jobs to invoke 'make' with
JOBS=32


CFLAGS=-fPIC

# tar files for various dependencies
TAR_GMP="./deps/gmp-6.2.0.tar.xz"
TAR_GLFW="./deps/glfw-3.3.2.tar.xz"
TAR_FFI="./deps/libffi-3.3.tar.gz"

# -*- PREP

#rm -rf $DEPDIR $PREFIXDIR

mkdir -p $DEPDIR
mkdir -p $PREFIXDIR


# -*- UNTAR ALL

tar xf $TAR_GMP -C $DEPDIR || { echo "Untarring 'GMP' failed"; exit 1; }
DIR_GMP=`echo $DEPDIR/gmp-*`

tar xf $TAR_GLFW -C $DEPDIR || { echo "Untarring 'GLFW' failed"; exit 1; }
DIR_GLFW=`echo $DEPDIR/glfw-*`

tar xf $TAR_FFI -C $DEPDIR || { echo "Untarring 'FFI' failed"; exit 1; }
DIR_FFI=`echo $DEPDIR/libffi-*`

DIR_FFMPEG=$DEPDIR
mkdir -p $DIR_FFMPEG

# -*- 1. Build GMP

cd $DIR_GMP

# extra flags to consider in some builds:
# --enable-fat --disable-assembly --enable-assert
./configure --prefix=$PREFIXDIR --enable-static --disable-shared --disable-assembly CFLAGS="-fPIC" &&
    make -j$JOBS &&
    make install || { echo "Compiling 'GMP' failed"; exit 1; }

cd $DIR_FFI

# -*- 2. Build FFI
# extra flags to consider in some builds:
# --enable-fat --disable-assembly --enable-assert
./configure --prefix=$PREFIXDIR --enable-static --disable-shared CFLAGS="-fPIC" &&
    make -j$JOBS &&
    make install || { echo "Compiling 'FFI' failed"; exit 1; }


# -*- 3. Build GLFW

cd $DIR_GLFW
cmake . -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX:PATH=$HOME/projects/kscript/deps/prefix \
    -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=true \
    -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF -DGLFW_BUILD_DOCS=OFF &&
    make -j$JOBS &&
    make install || { echo "Compiling 'GLFW' failed"; exit 1; }

# -*- 4. Build FFMPEG

cd $DIR_FFMPEG

# extra flags to consider in some builds:
# --enable-fat --disable-assembly --enable-assert
yes no | $KSDIR/tools/build-ffmpeg --build || { echo "Compiling 'FFMPEG' failed"; exit 1; }



# -*- PRINT INFO

echo "DONE BUILDING"




