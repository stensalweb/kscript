# BUILD.md

This file contains more detailed building/installation information for kscript.

## Dependencies

You can build with no requirements, but for standard releases, the following packages are required for a full build including standard modules:

Packages: `gmp`, `readline`, `curl`, `glfw3`, `X11`, (`libavcodec`, `libavutil`, ...)

## Building Deps

It's recommended to run `./build_deps.sh` (may take a while!), and individual packages may copy and/or link in static libraries built to be more portable

## System Packages

Linux (Ubuntu): `sudo apt install libgmp-dev libreadline-dev libcurl4-openssl-dev libglfw3-dev libx11-dev libavcodec-dev libcodec-extra libavutil-dev libavformat-dev`

MacOS: `brew install gmp readline curl glfw libav`



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


