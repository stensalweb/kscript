# BUILD.md

This file contains more detailed building/installation information for kscript.

Most of this information is detailed in `./Makefile` as well

To build, run these commands:

  * `KS_OPTS="..." make`, which configures & makes the library & executable (running like `KS_OPTS="" make` does a 'default' build. This is probably what you want)
  * `PREFIX=/usr/local sudo make install`, which installs the library & executable into the prefix (default: `/usr/local`)

Here are some example builds:

  * `Debug`: `KS_OPTS="" CFLAGS="-Og -g -std=c99 -Wall" make`, this builds a binary & library which can be used in a debugger, and prints out many warning messages
  * `Release` `KS_OPTS="KS_C_NO_TRACE" CFLAGS="-std=c99 -Ofast -ffast-math"`, this builds a fast binary with no `TRACE:` support

## `KS_OPTS`

Giving `KS_OPTS="..."` before the first `make` will set the build options

Here are a few options:

  * `KS_C_NO_TRACE`: disable all `ks_trace()` calls. This can be useful for release builds
  * `KS_C_NO_DEBUG`: disable all `ks_debug()` calls. This can be useful for release builds

## Dependencies

Although kscript itself strives to require 0 dependencies other than a C compiler and `make`, some of the standard libraries may require external libraries to be installed (for example, the multimedia library requires some audio/image libraries).

Here are a list of standard modules (found in `./std`) and their requirements



