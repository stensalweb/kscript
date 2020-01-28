#!/bin/sh
# builds the dependencies into a local direcotory

# capture where we started
START_DIR=$PWD

# for libsndfile
SRC_LIBSNDFILE=deps/libsndfile-*.tar.gz


# compile libsndfile
tar xf $SRC_LIBSNDFILE -C ./build
cd build/libsndfile-*

./autogen.sh
test $? -eq 0 || { echo "Compiling `libsndfile` failed!"; exit 1; }
./configure
test $? -eq 0 || { echo "Compiling `libsndfile` failed!"; exit 1; }
make -j8
test $? -eq 0 || { echo "Compiling `libsndfile` failed!"; exit 1; }
make check
test $? -eq 0 || { echo "Compiling `libsndfile` failed!"; exit 1; }


# get the library
LIB_LIBSNDFILE=$PWD/src/.libs/libsndfile.so



