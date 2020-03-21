#!/bin/bash
# run this to make a debian release, resulting in `kscript.deb`
# NOTE: This will reconfigure the project!
# NOTE: To isntall requirements, run `sudo apt install fakeroot dpkg lintian`
rm -rf debian/usr

# configure, make, and install (locally), then build it
./configure --prefix=/usr --dest-dir=$PWD/debian --disable-rpath && \
    make -j32 install && \
    fakeroot dpkg-deb --build debian $PWD/kscript.deb || { echo "Failed to build $PWD/kscript.deb"; exit 1; }

echo "Built $PWD/kscript.deb"

lintian kscript.deb --suppress-tags \
    non-dev-pkg-with-shlib-symlink,package-name-doesnt-match-sonames

echo "Checked $PWD/kscript.deb"
