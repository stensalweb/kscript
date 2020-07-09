# BUILD.md

This file contains more detailed building/installation information for kscript.

## Dependencies

You can build with no requirements, but for standard releases, the following packages are required for a full build including standard modules:

Packages: `gmp`, `readline`, `curl`, `glfw3`, `X11`, (`libavcodec`, `libavutil`, ...)

## Building Deps

It's recommended to run `./build_deps.sh` (may take a while!), and individual packages may copy and/or link in static libraries built to be more portable

## System Packages

Linux (Ubuntu): `sudo apt install cmake m4 libgmp-dev libreadline-dev libcurl4-openssl-dev libglfw3-dev xorg-dev libavcodec-dev libavcodec-extra libavutil-dev libavformat-dev`

MacOS: `brew install cmake gmp readline curl glfw libav`


Windows (cygwin):
cygw

```
make (4.3-1)
cmake (3.13.1-1)
gcc-core (9.3.0-2)
gcc-g++ (9.3.0-2)
python3 (3.8.3-1)
curl (7.66.0-1)
m4 (1.4.18-1)
autoconf (13-1)
automake (11-1)

zlib (1.2.11-1)
zlib-devel (1.2.11-1)

libX11 (1.6.9-1)
libX11-devel (1.6.9-1)
libgmp (6.2.0-2)
libgmp-devel (6.2.0-2)
libreadline (7.0.3-3)
libreadline-devel (7.0.3-3)
libcurl4 (7.66.0-1)
libcurl-devel (7.66.0-1)
fftw3 (3.3.8-1)
libfftw3-devel (3.3.8-1)
libffi6 (3.2.1-2)
libffi-devel (3.2.1-2)
libGL1 (19.1.6)
libGL-devel (19.1.6)

xorg-server (1.20.5-3)
xorg-server-devel (1.20.5-3)
xinit (1.4.1-1)

libXrand2 (1.5.2-1)
libXrandr-devel (1.5.2-1)
libXinerama1 (1.1.4-1)
libXinerama-devel (1.1.4-1)
libXcursor1 (1.2.0-1)
libXcursor-devel (1.2.0-1)
libXi6 (1.7.10-1)
libXi-devel (1.7.10-1)




```


## Build Process 

The basic build steps `1. configure -> 2. make [-> 3. install]`, where installation is optional

### 1. Configure

Run `./configure --help` to see options for configuration. The default, no arguments works fine for most uses:

```bash
$ ./configure
```

Ensure no errors were given. This will generate the file `./Makefile` that will build kscript with what dependencies were autodetected.

You can set an installation prefix via `./configure --prefix=/path/to/install`

#### Other Configurations

  * Force All: `./configure --with-gmp --with-curl --with-readline --with-glfw3 --with-libav`
  * Force None: `./configure --without-gmp --without-curl --without-readline --without-glfw3 --without-libav -M`
  * Debug Build: `CFLAGS="-Og -g -std=c99 -Wall" ./configure --build-type debug`
  * Release Build: `CFLAGS="-Ofast -ffast-math -std=c99" ./configure --build-type release --disable-trace`

### 2. Make

Now, run the following to compile the library:

```bash
$ make -j8
```

And wait for any errors, or for it to finish


### 3. Installation

To install to your system, you can now run:

```bash
$ sudo make install
```

You need `sudo` if you are installing globally, but if you had `--prefix=$HOME/prefix`, you can just run `make install`


## Done!

Now, to test it out, run:


```bash
$ ./bin/ks -h
Usage: ./bin/ks [options] FILE [args...]
       ./bin/ks [options] -e 'EXPR' [args...]
       ./bin/ks [options] - [args...]

Options:
  -h, --help            Prints this help/usage message
  -e, --expr [EXPR]     Run an inline expression, instead of a file
  -                     Start an interactive REPL shell
  -v[vv]                Increase verbosity (use '-vvv' for 'TRACE' level)
  -V, --version         Print out just the version information for kscript

kscript v0.0.1 release Jul  2 2020 16:46:50
Cade Brown <brown.cade@gmail.com>
```

Your input may differ slightly. To run a program, run:

```bash
$ ./bin/ks examples/hello_world.ks
Hello World
```


Check out the [docs](http://chemicaldevelopment.us/kscript/#/) for further info


