# BUILD.md

This file contains more detailed building/installation information for kscript.

Most of this information is detailed in `./Makefile` as well

To build, run these commands:

  * `./configure`, which configures & generates config files and makefiles
  * `make`, which actually builds the target
  * `PREFIX=/usr/local sudo make install`, which installs the library & executable into the prefix (default: `/usr/local`) (this part is optional)


Here are some example builds:

  * Debug Build: `CFLAGS="-Og -g -std=c99 -Wall" ./configure --build-type debug && make`
  * Release Build: `CFLAGS="-Ofast -ffast-math -std=c99" ./configure --build-type release --disable-trace && make`


Run `./configure -h` for descriptions of various options for configuration

## Directory Structure

The installation structure looks like:

  * `$PREFIX/` (default: `/usr/local` on Linux/Unix)
    * `bin/`
      * `ks` (the main binary, as a symlink to the specific version)
      * `ks-VER` (i.e. `ks-0.0.1`), for a specific version
    * `include/`
      * `ks-VER/` (i.e. `ks-0.0.1/`), the directory containing standard includes
    * `lib/`
      * `libks.so` (symlink to main library)
      * `ks-VER/` (i.e. `ks-0.0.1/`), the directory containing standard library & module installations
        * `libks.so.VER` (i.e. `libks.so.0.0.1`, for a specific version)
        * `modules/` a folder of actual modules (see below for their format)
        
### Modules


