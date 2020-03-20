#!/bin/bash
# run this to make a debian release
# NOTE: This will reconfigure the project!

# configure, make, and install (locally), then build it
./configure --prefix=/usr --dest-dir=$PWD/debian && \
    make -j32 install && \
    fakeroot dpkg-deb --build debian $PWD/kscript.deb || { echo "Failed to build $PWD/kscript.deb"; exit 1; }

echo "Built $PWD/kscript.deb"
