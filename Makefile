# Makefile - describes building the kscript library & executable
#
# Full makefile build:
# 1. run `make init`. This creates `include/ks_config.h`
# 2. edit `include/ks_config.h`, reading the comments to understand what each option does
# 3. run `make` to build the library, executable, etc into `lib/`, `bin/`
# 4. Run them
# 5. If desired, run `sudo make install` to install into `/usr/local/`.
#      To install into another directory, run `PREFIX=directory/to/install sudo make install`
#
# @author   : Cade Brown <brown.cade@gmail.com>
# @license  : WTFPL (http://www.wtfpl.net/)

# -*- CONFIGURATION

# set the C compiler (?= means 'set if not already set')
CC         ?= cc
# set the compiler flags
CFLAGS     ?= -O3 -std=c99
# set the installation prefix
PREFIX     ?= /usr/local


# -*- INPUT FILES

# the sources for our kscript library (addprefix basically just adds `src`
#   to each of the files, since we are in `./` and they're in `./src`)
libkscript_types_src := $(addprefix src/types/, none.c bool.c int.c str.c tuple.c list.c dict.c code.c kfunc.c type.c module.c parser.c ast.c cfunc.c)
libkscript_src   := $(addprefix src/, mem.c log.c err.c kso.c fmt.c exec.c funcs.c codegen.c util.c ) $(libkscript_types_src)
libkscript_src_h := $(addprefix ./include/, ks_config.h ks_bytecode.h ks_common.h ks_funcs.h ks_module.h ks_types.h ks.h kso.h)

# the sources for the kscript executable (so things can be ran from 
#   commandline)
kscript_src    := $(addprefix src/, ks.c)

# now, generate a list of `.o` files needed
libkscript_o   := $(patsubst %.c,%.o, $(libkscript_src))
kscript_o      := $(patsubst %.c,%.o, $(kscript_src))


# -*- OUTPUT FILES

# where to build the shared library to
libkscript_so  := ./lib/libkscript.so
# and static
libkscript_a   := ./lib/libkscript.a

# where to build the executable to
kscript_exe    := ./bin/ks


# -*- RULES

# these are rules that are `not real files`, but can be ran like `make clean`
.PHONY: default init clean uninstall

# by default, build the `ec` binary
default: $(kscript_exe)

# initializes the build process, cleaning, and then creating the configuration header
init: clean
	cp ks_config.T.h ./include/ks_config.h

# using wildcard means it only removes what exists, which makes for more useful
#   messages
clean:
	rm -rf $(wildcard $(kscript_o) $(libkscript_o) $(kscript_exe) $(libkscript_so) $(libkscript_a))

# rule to build the object files (.o's) from a C file
# in makefile, `%` is like a wildcard, `%.c` will match `DIR/ANYTHING.c`
# `$<`: means the input file (%.c in this case)
# `$@`: means the output file (%.o in thie case)
%.o: %.c
	$(CC) $(CFLAGS) -I./include -fPIC $< -c -o $@

# rule to build the shared object file (.so) from all the individual compilations
# Since `libkscript_o` contains many files, we use `$^` to mean `all input files together`
$(libkscript_so): $(libkscript_o)
	$(CC) $(CFLAGS) -shared $^ -o $@

# rule to build the static object file (.a)
$(libkscript_a): $(libkscript_o)
	$(AR) cr $@ $^

# rule to build the executable (no extension) from the library and it's `.o`'s
#   since we require a library, and object files, we don't use `$^`, but just build
#   explicitly
$(kscript_exe): $(kscript_o) $(libkscript_so) $(MOD_std_so)
	$(CC) $(CFLAGS) -Wl,-rpath=./lib/ -L./lib/ $(kscript_o) -lkscript -lm -ldl -o $@

# rule to install the whole package to PREFIX
install: $(libkscript_so) $(kscript_exe) $(libkscript_src_h)
	mkdir -p $(DESTDIR)$(PREFIX)/bin $(DESTDIR)$(PREFIX)/lib $(DESTDIR)$(PREFIX)/include
	cp -rf $(kscript_exe) $(DESTDIR)$(PREFIX)/bin
	cp -rf $(libkscript_so) $(DESTDIR)$(PREFIX)/lib
	cp -rf  $(libkscript_src_h) $(DESTDIR)$(PREFIX)/include
	@touch install

# rule to uninstall the whole package from PREFIX
uninstall:
	rm -f $(wildcard $(DESTDIR)$(PREFIX)/bin/$(notdir $(kscript_exe)) $(DESTDIR)$(PREFIX)/lib/$(notdir $(libkscript_so)) $(addprefix $(DESTDIR)$(PREFIX)/include/,$(notdir $(libkscript_src_h))))

# rule to build a tarfile
kscript.tar.gz: $(kscript_exe) $(libkscript_so)
	tar -cvf $@ $(kscript_exe) $(libkscript_so) $(libkscript_src_h)

