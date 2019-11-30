# Makefile - describes building the kscript project
#
# @author   : Cade Brown <cade@chemicaldevelopment.us>
# @license  : WTFPL (http://www.wtfpl.net/)

# -*- CONFIGURATION

# set the C compiler (?= means 'set if not already set')
CC         ?= cc
# set the compiler flags
CFLAGS     ?= -O3 -std=c99


# -*- INPUT FILES

# the sources for our kscript library (addprefix basically just adds `src`
#   to each of the files, since we are in `./` and they're in `./src`)
libkscript_src := $(addprefix src/, log.c str.c obj.c dict.c ast.c)

# the sources for the kscript executable (so things can be ran from 
#   commandline)
kscript_src    := $(addprefix src/, kscript.c)

# now, generate a list of `.o` files needed
libkscript_o   := $(patsubst %.c,%.o, $(libkscript_src))
kscript_o      := $(patsubst %.c,%.o, $(kscript_src))


# -*- OUTPUT FILES

# where to build the shared library to
libkscript_so  := libkscript.so

# where to build the executable to
kscript_exe    := kscript


# -*- RULES

# these are rules that are `not real files`, but can be ran like `make clean`
.PHONY: clean default

# by default, build the `ec` binary
default: $(kscript_exe)

# using wildcard means it only removes what exists, which makes for more useful
#   messages
clean:
	rm -rf $(wildcard $(kscript_o) $(libkscript_o) $(kscript_exe) $(libkscript_so))


# rule to build the object files (.o's) from a C file
# in makefile, `%` is like a wildcard, `%.c` will match `DIR/ANYTHING.c`
# `$<`: means the input file (%.c in this case)
# `$@`: means the output file (%.o in thie case)
%.o: %.c
	$(CC) $(CFLAGS) -fPIC $< -c -o $@

# rule to build the shared object file (.so) from all the individual compilations
# Since `libkscript_o` contains many files, we use `$^` to mean `all input files together`
$(libkscript_so): $(libkscript_o)
	$(CC) $(CFLAGS) -shared $^ -o $@

# rule to build the executable (no extension) from the library and it's `.o`'s
#   since we require a library, and object files, we don't use `$^`, but just build
#   explicitly
$(kscript_exe): $(kscript_o) $(libkscript_so)
	$(CC) $(CFLAGS) -L./ $(kscript_o) -lkscript -lm -o $@

