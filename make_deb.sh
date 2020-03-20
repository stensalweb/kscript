#!/bin/bash
# run this **AFTER** `./configure` to create a debian binary
DESTDIR=$PWD/debian make -j32

fakeroot dpkg-deb --build debian $PWD/kscript.deb

echo "Built $PWD/kscript.deb"
